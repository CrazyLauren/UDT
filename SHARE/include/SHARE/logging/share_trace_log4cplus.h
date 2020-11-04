/*
 *
 * share_trace_log4cplus.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_TRACE_LOG4CPLUS_H_
#define SHARE_TRACE_LOG4CPLUS_H_
#include <log4cplus/logger.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/nullappender.h>
#include <log4cplus/syslogappender.h>
#include <log4cplus/log4judpappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>
#include <string.h>
#include <iomanip>
#include <SHARE/logging/trace_macros.h>

#	undef INFO
#	undef WARNING
#	undef ERROR
#	undef FATAL
#ifndef LOG4CPLUS_MACRO_FUNCTION
#	define LOG4CPLUS_MACRO_FUNCTION() "unknown"
#endif

enum eLavel
{
	INFO = 0, WARNING, ERROR, FATAL, LEVEL_NOLOG,
};

// be sure in series

inline std::string level_to_string(eLavel aLevel)
{
	switch (aLevel)
	{
	case INFO:
		return "INFO";
		break;
	case WARNING:
		return "WARNING";
		break;

	case ERROR:
		return "ERROR";
		break;

	case FATAL:
		return "FATAL";
		break;

	case LEVEL_NOLOG:
		return "LEVEL_NOLOG";
		break;
	}
	return "Unknown";
}
#define LOG(severity)  _log4cplus_impl::__logging_t<severity>(__FILE__,__LINE__,LOG4CPLUS_MACRO_FUNCTION())

#define LOG_IF(severity, condition) \
  !(condition) ? (void) 0 : _log4cplus_impl::__gag_t()^LOG(severity)

#define PLOG_IF(severity, condition) \
		 !(condition) ? (void) 0 : _log4cplus_impl::__gag_t()^PLOG(severity)

#define DECLARE_flags( name )\
		_log4cplus_impl::flags_##name()

// Set whether log messages go to stderr instead of logfiles
#define FLAGS_logtostderr DECLARE_flags(logtostderr)

// Set whether log messages go to stderr in addition to logfiles.
#define FLAGS_alsologtostderr DECLARE_flags(alsologtostderr)

// Set color messages logged to stderr (if supported by terminal).
#define FLAGS_colorlogtostderr DECLARE_flags(colorlogtostderr)

// Log messages at a level >= this flag are automatically sent to
// stderr in addition to log files.
#define FLAGS_stderrthreshold DECLARE_flags(stderrthreshold)

// Set whether the log prefix should be prepended to each line of output.
#define FLAGS_log_prefix DECLARE_flags(log_prefix)

// Log messages at a level <= this flag are buffered.
// Log messages at a higher level are flushed immediately.
#define FLAGS_logbuflevel DECLARE_flags(logbuflevel)

// Sets the maximum number of seconds which logs may be buffered for.
#define FLAGS_logbufsecs DECLARE_flags(logbufsecs)


// If specified, logfiles are written into this directory instead of the
// default logging directory.
#define FLAGS_log_dir DECLARE_flags(log_dir)

// Sets the path of the directory into which to put additional links
// to the log files.
#define FLAGS_log_link DECLARE_flags(log_link)

// Sets the maximum log file size (in MB).
#define FLAGS_max_log_size DECLARE_flags(max_log_size)

// Sets whether to avoid logging to the disk if the disk is full.
#define FLAGS_stop_logging_if_full_disk DECLARE_flags(stop_logging_if_full_disk)

#define FLAGS_tosyslog DECLARE_flags(tosyslog)

#define FLAGS_toserver DECLARE_flags(toserver)

#define FLAGS_server_port DECLARE_flags(server_port)

#define FLAGS_server_ip DECLARE_flags(server_ip)

#define FLAGS_tosocket DECLARE_flags(tosocket)

#define FLAGS_socket_setting DECLARE_flags(socket_setting)

#define FLAGS_file_name DECLARE_flags(file_name)

#define FLAGS_output_func_name DECLARE_flags(output_func_name)

#define FLAGS_short_name DECLARE_flags(short_name)

struct SHARE_EXPORT _log4cplus_impl
{

#define DEFINE_FLAG( type, name)\
		static type& flags_##name();

	DEFINE_FLAG(bool, logtostderr)

	DEFINE_FLAG(bool, alsologtostderr)

	DEFINE_FLAG(bool, colorlogtostderr)

	DEFINE_FLAG(int, stderrthreshold)

	DEFINE_FLAG(bool, log_prefix)

	DEFINE_FLAG(int, logbuflevel)

	DEFINE_FLAG(int, logbufsecs)

	DEFINE_FLAG(std::string, log_dir)

DEFINE_FLAG	(std::string, log_link)



	DEFINE_FLAG(int, max_log_size)

	DEFINE_FLAG(bool, stop_logging_if_full_disk)

	DEFINE_FLAG(bool, tosyslog)

	DEFINE_FLAG(bool, toserver)

	DEFINE_FLAG(bool, tosocket)

	DEFINE_FLAG(bool, output_func_name)

	DEFINE_FLAG(int, server_port)

	DEFINE_FLAG(std::string, server_ip)

	DEFINE_FLAG(std::string, file_name)

	DEFINE_FLAG(std::string, socket_setting)

	DEFINE_FLAG(bool, short_name)

#undef DEFINE_FLAG

	enum eLoggerType
	{
		STDERR = 0, SYSLOG, TOFILE, TOTCP,SOCKET_LOGGER
	};
	static void init_log4cplus(char const *argv);
	static std::string const& std_patern();
	static void create_sym_link(std::string const& filename, eLavel aLevel);
	static int convert(eLavel aLevel);
	static std::string& programm_short_name();
	static time_t const* timestamp();
	static std::string gen_file_name(eLavel aLevel);

	static log4cplus::Logger& get_console_logger(eLavel _level);
	static bool check_logger_avaible(char const *,unsigned);
	static log4cplus::Logger& get_tcp_logger();
	static log4cplus::Logger& get_socket_logger();

	static bool& is_inited();
	static log4cplus::SharedAppenderPtr create_file_appender(eLavel alevel,bool aIsFlush);

	static void lock(eLavel);
	static void unlock(eLavel);

	template<eLavel aLevel>
	static log4cplus::SharedAppenderPtr get_file_appender(bool aIsFlush=false);

	static void add_files_apender_to(eLavel alevel,log4cplus::Logger&_logger,bool);

	template<eLavel alevel>
	static log4cplus::Logger& get_logging_to_file_impl();

	static void abort_impl(const char* );
	static log4cplus::Logger& get_logging_to_file(eLavel alevel);

	static log4cplus::Logger& get_syslog_logger(eLavel _level);

	static log4cplus::Logger& logger(eLavel TLevelType,eLoggerType aType);

	template<eLavel Level>
	struct __logging_t
	{
		char const * FFile;
		int const FLine;
		char const * FFunction;
		log4cplus::tostringstream & FStream;
		const bool FIsDoing;

		__logging_t(char const * aFile, int aLine, char const* aFunction) :
		FFile(aFile), //
		FLine(aLine),//
		FFunction(aFunction),//
		FStream(
				log4cplus::detail::get_macro_body_oss()),//
		FIsDoing(
				Level >= FLAGS_minloglevel && check_logger_avaible(FFile,FLine))
		{
			if(FLAGS_short_name)
			{
				const char* slash = strrchr(aFile, '/');
				#ifdef _WIN32
				if (!slash)
					slash = strrchr(aFile, '\\');
				#endif
				FFile= slash ? slash + 1 : aFile;
			}
		}
		~__logging_t()
		{
			if (FIsDoing)
			{
				lock(Level);

				if(!FLAGS_logtostderr)
				MOutPut<TOFILE>();

				if (FLAGS_logtostderr|| FLAGS_alsologtostderr ||Level>=FLAGS_stderrthreshold)
				MOutPut<STDERR>();

				if (FLAGS_tosyslog)
				MOutPut<SYSLOG>();

				if (FLAGS_toserver)
				MOutPut<TOTCP>();

				if (FLAGS_tosocket)
				MOutPut<SOCKET_LOGGER>();

				unlock(Level);

			}
			if(Level==FATAL)
			{
				abort_impl("fatal");
			}
		}
		template<eLoggerType aType> //I what to mutex for every logers
		inline void MOutPut()
		{
			log4cplus::Logger& _l=logger(Level,aType);
			int const _lev=convert(Level);
			if( _l.isEnabledFor (_lev))
			log4cplus::detail::macro_forced_log (_l,
					_lev, FStream.str(),
					FFile, FLine, FFunction);

		}
		template<typename T>
		inline __logging_t <Level>& operator<<(const T& aStream)
		{
			if (FIsDoing)
			FStream<<aStream;

			return *this;
		}
	};
	struct __gag_t
	{
		template<eLavel Level>
		inline void operator^(__logging_t <Level>&)
		{
		}
		inline void operator^(std::ostream&)
		{
		}
	}
	;
};
//namespace _log4cplus_impl
template<>
struct _log4cplus_impl::__logging_t<LEVEL_NOLOG>
{
	enum
	{
		E_LEVEL = LEVEL_NOLOG
	};
	__logging_t(char const * aFile, int aLine)
	{

	}
	__logging_t()
	{
	}
	template<typename T>
	inline __logging_t <LEVEL_NOLOG>& operator<<(const T& aStream)
	{

		return *this;
	}
	template<typename T>
	inline void MFileOutput(const T& aStream)

	{

	}
}
;

#endif /* SHARE_TRACE_LOG4CPLUS_H_ */
