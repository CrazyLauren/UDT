// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * initialize_signal_handler.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>


#if defined(GLOG) ||defined(CPLUS_LOG)

#ifdef HAVE_SIGNAL_H
#	include <signal.h>
namespace NSHARE
{
void signal_process(int signal_number)
{
	signal(signal_number, SIG_DFL );
	raise(signal_number);
}
static bool g_is_set=false;
static CThread::process_id_t g_entered_thread_id_pointer;
static std::vector<void (*)(int)> g_prev_signals(NSIG, SIG_DFL );

/** A signal handler
 *
 * @param aSignal
 */
void signal_handler(int aSignal)
{
	CThread::process_id_t _thread_id = NSHARE::CThread::sMThreadId();

	if (g_is_set)
	{
		if (_thread_id == g_entered_thread_id_pointer)
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
	<<"A signal code: "<<signal_process<<" Errno: "
	<<errno<<". "<<strerror(errno)<<".";

	std::cerr<<"Signal:"<<aSignal<<std::endl;
	get_log_terminate_handler()();

	if (g_prev_signals[aSignal] != SIG_DFL )
		(*g_prev_signals[aSignal])(aSignal);

	// Kill ourself by the default signal handler.
	signal_process(aSignal);
}
#		ifdef GLOG
extern "C" void install_failure_signal_handler()
{
	google::InstallFailureSignalHandler();
}
#		else
/** Initialize handler of signals
 *
 */
extern "C" void install_failure_signal_handler()
{
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
#ifdef SIGTTIN
		SIGTTIN,
#endif
		SIGTERM
	};

	for (size_t i = 0; i < sizeof(_signals) / sizeof(_signals[0]); ++i)
	{
		g_prev_signals[_signals[i]] = signal(_signals[i], signal_handler);
	}
}
}
#		endif//#elif defined(CPLUS_LOG)
#	else
namespace NSHARE
{
extern "C" void install_failure_signal_handler()
{
	//todo
}
}
#endif
#endif//#ifndef NOLOG


