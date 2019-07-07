// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * shared_types.cpp
 *
 *  Created on: 30.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <shared_types.h>
#include <CParserFactory.h>
//#include <udt_share.h>
#include <string.h>

using namespace NSHARE;
namespace NUDT
{
extern UDT_SHARE_EXPORT error_type const USER_ERROR_MASK = std::numeric_limits<
		error_type>::max() << eUserErrorStartBits;

error_type code_inner_error(error_type const& aError)
{
	return aError;
}

error_type encode_inner_error(error_type const& aError)
{
	return aError & (~USER_ERROR_MASK);
}
user_error_type encode_user_error(error_type const& aError)
{
	error_type const _error = ( aError& USER_ERROR_MASK)
			>> eUserErrorStartBits;
	CHECK_LE(_error, std::numeric_limits<user_error_type>::max());
	return static_cast<user_error_type>(_error);
}
error_type code_user_error(user_error_type const& aError)
{
	const error_type _user = aError;
	error_type const _error = (_user << eUserErrorStartBits)
			| E_USER_ERROR_BEGIN;
	return _error;
}

//---------------------------------------
const NSHARE::CText required_header_t::NAME = "rh";
const NSHARE::CText required_header_t::KEY_HEADER = "buf";
//---------------------------------------
const NSHARE::CText id_t::NAME = "id";
const NSHARE::CText id_t::KEY_NAME = "n";

const NSHARE::CText program_id_t::NAME = "info";
const NSHARE::CText program_id_t::KEY_TIME = "tm";
const NSHARE::CText program_id_t::KEY_PID = "pid";
const NSHARE::CText program_id_t::KEY_PATH = "path";
const NSHARE::CText program_id_t::KEY_TYPE = "type";
const NSHARE::CText program_id_t::KEY_ENDIAN = "edn";
//---------------------------
const NSHARE::CText uuids_t::NAME = "uuids";
uuids_t::uuids_t(NSHARE::CConfig const& aConf)
{
	VLOG(2) << "Create uuids_t from " << aConf;

	ConfigSet _set = aConf.MChildren(NSHARE::uuid_t::NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(4) << "Push uuid " << *_it;
		push_back(NSHARE::uuid_t(*_it));
	}
}
NSHARE::CConfig uuids_t::MSerialize() const
{
	CConfig _conf(NAME);

	if (!empty())
	{
		const_iterator _it = begin(), _it_end = end();
		for (; _it != _it_end; ++_it)
		{
			_conf.MAdd(_it->MSerialize());
		}
	}
	return _conf;
}
bool uuids_t::MIsValid() const
{
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		if (!_it->MIsValid())
			return false;
	}
	return true;
}
//---------------------------
const NSHARE::CText split_packet_t::NAME = "spt";
const NSHARE::CText split_packet_t::LAST = "last";
const NSHARE::CText split_packet_t::COUNTER = "ct";
const NSHARE::CText split_packet_t::COEFFICIENT = "k";
split_packet_t::split_packet_t(NSHARE::CConfig const& aConf) :
		FIsLast(false), //
		FCounter(0), //
		FCoefficient(0)
{
	VLOG(2) << "Create split_packet_t from " << aConf;
	aConf.MGetIfSet(LAST, FIsLast);
	aConf.MGetIfSet(COUNTER, FCounter);
	aConf.MGetIfSet(COEFFICIENT, FCoefficient);
}
NSHARE::CConfig split_packet_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet(LAST, FIsLast);
	_conf.MSet(COUNTER, FCounter);
	_conf.MSet<unsigned>(COEFFICIENT, FCoefficient);
	return _conf;
}
bool split_packet_t::MIsValid() const
{
	return FCounter > 0 && FCoefficient > 0;
}
bool split_packet_t::MIsSplited() const
{
	return !(FCounter == 1 && FCoefficient == 1 && FIsLast);
}
size_t split_packet_t::MGetAlignmentSize(size_t const& aSize) const
{
	size_t const _size =
			((aSize % FCoefficient == 0) || FCoefficient == 1) ?
					aSize : (aSize / FCoefficient + 1) * FCoefficient;
	return _size;
}
//---------------------------
const NSHARE::CText user_data_info_t::NAME = "usdt";
const NSHARE::CText user_data_info_t::KEY_PACKET_NUMBER = "pn";
//const NSHARE::CText user_data_info_t::KEY_PACKET_FROM = "f";
const NSHARE::CText user_data_info_t::KEY_PACKET_FROM = id_t::NAME;
const NSHARE::CText user_data_info_t::KEY_PACKET_TO = "to";
const NSHARE::CText user_data_info_t::KEY_REGISTRATORS = "reg";

