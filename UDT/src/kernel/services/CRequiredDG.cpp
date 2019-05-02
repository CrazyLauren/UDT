// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
<<<<<<< HEAD

=======
/*!\brief Adds the new handlers for the message
 *
*\return true if it the first "real" handler
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD

=======
/*!\brief Changes the byte order of a message header
 *
 *\return true - if no error
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
=======

/*!\brief Changes the byte order of a message
 *
 *\param aProtocol the protocol
 *\param aType the message header
 *\param aTo the message
 *
 *\return true - if no error
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
=======

/*!\brief Returns the valid message header
 *
 *\return true - no error
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
=======
/*!\brief Adds the receiver to the list
 * of the message's receiver
 *
 *\return true - if the receiver
 * receives the message for the first time
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD

=======
/*!\brief Removes the handler for the message
 *
 *\return true if it was the last handler
 */
>>>>>>> f3da2cc... see changelog.txt
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

<<<<<<< HEAD
=======
/*!\brief Removes the receiver from the list
 * of the message's receiver
 *
 *\return true - if the receiver will not more
 * receive the message.
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
=======
/*!\brief Get or Create the list of customer uuids
 * which is expected of messages from aFrom.
 *
 *\return the list
 */
>>>>>>> f3da2cc... see changelog.txt
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

<<<<<<< HEAD
/** \brief Adding receiver(aTo) for message (aWhat)
=======
/*!\brief Returns child of message
 *
 *\return the message children or the empty list
 */
CRequiredDG::msg_heritance_t const& CRequiredDG::MGetMessageChildren(
		NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType) const
{
	return sMGetMessageChildren(aMsgProtocol,aMsgType,FMsgsGenealogy);
}

/*!\brief Returns child of message
 *
 *\return the message children or the empty list
 */
CRequiredDG::msg_heritance_t const& CRequiredDG::sMGetMessageChildren(
		NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType,msg_inheritances_t const& aFrom)
{
	static msg_heritance_t const _fix;
	msg_inheritances_t::const_iterator _it=aFrom.find(aMsgProtocol);
	if(_it!=aFrom.end())
	{
		msg_inheritance_tree_t const& _tree=_it->second;

		msg_inheritance_tree_t::const_iterator _tit=_tree.find(aMsgType);
		if(_tit!=_tree.end())
		{
			msg_family_t const& _fam=_tit->second;
			return _fam.FChildren;
		}
	}

	return _fix;
}
/*!\brief Add children to message
 *
 */
void CRequiredDG::sMAddMessageChildren(
		NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType,
		msg_heritance_t const& aChildren,msg_inheritances_t *const aTo)
{
	DCHECK_NOTNULL(aTo);
	msg_inheritances_t& _to(*aTo);
	msg_inheritance_tree_t & _tree=_to[aMsgProtocol];
	msg_family_t& _family=_tree[aMsgType];
	_family.FChildren.insert(_family.FChildren.end(),aChildren.begin(),aChildren.end());
}
/*!\brief Returns parents of message
 *
 *\return the message parents or the empty list
 */
//todo зачем?
CRequiredDG::msg_heritance_t const& CRequiredDG::MGetMessageParents(
		NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType) const
{
	static msg_heritance_t const _fix;
	msg_inheritances_t::const_iterator _it=FMsgsGenealogy.find(aMsgProtocol);
	if(_it!=FMsgsGenealogy.end())
	{
		msg_inheritance_tree_t const& _tree=_it->second;

		msg_inheritance_tree_t::const_iterator _tit=_tree.find(aMsgType);
		if(_tit!=_tree.end())
		{
			msg_family_t const& _fam=_tit->second;
			return _fam.FParents;
		}
	}
	return _fix;
}
/*!\brief Adding receiver for the message children
 *
 * It adds children with taking into account
 * different message protocol. Therefore the info
 * can be added no only to "aIncludeTo". That is
 * if the protocol of message child  is differ from
 * the protocol of message, the new expected list
 * of messages will be created.
 *
 *\return a amount of first request of message
 */
unsigned CRequiredDG::MIncludeMessageChildren(NSHARE::uuid_t const & aFrom,
		NSHARE::uuid_t const & aTo, demand_dg_t const & aWhat,
		uuids_of_expecting_dg_t*  aIncludeTo, demand_dgs_for_t* const aNew, demand_dgs_for_t* const aOld)
{
	DCHECK_NOTNULL(aIncludeTo);
	unsigned _count=0;

	//inheritance
	msg_heritance_t const & _children = MGetMessageChildren(aWhat.FProtocol,
			aWhat.FWhat);
	msg_heritance_t::const_iterator _it = _children.begin(), _it_end(
			_children.end());

	demand_dg_t _child_msg(aWhat);
	_child_msg.FFlags.MSetFlag(demand_dg_t::E_AS_INHERITANCE,true);

	for (; _it != _it_end; ++_it)
	{
		if (_child_msg.FProtocol != _it->first)
		{
			_child_msg.FProtocol = _it->first;

			/// support inheritance between different protocols
			aIncludeTo =
					&MGetOrCreateExpectedListFor(aFrom, _child_msg.FProtocol).FExpected;
		}
		_child_msg.FWhat = _it->second;
		bool const _is = MRegisteringReceiverForMsg(_child_msg, aFrom, aTo,
				aIncludeTo,aNew,aOld);
		_count = _is ? _count + 1 : _count;
	}
	return _count;
}
/*!\brief Removing receiver from the message children
 *
 * For detail see method MIncludeMessageChildren.
 *
 *\return a amount of the last request of message
 */
