include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckCXXSourceCompiles)

configure_logging(${SHARE_DEPENDENCIES_PATH})

check_library_exists(dex dex_open "" HAVE_DEX)

check_function_exists(backtrace_symbols HAVE_BACKTRACE_SYMBOLS)
check_library_exists(execinfo backtrace_symbols "" HAVE_LIBEXECINFO)
if(HAVE_LIBEXECINFO)
  set(HAVE_BACKTRACE_SYMBOLS TRUE)
endif()

check_function_exists(CaptureStackBackTrace HAVE_CAPTURESTACKBACKTRACE)

check_library_exists(dbghelp SymFromAddr "" HAVE_DBGHELP)
check_include_files(cxxabi.h HAVE_CXXABI_H)
check_include_files(dlfcn.h  HAVE_DLFCN)
check_include_files(signal.h HAVE_SIGNAL_H)

if(HAVE_SIGNAL_H)
  check_symbol_exists(sigaction "signal.h" HAVE_SIGACTION)
endif()

check_include_files(futex.h HAVE_FUTEX_H)

if(NOT HAVE_FUTEX_H)
	unset(HAVE_FUTEX_H CACHE)
	check_include_files(linux/futex.h HAVE_FUTEX_H)
endif()

IF (WIN32)
	find_library(LIBADVAPI32 advapi32)	
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
# end of get interface addresses
						
include(FindVTune)

find_package(Vtune)



configure_file(CMakeModules/config.h.cmake ${CMAKE_BINARY_DIR}/include/config/config.h ESCAPE_QUOTES)
install(FILES ${CMAKE_BINARY_DIR}/include/config/config.h 
DESTINATION ${CMAKE_INSTALL_PREFIX}/include/config/)