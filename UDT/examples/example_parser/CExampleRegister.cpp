// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CExampleRegister.h
 *
 *  Created on:  04.05.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <customer.h>
#include <CParserFactory.h>
#include "import_export_macros.h"
#include "CExampleRegister.h"
#include "CExampleProtocolParser.h"

using namespace NUDT;

NSHARE::CText const CExampleRegister::NAME = "ExampleRegister";

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
static NSHARE::factory_registry_t g_factory;///< List of registrar

/*! \brief This function is looked for by the kernel
 * in the library. It's called it for
 * receive list of all available  plugin
 * registrars.
 *
 *\return list of all available  plugin
 */
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

