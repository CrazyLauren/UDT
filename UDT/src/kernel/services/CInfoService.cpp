/*
 * CInfoService.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 19.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <algorithm>
#include <iterator>
#include <core/IState.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/kernel_type.h>
#include <io/CKernelIo.h>
#include "CInfoService.h"


#include <boost/version.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#if (BOOST_VERSION / 100000 >=1) &&(BOOST_VERSION / 100 % 1000<=47)
using namespace boost::interprocess::detail;
#else
using namespace boost::interprocess::ipcdetail;
#endif
template<>
NUDT::CInfoService::singleton_pnt_t NUDT::CInfoService::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CInfoService::NAME = "udt_net";
CInfoService::CInfoService():IState(NAME)
{
	MInit();

}

CInfoService::~CInfoService()
{

}

void CInfoService::MSincAll(bool aIsChange, _data_info_t& _d_info,
		descriptors_t& _sent_to)
{
	typedef CDescriptors::d_list_t _list_t;
	CDescriptors::d_list_t _list;
	CDescriptors::sMGetInstance().MGetAll(_list);
	for (_list_t::const_iterator _jt = _list.begin(); _jt != _list.end(); ++_jt)
	{
		VLOG(4) << "Descriptor: " << _jt->first << " " << _jt->second;

		k_info_by_descriptor_t::iterator _kt = _d_info.FInfoForDesc.find(
				_jt->first);
		if (_kt == _d_info.FInfoForDesc.end())
		{
			//Send to E_CUSTOMER clients if data changed
			if (aIsChange)
			{
				VLOG(2) << "Send to E_CUSTOMER " << _jt->first;
				_sent_to.push_back(_jt->first);
			}
		}
		else
		{
			//Send to another clients if The lists is not equal
			if (_kt->second.FInfo != _d_info.FInfo)
			{
				VLOG(2) << "Send to E_KERNEL " << _jt->first;
				_sent_to.push_back(_jt->first);
				_kt->second.FInfo = _d_info.FInfo;
			}
			else
			{
				VLOG(2) << "Equal " << _jt->second.MGetConst();
				VLOG(4) << _kt->second.FInfo.MSerialize().MToJSON(true);
			}
		}
	}
}

void CInfoService::MSendInfo(const descriptors_t& _sent_to)
{
	if (!_sent_to.empty())
	{
		const r_access _access = FData.MGetRAccess();
		descriptors_t::const_iterator _it = _sent_to.begin(), _it_end(
				_sent_to.end());
		for (; _it != _it_end; ++_it)
		{
			VLOG(2) << "Sent to " << *_it;
			CKernelIo::sMGetInstance().MSendTo(*_it, _access.MGet().FInfo);
		}
	}
}

void CInfoService::MSynchronize2(descriptors_t& _sent_to, _data_info_t& _d_info,
		bool aIsChange)
{
	if (aIsChange)
		atomic_inc32(&FNumberOfChange);

	VLOG(2) << "Number of change " << FNumberOfChange << " is changed "
						<< aIsChange;
	VLOG(4) << _d_info.FInfo.MSerialize().MToJSON(true);
	MSincAll(aIsChange, _d_info, _sent_to);
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

	kernel_infos_array_t::const_iterator _it = _new_array.FInfo.begin(),
			_it_end(_new_array.FInfo.end());
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

void CInfoService::MRebuldKernels(const kern_links_t& _kinfo,
		const kernel_infos_array_t& _actual_array, _data_info_t& _new_array)
{
	kern_links_t::const_iterator _it = _kinfo.begin(), _it_end(_kinfo.end());
	for (; _it != _it_end; ++_it)
	{
		if (_it->FProgramm.FType == E_KERNEL)
		{

			VLOG(2) << "Looking for other customers and kernels. "
								<< _it->FProgramm;
			//MRebuildKernel(_new_array, _actual_array,*_it);
			MAddNewKernelTo(*_it, _new_array, _actual_array);
		}
	}
}

void CInfoService::MGetActualInfo(_data_info_t& _new_array,
		kernel_infos_array_t& _actual_array)
{
	k_info_by_descriptor_t::const_iterator _it =
			_new_array.FInfoForDesc.begin(), _it_end(
			_new_array.FInfoForDesc.end());
	for (; _it != _it_end; ++_it)
	{
		kernel_infos_array_t::const_iterator _jt = _it->second.FInfo.begin(),
				_jt_end = _it->second.FInfo.end();
		for (; _jt != _jt_end; ++_jt)
		{
			kernel_infos_array_t::iterator _kt = _actual_array.find(*_jt);
			if (_kt != _actual_array.end())
			{
				VLOG(2) << "Ups! " << _jt->FKernelInfo.FId << " is exist :"
									<< " Cur" << _kt->FIndexNumber << " Our "
									<< _jt->FIndexNumber;
				//todo wait counter

				if (_kt->FIndexNumber < _jt->FIndexNumber)
				{
					VLOG(2) << "info from " << _it->first << " latest";

					_actual_array.erase(_kt);
					_actual_array.insert(*_jt);
				}
				else
				{
					VLOG(2) << "Ignoring info from " << _it->first;
				}
			}
			else
			{
				VLOG(4) << "Add new " << _jt->MSerialize().MToJSON(true);
				_actual_array.insert(*_jt);
			}
		}
	}
}

bool CInfoService::MRebuild(_data_info_t& _new_array,
		kernel_infos_diff_t& _diff)
{
	VLOG(2) << "Rebuild kernel";

	kernel_infos_array_t _old;

	_old.swap(_new_array.FInfo);

	CHECK(_new_array.FInfo.empty());

	_new_array.MResetCache();
	_new_array.FWayCounter.clear();
	_new_array.FGraph.MReset();


	const kernel_infos_t _inf(CDescriptors::sMGetInstance().MGetInfos(NULL));

	VLOG(2) << _inf.FKernelInfo.FId << " idx=" << _inf.FIndexNumber;
	_new_array.FWayCounter[get_my_id().FId] = k_counter();

	kernel_infos_array_t::iterator _jt = _new_array.FInfo.insert(_inf).first;

	kernel_infos_array_t _actual_array;

	MGetActualInfo(_new_array, _actual_array);

	MRebuldKernels(_jt->FCustomerInfo, _actual_array, _new_array);//can be change for  _actual_array+ _inf

	MRebuildGraph(_new_array);

	VLOG(2) << "Finish Rebuilding";
	return MSetDiff(_old, _new_array.FInfo, _diff.FClosed, _diff.FOpened);
}
void CInfoService::MAddNewKernelTo(const kernel_link& _info,
		_data_info_t& _new_array, const kernel_infos_array_t& aKernelList)
{
	kernel_infos_t _new(_info.FProgramm);
	{
		kernel_infos_array_t::const_iterator _mit = aKernelList.find(_new);
		_new.FIndexNumber = _mit == aKernelList.end() ? 0 : _mit->FIndexNumber;
	}
	VLOG(2) << "Cur index " << _new.FIndexNumber;

	kernels_counter_t::iterator _ct = _new_array.FWayCounter.find(
			_info.FProgramm.FId);
	if (_ct == _new_array.FWayCounter.end())
	{
		VLOG(2) << "The first";
		_new_array.FWayCounter[_info.FProgramm.FId] = k_counter();

		MAddingNewKernelTo(_new, _new_array, aKernelList);
	}
	else
	{
		++_ct->second.FCount;
		VLOG(2) << "Ups! " << _info.FProgramm.FId << " is exist :"<<_ct->second.FCount;

		VLOG(2) << "Ignoring ";
	}
}
void CInfoService::MAddingNewKernelTo(kernel_infos_t& _new,
		_data_info_t& _new_array, const kernel_infos_array_t& aKernelList)
{
	VLOG(2) << "Adding a new kernel " << _new.FKernelInfo;

	CHECK(
			_new_array.FWayCounter.find(_new.FKernelInfo.FId)
					!= _new_array.FWayCounter.end());

	kernel_infos_array_t::const_iterator _it = aKernelList.find(_new);
	CHECK(_it != aKernelList.end());

	if (_it != aKernelList.end())
	{
		VLOG(2) << "Find customers.";
		kern_links_t::const_iterator _jt = _it->FCustomerInfo.begin(), _jt_end(
				_it->FCustomerInfo.end());

		for (; _jt != _jt_end; ++_jt)
		{
			_new.FCustomerInfo.insert(*_jt);

			VLOG(2) << "Add customer" << *_jt;
			switch(_jt->FProgramm.FType)
			{
				case  E_CONSUMER:
				break;
				case E_KERNEL:
					VLOG(2) << "Founding the other kernels";
					MAddNewKernelTo(*_jt, _new_array, aKernelList);
				break;
			}
		}
	}
	else
	{
		LOG(ERROR)<< "Kernel info for " << _new.FKernelInfo.FId
		<< " is not exist";
	}

	_new_array.FInfo.insert(_new);
	VLOG(2) << "Kernel is added ";
}

bool CInfoService::MSetDiff(const kernel_infos_array_t& aOld,
		const kernel_infos_array_t& aNewArray, k_diff_t& aRemoved,
		k_diff_t& aAdded)
{
	VLOG(2) << "Get different";
	VLOG(5) << aOld.MSerialize().MToJSON(true);
	VLOG(5) << aNewArray.MSerialize().MToJSON(true);

	kernel_infos_array_t::const_iterator _it_new = aNewArray.begin();
	kernel_infos_array_t::const_iterator _new_end = aNewArray.end();
	kernel_infos_array_t::const_iterator _it_old = aOld.begin();
	kernel_infos_array_t::const_iterator _old_end = aOld.end();
	const kernel_infos_array_t::key_compare _compare=kernel_infos_array_t::key_compare();//gcc fix
	while (_it_new != _new_end && _it_old != _old_end)
		if (_compare(*_it_new, *_it_old))
		{
			VLOG(5) << "Kernel has been added " << (*_it_new);
			aAdded.push_back(k_diff_t::value_type(*_it_new, true));
			++_it_new;
		}
		else if (_compare(*_it_old, *_it_new))
		{

			VLOG(5) << "Kernel has been removed " << (*_it_old);
			aRemoved.push_back(k_diff_t::value_type(*_it_old, true));
			++_it_old;
		}
		else
		{
			{
				k_diff_t::value_type _info;
				_info.second = false;
				std::set_difference(_it_old->FCustomerInfo.begin(),
						_it_old->FCustomerInfo.end(),
						_it_new->FCustomerInfo.begin(),
						_it_new->FCustomerInfo.end(),
						std::inserter(_info.first.FCustomerInfo,
								_info.first.FCustomerInfo.end()));
				if (!_info.first.FCustomerInfo.empty())
				{
					_info.first.FKernelInfo = _it_new->FKernelInfo;
					_info.first.FIndexNumber = _it_new->FIndexNumber;

					VLOG(5) << "Removed customers " << _info.first;
					aRemoved.push_back(_info);
				}
			}
			{
				k_diff_t::value_type _info;
				_info.second = false;
				std::set_difference(_it_new->FCustomerInfo.begin(),
						_it_new->FCustomerInfo.end(),
						_it_old->FCustomerInfo.begin(),
						_it_old->FCustomerInfo.end(),
						std::inserter(_info.first.FCustomerInfo,
								_info.first.FCustomerInfo.end()));
				if (!_info.first.FCustomerInfo.empty())
				{
					_info.first.FKernelInfo = _it_old->FKernelInfo;
					_info.first.FIndexNumber = _it_old->FIndexNumber;

					VLOG(5) << "Added customers " << _info.first;
					aAdded.push_back(_info);
				}
			}
			++_it_new;
			++_it_old;
		}
	for (; _it_new != _new_end; ++_it_new)
	{
		VLOG(5) << "Kernel has been added " << (*_it_new);
		aAdded.push_back(k_diff_t::value_type(*_it_new, true));
	}
	for (; _it_old != _old_end; ++_it_old)
	{
		VLOG(5) << "Kernel has been removed " << (*_it_old);
		aRemoved.push_back(k_diff_t::value_type(*_it_old, true));
	}
	return !aRemoved.empty() || !aAdded.empty();
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
bool CInfoService::MAddNewKernel(const kernel_link& _new, _data_info_t& _d_info,
		kernel_infos_diff_t& _diff)
{
	CHECK_EQ(_new.FProgramm.FType, E_KERNEL);
	LOG(INFO)<< " The kernel has been connected.";
	kernel_infos_t _kernel;
	_kernel.FKernelInfo = _new.FProgramm;
	_d_info.FInfo.insert(_kernel);
	++_d_info.FWayCounter[_kernel.FKernelInfo.FId].FCount;
	_diff.FOpened.push_back(k_diff_t::value_type(_kernel, true));
	return true;
}

void CInfoService::MDebugPrintState() const
{
	/*std::fstream _file;
	_file.open("./map.json",
			std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	if (_file.is_open())
	{
		r_access _access = FData.MGetRAccess();
		const _data_info_t& _d_info = _access.MGet();
		//		std::cout << "+-+-+-+-+-+-+-+-+-+-" << std::endl;
		//		std::cout << FNumberOfChange << std::endl;
		_d_info.FInfo.MSerialize().MToJSON(_file, true);
		//		std::cout << "+-+-+-+-+-+-+-+-+-+-" << std::endl;
		_file.close();
	}*/
}

