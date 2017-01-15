/*
 * trace_log4cplus.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifdef CPLUS_LOG
#ifdef NOLOG
#	undef NOLOG
#endif
#include <deftype>
//#include <unistd.h>
#include <logging/share_trace_log4cplus.h>
#include <logging/CTraceToSocket.h>
#define DEFINE_FLAG( type, name, defvalue) \
		/*static std::string flags_##name##_to_str();*/\
		 type& _log4cplus_impl::flags_##name() \
		{static type _v=/*log_state::sMPut(flags_##name##_to_str)||true? defvalue:*/defvalue;\
		return _v;}
/*		static std::string flags_##name##_to_str()\
		{\
			std::stringstream _buf;\
			_buf<<#name<<" = "<<_log4cplus_impl::flags_##name();\
			return _buf.str();\
		}

class log_state
{

public:
	typedef std::string (*func_t)();
	typedef std::vector<func_t> funcs_t;
	static bool sMPut(func_t aF)
	{
		sMArray().push_back(aF);
		return true;
	}
	static std::ostream& sMPrint(std::ostream& aStream)
	{
		for (funcs_t::const_iterator _it = sMArray().begin();
				_it != sMArray().end(); ++_it)
		{
			if (_it != sMArray().begin())
				aStream << std::endl;
			aStream << (*_it)();
		}

		return aStream;
	}
private:

	static std::vector<std::string (*)()>& sMArray()
	{
		static std::vector<std::string (*)()> _v;
		return _v;
	}
};*/
DEFINE_FLAG(bool, logtostderr, false)
;

DEFINE_FLAG(bool, alsologtostderr, false)
;

DEFINE_FLAG(bool, colorlogtostderr, false)
;

DEFINE_FLAG(int, stderrthreshold, ERROR)
;

DEFINE_FLAG(bool, log_prefix, false)
;

DEFINE_FLAG(int, logbuflevel, INFO)
;

DEFINE_FLAG(int, logbufsecs, -1)
;

DEFINE_FLAG(std::string, log_dir, ".")
;

DEFINE_FLAG(std::string, log_link, "")
;


DEFINE_FLAG(int, max_log_size, -1)
;

DEFINE_FLAG(bool, stop_logging_if_full_disk, true)
;

DEFINE_FLAG(bool, tosyslog, false)
;

DEFINE_FLAG(bool, toserver, false)
;

DEFINE_FLAG(bool, tosocket, false)
;

DEFINE_FLAG(bool, output_func_name, false)
;

DEFINE_FLAG(int, server_port, 6001)
;

DEFINE_FLAG(std::string, server_ip, "127.0.0.1")
;

DEFINE_FLAG(std::string, file_name, "")
;

DEFINE_FLAG(std::string, socket_setting, "")
;


