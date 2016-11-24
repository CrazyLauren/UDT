/*
 * CResources.cpp
 *
 *  Created on: 19.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "CResources.h"

#if defined(RRD_STATIC)
extern "C"
{
NSHARE::factory_registry_t*get_factory_registry();
}
#endif
using namespace NSHARE;
template<>
NUDT::CResources::singleton_pnt_t NUDT::CResources::singleton_t::sFSingleton = NULL;
namespace NUDT
{
CResources::CResources(std::vector<NSHARE::CText> const& aResources,
		NSHARE::CText const& aExtPath)
{
	std::vector<NSHARE::CText>::const_iterator _it=aResources.begin();
	for (; _it != aResources.end(); ++_it)
	{
		module_t _mod;
		_mod.FName=*_it;
		_mod.FRegister=NULL;
		FModules.push_back(_mod);
	}
	if(aExtPath.length())
	{
		LOG(ERROR)<<"Not implemented";
	}
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
	MLoadChannels();
	VLOG(0) << "Resource loading completed:" << this;
}
void CResources::MUnload()
{
	VLOG(0) << "Begining resource unloading:" << this;
	MUnloadChannels();
	VLOG(0) << "Resource unloading completed:" << this;
}
void CResources::MLoadChannels()
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
					// get the WindowRendererModule object for this module.
					_it->FRegister = _func();
					CHECK_NOTNULL(_it->FRegister);
				}
				catch(...)
				{
#if defined(RRD_STATIC)
					LOG(WARNING) << "The dynamic module '" << _it->FName << "' is static ";
					_it->FRegister = get_factory_registry();
#else
					LOG(ERROR)<<"Library "<<_it->FName<<" is not exist. Ignoring ...";
					continue;

#endif //defined(RRD_STATIC)

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
