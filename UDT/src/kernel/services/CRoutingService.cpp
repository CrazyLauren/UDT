/*
 * CRoutingService.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 16.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>
#include <internel_protocol.h>

#include "../core/kernel_type.h"
#include "../core/IState.h"
#include "../core/CDescriptors.h"
#include "../core/CDataObject.h"
#include "../io/CKernelIo.h"

#include "CRoutingService.h"
#include "CInfoService.h"

template<>
NUDT::CRoutingService::singleton_pnt_t NUDT::CRoutingService::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CRoutingService::NAME = "route";
CRoutingService::CRoutingService() :
		IState(NAME)
{
	MInit();
}

CRoutingService::~CRoutingService()
{
}
static void set_intersection(demand_dgs_for_t& aTo,
		demand_dgs_for_t const& aFrom)
{
	demand_dgs_for_t::iterator __first1 = aTo.begin(), __last1(aTo.end());
	demand_dgs_for_t::const_iterator __first2 = aFrom.begin(), __last2(
			aFrom.end());

	while (__first1 != __last1 && __first2 != __last2)
		if (__first1->first < __first2->first)
			++__first1;
		else if (__first2->first < __first1->first)
			++__first2;
		else
		{
			__first1->second.insert(__first1->second.end(),
					__first2->second.begin(), __first2->second.end());
			++__first1;
			++__first2;
		}
}
void CRoutingService::MHandleDiff(kernel_infos_diff_t const* aDiff)
{
	VLOG(2) << "Handle diff";
	uuids_t _inform_array;
	if (!aDiff->FOpened.empty())
	{
		k_diff_t::const_iterator _it = aDiff->FOpened.begin();
		k_diff_t::const_iterator _it_end = aDiff->FOpened.end();
		for (; _it != _it_end; ++_it)
		{
			MHandleNewId(_it->second, _it->first.FKernelInfo,
					_it->first.FCustomerInfo);

			if (_it->second)
			{
				//add new kernel
				_inform_array.push_back(_it->first.FKernelInfo.FId.FUuid);
			}
		}
	}
	if (!aDiff->FClosed.empty())
	{
		k_diff_t::const_iterator _it = aDiff->FClosed.begin();
		k_diff_t::const_iterator _it_end = aDiff->FClosed.end();
		for (; _it != _it_end; ++_it)
		{
			MHandleCloseId(_it->second, _it->first.FKernelInfo,
					_it->first.FCustomerInfo);
		}
	}
	if (!_inform_array.empty()) //inform new kernel aFor about demands of our customers
	{
		VLOG(2) << "inform new kernels  about demands of our customers. "
							<< _inform_array;
		demand_dgs_for_t _demands;
		r_route_access const _access = FRouteData.MGetRAccess();
		_route_t const & _route = _access.MGet();

		demand_dgs_for_t const& _all_d = _route.FRequiredDG.MGetDemands();
		const CDescriptors::d_list_t _list =
				CDescriptors::sMGetInstance().MGetAll(E_CONSUMER);
		CDescriptors::d_list_t::const_iterator _it = _list.begin(), _it_end(
				_list.end());

		for (; _it != _it_end; ++_it)
		{
			demand_dgs_for_t::const_iterator _jt = _all_d.find(
					_it->second.MGetConst().FProgramm.FId);
			if (_jt != _all_d.end())
			{
				VLOG(2) << "Push demands for " << _jt->first << " "
									<< _jt->second;
				_demands.insert(*_jt);
			}
		}
		if (!_demands.empty())
		{
			uuids_t _not_sent(
					MSendTo(routing_t(get_my_id().FId, _inform_array),
							_demands));
			LOG_IF(ERROR,!_not_sent.empty()) << "Cannot sent demands to "
														<< _not_sent;
			LOG_IF(INFO,_not_sent.empty()) << "Sent demands to "
													<< _inform_array;
		}
	}

}

void CRoutingService::MHandleNewDemands(descriptor_t aFrom,
		const demand_dgs_for_t& aDemands)
{
	VLOG(2) << "New demands  " << aDemands << " From: " << aFrom; //<<" to "<<aRoute;

	demand_dgs_for_t _new_receiver;
	w_route_access _access = FRouteData.MGetWAccess();
	{
		{
			const CDescriptors::d_list_t _list =
					CDescriptors::sMGetInstance().MGetAll(E_CONSUMER);
			CDescriptors::d_list_t::const_iterator _jt = _list.begin(), _jt_end(
					_list.end());

			for (; _jt != _jt_end; ++_jt)
			{
				_new_receiver.insert(
						demand_dgs_for_t::value_type(
								_jt->second.MGetConst().FProgramm.FId,
								demand_dgs_for_t::mapped_type()));
			}
		}

		_route_t & _route = _access.MGet();

		demand_dgs_for_t::const_iterator _it = aDemands.begin(), _it_end(
				aDemands.end());
		for (; _it != _it_end; ++_it)
		{
			demand_dgs_for_t const _added_to(
					_route.FRequiredDG.MSetDemandsDGFor(_it->first, _it->second).first);
			if (!_added_to.empty() && !_new_receiver.empty())
			{
				set_intersection(_new_receiver, _added_to);
			}
		}
	}
	if (!_new_receiver.empty())
	{
		MInformNewReceiver(_new_receiver);
	}
}
void CRoutingService::MHandleNewId(bool, program_id_t const& aFor,
		kern_links_t const& aNew)
{
	VLOG(2) << "New For: " << aFor;
	VLOG(5) << "id " << aNew;
	demand_dgs_for_t _new_receiver;
	{
		w_route_access _access = FRouteData.MGetWAccess();
		_route_t & _route = _access.MGet();

		{
			kern_links_t::const_iterator _it = aNew.begin(), _it_end =
					aNew.end();
			for (; _it != _it_end; ++_it)
			{
				VLOG(2) << "Next " << _it->FProgramm.FId;
				switch (_it->FProgramm.FType)
				{
				case E_KERNEL:
					break;
				case E_CONSUMER:
					_new_receiver = _route.FRequiredDG.MAddClient(
							_it->FProgramm.FId);
					break;
				}
			}
		}

	}

	if (!_new_receiver.empty())
		MInformNewReceiver(_new_receiver);

	VLOG(2) << "Handled";
}

void CRoutingService::MHandleCloseId(bool aIs, program_id_t const& aFor,
		kern_links_t const& aOld)
{
	VLOG(2) << "Close: " << aFor;
	VLOG(5) << "id " << aOld;
	{
		w_route_access _access = FRouteData.MGetWAccess();
		_route_t & _route = _access.MGet();

		kern_links_t::const_iterator _it = aOld.begin(), _it_end = aOld.end();
		for (; _it != _it_end; ++_it)
		{
			switch (_it->FProgramm.FType)
			{
			case E_KERNEL:
				break;
			case E_CONSUMER:
				_route.FRequiredDG.MRemoveClient(_it->FProgramm.FId.FUuid);
				break;
			}
		}
//		if(aIs)
//			_route.FRequiredDG.MRemoveClient(aFor.FId.FUuid);
	}
}

int CRoutingService::sMHandleDiff(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CRoutingService* _this = reinterpret_cast<CRoutingService*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(kernel_infos_diff_t::NAME, WHAT->FType);
	kernel_infos_diff_t const* _p =
			reinterpret_cast<kernel_infos_diff_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleDiff(_p);
	return 0;
}
int CRoutingService::sMHandleDemands(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CRoutingService* _this = reinterpret_cast<CRoutingService*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(demand_dgs_for_by_id_t::NAME, WHAT->FType);
	demand_dgs_for_by_id_t const* _p =
			reinterpret_cast<demand_dgs_for_by_id_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleNewDemands(_p->FId, _p->FVal);
	return 0;
}
int CRoutingService::sMHandleDemandId(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CRoutingService* _this = reinterpret_cast<CRoutingService*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(demands_id_t::NAME, WHAT->FType);
	demands_id_t const* _p = reinterpret_cast<demands_id_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleFrom(&_p->FVal, _p->FId);
	return 0;
}
void CRoutingService::MHandleFrom(demand_dgs_t const* aP, descriptor_t aFrom)
{
	VLOG(2) << "New demands  " << aP << " From: " << aFrom;
	std::pair<descriptor_info_t, bool> _info =
			CDescriptors::sMGetInstance().MGet(aFrom);
	demand_dgs_for_t _new_receiver;
	{
		w_route_access _access = FRouteData.MGetWAccess();

		VLOG_IF(2,!_info.second) << aFrom << " has been removed already.";
		if (_info.second)
		{
			_new_receiver = _access->FRequiredDG.MSetDemandsDGFor(
					_info.first.FProgramm.FId, *aP).first;
		}
	}
	if (!_new_receiver.empty())
		MInformNewReceiver(_new_receiver);

	uuids_t _inform_array(CInfoService::sMGetInstance().MGetOtherKernelds());
	if (!_inform_array.empty())
	{
		VLOG(2) << "inform new kernel aFor about demands of our customers "
							<< _inform_array;
		demand_dgs_for_t _c;
		_c.insert(demand_dgs_for_t::value_type(_info.first.FProgramm.FId, *aP));

		uuids_t _not_sent(
				MSendTo(routing_t(get_my_id().FId, _inform_array), _c));
		LOG_IF(ERROR,!_not_sent.empty()) << "Cannot sent demands to "
													<< _not_sent;
		LOG_IF(INFO,_not_sent.empty()) << "Sent demands to " << _inform_array;
	}
	else
	{
		VLOG(2) << "No another kernels";
	}
}
int CRoutingService::sMHandleUserDataId(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CRoutingService* _this = reinterpret_cast<CRoutingService*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(user_data_id_t::NAME, WHAT->FType);
	user_data_id_t const* _p =
			reinterpret_cast<user_data_id_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleFrom(&_p->FVal, _p->FId);
	return 0;
}

void CRoutingService::MNoteFailSend(const user_data_info_t& aInfo,
		const uuids_t& _non_sent)
{
	fail_send_t _sent(aInfo);
	_sent.FUUIDTo = _non_sent;
	MNoteFailSend(_sent);
}
void CRoutingService::MNoteFailSend(const fail_send_t& _sent)
{
	LOG(ERROR)<<"Cannot send  packet #"<<_sent.FPacketNumber<<" from " << _sent.FFrom << " to " << _sent.FUUIDTo.MGetConst();
	routing_t _f_to;
	_f_to.push_back(_sent.FFrom.FUuid);
	_f_to.FFrom = get_my_id().FId;
	MSendTo(_f_to, _sent);
}
CRequiredDG::req_uuids_t CRoutingService::MGetCustomersFor(
		user_data_t const& aData)
{
	r_route_access _access = FRouteData.MGetRAccess();
	CRequiredDG const& _rdg = _access->FRequiredDG;
	NSHARE::CBuffer const & _data = aData.FData;

	return (!aData.FDataId.FProtocol.empty() && aData.FDataId.FProtocol!=RAW_PROTOCOL_NAME) ? //if empty ==> numbered raw protocol
			_rdg.MGetCustomersFor(aData.FDataId.FFrom.FUuid, aData.FDataId.FProtocol,
			_data.ptr_const(), _data.size()) :
	//simplified parser for raw protocol
	_rdg.MGetCustomersFor(aData.FDataId.FFrom.FUuid,
			aData.FDataId.FRawProtocolNumber);
}
void CRoutingService::MHandleFrom(user_data_t const* aP, descriptor_t aFrom)
{
	VLOG(2) << "New  " << aP->FDataId << " From: " << aFrom;
	DLOG_IF(ERROR,!CDescriptors::sMGetInstance().MIsInfo(aFrom))<<aFrom<<" has been closed already. The msg is ignored...";

	uuids_t _to;
	if (aP->FDataId.FUUIDTo.MIs())
		_to = aP->FDataId.FUUIDTo.MGetConst(); //todo ignoring flags. Some program has to intercept packet. For example, Registrators
	/*else if (!aP->FDataId.FDestName.empty())
	 {
	 customers_names_t::const_iterator _it = aP->FDestName.begin();
	 r_route_access _access = FRouteData.MGetRAccess();
	 for (; _it != aP->FDestName.begin(); ++_it)
	 {
	 uuids_t const _var = _access->FRequiredDG.MGetClientsFor(*_it);
	 _to.insert(_to.end(), _var.begin(), _var.end());
	 }
	 }*/
	else
	{
		CRequiredDG::req_uuids_t const _req_uuids(MGetCustomersFor(*aP));

		for (CRequiredDG::req_uuids_t::const_iterator _kt = _req_uuids.begin();
				_kt != _req_uuids.end(); ++_kt)
			_to.push_back(_kt->first);

	}
	LOG_IF(INFO,_to.empty()) << "There are not any uuids for "
										<< aP->FDataId.FFrom
										<< ". Msg don't send. Ignoring...";
	if (!_to.empty())
	{
		VLOG(2) << "Try send";
		fail_send_array_t _non_sent;
		MSendTo(aP, routing_t(get_my_id().FId, _to), _non_sent, aFrom); //get uuid for
		if (!_non_sent.empty())
		{
			fail_send_array_t::const_iterator _it = _non_sent.begin(), _it_end(
					_non_sent.end());
			for (; _it != _it_end; ++_it)
				MNoteFailSend(*_it);
		}
	}
}