const NSHARE::CText user_data_info_t::KEY_PACKET_PROTOCOL = "pl";
const NSHARE::CText user_data_info_t::KEY_DATA_OFFSET = "nr";
const NSHARE::CText user_data_info_t::KEY_DATA_ENDIAN = "endian";

user_data_info_t::user_data_info_t(NSHARE::CConfig const& aConf) :
		//FFrom(aConf.MChild(KEY_PACKET_FROM)),//
		FDataOffset(0), //
		FSplit(aConf.MChild(split_packet_t::NAME)), //
		FDestination(aConf.MChild(uuids_t::NAME)), //
		FRegistrators(aConf.MChild(KEY_REGISTRATORS)), //
		FRouting(aConf.MChild(routing_t::NAME)), //
		FEndian(NSHARE::E_SHARE_ENDIAN)
{
	VLOG(2) << "Create user_data_info_t from " << aConf;
	aConf.MGetIfSet(KEY_PACKET_NUMBER, FPacketNumber);
	aConf.MGetIfSet(KEY_PACKET_PROTOCOL, FProtocol);
	aConf.MGetIfSet(KEY_DATA_OFFSET, FDataOffset);
	unsigned _val=0;
	if(aConf.MGetIfSet<unsigned>(KEY_DATA_ENDIAN, _val))
		FEndian=(NSHARE::eEndian)_val;

	ConfigSet _set = aConf.MChildren(demand_dg_t::HANDLER);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push handler " << *_it;
		FEventsList.push_back(
				_it->MValue(
						demand_dg_t::event_handler_t(demand_dg_t::NO_HANDLER)));
	}

	std::pair<required_header_t, bool> const _val2 = parse_head(aConf,
			FProtocol);
	if (_val2.second)
	{
		FWhat = _val2.first;
	}
}
NSHARE::CConfig user_data_info_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet(KEY_PACKET_NUMBER, FPacketNumber);
	_conf.MSet(KEY_PACKET_PROTOCOL, FProtocol);
	//_conf.MAdd(KEY_PACKET_FROM, FFrom.MSerialize());
	_conf.MAdd(FSplit.MSerialize());

	_conf.MAdd(serialize_head(FWhat, ""));

	if (!FDestination.empty())
	{
		_conf.MAdd(FDestination.MSerialize());
	}
	if (!FRegistrators.empty())
	{
		_conf.MAdd(KEY_REGISTRATORS, FRegistrators.MSerialize());
	}
	if (!FEventsList.empty())
	{
		std::vector<NUDT::demand_dg_t::event_handler_t>::const_iterator _it =
				FEventsList.begin(), _it_end = FEventsList.end();
		for (; _it != _it_end; ++_it)
		{
			_conf.MAdd(demand_dg_t::HANDLER, *_it);
		}
	}
	_conf.MAdd(routing_t::NAME, FRouting.MSerialize());

	_conf.MSet(KEY_DATA_OFFSET, FDataOffset);
	_conf.MSet<unsigned>(KEY_DATA_ENDIAN, FEndian);

	return _conf;
}
bool user_data_info_t::MIsMsgExist() const
{
	static const uint8_t _fix_buf[sizeof(FWhat.FMessageHeader)]={0};

	return memcmp(FWhat.FMessageHeader,_fix_buf,sizeof(FWhat.FMessageHeader))!=0;
}
bool user_data_info_t::MIsRaw() const
{
	return FProtocol.empty() || FProtocol == RAW_PROTOCOL_NAME;
}
bool user_data_info_t::MIsValid() const
{
	return !FProtocol.empty();
}
const NSHARE::CText kernel_link::NAME = "klink";
const NSHARE::CText kernel_link::LATENCY = "klat";
const NSHARE::CText kernel_link::TIME = "ctime";
const NSHARE::CText kernel_link::LINK = "link";
kernel_link::kernel_link(NSHARE::CConfig const& aConf) :
		FProgramm(aConf.MChild(program_id_t::NAME)), //
		FConnectTime(NSHARE::get_unix_time()), FLatency(
				std::numeric_limits<uint16_t>::max()) //
{
	aConf.MGetIfSet(LATENCY, FLatency);
	aConf.MGetIfSet(TIME, FConnectTime);
	aConf.MGetIfSet(LINK, FTypeLink);
}
kernel_link::kernel_link(program_id_t const& aInfo, uint16_t aLink) :
		FProgramm(aInfo), //
		FConnectTime(NSHARE::get_unix_time()), //
		FLatency(aLink) //
{
	;
}

