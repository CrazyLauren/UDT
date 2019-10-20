/*
 *
 * share_trace.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 23.01.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_TRACE_H_
#define SHARE_TRACE_H_

// \note There are 5 type of logging:
//	- NOLOG - no logging, but checking macros is exist;
//	- REMOVE_LOG - no logging, no checking macros - all trace info will be removed;
//	- GLOG - logging by google-glog library;
//	- CPLUS_LOG - logging by log4cplus library;
//	- COUT_LOG - logging to cout, cerr only.
//
// If neither of these macro are set, the macro NOLOG is setting.
//
// For corectly logging initiliaze, Function
// init_trace should be call at the first set-out of
// the "int main(nt argc, char *argv[])" function.
//a
// Make a bunch of macros for logging.  The way to log things is to stream
// things to LOG(<a particular severity level>).  E.g.,
//
//   LOG(INFO) << "Found " << num_cookies << " cookies";
//
// The supported severity levels for macros that allow you to specify one
// are (in increasing order of severity) INFO, WARNING, ERROR, and FATAL.
// Note that messages of a given severity are logged not only in the
// logfile for that severity, but also in all logfiles of lower severity.
// E.g., a message of severity FATAL will be logged to the logfiles of
// severity FATAL, ERROR, WARNING, and INFO.
//
// There is also the special severity of DFATAL, which logs FATAL in
// debug mode, ERROR in normal mode.
//
// Very important: logging a message at the FATAL severity level causes
// the program to terminate (after the message is logged).

// There are "verbose level" logging macros.  They look like
//
//   VLOG(1) << "I'm printed when you run the program with "verbose level" ==1
// 		(can be set by FLAGS_v or by program argumets, see below ) or higher";
//   VLOG(2) << "I'm printed when you run the program with "verbose level" ==2 or higher";
//
// These always log at the INFO log level (when they log at all). The verbose
// logging can also be turned on for each module separately, see program argumets.
// There's also VLOG_IS_ON(n) "verbose level" condition macro. To be used as
//
//   if (VLOG_IS_ON(2)) {
//     // do some logging preparation and logging
//     // that can't be accomplished with just VLOG(2) << ...;
//   }
// There are also "debug mode" logging macros like the ones above:
//
//   DLOG(INFO) << "Found cookies";
//
//   DLOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
//
//   DLOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";
//
// All "debug mode" logging is compiled away to nothing for non-debug mode
// compiles.
//
// We also have
//
//   LOG_ASSERT(assertion);
//   DLOG_ASSERT(assertion);
//
// which is syntactic sugar for {,D}LOG_IF(FATAL, assert fails) << assertion;
//
//
// You can capture log messages in a string, rather than reporting them
// immediately:
//
//   vector<string> errors;
//   LOG_STRING(ERROR, &errors) << "Couldn't parse cookie #" << cookie_num;
//
// This pushes back the new error onto 'errors'; if given a NULL pointer,
// it reports the error via LOG(ERROR).
//
// You can also do conditional logging:
//
//   LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
//
// You can also do occasional logging (log every n'th occurrence of an
// event):
//
//   LOG_EVERY_N(INFO, 10) << "Got the " << google::COUNTER << "th cookie";
//
// The above will cause log messages to be output on the 1st, 11th, 21st, ...
// times it is executed.  Note that the special google::COUNTER value is used
// to identify which repetition is happening.
//
// You can also do occasional conditional logging (log every n'th
// occurrence of an event, when condition is satisfied):
//
//   LOG_IF_EVERY_N(INFO, (size > 1024), 10) << "Got the " << google::COUNTER
//                                           << "th big cookie";
// 	 LOG_IF_EVERY_TIME(INFO,(size > 1024),2.0) << "when condition is satisfied
// and previous occurrence of an event was more than 2.0 seconds ago."
//
// You can log messages the first N times your code executes a line. E.g.
//
//   LOG_FIRST_N(INFO, 20) << "Got the " << google::COUNTER << "th cookie";
//
// Outputs log messages for the first 20 times it is executed.
//
// There are also VLOG_IF, VLOG_EVERY_N and VLOG_IF_EVERY_N "verbose level"
// condition macros for sample cases, when some extra computation and
// preparation for logs is not needed.
//   VLOG_IF(1, (size > 1024))
//      << "I'm printed when size is more than 1024 and when you run the "
//         "program with --v=1 or more";
//   VLOG_EVERY_N(1, 10)
//      << "I'm printed every 10th occurrence, and when you run the program "
//         "with --v=1 or more. Present occurence is " << google::COUNTER;
//   VLOG_IF_EVERY_N(1, (size > 1024), 10)
//      << "I'm printed on every 10th occurence of case when size is more "
//         " than 1024, when you run the program with --v=1 or more. ";
//         "Present occurence is " << google::COUNTER;
//
// Equality/Inequality checks - compare two values, and log a FATAL message
// including the two values when the result is not as expected.  The values
// must have operator<<(ostream, ...) defined.

// PLOG() and PLOG_IF() behave exactly like their LOG*
// with the addition that they postpend a description
// of the current state of errno to their output lines.
//
// CHECK dies with a fatal error if condition is not true.  It is *not*
// controlled by NDEBUG, so the check will be executed regardless of
// compilation mode.  Therefore, it is safe to do things like:
// CHECK(fp->Write(x) == 4)
// We are very careful to ensure that each argument is evaluated exactly
// once, and that anything which is legal to pass as a function argument is
// legal here.  In particular, the arguments may be temporary expressions
// which will end up being destroyed at the end of the apparent statement,
// for example:
//   	CHECK_EQ(string("abc")[1], 'b');
// 		CHECK_EQ(val1, val2)
// 		CHECK_NE(val1, val2)
// 		CHECK_LE(val1, val2)
// 		CHECK_LT(val1, val2)
// 		CHECK_GE(val1, val2)
// 		CHECK_GT(val1, val2)
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.
//		CHECK_NOTNULL(val)
// String (char*) equality/inequality checks.
// CASE versions are case-insensitive.
// Note that "s1" and "s2" may be temporary strings which are destroyed
// by the compiler at the end of the current "full expression"
// (e.g. CHECK_STREQ(Foo().c_str(), Bar().c_str())).
//		CHECK_STREQ(str1, str2)
//		CHECK_STRNE(str1, str2)
// WARNING: These don't compile correctly if one of the arguments is a pointer
// and the other is NULL. To work around this, simply static_cast NULL to the
// type of the desired pointer.
// A CHECK_ERR() macro that lets you assert the success of a function that
// returns -1 and sets errno in case of an error. E.g.
// CHECK_ERR(mkdir(path, 0700));
//
//
// Options of command line
// 	[-voption[,option]...],where option see below
// options:
//		func		- Set whether output the function name;
//		logtostderr - Set whether log messages go to stderr instead of logfiles;
//		alsostderr	- Set whether log messages go to stderr in addition to logfiles;
//		colorstderr	- Set color messages logged to stderr (if supported);
//		stderrthreshold=val - Log messages at a level >= "val" are automatically
//								sent to stderr in addition to log files;
//		logprefix	- Set whether the log prefix should be prepended to each
//					 	line of output;
//		logbuflevel=val	- Log messages at a level <= "val" are buffered.
//					  Log messages at a higher level are flushed immediately.;
//		logbufsecs=sec	- Sets the maximum number of seconds (for CPLUS_LOG buffer size in kb)
//						which logs may be buffered for;
//		level=val		- Log suppression level: messages logged at a lower level than "val"
// 						are suppressed. Val is to be equal of "info" or "warning" or "error" or "fatal";
//		logdir="dir"	- If specified, logfiles are written into "dir" directory instead of the
// 						default logging directory;
//		loglink="path" - Sets the "path" of the directory into which to put additional links
// 						to the log files (if available);
//		vlevel=val	 - Default maximal active verbose level (for all modules).FLAFS_v="val";
//		max_log_size=size - Sets the maximum log file size (in MB);
//		tosyslog	 - Set whether log messages go to syslog in addition (if available);
//		toserver="ip:port" -  Set whether log messages go to udp server
//					   in addition (if available). Ip - destination ip address,
//						port - destination port;
//		file="name"  - specifics file name;
//		module="str" - Gives the per-module maximal  verbose logging levels to override
// 						the value given by "level" option. E.g. "my_module=2,foo*=3"
//						would change the logging level for all code in source files
//						"my_module.*" and "foo*.*".
// 						The wildcarding functionality shown by (c)
//						supports both '*' (match 0 or more characters) and
//						'?' (match any single character) wildcards.
//						 For instance, module="mapreduce=2;file=1;gfs*=3",v=0
// 						 will cause:
//   						a. VLOG(2) and lower messages to be printed
//															from mapreduce.{h,cc}
//   						b. VLOG(1) and lower messages to be
//															printed from file.{h,cc}
//   						c. VLOG(3) and lower messages to be
//												printed from files prefixed with "gfs"
//   						d. VLOG(0) and lower messages to be printed from elsewhere
//
//		short_name		- Removes path from file name in the log
// Log lines have this form:
//
//     Lmmdd hh:mm:ss.uuuuuu threadid file:line] msg...
//
// where the fields are defined as follows:
//
//   L                A single character, representing the log level
//                    (eg 'I' for INFO)
//   mm               The month (zero padded; ie May is '05')
//   dd               The day (zero padded)
//   hh:mm:ss.uuuuuu  Time in hours, minutes and fractional seconds
//   threadid         The space-padded thread ID as returned by GetTID()
//                    (this matches the PID on Linux)
//   file             The file name
//   line             The line number
//   msg              The user-supplied message
//
// Example:
//
//   I1103 11:57:31.739339 24395 google.cc:2341] Command line: ./some_prog
//   I1103 11:57:31.739403 24395 google.cc:2342] Process id 24395
//
// Unless otherwise specified, logs will be written to the filename
// "<program name>.<hostname>.<user name>.log.<severity level>.", followed
// by the date, time, and pid (you can't prevent the date, time, and pid
// from being in the filename).
//
// NOTE: although the microseconds are useful for comparing events on
// a single machine, clocks on different machines may not be well
// synchronized.  Hence, use caution when comparing the low bits of
// timestamps from different machines.


/**\brief инициализация логирования
 *
 * При инциализации логирования считывается имя ПО и настройки логирования
 * заддаными в опции default_logging_option_name или default_logging_short_option_name
 *
 *\param argv -argv ф-ии main
 *\param argc argc ф-ии main
 */
