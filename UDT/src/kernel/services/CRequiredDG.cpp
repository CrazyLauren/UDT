/*
 * CRequiredDG.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 18.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <algorithm>

#include <core/kernel_type.h>
#include <CParserFactory.h>
#include "CRequiredDG.h"
namespace NUDT
{
const NSHARE::CText CRequiredDG::NAME = "demands";
//todo using hash algorithm
CRequiredDG::CRequiredDG()
{
	FMsgID = 0;
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
		MRemoveReceiversFor(aFor.FUuid, &_rval.second);
		return _rval;
	}
	demand_dgs_t _added;
	demand_dgs_t _removed;
	MGetDiffDemandsDG(aFor, aReqDgs, _removed, _added);
	FDGs[aFor] = aReqDgs;

	if (!_removed.empty())
		MRemoveDemandsFor(aFor.FUuid, _removed, &_rval.second);
	if (!_added.empty())
		MAddDemandsFor(aFor.FUuid, _added, &_rval.first);
	return _rval;
}

bool CRequiredDG::MAddHandler(demand_dg_t const & aWhat,
		msg_handlers_t& _handlers) const
{
	bool const _is_registator = aWhat.FFlags.MGetFlag(demand_dg_t::E_REGISTRATOR);

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

	if (!_is_registator)
	{
		++_handlers.FNumberOfRealHandlers;
		return _handlers.FNumberOfRealHandlers == 1;
	}
	else

		return false;
}

bool CRequiredDG::sMSwapHeaderEndian(NSHARE::CText const & aProtocol, required_header_t* aTo)
{
	IExtParser* const _p = CParserFactory::sMGetInstance().MGetFactory(aProtocol);
	if (!_p)
	{
		LOG(DFATAL)<< "Parsing module for " << aProtocol << " is not exist";
		return false;
	}
	if (!_p->MSwapEndian(aTo))
	{
		DLOG(ERROR)<< "Swap endian method is not definition in " << aProtocol << " for " << *aTo;
		return false;
	}
	return true;
}
bool CRequiredDG::sMSwapMessageEndian(NSHARE::CText const & aProtocol,
		required_header_t const& aType, NSHARE::CBuffer* aTo)
{
	IExtParser* const _p = CParserFactory::sMGetInstance().MGetFactory(
			aProtocol);
	if (!_p)
	{
		LOG(DFATAL)<< "Parsing module for " << aProtocol << " is not exist";
		return false;
	}
	size_t const _header_offset = _p->MDataOffset(aType);
	size_t const _size = aTo->size();
	if (!_p->MSwapEndian(aType, (uint8_t*) (aTo->ptr()),
			(uint8_t*) (aTo->ptr()) + _size))
	{
		LOG(ERROR)<< "Cannot swap endian by  " << aProtocol << " for " << aType;
		return false;
	}
	return true;
}
bool CRequiredDG::MGetValidHeader(const demand_dg_t& aWhat,demand_dg_t * aTo) const
{
	*aTo=aWhat;
	if (!aWhat.MIsValidEndian())
	{
		if(sMSwapHeaderEndian(aWhat.FProtocol,&aTo->FWhat))
			aTo->MSwitchEndianFlag();
		else
			return false;

	}
	return true;
}
bool CRequiredDG::MRegisteringReceiverForMsg(const demand_dg_t& aWhat,
		const NSHARE::uuid_t& aFrom, const NSHARE::uuid_t& aTo,
		uuids_of_expecting_dg_t* const aUUIds, demand_dgs_for_t* aNew)
{
	DCHECK(aWhat.MIsValidEndian());
	bool const _is_registator = aWhat.FFlags.MGetFlag(demand_dg_t::E_REGISTRATOR);

	uuids_of_receiver_t& _r_uuid = MGetOrCreateUUIDsOfReceivers(*aUUIds,
			aWhat.FWhat);

	VLOG_IF(2,_r_uuid.empty()) << "The Packet has not been received of yet.";


	CHECK_GE(_r_uuid.FNumberOfRealReceivers, 0);

	if (!_is_registator)
		++_r_uuid.FNumberOfRealReceivers;

	bool const _is_first_real = MAddHandler(aWhat, _r_uuid[aTo]);

	LOG_IF(INFO,_is_first_real) << "Now The packet:" << aWhat << " from "
										<< aFrom << " is received of by "
										<< aTo;
	LOG_IF(INFO,!_is_first_real) << "The packet has been received twice of by "
											<< aTo;

	if (_is_first_real && aNew) //не верно, могут быть другие callbacks типа регистратор
	{
		demand_dg_t _new(aWhat);
		_new.FUUIDFrom.MSet(aTo);
		(*aNew)[id_t(aFrom)].push_back(_new);
	}
	return true;
}

bool CRequiredDG::MRemoveHandler(demand_dg_t const & aWhat,
		msg_handlers_t& _handlers) const
{
	bool const _is_registator = aWhat.FFlags.MGetFlag(demand_dg_t::E_REGISTRATOR);
	bool _rval = false;

	if (!_is_registator)
	{
		CHECK_GT(_handlers.FNumberOfRealHandlers, 0);
		--_handlers.FNumberOfRealHandlers;

		_rval = _handlers.FNumberOfRealHandlers == 0;
	}

	msg_handlers_t::iterator _it = _handlers.begin();
	for (; _it != _handlers.end() && *_it != aWhat.FHandler; ++_it)
		;

	DCHECK(_it != _handlers.end());

	if (_it != _handlers.end())
		_handlers.erase(_it);

	return _rval;
}

bool CRequiredDG::MUnRegisteringReceiverForMsg(demand_dg_t const & aWhat,
		NSHARE::uuid_t const & aFrom, NSHARE::uuid_t const & aTo,
		uuids_of_expecting_dg_t* const aUUIds, demand_dgs_for_t* aOld)
{
	DCHECK(aWhat.MIsValidEndian());
	bool const _is_registator =aWhat.FFlags.MGetFlag(demand_dg_t::E_REGISTRATOR);

	uuids_of_receiver_t& _r_uuid = MGetOrCreateUUIDsOfReceivers(*aUUIds,
			aWhat.FWhat);

	if (_r_uuid.empty())
	{
		LOG(ERROR)<<"The packet: "<<aWhat.FWhat<<" is not sent by "<<aFrom;
		return false;
	}

	//looking for handlers for  UUID
	uuids_of_receiver_t::iterator _list_it = _r_uuid.find(aTo);
	if (_list_it == _r_uuid.end())
	{
		LOG(ERROR)<<"The packet: "<<aWhat.FWhat<<" from "<<aFrom<< " is not received by "<<aTo;
		return false;
	}

	if (!_is_registator)
		--_r_uuid.FNumberOfRealReceivers;

	bool const _is_last_real = MRemoveHandler(aWhat, _list_it->second);

	LOG_IF(INFO,_is_last_real) << "Now The packet:" << aWhat.FWhat
										<< " from " << aFrom
										<< " is not received of by " << aTo;

	LOG_IF(INFO,!_is_last_real) << "The packet:" << aWhat.FWhat << " from "
										<< aFrom << " still received of by "
										<< aTo;
	//cleanup maps
	if (_list_it->second.empty())
		_r_uuid.erase(_list_it);

//	if (_r_uuid.empty())
//		aUUIds->erase(_msg);

	if (_is_last_real && aOld)
	{
		demand_dg_t _tmp_dem(aWhat);
		_tmp_dem.FUUIDFrom.MSet(aTo);
		(*aOld)[id_t(aFrom)].push_back(_tmp_dem);
	}
	return true;
}
CRequiredDG::data_routing_t& CRequiredDG::MGetOrCreateExpectedListFor(
		NSHARE::uuid_t const & aFrom, NSHARE::CText const & aProtocol) const
{
	DCHECK(!aProtocol.empty());

	protocol_of_uuid_t::iterator _customer_it = FWhatIsSendingBy.find(aFrom);
	VLOG_IF(2,_customer_it==FWhatIsSendingBy.end()) << "The consumer " << aFrom
															<< " still was not sent.";
	if (_customer_it == FWhatIsSendingBy.end())
		_customer_it = FWhatIsSendingBy.insert(_customer_it,
				std::make_pair(aFrom, protocols_t()));

	CHECK(_customer_it != FWhatIsSendingBy.end());
	protocols_t& _proto = _customer_it->second;
	VLOG(2) << "Add new 'receiver' for " << _customer_it->first;
	protocols_t::iterator _prot_it = _proto.find(aProtocol);
	VLOG_IF(2,_prot_it==_proto.end()) << "Packets from '" << aFrom << "' by '"
												<< aProtocol
												<< "' protocol still were not sent.";
	if (_prot_it == _proto.end())
		_prot_it = _proto.insert(_prot_it,
				std::make_pair(aProtocol, data_routing_t()));

	CHECK(_prot_it != _proto.end());
	return _prot_it->second;
}

/** \brief Adding receiver(aTo) for message (aWhat)
 * 		   which was sent by user (aFrom)
 *
 *	\return if it's the first request of message
 *			by receiver then to New will be added aWhat
 */