unsigned CRequiredDG::MUnincludeMessageChildren(NSHARE::uuid_t const & aFrom,
		NSHARE::uuid_t const & aTo, demand_dg_t const & aWhat,
		uuids_of_expecting_dg_t*  aUnincludeFrom,demand_dgs_for_t* const aNew, demand_dgs_for_t* const aOld)
{
	DCHECK_NOTNULL(aUnincludeFrom);
	unsigned _count=0;

	//inheritance
	msg_heritance_t const & _children = MGetMessageChildren(aWhat.FProtocol,
			aWhat.FWhat);
	msg_heritance_t::const_iterator _it = _children.begin(), _it_end(
			_children.end());

	demand_dg_t _child_msg(aWhat);
	_child_msg.FFlags.MSetFlag(demand_dg_t::E_AS_INHERITANCE,true);

	for (; _it != _it_end; ++_it)
	{
		if (_child_msg.FProtocol != _it->first)
		{
			_child_msg.FProtocol = _it->first;

			/// support inheritance between different protocols
			aUnincludeFrom =
					&MGetOrCreateExpectedListFor(aFrom, _child_msg.FProtocol).FExpected;
		}
		_child_msg.FWhat = _it->second;

		bool const _is = MUnRegisteringReceiverForMsg(_child_msg, aFrom, aTo,
				aUnincludeFrom,aNew,aOld);

		_count = _is ? _count + 1 : _count;
	}
	return _count;
}

/**\brief Adding receiver(aTo) for message (aWhat)
>>>>>>> f3da2cc... see changelog.txt
 * 		   which was sent by user (aFrom)
 *
*\return if it's the first request of message
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

/**\brief Remove receiver(aTo) for message (aWhat)
 * 		   which was sent by user (aFrom)
 *
*\return if it's the last request of message
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
<<<<<<< HEAD
void CRequiredDG::MAddDemandsFor(NSHARE::uuid_t const& aFor,
		const demand_dgs_t& aAdded, demand_dgs_for_t* aNew)
=======
/*!\brief Requests senders sends the requirement messages to aFor
 *
 *
 */
void CRequiredDG::MAddSendersOfMsgFor(NSHARE::uuid_t const& aFor,
		const demand_dgs_t& aAdded, demand_dgs_for_t* const aNew,demand_dgs_for_t* const aRemoved)
>>>>>>> f3da2cc... see changelog.txt
{
	//update protocol_of_uuid_t
	//1) find MSGs which are required to aFor from existing UUID
	//2) add all demands to protocol_of_uuid_t
	demand_dgs_t::const_iterator _req_it = aAdded.begin();
	for (; _req_it != aAdded.end(); ++_req_it)
	{
		VLOG(2) << "Try received " << *_req_it;
<<<<<<< HEAD
		unique_uuids_t const _uuids = MGetUUIDFor(_req_it->FNameFrom);
		//todo here has to checked message inheritance
=======
		bool const _is_nearest=_req_it->FFlags.MGetFlag(demand_dg_t::E_NEAREST);
		bool const _is_invert=_req_it->FFlags.MGetFlag(demand_dg_t::E_INVERT_GROUP);
		unique_uuids_t _uuids = MGetUUIDFor(_req_it->FNameFrom,_is_invert);

		if (_is_nearest)
		{
			MGetOnlyNearestUUIDFor(&_uuids);


			//save info about current level

			DCHECK(FNearestInfo[aFor].find(_req_it->FHandler)==FNearestInfo[aFor].end());///<check for logical error
			if(!_uuids.empty())
				FNearestInfo[aFor][_req_it->FHandler]=_uuids.begin()->second;
			else
				FNearestInfo[aFor][_req_it->FHandler]=std::numeric_limits<unsigned>::max();
>>>>>>> f3da2cc... see changelog.txt

		//VLOG(2) << " Founded uuids: " << _uuids;

		unique_uuids_t::const_iterator _uuid_it = _uuids.begin();
		for (; _uuid_it != _uuids.end(); ++_uuid_it)
			MSendPacketFromTo(*_uuid_it, aFor, *_req_it, aNew);
	}
}
<<<<<<< HEAD
void CRequiredDG::MRemoveDemandsFor(NSHARE::uuid_t const& aTo,
		demand_dgs_t const& aWhat, demand_dgs_for_t* aRemoved)
=======

/*!\brief Requests senders does not send
 * the requirement messages to aFor
 *
 *
 */
void CRequiredDG::MRemoveSendersOfMsgFor(NSHARE::uuid_t const& aTo,
		demand_dgs_t const& aWhat,demand_dgs_for_t* const aNew, demand_dgs_for_t* const aRemoved)
>>>>>>> f3da2cc... see changelog.txt
{
	demand_dgs_t::const_iterator _jt = aWhat.begin(), _jt_end(aWhat.end());
	for (; _jt != _jt_end; ++_jt)
	{
		unique_uuids_t const _uuids = MGetUUIDFor(_jt->FNameFrom);

<<<<<<< HEAD
		//VLOG(2) << " Founded uuids: " << _uuids;
=======
			//remove info about current level
			const size_t _num=FNearestInfo[aTo].erase(_jt->FHandler);

			DCHECK_NE(_num,0);///<check for logical error
		}
>>>>>>> f3da2cc... see changelog.txt

		unique_uuids_t::const_iterator _uuid_it = _uuids.begin();
		for (; _uuid_it != _uuids.end(); ++_uuid_it)
		{
			MUnSendPacketFromTo(*_uuid_it, aTo, *_jt, aRemoved);
		}
	}
}
<<<<<<< HEAD
demand_dgs_for_t const& CRequiredDG::MGetDemands() const
{
	return FDGs;
}
=======


/*!\brief Gets list of added and removed demands
 *
 *\param [out] aRemoved list of removed (has not to be null)
 *\param [out] aAdded list of added (has not to be null)
 */
>>>>>>> f3da2cc... see changelog.txt
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
/**\brief Looking for message's receivers
 * 		   and adding theirs to list of message's receivers
 *
 */
