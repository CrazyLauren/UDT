/*
 * CResources.cpp
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>

template<>
NSHARE::CResources::singleton_pnt_t NSHARE::CResources::singleton_t::sFSingleton = NULL;
namespace NSHARE
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
const NSHARE::CText CResources::NAME = "res";
CResources::CResources(param_t const & aParam):
		FIsLoad(false)
{
	MSetParam(aParam);
}
CResources::CResources(NSHARE::CConfig const& aConfig):
				FIsLoad(false)
{
	MSetParam(param_t(aConfig));
}

CResources::~CResources()
{
	MUnload();
	VLOG(0) << "All resources unloaded :" << this;
}
bool CResources::MSetParam(param_t const & aParam)
{
	if(MIsLoaded())
		return false;
	FModules.clear();
	FSearchPath=aParam.search_path;

	param_t::libraries_registrators_t::const_iterator _it=aParam.libraries.begin();
	for (; _it != aParam.libraries.end(); ++_it)
	{
		module_t const _mod(*_it);
		FModules.push_back(_mod);
	}
	return false;
}
void CResources::MLoad()
{
	if (MIsLoaded())
		return;

	VLOG(0) << "Begining channels loading:" << this;
	for (mod_channels_t::iterator _it = FModules.begin(); _it != FModules.end();
			++_it)
	{
		VLOG(2) << "Load '" << _it->FRegInfo.library << "' dynamic module"<<" by "<< _it->FRegInfo.reg_func;
		LOG_IF(WARNING,_it->FRegister)
				<< "The Dynamic module '" <<  _it->FRegInfo.library
						<< "' has been loaded already";
		if (!_it->FRegister)
		{
			// load dynamic module
			if (!_it->FDynamic.get())
			{
				try
				{
					SHARED_PTR<CDynamicModule> _module(new CDynamicModule(_it->FRegInfo.library,FSearchPath));//throw std::invalid_argument if failed
					_it->FDynamic=_module;

					CHECK(!_it->FRegInfo.reg_func.empty());

					factory_registry_func_t _func = (factory_registry_func_t)(
							_it->FDynamic->MGetSymbolAddress(_it->FRegInfo.reg_func));

					LOG_IF(DFATAL,!_func)<<
					"Required function export '"<<FACTORY_REGISTRY_FUNC_NAME<<
					"' was not found in dynamic module '" << _it->FRegInfo.library<<" by "<<_it->FRegInfo.reg_func<< "'.";
					if(!_func)
						continue;
					_it->FRegister = _func(_it->FRegInfo.config.MIsEmpty()?//
							NULL://
							&_it->FRegInfo.config);
					CHECK_NOTNULL(_it->FRegister);
				}
				catch(...)
				{
					LOG(ERROR)<<"Library "<<_it->FRegInfo.library<<" is not exist. Ignoring ...";
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
				LOG(ERROR)<<"Invalid name of factory in "<<_it->FRegInfo.library<<". Ignoring ...";
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
		static_factory_registry(NULL)->clear();
	}
	FIsLoad=true;
}
void CResources::MUnload()
{
	if(!MIsLoaded())
		return;

	VLOG(0) << "Begining channels unloading:" << this;
	for (mod_channels_t::iterator _it = FModules.begin(); _it != FModules.end();
			++_it)
	{
		if (!_it->FDynamic.get())
		{
			LOG(WARNING)<<_it->FRegInfo.library<<"  were already removed.";
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
	FIsLoad=false;
}
NSHARE::CConfig CResources::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(param_t::SEARCH_PATH, FSearchPath);
	mod_channels_t::const_iterator _it = FModules.begin(), _it_end(
			FModules.end());
	for (; _it != _it_end; ++_it)
	{
		_conf.MAdd(_it->MSerialize());
	}
	return _conf;
}
CResources::module_t::module_t(param_t::reg_t const& aReg):
		FRegInfo(aReg),//
		FRegister(NULL)
{
}

NSHARE::CConfig CResources::module_t::MSerialize() const
{
	NSHARE::CConfig _conf(param_t::NAME);

	NSHARE::CConfig _lib(FRegInfo.library);
	if (FRegInfo.reg_func != FACTORY_REGISTRY_FUNC_NAME)
		_lib.MAdd(param_t::REGISTRY_FUNC_NAME, FRegInfo.reg_func);

	if (!FRegInfo.config.MIsEmpty())
		_lib.MAdd(param_t::REG_CONFIG, FRegInfo.config);

	_conf.MAdd(_lib);
	_conf.MAdd(param_t::LIBRARY, FRegInfo.library);
	if (FDynamic.get())
	{
		_conf.MAdd(FDynamic->MSerialize());
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

const NSHARE::CText CResources::param_t::NAME="emod";
const NSHARE::CText CResources::param_t::LIBRARY="lib";
const NSHARE::CText CResources::param_t::SEARCH_PATH="path";
const NSHARE::CText CResources::param_t::REGISTRY_FUNC_NAME="func_name";
const NSHARE::CText CResources::param_t::REG_CONFIG="reg_conf";

CResources::param_t::param_t()
{
	;
}
CResources::param_t::param_t(std::vector<NSHARE::CText> const& aLibraries,
		NSHARE::CText const& aPath):
		search_path(aPath)
{
	std::vector<NSHARE::CText>::const_iterator _it=aLibraries.begin(),_it_end=aLibraries.end();
	for(;_it!=_it_end;++_it)
	{
		reg_t _reg;
		_reg.library=*_it;
		_reg.reg_func=REGISTRY_FUNC_NAME;
		libraries.push_back(_reg);
	}
}
CResources::param_t::param_t(NSHARE::CConfig const& aConf)
{
	{
		NSHARE::CConfig const& _set = aConf.MChild(LIBRARY);
		if (!_set.MIsEmpty())
		{
			NSHARE::ConfigSet const& _child=_set.MChildren();
			ConfigSet::const_iterator _it = _child.begin();
			for (; _it != _child.end(); ++_it)
			{
				NSHARE::CConfig const &_lib= *_it;

				NSHARE::CText _f_name(REGISTRY_FUNC_NAME);
				_lib.MGetIfSet(REGISTRY_FUNC_NAME, _f_name);

				VLOG(1) << "Push library " <<_lib.MKey()<<" function "<<_f_name;
				reg_t _reg;
				 _reg.library=_lib.MKey();
				 _reg.reg_func=_f_name;
				 _reg.config=_lib.MChild(REG_CONFIG);
				libraries.push_back(_reg);
			}
		}
	}
	aConf.MGetIfSet(SEARCH_PATH,search_path);
}
bool CResources::param_t::MIsValid() const
{
	libraries_registrators_t::const_iterator _it=libraries.begin(),_it_end(libraries.end());
	for(;_it!=_it_end;++_it)
		if(_it->library.empty() ||_it->reg_func.empty())
			return false;
	return true;
}
NSHARE::CConfig CResources::param_t::MSerialize() const
{
	DCHECK(MIsValid());

	NSHARE::CConfig _conf(NAME);

	{
		NSHARE::CConfig _librarys(LIBRARY);
		libraries_registrators_t::const_iterator _it = libraries.begin(),
				_it_end(libraries.end());
		for (; _it != _it_end; ++_it)
		{
			NSHARE::CConfig _lib(_it->library);
			if(_it->reg_func!=FACTORY_REGISTRY_FUNC_NAME)
				_lib.MAdd(REGISTRY_FUNC_NAME,_it->reg_func);
			if(!_it->config.MIsEmpty())
				_lib.MAdd(REG_CONFIG,_it->config);

			_librarys.MAdd(_lib);
		}
		_conf.MAdd(_librarys);
	}
	if(!search_path.empty())
		_conf.MAdd(SEARCH_PATH,search_path);

	return _conf;
}
}//
