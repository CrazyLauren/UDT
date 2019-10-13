// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 *
 * CShareLogArgsParser.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.11.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#if defined(GLOG) || defined(COUT_LOG) || defined(CPLUS_LOG)
#	undef NOLOG //if defined NOLOG and the other logging macros - The SHARE library will not
//logging but the user can logging
#elif defined(REMOVE_LOG)
# define NOLOG
#endif
#include <share_socket.h>
#include <logging/CShareLogArgsParser.h>
#include <tclap/Arg.h>
#include <tclap/Constraint.h>
#include <tclap/CmdLine.h>

namespace TCLAP
{
template<>
struct ArgTraits<NSHARE::net_address>
{
	typedef ValueLike ValueCategory;
};
}
namespace NSHARE
{

CShareLogArgsParser::CShareLogArgsParser(const std::string& flag,
		const std::string& name,bool aIgnorable) :
		Arg(flag, name, sMGetDescription(), false, true, NULL)
{
	Arg::_ignoreable=aIgnorable;
	_acceptsMultipleValues = true;
}
CShareLogArgsParser::~CShareLogArgsParser()
{
}
bool CShareLogArgsParser::MIsFlag(const std::string& argFlag) const
{
	if (argFlag.compare(0, flagStartString().length(), flagStartString()) == 0)
	{
		if (!_flag.empty() && //
				(argFlag.compare(flagStartString().length(), _flag.length(),
						_flag) == 0) //
				)
			return true;
	}
	return false;
}
bool CShareLogArgsParser::MIsName(const std::string& argFlag) const
{
	if (argFlag.compare(0, nameStartString().length(), nameStartString()) == 0)
	{
		if (!_name.empty() && //
				(argFlag.compare(nameStartString().length(), _name.length(),
						_name) == 0) //
				)
			return true;
	}
	return false;
}
bool CShareLogArgsParser::argMatches(const std::string& argFlag) const
{
	if (argFlag.empty())
		return false;
	if (MIsFlag(argFlag))
		return true;
	if (MIsName(argFlag))
		return true;
	return false;
}
const std::pair<const char*, const char*> CShareLogArgsParser::FOptsCommects[] =
		{
#define LOGTOSTDERR    0
				std::make_pair("logtostderr",
						"Set whether log messages go to stderr instead of logfiles"),

#define ALSOLOGTOSTDERR   1
				std::make_pair("alsostderr",
						"Set whether log messages go to stderr in addition to logfiles"),

#define COLORLOGTOSTDERR   2
				std::make_pair("colorstderr",
						" Set color messages logged to stderr (if supported by terminal)."),

#define STDERRTHRESHOLD    3
				std::make_pair("stderrthreshold",
						" Log messages at a level >= this flag are automatically sent to"
								"stderr in addition to log files."),

#define LOG_PREFIX    4
				std::make_pair("logprefix",
						" Set whether the log prefix should be prepended to each line of output."),

#define LOGBUFLEVEL    5
				std::make_pair("logbuflevel",
						"Log messages at a level <= this flag are buffered."
								" Log messages at a higher level are flushed immediately."),

#define LOGBUFSECS    6
				std::make_pair("logbufsecs",
						" Sets the maximum number of seconds which logs may be buffered for."),

#define MINLOGLEVEL    7
				std::make_pair("level",
						" Log suppression level: messages logged at a lower level than this"
								" are suppressed."),

#define LOG_DIR    8
				std::make_pair("logdir",
						"If specified, logfiles are written into this directory instead of the"
								" default logging directory."),

#define LOG_LINK    9
				std::make_pair("loglink",
						" Sets the path of the directory into which to put additional links"
								" to the log files."),

#define V_LEVEL    10
				std::make_pair("vlevel",
						"default maximal active V-logging level"),

#define MAX_LOG_SIZE    11
				std::make_pair("max_log_size",
						"Sets the maximum log file size (in MB)."),

#define TOSYSLOG    12
				std::make_pair("tosyslog", "to syslog"),

#define TOSERVER   13
				std::make_pair("toserver",
						"to tcp server, example: toserver=\"10.0.0.1:5002\""),

#define FILE_NAME   14
				std::make_pair("file", "File name"),

#define VMODULE_ARG   15
				std::make_pair("module",
						" module=\"<name=vlevel[;name=vlevel]...>\" Gives the per-module maximal V-logging levels to override"
								" the value given by vlevel."
								" E.g. \"my_module=2;foo*=3\" would change the logging level"
								" for all code in source files \"my_module.*\" and \"foo*.*\""
								" (\"-inl\" suffixes are also disregarded for this matching)."),

#define OUTPUT_FUNC   16
				std::make_pair("func", " output function name"),

#define TO_SOCKET   17
				std::make_pair("tosocket",
						" Send to socket, Format: \"tosocket=\"Json-config\""),
#define SHORT_FILE_NAME   18
				std::make_pair("short_name", "Removes path from file name")
		//don't forget update methods sMGetDescription and MHandleValue
		};
#ifdef GLOG
#	undef TOSYSLOG
#	undef TOSERVER
#	undef OUTPUT_FUNC
#	undef TO_SOCKET
#elif defined(COUT_LOG)
#	undef LOGTOSTDERR
#	undef ALSOLOGTOSTDERR
#	undef COLORLOGTOSTDERR
#	undef STDERRTHRESHOLD
#	undef LOG_PREFIX
#	undef LOGBUFLEVEL
#	undef LOGBUFSECS
#	undef LOG_DIR
#	undef LOG_LINK
#	undef MAX_LOG_SIZE
#	undef TOSYSLOG
#	undef TOSERVER
#	undef FILE_NAME
#	undef OUTPUT_FUNC
#	undef TO_SOCKET
#	undef SHORT_FILE_NAME
#endif

#define IS_OPT(a) if(!_is&& (_is=(aOption==FOptsCommects[a].first)) )
void CShareLogArgsParser::MHandleValue(const std::string& aOption,
		const std::string& value) const
{
#ifndef NOLOG
	bool _is = false;
#ifdef GLOG
	using namespace google;
	using fLS::FLAGS_vmodule;
#endif

#ifdef 	LOGTOSTDERR
	IS_OPT(LOGTOSTDERR)
	{
		FLAGS_logtostderr= !FLAGS_logtostderr;
	}
#endif
//
#ifdef 	ALSOLOGTOSTDERR
	IS_OPT(ALSOLOGTOSTDERR)
	{
		FLAGS_alsologtostderr= !FLAGS_alsologtostderr;
	}
#endif
#ifdef 	COLORLOGTOSTDERR
	IS_OPT(COLORLOGTOSTDERR)
	{
		FLAGS_colorlogtostderr= !FLAGS_colorlogtostderr;
	}
#endif

#ifdef 	STDERRTHRESHOLD
	IS_OPT(STDERRTHRESHOLD)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");

		FLAGS_stderrthreshold= atoi(value.c_str());
	}
#endif

#ifdef 	LOG_PREFIX
	IS_OPT(LOG_PREFIX)
	{
		FLAGS_log_prefix= !FLAGS_log_prefix;
	}
#endif

#ifdef 	LOGBUFLEVEL
	IS_OPT(LOGBUFLEVEL)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		FLAGS_logbuflevel= atoi(value.c_str());
	}
