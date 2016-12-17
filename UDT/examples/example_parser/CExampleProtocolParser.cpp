/*
 * CExampleProtocolParser.cpp
 *
 *  Created on: 22.01.2016
 *      Author: Sergey
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
		const uint8_t* aItBegin, const uint8_t* aItEnd,uint8_t aMask, required_header_t const*)
{
	//Parsing buffer aItBegin:aItEnd
	result_t _result;

	obtained_dg_t _founded_dg;

	for (; aItBegin < aItEnd;)
	{
		msg_head_t const *_phead=(msg_head_t const*)aItBegin;

		_founded_dg.FBegin = aItBegin;
		aItBegin += _phead->FSize;
		_founded_dg.FEnd = aItBegin;

		_founded_dg.FType.FNumber = _phead->FType;

		_result.push_back(_founded_dg);
	}

	return _result;

}
NUDT::required_header_t CExampleProtocolParser::MHeader(
		const NSHARE::CConfig& aFrom) const
{
	NUDT::required_header_t _header;
	_header.FNumber=aFrom.MValue("num",0u);
	_header.FVersion=NSHARE::version_t(aFrom.MChild("ver"));
	return _header;
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
