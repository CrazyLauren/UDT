/*
 * CRequiredDG.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 18.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <core/kernel_type.h>
#include <CParserFactory.h>
#include "CRequiredDG.h"
namespace NUDT
{
const NSHARE::CText CRequiredDG::NAME = "demands";
//todo using hash algorithm
CRequiredDG::CRequiredDG()
{

}

CRequiredDG::~CRequiredDG()
{
}

std::pair<demand_dgs_for_t, demand_dgs_for_t> CRequiredDG::MSetDemandsDGFor(
		id_t const& aFor, demand_dgs_t const& aReqDgs)
{
	VLOG(2) << "Add demands for " << aFor;
	std::pair<demand_dgs_for_t, demand_dgs_for_t> _rval;
	if (aReqDgs.empty())
	{
		VLOG(2) << "no demands";
		MRemoveReceiversFor(aFor.FUuid, _rval.second);
		return _rval;
	}
	demand_dgs_t _added;
	demand_dgs_t _removed;
	MGetDiffDemandsDG(aFor, aReqDgs, _removed, _added);
	FDGs[aFor] = aReqDgs;

	if (!_removed.empty())
		MRemoveDemandsFor(aFor.FUuid, _removed, _rval.second);
	if (!_added.empty())
		MAddDemandsFor(aFor.FUuid, _added, _rval.first);
	return _rval;
}

void CRequiredDG::MSendPacketFromTo(NSHARE::uuid_t const& aFrom,
		NSHARE::uuid_t const& aTo, demand_dg_t const& aWhat,
		demand_dgs_for_t* aNew)
{
	VLOG(2) << "Send " << aWhat << " from " << aFrom << " to " << aTo;
	bool const _is_real_customer = aNew != NULL;

	protocol_of_uuid_t::iterator _customer_it = FWhatIsSendingBy.find(aFrom);

	VLOG_IF(2,_customer_it==FWhatIsSendingBy.end()) << "The consumer " << aFrom
															<< " still was not sent.";

	if (_customer_it == FWhatIsSendingBy.end())
		_customer_it = FWhatIsSendingBy.insert(_customer_it,
				std::make_pair(aFrom, protocols_t()));

	CHECK(_customer_it != FWhatIsSendingBy.end());

	protocols_t& _proto = _customer_it->second;

	VLOG(2) << "Add new 'receiver' for " << _customer_it->first;
	protocols_t::iterator _prot_it = _proto.find(aWhat.FProtocol);

	VLOG_IF(2,_prot_it==_proto.end()) << "Packets from '" << aFrom << "' by '"
												<< aWhat.FProtocol
												<< "' protocol still were not sent.";

	if (_prot_it == _proto.end())
		_prot_it = _proto.insert(_prot_it,
				std::make_pair(aWhat.FProtocol, uuids_of_expecting_dg_t()));

	CHECK(_prot_it != _proto.end());

	//looking for header
	uuids_of_expecting_dg_t& _uuids = _prot_it->second;
	uuids_of_expecting_dg_t::iterator _exp_it = _uuids.find(aWhat.FWhat);

	VLOG_IF(2,_exp_it==_uuids.end())
											<< "The Packet has not been received of yet.";

	if (_exp_it == _uuids.end())
		_exp_it = _uuids.insert(_exp_it,
				std::make_pair(aWhat.FWhat, uuids_of_receiver_t()));

	CHECK(_exp_it != _uuids.end());

	uuids_of_receiver_t& _r_uuid = _exp_it->second;

	CHECK_GE(_r_uuid.FNumberOfRealReceivers, 0);

	if (_is_real_customer)
		++_r_uuid.FNumberOfRealReceivers;
	else if (_r_uuid.FNumberOfRealReceivers == 0)
	{
		LOG(INFO)<<"Cannot register sending msg "<<aWhat<< " from " << aFrom
		<< " to "<< aTo<<" as no 'real' consumer";
		return;
	}

	msg_handlers_t& _handlers = _r_uuid[aTo];
	_handlers.push_back(aWhat.FHandler);

	if (aWhat.FWhat.FVersion.MIsExist())
	{
		LOG_IF(DFATAL,(_handlers.FVersion.MIsExist()&& _handlers.FVersion!=aWhat.FWhat.FVersion))
																											<< "Different version for msg "
																											<< aWhat.FWhat
																											<< " handler= "
																											<< aWhat.FHandler
																											<< " 'Old' version: "
																											<< _handlers.FVersion;
		_handlers.FVersion = aWhat.FWhat.FVersion;
	}

	bool const _is_new = _handlers.FNumberOfRealHandlers == 0;

	LOG_IF(INFO,_is_new) << "Now The packet:" << aWhat << " from " << aFrom
									<< " is received of by " << aTo;
	LOG_IF(INFO,!_is_new) << "The packet has been received twice of by "
									<< aTo;

	if (_is_real_customer)
		++_handlers.FNumberOfRealHandlers;

	if (_is_new && _is_real_customer)
	{
		demand_dg_t _new(aWhat);
		_new.FUUIDFrom.MSet(aTo);
		(*aNew)[id_t(aFrom)].push_back(_new);
	}
}
void CRequiredDG::MUnSendPacketFromTo(NSHARE::uuid_t const& aFrom,
		NSHARE::uuid_t const& aTo, demand_dg_t const& aWhat,
		demand_dgs_for_t* aOld)
{
	VLOG(2) << "Unsent " << aWhat << " from " << aFrom << " to " << aTo;
	bool const _is_real_customer = aOld != NULL;

	protocol_of_uuid_t::iterator _customer_it = FWhatIsSendingBy.find(aFrom);

//	DCHECK(_customer_it != FWhatIsSendingBy.end());

	if (_customer_it == FWhatIsSendingBy.end()) //if send some DG to uuid
		return;
	protocols_t& _proto = _customer_it->second;

	VLOG(2) << "Remove 'receiver' for " << _customer_it->first;
	protocols_t::iterator _prot_it = _proto.find(aWhat.FProtocol);

	if (_prot_it == _customer_it->second.end()) //there is the protocol
		return;

	VLOG(2) << "The protocol is exit";
	//looking for header
	uuids_of_expecting_dg_t& _uuids = _prot_it->second;
	uuids_of_expecting_dg_t::iterator _exp_it = _uuids.find(aWhat.FWhat);

	if (_exp_it == _uuids.end())
		return;

	VLOG(3) << "Head is found ";
	uuids_of_receiver_t& _r_uuid = _exp_it->second;
	CHECK(_r_uuid.FNumberOfRealReceivers > 0 || _r_uuid.empty());

	//looking for handlers for  UUID
	uuids_of_receiver_t::iterator _list_it = _exp_it->second.find(aTo);
	if (_list_it == _exp_it->second.end())
		return;

	msg_handlers_t& _handlers = _list_it->second;

	LOG(INFO)<<"Now The packet:"<<_exp_it->first<<" from "<<_customer_it->first<<" is not received of by "<<aTo;
	if (_is_real_customer)
	{
		CHECK_GT(_handlers.FNumberOfRealHandlers, 0);

		//decrease number of "real" callbacks
		//if zero remove all
		--_handlers.FNumberOfRealHandlers;
		--_r_uuid.FNumberOfRealReceivers;

		demand_dg_t _tmp_dem(aWhat);
		_tmp_dem.FUUIDFrom.MSet(aTo);
		(*aOld)[id_t(_customer_it->first)].push_back(_tmp_dem);

		bool const _is_remove_all = _r_uuid.FNumberOfRealReceivers == 0;
		VLOG_IF(1,_is_remove_all)
											<< "Removing all 'receivers' as the latest 'real' consumer will removed";

		if (_is_remove_all)
			_r_uuid.clear();
		else
			_r_uuid.erase(_list_it);
	}
	else
		_r_uuid.erase(_list_it);

	//cleanup maps
	if (_r_uuid.empty())
		_uuids.erase(_exp_it);

	if (_uuids.empty())
		_proto.erase(_prot_it);

	if (_proto.empty())
		FWhatIsSendingBy.erase(_customer_it);

}
void CRequiredDG::MAddDemandsFor(NSHARE::uuid_t const& aFor,
		const demand_dgs_t& aAdded, demand_dgs_for_t& aNew)
{
	//update protocol_of_uuid_t
	//1) find MSGs which are required to aFor from existing UUID
	//2) add all demands to protocol_of_uuid_t
	demand_dgs_t::const_iterator _req_it = aAdded.begin();
	for (; _req_it != aAdded.end(); ++_req_it)
	{
		VLOG(2) << "Try received " << *_req_it;
		unique_uuids_t const _uuids = MGetUUIDFor(_req_it->FNameFrom);

		//VLOG(2) << " Founded uuids: " << _uuids;
		bool const _is_registator = (_req_it->FFlags
				& demand_dg_t::E_REGISTRATOR) != 0;

		unique_uuids_t::const_iterator _uuid_it = _uuids.begin();
		for (; _uuid_it != _uuids.end(); ++_uuid_it)
			MSendPacketFromTo(*_uuid_it, aFor, *_req_it,
					_is_registator ? NULL : &aNew);
	}
}
void CRequiredDG::MRemoveDemandsFor(NSHARE::uuid_t const& aTo,
		demand_dgs_t const& aFrom, demand_dgs_for_t & aOld)
{
	demand_dgs_t::const_iterator _jt = aFrom.begin(), _jt_end(aFrom.end());
	for (; _jt != _jt_end; ++_jt)
	{
		bool const _is_registator = (_jt->FFlags & demand_dg_t::E_REGISTRATOR)
				!= 0;

		unique_uuids_t const _uuids = MGetUUIDFor(_jt->FNameFrom);

		//VLOG(2) << " Founded uuids: " << _uuids;

		unique_uuids_t::const_iterator _uuid_it = _uuids.begin();
		for (; _uuid_it != _uuids.end(); ++_uuid_it)
		{
			MUnSendPacketFromTo(*_uuid_it, aTo, *_jt,
					_is_registator ? NULL : &aOld);
		}
	}
}
demand_dgs_for_t const& CRequiredDG::MGetDemands() const
{
	return FDGs;
}
bool CRequiredDG::MGetDiffDemandsDG(id_t const& aFor, demand_dgs_t const& aNew,
		demand_dgs_t& aRemoved, demand_dgs_t& aAdd)
{
	demand_dgs_for_t::iterator _it = FDGs.find(aFor);
	if (_it != FDGs.end())
	{
		aRemoved = _it->second;
		aAdd = aNew;

		//search old
		demand_dgs_t::reverse_iterator _jt = aAdd.rbegin(), _jt_end(
				aAdd.rend());
		for (; _jt != _jt_end;)
		{
			demand_dgs_t::reverse_iterator _kt = aRemoved.rbegin(), _kt_end(
					aRemoved.rend());
			bool _is_found = false;
			for (; _kt != _kt_end; ++_kt)
			{
				if (*_kt == *_jt)
				{
					aRemoved.erase(_kt.base());
					_jt = demand_dgs_t::reverse_iterator(
							aAdd.erase(_jt.base()));
					_is_found = true;
					break;
				}
			}
			if (!_is_found)
				++_jt;
		}
	}
	else
	{
		aAdd = aNew;
	}
	return true;
}

demand_dgs_for_t CRequiredDG::MAddClient(id_t const& aId)
{
	VLOG(2) << "Add new client " << aId;
	const bool _is = FIds.insert(aId).second;
	CHECK(_is);
	demand_dgs_for_t _new;
	MAddReceiversFor(aId, _new);
	return _new;
}
demand_dgs_for_t CRequiredDG::MRemoveClient(NSHARE::uuid_t const& aUUID)
{
	demand_dgs_for_t _old;
	VLOG(2) << "Remove " << aUUID;

	if (size_t _val = FIds.erase(id_t(aUUID)))
	{
		VLOG(2) << "Erased " << _val;
		CHECK_EQ(_val, 1);
		MRemoveReceiversFor(aUUID, _old);
		FWhatIsSendingBy.erase(aUUID);
	}
	return _old;
}
void CRequiredDG::MFillByRawProtocol(user_datas_t& aFrom, user_datas_t& aFailed,
		fail_send_array_t& aFail) const
{
	CHECK_EQ(aFrom.size(), 1);
	uuids_of_receiver_t _receivers;
	user_data_t& _data = aFrom.front();
	int const _error = MGetCustomersFor(_receivers,
			_data.FDataId.FRouting.FFrom.FUuid,
			_data.FDataId.FRawProtocolNumber);

	if (_error < 0)
	{
		fail_send_t _sent(_data.FDataId, _data.FDataId.FDestination,
				(eError) _error);
		aFail.push_back(_sent);

		aFailed.splice(aFailed.end(), aFrom, aFrom.begin());
	}
	else
	{
		VLOG(4) << " OK " << " filling info";

		MFillRouteAndDestanationInfo(_receivers, _data.FDataId, aFail);
	}
}
void CRequiredDG::MFillMsgHandlersFor(user_datas_t & aFrom, user_datas_t &aTo,
		fail_send_array_t & aError) const
{
	user_datas_t _fails;
	for (; !aFrom.empty();)
	{
		int _error = E_NO_ERROR;
		user_data_info_t & _data_info = aFrom.front().FDataId;
		CHECK_EQ(_data_info.FRouting.size(), 1);

		required_header_t _header;
		protocols_t::const_iterator _prot_it;
		uuids_of_expecting_dg_t::const_iterator _header_it;
		uuids_of_receiver_t::const_iterator _uuid_it;

		NSHARE::CText const _protocol =
				_data_info.MIsRaw() ? RAW_PROTOCOL_NAME : _data_info.FProtocol;

		NSHARE::uuid_t const& _from = _data_info.FRouting.FFrom.FUuid;
		NSHARE::uuid_t const& _for = _data_info.FRouting.back();

		protocol_of_uuid_t::const_iterator _it = FWhatIsSendingBy.find(_from);

		if (_it == FWhatIsSendingBy.end())
		{
			LOG(ERROR)<< "The  "<<_from<<" is not sent msg to "<<_for;
			_error=E_HANDLER_IS_NOT_EXIST;
			goto error;
		}
		_prot_it = _it->second.find(_protocol);

		if (_prot_it == _it->second.end())
		{
			LOG(ERROR)<< "No handler for " << _protocol
			<< " is not exist";
			_error=E_HANDLER_IS_NOT_EXIST;
			goto error;
		}
		if (_data_info.MIsRaw())
		{
			_header.FNumber = _data_info.FRawProtocolNumber;
		}
		else
		{
			IExtParser* _p = CParserFactory::sMGetInstance().MGetFactory(
					_protocol);

			if (!_p)
			{
				LOG(ERROR)<< "Parsing module for " << _protocol
				<< " is not exist";
				_error=E_PARSER_IS_NOT_EXIST;
				goto error;
			}
			const size_t _size = aFrom.front().FData.size();

			IExtParser::result_t const _msgs = _p->MParserData(
					(const uint8_t*) aFrom.front().FData.ptr_const(),
					(const uint8_t*) aFrom.front().FData.ptr_const() + _size,
					_data_info.FRouting.FFrom.FUuid);
			VLOG(2)<<"Number of msg:"<<_msgs.size();
			if (_msgs.size() != 1)
			{
				LOG(ERROR)<< "No msg or the number of msg more then 1: "<<_size;
				_error=E_HANDLER_NO_MSG_OR_MORE_THAN_ONE;
				goto error;
			}
			else if(_msgs.front().FErrorCode!=0)
			{
				LOG(ERROR) << "Error occurred during parsing packet:"<<_msgs.front().FErrorCode;
				_error=_msgs.front().FErrorCode;
				goto error;
			}
			_header = _msgs.front().FType;
		}

		_header_it = _prot_it->second.find(_header);

		if (_header_it == _prot_it->second.end() )
		{
			LOG(ERROR)<< "No handler for " << _header
			<< " is not exist";
			_error=E_HANDLER_IS_NOT_EXIST;
			goto error;
		}
		_uuid_it=_header_it->second.find(_for);

		if (_uuid_it == _header_it->second.end() || _uuid_it->second.empty())
		{
			LOG(ERROR)<< "No handler for " << _header
			<< " is not exist";
			_error=E_HANDLER_IS_NOT_EXIST;
			goto error;
		}
		if(_uuid_it->second.FNumberOfRealHandlers==0 || _uuid_it->second.FNumberOfRealHandlers==_uuid_it->second.size())
		{
			VLOG(4)<<"A good programmer as he is not blending  a registrator and a real software.";
			_data_info.FEventsList = _uuid_it->second;
		}else
		{
			LOG(WARNING)<<"The programmer is a pervert as he is blending  a registrator and a real software !!! Please, Refractoring the code.";
			uuids_t::const_iterator _ut=_data_info.FDestination.begin(),_ut_end=_data_info.FDestination.end();
			for(;_ut!=_ut_end && _for!=*_ut;++_ut)
				;

			bool const _has_sent_by_uuid_to_registator=_ut==_ut_end;
			if(_has_sent_by_uuid_to_registator)
			{
				LOG(ERROR)<<"Is not implemented";
				_error=E_INCORRECT_USING_OF_UDT_1;
				goto error;
				//todo parsing again
				//filling only  registrator handler
			}
			else //
			{
				VLOG(2)<<"Can fill all handlers for the packet as it has been sent not by uuid";
				_data_info.FEventsList = _uuid_it->second;
			}
		}

		aTo.splice(aTo.end(), aFrom, aFrom.begin());

		continue;

		error:

		fail_send_t _sent(_data_info);
		_sent.MSetError(_error);
		aError.push_back(_sent);

		_fails.splice(_fails.end(), aFrom, aFrom.begin());

	}
	//return data back
	aFrom.splice(aFrom.end(), _fails);
}

void CRequiredDG::MFillMsgReceivers(user_datas_t & aFrom, user_datas_t& aTo,
		fail_send_array_t& aFail) const
{
	//todo caching of result!!!!!!!!!!
	//when sent by name the result can be calculated previously
	//when sent by UUID the result is not defined
	user_datas_t _fail_packets;
	for (; !aFrom.empty();)
	{
		user_datas_t _handling;	//for convenience
		_handling.splice(_handling.end(), aFrom, aFrom.begin());

		VLOG(4) << "Next packet:" << _handling.front().FDataId;

		if (_handling.front().FDataId.MIsRaw())	//simplified parser for raw protocol
			MFillByRawProtocol(_handling, _fail_packets, aFail);
		else
			MFillByUserProtocol(_handling, _fail_packets, aFail);

		aTo.splice(aTo.end(), _handling);
	}

	VLOG(3) << "Move back failed packets";
	aFrom.splice(aFrom.end(), _fail_packets);
}
void CRequiredDG::MFillRouteAndDestanationInfo(
		uuids_of_receiver_t const& aRoute, user_data_info_t& aInfo,
		fail_send_array_t& aFail) const
{
	VLOG(2) << "Filling " << aInfo;
	const bool _is_sent_by_uuid = !aInfo.FDestination.empty();
	uuids_t _dest = aInfo.FDestination;
	uuids_t _fail_version;

	//if sending by uuid than
	//1) put to routing list only if exist registrators
	//2) checking if uuid exist
	//if sending by name than
	//1) put to routing all
	//2) put to destination only if exist 'real receivers'

	for (CRequiredDG::uuids_of_receiver_t::const_iterator _kt = aRoute.begin();
			_kt != aRoute.end(); ++_kt)
	{
		NSHARE::uuid_t const& _uuid = _kt->first;
		bool const _is_registrator_exist = _kt->second.FNumberOfRealHandlers
				!= _kt->second.size();

		NSHARE::version_t const& _req_version=_kt->second.FVersion;
		bool _is_fail=false;
		//checking version
		if(_req_version.MIsExist())
		{
			if (aInfo.FVersion.MIsExist())
			{
				if (!_req_version.MIsCompatibleWith(aInfo.FVersion))
				{
					LOG(ERROR)<<"Protocol version for "<<aInfo<<" is not compatible. Requirement "<<_req_version;
					_fail_version.push_back(_uuid);
					_is_fail=true;
				}
			}
			else
			{
				LOG(WARNING)<<"The parser is not set up msg version but  it's requirement for "<<_uuid;
			}
			//if not compatible
		}

		if (!_is_sent_by_uuid)
		{
			if (_kt->second.FNumberOfRealHandlers > 0)
				aInfo.FDestination.push_back(_uuid);
			
			else if(!_is_fail)//ptotection in the event  of unsuccessfully 
			//sending to uuid which is distanation and registrator 
				aInfo.FRegistrators.push_back(_uuid);

			if(!_is_fail)
				aInfo.FRouting.push_back(_uuid);
		}
		else if(!_is_fail)
		{
			//checking if handler exist

			uuids_t::iterator _it = _dest.begin(), _it_end =
					_dest.end();
			for (; _it != _it_end && *_it != _uuid; ++_it)
				;

			if (_it != _it_end)
			{
				aInfo.FRouting.push_back(_uuid);
				_dest.erase(_it);
			}
			else if (_is_registrator_exist)//it's registrator
			{
				aInfo.FRouting.push_back(_uuid);
				aInfo.FRegistrators.push_back(_uuid);
			}
		}else
		{
			//remove double error
			uuids_t::iterator _it = _dest.begin(), _it_end =
					_dest.end();
			for (; _it != _it_end && *_it != _uuid; ++_it)
				;

			if (_it != _it_end)
			{
				_dest.erase(_it);
			}
		}
	}

	if (!_fail_version.empty())
	{

		fail_send_t _sent(aInfo, _fail_version, E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE);
		LOG(ERROR)<<" ERROR "<<_sent;
		aFail.push_back(_sent);
	}
	if (!_dest.empty())
	{

		fail_send_t _sent(aInfo, _dest, E_HANDLER_IS_NOT_EXIST);
		LOG(ERROR)<<" ERROR "<<_sent;
		aFail.push_back(_sent);
	}
}
int CRequiredDG::MGetCustomersFor(uuids_of_receiver_t& aTo,
		NSHARE::uuid_t const& aFor, unsigned const aNumber) const
{
	VLOG(3) << "Get uuids for " << aFor << " by " << RAW_PROTOCOL_NAME;
	protocol_of_uuid_t::const_iterator _it = FWhatIsSendingBy.find(aFor);
	VLOG_IF(2, _it == FWhatIsSendingBy.end()) << "There is not customers for "
														<< aFor;
	if (_it == FWhatIsSendingBy.end())
		return 0;
	protocols_t::const_iterator _prot_it = _it->second.find(RAW_PROTOCOL_NAME);
	LOG_IF(INFO,_prot_it==_it->second.end()) << "Nobody expects of data from '"
														<< _it->first
														<< "' by  '"
														<< RAW_PROTOCOL_NAME
														<< "' protocol.";
	if (_prot_it == _it->second.end())
		return 0;

	VLOG(2) << "Receive by raw protocol.";
	required_header_t _header;
	_header.FNumber = aNumber;

	uuids_of_expecting_dg_t::const_iterator _jt = _prot_it->second.find(
			_header);
	if (_jt != _prot_it->second.end())
	{
		aTo = _jt->second;
	}
	return 0;
}
void CRequiredDG::MFillByUserProtocol(user_datas_t& aFrom,
		user_datas_t& aFailed, fail_send_array_t& aFail) const
{
	CHECK_EQ(aFrom.size(), 1);

	user_data_t& _data = aFrom.front();
	const NSHARE::uuid_t _from = _data.FDataId.FRouting.FFrom.FUuid;

	VLOG(3) << "Get uuids for " << _from;

	protocol_of_uuid_t::const_iterator _it = FWhatIsSendingBy.find(_from);
	VLOG_IF(2, _it == FWhatIsSendingBy.end()) << "There is not receivers for "
														<< _from;
	if (_it == FWhatIsSendingBy.end())
		return;

	protocols_t::const_iterator _prot_it = _it->second.find(
			_data.FDataId.FProtocol);
	LOG_IF(INFO,_prot_it==_it->second.end()) << "Nobody expects of data from '"
														<< _it->first
														<< "' by  '"
														<< _data.FDataId.FProtocol
														<< "' protocol.";
	if (_prot_it == _it->second.end())
		return;

	IExtParser* _p = CParserFactory::sMGetInstance().MGetFactory(
			_data.FDataId.FProtocol);
	LOG_IF(DFATAL,!_p) << "Parsing module for " << _data.FDataId.FProtocol
								<< " is not exist";

	if (!_p || _data.FData.empty())
	{
		fail_send_t _sent(_data.FDataId,_data.FDataId.FDestination,E_PARSER_IS_NOT_EXIST);
		LOG(ERROR)<<" ERROR "<<_sent;
		aFail.push_back(_sent);
		return;
	}
	const size_t _size = _data.FData.size();

	const uint8_t*_begin=(const uint8_t*)_data.FData.ptr_const();
	VLOG(4) << "Parsing " << _size << " bytes  by "<<_data.FDataId.FProtocol;
	IExtParser::result_t const _msgs = _p->MParserData(_begin,_begin + _size, _from);

	VLOG(1) << "Founded " << _msgs.size() << " dg.";
	IExtParser::result_t::const_iterator _mt = _msgs.begin(), _mt_end(
			_msgs.end());

	bool const _is_only_one = _msgs.size() == 1;
	bool _has_to_be_pop = false;

	for (; _mt != _mt_end; ++_mt)
	{
		IExtParser::obtained_dg_t const& _msg = *_mt;

		if (!_is_only_one)	//Spiting buffer if need
		{

			user_data_t _new_packet;
			_new_packet.FDataId = _data.FDataId;

			CHECK(_msg.FBegin != _msg.FEnd);

			_new_packet.FData = NSHARE::CBuffer(NULL, _msg.FBegin, _msg.FEnd);
			aFrom.push_back(_new_packet);
			VLOG(2)<<"Split big packet "<<(_msg.FEnd-_msg.FBegin)<<" bytes";
			_has_to_be_pop = true;
		}
		user_data_t& _handling_data = aFrom.back();

		if (_msg.FErrorCode != 0)
		{
			fail_send_t _sent(_handling_data.FDataId);
			_sent.MSetUserError(_msg.FErrorCode);
			_sent.FRouting.insert(_sent.FRouting.end(),
					_sent.FDestination.begin(), _sent.FDestination.end());

			aFail.push_back(_sent);
			VLOG(2)<<"Fail "<<_sent;
			aFailed.splice(aFailed.end(), aFrom, --aFrom.end());//not begin as the data can be added above
		}
		else
		{
			uuids_of_expecting_dg_t::const_iterator _jt = _prot_it->second.find(
					_msg.FType);
			LOG_IF(INFO, _jt==_prot_it->second.end()) << "Packet "
																<< _p->MToConfig(
																		_msg.FType).MToJSON(
																		true)
																<< " from "
																<< _from
																<< " does not required. Ignoring ...";

			if (_jt != _prot_it->second.end())
			{
				VLOG(4) << " OK " << " filling info";
				_handling_data.FDataId.FVersion=_msg.FType.FVersion;
				MFillRouteAndDestanationInfo(_jt->second,
						_handling_data.FDataId, aFail);
			}
		}
	}

	if (_has_to_be_pop)	//as we split input buffer
		aFrom.pop_front();
}

bool CRequiredDG::MRemoveReceiversFor(NSHARE::uuid_t const& aUUID,
		demand_dgs_for_t & aOld)
{
	VLOG(2) << "Remove demands for " << aUUID;

	id_t _id;
	_id.FUuid = aUUID; //id_t comapred by it uuid only

	demand_dgs_for_t::iterator _it = FDGs.find(_id);
	if (_it != FDGs.end())
	{
		demand_dgs_t _old;
		_old.swap(_it->second);
		FDGs.erase(_it);

		MRemoveDemandsFor(aUUID, _old, aOld);
		return true;
	}
	return false;
}

void CRequiredDG::MAddReceiversFor(id_t const& aUId, demand_dgs_for_t& aNew)
{
	//update protocol_of_uuid_t
	//1) find DGs which are required from aUUID
	//2) add all demands to protocol_of_uuid_t

	typedef std::vector<std::pair<id_t, demand_dg_t> > dgs_t;
	dgs_t _demands;
	dgs_t _registrator;
	for (demand_dgs_for_t::const_iterator _it = FDGs.begin(); _it != FDGs.end();
			++_it)
	{
		demand_dgs_t::const_iterator _dem_it = _it->second.begin();
		for (; _dem_it != _it->second.end(); ++_dem_it)
		{
			if (MDoesIdConformTo(aUId, _dem_it->FNameFrom))
			{
				VLOG(2) << *_dem_it << " from " << aUId.FUuid
									<< " is required of by " << _it->first;
				std::pair<id_t, demand_dg_t> _val(_it->first, *_dem_it);
				if ((_dem_it->FFlags & demand_dg_t::E_REGISTRATOR) != 0)
					_registrator.push_back(_val);
				else
					_demands.push_back(_val);
			}
		}
	}

	if (!_demands.empty())
	{
		for (dgs_t::const_iterator _it = _demands.begin();
				_it != _demands.end(); ++_it)
			MSendPacketFromTo(aUId.FUuid, _it->first.FUuid, _it->second, &aNew);

		for (dgs_t::const_iterator _it = _registrator.begin();
				_it != _registrator.end(); ++_it)
			MSendPacketFromTo(aUId.FUuid, _it->first.FUuid, _it->second, NULL);
	}
}
bool CRequiredDG::MDoesIdConformTo(id_t const& _id,
		NSHARE::CRegistration const& _reg) const
{
	VLOG(4) << "Id " << _id << " reg " << _reg;
	bool const _is_name = _reg.MIsName();
	VLOG(4) << "Is name =" << _is_name;
	bool const _has_sended =
			((_is_name && _reg.MIsForMe(_id.FName)) || //
					(!_is_name
							&& _reg.MGetAddress().MIsSubpathOfForRegistration(
									_id.FName)));
	return _has_sended;
}
CRequiredDG::unique_uuids_t CRequiredDG::MGetUUIDFor(
		NSHARE::CRegistration const& aName) const
{
	VLOG(2) << "Get uuids for " << aName;
	VLOG(4) << " Size=" << FIds.size();
	unique_uuids_t _res;
	std::set<id_t>::const_iterator _it = FIds.begin(), _it_end(FIds.end());
	for (; _it != _it_end; ++_it)
	{
		if (MDoesIdConformTo(*_it, aName))
			_res.insert(_it->FUuid);
	}
	VLOG_IF(1,_res.empty()) << "No clients for " << aName;
	return _res;
}

NSHARE::CConfig CRequiredDG::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	protocol_of_uuid_t::const_iterator _it = FWhatIsSendingBy.begin(), _it_end(
			FWhatIsSendingBy.end());

	for (; _it != _it_end; ++_it)
	{
		protocols_t::const_iterator _prot_it = _it->second.begin(),
				_prot_it_end(_it->second.end());
		for (; _prot_it != _prot_it_end; ++_prot_it)

		{
			NSHARE::CText const& _protocol=_prot_it->first.empty()?RAW_PROTOCOL_NAME:_prot_it->first;
			NSHARE::CConfig const _from=_it->first.MSerialize();
			uuids_of_expecting_dg_t::const_iterator _jt =
					_prot_it->second.begin(), _jt_end(_prot_it->second.end());
			for (; _jt != _jt_end; ++_jt)
			{
				NSHARE::CConfig _prot("msg");
				_prot.MAdd(_from);
				_prot.MAdd(user_data_info_t::KEY_PACKET_PROTOCOL, _protocol);
				_prot.MAdd(serialize_head(_jt->first,_protocol));

				for (uuids_of_receiver_t::const_iterator _kt =
						_jt->second.begin(); _kt != _jt->second.end(); ++_kt)
				{
					NSHARE::CConfig _to(user_data_info_t::KEY_PACKET_TO);
					_to.MAdd(_kt->first.MSerialize());
					_to.MAdd(_kt->second.FVersion.MSerialize());
					_to.MAdd("is_registrator",_kt->second.FNumberOfRealHandlers!=_kt->second.size());
					_to.MAdd("is_real",_kt->second.FNumberOfRealHandlers!=0);

					_prot.MAdd(_to);
				}
				_conf.MAdd(_prot);
			}

		}

	}
	//_conf.MAdd(FDGs.MSerialize());
	return _conf;
}
} /* namespace NUDT */