bool CInfoService::MAddCustomerOptimizing(const kernel_link& _what,
		kernel_infos_diff_t& _diff)
{
	VLOG(2) << "Add new customer " << _what;
	const kernel_infos_t _inf(get_my_id());
	k_diff_t::value_type _new(_inf, false);
	_new.first.FCustomerInfo.insert(_what);
	_diff.FOpened.push_back(_new);
	return true;
}

bool CInfoService::MUpdateMyInfo(_data_info_t& _d_info)
{
	//check is exit
	bool _is_changed=false;
	{
		const program_id_t& _id = get_my_id();
		kernel_infos_array_t::iterator _it = _d_info.FInfo.find(
				kernel_infos_t(_id));
		DCHECK(_it != _d_info.FInfo.end());
		if (_it != _d_info.FInfo.end())
		{
			//Update
			kernel_infos_t _info(CDescriptors::sMGetInstance().MGetInfos());
			if (_info != *_it)
			{
				_d_info.FInfo.erase(_it);
				_d_info.FInfo.insert(_info);
				_is_changed = true;
			}
		}

	}
	return _is_changed;
}

void CInfoService::MHandleOpen(const descriptor_t& aFrom,
		const descriptor_info_t& aInfo)
{
	VLOG(2) << "New From: " << aInfo;
	NSHARE::CRAII<NSHARE::CMutex> _block(FUpdateMutex);
	VLOG(2) << "My turn ";
	//fast check before the thread is locked
	bool _is_changed = false;
	const kernel_link& _what = aInfo;
	descriptors_t _sent_to;
	kernel_infos_diff_t _diff;
	{
		w_access _access = FData.MGetWAccess();
		_data_info_t& _d_info = _access.MGet();
		_is_changed = (!MIsCustomer(aInfo, _d_info)) && MUpdateMyInfo(_d_info);
//		if (!_is_changed)
//		{
//			CHECK(
//					_d_info.FGraph.MIsEnge(get_my_id().FId.FUuid,
//							_what.FProgramm.FId.FUuid) &&		//there is enge
//							(_what.FProgramm.FType == E_CONSUMER ||		//
//									_d_info.FGraph.MIsVertex(
//											_what.FProgramm.FId.FUuid)));
//			VLOG(2) << "does not change";
//			return;
//		}
		switch(_what.FProgramm.FType )
		{
				case  E_CONSUMER:
					_is_changed = MAddCustomerOptimizing(_what, _diff);
					DCHECK(_is_changed);

					MSynchronize2(_sent_to, _d_info, _is_changed);
				break;
				case E_KERNEL:
					_is_changed = MAddNewKernel(_what, _d_info, _diff);
					DCHECK(_is_changed);

					_sent_to.push_back(aFrom);
					CHECK(_sent_to.size() == 1 && _sent_to.front() == aFrom);
				break;
		}
		//MSynchronize2(_sent_to, _d_info, _is_changed);
		//updating routing graph
		{
			CRouteGraph::vertexs_t _v;
			_v[_what.FProgramm.FId.FUuid] = _what.FLatency;
			_d_info.FGraph.MAddNode(get_my_id().FId.FUuid, _v);
		}
		_d_info.MResetCache();
	}
	MSendInfo(_sent_to);
	if (_diff.MIsValid())
	{
		MChangeInform(_diff);
		MDebugPrintState();
	}
	VLOG(2) << "Opening of " << _what << " finished";
}