void CRequiredDG::MFillByRawProtocol(user_datas_t*const aFrom, user_datas_t*const aTo,
		fail_send_array_t* const aFail) const
{
	DCHECK_NOTNULL(aFrom);
	DCHECK_NOTNULL(aTo);

	user_datas_t& _sent_datas(*aFrom);
	user_datas_t::iterator _ut=_sent_datas.begin();

	for (; _ut != _sent_datas.end();)
	{
		user_data_t& _data = *_ut;
		const NSHARE::uuid_t _from = _data.FDataId.FRouting.FFrom.FUuid;
		NSHARE::CText const& _protocol = RAW_PROTOCOL_NAME;

		VLOG(3) << "Get uuids for " << _from << " protocol " << _protocol;

		data_routing_t& _routing = MGetOrCreateExpectedListFor(_from,
				_protocol);

		if (_routing.FExpected.empty())
		{
			LOG(INFO) << "Nobody expects of data from '" << _from
									<< "' by  '" << _protocol << "' protocol.";
			aTo->splice(aTo->end(), _sent_datas, _ut++);//only post increment
			continue;
		}

		VLOG(2) << "Receive by raw protocol.";
		required_header_t const& _header = _data.FDataId.FWhat;
		uuids_of_receiver_t _receivers;

		uuids_of_receiver_t const& _ru = MGetUUIDsOfReceivers(
				_routing.FExpected, _header);

		if (!_ru.empty() && _ru.FNumberOfRealReceivers > 0)
		{
			MFillRouteAndDestanationInfo(_ru, &_data.FDataId, aFail);
			++_ut;
		}
		else
		{
			LOG(INFO) << "Packet " << _header.FNumber << " from " << _from
									<< " does not required. Ignoring ...";
			LOG_IF(INFO,_ru.FNumberOfRealReceivers==0)
																<< "As only registrars are exist!!!";
			aTo->splice(aTo->end(), _sent_datas, _ut++);//only post increment
		}
	}

}
<<<<<<< HEAD
CRequiredDG::msg_handlers_t const* CRequiredDG::MGetHandlers(user_data_info_t & _data_info) const
=======
/*!\brief Returns the list of message handlers of the customer
 * or null.
 *
 */
CRequiredDG::msg_handlers_t const* CRequiredDG::MGetHandlers(user_data_info_t const & aMsgInfo) const
>>>>>>> f3da2cc... see changelog.txt
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
				MFillByRawProtocol(&_handling, &_fail_packets, aFail);
			else
				MFillByUserProtocol(&_handling,& _fail_packets, aFail);

			_to.splice(_to.end(), _handling);
		}
	}

	VLOG(3) << "Move back failed packets";
	_from.splice(_from.end(), _fail_packets);
}
<<<<<<< HEAD

=======
/*!\brief Check for correction version of the sent
 * message and the requirement message's version.
 *
 */
>>>>>>> f3da2cc... see changelog.txt
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

/*\brief Adding uuid of receiver to the message
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

/**\brief Adding receiver to list of message's receivers
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

/**\brief  Checking valid of receivers and
 * 			adding theirs to the list of message's receivers
 *
 *
 */
void CRequiredDG::MFillRouteAndDestanationInfo(
		uuids_of_receiver_t const& aRoute, user_data_info_t* const aDataInfo,
		fail_send_array_t* const aErrors) const
{
	user_data_info_t& _data_info(*aDataInfo);
	fail_send_array_t& _fails(*aErrors);

	VLOG(2) << "Filling " << _data_info;
	DCHECK_GT(aRoute.FNumberOfRealReceivers, 0);
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

		///<generating errors


		if (!_sent_to.empty())
		{
			LOG(ERROR)<<"The user sends the msg to concrete uuids but they doesn't expect to receive this msg: "<<_sent_to;

			fail_send_t _sent(_data_info, _sent_to, E_HANDLER_IS_NOT_EXIST);
			_fails.push_back(_sent);
		}
		if (!_uuids_of_incorrect_version.empty())
		{
			LOG(ERROR)<<"Cannot send the msg the protocol's version is not compatible: "<< _uuids_of_incorrect_version;

			fail_send_t _sent(_data_info, _uuids_of_incorrect_version,
					E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE);
			_fails.push_back(_sent);
		}

		std::sort(aDataInfo->FDestination.begin(), aDataInfo->FDestination.end());
		std::sort(aDataInfo->FRegistrators.begin(), aDataInfo->FRegistrators.end());
		std::sort(aDataInfo->FRouting.begin(), aDataInfo->FRouting.end());
	}
}

<<<<<<< HEAD

uint32_t CRequiredDG::MNextMsgMask() const
=======
/*!\brief Returns next "inner" number
 *
 * If the user sent a few message into one
 * packet, it is not clear, what the message numbers has to be
 * equal. Therefore  Unique "inner" number is added to
 * the packet number for all messages into packet.
 */
uint32_t CRequiredDG::MNextPacketNumberMask() const
>>>>>>> f3da2cc... see changelog.txt
{
	return (++FMsgID) << (sizeof(FMsgID) * 8);
}

/**\brief removing messages that have user error's
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
				///< remove data from aDatas and add put it to aSendError
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
/**\brief Creating from raw buffer messages.
 * The raw buffer will removed.
 *
 *\return An iterator pointing to the next element
 *\note iterator is used for optimization
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
<<<<<<< HEAD

=======
/*!\brief Returns customer uuids list which is expected
 * the message
 *
 *\param aMap - list of all expected messages from
 *\param aMsg - the message
 *
 *\return list of uuids or empty list.
 *
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
=======
/*!\brief returns customer uuids list which is expected
 * the message, if the list is not exist, it will created
 *
 */
>>>>>>> f3da2cc... see changelog.txt
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

