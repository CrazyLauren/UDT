/*
 * CParserFactory.cpp
 *
 *  Created on: 21.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
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

template<>
NSHARE::CFactoryManager<NUDT::IExtParser>::singleton_pnt_t NSHARE::CSingleton<
		NSHARE::CFactoryManager<NUDT::IExtParser> >::sFSingleton = NULL;
namespace NUDT
{
extern UDT_SHARE_EXPORT const NSHARE::CText RAW_PROTOCOL_NAME = "raw";
class CRawProtocolParser:public IExtParser
{
public:
	CRawProtocolParser() :
			IExtParser(RAW_PROTOCOL_NAME)
	{
	}

	~CRawProtocolParser()
	{
	}

	result_t MParserData(const uint8_t* aItBegin,
			const uint8_t* aItEnd,NSHARE::uuid_t aFrom=NSHARE::uuid_t(),uint8_t aMask=ORDER_LE)
	{
		//Parsing buffer aItBegin:aItEnd
		static result_t _result(1);
		LOG(DFATAL)<<"In "<<RAW_PROTOCOL_NAME<<" calling  method 'parser data' is no sense. ";
		return _result;

	}
	NSHARE::CText MToString(
			const required_header_t& aHeader) const
	{
		NSHARE::CText _text;
		_text.MPrintf("# %d", aHeader.FNumber);
		return _text;
	}
	NUDT::required_header_t MHeader(
			const NSHARE::CConfig& aFrom) const
	{
		NUDT::required_header_t _header;
		_header.FNumber = aFrom.MValue("num", 0u);
		_header.FVersion = NSHARE::version_t(aFrom.MChild("ver"));
		return _header;
	}
	NSHARE::CConfig MToConfig(
			const NUDT::required_header_t& aHeader) const
	{
		NSHARE::CConfig _conf("head");
		_conf.MAdd("num", aHeader.FNumber);
		_conf.MAdd( aHeader.FVersion.MSerialize());
		return _conf;
	}
};

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