#endif

#ifdef 	LOGBUFSECS
	IS_OPT(LOGBUFSECS)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		FLAGS_logbufsecs= atoi(value.c_str());
	}
#endif

#ifdef 	MINLOGLEVEL
	IS_OPT(MINLOGLEVEL)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		std::string _str(value);
		if (_str == "info")
			FLAGS_minloglevel= INFO;
			else if (_str == "warning")
			FLAGS_minloglevel = WARNING;
			else if (_str == "error")
			FLAGS_minloglevel = ERROR;
			else if (_str == "fatal")
			FLAGS_minloglevel = FATAL;
			else
			throw TCLAP::ArgParseException(
					std::string("Option: ")
					+ aOption
					+ "- have invalid value \""
					+ value.c_str() + "\"");
		}
#endif

#ifdef 	LOG_DIR
	IS_OPT(LOG_DIR)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		FLAGS_log_dir= value.c_str();
	}
#endif

#ifdef 	LOG_LINK
	IS_OPT(LOG_LINK)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		FLAGS_log_link= value.c_str();
	}
#endif

#ifdef 	V_LEVEL
	IS_OPT(V_LEVEL)
	{
		if (value.empty())

			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		FLAGS_v= atoi(value.c_str());
	}
#endif

#ifdef 	MAX_LOG_SIZE
	IS_OPT(MAX_LOG_SIZE)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		FLAGS_max_log_size= atoi(value.c_str());
	}