NSHARE::CConfig kernel_link::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);

	_conf.MAdd(LATENCY, FLatency);
	_conf.MAdd(TIME, FConnectTime);
	_conf.MAdd(FProgramm.MSerialize());
	_conf.MAdd(LINK, FTypeLink);
	return _conf;
}
bool kernel_link::MIsValid() const
{
	return FProgramm.MIsValid()
			&& FLatency != std::numeric_limits<unsigned>::max();
}

const NSHARE::CText kernel_infos_t::NAME = "kinf";
const NSHARE::CText kernel_infos_t::CLIENT = kernel_link::NAME;
const NSHARE::CText kernel_infos_t::NUMBER = "indx";

kernel_infos_t::kernel_infos_t(NSHARE::CConfig const& aConf) :
		FKernelInfo(aConf.MChild(program_id_t::NAME)), //
		FIndexNumber(0) //
{
	VLOG(2) << "Create kernel_infos_t from " << aConf;
	aConf.MGetIfSet(NUMBER, FIndexNumber);

	ConfigSet _set = aConf.MChildren(CLIENT);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(4) << "Push info " << *_it;
		FCustomerInfo.insert(kernel_link(*_it));
	}
}
NSHARE::CConfig kernel_infos_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(FKernelInfo.MSerialize());
	_conf.MAdd(NUMBER, FIndexNumber);

	if (!FCustomerInfo.empty())
	{
		kern_links_t::const_iterator _it = FCustomerInfo.begin(), _it_end =
				FCustomerInfo.end();
		for (; _it != _it_end; ++_it)
		{
			_conf.MAdd(CLIENT, _it->MSerialize());
		}
	}
	return _conf;
}
bool kernel_infos_t::MIsValid() const
{
	if (!FKernelInfo.MIsValid())
		return false;
	kern_links_t::const_iterator _it(FCustomerInfo.begin()), _end(
			FCustomerInfo.end());
	for (; _it != _end; ++_it)
	{
		if (!_it->MIsValid())
			return false;
	}
	return true;
}
//---------------------------
const NSHARE::CText demand_dg_t::NAME = "dem";
const NSHARE::CText demand_dg_t::HANDLER = "hand";
const NSHARE::CText demand_dg_t::KEY_HANDLER_PRIORITY = "hand_priority";
const NSHARE::CText demand_dg_t::KEY_FLAGS = "dflag";

