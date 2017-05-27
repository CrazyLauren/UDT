/*
 * CExampleProtocolParser.cpp
 *
 *  Created on:  22.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <customer.h>
#include <CParserFactory.h>
#include <udt_example_protocol.h>
#include "CExampleProtocolParser.h"
#include "CExampleRegister.h"


using namespace NUDT;
NSHARE::CText const CExampleProtocolParser::NAME = PROTOCOL_NAME;
NSHARE::CText const CExampleRegister::NAME = "ExampleRegister";//!< The name must be unique

CExampleProtocolParser::CExampleProtocolParser() :
		IExtParser(NAME)
{

}

CExampleProtocolParser::~CExampleProtocolParser()
{
}

CExampleProtocolParser::result_t CExampleProtocolParser::MParserData(
		const uint8_t* aItBegin, const uint8_t* aItEnd,NSHARE::uuid_t aFrom,uint8_t aMask)
{
	//Parsing buffer aItBegin:aItEnd
	result_t _result;

	for (; aItBegin != aItEnd;)
	{
		const size_t _rem=aItEnd-aItBegin;
		obtained_dg_t _founded_dg;

		_founded_dg.FBegin = aItBegin;

		if (_rem < sizeof(msg_head_t))
		{
			aItBegin=aItEnd;
			_founded_dg.FErrorCode=E_INVALID_HEADER_SIZE;
		}
		else
		{
			msg_head_t const *_phead = (msg_head_t const*) aItBegin;
			if (_phead->FType != E_MSG_TEST)
			{
				aItBegin = aItEnd;
				_founded_dg.FErrorCode = E_INVALID_MSG_TYPE;
			}
			else if(_phead->FSize!=PACKET_SIZE/*test msg size*/)
			{
				aItBegin=aItEnd;
				_founded_dg.FErrorCode =E_INVALID_MSG_SIZE;
			}
			else if(_rem<_phead->FSize)
			{
				break;//buffer is small
			}
			else
			{
				aItBegin += _phead->FSize;
				_founded_dg.FType.FNumber = _phead->FType;
			}
		}
		_founded_dg.FEnd = aItBegin;

		_result.push_back(_founded_dg);
	}

	return _result;

}
std::pair<NUDT::required_header_t,bool> CExampleProtocolParser::MHeader(
		const NSHARE::CConfig& aFrom) const
{
	std::cout<<aFrom<<std::endl;
	NUDT::required_header_t _header;
	_header.FNumber=aFrom.MValue("num",0u);
	_header.FVersion=NSHARE::version_t(aFrom.MChild("ver"));
	return std::make_pair(_header, aFrom.MIsChild("num"));
}
NSHARE::CConfig CExampleProtocolParser::MToConfig(
		const NUDT::required_header_t& aHeader) const
{
	NSHARE::CConfig _conf("head");
	_conf.MAdd("num",aHeader.FNumber);
	_conf.MAdd("ver",aHeader.FVersion.MSerialize());
	return _conf;
}
//The code below is trivial for all parsers.
//You can use REGISTRE_ONLY_ONE_PROTOCOL_MODULE or define class register(see below)

//REGISTRE_ONLY_ONE_PROTOCOL_MODULE(CExampleProtocolParser,EXAMPLE_PARSER)

CExampleRegister::CExampleRegister() :
		NSHARE::CFactoryRegisterer(NAME,NSHARE::version_t(0,1))
{

}
void CExampleRegister::MUnregisterFactory() const
{
	CParserFactory::sMGetInstance().MRemoveFactory(CExampleProtocolParser::NAME);
}
void CExampleRegister::MAdding() const
{
	CParserFactory::sMAddFactory<CExampleProtocolParser>();
}
bool CExampleRegister::MIsAlreadyRegistered() const
{
	if (CParserFactory::sMGetInstancePtr())
		return CParserFactory::sMGetInstance().MIsFactoryPresent(
				CExampleProtocolParser::NAME);
	return false;

}

#if !defined(EXAMPLE_PARSER_STATIC)
static NSHARE::factory_registry_t g_factory;
extern "C" EXAMPLE_PARSER_EXPORT NSHARE::factory_registry_t* get_factory_registry()
{
	if(g_factory.empty())
	{
		g_factory.push_back(new CExampleRegister);
	}
	return &g_factory;
}
#else//#if !defined(EXAMPLE_PARSER_STATIC)
#	include <load_static_module.h>
namespace
{
	static NUDT::CStaticRegister< CExampleRegister> _reg;
}
#endif
