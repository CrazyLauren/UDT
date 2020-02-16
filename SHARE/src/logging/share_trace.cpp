// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * trace_share.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <exception>
#ifdef _MSC_VER
#	include <SHARE/win_unistd.h>
#	include <tchar.h>
#	pragma warning(disable : 4290)
#	pragma warning(disable : 4996)
#else
#	include <unistd.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <list>
#include <map>
#include <algorithm>
#include <vector>
#include <deque>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>
#include <string>
#include <SHARE/macro_attributes.h>

#include <string.h>
#include <math.h>
#include <iostream>
#include <stdint.h>
#include <functional>
#include <SHARE/UType/CDenyCopying.h>
#include <SHARE/UType/CRAII.h>
#include <SHARE/UType/atomic_t.h>
#include <SHARE/UType/IAllocater.h>
#include <SHARE/UType/CCOWPtr.h>
#include <SHARE/logging/share_trace.h>
#include <SHARE/UType/CText.h>
#include <SHARE/logging/vlog_is_on.h>


#if defined(CPLUS_LOG)
#	include <log4cplus/logger.h>
#endif

#include <SHARE/logging/CStackTrace.h>

void init_share_trace(char const *aProgrammName)
{
	if (!NSHARE::logging_impl::is_inited())
	{
		std::set_terminate(get_log_terminate_handler());
		install_failure_signal_handler();
#ifndef NOLOG

#endif

#ifdef GLOG
		init_trace_glog(aProgrammName);
#elif defined(CPLUS_LOG)
		init_trace_cplus(aProgrammName);
#endif
		NSHARE::logging_impl::is_inited() = true;
	}
}

void init_trace(int argc, char const *argv[])
{
	if (!NSHARE::logging_impl::is_inited())
	{
		parse_command_line_of_logging(argc, argv);
	}
}
#ifdef GLOG
extern void init_trace_glog(char const *aProgrammName)
{
	google::InitGoogleLogging(aProgrammName);
	google::InstallFailureSignalHandler();
	std::set_terminate(get_log_terminate_handler());
}
#else
extern void init_trace_glog(char const *aProgrammName)
{
}
;
#endif

static void terminate_handler_impl()
{
	NSHARE::CStackTrace _trace;
	_trace.MPrint(std::cerr);
#ifdef GLOG
	google::FlushLogFiles(google::INFO);
#elif defined(CPLUS_LOG)
	log4cplus::Logger::getRoot().shutdown();
#endif
	NSHARE::logging_impl::is_inited()=false;
}
extern void log_terminate_handler()
{
	terminate_handler_impl();
	using namespace std;
	terminate();
}
extern std::terminate_handler get_log_terminate_handler()
{

	return &terminate_handler_impl;
}
;

#ifdef GLOG
namespace fLS
{
	extern GOOGLE_GLOG_DLL_DECL std::string& FLAGS_vmodule;
}
#endif