std::string const& _log4cplus_impl::std_patern()
{
	static const std::string _pattern = FLAGS_output_func_name?
	"%-5p %d{%d.%m.%y %H:%M:%S.%q} %l - %M] %t %m\n" :
	"%-5p %d{%d.%m.%y %H:%M:%S.%q} %l] %t %m\n";
	return _pattern;
}
void _log4cplus_impl::create_sym_link(std::string const& filename,
		eLavel aLevel)
{
	// take directory from filename
	size_t _slash = filename.find_last_of('/');
	const std::string linkname = (FLAGS_file_name.empty()?programm_short_name():(FLAGS_file_name+".link"))+ '.'+ level_to_string(aLevel) + ".log";
	std::string linkpath;
	if (_slash != filename.npos)
		linkpath = filename.substr(0, _slash - 1); // get dirname
	linkpath += linkname;
	unlink(linkpath.c_str()); // delete old one if it exists

#if defined(HAVE_UNISTD_H)
	// We must have unistd.h.
	std::string linkdest= _slash!=filename.npos ? filename.substr(_slash+1,filename.npos) : filename;
	if (symlink(linkdest.c_str(), linkpath.c_str()) != 0)
	{

	}
	// Make an additional link to the log file in a place specified by
	// FLAGS_log_link, if indicated
	if (!FLAGS_log_link.empty())
	{
		linkpath = FLAGS_log_link + "/" + linkname;
		unlink(linkpath.c_str()); // delete old one if it exists
		if (symlink(filename.c_str(), linkpath.c_str()) != 0)
		{
			// silently ignore failures
		}
	}
#endif

}
int _log4cplus_impl::convert(eLavel aLevel)
{
	switch (aLevel)
	{
	case INFO:
		return log4cplus::INFO_LOG_LEVEL;
		break;
	case WARNING:
		return log4cplus::WARN_LOG_LEVEL;
		break;

	case ERROR:
		return log4cplus::ERROR_LOG_LEVEL;
		break;

	case FATAL:
		return log4cplus::FATAL_LOG_LEVEL;
		break;

	case LEVEL_NOLOG:
		return log4cplus::OFF_LOG_LEVEL;
		break;
	}
	return log4cplus::NOT_SET_LOG_LEVEL;
}
bool _log4cplus_impl::check_logger_avaible(char const * aFile, unsigned aLine)
{
	bool _is = _log4cplus_impl::is_inited();
	if (!_is)
	{
		std::cerr << "Logging is not inited but in " << aFile << ":" << aLine
				<< " logging is requrement.";
		//MASSERT_1(_log4cplus_impl::is_inited());
	}
	return _is;
}
log4cplus::Logger& _log4cplus_impl::get_console_logger(eLavel _level)
{
	MASSERT_1(_log4cplus_impl::is_inited());
	static const std::string _name = std::string("LoggingToConsole");
	static log4cplus::Logger _logger = log4cplus::Logger::getInstance(_name);
	if (!_logger.getAllAppenders().empty())
		return _logger;

	log4cplus::SharedAppenderPtr _append;
	bool _is_std_err = true;
	bool _is_flush = _level > FLAGS_logbuflevel;
	std::auto_ptr<log4cplus::Layout> _layout(
			new log4cplus::PatternLayout(_log4cplus_impl::std_patern()));

	_append = new log4cplus::ConsoleAppender(_is_std_err, _is_flush);
	_append->setName("LoggingToConsole");
	_append->setLayout(_layout);
	_logger.setLogLevel(convert(INFO));//fixme by default send all
	_logger.addAppender(_append);
	return _logger;

}
bool& _log4cplus_impl::is_inited()
{
	return NSHARE::logging_impl::is_inited();
}
log4cplus::Logger& _log4cplus_impl::get_socket_logger()
{
	MASSERT_1(_log4cplus_impl::is_inited());

	static const std::string _name = programm_short_name();
	static log4cplus::Logger _logger = log4cplus::Logger::getInstance(_name);

	if (!_logger.getAllAppenders().empty())
		return _logger;

	log4cplus::SharedAppenderPtr _append;
	std::auto_ptr<log4cplus::Layout> _layout(
			new log4cplus::PatternLayout(_log4cplus_impl::std_patern()));

	NSHARE::CConfig _conf;
	_conf.MFromJSON(FLAGS_socket_setting);

	_append = new NSHARE::CTraceToSocket(_conf);
	_append->setName("LoggingToSocket");
	_logger.setLogLevel(convert(INFO));//fixme by default send all
	_logger.addAppender(_append);
	return _logger;

}
log4cplus::Logger& _log4cplus_impl::get_tcp_logger()
{
	MASSERT_1(_log4cplus_impl::is_inited());
	static const std::string _name = programm_short_name();
	static log4cplus::Logger _logger = log4cplus::Logger::getInstance(_name);
	;
	if (!_logger.getAllAppenders().empty())
		return _logger;

	log4cplus::SharedAppenderPtr _append;
	std::auto_ptr<log4cplus::Layout> _layout(
			new log4cplus::PatternLayout(_log4cplus_impl::std_patern()));
	_append = new log4cplus::Log4jUdpAppender(FLAGS_server_ip, FLAGS_server_port);
	_append->setName("LoggingToServer");
	_logger.setLogLevel(convert(INFO));//fixme by default send all
	_logger.addAppender(_append);
	return _logger;
}

