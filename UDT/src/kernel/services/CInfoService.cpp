// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CInfoService.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 19.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <algorithm>
#include <iterator>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/kernel_type.h>
#include <io/CKernelIo.h>
#include "CInfoService.h"

template<>
NUDT::CInfoService::singleton_pnt_t NUDT::CInfoService::singleton_t::sFSingleton =
NULL;
namespace NUDT
{
const NSHARE::CText CInfoService::NAME = "udt_net";
CInfoService::CInfoService() :
		ICore(NAME)
{
	MInit();
}

CInfoService::~CInfoService()
{

}
bool CInfoService::MStart()
{
	{
		w_access _access = FData.MGetWAccess();

		k_diff_t _fix;

		CDescriptors::d_list_t _cons;
		CDescriptors::sMGetInstance().MGetAll(_cons);
		CDescriptors::d_list_t::const_iterator _it = _cons.begin(), _it_end =
				_cons.end();
		for (; _it != _it_end; ++_it)
		{
			if (_it->second.MIs())
			{
				MPutConsumerToMyInfo(_it->first, _it->second.MGetConst(),
						*_access, _fix);
			}
		}
	}
	return true;
}
void CInfoService::MSendNet(const descriptors_t& _sent_to)
{
	const r_access _access = FData.MGetRAccess();
	descriptors_t::const_iterator _it = _sent_to.begin(), _it_end(
			_sent_to.end());
	for (; _it != _it_end; ++_it)
	{
		VLOG(2) << "Sent to " << *_it;
		CKernelIo::sMGetInstance().MSendTo(*_it, _access.MGet().FNet);
	}
}

int CInfoService::sMHandleOpenId(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CInfoService* _this = reinterpret_cast<CInfoService*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(open_descriptor::NAME, WHAT->FType);
	const open_descriptor* _p =
			reinterpret_cast<const open_descriptor*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleOpen(_p->FId, _p->FInfo);
	return 0;
}

int CInfoService::sMHandleKernelInfo(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CInfoService* _this = reinterpret_cast<CInfoService*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(kernel_infos_array_id_t::NAME, WHAT->FType);
	const kernel_infos_array_id_t* _p =
			reinterpret_cast<kernel_infos_array_id_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandle(_p->FVal, _p->FId);
	return 0;
}

int CInfoService::sMHandleCloseId(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CInfoService* _this = reinterpret_cast<CInfoService*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(close_descriptor::NAME, WHAT->FType);
	const close_descriptor* _p =
			reinterpret_cast<close_descriptor*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleClose(_p->FId, _p->FInfo);
	return 0;
}

void CInfoService::MRebuildGraph(_data_info_t& _new_array)
{
	_new_array.FGraph.MReset();
	kernel_infos_array_t::const_iterator _it = _new_array.FNet.begin(), _it_end(
			_new_array.FNet.end());
	for (; _it != _it_end; ++_it)
	{
		CRouteGraph::vertexs_t _vertex;
		kern_links_t::const_iterator _jt = _it->FCustomerInfo.begin(), _jt_end(
				_it->FCustomerInfo.end());
		for (; _jt != _jt_end; ++_jt)
			_vertex.insert(
					CRouteGraph::vertexs_t::value_type(_jt->FProgramm.FId.FUuid,
							_jt->FLatency));
		_new_array.FGraph.MAddNode(_it->FKernelInfo.FId.FUuid, _vertex);
	}
}

void CInfoService::MChangeInform(const kernel_infos_diff_t& aInfo)
{
	VLOG(2) << " Diff";
//	VLOG(2) << aInfo.MSerialize().MToJSON(true);
//	std::cout << "====================" << std::endl;
//	std::cout << NSHARE::get_unix_time() << std::endl;
//	aInfo.MSerialize().MToJSON(std::cout, true);
//	std::cout << std::endl;
//	std::cout << "====================" << std::endl;
	CDataObject::sMGetInstance().MPush(aInfo);
}

bool CInfoService::MAddOrUpdateClientTo(kernel_infos_t& aTo,
		const kernel_link& aCustomer)
{
	kern_links_t& _pid = aTo.FCustomerInfo;
	kern_links_t::iterator _jt = _pid.begin();
	for (; _jt != _pid.end() && !(*_jt == aCustomer); ++_jt)
		;

	VLOG_IF(1, _jt == _pid.end()) << "The customer " << aCustomer
											<< " is not exist.";
	bool _is = false;
	if (_jt != _pid.end())
	{
		VLOG(2) << aCustomer << " is exist.";
		if (*_jt == aCustomer)
		{
			VLOG(2) << aCustomer << " is'not changed";
		}
		else
		{
			_pid.erase(_jt);
			_pid.insert(aCustomer); //fixme if returned true value the node graph will changing,
			//but the uuid of client is not changed therefore The fail occurs.
			_is = true;
		}
	}
	else
	{
		VLOG(2) << "insert the new customer " << aCustomer
							<< " into the array.";
		_is = _pid.insert(aCustomer).second;
	}
	if (_is)
	{
		VLOG(2) << "Updating time";
	}
	return false;
}

void CInfoService::MPopConsumerFromMyInfo(const descriptor_t& aFrom,
		const kernel_link& aNew, _data_info_t& _d_info, k_diff_t& aRemoved)
{
	const kernel_infos_t _my_info(get_my_id());

	{
		k_diff_t::value_type _old(_my_info, false);
		_old.first.FCustomerInfo.insert(aNew);
		aRemoved.push_back(_old);
	}
	{
		kernel_infos_array_t::iterator _it = _d_info.FNet.find(_my_info);
		CHECK(_it != _d_info.FNet.end());
		kernel_infos_t _new = *_it;
		 _d_info.FNet.erase(
				_it++);
		_new.FCustomerInfo.erase(aNew);
		++_new.FIndexNumber;
		_d_info.FNet.insert(_it, _new);
	}

	_d_info.FAllDescriptors.erase(
			std::remove(_d_info.FAllDescriptors.begin(),
					_d_info.FAllDescriptors.end(), aFrom),
			_d_info.FAllDescriptors.end());

	_d_info.FNet.MWasChanged();
}

void CInfoService::MPutConsumerToMyInfo(const descriptor_t& aFrom,
		const kernel_link& aNew, _data_info_t& _d_info, k_diff_t& aAdded)
{
	const kernel_infos_t _my_info(get_my_id());
	{
		k_diff_t::value_type _new(_my_info, false);
		_new.first.FCustomerInfo.insert(aNew);
		aAdded.push_back(_new);
	}
	kernel_infos_array_t::iterator _it = _d_info.FNet.find(_my_info);
	CHECK(_it != _d_info.FNet.end());
	kernel_infos_t _new = *_it;
	_d_info.FNet.erase(_it++);

	_new.FCustomerInfo.insert(aNew).second;
	++_new.FIndexNumber;
	_d_info.FNet.insert(_it, _new);

	_d_info.FAllDescriptors.push_back(aFrom);

	_d_info.FNet.MWasChanged();

}
bool CInfoService::MAddKernel(const kernel_link& aInfo,
		const descriptor_t& aFrom, _data_info_t& _d_info,
		kernel_infos_diff_t& _diff)
{
	VLOG(2) << "Add kernel " << aInfo.FProgramm.FId.FUuid;

	uuid_from_t::iterator _it = _d_info.FUUIDFrom.find(
			aInfo.FProgramm.FId.FUuid);
	if (_it == _d_info.FUUIDFrom.end())
	{
		kernel_infos_t _kernel(aInfo.FProgramm);
		MPutKernel(_kernel, aFrom, _diff.FOpened, _d_info.FUUIDFrom,
				_d_info.FNet);
		return true;
	}
	else
	{
		VLOG(2) << "Kernel is preset.";
		//DCHECK(_it->second.find(aFrom) == _it->second.end());
		return _it->second.insert(aFrom).second;
	}
}
void CInfoService::MHandleOpen(const descriptor_t& aFrom,
		const descriptor_info_t& aInfo)
{
	VLOG(2) << "New: " << aInfo;
	NSHARE::CRAII<NSHARE::CMutex> _block(FUpdateMutex);
	VLOG(2) << "My turn ";
	descriptors_t _sent_to;
	kernel_infos_diff_t _diff;
	{
		w_access _access = FData.MGetWAccess();
		_data_info_t& _d_info = _access.MGet();
		MPutConsumerToMyInfo(aFrom, aInfo, _d_info, _diff.FOpened);

		//put to child info
		switch (aInfo.FProgramm.FType)
		{
		case E_CONSUMER:
		{
			LOG(INFO)<< " The consumer has been connected."<<aInfo.FProgramm.FId;
			_sent_to = _d_info.FAllDescriptors;	//send net to all
		}
		break;
		case E_KERNEL:
		{
			LOG(INFO)<< " The kernel has been connected."<<aInfo.FProgramm.FId;
			if(MAddKernel(aInfo, aFrom, _d_info, _diff))
			_sent_to.push_back(aFrom);
			else
			_sent_to = _d_info.FAllDescriptors;	//send net to all
		}
		break;
	}
	//updating routing graph
		{
			CRouteGraph::vertexs_t _v;
			_v[aInfo.FProgramm.FId.FUuid] = aInfo.FLatency;
			_d_info.FGraph.MAddNode(get_my_id().FId.FUuid, _v);
		}
		_d_info.MResetCache();
	}
	if (_diff.MIsValid())
	{
		MSynchronize(_sent_to, _diff);
	}
	VLOG(2) << "Opening of " << aInfo << " finished";
}
bool CInfoService::MIsCustomer(const kernel_link& _what,
		_data_info_t& _d_info) const
{
	const program_id_t& _id = get_my_id();
	kernel_infos_array_t::const_iterator _it = _d_info.FNet.find(
			kernel_infos_t(_id));
	DCHECK(_it != _d_info.FNet.end());
	if (_it != _d_info.FNet.end())
	{
		{
			kern_links_t const& _progs = _it->FCustomerInfo;
			kern_links_t::const_iterator _jt = _progs.find(_what);
			if (_jt != _progs.end())
			{
				VLOG(2) << _what.FProgramm.FId << " is exist.";
				return true;
			}
		}
	}
	return false;
}

bool CInfoService::MRemoveKernel(const kernel_link& _what,
		const descriptor_t& aFrom, _data_info_t& _d_info,
		kernel_infos_diff_t& _diff)
{
	kernel_infos_array_t& _net = _d_info.FNet;
	uuid_from_t& _from = _d_info.FUUIDFrom;

	/*{
	 uuid_from_t::iterator _it = _from.find(_what.FProgramm.FId.FUuid);
	 CHECK(_it != _from.end());
	 _it->second.erase(aFrom);
	 }*/

	kernel_list_t::iterator _net_it = _net.begin();

	for (; _net_it != _net.end();)
	{
		uuid_from_t::iterator _it = _from.find(_net_it->FKernelInfo.FId.FUuid);
		CHECK(_it != _from.end());
		bool _is_removed = false;
		if (_it->second.erase(aFrom) != 0)
		{
			VLOG(5) << "Kernel is from " << aFrom;
			if (_it->second.empty())
			{
				VLOG(5) << "Kernel has been removed as connected by "
									<< _net_it->FKernelInfo.FId.FUuid;

				_diff.FClosed.push_back(k_diff_t::value_type(*_net_it, true));
				_from.erase(_it);

				 _net.erase(_net_it++);
				_is_removed = true;
			}
		}
		if (!_is_removed)
			++_net_it;
	}

	return true;
}

void CInfoService::MHandleClose(const descriptor_t& aFrom,
		const descriptor_info_t& _info)
{
	VLOG(2) << "Close: " << _info;
	NSHARE::CRAII<NSHARE::CMutex> _block(FUpdateMutex);
	VLOG(2) << "My turn ";
	const kernel_link& _what = _info;
	descriptors_t _sent_to;
	kernel_infos_diff_t _diff;
	{
		w_access _access = FData.MGetWAccess();

		_data_info_t& _d_info = _access.MGet();

		MPopConsumerFromMyInfo(aFrom, _what, _d_info, _diff.FClosed);

		_sent_to = _d_info.FAllDescriptors;	//inform all

		switch (_what.FProgramm.FType)
		{
		case E_CONSUMER:
		{
			_d_info.FGraph.MRemoveNode(_what.FProgramm.FId.FUuid);
			break;
		}

		case E_KERNEL:
		{
			VLOG(2) << "looking for the kernel in the kernel list";
			MRemoveKernel(_what, aFrom, _d_info, _diff);
			MRebuildGraph(_d_info);
			break;
		}

		}

		_d_info.MResetCache();
	}
	if (_diff.MIsValid())
	{
		MSynchronize(_sent_to, _diff);
	}
	VLOG(2) << "Finished Closing of " << _info;
}

void CInfoService::MSynchronize(const descriptors_t& _sent_to,
		const kernel_infos_diff_t& _diff)
{
	MSendNet(_sent_to);
	MChangeInform(_diff);
}

void CInfoService::MGetCustomerDiff(kernel_infos_t const& _it_old,
		kernel_infos_t const& _it_new, k_diff_t& aRemoved, k_diff_t& aAdded)
{
	{
		k_diff_t::value_type _info;
		_info.second = false;
		std::set_difference(_it_old.FCustomerInfo.begin(),
				_it_old.FCustomerInfo.end(), _it_new.FCustomerInfo.begin(),
				_it_new.FCustomerInfo.end(),
				std::inserter(_info.first.FCustomerInfo,
						_info.first.FCustomerInfo.end()));
		if (!_info.first.FCustomerInfo.empty())
		{
			_info.first.FKernelInfo = _it_new.FKernelInfo;
			_info.first.FIndexNumber = _it_new.FIndexNumber;
			VLOG(5) << "Removed customers " << _info.first;
			aRemoved.push_back(_info);
		}
	}
	{
		k_diff_t::value_type _info;
		_info.second = false;
		std::set_difference(_it_new.FCustomerInfo.begin(),
				_it_new.FCustomerInfo.end(), _it_old.FCustomerInfo.begin(),
				_it_old.FCustomerInfo.end(),
				std::inserter(_info.first.FCustomerInfo,
						_info.first.FCustomerInfo.end()));
		if (!_info.first.FCustomerInfo.empty())
		{
			_info.first.FKernelInfo = _it_old.FKernelInfo;
			_info.first.FIndexNumber = _it_old.FIndexNumber;
			VLOG(5) << "Added customers " << _info.first;
			aAdded.push_back(_info);
		}
	}
}

void CInfoService::MPutKernel(kernel_infos_t const& aNew,
		const descriptor_t& aFrom, k_diff_t& aAdded, uuid_from_t& _from,
		kernel_infos_array_t& aTo)
{
	VLOG(3) << "Kernel is not present. " << " From " << aFrom << " UUID:"
						<< aNew.FKernelInfo.FId.FUuid;
	VLOG(5) << aNew.MSerialize().MToJSON(true);

	aAdded.push_back(k_diff_t::value_type(aNew, true));
	DCHECK(_from.find(aNew.FKernelInfo.FId.FUuid) == _from.end());
	_from[aNew.FKernelInfo.FId.FUuid].insert(aFrom);
	aTo.insert(aNew);
}

void CInfoService::MPopKernel(kernel_infos_t const& aOld,
		const descriptor_t& aFrom, k_diff_t& aRemoved, uuid_from_t& _from,
		kernel_infos_array_t& aTo)
{
	VLOG(3) << "Kernel is not present. " << aOld.FKernelInfo.FId.FUuid;
	uuid_from_t::iterator _it = _from.find(aOld.FKernelInfo.FId.FUuid);
	CHECK(_it != _from.end());
	bool _is_removed = false;
	if (_it->second.erase(aFrom) != 0)
	{
		VLOG(5) << "Kernel is from " << aFrom;
		if (_it->second.empty())
		{
			VLOG(5) << "Kernel has been removed "
								<< aOld.MSerialize().MToJSON(true);
			aRemoved.push_back(k_diff_t::value_type(aOld, true));
			_from.erase(_it);
			_is_removed = true;
		}
	}

	if (!_is_removed)
	{
		aTo.insert(aOld);
	}
}
bool CInfoService::MSynchronizeNetInfo(_data_info_t& _d_info,
		const kernel_infos_array_t& aNewNet, descriptor_t const& aFrom,
		kernel_infos_diff_t& aDiff)
{
	VLOG(2) << "MSynchronizeInfo";

	kernel_infos_array_t& _old_net = _d_info.FNet;
	uuid_from_t& _from = _d_info.FUUIDFrom;
	k_diff_t& _was_removed = aDiff.FClosed;
	k_diff_t& _was_added = aDiff.FOpened;

	kernel_infos_array_t _new_net;

	VLOG(5) << _old_net.MSerialize().MToJSON(true);
	VLOG(5) << aNewNet.MSerialize().MToJSON(true);

	kernel_list_t::const_iterator _it_new = aNewNet.begin();
	kernel_list_t::const_iterator _new_end = aNewNet.end();
	kernel_list_t::const_iterator _it_old = _old_net.begin();
	kernel_list_t::const_iterator _old_end = _old_net.end();

	const kernel_list_t::key_compare _compare = kernel_list_t::key_compare();
	while (_it_new != _new_end && _it_old != _old_end)
		if (_compare(*_it_new, *_it_old))
		{
			MPutKernel(*_it_new, aFrom, _was_added, _from, _new_net);
			++_it_new;
		}
		else if (_compare(*_it_old, *_it_new))
		{
			MPopKernel(*_it_old, aFrom, _was_removed, _from, _new_net);
			++_it_old;
		}
		else
		{
			VLOG(5) << _it_new->MSerialize().MToJSON(true);
			VLOG(5) << _it_old->MSerialize().MToJSON(true);

			//_from[_it_new->FKernelInfo.FId.FUuid].insert(aFrom);
			//обдумать. Как проверить что *_it_new действительно подключён через aFrom
			// а не является следствием синхронизации

			if (_it_new->FIndexNumber > _it_old->FIndexNumber)//handle only new change
			{
				VLOG(4) << "The Data is new  Our:" << _it_old->FIndexNumber
									<< " Its:" << _it_new->FIndexNumber;
				MGetCustomerDiff(*_it_old, *_it_new, _was_removed, _was_added);
				_new_net.insert(*_it_new);
			}
			else
			{
				VLOG(4) << "The Data is out of date. Our:"
									<< _it_old->FIndexNumber << " Its:"
									<< _it_new->FIndexNumber;
				_new_net.insert(*_it_old);
			}

			++_it_new;
			++_it_old;
		}

	for (; _it_new != _new_end; ++_it_new)
	{
		MPutKernel(*_it_new, aFrom, _was_added, _from, _new_net);
	}
	for (; _it_old != _old_end; ++_it_old)
	{
		MPopKernel(*_it_old, aFrom, _was_removed, _from, _new_net);
	}
	bool const _is_changed = !_was_removed.empty() || !_was_added.empty();
	if (_is_changed)
	{
		_old_net.swap(_new_net);
		_old_net.MWasChanged();
		VLOG(5) << "Now :" << _old_net.MSerialize().MToJSON(true);
	}
	return _is_changed;
}
void CInfoService::MHandle(const kernel_infos_array_t& aNet,
		descriptor_t const& aFrom)
{
	VLOG(2) << "Kernel Info " << aFrom << " info="
						<< CDescriptors::sMGetInstance().MGet(aFrom).first;

	NSHARE::CRAII<NSHARE::CMutex> _block(FUpdateMutex);
	VLOG(2) << "My turn ";
	descriptors_t _sent_to;
	kernel_infos_diff_t _diff;
	{
		w_access _access = FData.MGetWAccess();
		_data_info_t& _d_info = _access.MGet();
		{
			VLOG(2) << "Handle data from " << aFrom;
			const bool _is_change = MSynchronizeNetInfo(_d_info, aNet, aFrom,
					_diff);
			if (_is_change)
			{
				//The kernel info can bee received before The Descriptor has been opened
				//Thus ignoring synchronize
				if (std::find(_d_info.FAllDescriptors.begin(),
						_d_info.FAllDescriptors.end(), aFrom)
						!= _d_info.FAllDescriptors.end())
				{
					//send to all without aFrom
					_sent_to = _d_info.FAllDescriptors;
					_sent_to.erase(
							std::remove(_sent_to.begin(), _sent_to.end(),
									aFrom), _sent_to.end());
				}

				_d_info.MResetCache();
				MRebuildGraph(_d_info);
			}

		}
	}
	if (_diff.MIsValid())
	{
		MSynchronize(_sent_to, _diff);
	}
	VLOG(2) << "Finished Handling Kernel Info result.";
}

inline void CInfoService::MInit()
{
	{
		w_access _access = FData.MGetWAccess();
		kernel_infos_t _my_info(get_my_id());

		k_diff_t _fix;
		MPutKernel(_my_info, CDescriptors::INVALID, _fix, _access->FUUIDFrom,
				_access->FNet);
	}
	{
		callback_data_t _cb(sMHandleOpenId, this);
		CDataObject::value_t _val(open_descriptor::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}
	{
		callback_data_t _cb(sMHandleCloseId, this);
		CDataObject::value_t _val(close_descriptor::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}
	{
		callback_data_t _cb(sMHandleKernelInfo, this);
		CDataObject::value_t _val(kernel_infos_array_id_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}
}

std::vector<descriptor_t> CInfoService::MNextDestinations(
		CRouteGraph::node_t const& aTo) const
{
	{
		const r_access _ac = FData.MGetRAccess();
		const _data_info_t& _info = _ac.MGet();
		next_destination_t::const_iterator _it = _info.FDestianationCache.find(
				aTo);
		if (_it != _info.FDestianationCache.end())
			return _it->second;
	}
	{
		w_access _ac = FData.MGetWAccess();
		_data_info_t& _info = _ac.MGet();
		CRouteGraph::path_t const _short = _info.FGraph.MShortestPath(
				get_my_id().FId.FUuid, aTo);		//todo all

		std::vector<descriptor_t> _next;
		if (!_short.empty())
		{
			descriptor_t const _val = CDescriptors::sMGetInstance().MGet(
					_short.front());
			if (_val != CDescriptors::INVALID)
			{
				_next.push_back(_val);
			}else
			{
				LOG(ERROR)<<"No descriptor for "<<	_short.front();
			}
		}
		_info.FDestianationCache[aTo] = _next;
		LOG_IF(INFO,_next.empty()) << "No pathes to " << aTo;
		return _next;
	}
}
bool CInfoService::MIsVertex(const CRouteGraph::node_t& name) const
{
	const r_access _ac = FData.MGetRAccess();
	const _data_info_t& _info = _ac.MGet();
	return _info.FGraph.MIsVertex(name);
}
uuids_t CInfoService::MGetOtherKernelds() const
{
	uuids_t _rval;
	const r_access _ac = FData.MGetRAccess();
	const _data_info_t& _info = _ac.MGet();
	kernel_infos_array_t::const_iterator _it = _info.FNet.begin(), _it_end(
			_info.FNet.end());
	for (; _it != _it_end; ++_it)
	{
		if (_it->FKernelInfo.FId.FUuid != get_my_id().FId.FUuid)
			_rval.push_back(_it->FKernelInfo.FId.FUuid);
	}
	return _rval;
}
NSHARE::CConfig CInfoService::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	{
		r_access _ac = FData.MGetRAccess();
		const _data_info_t& _info = _ac.MGet();
		kernel_infos_array_t::const_iterator _it = _info.FNet.begin(), _it_end(
				_info.FNet.end());

		for (; _it != _it_end; ++_it)
		{
			_config.MAdd(kernel_infos_t::NAME, _it->MSerialize());
		}
		_config.MAdd(_info.FGraph.MSerialize());
	}
	return _config;
}
void CInfoService::_data_info_t::MResetCache()
{
	FGraph.MResetCache();
	FDestianationCache.clear();
}
} /* namespace NUDT */
