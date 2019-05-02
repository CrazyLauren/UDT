// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CPacketDivisor.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <core/kernel_type.h>
#include <core/IState.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <io/CKernelIo.h>
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
	//todo handle close uuid ==> remove in FMergeOp
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
NSHARE::eCBRval CPacketDivisor::merge_operation_t::sMMergeOperation(
		const NSHARE::CThread* WHO, NSHARE::operation_t* WHAT, void* YOU_DATA)
{
	VLOG(2) << "User data Operation ";
	merge_operation_t* _p = reinterpret_cast<merge_operation_t*>(YOU_DATA);
	CHECK_NOTNULL(_p);

	return _p->MMergeOperation(WHO, WHAT);
}
bool CPacketDivisor::merge_operation_t::MCreatePacket(user_datas_t& aTo)
{
	const size_t _size = FBufForMerge.size();
	user_data_info_t _info(FFor);
	_info.FSplit = split_packet_t();		//reset info

	if (_size > 0)
	{
		aTo.push_back(user_data_t());
		aTo.back().FDataId = _info;

		FBufForMerge.MMoveTo(aTo.back().FData);

		CHECK(!aTo.back().FData.empty());
		return true;
	}
	return false;
}
NSHARE::eCBRval CPacketDivisor::merge_operation_t::MMergeOperation(
		const NSHARE::CThread* WHO, NSHARE::operation_t* WHAT)
{
	VLOG(2) << "Operation merge ";
	NSHARE::eCBRval _rval=NSHARE::E_CB_REMOVE;
	{

		NSHARE::CRAII<NSHARE::CMutex> _blocked(FThis.FMergeMutex);
		FIsWorking = true;
	}
	{
		user_datas_t _data;
		if (MMerging(_data))
		{
			int error;
			if ((error = CKernelIo::sMGetInstance().MSendUserData(FDescriptor,
					_data)) != E_NO_ERROR)
			{
				fail_send_t _fail(_data.front().FDataId);
				_fail.MSetError(error);
				CRoutingService::sMGetInstance().MNoteFailSend(_fail);
			}
		}

		if (FError != E_NO_ERROR)
		{
			fail_send_t _fail(FFor);
			_fail.MSetError(FError);
			CRoutingService::sMGetInstance().MNoteFailSend(_fail);
		}
	}
	{

		NSHARE::CRAII<NSHARE::CMutex> _blocked(FThis.FMergeMutex);

		if (MHasToBeRemoved())
		{
			FThis.FMergeOp.erase(
					merge_key(FFor.FRouting.FFrom.FUuid, FDescriptor));//warning erase all
		}else	if (!FNewPackets.empty())
		{
			_rval=NSHARE::E_CB_SAFE_IT;
		}
		else
		{
			FIsWorking = false;

		}
	}
	VLOG(2) << "Finish handle";
	return _rval;
}
CPacketDivisor::merge_operation_t::merge_operation_t(CPacketDivisor& aThis,
		user_data_info_t const& aFor, descriptor_t aDesc) :
		FThis(aThis),	//
		FIsWorking(false),	//
		FFor(aFor),//
		FDescriptor(aDesc),	//
		FError(E_NO_ERROR),//
		FIsMerged(false)
{
	if (aFor.FSplit.FCounter != 1)
	{
		LOG(ERROR)<<"Receive not the first packet  #"<<aFor.FPacketNumber<<" from "<<aFor.FRouting.FFrom.FUuid<<" to "<<aDesc;
		FError=E_PACKET_LOST;
	}
	else
	{
		VLOG(1) << "Starting receiving split packet  #"<<aFor.FPacketNumber<<" from "<<aFor.FRouting.FFrom.FUuid<<" to "<<aDesc;

		//FSplitLevel[aFor.FSplit.FCoefficient] =aFor.FSplit;
		CHECK(!aFor.FSplit.FIsLast);
	}
}
bool CPacketDivisor::merge_operation_t::MCheckingSequence(
		user_data_t const& _data)
{
	std::map<unsigned, split_packet_t>::iterator _it = FSplitLevel.find(
			_data.FDataId.FSplit.FCoefficient);
	bool const _is_new_level = _it == FSplitLevel.end();
	/*if (_is_new_level)
	 {
	 LOG(FATAL)<< "I don't remember why this check exist. If you receive this error please "
	 "writing the issue request.";
	 }
	 else */if (!_is_new_level
			&& ((_it->second.FCounter + 1) != _data.FDataId.FSplit.FCounter))
	{
		LOG(ERROR)<<"Several part of packet #"<<_data.FDataId.FPacketNumber<<" has been lost. Cur="
		<<_data.FDataId.FSplit.FCounter<<" last="<<_it->second.FCounter;

		FError=E_PACKET_LOST;
		CHECK(_data.FDataId.FSplit.MIsSplited());
		//return false;
	}
	FSplitLevel[_data.FDataId.FSplit.FCoefficient] = _data.FDataId.FSplit;
	return true;
}