#endif

#ifdef 	TOSYSLOG
	IS_OPT(TOSYSLOG)
	{
		FLAGS_tosyslog= !FLAGS_tosyslog;
	}
#endif

#ifdef 	TOSERVER
	IS_OPT(TOSERVER)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption
							+ "- must have value. Format =10.0.0.1:5002");
		std::string _str(value.c_str());
		size_t _pos = _str.find_first_of(':');
		if (_pos == _str.npos)
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption
							+ "- must have value. Format =10.0.0.1:5002");
		unsigned _begin = _str[0] == '"' ? 1 : 0;
		FLAGS_server_ip= _str.substr(_begin, _pos);
		FLAGS_server_port= atoi(
				_str.substr(_pos + 1, _str.length()-_pos-1-_begin).c_str());
		FLAGS_toserver= !FLAGS_toserver;
	}
#endif

#ifdef 	FILE_NAME
	IS_OPT(FILE_NAME)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
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
#	else
		FLAGS_file_name= aOption;
#	endif
	}
#endif

#ifdef 	VMODULE_ARG
	IS_OPT(VMODULE_ARG)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		NSHARE::CText _str(value);
		_str.MReplaceAll("\"", ""); //erase all "
		//_str.fin
#	ifdef GLOG
		_str.MReplaceAll(";",",");
#	endif
		FLAGS_vmodule= _str.MToStdString();
	}
#endif

#ifdef 	OUTPUT_FUNC
	IS_OPT(OUTPUT_FUNC)
	{
		FLAGS_output_func_name=!FLAGS_output_func_name;
	}
#endif
#ifdef TO_SOCKET
	IS_OPT(TO_SOCKET)
	{
		if (value.empty())
			throw TCLAP::ArgParseException(
					std::string("Option: ") + aOption + "- must have value.");
		FLAGS_tosocket=true;
		FLAGS_socket_setting=value;
	}
#endif

#ifdef SHORT_FILE_NAME
	IS_OPT(SHORT_FILE_NAME)
	{
		FLAGS_short_name=!FLAGS_short_name;
	}
#endif
	if (!_is)
		throw TCLAP::ArgParseException(
				std::string("Unknown option: ") + aOption);
#endif//#ifndef NOLOG
}

#define ADD_DESCRIPTION( aVal ) _str+='\n';_str+=FOptsCommects[aVal].first;_str+=" - ";_str+=FOptsCommects[aVal].second;