/**\brief Adding receivers info to message
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
		if (!_ru.empty() && _ru.FNumberOfRealReceivers>0)
		{
			VLOG(4) << " OK " << " filling info";
			MFillRouteAndDestanationInfo(_ru, &_handling_data.FDataId,
					aFail);
			++_it_buf;
		}
		else
		{
<<<<<<< HEAD
			LOG(INFO)<< "Packet " << _p.MToConfig(_msg.FType).MToJSON(true) << " from " << _handling_data.FDataId.FRouting.FFrom.FUuid << " does not required. Ignoring ...";
=======
			LOG_IF(INFO,_ru.FNumberOfRealReceivers==0)<<"Only registrars are exist!!!";
			LOG(INFO) << "Packet "
									<< serialize_head(_msg.FType,
											_handling_data.FDataId.FProtocol).MToJSON(
											true) << " from "
									<< _handling_data.FDataId.FRouting.FFrom.FUuid
									<< " does not required. Ignoring ...";
			LOG_IF(INFO,_ru.FNumberOfRealReceivers==0)<<"As only registrars are exist!!!";
>>>>>>> 38a6ae7... see changelog
			aTo->splice(aTo->end(),*aFrom,_it_buf++);//only postincrement
		}
	}
}
<<<<<<< HEAD

=======
/*!\brief Extracts messages from buffer
 *
 *\return not handled part of buffer.
 */
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
=======
/*!\brief Appends the buffered data
 * to beginning of the buffer.
 * The buffer will be freed.
 *
*\return true - if append
 *
 */
bool CRequiredDG::MAppendBufferedData(
		data_routing_t* const aBufferedData, user_data_t* const aTo) const
{
	DCHECK_NOTNULL(aBufferedData);
	DCHECK_NOTNULL(aTo);

	data_routing_t& _routing(*aBufferedData);
	user_data_t& _data(*aTo);
>>>>>>> f3da2cc... see changelog.txt

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
<<<<<<< HEAD

=======
/*!\brief is buffering the data
 *
 *\param aTo - where the data will buffering
 *\param aData - what is buffering.
 *
 *\return true if data was buffered
 */
>>>>>>> f3da2cc... see changelog.txt
bool CRequiredDG::MBufferingDataIfNeed(
		data_routing_t& _routing,NSHARE::CBuffer const & _tail) const
{
	NSHARE::CBuffer& _buffered_data = _routing.FBufferedData;

	if (!_tail.empty())
	{
		///< copy data from shared memory to private memory if need
		if (_buffered_data.MIsAllocatorEqual(NSHARE::CBuffer::sMDefAllaocter()))
			_buffered_data = _tail;
		else
			_buffered_data.deep_copy(_tail);

		return true;
	}
	return false;
}
/**\brief Parse data which was sent by user
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

<<<<<<< HEAD
			aTo->splice(aTo->end(),_sent_datas,_ut++);//warning must only  postincrement
			continue;
=======
				if (!_msgs_headers.empty())
				{
					_ut = MExtractMessages(_msgs_headers, &_sent_datas, _ut, &_msgs, *_p); ///< here increment iterator

					MRemoveDataWithUserErrors(&_msgs_headers, &_msgs, aTo, aFail);
				}
			}
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD

bool CRequiredDG::MRemoveReceiversFor(NSHARE::uuid_t const& aUUID,
		demand_dgs_for_t * aRemoved)
=======
/*!\brief Removes receivers for messages
 * which has been sent to aFrom
 *
 */
bool CRequiredDG::MRemoveAllSendersOfMsgsFor(NSHARE::uuid_t const& aUUID,
		demand_dgs_for_t* aNew,demand_dgs_for_t * aRemoved)
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD

void CRequiredDG::MAddReceiversFor(id_t const& aUId, demand_dgs_for_t& aNew)
=======
/*!\brief Returns demands which are corresponded to the handlers
 *
 *\param aHandlers - the handlers
 *\param aDemand - the demands
 *\param [out] aTo - where is saved the result
 *\return A amount of demands which  has been added
 */
inline unsigned CRequiredDG::sMGetDemandsByHandlers(
		msg_handlers_t const & aHandlers, demand_dgs_t const & aDemand,
		demand_dgs_t* aTo)
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
=======
	DCHECK_NOTNULL(aTo);

	unsigned _count=0;
	protocol_of_uuid_t::const_iterator _customer_it = FWhatIsSendingBy.find(
			aFrom);
	VLOG_IF(2,_customer_it==FWhatIsSendingBy.end())<< "The consumer " << aFrom << " still was not sent.";
	if (_customer_it == FWhatIsSendingBy.end())
	{
		DLOG(INFO)<< "Nobody is received packets which are sent by " << aFrom;
	}
	else
	{
		protocols_t const & _proto = _customer_it->second;
		protocols_t::const_iterator _prot_it = _proto.begin(), _it_end(_proto.end());
		for (;_prot_it != _it_end;++_prot_it)
		{
			VLOG(2) << "From protocol:" << _prot_it->first;

			data_routing_t const & _data = _prot_it->second;
			DLOG_IF(WARNING,!_data.FBufferedData.empty()) << " buffer of " << _prot_it->first << " from " << aFrom << " is not empty. size=" << _data.FBufferedData.size();

			uuids_of_expecting_dg_t const & _messages = _data.FExpected;
			uuids_of_expecting_dg_t::const_iterator _it = _messages.begin();
			for (;_it != _messages.end();++_it)
			{
				required_header_t const & _header = _it->first;
				uuids_of_receiver_t const & _uuids = _it->second;
				uuids_of_receiver_t::const_iterator _it_uuid = _uuids.begin();
				for (;_it_uuid != _uuids.end();++_it_uuid)
				{
					demand_dgs_t& _result = (*aTo)[id_t(_it_uuid->first)];//id_t is compared only by uuid

					//looking for demands which are requirement the  message (_header)

					msg_handlers_t const & _handlers = _it_uuid->second;
					demand_dgs_t const & _dgs = MGetDemandFor(_it_uuid->first);

					DCHECK(!_dgs.empty());
					if (!_dgs.empty())
						_count+=sMGetDemandsByHandlers(_handlers, _dgs, &_result);
				}
			}
		}
	}
	return _count;
}

/*!\brief Returns distance info to aFrom
 *  for the demands
 *
 */
