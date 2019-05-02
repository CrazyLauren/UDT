// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CMainChannelFactory.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2016
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
#include <io/IIOManager.h>
#include "CMainChannelFactory.h"

template<>
NUDT::CMainChannelFactory::singleton_pnt_t
NUDT::CMainChannelFactory::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CMainChannelFactory::NAME="mainchannels";
const NSHARE::CText CMainChannelFactory::MAIN_CHANNEL="mch";
const NSHARE::CText CMainChannelFactory::NAME_MAIN="name";
const NSHARE::CText IMainChannel::CONFIGURE_NAME=CMainChannelFactory::NAME;

CMainChannelFactory::CMainChannelFactory():IState(NAME)
{
}

CMainChannelFactory::~CMainChannelFactory()
{
}
NSHARE::CConfig CMainChannelFactory::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);

	factory_its_t _its=MGetIterator();
	for(;_its.FBegin!=_its.FEnd;++_its.FBegin)
	{
		NSHARE::CConfig _m(MAIN_CHANNEL);
		CHECK(!_its.FBegin->first.empty());
		_m.MAdd(NAME_MAIN,_its.FBegin->first);
		CHECK_NOTNULL(_its.FBegin->second);
		_m.MAdd(_its.FBegin->second->MSerializeRef());
		_conf.MAdd(_m);
	}
	return _conf;
}
}
