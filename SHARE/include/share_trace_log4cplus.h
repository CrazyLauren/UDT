/*
 *
 * share_trace_log4cplus.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 07.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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

#ifdef __MINGW32__
#	undef ERROR
#endif

#ifndef COMPILE_ASSERT
#	define COMPILE_ASSERT(aVal , aVal2)
#endif

enum eLavel
{
	INFO = 0, WARNING, ERROR, FATAL, LEVEL_NOLOG,
};
#ifdef NDEBUG
#	define DFATAL ERROR
#else
#	define	DFATAL FATAL
#endif
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

#define LOG_ASSERT(condition)  \
  LOG_IF(FATAL, !(condition)) << "Assert failed: " #condition

#define LOG_EVERY_N(severity, n)                                        \
		COMPILE_ASSERT( severity <= FATAL ,        INVALID_LOG_LEVEL);\
		static unsigned long long LOG_OCCURRENCES##severity##n = 0; \
		  if (!n || ++LOG_OCCURRENCES##severity##n%n==0)  LOG(severity)

#define LOG_IF_EVERY_N(severity, condition, n)\
		COMPILE_ASSERT( severity <= FATAL ,        INVALID_LOG_LEVEL);\
		static unsigned long long LOG_OCCURRENCES##n = 0; \
		  if ((!n || ++LOG_OCCURRENCES##n%n==0)&& (condition))  LOG(severity)

#define VLOG(verboselevel) LOG_IF(INFO, VLOG_IS_ON(verboselevel))

#define VLOG_IF(verboselevel, condition) \
  LOG_IF(INFO, (condition) && VLOG_IS_ON(verboselevel))

#define VLOG_EVERY_N(verboselevel, n) \
  LOG_IF_EVERY_N(INFO, VLOG_IS_ON(verboselevel), n)

#define VLOG_IF_EVERY_N(verboselevel, condition, n) \
  LOG_IF_EVERY_N(INFO, (condition) && VLOG_IS_ON(verboselevel), n)

#if defined(__GNUC__)
#define VLOG_IS_ON(verboselevel)                                \
  __extension__  \
  ({ static int* _vlocal = &_log4cplus_impl::vlog_uniitialized;/*vlog_uniitialized is very big num*/\
     (*_vlocal >= verboselevel) &&                          \
     (\
/*if _vlocal was inited, comparison "*_vlocal >= verboselevel" is valid.*/\
(_vlocal != &_log4cplus_impl::vlog_uniitialized ) ||   \
/*try to init _vlocal, and comparing "_vlocal" and  "verboselevel" again */\
(_log4cplus_impl::is_inited() &&_log4cplus_impl::init_gnu_vlog(&_vlocal, &FLAGS_v, __FILE__, verboselevel)) \
/*Cann't init _vlocal, using share flag*/\
|| (!_log4cplus_impl::is_inited() && FLAGS_v >= (verboselevel))\
	 ); })
#else
#define VLOG_IS_ON(verboselevel)\
		(FLAGS_v >= (verboselevel) || ((_log4cplus_impl::is_inited() && !_log4cplus_impl::get_modules().empty())&& _log4cplus_impl::is_v_log_on(__FILE__,verboselevel)))

#endif
#define CHECK(condition)  \
      LOG_IF(FATAL, !(condition)) \
             << "Check failed: " #condition " "

#define CHECK_EQ(val1, val2)\
	for(; !((val1)==(val2)); NSHARE::sleep(1))\
	LOG(FATAL)<<(val1)<<" != "<<(val2)

#define CHECK_NE(val1, val2)\
	for( ;!((val1)!=(val2));NSHARE::sleep(1))\
	LOG(FATAL)<<(val1)<<" == "<<(val2)

#define CHECK_LE(val1, val2)\
	for( ;!((val1)<=(val2)); NSHARE::sleep(1))\
	LOG(FATAL)<<(val1)<<" > "<<(val2)

#define CHECK_LT(val1, val2)\
	for( ;!((val1)<(val2)); NSHARE::sleep(1))\
	LOG(FATAL)<<(val1)<<" >= "<<(val2)