void CRequiredDG::MGetDistance(id_t const & aFrom, demand_dgs_for_t const& aFor,
		array_of_demand_for_t* aTo)
{
	DCHECK_NOTNULL(aTo);

	demand_dgs_for_t::const_iterator _it = aFor.begin();
	for (; _it != aFor.end(); ++_it)
	{
		DCHECK_NE(_it->first.FUuid, aFrom.FUuid);

		demand_dgs_t const & _dems = _it->second;
		demand_dgs_t::const_iterator _dem_it = _dems.begin();
		for (; _dem_it != _dems.end(); ++_dem_it)
		{
			bool const _is_invert = _dem_it->FFlags.MGetFlag(
					demand_dg_t::E_INVERT_GROUP);
			unsigned const _depth = MDoesIdConformTo(aFrom, _dem_it->FNameFrom,
					_is_invert);

			way_info_t _val;
			_val.FTo.FFor = _it->first.FUuid;
			_val.FTo.FWhat = *_dem_it;
			_val.FTo.FDistance = _depth;
			_val.FFrom=aFrom.FUuid;

			aTo->push_back(_val);
		}
	}
}

/*\brief Removes all receivers of the messages
 * which are sent by  aFrom
 *
 */
inline bool CRequiredDG::MRemoveAllReceiversForMsgsFrom(id_t const& aFrom,demand_dgs_for_t* aNew,demand_dgs_for_t* aRemoved)
{
	demand_dgs_for_t _result_dgs;

	unsigned const _amount=MGetWhatMsgSentFrom(aFrom.FUuid, &_result_dgs);

	if(_amount!=0)
	{
		DVLOG(1)<<"Remove "<<aFrom<<" for "<<_result_dgs;

		array_of_demand_for_t _new;
		array_of_demand_for_t _removed;

		MGetDistance(aFrom,_result_dgs,  &_removed);
		MTakeIntoAccountOnlyNearestSendersIfNeed(aFrom.FUuid, &_new, &_removed);
		MUpdateSendInfoFor(aFrom.FUuid, _new,_removed,  aNew, aRemoved);
	}


#ifndef NDEBUG
	//checking for all demands was removed
	protocol_of_uuid_t::iterator _customer_it = FWhatIsSendingBy.find(aFrom.FUuid);
	if (_customer_it != FWhatIsSendingBy.end())
	{
		protocols_t& _proto = _customer_it->second;
		for(;!_proto.empty();)
		{
			data_routing_t& _routing=_proto.begin()->second;
			uuids_of_expecting_dg_t& _expected=_routing.FExpected;

			for(;!_expected.empty();)
			{
				uuids_of_receiver_t& _list_of_uuids=_expected.begin()->second;

				for(;!_list_of_uuids.empty();)
				{
					msg_handlers_t& _handlers=_list_of_uuids.begin()->second;
					DCHECK(_handlers.empty());

					_list_of_uuids.erase(_list_of_uuids.begin());
				}
				_expected.erase(_expected.begin());
			}

			_proto.erase(_proto.begin());
		}

		FWhatIsSendingBy.erase(_customer_it);
	}
#else
	FWhatIsSendingBy.erase(aFrom.FUuid);
#endif

	return true;
}

/*!\brief Update distance info and removes program
 *  which is the more remote by group if need
 *
 *  As the  new sender of the packet is added,it need
 *  compare "distance" of new sender and current.
 *  if the distance of new sender is less then the old sender
 *  it need change sender of the packet.
 *
 *\param [in, out] aNew - list of senders which should be added
 *\param [out] aRemoved - list of senders which has to be removed
 *\param aFrom - Send messages from
 */
void CRequiredDG::MRemoveSenderIfNeed(NSHARE::uuid_t const& aFrom, array_of_demand_for_t*const aAdded,
		array_of_demand_for_t*const aRemoved)
{
	DCHECK_NOTNULL(aRemoved);

	array_of_demand_for_t& _new(*aAdded);
	array_of_demand_for_t::iterator _it_new = _new.begin();
	for (; _it_new != _new.end();)
	{
		NSHARE::uuid_t const & _to = _it_new->FTo.FFor;
		demand_dg_t const & _what = _it_new->FTo.FWhat;
		unsigned const _new_distance = _it_new->FTo.FDistance;
		bool const _is_check_nearest = _what.FFlags.MGetFlag(
				demand_dg_t::E_NEAREST);

		if (!_is_check_nearest)
			++_it_new; //!increment
		else
		{
			bool const _is_invert = _what.FFlags.MGetFlag(
					demand_dg_t::E_INVERT_GROUP);
			//fixme не уверен что верно для добавления нового клиента
			DCHECK(
					FNearestInfo[_to].find(_what.FHandler)
							!= FNearestInfo[_to].end()); ///<check for logical error
			unsigned &_current_distance = FNearestInfo[_to][_what.FHandler];
			if (_new_distance > _current_distance)
			{
				LOG(INFO) << "\n<--> Ignoring packet to " << _to
										<< " for demand " << _what
										<< " as distance " << _current_distance
										<< " is more then " << _new_distance
										<< " of " << aFrom;
				_it_new = _new.erase(_it_new); //!increment
			}
			else
			{
				if (_new_distance < _current_distance)
				{
					LOG(INFO) << "\n--- Disconnect packet sender to " << _to
											<< " for demand " << _what
											<< " as distance "<< _current_distance
											<< " is more then " << _new_distance
											<< " of " << aFrom;

					//save old uuid for demand

					unique_uuids_t _prev_uuids = MGetUUIDFor(_what.FNameFrom, _is_invert);
					MGetOnlyNearestUUIDFor(&_prev_uuids);
					unique_uuids_t::const_iterator _uuid_it = _prev_uuids.begin();
					for (;_uuid_it != _prev_uuids.end();++_uuid_it)
					{
						DCHECK_EQ(_uuid_it->second ,_current_distance);
						way_info_t _val;
						_val.FTo.FFor = _to;
						_val.FTo.FWhat = _what;
						_val.FTo.FDistance = _current_distance;
						_val.FFrom=_uuid_it->first;
						aRemoved->push_back(_val);
						LOG(INFO) << "\n--- Disconnect sender " << _uuid_it->first;
					}
					_current_distance = _new_distance;
				}
				else
				{
					LOG(INFO) << "Equal distance " << _current_distance;
				}
				++_it_new; //!increment
			}
		}
	}
}
/*!\brief Update distance info and adds program
 *  which is the less remote by group if need
 *
 *  As the sender of packet is removed,it need
 *  looking for a alternative sender. This method
 *  is doing it.
 *
 *\param [in] aRemoved - list of senders which should be removed
 *\param [out] aAdded - list of senders which has to be added
 *\param aFrom - Send messages from
 */