const demand_dg_t::event_handler_t demand_dg_t::NO_HANDLER = static_cast<event_handler_t>(-1);
const demand_dg_t::handler_priority_t demand_dg_t::HANDLER_DEFAULT_PRIORITY=std::numeric_limits<handler_priority_t>::max()/2;
extern std::pair<required_header_t, bool> UDT_SHARE_EXPORT parse_head(
		NSHARE::CConfig const& aConf, NSHARE::CText const& _proto)
{
	if (!aConf.MIsChild("rh"))
	{
		if (IExtParser* _p = CParserFactory::sMGetInstance().MGetFactory(
				_proto))
		{
			try
			{
				VLOG(2) << aConf.MChild(_proto).MToJSON(true);
				return _p->MHeader(aConf.MChild(_proto));
			} catch (...)
			{
				LOG(DFATAL)<<"No  header.	User fail "<<_proto;
				return std::make_pair(required_header_t(),false);
			}
		}
		else
		{
			LOG(DFATAL)<<"No  header. No key 'rh' and no parser for "<<_proto;
			return std::make_pair(required_header_t(),false);
		}
	}
	else
	return std::make_pair(required_header_t(aConf.MChild("rh")),true);
}
extern NSHARE::CConfig UDT_SHARE_EXPORT serialize_head(
		required_header_t const& aWhat, NSHARE::CText const& _proto)
{
	if (_proto.empty())
		return aWhat.MSerialize();
	else
	{
		if (IExtParser* _p = CParserFactory::sMGetInstance().MGetFactory(
				_proto))
		{
			try
			{
				NSHARE::CConfig const _is = _p->MToConfig(aWhat);
				if (!_is.MIsEmpty())
				{
					if (_is.MKey() == _proto)
						return _is;
					else
						return NSHARE::CConfig(_proto, _is);
				}
			} catch (...)
			{
				LOG(DFATAL) << "Unknown error";
			}
		}
		LOG(DFATAL)<<"Cannot serialize head by custom protocol: "<<_proto;
		return aWhat.MSerialize();
	}
}
demand_dg_t::demand_dg_t(NSHARE::CConfig const& aConf) :
		FHandlerPriority(HANDLER_DEFAULT_PRIORITY),//
		FNameFrom(aConf.MChild(user_data_info_t::KEY_PACKET_FROM)), //
		FHandler(NO_HANDLER), //
		FFlags(aConf.MValue < flags_t > (KEY_FLAGS, E_DEMAND_DEFAULT_FLAGS))
{
	VLOG(2) << "Create demand_dg_t from " << aConf;
	if (aConf.MGetIfSet(user_data_info_t::KEY_PACKET_PROTOCOL, FProtocol))
	{
		if (FProtocol.empty())
			FProtocol = RAW_PROTOCOL_NAME;
	}
	std::pair<required_header_t, bool> const _val = parse_head(aConf,
			FProtocol);
	if (!_val.second)
		FProtocol.clear(); //Head is not checking in MIsValid(), Therefore corrupting the other option
	else
	{
		FWhat = _val.first;
		aConf.MGetIfSet(HANDLER, FHandler);
		aConf.MGetIfSet(KEY_HANDLER_PRIORITY, FHandlerPriority);

		NSHARE::CConfig const & _set = aConf.MChild(NSHARE::uuid_t::NAME);
		if (!_set.MIsEmpty())
		{
			FUUIDFrom.MSet(NSHARE::uuid_t(_set));
		}
	}
}

