/*
 * log_signal_handler.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef LOG_SIGNAL_HANDLER_H_
#define LOG_SIGNAL_HANDLER_H_

/*
#include <signal.h>
#if defined(_POSIX_VERSION)
inline void log_term_handler(int aSignal, siginfo_t* info, void* other)
{
	LOG(ERROR)<<"The signal "<<aSignal<<" is been processing."
	<<"A signal code: "<<info->si_code<<" Errno: "
	<<info->si_errno<<". "<<strerror(info->si_errno)<<".";

	switch(info->si_code)
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
	switch (aSignal)
	{
		case SIGINT:
		case SIGQUIT:
		case SIGKILL:
		case SIGALRM:
		case SIGUSR1:
		case SIGUSR2:
			break;

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

		case SIGILL:
		case SIGFPE:
		case SIGBUS:

		case SIGHUP:
		case SIGTRAP:
		//case SIGIOT:
		case SIGABRT:
		//case SIGEMT:
		case SIGDEADLK:

		case SIGSYS:
		case SIGPIPE:

		case SIGTERM:

		case SIGPWR:
		case SIGWINCH:
		case SIGURG:
		case SIGIO:
		case SIGSTOP:
		case SIGTSTP:
		case SIGCONT:
		case SIGTTIN:
		case SIGTTOU:
		case SIGVTALRM:
		case SIGPROF:
		case SIGXCPU:
		case SIGXFSZ:
		break;
		default:
		break;

	}
}
#endif

*/
#endif /* LOG_SIGNAL_HANDLER_H_ */
