// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * dymamic_modules.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <stdexcept>

namespace NSHARE
{

#ifndef HAVE_DLFCN
#undef __MINGW32__
#undef __MINGW64__
#endif

#if (defined(__WIN32__) || defined(_WIN32)) && !defined(__MINGW32__)
#   if defined(_MSC_VER)
#       pragma warning(disable : 4552)  // warning: operator has no effect; expected operator with side-effect
#	elif defined(__MINGW32__) && __GNUC__>=4 && __GNUC_MINOR__>=3
#		pragma GCC diagnostic ignored  "-Wunused-value"
#   endif
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   define DYN_LIB_LOAD( a ) LoadLibraryEx( (a).c_str(),NULL,LOAD_WITH_ALTERED_SEARCH_PATH )
#   define DYN_LIB_GETSYM( a, b ) GetProcAddress( a, (b).c_str() )
#   define DYN_LIB_UNLOAD( a ) !FreeLibrary( a )
typedef HMODULE DYN_LIB_HANDLE;
#endif

#if defined(__linux__)  || defined(__FreeBSD__) || defined(__NetBSD__)  || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__QNX__)
#   include <dlfcn.h>
#   define DYN_LIB_LOAD( a ) dlopen( (a).c_str(), RTLD_LAZY )
#   define DYN_LIB_GETSYM( a, b ) dlsym( a, (b).c_str() )
#   define DYN_LIB_UNLOAD( a ) dlclose( a )
typedef void* DYN_LIB_HANDLE;
#endif

#ifndef NDEBUG
#	ifdef CMAKE_DEBUG_POSTFIX
const NSHARE::CText CDynamicModule::LIBRARY_UNIQUE_POSTFIX=CMAKE_DEBUG_POSTFIX;
#	else
const NSHARE::CText CDynamicModule::LIBRARY_UNIQUE_POSTFIX="";
#	endif//#	ifndef CMAKE_DEBUG_POSTFIX
#else
#	ifdef CMAKE_RELEASE_POSTFIX
const NSHARE::CText CDynamicModule::LIBRARY_UNIQUE_POSTFIX=CMAKE_RELEASE_POSTFIX;
#	else
const NSHARE::CText CDynamicModule::LIBRARY_UNIQUE_POSTFIX="";
#	endif//#	ifndef CMAKE_RELEASE_POSTFIX
#endif //#ifndef NDEBUG
#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
const NSHARE::CText CDynamicModule::LIBRARY_EXTENSION(".dll");
#	ifdef __CYGWIN__
const NSHARE::CText CDynamicModule::LIBRARY_PREFIX("cyg");
#	elif defined(__MINGW32__)
const NSHARE::CText CDynamicModule::LIBRARY_PREFIX("lib");
#	else
const NSHARE::CText CDynamicModule::LIBRARY_PREFIX("");
#	endif
#else
const NSHARE::CText CDynamicModule::LIBRARY_EXTENSION(".so");
const NSHARE::CText CDynamicModule::LIBRARY_PREFIX("lib");
#endif


const NSHARE::CText CDynamicModule::NAME = "lib";
struct CDynamicModule::CImpl
{
	CImpl(const string_t& name) :
			FModuleName(name), FHandle(0)
	{
	}

	~CImpl()
	{
		DYN_LIB_UNLOAD(FHandle);
	}

	string_t FModuleName;
	DYN_LIB_HANDLE FHandle;
};
NSHARE::CConfig CDynamicModule::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd("name",FPimpl->FModuleName);
	return _conf;
}
bool CDynamicModule::MIsValid()const
{
	return !FPimpl->FModuleName.empty();
}
//----------------------------------------------------------------------------//
//extern const char MODULE_DIR_VAR_NAME[] = "";

static bool has_postfix(const CText& name)
{
	const size_t ext_len = CDynamicModule::LIBRARY_EXTENSION.length();

	if (name.length() < ext_len)
		return false;

	return name.compare(name.length() - ext_len, ext_len, CDynamicModule::LIBRARY_EXTENSION) == 0;
}