NSHARE::CConfig demand_dg_t::MSerialize(bool aIsSerializeHeadAsRaw) const
{
	CConfig _conf(NAME);
	NSHARE::CText _protcol = FProtocol.empty() ? RAW_PROTOCOL_NAME : FProtocol;

	_conf.MAdd(serialize_head(FWhat, aIsSerializeHeadAsRaw ? "" : FProtocol));

	_conf.MAdd(user_data_info_t::KEY_PACKET_FROM, FNameFrom.MSerialize());
	_conf.MAdd(user_data_info_t::KEY_PACKET_PROTOCOL, FProtocol);
	_conf.MAdd(HANDLER, FHandler);
	_conf.MAdd(KEY_HANDLER_PRIORITY, FHandlerPriority);
	_conf.MAdd(KEY_FLAGS, FFlags.MGetMask());

	if (FUUIDFrom.MIs())
	{
		_conf.MAdd(FUUIDFrom.MGetConst().MSerialize());
	}

	return _conf;
}
bool demand_dg_t::MIsValid() const
{
	return !FProtocol.empty() && (FNameFrom.MIsValid() || FUUIDFrom.MIs())
			&& FHandler != NO_HANDLER;
}
void demand_dg_t::MSwitchEndianFlag()
{
	if(FFlags.MGetFlag(E_IS_BIG_ENDIAN))
		FFlags.MSetFlag(E_IS_BIG_ENDIAN,false);
	else
		FFlags.MSetFlag(E_IS_BIG_ENDIAN,true);

}
bool demand_dg_t::MIsValidEndian() const
{
	eEndian const _endian =
			FFlags.MGetFlag(E_IS_BIG_ENDIAN) ?
					E_SHARE_BIG_ENDIAN : E_SHARE_LITTLE_ENDIAN;
	return _endian == E_SHARE_ENDIAN;
}
bool demand_dg_t::MIsEqual(demand_dg_t const& aRht) const
{
	return FProtocol == aRht.FProtocol && //
			FNameFrom == aRht.FNameFrom && //
			memcmp(FWhat.FMessageHeader, aRht.FWhat.FMessageHeader,
					sizeof(FWhat.FMessageHeader)) == 0;
}
bool demand_dg_t::operator==(demand_dg_t const& aRht) const
{
	return MIsEqual(aRht) && FUUIDFrom == aRht.FUUIDFrom;
}
//---------------------------
const NSHARE::CText demand_dgs_t::NAME = "dems";
demand_dgs_t::demand_dgs_t(NSHARE::CConfig const& aConf)
{
	VLOG(2) << "Create demand_dgs_t from " << aConf;
	ConfigSet _set = aConf.MChildren(demand_dg_t::NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push info " << *_it;
		push_back(demand_dg_t(*_it));
	}
}
NSHARE::CConfig demand_dgs_t::MSerialize(bool aIsSerializeHeadAsRaw) const
{
	CConfig _conf(NAME);
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		_conf.MAdd(_it->MSerialize(aIsSerializeHeadAsRaw));
	}

	return _conf;
}
bool demand_dgs_t::MIsValid() const
{
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		if (!_it->MIsValid())
			return false;
	}
	return true;
}
//---------------------------
const NSHARE::CText kernel_infos_array_t::NAME = "kinfs";
const NSHARE::CText kernel_infos_array_t::NUMBER_OF_CHANGE = "numc";

kernel_infos_array_t::kernel_infos_array_t(NSHARE::CConfig const& aConf):FNumberOfChange(0)
{
	VLOG(2) << "Create kernel info from " << aConf;
	aConf.MGetIfSet(NUMBER_OF_CHANGE, FNumberOfChange);
	ConfigSet _set = aConf.MChildren(kernel_infos_t::NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push kernel info " << *_it;
		insert(kernel_infos_t(*_it));
	}
}
NSHARE::CConfig kernel_infos_array_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet (NUMBER_OF_CHANGE, FNumberOfChange);
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		_conf.MAdd(_it->MSerialize());
	}

	return _conf;
}
kernel_list_t& kernel_infos_array_t::MVec()
{
	return *this;
}
bool kernel_infos_array_t::MIsValid() const
{
	if(FNumberOfChange==0)
		return false;
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		if (!_it->MIsValid())
			return false;
	}
	return true;
}
//-----------------
const NSHARE::CText kernel_infos_diff_t::NAME = "kidif";
const NSHARE::CText kernel_infos_diff_t::OPENED_KERNELS = "opened";
const NSHARE::CText kernel_infos_diff_t::CLOSED_KERNELS = "closed";

