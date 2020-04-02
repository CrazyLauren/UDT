include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCXXSourceCompiles)
include(CheckTypeSize)
include(FindPackageHandleStandardArgs)


check_library_exists(dex dex_open "" HAVE_DEX)

check_function_exists(backtrace_symbols HAVE_BACKTRACE_SYMBOLS)
check_library_exists(execinfo backtrace_symbols "" HAVE_LIBEXECINFO)
if(HAVE_LIBEXECINFO)
  set(HAVE_BACKTRACE_SYMBOLS TRUE)
endif()

check_function_exists(CaptureStackBackTrace HAVE_CAPTURESTACKBACKTRACE)

check_library_exists(dbghelp SymFromAddr "" HAVE_DBGHELP)
check_include_file_cxx(cxxabi.h HAVE_CXXABI_H)
check_include_files(dlfcn.h  HAVE_DLFCN)
check_include_files(signal.h HAVE_SIGNAL_H)
check_include_files(semaphore.h HAVE_POSIX_SEMAPHORES)
check_include_files(pthread.h HAVE_PTHREAD_H)

if(HAVE_POSIX_SEMAPHORES)
	set(CMAKE_EXTRA_INCLUDE_FILES semaphore.h )
	check_type_size("sem_t " SIZEOF_SEM_T LANGUAGE CXX)
endif()

if(HAVE_PTHREAD_H)
	set(CMAKE_EXTRA_INCLUDE_FILES pthread.h  )
	check_type_size("pthread_mutex_t " SIZEOF_PTHREAD_MUTEX_T LANGUAGE CXX)
	check_type_size("pthread_cond_t " SIZEOF_PTHREAD_COND_T LANGUAGE CXX)
endif()

if(HAVE_SIGNAL_H)
  check_symbol_exists(sigaction "signal.h" HAVE_SIGACTION)
endif()

check_include_files(futex.h HAVE_FUTEX_H)

if(NOT HAVE_FUTEX_H)
	unset(HAVE_FUTEX_H CACHE)
	check_include_files(linux/futex.h HAVE_FUTEX_H)
endif()


check_include_files(unistd.h HAVE_UNISTD_H)
check_function_exists(symlink HAVE_SYMLINK)

check_cxx_source_compiles("int main() { int _b=({ int x=2;5>x;})?0: 1; return 0;}"
                        HAVE_STATEMENTS_WITH_INITIALIZER )


check_function_exists(inet_ntop HAVE_INET_NTOP)

check_include_files("windows.h;winerror.h" HAVE_WINERROR_H)

#get interface addresses

check_include_files("windows.h;iphlpapi.h" HAVE_IPHLPAPI_H)
check_include_files("ifaddrs.h" HAVE_GETIFADDRS)
check_include_files("windows.h;winsock.h" HAVE_WINSOCK_H)

if(NOT HAVE_IPHLPAPI_H AND NOT HAVE_GETIFADDRS)
	message(FATAL_ERROR "Cannot implemente function for get interface addresses ")
endif()

check_include_files(x86intrin.h HAVE_X86INTRIN_H)
check_include_files(intrin.h HAVE_INTRIN_H)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(${PROJECT_NAME}_PLATFORM_DEFENITIONS  "" CACHE STRING "" FORCE)

if (MSVC)
	# This option is to enable the /MP switch for Visual Studio 2005 and above compilers
	option(BUILD_BY_MP "Set to ON to build  with the /MP option (Visual Studio 2005 and above)." ON)

	mark_as_advanced(BUILD_BY_MP)

	if(BUILD_BY_MP)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	endif(BUILD_BY_MP)

	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4200") #disabling of 4200 warning
	set(${PROJECT_NAME}_PLATFORM_DEFENITIONS 
		${${PROJECT_NAME}_PLATFORM_DEFENITIONS} 
			-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -DNOMINMAX 
			CACHE STRING "" FORCE
		)
    
    if(LIBADVAPI32)
    	set(HAVE_LIBADVAPI32 1)
    endif()
endif()


