/*
 * trace_share.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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

#include <stdlib.h>
#include <ctype.h>
#include <list>
#include <map>
#include <algorithm>
#include <vector>
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
#include <UType/CText.h>
#ifdef GLOG
#	include <glog/logging.h>
#elif defined(CPLUS_LOG)
#	include <share_trace_log4cplus.h>
#	include <log4cplus/logger.h>
#elif defined(COUT_LOG)
#	include <share_cout_log.h>
#else
# 	include <share_nolog.h>
#endif
#include <limits>
#include <deque>
#include <set>
#include <UType/deftype.h>
#include <UType/CEvent.h>
#include <UType/CDenyCopying.h>
#include <UType/CConfig.h>
#include <UType/CThread.h>
namespace NSHARE
{
extern unsigned sleep(unsigned aVal);
}
extern SHARE_EXPORT int parsing_logging_arguments(int argc, char *argv[]);
extern SHARE_EXPORT void init_trace(int argc, char *argv[]);
extern SHARE_EXPORT void init_trace_cplus(int argc, char *argv[]);
extern SHARE_EXPORT void init_trace_glog(int argc, char *argv[]);
extern SHARE_EXPORT std::terminate_handler get_log_terminate_handler();
extern "C" SHARE_EXPORT void install_failure_signal_handler();

void init_trace(int argc,char *argv[])
{
	static bool _is = false;
	if (!_is)
	{
		_is = true;
#ifndef NOLOG

#endif

#ifdef GLOG
		parsing_logging_arguments(argc, argv);
		init_trace_glog(argc, argv);
#elif defined(CPLUS_LOG)
		parsing_logging_arguments(argc, argv);
		init_trace_cplus(argc,argv);
#endif
	}
};


#ifdef GLOG
extern void init_trace_glog(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	google::InstallFailureSignalHandler();
	std::set_terminate(get_log_terminate_handler());
}
#else
extern void init_trace_glog(int argc,char *argv[])
{
};
#endif
//#include <deftype>

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

#if defined(_MSC_VER)
extern "C" int getopt(  int nargc,_TCHAR * const *nargv,const _TCHAR *ostr);
extern "C" _TCHAR  *optarg;
#else
extern "C" int getopt(int, char * const [], const char *);
extern "C" char *optarg;
#endif
#if defined(__MINGW32__) || defined(_MSC_VER)
extern "C" int getsubopt(char **optionp, char **valuep, char * const *tokens);
#endif
extern int parsing_logging_arguments(int argc, char *argv[])
{
#ifdef GLOG
	using namespace google;
	using fLS::FLAGS_vmodule;
#endif
	char *verbose_opts[] =
	{
#define LOGTOSTDERR    0
			(char*) "logtostderr", //Set whether log messages go to stderr instead of logfiles
#define ALSOLOGTOSTDERR   1// Set whether log messages go to stderr in addition to logfiles
			(char*) "alsostderr",
#define COLORLOGTOSTDERR   2// Set color messages logged to stderr (if supported by terminal).
			(char*) "colorstderr",
#define STDERRTHRESHOLD    3 // Log messages at a level >= this flag are automatically sent to
			// stderr in addition to log files.
			(char*) "stderrthreshold",
#define LOG_PREFIX    4  // Set whether the log prefix should be prepended to each line of output.
			(char*) "logprefix",

#define LOGBUFLEVEL    5 // Log messages at a level <= this flag are buffered.
			// Log messages at a higher level are flushed immediately.
			(char*) "logbuflevel",
#define LOGBUFSECS    6 // Sets the maximum number of seconds which logs may be buffered for.
			(char*) "logbufsecs",
#define MINLOGLEVEL    7 // Log suppression level: messages logged at a lower level than this
			// are suppressed.
			(char*) "level",

#define LOG_DIR    8 // If specified, logfiles are written into this directory instead of the
			// default logging directory.
			(char*) "logdir",
#define LOG_LINK    9 // Sets the path of the directory into which to put additional links
			// to the log files.
			(char*) "loglink",
#define V_LEVEL    10 //default maximal active V-logging level
			(char*) "vlevel",
#define MAX_LOG_SIZE    11//Sets the maximum log file size (in MB).
			(char*) "max_log_size",

#define TOSYSLOG    12//to syslog
			(char*) "tosyslog",
#define TOSERVER   13//to tcp server, example: toserver="10.0.0.1:5002"
			(char*) "toserver",
#define FILE_NAME   14//File name
			(char*) "file",
#define VMODULE_ARG   15
// module=<str> Gives the per-module maximal V-logging levels to override
// the value given by --v.
// E.g. "my_module=2,foo*=3" would change the logging level
// for all code in source files "my_module.*" and "foo*.*"
// ("-inl" suffixes are also disregarded for this matching).
			(char*) "module",
#define OUTPUT_FUNC   16
			(char*) "func", NULL };
	(void)verbose_opts;
	char opt;
	while ((opt = getopt(argc, argv, "v:")) != -1)
		switch (opt)
		{
		case 'v':
		{
#if defined(GLOG) || defined(CPLUS_LOG)
			char *options = optarg;
			char *value = NULL;
			while (*options != '\0')
			{
				switch (getsubopt(&options, verbose_opts, &value))
				{
				case LOGTOSTDERR: //+
					FLAGS_logtostderr= !FLAGS_logtostderr;
					break;

					case ALSOLOGTOSTDERR: //+
					FLAGS_alsologtostderr = !FLAGS_alsologtostderr;
					break;

					case COLORLOGTOSTDERR:
					FLAGS_colorlogtostderr = !FLAGS_colorlogtostderr;
					break;

					case STDERRTHRESHOLD://+
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[STDERRTHRESHOLD]
							+ "- must have value.");

					FLAGS_stderrthreshold = atoi(value);//FIXME NSHARE
					break;

					case LOG_PREFIX:
					FLAGS_log_prefix = !FLAGS_log_prefix;
					break;

					case LOGBUFLEVEL://+
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[LOGBUFLEVEL]
							+ "- must have value.");
					FLAGS_logbuflevel = atoi(value);
					break;

					case LOGBUFSECS:
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[LOGBUFSECS]
							+ "- must have value.");
					FLAGS_logbufsecs = atoi(value);
					break;

					case MINLOGLEVEL:
					{
						if (value == NULL)
						throw std::invalid_argument(
								std::string("Option: ")
								+ verbose_opts[MINLOGLEVEL]
								+ "- must have value.");
						std::string _str(value);
						if (_str == "info")
						FLAGS_minloglevel = INFO;
						else if (_str == "warning")
						FLAGS_minloglevel = WARNING;
						else if (_str == "error")
						FLAGS_minloglevel = ERROR;
						else if (_str == "fatal")
						FLAGS_minloglevel = FATAL;
						else
						throw std::invalid_argument(
								std::string("Option: ")
								+ verbose_opts[MINLOGLEVEL]
								+ "- have invalid value \""
								+ value + "\"");
						break;
					}

					case LOG_DIR: //+
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[LOG_DIR]
							+ "- must have value.");
					FLAGS_log_dir = value;
					break;

					case LOG_LINK://+
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[LOG_LINK]
							+ "- must have value.");
					FLAGS_log_link = value;

					break;

					case V_LEVEL://+
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[V_LEVEL]
							+ "- must have value.");
					FLAGS_v = atoi(value);
					break;

					case MAX_LOG_SIZE://+
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[MAX_LOG_SIZE]
							+ "- must have value.");
					FLAGS_max_log_size = atoi(value);
					break;

					case TOSYSLOG:
#	ifdef GLOG
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[TOSYSLOG]
							+ "- invalidate,  because syslog isn't available by using glog library.");
#	else
					FLAGS_tosyslog = !FLAGS_tosyslog;
#	endif
					break;

					case TOSERVER:
					{
#	ifdef GLOG
						throw std::invalid_argument(
								std::string("Option: ")
								+ verbose_opts[TOSERVER]
								+ "- invalidate,  because syslog isn't available by using glog library.");
#else
						if (value == NULL)
						throw std::invalid_argument(
								std::string("Option: ")
								+ verbose_opts[TOSERVER]
								+ "- must have value. Format =10.0.0.1:5002");
						std::string _str(value);
						unsigned _pos = _str.find_first_of(':');
						if (_pos == _str.npos)
						throw std::invalid_argument(
								std::string("Option: ")
								+ verbose_opts[TOSERVER]
								+ "- must have value. Format =10.0.0.1:5002");
						unsigned _begin=_str[0]=='"'?1:0;
						FLAGS_server_ip = _str.substr(_begin, _pos);
						FLAGS_server_port = atoi(
								_str.substr(_pos + 1, _str.length()-_pos-1-_begin).c_str());
						FLAGS_toserver = !FLAGS_toserver;
#endif
					}

					break;

					case FILE_NAME: //+
					if (value == NULL)
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[FILE_NAME]
							+ "- must have value.");
#	ifdef GLOG
					google::SetLogDestination(INFO,
							std::string(
									FLAGS_log_dir + "/" + value
									+ ".INFO").c_str());
					google::SetLogDestination(WARNING,
							std::string(
									FLAGS_log_dir + "/" + value
									+ ".WARNING").c_str());
					google::SetLogDestination(ERROR,
							std::string(
									FLAGS_log_dir + "/" + value
									+ ".ERROR").c_str());
					google::SetLogDestination(FATAL,
							std::string(
									FLAGS_log_dir + "/" + value
									+ ".FATAL").c_str());
#else
					FLAGS_file_name = value;
#endif
					break;
					case VMODULE_ARG: //+
					{
						if (value == NULL)
						throw std::invalid_argument(
								std::string("Option: ")
								+ verbose_opts[VMODULE_ARG]
								+ "- must have value.");
						NSHARE::CText _str(value);
						_str.MReplaceAll("\"","");//erase all "
						//_str.fin
#	ifdef GLOG
						_str.MReplaceAll(";",",");
#endif
						FLAGS_vmodule = _str.MToStdString();
					}
					break;

					case OUTPUT_FUNC:
#	ifdef GLOG
					throw std::invalid_argument(
							std::string("Option: ")
							+ verbose_opts[OUTPUT_FUNC]
							+ "- invalidate,  because syslog isn't available by using glog library.");
#	else
					FLAGS_output_func_name=!FLAGS_output_func_name;
#	endif
					break;
					default:
					throw std::invalid_argument(
							std::string("Unknown option: ") + value);
					break;

				}
			}
			break;
#else
			throw std::invalid_argument(
					std::string("Option \"-v\" is not available, because \"NOLOG\" macro is set."));
#endif //#ifndef NOLOG
		}

		default:
			break;
		}
	return 0;
}
#if defined(GLOG) ||defined(CPLUS_LOG)
//#ifndef NOLOG
#	ifdef STANDART_LOG
struct standart_log_setting
{
	static char* name()
	{
		static char g_name[32] = "TODO";
		return g_name;
	}
	standart_log_setting()
	{
		FLAGS_log_dir = "/var/log";
		FLAGS_minloglevel = google::WARNING;
		FLAGS_logbuflevel = google::INFO;

		char *g_p_name[1] =
		{	name()};
		init_trace(1,g_p_name);
		FLAGS_v = 3;
		FLAGS_logbuflevel = 3;
		FLAGS_logbufsecs = 4;
	}
};
__attribute__((constructor)) void init()
{
	static standart_log_setting _init;
	_init.name();
}
#	endif//#SSTANDART_LOG
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
extern "C"  void install_failure_signal_handler()
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
		if (_thread_id==g_entered_thread_id_pointer)
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
#endif//#elif defined(__MINGW32__)//_POSIX_VERSION

#endif//#ifndef NOLOG
