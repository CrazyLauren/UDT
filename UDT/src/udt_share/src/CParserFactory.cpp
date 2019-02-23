/*
 * CParserFactory.cpp
 *
 *  Created on: 21.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <programm_id.h>
#include <time.h>
#include <internel_protocol.h>
#include <shared_types.h>
#include <udt_share.h>
#include <CParserFactory.h>
#include "CRawProtocol.h"

template<>
NSHARE::CFactoryManager<NUDT::IExtParser>::singleton_pnt_t NSHARE::CSingleton<
		NSHARE::CFactoryManager<NUDT::IExtParser> >::sFSingleton = NULL;
namespace NUDT
{
extern UDT_SHARE_EXPORT const NSHARE::CText RAW_PROTOCOL_NAME = "raw";


const NSHARE::CText CParserFactory::NAME="parsers";
const NSHARE::CText CParserFactory::PARSER="parser";
CParserFactory::CParserFactory()
{
	MAddFactory(new CRawProtocolParser());
}
NSHARE::CConfig CParserFactory::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);

	factory_its_t _its = MGetIterator();
	for (; _its.FBegin != _its.FEnd; ++_its.FBegin)
	{
		_conf.MAdd(_its.FBegin->first, _its.FBegin->second->MDescription());
	}
	return _conf;
}
}