void CRequiredDG::MAddSenderIfNeed(NSHARE::uuid_t const & aFrom,
		array_of_demand_for_t const& aRemoved, array_of_demand_for_t*const aAdded)
{
	DCHECK_NOTNULL(aAdded);
	// Отправитель пакета был удалён. Ищем кто его может заменить

	array_of_demand_for_t::const_iterator _it_rem = aRemoved.begin();
	for (; _it_rem != aRemoved.end(); ++_it_rem)
	{
		NSHARE::uuid_t const & _to = _it_rem->FTo.FFor;
		demand_dg_t const & _what = _it_rem->FTo.FWhat;
		unsigned const _distance_of_rem = _it_rem->FTo.FDistance;
		bool const _is_check_nearest = _what.FFlags.MGetFlag(
				demand_dg_t::E_NEAREST);

		if (_is_check_nearest)
		{
			DCHECK_NE(_distance_of_rem, 0);
			DCHECK(
					FNearestInfo[_to].find(_what.FHandler)
							!= FNearestInfo[_to].end()); ///<check for logical error

			unsigned &_current_depth = FNearestInfo[_to][_what.FHandler];

			CHECK_GE(_distance_of_rem, _current_depth);

			if (_distance_of_rem == _current_depth)
			{
				LOG(INFO)<< "Sender "<<aFrom<<" of packet to " << _to
						<< " for demand " << _what<<" has been removed";

				bool const _is_invert = _what.FFlags.MGetFlag(demand_dg_t::E_INVERT_GROUP);
				unique_uuids_t _prev_uuids = MGetUUIDFor(_what.FNameFrom, _is_invert);
				size_t const _num = _prev_uuids.erase(aFrom);

				DCHECK_EQ(_num,1);

				if (!_prev_uuids.empty())
				{
					MGetOnlyNearestUUIDFor(&_prev_uuids);
					unsigned const _new_depth = _prev_uuids.begin()->second;
					DCHECK_GE(_new_depth, _current_depth);
					if (_new_depth != _current_depth)
					{
						LOG(INFO) << "Send as " << _what << " from :";

						LOG(INFO) << "\n+++ Connect packet sender to " << _to
												<< " for demand " << _what
												<< " as distance "<< _new_depth
												<< " is more then " << _current_depth
												<< " of removed  " << aFrom;

						unique_uuids_t::const_iterator _uuid_it = _prev_uuids.begin();
						for (;_uuid_it != _prev_uuids.end();++_uuid_it)
						{
							DCHECK_EQ(_uuid_it->second ,_new_depth);
							way_info_t _val;
							_val.FTo.FFor =_to;
							_val.FTo.FWhat = _what;
							_val.FTo.FDistance = _new_depth;
							_val.FFrom= _uuid_it->first;
							aAdded->push_back(_val);
							LOG(INFO) << "\n+++ Connect sender" << _uuid_it->first;
						}

						_current_depth = _new_depth;
					}
					else
					{
						LOG(INFO) << "The sender "<<aFrom<<" is nonunique";
					}
				}
				else
					LOG(INFO) << "No more senders"<< " for demand " << _what;
			}
		}
	}
}

/*!\brief Update distance info and removes (adds) program
 *  which is the more(less) remote by group if need
 *
 *\param [in, out] aNew - list of senders which should be added
 *\param [in, out] aRemoved - list of senders which should be removed
 *\param aFrom - Send messages from
 *
 */
void CRequiredDG::MTakeIntoAccountOnlyNearestSendersIfNeed(
		NSHARE::uuid_t const& aFrom, array_of_demand_for_t* aAdded,
		array_of_demand_for_t* aRemoved)
{
	array_of_demand_for_t _new_removed;
	array_of_demand_for_t _new_added;
	unsigned _count=0;

	if(aAdded)
	{
		MRemoveSenderIfNeed(aFrom,aAdded, &_new_removed);
	}
	if (aRemoved)
	{
		MAddSenderIfNeed(aFrom, *aRemoved,& _new_added);
	}

	if(aRemoved)
	{
		if(aRemoved->empty())
			aRemoved->swap(_new_removed);
		else
			aRemoved->insert(aRemoved->end(),_new_removed.begin(),_new_removed.end());
	}
	if(aAdded)
	{
		if(aAdded->empty())
			aAdded->swap(_new_added);
		else
			aAdded->insert(aAdded->end(),_new_added.begin(),_new_added.end());
	}
}

/*!\brief update info about route of packets from
 *
 *\return amount of change
 */
unsigned CRequiredDG::MUpdateSendInfoFor(NSHARE::uuid_t const& aFrom,
		array_of_demand_for_t const& aAdd, array_of_demand_for_t const& aRemove,
		demand_dgs_for_t* aNew, demand_dgs_for_t* aRemoved)
{
	unsigned _count = 0;
	for (array_of_demand_for_t::const_iterator _it = aRemove.begin();
			_it != aRemove.end(); ++_it)
	{
		bool _is = false;
		if (!_it->FTo.FWhat.FFlags.MGetFlag(demand_dg_t::E_REGISTRATOR))
			_is = MUnSendPacketFromTo(_it->FFrom, _it->FTo.FFor, _it->FTo.FWhat, aNew,	aRemoved);
		else
			_is = MUnSendPacketFromTo(_it->FFrom, _it->FTo.FFor, _it->FTo.FWhat, NULL,	NULL); //does not inform if registrar

		_count = _is ? _count + 1 : _count;
	}
	for (array_of_demand_for_t::const_iterator _it = aAdd.begin();
			_it != aAdd.end(); ++_it)
	{
		bool _is = false;
		if (!_it->FTo.FWhat.FFlags.MGetFlag(demand_dg_t::E_REGISTRATOR))
			_is = MSendPacketFromTo(_it->FFrom, _it->FTo.FFor, _it->FTo.FWhat, aNew,aRemoved);
		else
			_is = MSendPacketFromTo(_it->FFrom, _it->FTo.FFor, _it->FTo.FWhat, NULL, NULL); //does not inform if registrar
		_count = _is ? _count + 1 : _count;
	}
	return _count;
}
/*!\brief Returns list of receivers of messages from
 *
 */