#define CHECK_GE(val1, val2)\
	for(; !((val1)>=(val2));NSHARE::sleep(1))\
	LOG(FATAL)<<(val1)<<" < "<<(val2)

#define CHECK_GT(val1, val2)\
	for(; !((val1)>(val2)); NSHARE::sleep(1))\
	LOG(FATAL)<<(val1)<<" <= "<<(val2)

#define CHECK_NOTNULL(val) \
		LOG_IF(FATAL, val==NULL)<<"'"<< #val<< "' Must be non NULL. "

#define CHECK_STREQ(s1, s2) \
		for( ;strcmp(s1,s2); NSHARE::sleep(1))\
		LOG(FATAL)<<(s1)<<" -not equal- "<<(s2)

#define CHECK_STRNE(s1, s2) \
		for(; !(strcmp(s1,s2));NSHARE::sleep(1))\
		LOG(FATAL)<<(s1)<<" -equal- "<<(s2)

#define PLOG(severity) LOG(severity)<<(errno==0?"": ::strerror(errno))<<" "

#define PLOG_IF(severity, condition) \
		 !(condition) ? (void) 0 : _log4cplus_impl::__gag_t()^PLOG(severity)

#define CHECK_ERR(invocation)\
	PLOG_IF(FATAL, (invocation) == -1)<< #invocation

#ifndef NDEBUG

#define DLOG(severity) LOG(severity)
#define DVLOG(verboselevel) VLOG(verboselevel)
#define DLOG_IF(severity, condition) LOG_IF(severity, condition)
#define DLOG_EVERY_N(severity, n) LOG_EVERY_N(severity, n)
#define DLOG_IF_EVERY_N(severity, condition, n) \
  LOG_IF_EVERY_N(severity, condition, n)
#define DLOG_IF_EVERY_TIME(severity, condition,period)\
		 LOG_IF_EVERY_N(severity, condition, period)
#define DLOG_ASSERT(condition) LOG_ASSERT(condition)

// debug-only checking.  not executed in NDEBUG mode.
#define DCHECK(condition) CHECK(condition)
#define DCHECK_EQ(val1, val2) CHECK_EQ(val1, val2)
#define DCHECK_NE(val1, val2) CHECK_NE(val1, val2)
#define DCHECK_LE(val1, val2) CHECK_LE(val1, val2)
#define DCHECK_LT(val1, val2) CHECK_LT(val1, val2)
#define DCHECK_GE(val1, val2) CHECK_GE(val1, val2)
#define DCHECK_GT(val1, val2) CHECK_GT(val1, val2)
#define DCHECK_NOTNULL(val) CHECK_NOTNULL(val)
#define DCHECK_STREQ(str1, str2) CHECK_STREQ(str1, str2)
#define DCHECK_STRCASEEQ(str1, str2) CHECK_STRCASEEQ(str1, str2)

#else  // NDEBUG
#define DLOG(severity) \
		true?(void)0:_log4cplus_impl::__gag_t()^std::cout

#define DVLOG(verboselevel) \
		DLOG(0)

#define DLOG_IF(severity, condition) \
		DLOG(0)

#define DLOG_EVERY_N(severity, n) \
		DLOG(0)

#define DLOG_IF_EVERY_N(severity, condition, n) \
		DLOG(0)

#define DLOG_IF_EVERY_TIME(severity, condition, n) \
		DLOG(0)

#define DLOG_ASSERT(condition) \
		DLOG(0)

// MSVC warning C4127: conditional expression is constant
#define DCHECK(condition) \
		DLOG(0)

#define DCHECK_EQ(val1, val2) \
		DLOG(0)

#define DCHECK_NE(val1, val2) \
		DLOG(0)

#define DCHECK_LE(val1, val2) \
		DLOG(0)

#define DCHECK_LT(val1, val2) \
		DLOG(0)

#define DCHECK_GE(val1, val2) \
		DLOG(0)