bool CPacketDivisor::merge_operation_t::MMerging(user_datas_t& aTo)
{
	bool _is_last = false;

	FThis.FMergedPacketsMutex.MLock();//<------
	CHECK(!FNewPackets.empty());

	for (; !FNewPackets.empty() && !_is_last;//
			FMergedPackets.splice(FMergedPackets.end(), FNewPackets,FNewPackets.begin()))
	{
		VLOG(3) << "Next " << FNewPackets.front().FDataId;
		if (!MCheckingSequence(FNewPackets.front()))
			return false;
		FThis.FMergedPacketsMutex.MUnlock();//<------
		_is_last = MMergePacket(FNewPackets.front());
		FThis.FMergedPacketsMutex.MLock();//<------
	}
	FThis.FMergedPacketsMutex.MUnlock();//<------

	VLOG(2) << "EOK:";
	if (_is_last)
	{
		VLOG(2) << "The packet was merged";
		CHECK(FNewPackets.empty());
		if (!MCreatePacket(aTo))
			FError = E_MERGE_ERROR;
		else
			FIsMerged=true;
	}
	return FError == E_NO_ERROR && FIsMerged;
}

bool CPacketDivisor::merge_operation_t::MHasToBeRemoved() const
{
	return FIsMerged || FError != E_NO_ERROR;
}
void CPacketDivisor::merge_operation_t::MMerge(user_datas_t & aVal,
		user_datas_t& aTo)
{
	bool _need_call = false;
	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FThis.FMergedPacketsMutex);
		_need_call = !FIsWorking && FNewPackets.empty();
		VLOG_IF(4,_need_call) << "Need call handling data";

		if (FFor.FPacketNumber != aVal.front().FDataId.FPacketNumber)
		{
			LOG(ERROR)<<"The packet #"<<FFor.FPacketNumber<<" has been lost. Ignoring ...";
			if(FError==E_NO_ERROR)	//maybe the error is occurred in constructor
			FError=E_PACKET_LOST;
		}
		else
		{
			FNewPackets.splice(FNewPackets.end(), aVal);
		}
	}
	if (FError != E_NO_ERROR)
		return ;

#ifndef		NO_MERGE_THREAD
	if (_need_call)
	{

		NSHARE::operation_t _op(sMMergeOperation, this,
				NSHARE::operation_t::AS_LOWER);
		CDataObject::sMGetInstance().MPutOperation(_op);
	}
#else
	user_datas_t _data;
	if (MMerging(_data))
	{
		VLOG(2) << "!Ura copy merged data";
		aTo.splice(aTo.end(), _data);
	}
