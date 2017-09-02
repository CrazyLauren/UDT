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

#ifdef MINGW_WITHOUT_DLFCN
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

#if defined(__WIN32__) || defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
static const NSHARE::CDynamicModule::string_t g_libraryExtension(".dll");
#else
static const NSHARE::CDynamicModule::string_t g_libraryExtension(".so");
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
//----------------------------------------------------------------------------//
//extern const char MODULE_DIR_VAR_NAME[] = "";

static bool has_extension(const CText& name)
{
	const size_t ext_len = g_libraryExtension.length();

	if (name.length() < ext_len)
		return false;

	return name.compare(name.length() - ext_len, ext_len, g_libraryExtension) == 0;
}
//----------------------------------------------------------------------------//
static void append_extension(CText& name)
{
	name.append(g_libraryExtension);
}
//----------------------------------------------------------------------------//
static void add_suffixes(CText& name)
{
	append_extension(name);
}

//----------------------------------------------------------------------------//
static NSHARE::CDynamicModule::string_t get_module_env_var()
{
	//TODO
	return NSHARE::CDynamicModule::string_t();
}

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
	DYN_LIB_HANDLE handle = 0;

	const NSHARE::CDynamicModule::string_t envModuleDir(/*aPath.empty()?get_module_env_var():*/aPath);

	if (!envModuleDir.empty())
		handle = DYN_LIB_LOAD(envModuleDir + '/' + name);

	if (!handle)
		handle = DYN_LIB_LOAD(name);

	if (!handle)
		handle = DYN_LIB_LOAD("./" + name);

	return handle;
}

CDynamicModule::CDynamicModule(const string_t& name,const string_t& aPath) :
		FPimpl(new CImpl(name))
{
	CHECK (!name.empty());

	if (!has_extension(FPimpl->FModuleName))
	add_suffixes(FPimpl->FModuleName);

	FPimpl->FHandle = dyn_lib_load(FPimpl->FModuleName,aPath);

#if defined(__linux__)  || defined(__MINGW32__) || defined(__FreeBSD__) || defined(__NetBSD__)||defined(__QNX__)
	if (!FPimpl->FHandle && FPimpl->FModuleName.compare(0, 3, "lib") != 0)
	{
		FPimpl->FModuleName.insert(0, "lib");
		FPimpl->FHandle = dyn_lib_load(FPimpl->FModuleName,aPath);
	}
#endif

#if defined(__CYGWIN__) 
	// see if adding a leading 'cyg' helps us to open the library
	if (!FPimpl->FHandle && FPimpl->FModuleName.compare(0, 3, "cyg") != 0)
	{
		FPimpl->FModuleName.insert(0, "cyg");
		FPimpl->FHandle = dyn_lib_load(FPimpl->FModuleName,aPath);
	}
#endif

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
