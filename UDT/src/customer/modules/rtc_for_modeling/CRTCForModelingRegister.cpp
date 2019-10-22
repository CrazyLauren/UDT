/*
 * CRTCForModelingRegister.cpp
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/revision.h>
#include <SHARE/random_value.h>
#include <CRTCFactory.h>

#include "CRTCForModelingModule.h"
#include "CRTCForModelingRegister.h"
DECLARATION_VERSION_FOR(rtc_for_modeling)
namespace NUDT
{
using namespace NSHARE;
NSHARE::CText const CRTCForModelingRegister::NAME = "rtc_modeling_registrator";



CRTCForModelingRegister::CRTCForModelingRegister() :
		NSHARE::CFactoryRegisterer(NAME,
				NSHARE::version_t(MAJOR_VERSION_OF(rtc_for_modeling),
						MINOR_VERSION_OF(rtc_for_modeling),
						REVISION_OF(rtc_for_modeling)))
{

}
void CRTCForModelingRegister::MUnregisterFactory() const
{
	CRTCFactory::sMGetInstance().MRemoveFactory(CRTCForModelingModule::NAME);
}
void CRTCForModelingRegister::MAdding() const
{
	//CRTCForModeling* _p =
	CRTCFactory::sMAddFactory<CRTCForModelingModule>();
}
bool CRTCForModelingRegister::MIsAlreadyRegistered() const
{
	if (CRTCFactory::sMGetInstancePtr())
		return CRTCFactory::sMGetInstance().MIsFactoryPresent(
				CRTCForModelingModule::NAME);
	return false;

}
}

#if !defined(RTC_FOR_MODELING_STATIC)
static NSHARE::factory_registry_t g_factory;
extern "C" RTC_FOR_MODELING_EXPORT NSHARE::factory_registry_t* get_factory_registry()
{
	if (g_factory.empty())
	{
		g_factory.push_back(new NUDT::CRTCForModelingRegister());
	}
	return &g_factory;
}
#else
#	include <load_static_module.h>
namespace
{
	static NUDT::CStaticRegister<NUDT::CRTCForModelingRegister> _reg;
}
#endif

