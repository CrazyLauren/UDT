/*
 * CPacketDivisor.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "../core/kernel_type.h"
#include "../core/IState.h"
#include "../core/CDescriptors.h"
#include "../core/CDataObject.h"
#include "../io/CKernelIo.h"
#include "CPacketDivisor.h"
#include "CRoutingService.h"

#define NO_MERGE_THREAD
template<>
NUDT::CPacketDivisor::singleton_pnt_t NUDT::CPacketDivisor::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CPacketDivisor::NAME = "div";
CPacketDivisor::CPacketDivisor() :
		IState(NAME)
{
}

CPacketDivisor::~CPacketDivisor()
{
	//todo handle close uuid ==> remove in FSplitInfo
}
void CPacketDivisor::MSetLimitsFor(descriptor_t const& aFor,
		split_info const& aWhat)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FLimitsMutex);
	FLimitsInfo[aFor] = aWhat;
}
std::pair<split_info, bool> CPacketDivisor::MGetLimitsFor(
		descriptor_t const& aFor) const
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FLimitsMutex);
	d_info_t::const_iterator _it = FLimitsInfo.find(aFor);

	return _it == FLimitsInfo.end() ?
			std::pair<split_info, bool>(split_info(), false) :
			std::pair<split_info, bool>(_it->second, true);
}
bool CPacketDivisor::MRemoveLimitsFor(descriptor_t const& aFor)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FLimitsMutex);
	return FLimitsInfo.erase(aFor) > 0;
}

void CPacketDivisor::MGetLimitsFor(const user_data_t& aWhat,
		const input_t& aSendTo, limits_t& aCannotSplit, //
		limits_t& aNoLimit, //the rules of sending is definition by _cannot_split or _has_limits
		limits_for_t& aHasLimits)
{
	limit_for_value_t _common_limit;
	_common_limit.first = std::numeric_limits<size_t>::max();

	input_t::const_iterator _it_out = aSendTo.begin(), _it_out_end =
			aSendTo.end();

	NSHARE::CRAII<NSHARE::CMutex> _lock(FLimitsMutex);
	for (; _it_out != _it_out_end; ++_it_out)
	{
		d_info_t::const_iterator _jt = FLimitsInfo.find(_it_out->first);
		VLOG_IF(2,_jt==FLimitsInfo.end()) << "No packet limits for "
													<< _it_out->first;
		if (_jt != FLimitsInfo.end())
		{
			split_info const& _info = _jt->second;
			size_t const _align_data_size = _info.MDataSize(aWhat);
			size_t const _max_size = _info.FMaxSize;

			if (_info.FType.MGetFlag(split_info::CAN_NOT_SPLIT))
			{
				VLOG(2) << "The packet #" << aWhat.FDataId.FPacketNumber
									<< " cannot be split for "
									<< _it_out->first;
//				if(_max_size ==0 || _max_size >= _align_data_size)
				aCannotSplit.push_back(*_it_out);
//				else
//				{
//					LOG(ERROR)<<"Cannot split packet  #"<<aWhat.FDataId.FPacketNumber<<" for "<<_jt->first<<" as max="<<_max_size
//					<<" data size="<<_align_data_size;
//				}
			}
			else if (!_info.FType.MGetFlag(split_info::LIMITED))
			{
				VLOG(2) << "Can send";
				aNoLimit.push_back(*_it_out);
			}
			else
			{
				VLOG(2) << "Max size for " << _jt->first << " is "
									<< _max_size << " bytes Data size="
									<< _align_data_size;
				if (_max_size == 0 || _max_size >= _align_data_size)
				{
					VLOG(2) << "Can send";
					aNoLimit.push_back(*_it_out);
				}
				else if (_info.FType.MGetFlag(split_info::IS_UNIQUE))
				{
					VLOG(2) << "The packet #" << aWhat.FDataId.FPacketNumber
										<< " has to be limited for "
										<< _it_out->first << " by " << _max_size
										<< " bytes.";
					limits_t _lim;
					_lim.push_back(*_it_out);
					aHasLimits.push_back(limit_for_value_t(_max_size, _lim));
				}
				else
				{
					VLOG(2) << "The packet #" << aWhat.FDataId.FPacketNumber
										<< " has to be limited for "
										<< _it_out->first;
					_common_limit.first = std::min(_common_limit.first,
							_max_size);
					_common_limit.second.push_back(*_it_out);
				}
			}
		}
		else
		{
			VLOG(2) << "No limit for " << _it_out->first;
			aNoLimit.push_back(*_it_out); //todo default operation
		}
	}
	if (!_common_limit.second.empty())
	{
		VLOG(2) << "There are limits.";
		aHasLimits.push_back(_common_limit);
	}
}

CPacketDivisor::eError CPacketDivisor::MCheckingSequence(
		const user_data_t& aWhat, fail_send_array_t& _non_sent)
{
	last_packet_info::iterator _it_uuid = FSplitInfo.find(
			aWhat.FDataId.FFrom.FUuid);
	bool const _is_first_split_packet = _it_uuid == FSplitInfo.end();
	bool const _was_splited = aWhat.FDataId.FSplit.MIsSplited();
	if (_is_first_split_packet)
	{
		if (_was_splited)
		{
			if (aWhat.FDataId.FSplit.FCounter != 1)
			{
				LOG(ERROR)<<"Receive not the first packet";
				return CPacketDivisor::E_PACKET_LOST;
			}
			else
			{
				VLOG(2) << "Starting receiving split packet from "
				<< aWhat.FDataId;

				last_packet_info::value_type _new_val(aWhat.FDataId.FFrom.FUuid,
						split_packet_info_t());
				_new_val.second.FInform = aWhat.FDataId;
				_new_val.second.FSplitLevel[aWhat.FDataId.FSplit.FCoefficient] =
				aWhat.FDataId.FSplit;
				_it_uuid = FSplitInfo.insert(_new_val).first;

				CHECK(!aWhat.FDataId.FSplit.FIsLast);
			}
		}
	}
	else
	{
		VLOG(2)
		<< "It's not the first split packet. Checking packet sequence."<<aWhat.FDataId.FSplit;

		if (_it_uuid->second.FInform.FPacketNumber != aWhat.FDataId.FPacketNumber)
		{
			LOG(ERROR)<<"The packet #"<<_it_uuid->second.FInform.FPacketNumber<<" has been lost.";
			fail_send_t _fail(_it_uuid->second.FInform);
			_non_sent.push_back(_fail);
			FSplitInfo.erase(_it_uuid);
			//return CPacketDivisor::E_PACKET_SEQUENCE_FAIL;
		}
		else
		{
			std::map<unsigned, split_packet_t>::iterator _it=_it_uuid->second.FSplitLevel.find(aWhat.FDataId.FSplit.FCoefficient);
			bool const _is_new_level=_it==_it_uuid->second.FSplitLevel.end();
			if(_is_new_level)
			{
				VLOG(2)<<"New split level "<<aWhat.FDataId.FSplit;
				_it_uuid->second.FSplitLevel[aWhat.FDataId.FSplit.FCoefficient]=aWhat.FDataId.FSplit;
			}
			else
			if((_it->second.FCounter+1) !=aWhat.FDataId.FSplit.FCounter)
			{
				LOG(ERROR)<<"Several part of packet #"<<aWhat.FDataId.FPacketNumber<<" has been lost. Cur="
				<<aWhat.FDataId.FSplit.FCounter<<" last="<<_it->second.FCounter;

				fail_send_t _fail(_it_uuid->second.FInform);
				_fail.FSplit=_it->second;
				_non_sent.push_back(_fail);

				FSplitInfo.erase(_it_uuid);
				CHECK(aWhat.FDataId.FSplit.MIsSplited());

				return CPacketDivisor::E_PACKET_LOST;
			}
		}
	}
	return CPacketDivisor::E_EOK;
}

void CPacketDivisor::MPushPacketWithoutChange(const user_data_t& aWhat,
		limits_t& aLimits, packed_user_data_t& aTo)
{
	packed_user_data_t::value_type _val;
	_val.FDataList.push_back(aWhat);
	_val.FFor.swap(aLimits);
	aTo.push_back(_val);
}

CPacketDivisor::eError CPacketDivisor::MProcessNotSplitedPacket(
		const user_data_t& aWhat, limits_t & _no_limit,
		limits_t & _cannot_split, limits_for_t &_has_limits,
		packed_user_data_t& aTo)
{
	VLOG(2) << aWhat.FDataId << " is not split.";
	if (!_no_limit.empty() || !_cannot_split.empty())
	{
		VLOG(2) << "Push data without change";
		if (!_cannot_split.empty())
			_no_limit.insert(_no_limit.end(), _cannot_split.begin(),
					_cannot_split.end());
		MPushPacketWithoutChange(aWhat, _no_limit, aTo);
	}
	if (!_has_limits.empty())
	{
		MLimitPacketSize(aWhat, _has_limits, aTo);
	}
	return E_EOK;
}

CPacketDivisor::split_packet_info_t& CPacketDivisor::MGetSplitInfoFor(
		const user_data_t& aWhat)
{
	last_packet_info::iterator _it_uuid = FSplitInfo.find(
			aWhat.FDataId.FFrom.FUuid);
	CHECK(_it_uuid != FSplitInfo.end());
	return _it_uuid->second;
}

void CPacketDivisor::MUpdatingSplitInfo(const user_data_t& aWhat)
{
	last_packet_info::iterator _it_uuid = FSplitInfo.find(
			aWhat.FDataId.FFrom.FUuid);
	CHECK(_it_uuid != FSplitInfo.end());

	if (aWhat.FDataId.FSplit.FIsLast)
	{
		VLOG(2) << "It was last packet of #" << aWhat.FDataId.FPacketNumber;
		FSplitInfo.erase(_it_uuid);
	}
	else
	{
		VLOG(2) << "Update split info " << aWhat.FDataId.FSplit;
		_it_uuid->second.FSplitLevel[aWhat.FDataId.FSplit.FCoefficient] =
				aWhat.FDataId.FSplit;
	}
}

void CPacketDivisor::MLimitPacketSize(const user_data_t& aWhat,
		limits_for_t & _has_limits, packed_user_data_t& aTo)
{
	VLOG(2) << "Limiting data size...";
	limits_for_t::iterator _it = _has_limits.begin(), _it_end(
			_has_limits.end());
	for (; _it != _it_end; ++_it)
	{
		VLOG(2) << "There is limit " << _it->first << " bytes";
		packed_user_data_t::value_type _val;
		_val.FFor.swap(_it->second);

		MSplit(aWhat, _val.FDataList, _it->first);
		VLOG(2) << "Number of part :" << _val.FDataList.size();

		aTo.push_back(_val);
	}
}

bool CPacketDivisor::merge_operation_t::MMergePacket(const user_data_t& aWhat)
{
	VLOG(2) << "Merging packet " << aWhat.FDataId.FPacketNumber
						<< " block size " << aWhat.FData.size();
	if (FBufForMerge.empty())
	{
		CHECK_EQ(aWhat.FDataId.FSplit.FCounter, 1);
		size_t const _estimated_size = aWhat.FDataId.FSplit.FCoefficient
				* aWhat.FData.size();

		VLOG(2) << "Estimated size of packet #"
							<< aWhat.FDataId.FPacketNumber << " is "
							<< _estimated_size << " bytes.";

		// aWhat.FData.size()/2 - reserve for last block
		//it's size within the limits of 1 to (2*aWhat.FData.size()-1) bytes
		FBufForMerge.reserve(_estimated_size + aWhat.FData.size() / 2);

	}
	FBufForMerge.insert(FBufForMerge.end(), aWhat.FData.begin(),
			aWhat.FData.end());
	if (aWhat.FDataId.FSplit.FIsLast)
	{
		VLOG(2) << "Handle last packet of #" << aWhat.FDataId.FPacketNumber
							<< " obtained size is " << FBufForMerge.size();
		return true;
	}
	return false;
}
int CPacketDivisor::merge_operation_t::sMMergeOperation(
		const NSHARE::CThread* WHO, NSHARE::operation_t* WHAT, void* YOU_DATA)
{
	VLOG(2) << "User data Operation ";
	merge_operation_t* _p = reinterpret_cast<merge_operation_t*>(YOU_DATA);
	CHECK_NOTNULL(_p);
	_p->MMergeOperation(WHO, WHAT);
	return 0;
}
void CPacketDivisor::merge_operation_t::MFilteringForMaxSize(const size_t aSize,
		uuids_t& _non_sent)
{
	limits_t _copy;
	FFor.swap(_copy);
	limits_t::const_iterator _it = _copy.begin(), _it_end(_copy.end());

	NSHARE::CRAII<NSHARE::CMutex> _lock(FThis.FLimitsMutex);
	for (; _it != _it_end; ++_it)
	{

		d_info_t::const_iterator _it_info = FThis.FLimitsInfo.find(_it->first);
		if (_it_info != FThis.FLimitsInfo.end())
		{
			if (_it_info->second.FMaxSize > 0
					&& aSize > _it_info->second.FMaxSize)
			{
				LOG(ERROR)<<"Cannot handle merged packet by "
				<<_it->first<<" as max size="<<_it_info->second.FMaxSize<<" data size="<<aSize;
				_non_sent.insert(_non_sent.end(),_it->second.begin(),_it->second.end());
				continue;
			}
		}
		FFor.push_back(*_it);
	}
}
void CPacketDivisor::merge_operation_t::MSendPacket(
		fail_send_array_t& _non_sent, user_data_info_t const& aInfo)
{

	uuids_t _fails;
	MFilteringForMaxSize(FBufForMerge.size(), _fails);

	if (!FFor.empty())
	{
		NSHARE::IAllocater* _alloc =
				CDataObject::sMGetInstance().MDefAllocater();
		std::vector<user_data_t> _v(1);
		if (FBufForMerge.MIsAllocatorEqual(_alloc))
		{
			FBufForMerge.MMoveTo(_v.front().FData);
		}
		else
		{
			VLOG(2) << "Allocate memory to default buffer.";
			NSHARE::CBuffer _data(FBufForMerge.size(), 0, _alloc);
			if (_data.size() >= FBufForMerge.size())
			{
				_data.deep_copy(FBufForMerge);//warning with SM the thread can be locked!!!
				VLOG(2) << "Copy OK";
				FBufForMerge.clear();
				_data.MMoveTo(_v.front().FData);
			}
			else
			{
				LOG(ERROR)<<"Cannot allocate memory";
				_data.clear();
			}
		}
		if (!_v.front().FData.empty())
		{
			_v.front().FDataId = aInfo;
			VLOG(2) << "Send merge data to " << _v.front().FDataId;
			CKernelIo::sMGetInstance().MSendTo(FFor, _v, _fails);
		}
		else
		{
			limits_t::const_iterator _it = FFor.begin(), _it_end = FFor.end();
			for (; _it != _it_end; ++_it)
				_fails.insert(_fails.end(), _it->second.begin(),
						_it->second.end());
		}
	}
	if (!_fails.empty())
	{
		VLOG(2) << "put fails";
		fail_send_t _fail(aInfo);
		_fail.FUUIDTo = _fails;
		_fail.FError = fail_send_t::E_THE_BUFFER_IS_SMALL;
		_non_sent.push_back(_fail);
	}

}

void CPacketDivisor::merge_operation_t::MMergeOperation(
		const NSHARE::CThread* WHO, NSHARE::operation_t* WHAT)
{
	VLOG(2) << "Operation merge ";
	std::vector<user_data_t> _datas;
	NSHARE::smart_field_t<NSHARE::uuid_t> _remove;
	{
#ifndef		NO_MERGE_THREAD
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FThis.FMergeMutex);
#endif
		CHECK(!FSplitPackets.empty());
		_datas.swap(FSplitPackets);
		CHECK(FSplitPackets.empty());
		CHECK(!_datas.empty());
		FIsWorking = true;
	}
	{

		std::vector<user_data_t>::const_iterator _it = _datas.begin(), _it_end(
				_datas.end());
		bool _is_last = false;
		for (; _it != _it_end; ++_it)
		{
			_is_last = MMergePacket(*_it);
		}
		VLOG(2) << "EOK:" << _datas.size();
		if (_is_last)
		{
			VLOG(2) << "The packet was merged";
			user_data_info_t const _id = _datas.back().FDataId;
			_remove.MSet(_id.FFrom.FUuid);

			_datas.clear();
			fail_send_array_t _non_sent;
			MSendPacket(_non_sent, _id);
			if (!_non_sent.empty())
			{
				fail_send_array_t::const_iterator _it = _non_sent.begin(),
						_it_end(_non_sent.end());
				for (; _it != _it_end; ++_it)
					CRoutingService::sMGetInstance().MNoteFailSend(*_it);
			}
		}
		else
			_datas.clear();
	}
	{
#ifndef		NO_MERGE_THREAD
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FThis.FMergeMutex);
#endif
		if (!FSplitPackets.empty())
		{
			WHAT->MKeep(true);
		}
		else
		{
			FIsWorking = false;
			if (_remove.MIs())
			{
				FThis.FMergeOp.erase(_remove.MGetConst());	//warning erase all
			}
		}
	}
	VLOG(2) << "Finish handle";
}
void CPacketDivisor::merge_operation_t::MMerge(const user_data_t & aVal)
{
	bool _need_call = false;
	{
		_need_call = !FIsWorking && FSplitPackets.empty();
		VLOG_IF(4,_need_call) << "Need call handling data";
		FSplitPackets.push_back(aVal);
	}

	if (_need_call)
	{
		NSHARE::operation_t _op(sMMergeOperation, this,
				NSHARE::operation_t::AS_LOWER);
#ifndef		NO_MERGE_THREAD
		CDataObject::sMGetInstance().MPutOperation(_op);
#else
		sMMergeOperation(NULL,&_op,this);
#endif
	}

}
void CPacketDivisor::MMerge(const user_data_t& aWhat, limits_t& _cannot_split,
		fail_send_array_t& _non_sent)
{
	VLOG(2) << "Merge  " << aWhat.FDataId;

	{
#ifndef		NO_MERGE_THREAD
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FMergeMutex);
#endif
		std::map<NSHARE::uuid_t, merge_operation_t>::iterator _it =
				FMergeOp.find(aWhat.FDataId.FFrom.FUuid);
		if (_it == FMergeOp.end())
		{
#ifdef	NO_MERGE_THREAD
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FMergeMutex);
#endif

			std::map<NSHARE::uuid_t, merge_operation_t>::value_type const _val(
					aWhat.FDataId.FFrom.FUuid,
					merge_operation_t(*this, _cannot_split));
			_it = FMergeOp.insert(_val).first;
		}
		_it->second.MMerge(aWhat);
	}
}

CPacketDivisor::eError CPacketDivisor::MProcessSpltedPacket(
		const user_data_t& aWhat, limits_t& _no_limit, limits_t& _cannot_split,
		limits_for_t& _has_limits, packed_user_data_t& aTo,
		fail_send_array_t & _non_sent)
{
	VLOG(2) << aWhat.FDataId << " is split. New:" << aWhat.FDataId.FSplit;
	if (!_no_limit.empty())
	{
		VLOG(2) << "Push data without change";
		MPushPacketWithoutChange(aWhat, _no_limit, aTo);
	}
	if (!_has_limits.empty())
	{
		MLimitPacketSize(aWhat, _has_limits, aTo);
	}
	//merge
	if (!_cannot_split.empty())
	{
		MMerge(aWhat, _cannot_split, _non_sent);
	}
	return E_EOK;
}

CPacketDivisor::eError CPacketDivisor::MSplitOrMergeIfNeed(
		user_data_t const& aWhat, input_t const& aSendTo,
		packed_user_data_t& aTo, fail_send_array_t & _non_sent)
{
	//DCHECK_NO_MULTI_THREAD
	VLOG(2) << aWhat.FDataId;
	bool const _was_splited = aWhat.FDataId.FSplit.MIsSplited();

	if (!FSplitInfo.empty() || _was_splited) //optimization
	{
		if (MCheckingSequence(aWhat, _non_sent)
				== CPacketDivisor::E_PACKET_LOST)
			return CPacketDivisor::E_PACKET_LOST;
	}
	limits_t _no_limit; //the rules of send definition by _cannot_split or _has_limits
	limits_t _cannot_split;
	limits_for_t _has_limits;
	MGetLimitsFor(aWhat, aSendTo, _cannot_split, _no_limit, _has_limits);

	eError _error = E_EOK;
	if (_has_limits.empty() && //
			(!_was_splited || _cannot_split.empty()))
	{
		VLOG(2) << "Does not need split packet";
		_error = E_DOES_NOT_NEED;
	}
	else
	{
		if (!_was_splited)
			_error = MProcessNotSplitedPacket(aWhat, _no_limit, _cannot_split,
					_has_limits, aTo);
		else
			_error = MProcessSpltedPacket(aWhat, _no_limit, _cannot_split,
					_has_limits, aTo, _non_sent);
	}
	if (_was_splited)
	{
		MUpdatingSplitInfo(aWhat);
	}
	return _error;
}
size_t CPacketDivisor::MSplit(const user_data_t& aWhat,
		std::vector<user_data_t>& aTo, size_t aPartSize)
{
	const size_t _data_size = aWhat.FData.size();
	const size_t _align_full_size = /*aWhat.FDataId.FSplit.MGetAlignmentSize(
	 _data_size)*/_data_size;
	CHECK_GT(_align_full_size, 0);
	if (aPartSize >= _align_full_size)
	{
		LOG(WARNING)<<"Splitting packet error part size="<<aPartSize<<" data size="<<_align_full_size;
		aPartSize=_align_full_size-1;
	}
	CHECK_LT(aPartSize, _align_full_size);
	const bool _is_whole = (_align_full_size % aPartSize) == 0;

	size_t const _num_part =
			_is_whole ?
					_align_full_size / aPartSize :
					(_align_full_size / aPartSize) + 1;
	CHECK_GT(_num_part, 0);
	//the number of part must be even, The size of all parts with the exception of last must be equal of.

	size_t const _valid_num_part =
			(_num_part % 2 == 0) ? _num_part : _num_part + 1;
	size_t const _valid_part_size = _align_full_size / _valid_num_part;

	size_t const _new_coef = aWhat.FDataId.FSplit.FCoefficient
			* _valid_num_part;

	VLOG(2) << "Valid the Num of  parts for " << _align_full_size
						<< " PartSize=" << aPartSize << " is "
						<< _valid_num_part << " new coefficient:" << _new_coef
						<< " valid part=" << _valid_part_size
						<< " raw data size=" << _data_size;

	unsigned i = 0;
	unsigned const _first_num = aWhat.FDataId.FSplit.FCounter * _valid_num_part
			- _valid_num_part + 1;
	for (unsigned _last_but_one = _valid_num_part - 1; i < _last_but_one; ++i)
	{
		user_data_t _data;
		_data.FDataId = aWhat.FDataId;
		_data.FDataId.FSplit.FCoefficient = _new_coef;
		_data.FDataId.FSplit.FCounter = _first_num + i;
		_data.FDataId.FSplit.FIsLast = false;

		NSHARE::CBuffer::const_iterator const _b_it = aWhat.FData.begin()
				+ _valid_part_size * i;
		NSHARE::CBuffer::const_iterator const _e_it = _b_it + _valid_part_size;
		_data.FData.insert(_data.FData.end(), _b_it, _e_it);
		aTo.push_back(_data);
	}
	//handle last
	if (_valid_num_part != 1)
	{
		user_data_t _data;
		_data.FDataId = aWhat.FDataId;
		_data.FDataId.FSplit.FCoefficient = _new_coef;
		_data.FDataId.FSplit.FCounter = _first_num + i;

		VLOG_IF(1,aWhat.FDataId.FSplit.FIsLast) << "It's the last block.";

		NSHARE::CBuffer::const_iterator const _b_it = aWhat.FData.begin()
				+ _valid_part_size * i;
		_data.FData.insert(_data.FData.end(), _b_it, aWhat.FData.end());
		aTo.push_back(_data);
	}
	else
	{
		LOG(ERROR)<<"The data is not split:"<<_align_full_size<<";"<<aPartSize;
		aTo.push_back(aWhat);
	}
	return _valid_part_size;
}
//std::pair<user_data_t const&, eError> MMergeIfNeed(user_data_t const& aWhat);//when receive packet

NSHARE::CConfig CPacketDivisor::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	NSHARE::CRAII<NSHARE::CMutex> _lock(FLimitsMutex);
//	{
//		last_packet_info::const_iterator _it = FSplitInfo.begin();
//		for (; _it != FSplitInfo.end(); ++_it)
//		{
//			NSHARE::CConfig _inf("split");
//			_inf.MAdd(_it->first.MSerialize());
//			_inf.MAdd(_it->second.FInfo.MSerialize());
//			_inf.MAdd("msize", _it->second.FMergeBuf.size());
//			_conf.MAdd(_inf);
//		}
//	}
	{
		d_info_t::const_iterator _it = FLimitsInfo.begin(), _it_end(
				FLimitsInfo.end());

		for (; _it != _it_end; ++_it)
		{
			NSHARE::CConfig _inf("lim");
			_inf.MAdd(
					CDescriptors::sMGetInstance().MGet(_it->first).first.MSerialize());
			_inf.MAdd(_it->second.MSerialize());
			_conf.MAdd(_inf);
		}
	}
	return _conf;
}
} /* namespace NSHARE */
