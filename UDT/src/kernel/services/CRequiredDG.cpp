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
#include "../core/kernel_type.h"
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
		MRemoveDemandsDG(aFor.FUuid, _rval.second);
		return _rval;
	}
	demand_dgs_t _added;
	demand_dgs_t _removed;
	MGetDiffDemandsDG(aFor, aReqDgs, _removed, _added);
	FDGs[aFor] = aReqDgs;

	if (!_removed.empty())
		MUnSendPacketToFrom(aFor.FUuid, _removed, _rval.second);
	if (!_added.empty())
		MUpdateRequrementFor(_added, aFor, _rval.first);
	return _rval;
}

void CRequiredDG::MSendPacketFromTo(NSHARE::uuid_t const& aFrom,
		NSHARE::uuid_t const& aTo, demand_dg_t const& aWhat,
		demand_dgs_for_t& aNew)
{
	protocol_of_uuid_t::iterator _customer_it = FNeedSendTo.find(aFrom);

	VLOG_IF(2,_customer_it==FNeedSendTo.end()) << "Packets from " << aFrom
														<< " still were not required.";

	if (_customer_it == FNeedSendTo.end())
		_customer_it = FNeedSendTo.insert(_customer_it,
				std::make_pair(aFrom, protocols_t()));

	DCHECK(_customer_it != FNeedSendTo.end());
	if (_customer_it == FNeedSendTo.end())
		return;
	protocols_t& _proto = _customer_it->second;
	VLOG(2) << "Add new customer to " << _customer_it->first;
	protocols_t::iterator _prot_it = _proto.find(aWhat.FProtocol);

	VLOG_IF(2,_prot_it==_proto.end()) << "Packets from '" << aFrom << "' by '"
												<< aWhat.FProtocol
												<< "' protocol still were not required.";
	if (_prot_it == _proto.end())
		_prot_it = _proto.insert(_prot_it,
				std::make_pair(aWhat.FProtocol, uuids_of_expecting_dg_t()));

	DCHECK(_prot_it != _proto.end());
	if (_prot_it == _proto.end())
		return;
	uuids_of_expecting_dg_t& _uuids = _prot_it->second;
	uuids_of_expecting_dg_t::iterator _exp_it = _uuids.find(aWhat.FWhat);

	VLOG_IF(2,_exp_it==_uuids.end())
											<< "The Packet has not been  received of yet.";

	if (_exp_it == _uuids.end())
		_exp_it = _uuids.insert(_exp_it,
				std::make_pair(aWhat.FWhat, unique_uuids_t()));

	CHECK(_exp_it != _uuids.end());

	bool const _is = _exp_it->second.insert(aTo).second;
	LOG_IF(INFO,_is) << "Now The packet:" << aWhat << " from " << aFrom
								<< " is received of by " << aTo;
	LOG_IF(INFO,!_is) << "WTF? The packet has been received twice of by " << aTo;

	if (_is)
	{
		demand_dg_t _new(aWhat);
		_new.FUUIDFrom.MSet(aTo);
		aNew[id_t(aFrom)].push_back(_new);
	}
}
void CRequiredDG::MUnSendPacketToFrom(NSHARE::uuid_t const& aTo,
		demand_dgs_t const& aFrom, demand_dgs_for_t & aOld)
{
//	unique_uuids_t _res;
	std::set<id_t>::const_iterator _it = FIds.begin(), _it_end(FIds.end());
	for (; _it != _it_end; ++_it)
	{
		id_t const& _id = *_it;
		VLOG(4) << "Handle " << _id;
		demand_dgs_t::const_iterator _jt = aFrom.begin(), _jt_end(aFrom.end());
		for (; _jt != _jt_end; ++_jt)
		{
			NSHARE::CRegistration const& _reg = _jt->FNameFrom;
			VLOG(4) << "Reg " << _reg;
			if (MDoesIdConformTo(_id, _reg))
			{
				VLOG(2) << "Is for me " << _reg << " id: " << _id;

				protocol_of_uuid_t::iterator _customer_it = FNeedSendTo.find(
						_id.FUuid);

				DCHECK(_customer_it != FNeedSendTo.end());

				if (_customer_it != FNeedSendTo.end()) //if send some DG to uuid
				{
					NSHARE::CText const& _protocol=_jt->FProtocol;
					VLOG(3) <<"Protocol:" <<_protocol;

					protocols_t::iterator _prot_it = _customer_it->second.find(
							_protocol);
					if (_prot_it != _customer_it->second.end()) //there is the protocol
					{
						required_header_t const& _head=_jt->FWhat;
						VLOG(3) << "The protocol is exist Header= "<<_head;

						//looking for header
						uuids_of_expecting_dg_t& _uuids = _prot_it->second;
						uuids_of_expecting_dg_t::iterator _exp_it = _uuids.find(
								_head);
						if (_exp_it != _uuids.end())
						{
							VLOG(3)<<"Head is found ";
							//looking for our UUID
							unique_uuids_t::iterator _list_it =
									_exp_it->second.find(aTo);

							if (_list_it != _exp_it->second.end())
							{
								LOG(INFO)<<"Now The packet:"<<_exp_it->first<<" from "<<_customer_it->first<<" is not received of by "<<aTo;
								demand_dg_t _tmp_dem(*_jt);
								_tmp_dem.FUUIDFrom.MSet(aTo);
								aOld[id_t( _customer_it->first)].push_back(_tmp_dem);
								_exp_it->second.erase(_list_it);
								break;
							}
						}
					}

				}
			}
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
bool CRequiredDG::MRemoveDemandsDG(NSHARE::uuid_t const& aUUID,
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

		MUnSendPacketToFrom(aUUID, _old, aOld);
		return true;
	}
	return false;
}
demand_dgs_for_t CRequiredDG::MRemoveClient(NSHARE::uuid_t const& aUUID)
{
	demand_dgs_for_t _old;
	VLOG(2) << "Remove " << aUUID;

	if (size_t _val = FIds.erase(id_t(aUUID)))
	{
		VLOG(2) << "Erased " << _val;
		CHECK_EQ(_val, 1);
		MRemoveDemandsDG(aUUID, _old);
		FNeedSendTo.erase(aUUID);
	}
	return _old;
}
NSHARE::smart_field_t<uuids_t> CRequiredDG::MGetCustomersFor(
		NSHARE::uuid_t const& aFor, unsigned const aNumber) const
{
	NSHARE::smart_field_t<uuids_t> _rval;
	VLOG(2) << "Get uuids for " << aFor << " by " << RAW_PROTOCOL_NAME;
	protocol_of_uuid_t::const_iterator _it = FNeedSendTo.find(aFor);
	VLOG_IF(2, _it == FNeedSendTo.end()) << "There is not customers for "
												<< aFor;
	if (_it == FNeedSendTo.end())
		return _rval;
	protocols_t::const_iterator _prot_it = _it->second.find(RAW_PROTOCOL_NAME);
	LOG_IF(INFO,_prot_it==_it->second.end()) << "Nobody expects of data from '"
														<< _it->first
														<< "' by  '"
														<< RAW_PROTOCOL_NAME
														<< "' protocol.";
	if (_prot_it == _it->second.end())
		return _rval;

	VLOG(2) << "Receive by raw protocol.";
	required_header_t _header;
	_header.FNumber = aNumber;

	uuids_of_expecting_dg_t::const_iterator _jt = _prot_it->second.find(
			_header);
	if (_jt != _prot_it->second.end())
	{
		for (unique_uuids_t::const_iterator _kt = _jt->second.begin();
				_kt != _jt->second.end(); ++_kt)
			_rval.MGet().push_back(*_kt);
	}
	return _rval;
}

NSHARE::smart_field_t<uuids_t> CRequiredDG::MGetCustomersFor(
		NSHARE::uuid_t const& aFor, NSHARE::CText const& aProtocol,
		void const* aData, unsigned const aSize) const
{
	if (aProtocol == RAW_PROTOCOL_NAME)
		return MGetCustomersFor(aFor, 0);

	NSHARE::smart_field_t<uuids_t> _rval;
	VLOG(2) << "Get uuids for " << aFor;
	protocol_of_uuid_t::const_iterator _it = FNeedSendTo.find(aFor);
	VLOG_IF(2, _it == FNeedSendTo.end()) << "There is not customers for "
												<< aFor;
	if (_it == FNeedSendTo.end())
		return _rval;
	protocols_t::const_iterator _prot_it = _it->second.find(aProtocol);
	LOG_IF(INFO,_prot_it==_it->second.end()) << "Nobody expects of data from '"
														<< _it->first
														<< "' by  '"
														<< aProtocol
														<< "' protocol.";
	if (_prot_it == _it->second.end())
		return _rval;

	IExtParser* _p = CParserFactory::sMGetInstance().MGetFactory(aProtocol);
	LOG_IF(DFATAL,!_p) << "Parsing module for " << aProtocol
								<< " is not exist";
	if (!_p || aSize == 0)
		return _rval;
	IExtParser::result_t _result = _p->MParserData((const uint8_t*) aData,
			(const uint8_t*) aData + aSize);
	VLOG(1) << "Founded " << _result.size() << " dg.";
	CHECK(_result.size() == 1);
	uuids_of_expecting_dg_t::const_iterator _jt = _prot_it->second.find(
			_result.front().FType);

	LOG_IF(INFO, _jt==_prot_it->second.end()) << "Packet "
														<< _p->MToConfig(
																_result.front().FType).MToJSON(
																true)
														<< " from " << aFor
														<< " does not required. Ignoring ...";

	if (_jt != _prot_it->second.end())
		for (unique_uuids_t::const_iterator _kt = _jt->second.begin();
				_kt != _jt->second.end(); ++_kt)
			_rval.MGet().push_back(*_kt);

	return _rval;
}

void CRequiredDG::MUpdateRequrementFor(const demand_dgs_t& aAdded,
		const id_t& aFor, demand_dgs_for_t& aNew)
{
	//update protocol_of_uuid_t
	//1) to find DGs which are required to aFor from existing UUID
	//2) to add all demands to protocol_of_uuid_t
	demand_dgs_t::const_iterator _req_it = aAdded.begin();
	for (; _req_it != aAdded.end(); ++_req_it)
	{
		VLOG(2) << "Try received " << *_req_it;
		unique_uuids_t _uuids = MGetUUIDFor(_req_it->FNameFrom);

		//VLOG(2) << " Founded uuids: " << _uuids;

		unique_uuids_t::iterator _uuid_it = _uuids.begin();
		for (; _uuid_it != _uuids.end(); ++_uuid_it)
			MSendPacketFromTo(*_uuid_it, aFor.FUuid, *_req_it, aNew);
	}
}
void CRequiredDG::MUpdateRecipientOf(id_t const& aUId, demand_dgs_for_t& aNew)
{
	//update protocol_of_uuid_t
	//1) to find DGs which are required from aUUID
	//2) to add all demands to protocol_of_uuid_t

	typedef std::vector<std::pair<id_t, demand_dg_t> > dgs_t;
	dgs_t _demands;
	for (demand_dgs_for_t::const_iterator _it = FDGs.begin(); _it != FDGs.end();
			++_it)
	{
		demand_dgs_t::const_iterator _dem_it = _it->second.begin();
		for (; _dem_it != _it->second.end(); ++_dem_it)
		{
			if(MDoesIdConformTo(aUId,_dem_it->FNameFrom))
			{
				VLOG(2) << *_dem_it << " from " << aUId.FUuid
									<< " is required of by " << _it->first;
				std::pair<id_t, demand_dg_t> _val(_it->first, *_dem_it);
				_demands.push_back(_val);
			}
		}
	}
	for (dgs_t::const_iterator _it = _demands.begin(); _it != _demands.end();
			++_it)
		MSendPacketFromTo(aUId.FUuid, _it->first.FUuid, _it->second, aNew);
}
bool CRequiredDG::MDoesIdConformTo(id_t const& _id,
		NSHARE::CRegistration const& _reg) const
{
	VLOG(4)<<"Id "<<_id<<" reg "<<_reg;
	bool const _is_name = _reg.MIsName();
	VLOG(4)<<"Is name ="<<_is_name;
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
	VLOG(4)<<" Size="<<FIds.size();
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
demand_dgs_for_t CRequiredDG::MAddClient(id_t const& aId)
{
	VLOG(2)<<"Add new client "<<aId;
	const bool _is=FIds.insert(aId).second;
	CHECK(_is);
	demand_dgs_for_t _new;
	MUpdateRecipientOf(aId, _new);
	return _new;
}

NSHARE::CConfig CRequiredDG::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	protocol_of_uuid_t::const_iterator _it = FNeedSendTo.begin(), _it_end(
			FNeedSendTo.end());

	for (; _it != _it_end; ++_it)
	{
		protocols_t::const_iterator _prot_it = _it->second.begin(),
				_prot_it_end(_it->second.end());
		for (; _prot_it != _prot_it_end; ++_prot_it)

		{
			NSHARE::CConfig _prot("demand");
			_prot.MAdd("from", _it->first.MSerialize());

			_prot.MAdd(user_data_info_t::KEY_PACKET_PROTOCOL, _prot_it->first);

			IExtParser* _p = CParserFactory::sMGetInstance().MGetFactory(
					_prot_it->first);

			uuids_of_expecting_dg_t::const_iterator _jt =
					_prot_it->second.begin(), _jt_end(_prot_it->second.end());
			for (; _jt != _jt_end; ++_jt)
			{
				NSHARE::CConfig _dg("dg");
				if (_p)
					_dg.MAdd(_p->MToConfig(_jt->first));
				else
				{
					_dg.MAdd("raw", _jt->first.MSerialize());
				}

				for (unique_uuids_t::const_iterator _kt = _jt->second.begin();
						_kt != _jt->second.end(); ++_kt)
				{
					_dg.MAdd(user_data_info_t::KEY_PACKET_TO,
							_kt->MSerialize());
				}
				_conf.MAdd(_dg);
			}
			_conf.MAdd(_prot);
		}

	}
	return _conf;
}
} /* namespace NUDT */
