// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CRoutingService.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 16.02.2016
 *      Author:  https://github.com/CrazyLauren
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
			routing_t _to(get_my_id().FId, _inform_array);
			routing_t _not_sent(get_my_id().FId,uuids_t());

			MSendTo(_to, _not_sent, _demands);

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
	VLOG(2) << "New demands  " << *aP << " From: " << aFrom;
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

		routing_t _to(get_my_id().FId, _inform_array);
		routing_t _not_sent(get_my_id().FId,uuids_t());
		MSendTo(_to, _not_sent, _c);

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

				_repeates.erase(_jt++);
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

	///< At least the error has to send to registrar and sender
	routing_t _f_to;
	_f_to.insert(_f_to.end(),_sent.FRegistrators.begin(),_sent.FRegistrators.end());
	_f_to.push_back(_sent.FRouting.FFrom.FUuid);
	_f_to.FFrom = get_my_id().FId;

	///< In addition the error has to be send:

	if(_sent.FError.MGetFlag(E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE)//
			||_sent.FError.MGetFlag(E_CANNOT_SWAP_ENDIAN)//
			||_sent.FError.MGetFlag(E_PARSER_IS_NOT_EXIST)//
			||_sent.FError.MGetFlag(E_PACKET_LOST)//
			|| true//All error !!!!!!!!!!!!!!!!!!!!!!!!
			)
		_f_to.insert(_f_to.end(),_sent.FRouting.begin(),_sent.FRouting.end());//receivers


	routing_t _not_sent(get_my_id().FId,uuids_t());
	MSendTo(_f_to, _not_sent,_sent);

	LOG_IF(ERROR,!_not_sent.empty()) << "Cannot sent fails to "
												<< _not_sent;
	LOG_IF(INFO,_not_sent.empty()) << "Sent fails to "
											<< _f_to;
}
void CRoutingService::MFillMsgReceivers(user_datas_t & aFrom, user_datas_t& aTo,
		fail_send_array_t&aFail)
{
	r_route_access _access = FRouteData.MGetRAccess();
	CRequiredDG const& _rdg = _access->FRequiredDG;
	_rdg.MFillMsgReceivers(&aFrom, &aTo, &aFail);
}
void CRoutingService::MFillMsgHandlersFor(user_datas_t & aFrom,
		user_datas_t& aTo, fail_send_array_t & aError)
{
	r_route_access _access = FRouteData.MGetRAccess();
	CRequiredDG const& _rdg = _access->FRequiredDG;
	_rdg.MFillMsgHandlersFor(aFrom, aTo, aError);

}
<<<<<<< HEAD
=======
/*!\brief Devides the data into to group
 * the data which have routing info
 * the data which havn't routing info
 *
 *\parma [in] aData - the data, after deviding will empty
 *\parma [out] aNotRouted - list of not routed data
 *\parma [out] aRouted - list of routed data
 */