bool CInfoService::MIsCustomer(const kernel_link& _what,
		_data_info_t& _d_info) const
{
	const program_id_t& _id = get_my_id();
	kernel_infos_array_t::const_iterator _it = _d_info.FInfo.find(
			kernel_infos_t(_id));
	DCHECK(_it != _d_info.FInfo.end());
	if (_it != _d_info.FInfo.end())
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

bool CInfoService::MRemoveCustomer(const kernel_link& _what,
		_data_info_t& _d_info, kernel_infos_diff_t& _diff)
{
	bool _is_changed;
	//MRemoveCustomerOptimizing(_what, _diff, _d_info);
	_is_changed = MUpdateMyInfo(_d_info);
	CHECK(_is_changed);
	const kernel_infos_t _inf(get_my_id());
	k_diff_t::value_type _old(_inf, false);
	_old.first.FCustomerInfo.insert(_what);
	_diff.FClosed.push_back(_old);
	//updating routing graph
	{
		_d_info.FGraph.MRemoveNode(_what.FProgramm.FId.FUuid);
	}
	return _is_changed;
}
bool CInfoService::MEraseKernel2(const kernel_link& _info,
		const descriptor_t& aFrom, _data_info_t& _d_info)
{
	kernel_infos_t _in;
	_in.FKernelInfo = _info.FProgramm;
	CHECK(_d_info.FInfo.find(_in) != _d_info.FInfo.end());
	//Then removing it and rebuilding the kernels info array
	return _d_info.FInfoForDesc.erase(aFrom) > 0;
}

bool CInfoService::MRemoveKernelImpl(const kernel_link& _what,
		const descriptor_t& aFrom, _data_info_t& _d_info,
		kernel_infos_diff_t& _diff)
{
	MEraseKernel2(_what, aFrom, _d_info);
	return MRebuild(_d_info, _diff);
}

void CInfoService::MHandleClose(const descriptor_t& aFrom,
		const descriptor_info_t& _info)
{
	VLOG(2) << "Close: " << _info;
	NSHARE::CRAII<NSHARE::CMutex> _block(FUpdateMutex);
	VLOG(2) << "My turn ";
	bool _is_changed = false;
	const kernel_link& _what = _info;
	descriptors_t _sent_to;
	kernel_infos_diff_t _diff;
	{
		w_access _access = FData.MGetWAccess();
		_data_info_t& _d_info = _access.MGet();
		_is_changed = MIsCustomer(_what, _d_info);
		if (!_is_changed)
		{
			LOG(ERROR)<< "The customer " << _what << " is not exist.";
			return;
		}
		switch(_what.FProgramm.FType )
		{
				case  E_CONSUMER:
					//MRemoveCustomerOptimizing(_what, _diff, _d_info);

					_is_changed = MRemoveCustomer(_what, _d_info, _diff);
				break;
				case E_KERNEL:
					VLOG(2) << "looking for the kernel in the kernel list";

					_is_changed = MRemoveKernelImpl(_what, aFrom, _d_info, _diff);
				break;
		}
		if (_is_changed)
		{
			MSynchronize2(_sent_to, _d_info, _is_changed);
		}
		_d_info.MResetCache();
	}
	MSendInfo(_sent_to);
	if (_diff.MIsValid())
	{
		MChangeInform(_diff);
		MDebugPrintState();
	}
	VLOG(2) << "Finished Closing of " << _info;
}

void CInfoService::MHandle(const kernel_infos_array_t& aFrom, descriptor_t aId)
{
	VLOG(2) << "Kernel Info " << aId << " info="
						<< CDescriptors::sMGetInstance().MGet(aId).first;
	NSHARE::CRAII<NSHARE::CMutex> _block(FUpdateMutex);
	VLOG(2) << "My turn ";
	bool _is_changed = false;
	bool _is_new = false;
	descriptors_t _sent_to;
	kernel_infos_diff_t _diff;
	{
		w_access _access = FData.MGetWAccess();
		_data_info_t& _d_info = _access.MGet();
		{
			VLOG(2) << "Handle data from " << aId;
			if (_d_info.FInfoForDesc[aId].FInfo != aFrom)
			{
				_d_info.FInfoForDesc[aId] = k_info_t(aFrom, ++FInfoPriority);
				_is_new = true;
				VLOG(5) << " From " << aId << " "
									<< aFrom.MSerialize().MToJSON(true);
			}
			else
			{
				VLOG(2) << "Does not change.";
			}
		}
		if (_is_new)
		{
			_is_changed = MRebuild(_d_info, _diff);
			MSynchronize2(_sent_to, _d_info, _is_changed);
		}
		_d_info.MResetCache();
	}
	MSendInfo(_sent_to);
	if (_is_changed)
	{
		MDebugPrintState();
		MChangeInform(_diff);
	}
	VLOG(2) << "Finished Handling Kernel Info result: new=" << _is_new
						<< " changed=" << _is_changed;
}

inline void CInfoService::MInit()
{
	FNumberOfChange = 0;
	FInfoPriority = 1;
	{
		w_access _access = FData.MGetWAccess();
		_access->FInfo.insert(CDescriptors::sMGetInstance().MGetInfos());
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

std::vector<descriptor_t> CInfoService::MNextDestinations(CRouteGraph::node_t const& aTo) const
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
	kernel_infos_array_t::const_iterator _it = _info.FInfo.begin(), _it_end(
			_info.FInfo.end());
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
	_config.MAdd("num", FNumberOfChange);
	{
		r_access _ac = FData.MGetRAccess();
		const _data_info_t& _info = _ac.MGet();
		kernel_infos_array_t::const_iterator _it = _info.FInfo.begin(), _it_end(
				_info.FInfo.end());

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
