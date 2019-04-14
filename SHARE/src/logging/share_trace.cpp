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
#	include <win_unistd.h>
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
#include <macro_attributes.h>
#if   defined(__QNX__)||defined(unix) //||  defined(__MINGW32__)
#include <pthread.h>                        // POSIX threads support
#include <errno.h>
#elif defined(_WIN32)
#include <windows.h>
#endif
#include <string.h>
#include <math.h>
#include <iostream>
#include <stdint.h>
#include <functional>
#include <UType/CDenyCopying.h>
#include <UType/CRAII.h>
#include <UType/atomic_t.h>
#include <UType/IAllocater.h>
#include <UType/CCOWPtr.h>
#include <UType/CText.h>
#include <logging/vlog_is_on.h>
#ifdef GLOG
#	include <glog/logging.h>
#elif defined(CPLUS_LOG)
#	include <logging/share_trace_log4cplus.h>
#	include <log4cplus/logger.h>
#elif defined(COUT_LOG)
#	include <logging/share_cout_log.h>
#else
# 	include <logging/share_nolog.h>
#endif
#include <limits>
#include <set>
#include <UType/cb_t.h>
#include <UType/CFlags.h>
#include <UType/CMutex.h>
#include <UType/CEvent.h>

#include <UType/CConfig.h>
#include <UType/CThread.h>
#include <logging/CShareLogArgsParser.h>
#include <tclap/CmdLine.h>
namespace NSHARE
{
extern unsigned sleep(unsigned aVal);
}
extern SHARE_EXPORT void init_trace(int argc, char *argv[]);
extern SHARE_EXPORT void init_share_trace(char const*aProgrammName);
extern SHARE_EXPORT void init_trace_cplus(char const*aProgrammName);
extern SHARE_EXPORT void init_trace_glog(char const*aProgrammName);
extern SHARE_EXPORT std::terminate_handler get_log_terminate_handler();
extern "C" SHARE_EXPORT void install_failure_signal_handler();

extern SHARE_EXPORT char default_logging_option_name[];
extern SHARE_EXPORT char default_logging_short_option_name;