void CRoutingService::MClassifyMessages(user_datas_t* const aData,
		user_datas_t* const aNotRouted, user_datas_t* const aRouted) const
{
	DCHECK_NOTNULL(aData);
	DCHECK_NOTNULL(aNotRouted);
	DCHECK_NOTNULL(aRouted);

	user_datas_t& _recv_data(*aData);
	user_datas_t& _to_not_routed(*aNotRouted);
	user_datas_t& _to_routed(*aRouted);
>>>>>>> f3da2cc... see changelog.txt

void CRoutingService::MClassifyMessages(routing_user_data_t& aData,
		user_datas_t& aNotRouted, user_datas_t& aRouted) const
{
	//divide into two group from consumer and other.
	//the sequence is not broken
	for (user_datas_t& _recv_data = aData.FData; !_recv_data.empty();)
	{
		if (_recv_data.front().FDataId.FRouting.empty())
		{
			VLOG(3) << " It's from consumer " << _recv_data.front().FDataId;
			aNotRouted.splice(aNotRouted.end(), _recv_data,
					_recv_data.begin());
		}
		else
		{
			VLOG(3) << " It's from kernel " << _recv_data.front().FDataId;
			aRouted.splice(aRouted.end(), _recv_data, _recv_data.begin());
		}
	}
}

void CRoutingService::MSwapEndianIfNeed(user_datas_t & aFrom,user_datas_t& aTo,
		fail_send_array_t& _errors_list) const
{
	user_datas_t::iterator _rit = aFrom.begin();
	for (; _rit != aFrom.end();)
	{
		user_data_t& _data(*_rit);
		_data.FData=_data.FData.MCreateCopy();

		DCHECK_NE(_data.FDataId.FEndian, NSHARE::E_SHARE_ENDIAN);

		unsigned const _error=CRequiredDG::sMSwapEndian(_data);

		if(_error!=E_NO_ERROR)
		{
			fail_send_t _sent(_data.FDataId);
			_sent.MSetError(_error);
			_errors_list.push_back(_sent);

			 ++_rit;
		}
		else
			aTo.splice(aTo.end(),aFrom,_rit++);//only post increment!!!
	}
}
<<<<<<< HEAD
=======
/*!\brief Distributes messages to output descriptors
 * on basis of theirs routes
 *
 *\param [in] aWhat the data, after distribution will be empty
 *\param [out] aHasNotRoute the data which has not route
 *\param [out] aSendToDescriptors the result
 *\param [out] aErrorList Where the error has to be putted
 */
void CRoutingService::MDistributeMsgByDescriptors(user_datas_t* const aWhat,
		user_datas_t* const aHasNotRoute,
		output_user_data_t* const aSendToDescriptors,fail_send_array_t* const aErrorList) const
{
	user_datas_t& _from(*aWhat);
	user_datas_t& _to(*aHasNotRoute);
	output_user_data_t& _send_to_desc(*aSendToDescriptors);
	fail_send_array_t& _errors_list(*aErrorList);
>>>>>>> f3da2cc... see changelog.txt

void CRoutingService::MDistributeMsgByDescriptors(user_datas_t& aFrom,
		user_datas_t& aTo,
		output_user_data_t& aSendToDescriptors,fail_send_array_t& _errors_list) const
{
	user_datas_t::iterator _it = aFrom.begin();
	for (; _it != aFrom.end();)
	{
		user_data_t& _data = *_it;
		unsigned _error = E_NO_ERROR;

		if (!_data.FDataId.FRouting.empty())
		{
			VLOG(2) << "Send  packet #" << _data.FDataId.FPacketNumber
								<< " from " << _data.FDataId.FRouting.FFrom
								<< " to " << _data.FDataId.FRouting;

			output_decriptors_for_t _descr;
			routing_t _has_not_route;

			MGetOutputDescriptors(_data.FDataId.FRouting, _has_not_route, _descr);

			if (!_descr.empty())
			{
				if (!_has_not_route.empty())//it's not fatal error
				{
					aTo.push_back(_data);
					aTo.back().FDataId.FRouting.swap(_has_not_route);

					fail_send_t _sent(aTo.back().FDataId);
					_sent.MSetError(E_NO_ROUTE);
					_errors_list.push_back(_sent);
				}

				VLOG_IF(1,_descr.size()<_data.FDataId.FRouting.size())
																				<< "It has been reduced the number of packets for "
																				<< (_descr.size())
																				<< " from "
																				<< _data.FDataId.FRouting.size();
				output_user_data_t _ouput;
				MRoutingMsgToDescriptor(_data, _descr, &_ouput);

				if (!_ouput.empty())
				{
					MSortedSplice(_ouput, aSendToDescriptors);
				}
				else
				{
					LOG(ERROR)<< " WTF?";
					_error = E_NO_ROUTE;
				}
			}
			else
			{
				LOG(ERROR)<< "There are not descriptors. WTF?";
				_error = E_NO_ROUTE;
			}

		}
		else
		{
			_error = E_UNKNOWN_ERROR;
			LOG(DFATAL) << "There are no any receivers for "<< _data.FDataId<< ". Msg don't send. Ignoring...";
		}

		if (_error != E_NO_ERROR)
		{
			fail_send_t _sent(_data.FDataId);
			_sent.MSetError(_error);
			_errors_list.push_back(_sent);

			++_it;
		}
		else
			aTo.splice(aTo.end(), aFrom, _it++);//only post increment!!!
	}
}
inline void CRoutingService::MRoutingMsgToDescriptor(user_data_t const & aMsg,
		output_decriptors_for_t const& aDesc,
		output_user_data_t * const aTo) const
{
	output_user_data_t& _ouput=*aTo;
	output_decriptors_for_t::const_iterator _jt = aDesc.begin(),
			_jt_end = aDesc.end();
	for (; _jt != _jt_end; ++_jt)
	{
		_ouput.push_back(routing_user_data_t());
		_ouput.back().FDesc=_jt->first;
		_ouput.back().FData.push_back(aMsg);
		_ouput.back().FData.back().FDataId.FRouting = _jt->second;
	}
}
void CRoutingService::MSortedSplice(output_user_data_t& aWhat, output_user_data_t& aTo) const
{
	output_user_data_t::iterator _from=aWhat.begin();
	output_user_data_t::iterator _to=aTo.begin();
	while (_from != aWhat.end() && _to != aTo.end())
	{
		if (_to->FDesc < _from->FDesc)
		{
			++_to;
		}
		else if(_to->FDesc==_from->FDesc)
		{
			_to->FData.splice(_to->FData.end(),_from->FData);
			_from=aWhat.erase(_from);
			++_to;
		}else
		{
			aTo.splice(_to,aWhat,_from++);//only post increment!!!
		}
	}
	if(_from!=aWhat.end())
		aTo.splice(aTo.end(),aWhat);

	CHECK(aWhat.empty());
}
<<<<<<< HEAD
void CRoutingService::MExtractMsgThatHasToBeSwaped(user_datas_t & aFrom,user_datas_t & aTo) const
=======
/*!\brief Moves the data that has to be swapped
 *
 *\param [in] aFrom - moved from
 *\param [out] aTo - moved to
 *
 */
void CRoutingService::MExtractMsgThatHasToBeSwaped(user_datas_t *const aFrom,user_datas_t *const aTo) const
>>>>>>> f3da2cc... see changelog.txt
{
	user_datas_t::iterator _rit = aFrom.begin();
	for (; _rit != aFrom.end();)
	{
		user_data_t& _data = *_rit;

		routing_t& _routing_to = _data.FDataId.FRouting;
		routing_t _has_to_swaped_for;

		if (_data.FDataId.FEndian != NSHARE::E_SHARE_ENDIAN)
		{
			DVLOG(2)<<"The data endian is not valid ";

			routing_t::iterator _it = _routing_to.begin();
			for (; _it != _routing_to.end();)
			{
				if (CDescriptors::sMGetInstance().MIs(*_it))
				{
					_has_to_swaped_for.push_back(*_it);
					_it=_routing_to.erase(_it);
				}
				else
					++_it;
			}
		}

		if(!_has_to_swaped_for.empty())
		{
			VLOG(1)<<"The byte endian for "<<_has_to_swaped_for<< " has to swapped." ;

			if(_routing_to.empty())
				aTo.splice(aTo.end(),aFrom,_rit++); //only post increment!!!
			else
			{
				aTo.push_back(_data);
				 ++_rit;
			}

			aTo.back().FDataId.FRouting.swap(_has_to_swaped_for);
		}else
			 ++_rit;
	}
}

<<<<<<< HEAD
void CRoutingService::MSendMessages(user_datas_t& aFrom,
		user_datas_t& _has_not_route, fail_send_array_t& _errors_list)
=======
/*!\brief Sends the data
 *
 *\param [in] aFrom - the data which has to be sent
 *\param [out] aHasNotRoute - the data which isn't sent
 *\param [out] aErrorList - Where puts the errors
 *
 */
void CRoutingService::MSendMessages(user_datas_t*const aFrom,
		user_datas_t*const aHasNotRoute, fail_send_array_t*const aErrorList)
>>>>>>> f3da2cc... see changelog.txt
{
	output_user_data_t _send_to_descriptors;
	MDistributeMsgByDescriptors(aFrom, _has_not_route,
			_send_to_descriptors, _errors_list);
	//when for all send data
	if (!_send_to_descriptors.empty())
	{
		CKernelIo::sMGetInstance().MSendTo(_send_to_descriptors, _errors_list,
				_has_not_route);
	}
	DCHECK(aFrom.empty());
}

<<<<<<< HEAD
=======
/*!\brief Filling routing info  and sending the received data
 *
 *
 */
>>>>>>> f3da2cc... see changelog.txt
void CRoutingService::MHandleFrom(routing_user_data_t& aData)
{
	DCHECK(!aData.FData.empty());
	VLOG(2) << "New  From: " << aData.FDesc;
	DLOG_IF(ERROR,!CDescriptors::sMGetInstance().MIsInfo(aData.FDesc))<<aData.FDesc<<" has been closed already. The msg is ignored...";

	fail_send_array_t _errors_list;
	user_datas_t _has_not_route;
	user_datas_t _has_route;
	user_datas_t _has_to_swaped;

	MClassifyMessages(aData, _has_not_route, _has_route);

	if (!_has_not_route.empty())
			MFillMsgReceivers(_has_not_route, _has_route, _errors_list);

	MExtractMsgThatHasToBeSwaped(_has_route,_has_to_swaped);

	if (!_has_route.empty())
		MSendMessages(_has_route, _has_not_route, _errors_list);

	if (!_has_to_swaped.empty())
	{
		MSwapEndianIfNeed(_has_to_swaped,_has_route,_errors_list);
		MSendMessages(_has_route, _has_not_route, _errors_list);
	}

	if (!_errors_list.empty())
	{
		MNoteFailSend(_errors_list);
	}

	DCHECK(aData.FData.empty());
	if (!_has_not_route.empty())
	{
		LOG(WARNING)<<"Some packets not handled, maybe the other callbacks is handled.";
		aData.FData.swap(_has_not_route);
	}
}
<<<<<<< HEAD
=======
/*!\brief Return whither the data has to be sent that
 * it will be delivery to uuid
 *
 *\param [in] aTo - whither the data will sent, after return
 * it contains only uuids that has output decriptors
 *\param [out] aNotRoute - whither the data cannot be sent as no route
 *\param [out] aOutput - result
 */
void CRoutingService::MGetOutputDescriptors(routing_t*const aTo, routing_t*const aNotRoute,
		output_decriptors_for_t *const aOutput) const
{
	routing_t& _to(*aTo);
	routing_t& _not_route(*aNotRoute);
	output_decriptors_for_t& _descr(*aOutput);
>>>>>>> f3da2cc... see changelog.txt

void CRoutingService::MGetOutputDescriptors(routing_t& aFrom, routing_t& aTo,
		output_decriptors_for_t& _descr) const
{
	for (uuids_t::iterator _it = aFrom.begin(); _it != aFrom.end();
			)
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
								routing_t(aFrom.FFrom, uuids_t()))).first;
			}
			_jt->second.push_back(*_it);

			++_it;
		}
		else
		{
			LOG(ERROR)<<"No route to "<<(*_it);
			aTo.push_back(*_it);

			_it=aFrom.erase(_it);
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
<<<<<<< HEAD
=======
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
/*!\brief Callback for data handler
 *
 */
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
>>>>>>> f3da2cc... see changelog.txt
} /* namespace NUDT */