log4cplus::SharedAppenderPtr _log4cplus_impl::create_file_appender(
		eLavel alevel, bool aIsFlush)
{
	log4cplus::SharedAppenderPtr _append;
	std::string const _file_name = gen_file_name(alevel);
	if (FLAGS_minloglevel>alevel)
	{
		_append=new log4cplus::NullAppender;
		return _append;
	}

	//bool _is_flush = alevel > FLAGS_logbuflevel;
	if (!FLAGS_logtostderr)
	{
		std::auto_ptr<log4cplus::Layout> _layout(
				new log4cplus::PatternLayout(_log4cplus_impl::std_patern()));

		if(FLAGS_max_log_size>0)
		_append = new log4cplus::RollingFileAppender(_file_name,FLAGS_max_log_size*1024*1024,std::ios::trunc,aIsFlush);
		else
		_append = new log4cplus::FileAppender(_file_name,std::ios::trunc,aIsFlush);
		_append->setName("LogFile"+programm_short_name()+level_to_string(alevel));
		_append->setLayout(_layout);
	}
	create_sym_link(_file_name, alevel);
	return _append;
}
std::string& _log4cplus_impl::programm_short_name()
{
	static std::string _v = "";
	return _v;
}
time_t const* _log4cplus_impl::timestamp()
{
	static time_t _time = ::time(NULL);
	return &_time;
}
std::string _log4cplus_impl::gen_file_name(eLavel aLevel)
{
	std::string _name = FLAGS_log_dir+"/";

	if(FLAGS_file_name.empty())
	{
		_name += programm_short_name() + ".";
		std::ostringstream _pid_stream;
		_pid_stream<<NSHARE::CThread::sMPid()<<".";
		_name +=_pid_stream.str();
#ifdef _WIN32
			const char* _user=getenv("USERNAME");
#else
			const char* _user = getenv("USER");
			;
#endif
			if (_user) _name +=std::string( _user) + ".";

			_name += level_to_string(aLevel) + '.';

			struct ::tm tm_time;
#ifndef _WIN32
			localtime_r(timestamp(), &tm_time);
#else
			struct ::tm *_p;
			_p=::localtime(timestamp());
			assert(_p);
			::memcpy(&tm_time,_p,sizeof(tm_time));
#endif

			std::ostringstream time_pid_stream;
			time_pid_stream.fill('0');
			time_pid_stream << 1900 + tm_time.tm_year << "_" << std::setw(2)
			<< 1 + tm_time.tm_mon << "_" << std::setw(2) << tm_time.tm_mday
			<< '-' << std::setw(2) << tm_time.tm_hour << "_"
			<< std::setw(2) << tm_time.tm_min << "_" << std::setw(2)
			<< tm_time.tm_sec << '.';
			_name+=time_pid_stream.str();
			_name += "log";
		}
		else
		_name+=FLAGS_file_name+"."+level_to_string(aLevel)+".log";

		return _name;
	}
void _log4cplus_impl::init_log4cplus(char const *argv)
{
	if (is_inited())
		return;
	is_inited() = true;

	const char* slash = strrchr(argv, '/');
#ifdef _WIN32
	if (!slash)
		slash = strrchr(argv, '\\');
#endif
	programm_short_name() = slash ? slash + 1 : argv;

	timestamp();
}



static NSHARE::CMutex _mutex[LEVEL_NOLOG];
void _log4cplus_impl::lock(eLavel aLevel)
{

	switch (aLevel)
	{
	case INFO:
	case WARNING:
	case ERROR:
	case FATAL:
		_mutex[aLevel].MLock();
		break;
	case LEVEL_NOLOG:
		break;
	}

}
void _log4cplus_impl::unlock(eLavel aLevel)
{
	switch (aLevel)
	{
	case INFO:
	case WARNING:
	case ERROR:
	case FATAL:
		_mutex[aLevel].MUnlock();
		break;
	case LEVEL_NOLOG:
		break;
	}

}
template<eLavel alevel>
log4cplus::SharedAppenderPtr _log4cplus_impl::get_file_appender(bool aIsFlush)
{
	static log4cplus::SharedAppenderPtr _append;
	if (_append.get())
		return _append;
	_append = create_file_appender(alevel, aIsFlush);
	return _append;
}
inline void _log4cplus_impl::add_files_apender_to(eLavel alevel,
		log4cplus::Logger&_logger, bool _is_flush)
{
	switch (alevel)
	{
	case WARNING:
		_logger.addAppender(get_file_appender < WARNING > (_is_flush));
		add_files_apender_to(INFO, _logger, _is_flush); //FIXME problems with correct writing
		break;
	case ERROR:
		_logger.addAppender(get_file_appender < ERROR > (_is_flush));
		add_files_apender_to(WARNING, _logger, _is_flush); //FIXME problems with correct writing
		break;

	case FATAL:
		_logger.addAppender(get_file_appender < FATAL > (_is_flush));
		add_files_apender_to(ERROR, _logger, _is_flush); //FIXME problems with correct writing
		break;
	case INFO:
		_logger.addAppender(get_file_appender<INFO>(_is_flush));
		break;
	case LEVEL_NOLOG:
	{
		static log4cplus::SharedAppenderPtr _append(
				new log4cplus::NullAppender);
		_logger.addAppender(_append);
	}
		break;
	}
}
template<eLavel alevel>
log4cplus::Logger& _log4cplus_impl::get_logging_to_file_impl()
{
	const eLavel _level = alevel;
	static const std::string _name = std::string("Logging")
			+ level_to_string(_level);
	static log4cplus::Logger _logger = log4cplus::Logger::getInstance(_name);
	if (!_logger.getAllAppenders().empty())
		return _logger;

	bool _is_flush = alevel > FLAGS_logbuflevel;
	add_files_apender_to(alevel, _logger, _is_flush);

	_logger.setLogLevel(convert(_level)); //FIXME
	return _logger;
}

