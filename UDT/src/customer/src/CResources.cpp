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