std::string CShareLogArgsParser::sMGetDescription()
{
	std::string _str("Verbose output by "
			SHARE_TRACE_NAME
			". Option see below:");
#ifndef NOLOG
#ifdef 	LOGTOSTDERR
	ADD_DESCRIPTION(LOGTOSTDERR)
#endif

#ifdef 	ALSOLOGTOSTDERR
	ADD_DESCRIPTION(ALSOLOGTOSTDERR)
#endif

#ifdef 	COLORLOGTOSTDERR
	ADD_DESCRIPTION(COLORLOGTOSTDERR)
#endif

#ifdef 	STDERRTHRESHOLD
	ADD_DESCRIPTION(STDERRTHRESHOLD)
#endif

#ifdef 	LOG_PREFIX
	ADD_DESCRIPTION(LOG_PREFIX)
#endif

#ifdef 	LOGBUFLEVEL
	ADD_DESCRIPTION(LOGBUFLEVEL)
#endif

#ifdef 	LOGBUFSECS
	ADD_DESCRIPTION(LOGBUFSECS)
#endif

#ifdef 	MINLOGLEVEL
	ADD_DESCRIPTION(MINLOGLEVEL)
#endif

#ifdef 	LOG_DIR
	ADD_DESCRIPTION(LOG_DIR)
#endif

#ifdef 	LOG_LINK
	ADD_DESCRIPTION(LOG_LINK)
#endif

#ifdef 	V_LEVEL
	ADD_DESCRIPTION(V_LEVEL)
#endif

#ifdef 	MAX_LOG_SIZE
	ADD_DESCRIPTION(MAX_LOG_SIZE)
#endif

#ifdef 	TOSYSLOG
	ADD_DESCRIPTION(TOSYSLOG)
#endif

#ifdef 	TOSERVER
	ADD_DESCRIPTION(TOSERVER)
#endif

#ifdef 	FILE_NAME
	ADD_DESCRIPTION(FILE_NAME)
#endif

#ifdef 	VMODULE_ARG
	ADD_DESCRIPTION(VMODULE_ARG)
#endif

#ifdef 	OUTPUT_FUNC
	ADD_DESCRIPTION(OUTPUT_FUNC)
#endif
#ifdef TO_SOCKET
	ADD_DESCRIPTION(TO_SOCKET)
#endif

#ifdef SHORT_FILE_NAME
	ADD_DESCRIPTION(SHORT_FILE_NAME)
#endif

#if defined(GLOG) || defined(CPLUS_LOG)
	_str +=
			"\n Log lines have this form:\n"
					" Lmmdd hh:mm:ss.uuuuuu threadid file:line] func msg...\n"
					"where the fields are defined as follows:\n"
					"   L                A single character, representing the log level (eg 'I' for INFO)\n"
					"   mm               The month (zero padded; ie May is '05')\n"
					"   dd               The day (zero padded)\n"
					"   hh:mm:ss.uuuuuu  Time in hours, minutes and fractional seconds\n"
					"   threadid         The space-padded thread ID\n"
					"   file             The file name\n"
					"   line             The line number\n"
					"   func             The function name\n"
					"   msg              The user-supplied message";
#elif defined(COUT_LOG)
	_str +=
			"\n Log lines have this form:\n"
			" file:line] msg...\n"
			"where the fields are defined as follows:\n"
			"   file             The file name\n"
			"   line             The line number\n"
			"   msg              The user-supplied message";

#endif

#endif //#ifndef NOLOG
	return _str;
}
void CShareLogArgsParser::MParseValue(const std::string& aValue) const
{
#if defined(GLOG) || defined(CPLUS_LOG) ||  defined(COUT_LOG)
	std::string::const_iterator _it = aValue.begin(), _it_end(aValue.end());
	do
	{
		// skip leading white-space, commas
		for (; _it != _it_end && (*_it == ',' || *_it == ' ' || *_it == '\t');
				++_it)
			;

		if (_it == _it_end)
		{
			throw(TCLAP::ArgParseException("Missing a value for this argument!",
					toString()));
		}
		else
		{
			std::string _option;
			for (_option.push_back(*_it);
					++_it != _it_end && *_it != ',' && *_it != '=';)
				if (*_it != ' ' && *_it != '\t')
					_option.push_back(*_it);

			std::string _value;
			if (_it != _it_end && *_it == '=')
			{
				for (; ++_it != _it_end && *_it != ',';)
				{
					_value.push_back(*_it);
				}
			}
			MHandleValue(_option, _value);
		}
	} while (_it != _it_end);
#else
	throw TCLAP::ArgParseException(
			std::string("Option \"-v\" is not available, because \"NOLOG\" macro is set."));
#endif //#ifndef NOLOG
}
void CShareLogArgsParser::reset()
{
	Arg::reset();		//todo cannot reset
}
std::string CShareLogArgsParser::shortID(const std::string& val) const
{
	return Arg::shortID("option[,option]...");
}
std::string CShareLogArgsParser::longID(const std::string& val) const
{
	return Arg::longID("option[,option]...");
}
} /* namespace NSHARE */

char const default_logging_option_name[] = "verbose";
char const default_logging_short_option_name = 'v';

void parse_command_line_of_logging(int argc, const char* argv[])
{
	using namespace TCLAP;
	using namespace NSHARE;
	using namespace std;
	try
	{
		const char _log_flag[] =
		{ default_logging_short_option_name, '\0' };
		CmdLine cmd("fix", ' ', "0.9", false, true);
		cmd.setExceptionHandling(false);
		CShareLogArgsParser _logging(_log_flag, default_logging_option_name,
				false);
		cmd.add(_logging);
		cmd.parse(argc, argv);
		init_share_trace(cmd.getProgramName().c_str());
	} catch (ArgException& e) // catch any exceptions
	{
		cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
	}
}