void CRequiredDG::MGetListOfReceiversOfMsgsFrom(id_t const & aFrom,
		array_of_demand_for_t* const aTo) const
{
	DCHECK_NOTNULL(aTo);
	array_of_demand_for_t& _new(*aTo);

	demand_dgs_for_t const & _requr = MGetDemands();

	for (demand_dgs_for_t::const_iterator _it = _requr.begin();
			_it != _requr.end(); ++_it)
	{
		demand_dgs_t::const_iterator _dem_it = _it->second.begin();
		for (; _dem_it != _it->second.end(); ++_dem_it)
		{
			bool const _is_invert = _dem_it->FFlags.MGetFlag(
					demand_dg_t::E_INVERT_GROUP);

			unsigned const _depth = MDoesIdConformTo(aFrom, _dem_it->FNameFrom,
					_is_invert);

			bool const _is_add = _depth != 0;
			if (_is_add)
			{
				VLOG(2) << *_dem_it << " from " << aFrom.FUuid << " is required of by " << _it->first;
				way_info_t  _val;
				_val.FTo.FFor=_it->first.FUuid;
				_val.FTo.FWhat=*_dem_it;
				_val.FTo.FDistance=_depth;
				_val.FFrom=aFrom.FUuid;

				_new.push_back(_val);
			}
		}
	}
}

/*!\brief Adds receivers for messages which will be sent by aFrom
 *
 */
void CRequiredDG::MAddReceiversOfMsgsFrom(id_t const& aFrom, demand_dgs_for_t* aNew,demand_dgs_for_t* aRemoved)
{
	//update protocol_of_uuid_t
	array_of_demand_for_t _new;
	array_of_demand_for_t _removed;

	MGetListOfReceiversOfMsgsFrom(aFrom, &_new);
	MTakeIntoAccountOnlyNearestSendersIfNeed(aFrom.FUuid,&_new, &_removed);
	MUpdateSendInfoFor(aFrom.FUuid, _new,_removed,  aNew, aRemoved);
}

/*!\brief Checks for mathing yhe program id to
 * "regular expression"
 *
 *\param aTestId Id which is tested
 *\param aReqExp regular expression
 *\param isInvertGroup - invert group of id that is
 * 		group delimiting working inside out:
 * 		if 	- aTestId is equal order@com.ru.putin,
 * 			- aReqExp is equal order@com.ru,
 * 			- isInvertGroup is:
 * 		false then it returns false
 * 		true then it returns true
 *
 *\return "match depth" of aTestId in aReqExp or
 * 			0 if no match
 */
unsigned CRequiredDG::MDoesIdConformTo(id_t const& aTestId,
		NSHARE::CProgramName const& aReqExp,bool isInvertGroup) const
{
	VLOG(4) << "Id " << aTestId << " reg " << aReqExp<<" flag = "<<isInvertGroup;
	bool const _is_name = aReqExp.MIsName();

	if (_is_name)
	{
		if(!isInvertGroup)
			return aReqExp.MIsForMe(aTestId.FName);
		else
		{

			NSHARE::CProgramName const _old_name(aTestId.FName);
			NSHARE::CProgramName const _new_req_exp(aReqExp.MGetName(),_old_name.MGetAddress());
			NSHARE::CProgramName const _new_name(_old_name.MGetName(),aReqExp.MGetAddress());

			VLOG(2)<<"Inverting group: old name:"<<aTestId.FName<<", new name:"<<_new_name.MGetRawName()
					<<", old reqular expression:"<<aReqExp.MGetRawName()<<", new expression:"<<_new_req_exp.MGetRawName();

			return _new_req_exp.MIsForMe(_new_name);
		}
	}
	else
	{
		if(!isInvertGroup)
			return aReqExp.MGetAddress().MIsSubGroupOf(aTestId.FName);
		else
		{
			NSHARE::CProgramName const _old_name(aTestId.FName);
			NSHARE::CProgramName const _new_req_exp(aReqExp.MGetName(),_old_name.MGetAddress());
			NSHARE::CProgramName const _new_name(_old_name.MGetName(),aReqExp.MGetAddress());

			VLOG(2)<<"Inverting group: old name:"<<aTestId.FName<<", new name:"<<_new_name.MGetRawName()
					<<", old reqular expression:"<<aReqExp.MGetRawName()<<", new expression:"<<_new_req_exp.MGetRawName();

			return _new_req_exp.MGetAddress().MIsSubGroupOf(_new_name);
		}
	}
}
/*!\brief Filtering uuids by group depth
 *
 */
void  CRequiredDG::MGetOnlyNearestUUIDFor(unique_uuids_t *aTo) const
{
	DCHECK_NOTNULL(aTo);

	unique_uuids_t& _data(*aTo);
	if (!_data.empty())
	{
		typedef std::multimap<unsigned, NSHARE::uuid_t> priority_uuids_t;
		priority_uuids_t _prior;

		{
			unique_uuids_t::const_iterator _it = _data.begin(), _it_end(
					_data.end());
			for (; _it != _it_end; ++_it)
			{
				_prior.insert(
						priority_uuids_t::value_type(_it->second, _it->first));
			}
		}
		{
			_data.clear();
			priority_uuids_t::const_iterator _jt = _prior.begin(), _jt_end(
					_prior.end());
			unsigned const _min_prior = _jt->first;
			for (; _jt != _jt_end && _min_prior == _jt->first; ++_jt)
				_data[_jt->second]=_jt->first;
		}
	}
}