void CRoutingService::MGetOutputDescriptors(const routing_t& aSendTo,
		output_decriptors_for_t& _descr, uuids_t& _non_sent) const
{
	{
		for (uuids_t::const_iterator _it = aSendTo.begin();
				_it != aSendTo.end(); ++_it)
		{
			descriptor_t _d = MNextDestinationNode(*_it);
			if (CDescriptors::sMIsValid(_d))
			{
				//todo если _d я вляется получаетелем, что делать?
				//loop back protection
				//				if (_d != aFrom
				//						|| CDescriptors::sMIsValid(
				//								CDescriptors::sMGetInstance().MGet(*_it)))
				output_decriptors_for_t::iterator _jt = _descr.find(_d);
				if (_jt == _descr.end())
				{
					_jt =
							_descr.insert(
									output_decriptors_for_t::value_type(_d,
											aSendTo)).first;
					_jt->second.clear();
				}
				_jt->second.push_back(*_it);
			}
			else
				_non_sent.push_back(*_it);
		}
	}
}

inline void CRoutingService::MSendTo(user_data_t const* aP,
		routing_t const& aTo, fail_send_array_t & _non_sent, descriptor_t aFrom)
{
	CHECK(!aTo.empty());
	VLOG(2) << "Send  packet #" << aP->FDataId.FPacketNumber << " from "
						<< aP->FDataId.FFrom << " to " << aTo;
	user_data_t _data(*aP);
//	if (_data.FDataId.FFrom.FName.empty())//fixme remove add only if sent to customer
//	{
//		VLOG(2) << "New name ";
//		std::pair<descriptor_info_t, bool> _info =
//				CDescriptors::sMGetInstance().MGet(aFrom);
//		if (_info.second)
//			_data.FDataId.FFrom = _info.first.FProgramm.FId;
//		else
//			_data.FDataId.FFrom.FName = "Unknown";
//		VLOG(2) << "New name is " << _data.FDataId.FFrom;
//	}

	output_decriptors_for_t _descr;

	uuids_t _fail;
	MGetOutputDescriptors(aTo, _descr, _fail);
	if (!_fail.empty())
	{
		fail_send_t _sent(aP->FDataId);
		_sent.FUUIDTo = _fail;
		_non_sent.push_back(_sent);
	}

	LOG_IF(ERROR,_descr.empty()) << "There are not descriptors. WTF?";

	VLOG_IF(1,_descr.size()<aTo.size())
												<< "It has been reduced the number of packets for "
												<< (_descr.size()) << " from "
												<< aTo.size();

	if (!_descr.empty())
	{
		CKernelIo::sMGetInstance().MSendTo(_descr, _data, _non_sent);
//		for (output_decriptors_for_t::const_iterator _it =
//				_descr.begin(); _it != _descr.end(); ++_it)
//		{
//			_data.FDataId.FUUIDTo = _it->second;		//by historical reason
//
//			if (CKernelIo::sMGetInstance().MSendTo(_it->first, _data,
//					_it->second))
//			{
//				LOG(INFO)<< "Sent  packet #"<<aP->FDataId.FPacketNumber<<" from " << aP->FDataId.FFrom << " to " << _data.FDataId.FUUIDTo.MGet();
//			}
//			else
//			{
//				//LOG(ERROR)<<"Cannot send  packet #"<<aP->FDataId.FPacketNumber<<" from " << aP->FDataId.FFrom << " to " << _data.FDataId.FUUIDTo.MGet();
//				_non_sent.insert(_non_sent.end(),_it->second.begin(),_it->second.end());
//				CHECK(!_non_sent.empty());
//			}
//		}
	}
}
descriptor_t CRoutingService::MNextDestinationNode(
		NSHARE::uuid_t const& aTo) const
{
	//find route
	descriptor_t _d = CDescriptors::sMGetInstance().MGet(aTo); //fast check for customers
	if (!CDescriptors::sMIsValid(_d))
	{
		r_route_access _access = FRouteData.MGetRAccess();
//		bool _is = CDescriptors::sMGetInstance().MIs(_d);
//		if (!_is)
//		{
//			LOG(ERROR)<<"The descriptor has been closed. Ignoring...";
//			return CDescriptors::INVALID;
//		}
		CRouteGraph::path_t _path = CInfoService::sMGetInstance().MShortestPath(
				aTo);
		if (_path.empty())
		{
			LOG(ERROR)<< "There is not path for " << aTo;
			return CDescriptors::INVALID;
		}
		//LOG_IF(ERROR,_path.size()==1) << "There is not path for " << aTo;
//		if (_path.size() == 1)
//			return CDescriptors::INVALID;
		VLOG(2) << "Route for " << aTo << ":" << _path;
		_d = CDescriptors::sMGetInstance().MGet(_path.front());
		LOG_IF(ERROR,!CDescriptors::sMIsValid(_d))<<"Invalid descriptor  for "<<_path.front();

	}
	return _d;
}

