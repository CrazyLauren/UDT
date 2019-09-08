// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * diagnostic_io_t.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#include <deftype>
#include <Socket/diagnostic_io_t.h>

namespace NSHARE
{
const NSHARE::CText sent_state_t::NAME="io_error";
const NSHARE::CText sent_state_t::KEY_ERROR="error";
const NSHARE::CText sent_state_t::KEY_BYTE="bytes";

const NSHARE::CText sent_state_t::KEY_INVALID_VALUE="invalid_value";
const NSHARE::CText sent_state_t::KEY_NOT_OPENED="not_opened";
const NSHARE::CText sent_state_t:: KEY_TOO_LARGE="too_large";
const NSHARE::CText sent_state_t::KEY_AGAIN="again";
const NSHARE::CText sent_state_t::KEY_UNKNOWN_ERROR="unknown_error";
const NSHARE::CText sent_state_t:: KEY_SOCKET_CLOSED="socket_closed";

sent_state_t::sent_state_t(NSHARE::CConfig const& aConf): //
				FError(0),//
				FBytes(0) //
{

	aConf.MGetIfSet(KEY_BYTE,FBytes);

	FError.MSetFlag(E_INVALID_VALUE,
			aConf.MValue(KEY_INVALID_VALUE,
					FError.MGetFlag(E_INVALID_VALUE)));
	FError.MSetFlag(E_NOT_OPENED,
			aConf.MValue(KEY_NOT_OPENED, FError.MGetFlag(E_NOT_OPENED)));
	FError.MSetFlag(E_TOO_LARGE,
			aConf.MValue(KEY_TOO_LARGE, FError.MGetFlag(E_TOO_LARGE)));
	FError.MSetFlag(E_AGAIN,
			aConf.MValue(KEY_AGAIN, FError.MGetFlag(E_AGAIN)));
	FError.MSetFlag(E_ERROR,
			aConf.MValue(KEY_UNKNOWN_ERROR, FError.MGetFlag(E_ERROR)));
	FError.MSetFlag(E_SOCKET_CLOSED,
			aConf.MValue(KEY_SOCKET_CLOSED, FError.MGetFlag(E_SOCKET_CLOSED)));
}

CConfig sent_state_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(KEY_BYTE,FBytes);

	_conf.MAdd(KEY_INVALID_VALUE, FError.MGetFlag(E_INVALID_VALUE));
	_conf.MAdd(KEY_NOT_OPENED, FError.MGetFlag(E_NOT_OPENED));
	_conf.MAdd(KEY_TOO_LARGE, FError.MGetFlag(E_TOO_LARGE));
	_conf.MAdd(KEY_AGAIN, FError.MGetFlag(E_AGAIN));
	_conf.MAdd(KEY_UNKNOWN_ERROR, FError.MGetFlag(E_ERROR));
	_conf.MAdd(KEY_SOCKET_CLOSED, FError.MGetFlag(E_SOCKET_CLOSED));

	return _conf;
}
bool sent_state_t::MIsValid() const
{
	return (FError.MGetMask()>>E_MAX_BITWISE_CODE)==0;
}

const NSHARE::CText diagnostic_io_t::NAME="io_state";
const NSHARE::CText diagnostic_io_t::KEY_RECVDATA="recv";
const NSHARE::CText diagnostic_io_t::KEY_SENTDATA="sent";
const NSHARE::CText diagnostic_io_t::KEY_RECVCOUNT="recvc";
const NSHARE::CText diagnostic_io_t::KEY_SENTCOUNT="sentc";

diagnostic_io_t::diagnostic_io_t()
{
	FRecvData=0;
	FRecvCount=0;
	FSentData=0;
	FSentCount=0;
}
CConfig diagnostic_io_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(KEY_RECVDATA,FRecvData);
	_conf.MAdd(KEY_SENTDATA,FSentData);
	_conf.MAdd(KEY_RECVCOUNT,FRecvCount);
	_conf.MAdd(KEY_SENTCOUNT,FSentCount);

	const unsigned _last_bits = sent_state_t::E_MAX_BITWISE_CODE;

	for (unsigned i = 0; i < _last_bits; ++i)
	{
		if(!FErrorInfo[i].MIs())
			_conf.MAdd(FErrorInfo[i].MSerialize());

	}

	return _conf;
}
diagnostic_io_t::diagnostic_io_t(NSHARE::CConfig const& aConf)
{
	FRecvData=0;
	FRecvCount=0;
	FSentData=0;
	FSentCount=0;

	aConf.MGetIfSet(KEY_RECVDATA,FRecvData);
	aConf.MGetIfSet(KEY_SENTDATA,FSentData);
	aConf.MGetIfSet(KEY_RECVCOUNT,FRecvCount);
	aConf.MGetIfSet(KEY_SENTCOUNT,FSentCount);

	ConfigSet const _set=aConf.MChildren(sent_state_t::NAME);

	ConfigSet::const_iterator _it=_set.begin(),_it_end(_set.end());

	for(;_it!=_it_end;++_it)
	{
		sent_state_t const _state(*_it);
		DCHECK(_state.MIsValid());

		MSend(_state);
	}
}
bool diagnostic_io_t::MIsValid() const
{
	return true;
}
sent_state_t const& diagnostic_io_t::MGetState(sent_state_t::eSendState const& aError) const
{
	switch(aError)
	{
	case sent_state_t::E_INVALID_VALUE:
		return FErrorInfo[0];
	case sent_state_t::E_NOT_OPENED:
			return FErrorInfo[1];
	case sent_state_t::E_TOO_LARGE:
			return FErrorInfo[2];
	case sent_state_t::E_AGAIN:
			return FErrorInfo[3];
	case sent_state_t::E_ERROR:
			return FErrorInfo[4];
	case sent_state_t::E_SOCKET_CLOSED:
			return FErrorInfo[5];
	case sent_state_t::E_MAX_BITWISE_CODE:
	case sent_state_t::E_SENDED:
		break;
	}
	DCHECK(false);
	static sent_state_t const _fix;
	return _fix;
}
};