CRequiredDG::unique_uuids_t CRequiredDG::MGetUUIDFor(
		NSHARE::CProgramName const& aName,bool isInvertGroup) const
{
	unique_id_t const& _uuids=MGetUniquieID();

>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD

NSHARE::CConfig CRequiredDG::MSerialize() const
=======
/*!\brief serialize info about messages which
 * are expected by programs
 *
 */
void CRequiredDG::MSerializeMsgExpectedList(NSHARE::CConfig* const aTo) const
>>>>>>> f3da2cc... see changelog.txt
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

<<<<<<< HEAD
		}
=======
NSHARE::CConfig CRequiredDG::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);

	MSerializeMsgExpectedList(&_conf);
	//todo serizize FDGs;
	//todo serialize FMsgChildren

	_conf.MAdd("cur_inner_number",FMsgID);

	return _conf;
}

/*!\brief Reading child of messages
 *
 */
inline void CRequiredDG::MReadMsgChild(msg_inheritances_t * const aTo) const
{
	DCHECK_NOTNULL(aTo);

	msg_inheritances_t& _genealogy(*aTo);
	CParserFactory::factory_its_t _i =
			CParserFactory::sMGetInstance().MGetIterator();
	for (; _i.FBegin != _i.FEnd; ++_i.FBegin)
	{
		IExtParser::inheritances_info_t const _val(
				_i.FBegin->second->MGetInheritances());
		msg_inheritance_tree_t& _current_tree = _genealogy[_i.FBegin->first];
		IExtParser::inheritances_info_t::const_iterator _jt = _val.begin(),
				_jt_end = _val.end();
		for (; _jt != _jt_end; ++_jt)
		{
			msg_family_t& _family = _current_tree[*_jt];
			msg_header_t const _child(_jt->FChildProtcol, _jt->FChildHeader);
			_family.FChildren.push_back(_child);
		}
	}
}
/*!\brief Creates genealogy tree of all messages
 * using info about messages child
 *
 *\param [in] aChildInfo info about messages child
 *\param [out] aChildInfo genealogy tree
 *
 *\return max depth inheritance
 */
inline unsigned CRequiredDG::MCreateGenealogyTreeFromChildInfo(
		msg_inheritances_t * const aChildInfo,msg_inheritances_t * const aTo) const
{

	msg_inheritances_t& _to(*aTo);
	msg_inheritances_t& _child_info(*aChildInfo);

	unsigned _depth=0;
	msg_inheritances_t::iterator _it = _child_info.begin();
	for (; !_child_info.empty() && _depth < FMaxInheritanceDepth;)
	{
		if (_it == _child_info.end())
		{
			//next circle
			_it = _child_info.begin();
			++_depth;
		}

		NSHARE::CText const& _protocol=_it->first;
		msg_inheritance_tree_t & _tree = _it->second;

		msg_inheritance_tree_t::iterator _tit = _tree.begin();
		for (; _tit != _tree.end(); )
		{
			required_header_t const& _current = _tit->first;
			msg_heritance_t& _children = _tit->second.FChildren;

			DCHECK(!_children.empty());
			DCHECK_EQ(_children.size(),1);

			msg_header_t const& _my_child = _children.back();

			msg_heritance_t const& _child_of_my_child = sMGetMessageChildren(
					_my_child.first, _my_child.second, _child_info);

			if (_child_of_my_child.empty())
			{

				{	//copy children of my child to my family tree
					msg_heritance_t const& _children_of_my_child(
							sMGetMessageChildren(_my_child.first,
									_my_child.second, _to));

					if (!_children_of_my_child.empty())
						_children.insert(_children.end(),
								_children_of_my_child.begin(),
								_children_of_my_child.end());
				}
>>>>>>> f3da2cc... see changelog.txt

	}
	//_conf.MAdd(FDGs.MSerialize());
	return _conf;
}
<<<<<<< HEAD
inline void CRequiredDG::MInitializeMsgInheritance()//todo
=======
/*!\brief Fills parent info for message hierarchy
 *
 */
void CRequiredDG::MParentInfo(msg_inheritances_t * const aTo) const
>>>>>>> f3da2cc... see changelog.txt
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
<<<<<<< HEAD
	typedef std::set<required_header_t, CReqHeaderFastLessCompare> msg_children_t;
	typedef std::map<required_header_t, msg_children_t,
			CReqHeaderFastLessCompare> msg_inheritance_tree_t;
=======
}
/*!\brief initialize inherent map
 *
 */
inline void CRequiredDG::MInitializeMsgInheritance()
{
	msg_inheritances_t _childrens,_genealogy;
	MReadMsgChild(&_childrens);
	unsigned const _depth = MCreateGenealogyTreeFromChildInfo(&_childrens,
			&_genealogy);
	DCHECK(_childrens.empty());

	if (sMCheckCorrectionOfGenealogyTree(_genealogy,NULL)==0)
	{
		MParentInfo(&_genealogy);

		DCHECK_EQ(sMCheckCorrectionOfGenealogyTree(_genealogy,NULL),0);

		FMsgsGenealogy.swap(_genealogy);
	}
	else
	{
		///todo change state
	}
}
>>>>>>> f3da2cc... see changelog.txt

	typedef std::map<NSHARE::CText, msg_inheritance_tree_t> msg_inheritances_t;

<<<<<<< HEAD
	msg_inheritances_t _inheritances;
=======
/*!\brief Checks correction of genealogy tree
 *
 *\param aWhat - genealogy tree
 *\param aTo - if not NULL, than here is saved failed heritance
*\return 0 if correct
 *			else amount of errors
 */
unsigned CRequiredDG::sMCheckCorrectionOfGenealogyTree(
		msg_inheritances_t const& aWhat, msg_inheritances_t* aTo)
{
	typedef std::set<msg_heritance_t::value_type, unique_compare_t> unique_heritance_t;

	unsigned _amount_of = 0;

	msg_inheritances_t::const_iterator _it = aWhat.begin(), _it_end(
			aWhat.end());
	for (; _it != _it_end; ++_it)
>>>>>>> f3da2cc... see changelog.txt
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
