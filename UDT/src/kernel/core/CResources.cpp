// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
#include <SHARE/fdir.h>
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
const NSHARE::CText CResources::NAME = "modules";
const NSHARE::CText CResources::MODULES_PATH = "modules_path";
const NSHARE::CText CResources::LIST_OF_LOADED_LIBRARY = "libraries";
const NSHARE::CText CResources::ONLY_SPECIFIED_LIBRARY = "only_specified";

CResources::CResources(NSHARE::CConfig const& aConf) :
		ICore(NAME),//
		FDontSearchLibrary(false)
{
	CConfig const& _libs=aConf.MChild(LIST_OF_LOADED_LIBRARY);
	if(!_libs.MIsEmpty())
	{
		ConfigSet _set = _libs.MChildren();
		ConfigSet::const_iterator _it = _set.begin();
		for (; _it != _set.end(); ++_it)
		{
			MPutModule(_it->MKey(),*_it);
		}
	}
	aConf.MGetIfSet(MODULES_PATH,FExtLibraryPath);
	aConf.MGetIfSet(ONLY_SPECIFIED_LIBRARY,FDontSearchLibrary);

}

CResources::~CResources()
{
	MUnload();
	VLOG(0) << "All resources unloaded :" << this;
}

bool CResources::MStart()
{
	MLoad();
	return true;
}
void CResources::MStop()
{
	MUnload();
}

/*! \brief Put module to FModules
 *
 *\param aName A name of module without .so(dll)
 *\param aConf A passed configure information
 *\return true if put
 */
bool CResources::MPutModule(NSHARE::CText const& aName, NSHARE::CConfig aConf)
{
	VLOG(4) << "Push library " << aName;
	aConf.MAdd(module_t::LIBRARY_NAME, aName);
	module_t _mod(aConf);
	bool const _result= FModules.insert(_mod).second;

	DLOG_IF(WARNING,!_result)<<"The library is not put as it's exist:"<<aName;

	return _result;
}

/*! \brief Looking for libraries and put result
 *
 */
void CResources::MLookingForLibraries()
{
	VLOG(0) << "Looking for libraries in "<<FExtLibraryPath;

	const std::string _path=FExtLibraryPath.empty()?"./":FExtLibraryPath.c_str();

	///1) Get list of all files in the directory
	std::list<std::string> _to;
	NSHARE::get_name_of_files_of_dir(&_to, _path);

	VLOG(4) << "Amount of files "<<_to.size();
	///2) Put all library
	for (std::list<std::string>::iterator _it = _to.begin(); _it != _to.end();
			++_it)
	{
		VLOG(2)<<"Try to load: "<<*_it;
		if(NSHARE::CDynamicModule::sMIsNameOfLibrary(*_it))
		{
			VLOG(0)<<"Found library:"<<*_it;

			MPutModule(NSHARE::CDynamicModule::sMGetLibraryName(*_it));
		}
	}
}
void CResources::MLoad()
{
	VLOG(0) << "Begining resource loading:" << this;

	if(!FDontSearchLibrary)
		MLookingForLibraries();

	MLoadLibraries();

	VLOG(0) << "Resource loading completed:" << this;
}
void CResources::MUnload()
{
	VLOG(0) << "Begining resource unloading:" << this;
	MUnloadLibraries();
	VLOG(0) << "Resource unloading completed:" << this;
}

void CResources::MLoadLibraries()
{
	VLOG(0) << "Begining channels loading:" << this;
	for (mod_channels_t::iterator _it = FModules.begin(); _it != FModules.end();
			++_it)
	{
		_it->MLoad(FExtLibraryPath);
	}
}
void CResources::MUnloadLibraries()
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
const NSHARE::CText CResources::module_t::NAME = "emod";
const NSHARE::CText CResources::module_t::HAS_TO_BE_LOADED = "has_to_be_loaded";
const NSHARE::CText CResources::module_t::MODULE_SETTING = "configure";
const NSHARE::CText CResources::module_t::LIBRARY_NAME = "ename";
const NSHARE::CText CResources::module_t::ERROR_CODE = "error_code";

