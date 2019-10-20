// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CResources.cpp
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright В© 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "CResources.h"
#include <config/customer/customer_config.h>
using namespace NSHARE;
template<>
NUDT::CResources::singleton_pnt_t NUDT::CResources::singleton_t::sFSingleton = NULL;
namespace NUDT
{
extern "C" NSHARE::factory_registry_t* static_factory_registry(NSHARE::CFactoryRegisterer* aVal)
{
	static NSHARE::CMutex _mutex;
	NSHARE::CRAII<NSHARE::CMutex> _lock(_mutex);//may be don't need
	static NSHARE::factory_registry_t _registry;
	if (aVal)
	{
		_registry.push_back(aVal);
	}
	return &_registry;
}
const NSHARE::CText CResources::NAME = "modules";
const NSHARE::CText CResources::MODULES_PATH = "modules_path";
const NSHARE::CText CResources::LIST_OF_LOADED_LIBRARY = "libraries";
const NSHARE::CText CResources::ONLY_SPECIFIED_LIBRARY = "only_specified";

CResources::CResources(NSHARE::CConfig const& aConf)
{
    NSHARE::CText const _static_modules(CUSTOMER_WITH_STATIC_MODULES);

    CConfig const& _libs=aConf.MChild(LIST_OF_LOADED_LIBRARY);
    if(!_libs.MIsEmpty())
    {
        ConfigSet _set = _libs.MChildren();
        ConfigSet::const_iterator _it = _set.begin();
        for (; _it != _set.end(); ++_it)
        {
            if(_static_modules.find((_it)->MKey())==NSHARE::CText::npos)
            {
                module_t _mod;
                _mod.FName = _it->MKey();
                _mod.FRegister = NULL;
                FModules.push_back(_mod);
            }
        }
    }
    aConf.MGetIfSet(MODULES_PATH,FExtLibraryPath);
    if(FExtLibraryPath.length())
    {
        LOG(ERROR)<<"Not implemented";
    }
    //aConf.MGetIfSet(ONLY_SPECIFIED_LIBRARY,FDontSearchLibrary);
}

CResources::~CResources()
{
	MUnload();
	VLOG(0) << "All resources unloaded :" << this;
}

void CResources::MLoad()
{
	VLOG(0) << "Begining resource loading:" << this;
	//all resources
	MLoadLibrariess();
	VLOG(0) << "Resource loading completed:" << this;
}
void CResources::MUnload()
{
	VLOG(0) << "Begining resource unloading:" << this;
	MUnloadChannels();
	VLOG(0) << "Resource unloading completed:" << this;
}
void CResources::MLoadLibrariess()
{
	VLOG(0) << "Begining channels loading:" << this;
	for (mod_channels_t::iterator _it = FModules.begin(); _it != FModules.end();
			++_it)
	{
		VLOG(2) << "Load '" << _it->FName << "' dynamic module";
		LOG_IF(WARNING,_it->FRegister)
				<< "The Dynamic module '" << _it->FName
						<< "' has been loaded already";
		if (!_it->FRegister)
		{
			// load dynamic module
			if (!_it->FDynamic.get())
			{
				try
				{
					SHARED_PTR<CDynamicModule> _module(new CDynamicModule(_it->FName));//throw std::invalid_argument if failed
					_it->FDynamic=_module;

					factory_registry_func_t _func = (factory_registry_func_t)(
							_it->FDynamic->MGetSymbolAddress(FACTORY_REGISTRY_FUNC_NAME));

					LOG_IF(DFATAL,!_func)<<
					"Required function export '"<<FACTORY_REGISTRY_FUNC_NAME<<
					"' was not found in dynamic module '" << _it->FName<< "'.";
					if(!_func)
					continue;
					
					_it->FRegister = _func(NULL);
					CHECK_NOTNULL(_it->FRegister);
				}
				catch(...)
				{
					LOG(ERROR)<<"Library "<<_it->FName<<" is not exist. Ignoring ...";
					continue;
				}
			}
		}
		VLOG(2) << "adding all	available factories.";
		factory_registry_t::iterator _jt = _it->FRegister->begin();
		for (; _jt != _it->FRegister->end(); ++_jt)
		{
			if (!(*_jt))
			{
				LOG(ERROR)<<"Null pointer of Factory register. Ignoring ...";
				continue;
			}
			if((*_jt)->FType.empty())
			{
				LOG(ERROR)<<"Invalid name of factory in "<<_it->FName<<". Ignoring ...";
				continue;
			}
			VLOG(2) << "Registry " << (*_jt)->FType<<" Version:"<<(*_jt)->FVersion;
			(*_jt)->MRegisterFactory();
		}
	}
	{
		factory_registry_t::iterator _jt(static_factory_registry(NULL)->begin()), _jt_end(static_factory_registry(NULL)->end());
		for (; _jt != _jt_end; ++_jt)
		{
			if (!(*_jt))
			{
				LOG(ERROR) << "Null pointer of Factory register. Ignoring ...";
				continue;
			}
			if ((*_jt)->FType.empty())
			{
				LOG(ERROR) << "Invalid name of factory in static module list. Ignoring ...";
				continue;
			}
			VLOG(2) << "Registry " << (*_jt)->FType << " Version:" << (*_jt)->FVersion;
			(*_jt)->MRegisterFactory();
		}
	}
}
void CResources::MUnloadChannels()
{
	VLOG(0) << "Begining channels unloading:" << this;
	for (mod_channels_t::iterator _it = FModules.begin(); _it != FModules.end();
			++_it)
	{
		if (!_it->FDynamic.get())
		{
			LOG(WARNING)<<_it->FName<<"  were already removed.";
			continue;
		}
		VLOG(2) << "unregister all factories.";
		CHECK_NOTNULL(_it->FRegister);
		factory_registry_t::iterator _jt = _it->FRegister->begin();
		for (; _jt != _it->FRegister->end(); ++_jt)
		{
			VLOG(2) << "Registry " << (*_jt)->FType;
			(*_jt)->MUnregisterFactory();
		}
		_it->FRegister = 0;
	}
}
}//