NSHARE::CConfig kernel_infos_diff_t::MSerialize() const
{
	CConfig _conf(NAME);
	if (!FOpened.empty())
	{
		_conf.MAdd(OPENED_KERNELS, CConfig());
		CConfig* _new = _conf.MMutableChild(OPENED_KERNELS);
		CHECK_NOTNULL(_new);
		k_diff_t::const_iterator _it = FOpened.begin(), _it_end = FOpened.end();
		for (; _it != _it_end; ++_it)
		{
			NSHARE::CConfig _con(_it->first.MSerialize());
			_con.MAdd("kop", _it->second);
			_new->MAdd(_con);
		}
	}
	if (!FClosed.empty())
	{
		_conf.MAdd(CLOSED_KERNELS, CConfig());
		CConfig* _new = _conf.MMutableChild(CLOSED_KERNELS);
		CHECK_NOTNULL(_new);
		k_diff_t::const_iterator _it = FClosed.begin(), _it_end = FClosed.end();
		for (; _it != _it_end; ++_it)
		{
			NSHARE::CConfig _con(_it->first.MSerialize());
			_con.MAdd("kcl", _it->second);
			_new->MAdd(_con);
		}
	}
	return _conf;
}

bool kernel_infos_diff_t::MIsValid() const
{
	return !FOpened.empty() || !FClosed.empty();
}
//-----------------
const NSHARE::CText demand_dgs_for_t::NAME = "demf";
demand_dgs_for_t::demand_dgs_for_t(NSHARE::CConfig const& aConf)
{
	VLOG(2) << "Create demands for info from " << aConf;
	ConfigSet _set = aConf.MChildren(id_t::NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push kernel info " << *_it;
		insert(
				value_type(id_t(*_it),
						demand_dgs_t((_it)->MChild(demand_dgs_t::NAME))));
	}
}

NSHARE::CConfig demand_dgs_for_t::MSerialize() const
{
	CConfig _conf(NAME);
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		CConfig _id(_it->first.MSerialize());
		_id.MAdd(_it->second.MSerialize());
		_conf.MAdd(_id);
	}

	return _conf;
}

bool demand_dgs_for_t::MIsValid() const
{
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		if (!_it->first.MIsValid() || !_it->second.MIsValid())
			return false;
	}
	return true;
}
//-----------------
const NSHARE::CText error_info_t::NAME = "err_";
//const NSHARE::CText error_info_t::WHERE = "who";
const NSHARE::CText error_info_t::WHERE = id_t::NAME;
const NSHARE::CText error_info_t::CODE = "code";
error_info_t::error_info_t() :
		FError(E_NO_ERROR)
{
	;
}
error_info_t::error_info_t(NSHARE::CConfig const& aConf) :
		FError(
				static_cast<eErrorBitwiseCode>(aConf.MValue < error_type
						> (CODE, E_NO_ERROR))), //
		FWhere(FError != E_NO_ERROR ? id_t(aConf.MChild(WHERE)) : id_t()), //
		FTo(
				FError != E_NO_ERROR ?
						uuids_t(aConf.MChild(uuids_t::NAME)) : uuids_t())
{
	VLOG(2) << "Create error from " << aConf;

}
NSHARE::CConfig error_info_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet < error_type > (CODE, FError);
	if (MIsValid())
	{
		_conf.MAdd(WHERE, FWhere.MSerialize());
		_conf.MAdd(FTo.MSerialize());
	}
	return _conf;
}
bool error_info_t::MIsValid() const
{
	return FError != E_NO_ERROR;
}

//-----------------
const NSHARE::CText routing_t::NAME = "rtg_";
const NSHARE::CText routing_t::FROM = user_data_info_t::KEY_PACKET_FROM;

routing_t::routing_t(NSHARE::CConfig const& aConf) :
		uuids_t(aConf.MChild(uuids_t::NAME)), FFrom(aConf.MChild(FROM))
{
	;
}
NSHARE::CConfig routing_t::MSerialize() const
{
	CConfig _conf(NAME);
	if (MIsValid())
	{
		_conf.MAdd(FROM, FFrom.MSerialize());
		_conf.MAdd(uuids_t::MSerialize());
	}
//	else
//		DCHECK(empty());
	return _conf;
}
bool routing_t::MIsValid() const
{
	bool const _is = FFrom.MIsValid();
	if (_is)
		return uuids_t::MIsValid();
	else
		DCHECK(empty());
	return _is;
}

