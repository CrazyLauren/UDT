/*
 * shared_types.cpp
 *
 *  Created on: 30.03.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <shared_types.h>
//#include <udt_share.h>
#include <string.h>



#define ID_NAME "id"
#define PROGRAM_ID_NAME "info"

using namespace NSHARE;
namespace NUDT
{
extern UDT_SHARE_EXPORT const NSHARE::CText RAW_PROTOCOL_NAME="raw";
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
		const_iterator _it = begin(), _it_end =end();
		for (; _it != _it_end; ++_it)
		{
			_conf.MAdd(_it->MSerialize());
		}
	}
	return _conf;
}
bool uuids_t::MIsValid() const
{
	const_iterator _it(begin()), _end(
			end());
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
	VLOG(2)<<"Create split_packet_t from "<<aConf;
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
	return FCounter>0 && FCoefficient>0;
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
const NSHARE::CText user_data_info_t::KEY_PACKET_FROM = "f";
const NSHARE::CText user_data_info_t::KEY_PACKET_TO = "to";
const NSHARE::CText user_data_info_t::KEY_PACKET_PROTOCOL = "pl";
const NSHARE::CText user_data_info_t::KEY_RAW_PROTOCOL_NUM = "nr";

user_data_info_t::user_data_info_t(NSHARE::CConfig const& aConf) :
		FFrom(aConf.MChild(KEY_PACKET_FROM)),//
		FRawProtocolNumber(0),//
		FSplit(aConf.MChild(split_packet_t::NAME))
{
	VLOG(2) << "Create user_data_info_t from " << aConf;
	aConf.MGetIfSet(KEY_PACKET_NUMBER, FPacketNumber);
	aConf.MGetIfSet(KEY_PACKET_PROTOCOL, FProtocol);
	aConf.MGetIfSet(KEY_RAW_PROTOCOL_NUM, FRawProtocolNumber);
	ConfigSet _set = aConf.MChildren(KEY_PACKET_TO);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push uuid " << *_it;
		FUUIDTo.MGet().push_back(NSHARE::uuid_t(*_it));
	}
}
NSHARE::CConfig user_data_info_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet(KEY_PACKET_NUMBER, FPacketNumber);
	_conf.MSet(KEY_PACKET_PROTOCOL, FProtocol);
	_conf.MAdd(KEY_PACKET_FROM, FFrom.MSerialize());
	_conf.MAdd(split_packet_t::NAME, FSplit.MSerialize());

	if (FUUIDTo.MIs())
	{
		uuids_t::const_iterator _it = FUUIDTo.MGetConst().begin(), _it_end =
				FUUIDTo.MGetConst().end();
		for (; _it != _it_end; ++_it)
		{
			_conf.MAdd(KEY_PACKET_TO, _it->MSerialize());
		}
	}
	_conf.MSet(KEY_RAW_PROTOCOL_NUM, FRawProtocolNumber);

	return _conf;
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
		FProgramm(aConf.MChild(PROGRAM_ID_NAME)), //
		FLatency(std::numeric_limits<uint16_t>::max()),//
		FConnectTime(NSHARE::get_unix_time())
{
	aConf.MGetIfSet(LATENCY, FLatency);
	aConf.MGetIfSet(TIME, FConnectTime);
	aConf.MGetIfSet(LINK, FTypeLink);
}
kernel_link::kernel_link(programm_id_t const& aInfo, uint16_t aLink):
		FProgramm(aInfo),//
		FLatency(aLink),//
		FConnectTime(NSHARE::get_unix_time())
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
	return FProgramm.MIsValid() && FLatency != std::numeric_limits<unsigned>::max();
}

const NSHARE::CText kernel_infos_t::NAME = "kinf";
const NSHARE::CText kernel_infos_t::CLIENT = kernel_link::NAME;
const NSHARE::CText kernel_infos_t::NUMBER = "indx";

kernel_infos_t::kernel_infos_t(NSHARE::CConfig const& aConf) :
		FKernelInfo(aConf.MChild(PROGRAM_ID_NAME)),//
		FIndexNumber(0)//
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

demand_dg_t::demand_dg_t(NSHARE::CConfig const& aConf) :
		FWhat(aConf.MChild("rh")),//
		FNameFrom(aConf.MChild(user_data_info_t::KEY_PACKET_FROM))
{
	VLOG(2) << "Create demand_dg_t from " << aConf;
	aConf.MGetIfSet(user_data_info_t::KEY_PACKET_PROTOCOL, FProtocol);

	NSHARE::CConfig const & _set = aConf.MChild(NSHARE::uuid_t::NAME);
	if (!_set.MIsEmpty())
	{
		FUUIDFrom.MSet(NSHARE::uuid_t (_set));
	}
}
NSHARE::CConfig demand_dg_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(FWhat.MSerialize());
	_conf.MAdd(user_data_info_t::KEY_PACKET_FROM, FNameFrom.MSerialize());
	_conf.MAdd(user_data_info_t::KEY_PACKET_PROTOCOL, FProtocol);
	if(FUUIDFrom.MIs())
	{
		_conf.MAdd(FUUIDFrom.MGetConst().MSerialize());
	}

	return _conf;
}
bool demand_dg_t::MIsValid() const
{
	return !FProtocol.empty() && (FNameFrom.MIsValid() || FUUIDFrom.MIs());
}
bool demand_dg_t::operator==(demand_dg_t const& aRht) const
{
	return FProtocol == aRht.FProtocol && //
			FNameFrom == aRht.FNameFrom && //
			memcmp(FWhat.FReserved, aRht.FWhat.FReserved, sizeof(FWhat.FReserved))
					== 0 && //
			FUUIDFrom == aRht.FUUIDFrom;
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
NSHARE::CConfig demand_dgs_t::MSerialize() const
{
	CConfig _conf(NAME);
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		_conf.MAdd(_it->MSerialize());
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
kernel_infos_array_t::kernel_infos_array_t(NSHARE::CConfig const& aConf)
{
	VLOG(2) << "Create kernel info from " << aConf;
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
		CConfig *_new = _conf.MMutableChild(OPENED_KERNELS);
		CHECK_NOTNULL(_new);
		k_diff_t::const_iterator _it = FOpened.begin(),
				_it_end = FOpened.end();
		for (; _it != _it_end; ++_it)
		{
			NSHARE::CConfig _con(_it->first.MSerialize());
			_con.MAdd("kop",_it->second);
			_new->MAdd(_con);
		}
	}
	if (!FClosed.empty())
	{
		_conf.MAdd(CLOSED_KERNELS, CConfig());
		CConfig *_new = _conf.MMutableChild(CLOSED_KERNELS);
		CHECK_NOTNULL(_new);
		k_diff_t::const_iterator _it = FClosed.begin(),
				_it_end = FClosed.end();
		for (; _it != _it_end; ++_it)
		{
			NSHARE::CConfig _con(_it->first.MSerialize());
			_con.MAdd("kcl",_it->second);
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
	ConfigSet _set = aConf.MChildren(ID_NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push kernel info " << *_it;
		insert(
				value_type(id_t(*_it),
						demand_dgs_t(_it->MChild(demand_dgs_t::NAME))));
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
const NSHARE::CText error_info_t::NAME = "err";
const NSHARE::CText error_info_t::WHERE = "who";
const NSHARE::CText error_info_t::CODE = "code";
error_info_t::error_info_t() :
		FError(E_NO_ERROR)
{
	;
}
error_info_t::error_info_t(NSHARE::CConfig const& aConf) :
		FError(
				static_cast<eErrorCode>(aConf.MValue<unsigned>(CODE, E_NO_ERROR))), //
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
	_conf.MSet<unsigned>(CODE, FError);
	if(MIsValid())
	{
		_conf.MAdd(WHERE,FWhere.MSerialize());
		_conf.MAdd(FTo.MSerialize());
	}
	return _conf;
}
bool error_info_t::MIsValid() const
{
	return FError!=E_NO_ERROR;
}

//-----------------
const NSHARE::CText routing_t::NAME = "rtg";
const NSHARE::CText routing_t::FROM = user_data_info_t::KEY_PACKET_FROM;

routing_t::routing_t(NSHARE::CConfig const& aConf):
		uuids_t(aConf.MChild(uuids_t::NAME)),
		FFrom(aConf.MChild(FROM))
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
	else
		DCHECK(empty());
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
	ConfigSet _set = aConf.MChildren(PROGRAM_ID_NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push info " << *_it;
		insert(programm_id_t(*_it));
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
}
