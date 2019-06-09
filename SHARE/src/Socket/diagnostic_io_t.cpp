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

CConfig sent_state_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(KEY_BYTE,FBytes);
	_conf.MAdd(KEY_ERROR,FError);
	return _conf;
}


const NSHARE::CText diagnostic_io_t::NAME="io_state";
const NSHARE::CText diagnostic_io_t::KEY_RECVDATA="recv";
const NSHARE::CText diagnostic_io_t::KEY_SENTDATA="sent";
const NSHARE::CText diagnostic_io_t::KEY_RECVCOUNT="recvc";
const NSHARE::CText diagnostic_io_t::KEY_SENTCOUNT="sentc";

diagnostic_io_t::diagnostic_io_t()
{
	memset(this,0,sizeof(*this));
}
CConfig diagnostic_io_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(KEY_RECVDATA,FRecvData);
	_conf.MAdd(KEY_SENTDATA,FSentData);
	_conf.MAdd(KEY_RECVCOUNT,FRecvCount);
	_conf.MAdd(KEY_SENTCOUNT,FSentCount);


	const unsigned _last_bits = sent_state_t::E_MAX_BITWISE_CODE;

	for (unsigned i = 0; i <= _last_bits; ++i)
	{
		if(!FErrorInfo[i].MIs())
			_conf.MAdd(FErrorInfo[i].MSerialize());
	}
	return _conf;
}

};