const NSHARE::CText progs_id_t::NAME = "pids";

progs_id_t::progs_id_t(NSHARE::CConfig const& aConf)
{
	VLOG(2) << "Create progs_id_t from " << aConf;
	ConfigSet _set = aConf.MChildren(program_id_t::NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push info " << *_it;
		insert(program_id_t(*_it));
	}
}
NSHARE::CConfig progs_id_t::MSerialize() const
{
	CConfig _conf(NAME);
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		_conf.MAdd(_it->MSerialize());
	}

	return _conf;
}
bool progs_id_t::MIsValid() const
{
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		if (!_it->MIsValid())
			return false;
	}
	return true;
}

const NSHARE::CText fail_send_t::NAME = "fsend";
const NSHARE::CText fail_send_t::CODE = "ecode";

fail_send_t::fail_send_t(user_data_info_t const& aRht) :
		user_data_info_t(aRht), //
		FError(E_NO_ERROR)
{
}
fail_send_t::fail_send_t(user_data_info_t const& aRht, const uuids_t& aTo,
		error_type aError) :
		user_data_info_t(aRht)
{
	MSetError(aError);
	FRouting.clear();
	FRouting.insert(FRouting.end(), aTo.begin(), aTo.end());
}
fail_send_t::fail_send_t(NSHARE::CConfig const& aConf) :
		user_data_info_t(aConf.MChild(user_data_info_t::NAME)), //
		FError(aConf.MValue < error_type > (CODE, E_NO_ERROR))
{
	VLOG(5) << "Result: " << *this;
}
NSHARE::CConfig fail_send_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(user_data_info_t::MSerialize());
	_conf.MSet(CODE, FError.MGetMask());
	return _conf;
}
bool fail_send_t::MIsError() const
{
	return FError.MGetMask() == E_NO_ERROR;
}
bool fail_send_t::MIsValid() const
{
	return user_data_info_t::MIsValid();
}
error_type fail_send_t::MGetInnerError() const
{
	return encode_inner_error(FError.MGetMask());
}
user_error_type fail_send_t::MGetUserError() const
{
	if (!FError.MGetFlag(E_USER_ERROR_BEGIN))
		return E_NO_ERROR;
	else
	{
		return encode_user_error(FError.MGetMask());
	}
}
void fail_send_t::MSetUserError(user_error_type aError)
{
	FError.MSetFlag(code_user_error(aError), true);
}
void fail_send_t::MSetError(error_type aError)
{
	FError.MSetFlag(code_inner_error(aError), true);
}
const NSHARE::CText user_data_t::NAME = "udata";
const NSHARE::CText user_data_t::DATA = "data";
const NSHARE::CText user_data_t::HEADER = "head";
const NSHARE::CText user_data_t::PARSER = "by_parser";