void init_share_trace(char const *aProgrammName)
{
	if (!NSHARE::logging_impl::is_inited())
	{

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

void init_trace(int argc, char *argv[])
{
	if (!NSHARE::logging_impl::is_inited())
	{
		using namespace TCLAP;
		using namespace std;
		using namespace NSHARE;
		try
		{

			char const _log_flag[] =
			{ default_logging_short_option_name, '\0' };

			CmdLine cmd("fix", ' ', "0.9", false,true);
			cmd.setExceptionHandling(false);

			CShareLogArgsParser _logging(_log_flag,
					default_logging_option_name,false);
			cmd.add(_logging);
			
			cmd.parse(argc, argv);

			init_share_trace(cmd.getProgramName().c_str());

		} catch (ArgException &e)  // catch any exceptions
		{
			cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
			return;
		}
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

static void fucking_win_terminate_handler()
{
#ifdef GLOG
	google::FlushLogFiles(google::INFO);
#elif defined(CPLUS_LOG)
	log4cplus::Logger::getRoot().shutdown();
#endif
}
extern std::terminate_handler get_log_terminate_handler()
{

	return &fucking_win_terminate_handler;
}
;

#ifdef GLOG
namespace fLS
{
	extern GOOGLE_GLOG_DLL_DECL std::string& FLAGS_vmodule;
}
#endif

char default_logging_option_name[] = "verbose";
char default_logging_short_option_name = 'v';

#if defined(GLOG) ||defined(CPLUS_LOG)

#	ifdef _POSIX_VERSION
#	include <signal.h>
void signal_process(int signal_number)
{
	struct sigaction sig_action;
	memset(&sig_action, 0, sizeof(sig_action));
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_handler = SIG_DFL;
	sigaction(signal_number, &sig_action, NULL);
	kill(getpid(), signal_number);
}
static bool g_is_set=false;
static pthread_t g_entered_thread_id_pointer;
void signal_handler(int aSignal, siginfo_t *info, void *ucontext)
{

	pthread_t _thread_id = pthread_self();
	if (g_is_set)
	{
		if (pthread_equal(_thread_id, g_entered_thread_id_pointer))
		{
			signal_process(aSignal);
		}
		while (true)
		{
			std::cerr << "Sleep 1" << std::endl;
			sleep(1);
		}
	}
	else
	{
		g_entered_thread_id_pointer = _thread_id;
		g_is_set=true;
	}

	LOG(ERROR)<<"The signal "<<aSignal<<" is been processing."
	<<"A signal code: "<<info->si_code<<" Errno: "
	<<info->si_errno<<". "<<strerror(info->si_errno)<<".";

	switch (aSignal)
	{
		case SIGSEGV:
		LOG(ERROR)<<"Addr: "<<info->si_addr;
		break;
		case SIGCLD:
		{
			LOG(ERROR)<<"PID: "<<info->si_pid;
			LOG(ERROR)<<"Status: "<<info->si_status;
			LOG(ERROR)<<"Utime: "<<info->si_utime;
			LOG(ERROR)<<"Stime: "<<info->si_stime;
		}
		break;
		default:
		break;
	}
	switch (info->si_code)
	{
		case SI_USER:
		case SI_QUEUE:
		{
			LOG(ERROR)<<"PID: "<<info->si_pid;
			LOG(ERROR)<<"UID: "<<info->si_uid;
		}
		break;

		default:
		break;
	}
	fucking_win_terminate_handler();
	// Kill ourself by the default signal handler.
	signal_process(aSignal);
}
#		ifdef GLOG
extern "C" void install_failure_signal_handler()
{
	google::InstallFailureSignalHandler();;
}
#		elif defined(CPLUS_LOG)
extern "C" void install_failure_signal_handler()
{
	struct sigaction sig_action;
	memset(&sig_action, 0, sizeof(sig_action));
	sigemptyset(&sig_action.sa_mask);
	sig_action.sa_flags |= SA_SIGINFO;
	sig_action.sa_sigaction = &signal_handler;

	int _signals[]=
	{
		SIGSEGV,
		SIGILL,
		SIGFPE,
		SIGABRT,
#ifdef SIGBUS
		SIGBUS,
#endif
#ifdef SIGBREAK
		SIGBREAK,
#elif defined(SIGTTIN)
		SIGTTIN,
#else
		21,
#endif
		SIGTTIN,
		SIGTERM
	};

	for (size_t i = 0; i < sizeof(_signals)/sizeof(_signals[0]); ++i)
	{
		sigaction(_signals[i], &sig_action, NULL);
	}
}
#		endif//#elif defined(CPLUS_LOG)
#	elif defined(__MINGW32__)//_POSIX_VERSION
#		include <signal.h>
static unsigned g_entered_thread_id_pointer;
static bool g_is_set = false;
static std::vector<void (*)(int)> g_prev_signals(NSIG, SIG_DFL );
void signal_process(int signal_number)
{
	signal(signal_number, SIG_DFL );
	raise(signal_number);
}
void signal_handler(int aSignal)
{
	unsigned _thread_id = NSHARE::CThread::sMThreadId();
	if (g_is_set)
	{
		if (_thread_id == g_entered_thread_id_pointer)
		{
			signal_process(aSignal);
		}
		while (true)
		{
			std::cerr << "Sleep 1" << std::endl;
			NSHARE::sleep(1);
		}
	}
	else
	{
		g_entered_thread_id_pointer = _thread_id;
		g_is_set = true;
	}

	LOG(ERROR)<<"The signal \""<<aSignal<<"\" is been processing."
	<<" Errno: "
	<<errno<<". "<<strerror(errno)<<".";
	fucking_win_terminate_handler();

	if (g_prev_signals[aSignal] != SIG_DFL )
		(*g_prev_signals[aSignal])(aSignal);

	// Kill ourself by the default signal handler.
	signal_process(aSignal);
}

extern "C" void install_failure_signal_handler()
{
	int _signals[] =
	{ SIGSEGV, SIGILL, SIGFPE, SIGABRT,
#	ifdef SIGBUS
			SIGBUS,
#	endif
			SIGBREAK, SIGTERM };

	for (size_t i = 0; i < sizeof(_signals) / sizeof(_signals[0]); ++i)
	{
		g_prev_signals[_signals[i]] = signal(_signals[i], signal_handler);
	}
}
#else
extern "C" void install_failure_signal_handler()
{
	//todo
}
#endif//#elif defined(__MINGW32__)//_POSIX_VERSION
#endif//#ifndef NOLOG

