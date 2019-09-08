/*
 * CRTCForModeling.cpp
 *
 *  Created on: 11 сент. 2019 г.
 *      Author: User
 */

#include <deftype>
#include <revision.h>
#include <random_value.h>
#include <CRTCFactory.h>

#include "CRTCForModelingModule.h"
#include "CRTCForModelingRegister.h"

DECLARATION_VERSION_FOR(rtc_for_modeling)
namespace NUDT
{
using namespace NSHARE;
NSHARE::CText const CRTCForModelingModule::NAME = "modeling";
NSHARE::CText const CRTCForModelingRegister::NAME = "rtc_modeling_registrator";

CRTCForModelingModule::CRTCForModelingModule() :
		IRtcControl(NAME)
{

}

CRTCForModelingRegister::CRTCForModelingRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(MAJOR_VERSION_OF(rtc_for_modeling), MINOR_VERSION_OF(rtc_for_modeling), REVISION_OF(rtc_for_modeling)))
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
} /* namespace NUDT */

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