inline void CRoutingService::MInit()
{
	{
		program_id_t _my = get_my_id();
		w_route_access _access = FRouteData.MGetWAccess();
		_access->FRequiredDG.MAddClient(_my.FId);
	}
	{
		callback_data_t _cb(sMHandleDemandId, this);
		CDataObject::value_t _val(demands_id_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}
	{
		callback_data_t _cb(sMHandleUserDataId, this);
		CDataObject::value_t _val(user_data_id_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}

	{
		callback_data_t _cb(sMHandleDemands, this);
		CDataObject::value_t _val(demand_dgs_for_by_id_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}
//	{
//		callback_data_t _cb(sMHandleOpenId, this);
//		CDataObject::value_t _val(open_descriptor::NAME, _cb);
//		CDataObject::sMGetInstance() += _val;
//	}
//	{
//		callback_data_t _cb(sMHandleCloseId, this);
//		CDataObject::value_t _val(close_descriptor::NAME, _cb);
//		CDataObject::sMGetInstance() += _val;
//	}
	{
		callback_data_t _cb(sMHandleDiff, this);
		CDataObject::value_t _val(kernel_infos_diff_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}

}
NSHARE::CConfig CRoutingService::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	{
		r_route_access _access = FRouteData.MGetRAccess();
		_route_t const & _route = _access.MGet();
		//_conf.MAdd(_route.FGraph.MSerialize());
		_conf.MAdd(_route.FRequiredDG.MSerialize());
	}
	return _conf;
}
void CRoutingService::MInformNewReceiver(demand_dgs_for_t & aNewRecveiver)
{
	VLOG(2) << "Inform";
	demand_dgs_for_t::iterator _it = aNewRecveiver.begin(), _it_end(
			aNewRecveiver.end());
	for (; _it != _it_end; ++_it)
	{
		descriptor_t const _to = CDescriptors::sMGetInstance().MGet(
				_it->first.FUuid);
		if (CDescriptors::sMIsValid(_to))
		{
			demand_dgs_for_t _copy;
			_copy[_it->first].swap(_it->second);
			DCHECK_EQ(
					CDescriptors::sMGetInstance().MGet(_to).first.FProgramm.FType,
					E_CONSUMER);
			CKernelIo::sMGetInstance().MSendTo(_to, _copy);
		}
	}
}
} /* namespace NUDT */
