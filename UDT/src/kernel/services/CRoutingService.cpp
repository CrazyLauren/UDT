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
#include <share_socket.h>
#include <internel_protocol.h>

#include <core/kernel_type.h>
#include <core/IState.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <io/CKernelIo.h>

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
	CHECK_EQ(routing_user_data_t::NAME, WHAT->FType);
	CHECK_NOTNULL(WHAT->FPointToData);
	routing_user_data_t & _p =
			*reinterpret_cast<routing_user_data_t*>(WHAT->FPointToData);

	_this->MHandleFrom(_p);
	return 0;
}

namespace
{
struct _compare_fail
{
	bool operator()(const fail_send_t& a, const fail_send_t& b) const
	{
		return a.FPacketNumber == b.FPacketNumber
				&& a.FRouting.FFrom.FUuid == b.FRouting.FFrom.FUuid;
	}
};
}
void CRoutingService::MNoteFailSend(const fail_send_array_t& aFail)
{

	typedef std::set<fail_send_t, _compare_fail> _packet_repeated;
	//for instance. If splitted packet has been lost the same error is occurred for all packets

	_packet_repeated _repeates;
	{
		fail_send_array_t::const_iterator _it = aFail.begin(), _it_end =
				aFail.end();
		for (; _it != _it_end; ++_it)
		{
			_packet_repeated::iterator _jt = _repeates.find(*_it);
			if (_jt == _repeates.end())
			{
				_jt = _repeates.insert(*_it).first;
			}
			else
			{
				fail_send_t _new(*_jt);
				_new.MSetError(_it->FError.MGetMask());

				std::set<NSHARE::uuid_t> _unique;
				uuids_t _new_uuids;
				{
					uuids_t::const_iterator _ut = _new.FRouting.begin(), _ut_end(
							_new.FRouting.end());
					for (; _ut != _ut_end; ++_ut)
					{
						if(_unique.insert(*_ut).second)
							_new_uuids.push_back(*_ut);
					}
				}
				{
					uuids_t::const_iterator _ut = _it->FRouting.begin(),
							_ut_end(_it->FRouting.end());
					for (; _ut != _ut_end; ++_ut)
					{
						if (_unique.insert(*_ut).second)
							_new_uuids.push_back(*_ut);
					}
				}
				_new.FRouting.swap(_new_uuids);

				_repeates.erase(_jt);
				_repeates.insert(_jt, _new);
			}
		}
	}
	{
		_packet_repeated::const_iterator _it = _repeates.begin(), _it_end =
				_repeates.end();
		for (; _it != _it_end; ++_it)
		{
			MNoteFailSend(*_it);
		}
	}
}
void CRoutingService::MNoteFailSend(const fail_send_t& _sent)
{
	LOG(ERROR)<<"Cannot send  packet #"<<_sent.FPacketNumber<<" from " << _sent.FRouting.FFrom << " to " << _sent.FRouting;
	routing_t _f_to;
	_f_to.insert(_f_to.end(),_sent.FRegistrators.begin(),_sent.FRegistrators.end());
	_f_to.push_back(_sent.FRouting.FFrom.FUuid);
	_f_to.FFrom = get_my_id().FId;
	MSendTo(_f_to, _sent);
}
void CRoutingService::MFillMsgReceivers(user_datas_t & aFrom, user_datas_t& aTo,
		fail_send_array_t&aFail)
{
	r_route_access _access = FRouteData.MGetRAccess();
	CRequiredDG const& _rdg = _access->FRequiredDG;
	_rdg.MFillMsgReceivers(aFrom, aTo, aFail);
}
void CRoutingService::MFillMsgHandlersFor(user_datas_t & aFrom,
		user_datas_t& aTo, fail_send_array_t & aError)
{
	r_route_access _access = FRouteData.MGetRAccess();
	CRequiredDG const& _rdg = _access->FRequiredDG;
	_rdg.MFillMsgHandlersFor(aFrom, aTo, aError);

}
void CRoutingService::MHandleFrom(routing_user_data_t& aData)
{
	CHECK(!aData.FData.empty());
	VLOG(2) << "New  From: " << aData.FDesc;
	DLOG_IF(ERROR,!CDescriptors::sMGetInstance().MIsInfo(aData.FDesc))<<aData.FDesc<<" has been closed already. The msg is ignored...";

	fail_send_array_t _non_sent;

	user_datas_t _routed;
	user_datas_t _not_routed;
	{
		//divide into two group from consumer and other.
		//the sequence is not broken
		for (user_datas_t& _recv_data = aData.FData; !_recv_data.empty();)
		{
			if (_recv_data.front().FDataId.FRouting.empty())	//from consumer
			{
				VLOG(3) << " It's from consumer "
									<< _recv_data.front().FDataId;
				_not_routed.splice(_not_routed.end(), _recv_data,
						_recv_data.begin());
			}
			else
			{
				VLOG(3) << " It's from kernel "
									<< _recv_data.front().FDataId;
				_routed.splice(_routed.end(), _recv_data, _recv_data.begin());
			}
		}
		if (!_not_routed.empty())
			MFillMsgReceivers(_not_routed, _routed, _non_sent);
	}

	output_user_data_t _route_by_desc;

	user_datas_t::iterator _it = _routed.begin(), _it_end = _routed.end();

	for (; _it != _it_end; ++_it)
	{
		user_data_t &_data = *_it;
		LOG_IF(INFO,_data.FDataId.FRouting.empty())
															<< "There are no any receivers for "
															<< _data.FDataId
															<< ". Msg don't send. Ignoring...";

		if (!_data.FDataId.FRouting.empty())
		{
			VLOG(2) << "Send  packet #" << _data.FDataId.FPacketNumber
								<< " from " << _data.FDataId.FRouting.FFrom
								<< " to " << _data.FDataId.FRouting;

			//converting uuid to descriptor

			output_decriptors_for_t _descr;
			uuids_t _fail;
			MGetOutputDescriptors(_data.FDataId.FRouting, _descr, _fail);

			if (!_fail.empty())
			{
				fail_send_t _sent(_data.FDataId);
				_sent.FRouting.swap(_fail);
				_sent.MSetError(E_NO_ROUTE);
				_non_sent.push_back(_sent);
			}

			LOG_IF(ERROR,_descr.empty()) << "There are not descriptors. WTF?";

			VLOG_IF(1,_descr.size()<_data.FDataId.FRouting.size())
																			<< "It has been reduced the number of packets for "
																			<< (_descr.size())
																			<< " from "
																			<< _data.FDataId.FRouting.size();
			output_decriptors_for_t::const_iterator _jt = _descr.begin(),
					_jt_end = _descr.end();
			for (; _jt != _jt_end; ++_jt)
			{
				_data.FDataId.FRouting = _jt->second;

				output_user_data_t::iterator _kt = _route_by_desc.begin(),
						_kt_end = _route_by_desc.end();

				for (; _kt != _kt_end && _kt->FDesc != _jt->first; ++_kt)
					//todo optimize
					;

				if (_kt == _kt_end)
				{
					_route_by_desc.push_back(routing_user_data_t());
					_kt = --_route_by_desc.end();
					_kt->FDesc = _jt->first;
				}
				_kt->FData.push_back(_data);
			}

		}

	}

	//when for all send data
	if (!_route_by_desc.empty())
	{
		CKernelIo::sMGetInstance().MSendTo(_route_by_desc, _non_sent,
				_not_routed);
	}

	if (!_non_sent.empty())
	{
		MNoteFailSend(_non_sent);
	}

	if (!_not_routed.empty())
	{
		LOG(WARNING)<<"Some packets not handled, maybe the other callbacks is handled.";
		aData.FData.splice(aData.FData.end(),_not_routed);
	}
}

void CRoutingService::MGetOutputDescriptors(const routing_t& aSendTo,
		output_decriptors_for_t& _descr, uuids_t& _non_sent) const
{
	{
		for (uuids_t::const_iterator _it = aSendTo.begin();
				_it != aSendTo.end(); ++_it)
		{
			std::vector<descriptor_t> const _d =
					CInfoService::sMGetInstance().MNextDestinations(*_it);
			//todo several destination
			if (!_d.empty() && CDescriptors::sMIsValid(_d.front()))
			{
				output_decriptors_for_t::iterator _jt = _descr.find(_d.front());
				if (_jt == _descr.end())
				{
					_jt = _descr.insert(
							output_decriptors_for_t::value_type(_d.front(),
									routing_t(aSendTo.FFrom, uuids_t()))).first;
				}
				_jt->second.push_back(*_it);
			}
			else
				_non_sent.push_back(*_it);
		}
	}
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
		CDataObject::value_t _val(routing_user_data_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}

	{
		callback_data_t _cb(sMHandleDemands, this);
		CDataObject::value_t _val(demand_dgs_for_by_id_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}

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
