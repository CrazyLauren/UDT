/*
 * trace_macros.h
 *
 *  Created on: 12.11.2016
 *      Author: Sergey
 */

#ifndef TRACE_MACROS_H_
#define TRACE_MACROS_H_

#	undef VLOG
#	undef LOG
#	undef LOG_IF
#	undef VLOG_IF
#	undef DVLOG
#	undef DLOG
#	undef DLOG_IF
#	undef LOG_ASSERT
#	undef CHECK_NE
#	undef CHECK_LE
#	undef CHECK_GT
#	undef CHECK_GE
#	undef CHECK_LT
#	undef CHECK_EQ
#	undef CHECK_NOTNULL
#	undef CHECK
#	undef DCHECK

#ifdef NDEBUG
#	define DFATAL ERROR
#else
#	define	DFATAL FATAL
#endif

#ifndef COMPILE_ASSERT
#	define COMPILE_ASSERT(aVal , aVal2)
#endif

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

#define CHECK_ERR(invocation)\
	PLOG_IF(FATAL, (invocation) == -1)<< #invocation

#ifndef NDEBUG

#	define DLOG(severity) LOG(severity)
#	define DVLOG(verboselevel) VLOG(verboselevel)
#	define DLOG_IF(severity, condition) LOG_IF(severity, condition)
#	define DLOG_EVERY_N(severity, n) LOG_EVERY_N(severity, n)
#	define DLOG_IF_EVERY_N(severity, condition, n) \
  LOG_IF_EVERY_N(severity, condition, n)
#	define DLOG_IF_EVERY_TIME(severity, condition,period)\
		 LOG_IF_EVERY_N(severity, condition, period)
#	define DLOG_ASSERT(condition) LOG_ASSERT(condition)

// debug-only checking.  not executed in NDEBUG mode.
#	define DCHECK(condition) CHECK(condition)
#	define DCHECK_EQ(val1, val2) CHECK_EQ(val1, val2)
#	define DCHECK_NE(val1, val2) CHECK_NE(val1, val2)
#	define DCHECK_LE(val1, val2) CHECK_LE(val1, val2)
#	define DCHECK_LT(val1, val2) CHECK_LT(val1, val2)
#	define DCHECK_GE(val1, val2) CHECK_GE(val1, val2)
#	define DCHECK_GT(val1, val2) CHECK_GT(val1, val2)
#	define DCHECK_NOTNULL(val) CHECK_NOTNULL(val)
#	define DCHECK_STREQ(str1, str2) CHECK_STREQ(str1, str2)
#	define DCHECK_STRCASEEQ(str1, str2) CHECK_STRCASEEQ(str1, str2)

#else  // NDEBUG
#	define DLOG(severity) \
		LOG_IF(severity,false)

#	define DVLOG(verboselevel) \
		DLOG(0)

#	define DLOG_IF(severity, condition) \
		DLOG(0)

#	define DLOG_EVERY_N(severity, n) \
		DLOG(0)

#	define DLOG_IF_EVERY_N(severity, condition, n) \
		DLOG(0)

#	define DLOG_IF_EVERY_TIME(severity, condition, n) \
		DLOG(0)

#	define DLOG_ASSERT(condition) \
		DLOG(0)

// MSVC warning C4127: conditional expression is constant
#	define DCHECK(condition) \
		DLOG(0)

#	define DCHECK_EQ(val1, val2) \
		DLOG(0)

#	define DCHECK_NE(val1, val2) \
		DLOG(0)

#	define DCHECK_LE(val1, val2) \
		DLOG(0)

#	define DCHECK_LT(val1, val2) \
		DLOG(0)

#	define DCHECK_GE(val1, val2) \
		DLOG(0)

#	define DCHECK_GT(val1, val2) \
		DLOG(0)

#	define DCHECK_NOTNULL(val) (val)

#	define DCHECK_STREQ(str1, str2) \
		DLOG(0)

#	define DCHECK_STRCASEEQ(str1, str2) \
		DLOG(0)

#	define DCHECK_STRNE(str1, str2) \
		DLOG(0)

#	define DCHECK_STRCASENE(str1, str2) \
		DLOG(0)
#endif  // NDEBUG

#endif /* TRACE_MACROS_H_ */