#endif
}
void CPacketDivisor::MMerge(descriptor_t aFor, user_datas_t& aWhat,
		user_datas_t& aTo, user_datas_t& aFails, fail_send_array_t& _non_sent)
{
	//
	CHECK(!aWhat.empty());
	CHECK(CDescriptors::sMIsValid(aFor));

	unsigned const _packet_number = aWhat.front().FDataId.FPacketNumber;
	NSHARE::uuid_t const _uuid = aWhat.front().FDataId.FRouting.FFrom.FUuid;
	const merge_key _key(_uuid, aFor);
	VLOG(2) << "Merge  #" << _packet_number << " from " << _uuid<<" To "<<aFor<<aWhat.front().FDataId.FSplit;

	user_datas_t _merging;
	{
		//looking for all splitted packets
		_merging.splice(_merging.end(), aWhat, aWhat.begin());
		for (;
				!aWhat.empty()
						&& _packet_number == aWhat.front().FDataId.FPacketNumber//
						&& _uuid == aWhat.front().FDataId.FRouting.FFrom.FUuid//
				;)
		{
			CHECK(aWhat.front().FDataId.FSplit.MIsSplited());
			VLOG(1) << "Put  " << aWhat.front().FDataId.FPacketNumber
								<< " from "
								<< aWhat.front().FDataId.FRouting.FFrom.FUuid;
			_merging.splice(_merging.end(), aWhat, aWhat.begin());
		}
	}
	for (; !_merging.empty();)
	{

		merge_operations_map_t::iterator _it;
		{
			NSHARE::CRAII<NSHARE::CMutex> _blocked(FMergeMutex);
			_it = FMergeOp.find(_key);
			if (_it == FMergeOp.end())
			{

				merge_operations_map_t::value_type const _val(_key,
						merge_operation_t(*this, _merging.front().FDataId,
								aFor));
				_it = FMergeOp.insert(_val).first;
			}
		}
		_it->second.MMerge(_merging, aTo);
		if (_it->second.FError != E_NO_ERROR)
		{
			LOG(ERROR)<<"Merge error "<<_it->second.FError;
			fail_send_t _fail(_it->second.FFor);
			_fail.MSetError(_it->second.FError);
			_non_sent.push_back(_fail);
			NSHARE::CRAII<NSHARE::CMutex> _blocked(FMergedPacketsMutex);
			aFails.splice(aFails.end(), _it->second.FMergedPackets);
			aFails.splice(aFails.end(), _it->second.FNewPackets);
		}
		if (_it->second.MHasToBeRemoved())
		{
			NSHARE::CRAII<NSHARE::CMutex> _blocked(FMergeMutex);
			VLOG(2)<<"Remove packet "<<_it->second.FFor.FPacketNumber<<" from "<<_it->second.FFor.FRouting.FFrom.FUuid<<" to "<<_it->second.FDescriptor;
			FMergeOp.erase(_it);
		}
	}
}

