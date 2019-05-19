/*
 *
 * share_nolog.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_NOLOG_H_
#define SHARE_NOLOG_H_
#include <type_manip.h>
#undef INFO
#undef WARNING
#undef ERROR
#undef FATAL
#undef DFATAL
enum eNoLogLevel
{
	INFO,
	WARNING,
	ERROR,
	FATAL,
#ifdef NDEBUG
	DFATAL=ERROR
#else
	DFATAL=FATAL
#endif
};

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

#	define LOG(aLevel)  (!NSHARE::is_val_equal<aLevel,FATAL>::result)?(void)0: NSHARE::logging_impl::__gag_nc_t()&NSHARE::logging_impl::__logging_nc_t<aLevel>()<<__FILE__<<":"<<__LINE__<<" "
#	define VLOG(some)  true?(void)0:NSHARE::logging_impl::__gag_nc_t()^ std::cout
#	define LOG_IF(aLevel,condition) !(NSHARE::is_val_equal<aLevel,FATAL>::result && (condition))?(void)0: NSHARE::logging_impl::__gag_nc_t()& NSHARE::logging_impl::__logging_nc_t<aLevel>()<<__FILE__<<":"<<__LINE__<<" "

#	define VLOG_IF(some,other) VLOG(some)
#	define DVLOG(some) VLOG(some)
#	define DLOG(some) LOG(some)
#	define DLOG_IF(some,other) VLOG_IF(some,other)
#	define DVLOG_IF(some,other) VLOG_IF(some,other)
#	define LOG_ASSERT(some) (some)?(void)0:NSHARE::logging_impl::__gag_nc_t()& NSHARE::logging_impl::__logging_nc_t<FATAL>()<<__FILE__<<":"<<__LINE__<<" "
#ifndef NDEBUG
#	define DLOG_ASSERT(condition) LOG_ASSERT(condition)
#else
#	define DLOG_ASSERT(condition) true?(void)0: NSHARE::logging_impl::__gag_nc_t()^ std::cout
#endif
#	define CHECK_NE(some,some1) LOG_ASSERT((some)!=(some1))
#	define CHECK_LT(some,some1) LOG_ASSERT((some)<(some1))
#	define CHECK_LE(some,some1) LOG_ASSERT((some)<=(some1))
#	define CHECK_GT(some,some1) LOG_ASSERT((some)>(some1))
#	define CHECK_GE(some,some1) LOG_ASSERT((some)>=(some1))
#	define CHECK_EQ(some,some1) LOG_ASSERT((some)==(some1))
#	define CHECK_NOTNULL(aVal) LOG_ASSERT((aVal)!=NULL)
#	define DCHECK_POINTER_ALIGN(aVal) LOG_ASSERT(NSHARE::impl::check_pointer_align(aVal))
#	define CHECK(aVal) LOG_ASSERT(aVal)

#	define DCHECK_NE(some,some1) DLOG_ASSERT((some)!=(some1))
#	define DCHECK_LT(some,some1) DLOG_ASSERT((some)<(some1))
#	define DCHECK_LE(some,some1) DLOG_ASSERT((some)<=(some1))
#	define DCHECK_GT(some,some1) DLOG_ASSERT((some)>(some1))
#	define DCHECK_GE(some,some1) DLOG_ASSERT((some)>=(some1))
#	define DCHECK_EQ(some,some1) DLOG_ASSERT((some)==(some1))
#	define DCHECK_NOTNULL(aVal) DLOG_ASSERT((aVal)!=NULL)
#	define DCHECK(aVal) DLOG_ASSERT(aVal)

#	define VLOG_EVERY_N(verboselevel, n) VLOG(verboselevel)

namespace NSHARE
{
	namespace logging_impl 
	{
		template<eNoLogLevel aVal>
		struct __logging_nc_t
		{
			template<typename T>
			inline __logging_nc_t <aVal>& operator<<(const T& aStream)
			{
				return *this;
			}
		};
		template<>
		struct __logging_nc_t<FATAL>
		{
			__logging_nc_t()
			{
				std::cerr << "FATAL:";
			}
			~__logging_nc_t()
			{
				using namespace std;
				cerr << endl;
				log_terminate_handler();
			}
			template<typename T>
			inline __logging_nc_t<FATAL>& operator<<(const T& aStream)
			{
				std::cerr << aStream;
				return *this;
			}
		};
		struct __gag_nc_t
		{
			template<eNoLogLevel Level>
			inline void operator&(NSHARE::logging_impl::__logging_nc_t <Level> const&)
			{
			}
			inline void operator^(std::ostream&)
			{
			}
		};

	}
	namespace impl
	{
	template<typename T>
	inline bool check_pointer_align(T const * aVal)
	{
		return (uintptr_t)aVal % __alignof(T) == 0;
	}
	}
}//

#endif /* SHARE_NOLOG_H_ */