void _log4cplus_impl::abort_impl(const char* aWhat)
{
	using namespace std;
	cerr<<aWhat<<endl;
	log4cplus::Logger::getRoot().shutdown();
	//throw std::runtime_error(aWhat);
	std::abort();
}
log4cplus::Logger& _log4cplus_impl::get_logging_to_file(eLavel alevel)
{
	switch (alevel)
	{
	case WARNING:
		return get_logging_to_file_impl<WARNING>();
	case ERROR:
		return get_logging_to_file_impl<ERROR>();
	case FATAL:
		return get_logging_to_file_impl<FATAL>();
	case INFO:
		return get_logging_to_file_impl<INFO>();
		break;
	case LEVEL_NOLOG:
		return get_logging_to_file_impl<LEVEL_NOLOG>();
	}
	static log4cplus::Logger*_p=NULL;
	abort_impl("Error no logger ");
	return *_p;
}
log4cplus::Logger& _log4cplus_impl::get_syslog_logger(eLavel _level)
{
	static const std::string _name = std::string("LoggingToSysLog")
			+ level_to_string(_level);
	static log4cplus::Logger _logger = log4cplus::Logger::getInstance(_name);
	if (!_logger.getAllAppenders().empty())
		return _logger;

	std::auto_ptr<log4cplus::Layout> _layout(
			new log4cplus::PatternLayout(_log4cplus_impl::std_patern()));
	log4cplus::SharedAppenderPtr _append;
#if defined (LOG4CPLUS_HAVE_SYSLOG_H)
	_append = new log4cplus::SysLogAppender(_name);
#else
#	ifndef _WIN32
#		warning "log4cplus have not syslog header."
#	endif
	_append = new log4cplus::SysLogAppender(_name, "127.0.0.1");
#endif
	_append->setName("LogSysLog");
	_append->setLayout(_layout);
	_logger.addAppender(_append);
	_logger.setLogLevel(convert(INFO));//fixme
	return _logger;
}
log4cplus::Logger& _log4cplus_impl::logger(eLavel TLevelType, eLoggerType aType)
{
	switch (aType)
	{
	case TOFILE:
		return get_logging_to_file(TLevelType);
		break;
	case SYSLOG:
		return get_syslog_logger(TLevelType);
		break;
	case STDERR:
		return get_console_logger(TLevelType);
		break;
	case TOTCP:
		return get_tcp_logger();
		break;

	case SOCKET_LOGGER:
		return get_socket_logger();
		break;
	}
	std::cerr << "Invalide Logger type " << aType << std::endl;
	static log4cplus::Logger _def_logger = log4cplus::Logger::getRoot();
	return _def_logger;
}

extern SHARE_EXPORT void init_trace_cplus(char const*aProgrammName)
{
	_log4cplus_impl::init_log4cplus(aProgrammName);
	std::set_terminate(get_log_terminate_handler());
	install_failure_signal_handler();
}
#else
#include <macro_attributes.h>
extern SHARE_EXPORT void init_trace_cplus(char const*aProgrammName)
{
}
#endif