CResources::module_t::module_t(NSHARE::CText const& aName)://
		FName(aName),//
		FRegister(NULL),//
		FError(E_NO_ERROR), //
		FIsRequired(true)
{
	DCHECK(!NSHARE::CDynamicModule::sMIsNameOfLibrary(aName));
}
CResources::module_t::module_t(NSHARE::CConfig const& aConf) : //
		FName(aConf.MValue(LIBRARY_NAME, NSHARE::CText())), //
		FRegister(NULL), //
		FConfig(aConf.MChild(MODULE_SETTING)),
		FError(E_NO_ERROR), //
		FIsRequired(true)
{
	FError = static_cast<eError>(aConf.MValue(ERROR_CODE, 0));
	aConf.MGetIfSet(HAS_TO_BE_LOADED, FIsRequired);
}
bool CResources::module_t::MIsValid() const
{
	return !FName.empty(); //

}
bool CResources::module_t::operator<(module_t const& aRht) const
{
	return CStringFastLessCompare()(FName,aRht.FName);
}
/*!\brief Serialize object
 *
 * The key of serialized object is #NAME
 *
 *\return Serialized object.
 */
NSHARE::CConfig CResources::module_t::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(LIBRARY_NAME, FName);

	if(!FConfig.MIsEmpty())
		_conf.MAdd(MODULE_SETTING, FConfig);

	_conf.MAdd(ERROR_CODE, FError);
	_conf.MAdd(HAS_TO_BE_LOADED, FIsRequired);

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
/*! \brief Load the library
 *
 *\param aPath Path to library
 *\return true if loaded
 */
bool CResources::module_t::MLoad(NSHARE::CText const& aPath) const
{
	VLOG(2) << "Load '" << FName << "' dynamic module";
	DLOG_IF(FATAL,FRegister) << "The Dynamic module '" << FName
										<< "' has been loaded already";
	if (!FRegister)
	{
		///1) load dynamic library
		if (!FDynamic.get())
		{
			const NSHARE::CText& _name = FName;
			try
			{

				SHARED_PTR<CDynamicModule> _module(new CDynamicModule(_name,aPath)); //throw std::invalid_argument if failed
				FDynamic = _module;

				factory_registry_func_t _func =
						(factory_registry_func_t) (FDynamic->MGetSymbolAddress(
						FACTORY_REGISTRY_FUNC_NAME));
				if (!_func)
				{
					LOG(DFATAL) << "Required function export '"
													<< FACTORY_REGISTRY_FUNC_NAME
													<<
													"' was not found in dynamic module '"
													<< _name << "'.";
					FError = E_NO_FUNCTION;
					return false;
				}

				FRegister = _func(&FConfig);
				CHECK_NOTNULL(FRegister);
			} catch (...)
			{
#if defined(RRD_STATIC)
						LOG(WARNING) << "The dynamic module '" << FName << "' is static ";
						FRegister = get_factory_registry();
#else
				LOG(DFATAL) << "Library " << NSHARE::CDynamicModule::sMGetLibraryNameInSystem(_name)
										<< " is not exist. Ignoring ...";
				FError = E_NO_FUNCTION;
				return false;

#endif //defined(RRD_STATIC)
			}
		}
		else
		{
			DLOG(FATAL) << "It has been loaded before :" << FName;
		}

		VLOG(2) << "adding all	available factories.";

		///2) Registering all factory of library

		factory_registry_t::iterator _jt = FRegister->begin();
		for (; _jt != FRegister->end(); ++_jt)
		{
			if (!(*_jt))
			{
				LOG(DFATAL) << "Null pointer of Factory register. Ignoring ...";
				FError = E_NO_FACTORY_REGISTER;
				return false;
			}
			if ((*_jt)->FType.empty())
			{
				LOG(DFATAL) << "Invalid name of factory in " << FName
										<< ". Ignoring ...";
				FError = INVALID_NAME_OF_FACTORY;
				return false;
			}
			VLOG(2) << "Registry " << (*_jt)->FType << " Version:"
								<< (*_jt)->FVersion;
			(*_jt)->MRegisterFactory();
		}
	}


	return true;
}
} //
