include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)

configure_logging(${SHARE_DEPENDENCIES_PATH})

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
check_include_file_cxx(signal.h HAVE_SIGNAL_H)
if(HAVE_SIGNAL_H)
  check_symbol_exists(sigaction "signal.h" HAVE_SIGACTION)
endif()

IF (WIN32)
	find_library(LIBADVAPI32 advapi32)
endif()

include(FindVTune)

find_package(Vtune)



configure_file(CMakeModules/config.h.cmake ${CMAKE_BINARY_DIR}/include/config/config.h ESCAPE_QUOTES)
install(FILES ${CMAKE_BINARY_DIR}/include/config/config.h 
DESTINATION ${CMAKE_INSTALL_PREFIX}/include/config/config.h)