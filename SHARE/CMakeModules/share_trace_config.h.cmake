/*
 *
 * share_trace_config.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 23.01.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_TRACE_CONFIG_H_
#define SHARE_TRACE_CONFIG_H_

#define SHARE_USE_GLOG @USE_GLOG@
#define SHARE_USE_CPLUS @USE_CPLUS@

#if defined(CPLUS_LOG) && !SHARE_USE_CPLUS
#	error "@TARGET_NAME@ library isn't used log4cplus. Remake @TARGET_NAME@ with ${TARGET_NAME}_LOGGING_TO_LOG4CPLUS option."
#endif

#if defined(GLOG) && !SHARE_USE_GLOG
#	error "@TARGET_NAME@ library isn't used Glog. Remake @TARGET_NAME@ with ${TARGET_NAME}_LOGGING_TO_GLOG option."
#endif

/**\name selection logging system
 *\{
 */
#ifndef REMOVE_LOG
#	ifndef NOLOG
#		ifdef GLOG
#			include <glog/logging.h>
#		elif defined(COUT_LOG)//#ifdef GLOG
# 			include <SHARE/logging/share_cout_log.h>
#		elif defined(CPLUS_LOG)
#			define OUTPUT_FUNCTION_NAME
# 			include <SHARE/logging/share_trace_log4cplus.h>
#		else
#			define NOLOG
#		endif //#GLOG
#	endif //#ifndef NOLOG
#	if defined(NOLOG) //#ifndef NOLOG
# 		include <SHARE/logging/share_nolog.h>
#	endif//#ifndef NOLOG
//\}

/**\def LOG_IF_EVERY_TIME
 *\brief convenient macro for logging if condition is true and the last logging time
 * is more than "period".
 *
 */
#if defined(HAVE_GET_TIME) && !defined(NOLOG)

#	define LOG_IF_EVERY_TIME(severity, condition,period)\
		COMPILE_ASSERT( severity <= FATAL ,        INVALID_LOG_LEVEL);\
		static double LOG_OCCURRENCES = NSAHRE::get_time(); \
		  if (((LOG_OCCURRENCES+period)<NSAHRE::get_time())&& (condition) && ((LOG_OCCURRENCES= get_time())))  LOG(severity)
#else
#	define	LOG_IF_EVERY_TIME(severity, condition,period) LOG(severity)<<"***ERROR***: LOG_IF_EVERY_TIME isn't available."
#endif

/**\name define using logging system name
 *\{
 */
#	ifdef GLOG
#		define SHARE_TRACE_NAME "glog"
#	elif defined(COUT_LOG)//#ifdef GLOG
#		define SHARE_TRACE_NAME "cout"
#	elif defined(CPLUS_LOG)
#		define SHARE_TRACE_NAME "log4cplus"
#	else
#		define SHARE_TRACE_NAME "nolog"
#	endif //#GLOG
#else //#ifndef REMOVE_LOG
#	include <SHARE/logging/share_dellog.h>
#	define SHARE_TRACE_NAME "removed"
#endif //#ifndef REMOVE_LOG
//\}

#endif /*SHARE_TRACE_CONFIG_H_*/