static bool has_extension(const CText& name)
{
	const size_t ext_len = CDynamicModule::LIBRARY_EXTENSION.length();

	if (name.length() < ext_len)
		return false;

	return name.compare(name.length() - ext_len, ext_len, CDynamicModule::LIBRARY_EXTENSION) == 0;
}
static bool has_prefix(const CText& name)
{
	const size_t ext_len = CDynamicModule::LIBRARY_PREFIX.length();

	if(ext_len==0)
		return false;

	if (ext_len>name.length())
		return false;
	return name.compare(0, ext_len, CDynamicModule::LIBRARY_PREFIX) == 0;
}
bool NSHARE::CDynamicModule::sMIsNameOfLibrary(const CText& aName,bool aAllowPostifx)
{
	if(!aAllowPostifx)
		return has_extension(aName);
	else
	{
		bool _is=has_extension(aName);
		if(_is && !CDynamicModule::LIBRARY_UNIQUE_POSTFIX.empty())
		{
			CText _name(aName);
			_name.erase(_name.length() - CDynamicModule::LIBRARY_EXTENSION.length(),
					CDynamicModule::LIBRARY_EXTENSION.length());

			_is=has_postfix(_name);
		}
		return _is;
	}
}
CText NSHARE::CDynamicModule::sMGetLibraryName(CText name)
{
	if (has_extension(name))
	{
		name.erase(name.length() - CDynamicModule::LIBRARY_EXTENSION.length(),
				CDynamicModule::LIBRARY_EXTENSION.length());

		if(!CDynamicModule::LIBRARY_UNIQUE_POSTFIX.empty() && has_postfix(name))
		{
			name.erase(name.length() - CDynamicModule::LIBRARY_UNIQUE_POSTFIX.length(),
					CDynamicModule::LIBRARY_UNIQUE_POSTFIX.length());
		}
	}
	if (has_prefix(name))
	{
		name.erase(0, CDynamicModule::LIBRARY_PREFIX.length());
	}
	return name;
}
CText NSHARE::CDynamicModule::sMGetLibraryNameInSystem(CText name)
{
	if(!CDynamicModule::LIBRARY_UNIQUE_POSTFIX.empty())
	{
		name.append(CDynamicModule::LIBRARY_UNIQUE_POSTFIX);
	}
	name.append(CDynamicModule::LIBRARY_EXTENSION);
	if(!CDynamicModule::LIBRARY_PREFIX.empty())
	{
		name.insert(0,CDynamicModule::LIBRARY_PREFIX);
	}
	return name;
}
//----------------------------------------------------------------------------//
/*static NSHARE::CDynamicModule::string_t get_module_env_var()
{
	//TODO
	return NSHARE::CDynamicModule::string_t();
}*/

//----------------------------------------------------------------------------//
static NSHARE::CDynamicModule::string_t get_failure_str()
{
	NSHARE::CDynamicModule::string_t retMsg;
#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__CYGWIN__) || defined(__MINGW32__) ||defined(__QNX__)
	retMsg = dlerror();
#elif defined(__WIN32__) || defined(_WIN32)
	LPVOID msgBuffer;

	if (FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
					| FORMAT_MESSAGE_IGNORE_INSERTS, 0, GetLastError(),
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			reinterpret_cast<LPTSTR>(&msgBuffer), 0, 0))
	{
		retMsg = reinterpret_cast<LPTSTR>(msgBuffer);
		LocalFree(msgBuffer);
	}
	else
	{
		retMsg = "Unknown Error";
	}
#else
	retMsg = "Unknown Error";
#endif
	return retMsg;
}

static DYN_LIB_HANDLE dyn_lib_load(const CText& name,const CText& aPath)
{
	DYN_LIB_HANDLE handle = NULL;

	const NSHARE::CDynamicModule::string_t envModuleDir(/*aPath.empty()?get_module_env_var():*/aPath);

	if (!envModuleDir.empty())
		handle = DYN_LIB_LOAD(envModuleDir + '/' + name);
	else
	{		
		handle = DYN_LIB_LOAD(name);

		if (handle == NULL)
			handle = DYN_LIB_LOAD("./" + name);
	}
	return handle;
}

CDynamicModule::CDynamicModule(const string_t& name,const string_t& aPath) :
		FPimpl(new CImpl(name))
{
	CHECK (!name.empty());

	if (!has_extension(FPimpl->FModuleName))
		FPimpl->FModuleName=sMGetLibraryNameInSystem(FPimpl->FModuleName);

	FPimpl->FHandle = dyn_lib_load(FPimpl->FModuleName,aPath);

	// check for library load failure
	if (!FPimpl->FHandle)
		throw std::invalid_argument(("Failed to load module '" +
					FPimpl->FModuleName + "': " + get_failure_str()).c_str());
}

CDynamicModule::~CDynamicModule()
{
	delete FPimpl;
}

const CDynamicModule::string_t& CDynamicModule::MGetModuleName() const
{
	return FPimpl->FModuleName;
}

void* CDynamicModule::MGetSymbolAddress(const CText& symbol) const
{
	return (void*) DYN_LIB_GETSYM(FPimpl->FHandle, symbol);
}

}
