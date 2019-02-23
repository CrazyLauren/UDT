/*
 * CResources.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "IState.h"
#include "CResources.h"

#if defined(RRD_STATIC)
extern "C"
{
	NSHARE::factory_registry_t*get_factory_registry();
}
#endif
using namespace NSHARE;
template<>
NUDT::CResources::singleton_pnt_t NUDT::CResources::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CResources::NAME = "res";
CResources::CResources(std::vector<NSHARE::CText> const& aResources,
		NSHARE::CText const& aExtPath) :
		IState(NAME), FExtLibraryPath(aExtPath)
{
	std::vector<NSHARE::CText>::const_iterator _it = aResources.begin();
	for (; _it != aResources.end(); ++_it)
	{
		module_t _mod;
		_mod.FName = *_it;
		_mod.FRegister = NULL;
		FModules.push_back(_mod);
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
		LOG_IF(WARNING,_it->FRegister) << "The Dynamic module '" << _it->FName
												<< "' has been loaded already";
		if (!_it->FRegister)
		{
			// load dynamic module
			if (!_it->FDynamic.get())
			{
				const NSHARE::CText& _name=_it->FName;
				try
				{

					SHARED_PTR<CDynamicModule> _module(new CDynamicModule(_name)); //throw std::invalid_argument if failed
					_it->FDynamic=_module;

					factory_registry_func_t _func = (factory_registry_func_t)(
							_it->FDynamic->MGetSymbolAddress(FACTORY_REGISTRY_FUNC_NAME));

					LOG_IF(DFATAL,!_func)<<
					"Required function export '"<<FACTORY_REGISTRY_FUNC_NAME<<
					"' was not found in dynamic module '" <<_name<< "'.";
					if(!_func)
					continue;
					
					_it->FRegister = _func(NULL);
					CHECK_NOTNULL(_it->FRegister);
				}
				catch(...)
				{
#if defined(RRD_STATIC)
					LOG(WARNING) << "The dynamic module '" << _it->FName << "' is static ";
					_it->FRegister = get_factory_registry();
#else
					LOG(ERROR)<<"Library "<<_name<<" is not exist. Ignoring ...";
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
NSHARE::CConfig CResources::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd("epath", FExtLibraryPath);
	mod_channels_t::const_iterator _it = FModules.begin(), _it_end(
			FModules.end());
	for (; _it != _it_end; ++_it)
	{
		_conf.MAdd(_it->MSerialize());
	}
	return _conf;
}
NSHARE::CConfig CResources::module_t::MSerialize() const
{
	NSHARE::CConfig _conf("emod");
	_conf.MAdd("ename", FName);
	if (FDynamic.get())
	{
		_conf.MAdd(NSHARE::CDynamicModule::NAME, FDynamic->MSerialize());
	}
	if (FRegister)
	{
		factory_registry_t::const_iterator _it = FRegister->begin(), _it_end =
				FRegister->end();
		for (; _it != _it_end; ++_it)
		{
			if (*_it)
				_conf.MAdd((*_it)->MSerialize());
			else
			{
				_conf.MAdd("null", "null");
			}
		}
	}
	return _conf;
}
} //