void CPacketDivisor::MSplitOrMergeIfNeed(descriptor_t aFor,
		user_datas_t & aWhat, user_datas_t& aTo, fail_send_array_t & _non_sent)
{
	NSHARE::smart_field_t<split_info> _split;
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FLimitsMutex);
		d_info_t::const_iterator _jt = FLimitsInfo.find(aFor);
		VLOG_IF(2,_jt==FLimitsInfo.end()) << "No packet limits for " << aFor;
		if (_jt != FLimitsInfo.end())
		{
			_split = _jt->second;
		}
	}
	if (!_split.MIs())
	{
		aTo.splice(aTo.end(), aWhat);
		VLOG(2) << "No limit for " << aFor;
		return;
	}

	split_info const& _info = _split.MGetConst(); //
	size_t const _max_size = _info.FMaxSize;
	user_datas_t _fails;
	for (; !aWhat.empty();)
	{

		size_t const _align_data_size = _info.MDataSize(aWhat.front());
		VLOG(2) << "Max size for " << aFor << " is " << _max_size
							<< " bytes Data size=" << _align_data_size;

		if (_info.FType.MGetFlag(split_info::CAN_NOT_SPLIT))
		{
			VLOG(2) << "The packet #" << aWhat.front().FDataId.FPacketNumber
								<< " cannot be split for " << aFor;
			if (aWhat.front().FDataId.FSplit.MIsSplited())
				MMerge(aFor, aWhat, aTo, _fails, _non_sent);
			else if(_max_size == 0 || _max_size >= _align_data_size)
				aTo.splice(aTo.end(), aWhat, aWhat.begin());
			else
			{
				LOG(ERROR)<<"The data size is more than "<<_max_size;
				fail_send_t _fail(aWhat.front().FDataId);
				_fail.MSetError(E_DATA_TOO_LARGE);
				_non_sent.push_back(_fail);
				_fails.splice(_fails.end(), aWhat, aWhat.begin());
			}
		}
		//no limit
		else if (!_info.FType.MGetFlag(split_info::LIMITED))
		{
			VLOG(2) << "Can send";
			aTo.splice(aTo.end(), aWhat, aWhat.begin());
		}
		//limited
		else if (_max_size == 0 || _max_size >= _align_data_size)
		{
			VLOG(2) << "Can send";
			aTo.splice(aTo.end(), aWhat, aWhat.begin());
		}
		else
		{
			VLOG(2) << "The packet #" << aWhat.front().FDataId.FPacketNumber
								<< " has to be limited for " << aFor;
			MSplit(aWhat, aTo, _max_size);
		}
		//todo
		//1)checking received packets sequence for secondary level of splitting
	}
	aWhat.swap(_fails);
}

size_t CPacketDivisor::MSplit(user_datas_t& aData, user_datas_t& aTo,
		size_t aPartSize)
{
	user_data_t const& _h_data = aData.front();
	const size_t _data_size = _h_data.FData.size();
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

	size_t const _new_coef = _h_data.FDataId.FSplit.FCoefficient
			* _valid_num_part;

	VLOG(2) << "Valid the Num of  parts for " << _align_full_size
						<< " PartSize=" << aPartSize << " is "
						<< _valid_num_part << " new coefficient:" << _new_coef
						<< " valid part=" << _valid_part_size
						<< " raw data size=" << _data_size;

	unsigned i = 0;
	size_t const _first_num = _h_data.FDataId.FSplit.FCounter
			* _valid_num_part - _valid_num_part + 1;
	for (size_t _last_but_one = _valid_num_part - 1; i < _last_but_one; ++i)
	{
		user_data_t _data;
		_data.FDataId = _h_data.FDataId;
		_data.FDataId.FSplit.FCoefficient = static_cast<uint16_t>(_new_coef);
		_data.FDataId.FSplit.FCounter = static_cast<uint16_t>(_first_num + i);
		_data.FDataId.FSplit.FIsLast = false;

		NSHARE::CBuffer::const_iterator const _b_it = _h_data.FData.begin()
				+ _valid_part_size * i;
		NSHARE::CBuffer::const_iterator const _e_it = _b_it + _valid_part_size;
		_data.FData.insert(_data.FData.end(), _b_it, _e_it);
		aTo.push_back(_data);
	}
	//handle last
	if (_valid_num_part != 1)
	{
		user_data_t _data;
		_data.FDataId = _h_data.FDataId;
		_data.FDataId.FSplit.FCoefficient = static_cast<uint16_t>(_new_coef);
		_data.FDataId.FSplit.FCounter = static_cast<uint16_t>(_first_num + i);

		VLOG_IF(1,_h_data.FDataId.FSplit.FIsLast) << "It's the last block.";

		NSHARE::CBuffer::const_iterator const _b_it = _h_data.FData.begin()
				+ _valid_part_size * i;
		_data.FData.insert(_data.FData.end(), _b_it, _h_data.FData.end());
		aTo.push_back(_data);

		aData.pop_front();
	}
	else
	{
		LOG(DFATAL)<<"The data is not split:"<<_align_full_size<<";"<<aPartSize;
		aTo.splice(aTo.end(),aData,aData.begin());
	}
	VLOG(2) << "The Size of part is " << _valid_part_size;
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
