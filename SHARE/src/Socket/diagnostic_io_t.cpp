/*
 * diagnostic_io_t.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#include <deftype>
#include <Socket/diagnostic_io_t.h>

namespace NSHARE
{
const NSHARE::CText diagnostic_io_t::NAME="io_state";
diagnostic_io_t::diagnostic_io_t()
{
	memset(this,0,sizeof(*this));
}
CConfig diagnostic_io_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd("recv",FRecvData);
	_conf.MAdd("sent",FSentData);
	_conf.MAdd("recvc",FRecvCount);
	_conf.MAdd("sentc",FSentCount);
	return _conf;
}
void diagnostic_io_t::MRecv(size_t aCount)const
{
	++FRecvCount;
	FRecvData+=aCount;
}
void diagnostic_io_t::MSend(size_t aCount)const
{
	++FSentCount;
	FSentData+=aCount;
}

};