NSHARE::CConfig user_data_t::MSerialize(NSHARE::CText const & aName) const
{
	return MSerialize(
			aName.empty() ?
					NULL : CParserFactory::sMGetInstance().MGetFactory(aName));
}
NSHARE::CConfig user_data_t::MSerialize(/*required_header_t const& aHead,*/
IExtParser* aP) const
{

	const size_t _size = FData.size();
	const uint8_t* _begin = (const uint8_t*) FData.ptr_const();

	CConfig _conf(NAME);
	_conf.MAdd(FDataId.MSerialize());

	//NSHARE::CConfig _data;
	bool _is_base64 = false;

	if (aP)
	{
		size_t _data_offset=0;
		try
		{
			if (FDataId.MIsRaw())
			{
				IExtParser* const _p =
						CParserFactory::sMGetInstance().MGetFactory(
								RAW_PROTOCOL_NAME);
				DCHECK_NOTNULL(_p);
				_data_offset = _p->MDataOffset(FDataId.FWhat);
			}
			else
			{
				_data_offset = aP->MDataOffset(FDataId.FWhat);
			}
		} catch (...)
		{
			LOG(DFATAL) << "Exception is occurred";
			_is_base64 = true;
		}
		if (!_is_base64)
			try
			{
				NSHARE::CConfig _data(
						aP->MToConfig(FDataId.FWhat, _begin + _data_offset,
								_begin + _size - _data_offset));
				_data.MKey() = DATA;
				if (_data.MIsOnlyKey())
				{
					LOG(ERROR) << "Cannot serialize by " << aP->MGetType();
					_is_base64 = true;
				}
				else
				{
					_conf.MAdd(_data);
					_conf.MAdd(PARSER, aP->MGetType());
				}

			} catch (...)
			{
				LOG(DFATAL) << "Exception is occurred";
				_is_base64 = true;
			}

	}
	else
		_is_base64=true;

	if (_is_base64)
	{
		NSHARE::CConfig const _data(DATA, FData);
		_conf.MAdd(PARSER, "base64");
		_conf.MAdd(_data);
	}
	return _conf;
}
bool user_data_t::MIsValid() const
{
	return FDataId.MIsValid() && !FData.empty();
}
void user_data_t::MMoveTo(user_data_t & aTo)
{
	aTo.FDataId=FDataId;
	FData.MMoveTo(aTo.FData);
}
const NSHARE::CText main_ch_param_t::NAME = "main";
const NSHARE::CText main_ch_param_t::CHANNEL = "channel";
main_ch_param_t::main_ch_param_t(NSHARE::CConfig const& aConf)
{
	VLOG(5) << " " << aConf.MToJSON(true);
	aConf.MGetIfSet(CHANNEL, FType);
	FValue=aConf.MChild("val");
	VLOG(5) << " " << FValue;
}
NSHARE::CConfig main_ch_param_t::MSerialize() const
{
	NSHARE::CConfig _main(NAME);
	_main.MSet(CHANNEL, FType);
	_main.MAdd("val",FValue);
	VLOG(5) << " " << _main.MToJSON(true);
	return _main;
}
bool main_ch_param_t::MIsValid()const
{
	return !FType.empty();
}
const NSHARE::CText network_channel_t::NAME = "network_channel_info";
const NSHARE::CText network_channel_t::PROTOCOL = "protocol";
const NSHARE::CText network_channel_t::AUTO_REMOVE = "auto_remove";
network_channel_t::network_channel_t(NSHARE::CConfig const& aConf) : //
		FAddress(aConf.MChild(net_address::NAME)), //
		FIsAutoRemoved(false)
{
	aConf.MGetIfSet(PROTOCOL, FProtocolType);
	aConf.MGetIfSet(AUTO_REMOVE, FIsAutoRemoved);
}
NSHARE::CConfig network_channel_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FAddress.MSerialize());
	_conf.MAdd(PROTOCOL, FProtocolType);
	_conf.MAdd(AUTO_REMOVE, FIsAutoRemoved);
	return _conf;
}
bool network_channel_t::MIsValid() const
{
	return FAddress.MIsValid() //
			&& !FProtocolType.empty();
}

const NSHARE::CText auto_search_info_t::NAME = "auto_search_info";
auto_search_info_t::auto_search_info_t(NSHARE::CConfig const& aConf) :
		FProgramm(aConf.MChild(program_id_t::NAME)), //
		FChannel(aConf.MChild(network_channel_t::NAME))
{
}

NSHARE::CConfig auto_search_info_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FProgramm.MSerialize());
	_conf.MAdd(FChannel.MSerialize());
	return _conf;
}
bool auto_search_info_t::MIsValid() const
{
	return FProgramm.MIsValid() //
	&& FChannel.MIsValid();
}

}