void CRequiredDG::MSendPacketFromTo(NSHARE::uuid_t const& aFrom,
		NSHARE::uuid_t const& aTo, demand_dg_t const& aWhat,
		demand_dgs_for_t* aNew)
{
	VLOG(2) << "Send " << aWhat << " from " << aFrom << " to " << aTo;

	uuids_of_expecting_dg_t& _expected = MGetOrCreateExpectedListFor(aFrom,
			aWhat.FProtocol).FExpected;
	demand_dg_t _msg;
	if (MGetValidHeader(aWhat, &_msg))
		MRegisteringReceiverForMsg(_msg, aFrom, aTo, &_expected, aNew);
	else
		LOG(DFATAL)<<"The message header is not valid.";//todo send error

}

/** \brief Remove receiver(aTo) for message (aWhat)
 * 		   which was sent by user (aFrom)
 *
 *	\return if it's the last request of message
 *			by receiver then to aOld will be added aWhat
 */
void CRequiredDG::MUnSendPacketFromTo(NSHARE::uuid_t const& aFrom,
		NSHARE::uuid_t const& aTo, demand_dg_t const& aWhat,
		demand_dgs_for_t* aOld)
{
	VLOG(2) << "Unsent " << aWhat << " from " << aFrom << " to " << aTo;

	uuids_of_expecting_dg_t& _uuids = MGetOrCreateExpectedListFor(aFrom,
			aWhat.FProtocol).FExpected;
	demand_dg_t _msg;
	if (MGetValidHeader(aWhat, &_msg))
		MUnRegisteringReceiverForMsg(_msg, aFrom, aTo, &_uuids, aOld);
	else
		LOG(WARNING)<<"The message header is not valid.";
}
void CRequiredDG::MAddDemandsFor(NSHARE::uuid_t const& aFor,
		const demand_dgs_t& aAdded, demand_dgs_for_t* aNew)
{
	//update protocol_of_uuid_t
	//1) find MSGs which are required to aFor from existing UUID
	//2) add all demands to protocol_of_uuid_t
	demand_dgs_t::const_iterator _req_it = aAdded.begin();
	for (; _req_it != aAdded.end(); ++_req_it)
	{
		VLOG(2) << "Try received " << *_req_it;
		unique_uuids_t const _uuids = MGetUUIDFor(_req_it->FNameFrom);
		//todo here has to checked message inheritance

		//VLOG(2) << " Founded uuids: " << _uuids;

		unique_uuids_t::const_iterator _uuid_it = _uuids.begin();
		for (; _uuid_it != _uuids.end(); ++_uuid_it)
			MSendPacketFromTo(*_uuid_it, aFor, *_req_it, aNew);
	}
}
void CRequiredDG::MRemoveDemandsFor(NSHARE::uuid_t const& aTo,
		demand_dgs_t const& aWhat, demand_dgs_for_t* aRemoved)
{
	demand_dgs_t::const_iterator _jt = aWhat.begin(), _jt_end(aWhat.end());
	for (; _jt != _jt_end; ++_jt)
	{
		unique_uuids_t const _uuids = MGetUUIDFor(_jt->FNameFrom);

		//VLOG(2) << " Founded uuids: " << _uuids;

		unique_uuids_t::const_iterator _uuid_it = _uuids.begin();
		for (; _uuid_it != _uuids.end(); ++_uuid_it)
		{
			MUnSendPacketFromTo(*_uuid_it, aTo, *_jt, aRemoved);
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
		demand_dgs_t::reverse_iterator _jt = aAdd.rbegin();
		for (; _jt != aAdd.rend();)
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
		MRemoveReceiversFor(aUUID, &_old);
		FWhatIsSendingBy.erase(aUUID);
	}
	return _old;
}
/** \brief Looking for message's receivers
 * 		   and adding theirs to list of message's receivers
 *
 */
void CRequiredDG::MFillByRawProtocol(user_datas_t& aFrom, user_datas_t& aFailed,
		fail_send_array_t* const aFail) const
{
	DCHECK_EQ(aFrom.size(), 1);

	user_data_t& _data = aFrom.front();
	const NSHARE::uuid_t _from = _data.FDataId.FRouting.FFrom.FUuid;
	NSHARE::CText const& _protocol=RAW_PROTOCOL_NAME;

	VLOG(3) << "Get uuids for " << _from<<" protocol "<<_protocol;

	data_routing_t& _routing=MGetOrCreateExpectedListFor(_from,_protocol);

	if (_routing.FExpected.empty())
	{
		LOG(INFO)<< "Nobody expects of data from '" << _from<< "' by  '"<< _protocol<< "' protocol.";
		return;
	}

	VLOG(2) << "Receive by raw protocol.";
	required_header_t const& _header=_data.FDataId.FWhat;
	uuids_of_receiver_t _receivers;

	uuids_of_receiver_t const& _ru = MGetUUIDsOfReceivers(_routing.FExpected,
			_header);

	if (!_ru.empty())
		MFillRouteAndDestanationInfo(_ru, &_data.FDataId, aFail);
	else
		LOG(INFO)<< "Packet "<< _header.FNumber << " from "<< _from<< " does not required. Ignoring ...";

}
CRequiredDG::msg_handlers_t const* CRequiredDG::MGetHandlers(user_data_info_t & _data_info) const
{
	required_header_t & _header=_data_info.FWhat;
	NSHARE::uuid_t const& _from = _data_info.FRouting.FFrom.FUuid;
	NSHARE::uuid_t const& _for = _data_info.FRouting.back();
	NSHARE::CText const _protocol =
			_data_info.MIsRaw() ? RAW_PROTOCOL_NAME : _data_info.FProtocol;

	uuids_of_expecting_dg_t& _expected = MGetOrCreateExpectedListFor(_from,
			_protocol).FExpected;
	uuids_of_expecting_dg_t::const_iterator _header_it;
	uuids_of_receiver_t::const_iterator _uuid_it;

	if(_expected.empty())
	{
		LOG(ERROR)<< "The  "<<_from<<" is not sent msg to "<<_for;
		return NULL;
	}
	_header_it=_expected.find(_header);

	if (_header_it == _expected.end())
	{
		LOG(ERROR)<< "No handler for " << _header
		<< " is not exist";
		return NULL;
	}
	_uuid_it = _header_it->second.find(_for);

	if (_uuid_it == _header_it->second.end() || _uuid_it->second.empty())
	{
		LOG(ERROR)<< "No handler for " << _header
		<< " is not exist";
		return NULL;
	}
	return &_uuid_it->second;
}

unsigned CRequiredDG::sMSwapEndian(user_data_t& _data)
{
	user_data_info_t & _data_info = _data.FDataId;
	unsigned _error = E_NO_ERROR;
	if (_data_info.FEndian != NSHARE::E_SHARE_ENDIAN)
	{

		required_header_t & _header = _data_info.FWhat;
		NSHARE::CText const _protocol =
				_data_info.MIsRaw() ? RAW_PROTOCOL_NAME : _data_info.FProtocol;

		DCHECK_NE(_data_info.FEndian, NSHARE::E_SHARE_ENDIAN);

		if (sMSwapHeaderEndian(_protocol, &_header)
				&& sMSwapMessageEndian(_protocol, _header, &_data.FData))
			_data_info.FEndian = NSHARE::E_SHARE_ENDIAN;
		else
		{
			LOG(ERROR)<< "Cannot swap endian by  " << _protocol << " for " << _header;
			_error = E_CANNOT_SWAP_ENDIAN;
		}
	}
	return _error;
}

void CRequiredDG::MFillMsgHandlersFor(user_datas_t & aFrom, user_datas_t &aTo,
		fail_send_array_t & aError) const
{
	user_datas_t _fails;
	for (; !aFrom.empty();)
	{
		unsigned _error = E_NO_ERROR;
		user_data_t& _data=aFrom.front();
		user_data_info_t & _data_info = _data.FDataId;
		required_header_t & _header=_data_info.FWhat;
		NSHARE::uuid_t const& _for = _data_info.FRouting.back();
		msg_handlers_t const *_handlers=NULL;

		CHECK_EQ(_data_info.FRouting.size(), 1);

		NSHARE::CText const _protocol =
				_data_info.MIsRaw() ? RAW_PROTOCOL_NAME : _data_info.FProtocol;


//		IExtParser* const _p = CParserFactory::sMGetInstance().MGetFactory(
//				_protocol);//has to be before the other methods as they can use parser
//		if (_p)
//			_data_info.FDataOffset=_p->MDataOffset(_header);
//		else
//		{
//			LOG(ERROR)<< "Parsing module for " << _protocol << " is not exist";
//			_error = E_PARSER_IS_NOT_EXIST;
//			goto error;
//		}

		if(_data_info.FEndian != NSHARE::E_SHARE_ENDIAN)
		{
			_error=sMSwapEndian(_data);
			if(_error!=E_NO_ERROR)
				goto error;
		}

		DCHECK_EQ(_data_info.FEndian , NSHARE::E_SHARE_ENDIAN);

		_handlers=MGetHandlers(_data_info);
		if(!_handlers)
		{
			LOG(ERROR)<< "No handler for " << _header<< " is not exist";
			_error=E_HANDLER_IS_NOT_EXIST;
			goto error;
		}

		if (_handlers->FNumberOfRealHandlers == 0
				|| _handlers->FNumberOfRealHandlers
						== _handlers->size())
		{
			VLOG(4)
								<< "A good programmer as he is not blending  a registrator and a real software.";
			_data_info.FEventsList = *_handlers;
		}
		else
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
				_data_info.FEventsList = *_handlers;
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

void CRequiredDG::MFillMsgReceivers(user_datas_t * const aFrom,
		user_datas_t * const aTo, fail_send_array_t * const aFail) const
{
	DCHECK_NOTNULL(aFrom);
	DCHECK_NOTNULL(aTo);
	user_datas_t& _from(*aFrom); //optimization
	user_datas_t& _to(*aTo); //optimization

	//todo caching of result!!!!!!!!!!
	//when sent by name the result can be calculated previously
	//when sent by UUID the result is not defined
	user_datas_t _fail_packets;
	for (; !_from.empty();)
	{
		user_datas_t _handling;	//for convenience
		_handling.splice(_handling.end(), _from, _from.begin());

		VLOG(4) << "Next packet:" << _handling.front().FDataId;

		if (_handling.front().FData.empty())
		{
			LOG(DFATAL)<<"Empty packet: "<<_handling.front().FDataId;
			if(aFail)
			{
				fail_send_t _sent(_handling.front().FDataId, _handling.front().FDataId.FDestination,
						E_UNKNOWN_ERROR);
				aFail->push_back(_sent);
			}

			_fail_packets.splice(_fail_packets.end(), _handling);
		}
		else
		{
			if (_handling.front().FDataId.MIsRaw())	//simplified parser for raw protocol
				MFillByRawProtocol(_handling, _fail_packets, aFail);
			else
				MFillByUserProtocol(&_handling,& _fail_packets, aFail);

			_to.splice(_to.end(), _handling);
		}
	}

	VLOG(3) << "Move back failed packets";
	_from.splice(_from.end(), _fail_packets);
}

bool CRequiredDG::MCheckVersion(const NSHARE::version_t& _req_version,
		const NSHARE::uuid_t& _uuid, user_data_info_t& aInfo) const
{
	//checking version
	if (_req_version.MIsExist())
	{
		if (aInfo.FWhat.FVersion.MIsExist())
		{
			if (!_req_version.MIsCompatibleWith(aInfo.FWhat.FVersion))
			{
				LOG(ERROR)<< "Protocol version for " << aInfo << " is not compatible. Requirement " << _req_version;

				return false;
			}
		}
		else
		{
			LOG(WARNING) << "The parser is not set up msg version but  it's requirement for " << _uuid;
		}
		//if not compatible
	}
	return true;
}

/* \brief Adding uuid of receiver to the message
 *
 */
void CRequiredDG::MAddUUIDToRoute(msg_handlers_t const& aM,
		NSHARE::uuid_t const& _uuid, user_data_info_t& aInfo) const
{
	if (aM.FNumberOfRealHandlers > 0)
		aInfo.FDestination.push_back(_uuid);
	else
		aInfo.FRegistrators.push_back(_uuid);

	aInfo.FRouting.push_back(_uuid);
}

/** \brief Adding receiver to list of message's receivers
 *
 */
error_type CRequiredDG::MAddReceiverOfMsg(msg_handlers_t const& aHandlers,
		NSHARE::uuid_t const & _uuid, user_data_info_t& aInfo) const
{
	if (MCheckVersion(aHandlers.FVersion, _uuid, aInfo))
	{
		MAddUUIDToRoute(aHandlers, _uuid, aInfo);
	}
	else
	{
		return E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE;
	}
	return E_NO_ERROR;
}

/** \brief  Checking valid of receivers and
 * 			adding theirs to the list of message's receivers
 *
 */
void CRequiredDG::MFillRouteAndDestanationInfo(
		uuids_of_receiver_t const& aRoute, user_data_info_t* const aDataInfo,
		fail_send_array_t* const aErrors) const
{
	user_data_info_t& _data_info(*aDataInfo);
	fail_send_array_t& _fails(*aErrors);

	VLOG(2) << "Filling " << _data_info;
	if (aRoute.FNumberOfRealReceivers > 0)
	{
		uuids_t _uuids_of_incorrect_version;
		uuids_t _sent_to;

		const bool _is_by_subscriber = _data_info.FDestination.empty();//the data sent to concrete receiver or
																		//to all subscribers

		_data_info.FDestination.swap(_sent_to);
		std::sort(_sent_to.begin(), _sent_to.end());

		for (CRequiredDG::uuids_of_receiver_t::const_iterator _kt =
				aRoute.begin(); _kt != aRoute.end(); ++_kt)
		{
			NSHARE::uuid_t const& _uuid = _kt->first;
			msg_handlers_t const& _handlers = _kt->second;

			if (!_is_by_subscriber)//send message directly to uuids
			{
				uuids_t::iterator _it = std::find(_sent_to.begin(),
						_sent_to.end(), _uuid);
				bool const _is_found = _it != _sent_to.end();

				if (_is_found)
				{
					//The receiver is  subscribered to the msg

					_sent_to.erase(_it);

					error_type const _error = MAddReceiverOfMsg(_handlers,
							_uuid, _data_info);
					if (_error != E_NO_ERROR)
						_uuids_of_incorrect_version.push_back(_uuid);
				}
				else if (_handlers.MIsRegistrarExist())	//The registrar has to receive the msg
				{
					MAddUUIDToRoute(_handlers, _uuid, _data_info);
				}
			}
			else
			{
				error_type const _error = MAddReceiverOfMsg(_handlers, _uuid,
						_data_info);
				if (_error != E_NO_ERROR)
					_uuids_of_incorrect_version.push_back(_uuid);
			}
		}

		//!<generating errors


		if (!_sent_to.empty())
		{
			LOG(ERROR)<<"The user sends the msg to concrete uuids but they doesn't expect to receive this msg: "<<_sent_to;

			fail_send_t _sent(_data_info, _sent_to, E_HANDLER_IS_NOT_EXIST);
			_fails.push_back(_sent);
		}
		if (!_uuids_of_incorrect_version.empty())
		{
			LOG(ERROR)<<"Cannot send the msg the protocol's version is not compatible: "<<_sent_to;

			fail_send_t _sent(_data_info, _uuids_of_incorrect_version,
					E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE);
			_fails.push_back(_sent);
		}

		std::sort(aDataInfo->FDestination.begin(), aDataInfo->FDestination.end());
		std::sort(aDataInfo->FRegistrators.begin(), aDataInfo->FRegistrators.end());
		std::sort(aDataInfo->FRouting.begin(), aDataInfo->FRouting.end());
	}else
	{
		LOG(INFO)<<"Only registrars are exist!!!";
	}
}


uint32_t CRequiredDG::MNextMsgMask() const
{
	return (++FMsgID) << (sizeof(FMsgID) * 8);
}

/** \brief removing messages that have user error's
 *
 */
void CRequiredDG::MRemoveDataWithUserErrors(IExtParser::result_t * const aMsg,
		user_datas_t * const aFrom, user_datas_t * const aTo,
		fail_send_array_t* const aFail) const
{
	DCHECK_NOTNULL(aMsg);
	DCHECK_NOTNULL(aFrom);
	DCHECK_NOTNULL(aTo);

	IExtParser::result_t& _msgs(*aMsg);
	user_datas_t& _datas(*aFrom);
	user_datas_t& _invalide_datas(*aTo);

	IExtParser::result_t::iterator _mt = _msgs.begin();
	user_datas_t::iterator _it_buf = _datas.begin();

	for (; _mt != _msgs.end();)
	{
		CHECK(_it_buf != _datas.end());

		IExtParser::obtained_dg_t const & _msg = *_mt;
		user_data_t& _handling_data = *_it_buf;

		if (_msg.FErrorCode != E_NO_ERROR)
		{
			LOG(ERROR)<<"Some user error code="<<_msg.FErrorCode<<" ";
			if(aFail)
			{
				fail_send_t _sent(_handling_data.FDataId);
				_sent.MSetUserError(_msg.FErrorCode);
				aFail->push_back(_sent);

				VLOG(2) << "Fail " << _sent;
			}
			{
				//!< remove data from aDatas and add put it to aSendError
				_invalide_datas.splice(_invalide_datas.end(), _datas, _it_buf++);//only postincrement
			}
			{
				_mt = _msgs.erase(_mt);
			}

		}
		else
		{
			++_mt;
			++_it_buf;
		}
	}
}
/** \brief Creating from raw buffer messages.
 * The raw buffer will removed.
 *
 * \return An iterator pointing to the next element
 * \note iterator is used for optimization
 *
 */
user_datas_t::iterator CRequiredDG::MExtractMessages(IExtParser::result_t const & _msgs,
		user_datas_t *const aFrom,
		user_datas_t::iterator aWhat, user_datas_t *const  aTo,IExtParser const& aP) const
{
	DCHECK(aFrom->end()!=aWhat);

	bool const _is_only_one = _msgs.size() == 1;

	if (!_is_only_one)
	{

		IExtParser::result_t::const_iterator _mt = _msgs.begin(), _mt_end(
				_msgs.end());
		for (; _mt != _mt_end; ++_mt)
		{
			IExtParser::obtained_dg_t const & _msg = *_mt;

			user_data_t _new_packet;
			_new_packet.FDataId = aWhat->FDataId;
			_new_packet.FDataId.FPacketNumber |= MNextMsgMask();
			_new_packet.FDataId.FWhat = _msg.FType;
			_new_packet.FDataId.FDataOffset=aP.MDataOffset(_msg.FType);

			CHECK_LT(_msg.FBegin, _msg.FEnd);

			_new_packet.FData = NSHARE::CBuffer(_msg.FBegin, _msg.FEnd,3.0);
			aTo->push_back(_new_packet);

			VLOG(2) << "Split big packet " << (_msg.FEnd - _msg.FBegin)
								<< " bytes";
		}

		aWhat=aFrom->erase(aWhat);
	}else
	{
		aWhat->FDataId.FWhat= _msgs.front().FType;
		aWhat->FDataId.FDataOffset=aP.MDataOffset(_msgs.front().FType);

		aTo->splice(aTo->end(),*aFrom,aWhat++);//warning only postincrement
	}
	DCHECK(!aTo->empty());
	return aWhat;
}

CRequiredDG::uuids_of_receiver_t const& CRequiredDG::MGetUUIDsOfReceivers(
		uuids_of_expecting_dg_t const & aUUIDs,
		required_header_t const & _msg) const
{
	uuids_of_expecting_dg_t::const_iterator _jt = aUUIDs.find(_msg);

	if(_jt!=aUUIDs.end())
		return _jt->second;
	else
	{
		static uuids_of_receiver_t const _fix;
		return _fix;
	}
}
CRequiredDG::uuids_of_receiver_t& CRequiredDG::MGetOrCreateUUIDsOfReceivers(
		uuids_of_expecting_dg_t & aUUIDs,
		required_header_t const & _msg)
{
	uuids_of_expecting_dg_t::iterator _jt = aUUIDs.find(_msg);

	if(_jt==aUUIDs.end())
	{
		_jt = aUUIDs.insert(
						std::make_pair(_msg, uuids_of_receiver_t())).first;
	}
	return _jt->second;
}

/** \brief Adding receivers info to message
 *
 */
void CRequiredDG::MAddReceiversForMessages(IExtParser::result_t const& _msgs,
		user_datas_t* aFrom, user_datas_t * const aTo,
		uuids_of_expecting_dg_t const& aUUIDs, IExtParser const& _p,
		fail_send_array_t* const aFail) const
{
	IExtParser::result_t::const_iterator _mt = _msgs.begin(), _mt_end(
			_msgs.end());
	user_datas_t::iterator _it_buf = aFrom->begin();

	for (; _mt != _mt_end; ++_mt)
	{
		CHECK(_it_buf != aFrom->end());

		IExtParser::obtained_dg_t const & _msg = *_mt;
		user_data_t& _handling_data = *_it_buf;

		uuids_of_receiver_t const & _ru = MGetUUIDsOfReceivers(aUUIDs,
				_msg.FType);
		if (!_ru.empty())
		{
			VLOG(4) << " OK " << " filling info";
			MFillRouteAndDestanationInfo(_ru, &_handling_data.FDataId,
					aFail);
			++_it_buf;
		}
		else
		{
			LOG(INFO)<< "Packet " << _p.MToConfig(_msg.FType).MToJSON(true) << " from " << _handling_data.FDataId.FRouting.FFrom.FUuid << " does not required. Ignoring ...";
			aTo->splice(aTo->end(),*aFrom,_it_buf++);//only postincrement
		}
	}
}

NSHARE::CBuffer CRequiredDG::MParseData(IExtParser::result_t* aMsgs,
		user_data_t& _data, IExtParser& _p) const
{
	NSHARE::CBuffer& _buf = _data.FData;
	IExtParser::result_t& _msgs = *aMsgs;
	NSHARE::uuid_t const & _from = _data.FDataId.FRouting.FFrom.FUuid;

	size_t const _size = _buf.size();
	uint8_t const * _begin = (uint8_t const *) (_buf.ptr_const());
	uint8_t const * _end = _begin + _size;
	NSHARE::CBuffer _rval;

	VLOG(4) << "Parsing " << _size << " bytes  by "
						<< _data.FDataId.FProtocol;

	_msgs = _p.MParserData(_begin, _end, _from);
	VLOG(1) << "Founded " << _msgs.size() << " dg.";

	if (!_msgs.empty())
	{

		CHECK_LE(_msgs.back().FEnd, _end);
		CHECK_EQ(_msgs.back().FBegin, _begin);
		if (_end != _msgs.back().FEnd)
		{
			VLOG(2) << "Copy data to buffer ";
			LOG_IF(FATAL,_msgs.back().FEnd>=_end)
															<< "Invalid address of FEnd. "
															<< _msgs.back().FEnd
															<< " > " << _end;
			_rval = NSHARE::CBuffer(_msgs.back().FEnd, _end); //copy tail
			_buf.resize(_msgs.back().FEnd - _msgs.front().FBegin);
		}
		CHECK_EQ(_msgs.back().FEnd, _end);
	}
	else
	{
		VLOG(2) << "No msg";
		_rval = _buf;

	}
	return _rval;
}

void CRequiredDG::MAppendBufferedData(
		data_routing_t& _routing, user_data_t& _data) const
{
	NSHARE::CBuffer& _buffered_data = _routing.FBufferedData;
	VLOG(2) << "Buffer size= " << _buffered_data.size();
	bool const _was_buffered = !_buffered_data.empty();
	if (_was_buffered)
	{
		VLOG(2) << "Copy buffered data ";
		NSHARE::CBuffer const & _const_buf = _data.FData;
		_buffered_data.insert(_buffered_data.end(), _const_buf.begin(),
				_const_buf.end());
		_data.FData = _buffered_data;
		_buffered_data.release();
	}
}

bool CRequiredDG::MBufferingDataIfNeed(
		data_routing_t& _routing,NSHARE::CBuffer const & _tail) const
{
	NSHARE::CBuffer& _buffered_data = _routing.FBufferedData;

	if (!_tail.empty())
	{
		//!< copy data from shared memory to private memory if need
		if (_buffered_data.MIsAllocatorEqual(NSHARE::CBuffer::sMDefAllaocter()))
			_buffered_data = _tail;
		else
			_buffered_data.deep_copy(_tail);

		return true;
	}
	return false;
}
/** \brief Parse data which was sent by user
 * 		   and adding message's receivers
 *
 */
void CRequiredDG::MFillByUserProtocol(user_datas_t*const  aFrom,
		user_datas_t*const  aTo, fail_send_array_t* const aFail) const
{
	DCHECK_NOTNULL(aFrom);
	DCHECK_NOTNULL(aTo);

	user_datas_t& _sent_datas(*aFrom);

	user_datas_t::iterator _ut=_sent_datas.begin();

	for (; _ut != _sent_datas.end();)
	{
		user_data_t& _data = *_ut;
		const NSHARE::uuid_t _from = _data.FDataId.FRouting.FFrom.FUuid;
		NSHARE::CText const& _protocol = _data.FDataId.FProtocol;

		VLOG(3) << "Data from" << _from << " protocol " << _protocol;

		CRequiredDG::data_routing_t& _routing = MGetOrCreateExpectedListFor(
				_from, _protocol);

		if (_routing.FExpected.empty())
		{
			LOG(INFO)<< "Nobody expects of data from '" << _from<< "' by  '"<< _protocol<< "' protocol.";
			aTo->splice(aTo->end(),_sent_datas,_ut++);//warning must only  postincrement
			continue;
		}

		IExtParser* const _p = CParserFactory::sMGetInstance().MGetFactory(
				_data.FDataId.FProtocol);
		if (!_p)
		{
			LOG(DFATAL)<< "Parsing module for " << _data.FDataId.FProtocol<< " is not exist";
			if (aFail)
			{
				fail_send_t _sent(_data.FDataId, _data.FDataId.FDestination,
						E_PARSER_IS_NOT_EXIST);
				aFail->push_back(_sent);
			}

			aTo->splice(aTo->end(),_sent_datas,_ut++);//warning must only  postincrement
			continue;
		}
		MAppendBufferedData(_routing, _data);

		IExtParser::result_t _msgs;
		NSHARE::CBuffer const _tail = MParseData(&_msgs, _data, *_p);

		MBufferingDataIfNeed(_routing, _tail);

		if (!_msgs.empty())
		{
			user_datas_t _to;
			_ut=MExtractMessages(_msgs,&_sent_datas,_ut, &_to, *_p);//!< here increment iterator

			MRemoveDataWithUserErrors(&_msgs, &_to, aTo, aFail);

			MAddReceiversForMessages(_msgs,  &_to,aTo,_routing.FExpected,
					*_p, aFail);

			if(!_to.empty())
				_sent_datas.splice(_ut,_to);
		}
		else
		{
			DLOG(WARNING)<<"No messages";
			aTo->splice(aTo->end(),_sent_datas,_ut++);//only post increment
		}
	}
}

bool CRequiredDG::MRemoveReceiversFor(NSHARE::uuid_t const& aUUID,
		demand_dgs_for_t * aRemoved)
{
	VLOG(2) << "Remove receivers for " << aUUID;

	id_t _id;
	_id.FUuid = aUUID; //id_t comapred by it uuid only

	demand_dgs_for_t::iterator _it = FDGs.find(_id);
	if (_it != FDGs.end())
	{
		demand_dgs_t _remove_list;
		_remove_list.swap(_it->second);
		FDGs.erase(_it);

		MRemoveDemandsFor(aUUID, _remove_list, aRemoved);
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
	//todo here has to checked message inheritance

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
				if (_dem_it->FFlags.MGetFlag(demand_dg_t::E_REGISTRATOR))
					_registrator.push_back(_val);
				else
					_demands.push_back(_val);
			}
		}
	}
//fixme раньше было не закомментировано
//	if (!_demands.empty())
//	{
	for (dgs_t::const_iterator _it = _demands.begin(); _it != _demands.end();
			++_it)
		MSendPacketFromTo(aUId.FUuid, _it->first.FUuid, _it->second, &aNew);

	for (dgs_t::const_iterator _it = _registrator.begin();
			_it != _registrator.end(); ++_it)
		MSendPacketFromTo(aUId.FUuid, _it->first.FUuid, _it->second, NULL);
//	}
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
			NSHARE::CText const& _protocol =
					_prot_it->first.empty() ?
							RAW_PROTOCOL_NAME : _prot_it->first;
			NSHARE::CConfig const _from = _it->first.MSerialize();
			uuids_of_expecting_dg_t::const_iterator _jt =
					_prot_it->second.FExpected.begin(), _jt_end(
					_prot_it->second.FExpected.end());
			for (; _jt != _jt_end; ++_jt)
			{
				NSHARE::CConfig _prot("msg");
				_prot.MAdd(_from);
				_prot.MAdd(user_data_info_t::KEY_PACKET_PROTOCOL, _protocol);
				_prot.MAdd(serialize_head(_jt->first, _protocol));

				for (uuids_of_receiver_t::const_iterator _kt =
						_jt->second.begin(); _kt != _jt->second.end(); ++_kt)
				{
					NSHARE::CConfig _to(user_data_info_t::KEY_PACKET_TO);
					_to.MAdd(_kt->first.MSerialize());
					_to.MAdd(_kt->second.FVersion.MSerialize());
					_to.MAdd("is_registrator", _kt->second.MIsRegistrarExist());
					_to.MAdd("is_real", _kt->second.FNumberOfRealHandlers != 0);

					_prot.MAdd(_to);
				}
				_conf.MAdd(_prot);
			}

		}

	}
	//_conf.MAdd(FDGs.MSerialize());
	return _conf;
}
inline void CRequiredDG::MInitializeMsgInheritance()//todo
{
	typedef std::map<NSHARE::CText,IExtParser::inheritances_info_t> inheritances_t;

	inheritances_t _tree;
	{//read all Inheritance info
		CParserFactory::factory_its_t _i =
				CParserFactory::sMGetInstance().MGetIterator();
		for (; _i.FBegin != _i.FEnd; ++_i.FBegin)
		{
			IExtParser::inheritances_info_t const _val =
					_i.FBegin->second->MGetInheritances();
			_tree[_i.FBegin->first] = _val;
		}
	}
	typedef std::set<required_header_t, CReqHeaderFastLessCompare> msg_children_t;
	typedef std::map<required_header_t, msg_children_t,
			CReqHeaderFastLessCompare> msg_inheritance_tree_t;

	typedef std::map<NSHARE::CText, msg_inheritance_tree_t> msg_inheritances_t;

	msg_inheritances_t _inheritances;
	{
		inheritances_t::const_iterator _it=_tree.begin(),_it_end(_tree.end());
		for(;_it!=_it_end;++_it)
		{
			msg_inheritance_tree_t& _current=_inheritances[_it->first];

			IExtParser::inheritances_info_t const& _process=_it->second;
			IExtParser::inheritances_info_t::const_iterator _jt =
					_process.begin(), _jt_end = _process.end();

			for(;_jt!=_jt_end;++_jt)
			{
				//доделать
				//1) нужно найти всех потомков и родителей
				//2) подумать как хранить типы протоколов
				//3) customer должен получать в качестве указателя блок начала данных
				msg_children_t& _children=_current[*_jt];
				_children.insert(_jt->FChildHeader);
			}
		}
	}
}
} /* namespace NUDT */