#define DCHECK_GT(val1, val2) \
		DLOG(0)

#define DCHECK_NOTNULL(val) (val)

#define DCHECK_STREQ(str1, str2) \
		DLOG(0)

#define DCHECK_STRCASEEQ(str1, str2) \
		DLOG(0)

#define DCHECK_STRNE(str1, str2) \
		DLOG(0)

#define DCHECK_STRCASENE(str1, str2) \
		DLOG(0)
#endif  // NDEBUG
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

// Log suppression level: messages logged at a lower level than this
// are suppressed.
#define FLAGS_minloglevel DECLARE_flags(minloglevel)

// If specified, logfiles are written into this directory instead of the
// default logging directory.
#define FLAGS_log_dir DECLARE_flags(log_dir)

// Sets the path of the directory into which to put additional links
// to the log files.
#define FLAGS_log_link DECLARE_flags(log_link)

#define FLAGS_v DECLARE_flags(v)  // in vlog_is_on.cc
// Sets the maximum log file size (in MB).
#define FLAGS_max_log_size DECLARE_flags(max_log_size)

// Sets whether to avoid logging to the disk if the disk is full.
#define FLAGS_stop_logging_if_full_disk DECLARE_flags(stop_logging_if_full_disk)

#define FLAGS_tosyslog DECLARE_flags(tosyslog)

#define FLAGS_toserver DECLARE_flags(toserver)

#define FLAGS_server_port DECLARE_flags(server_port)

#define FLAGS_server_ip DECLARE_flags(server_ip)

#define FLAGS_file_name DECLARE_flags(file_name)

#define FLAGS_vmodule DECLARE_flags(vmodule)

#define FLAGS_output_func_name DECLARE_flags(output_func_name)

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

	DEFINE_FLAG(int, minloglevel)

	DEFINE_FLAG(std::string, log_dir)

DEFINE_FLAG	(std::string, log_link)

	DEFINE_FLAG(int, v)

	DEFINE_FLAG(int, max_log_size)

	DEFINE_FLAG(bool, stop_logging_if_full_disk)

	DEFINE_FLAG(bool, tosyslog)

	DEFINE_FLAG(bool, toserver)

	DEFINE_FLAG(bool, output_func_name)

	DEFINE_FLAG(int, server_port)

	DEFINE_FLAG(std::string, server_ip)

	DEFINE_FLAG(std::string, file_name)

	DEFINE_FLAG(std::string, vmodule)
#undef DEFINE_FLAG

	enum eLoggerType
	{
		STDERR = 0, SYSLOG, TOFILE, TOTCP
	};
	struct module_info_t
	{
		std::string FPattern;
		mutable int FLevel;
	};
	static void init_log4cplus(char *argv);
	static std::string const& std_patern();
	static void create_sym_link(std::string const& filename, eLavel aLevel);
	static int convert(eLavel aLevel);
	static std::string& programm_short_name();
	static time_t const* timestamp();
	static std::string gen_file_name(eLavel aLevel);
	static std::list<module_info_t> init_modules();
	static std::list<module_info_t>& get_modules();
	static log4cplus::Logger& get_console_logger(eLavel _level);
	static bool check_logger_avaible(char const *,unsigned);
	static log4cplus::Logger& get_tcp_logger();
	static bool& is_inited();
// Implementation of fnmatch that does not need 0-termination
// of arguments and does not allocate any memory,
// but we only support "*" and "?" wildcards, not the "[...]" patterns.
	static bool fnmatch(const char* pattern, size_t patt_len, const char* str,
			size_t str_len)//from google
	;
	static bool is_file_level(int**,const char* aFile);
	static int get_file_level(const char* aFile);
	static bool is_v_log_on(const char* aFile, int verboselevel);
	static int vlog_uniitialized;
	static bool init_gnu_vlog(int**, int*,const char* aFile, int verboselevel);
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

				if (FLAGS_toserver)
				MOutPut<TOTCP>();

				unlock(Level);

				if(Level==FATAL)
				{
					abort_impl("fatal");
				}
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