extern SHARE_EXPORT void init_trace(int argc, char const *argv[]);
inline void init_trace(int argc, char *argv[])
{
	init_trace (argc, (char const**)argv);
}
/**\brief инициализация логирования
 *
 *	Инициализация логирования без использования коммандной строки.
 *	Для "разбора" коммандной строки в части логирования
 *	см. CShareLogArgsParser
 *
 *\param aFileName имя программы
 */
extern SHARE_EXPORT void init_share_trace(char const *aFileName);

/**\brief полное название опции используемой при настройки логирования
 * через коммандную строку
 *
 * По умолчанию, опция называется "verbose". Название опции
 *  можно изменить, но до вызова метода init_trace
 */
extern SHARE_EXPORT char const default_logging_option_name[];

/**\brief короткое название опции используемой при настройки логирования
 * через коммандную строку
 *
 * По умолчанию, опция называется "v". Название опции
 * можно изменить, но до вызова метода init_trace
 */
extern SHARE_EXPORT char const default_logging_short_option_name;


/** Parse argument of command line for logging
 *
 * @param argc
 * @param argv
 */
extern SHARE_EXPORT void parse_command_line_of_logging(int argc, const char* argv[]);

/**\name implementation function
 *\internal
 *\{
 */
extern SHARE_EXPORT void init_trace_cplus(char const*aProgrammName);
extern SHARE_EXPORT void init_trace_glog(char const*aProgrammName);
extern SHARE_EXPORT std::terminate_handler get_log_terminate_handler();
extern SHARE_EXPORT void log_terminate_handler();
extern "C" SHARE_EXPORT void install_failure_signal_handler();
//\}

#include <share/logging/share_trace_config.h>

#endif /*SHARE_TRACE_H_*/
