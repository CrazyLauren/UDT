/*
 *
 * share_cout_log.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_COUT_LOG_H_
#define SHARE_COUT_LOG_H_

#include <logging/trace_macros.h>

#	undef INFO
#	undef WARNING
#	undef ERROR
#	undef FATAL

enum eLavel
{
	INFO = 0, WARNING, ERROR, FATAL
};
namespace NSHARE
{
namespace logging_impl{
SHARE_EXPORT void lock_cout_mutex();
SHARE_EXPORT void unlock_cout_mutex();
SHARE_EXPORT void lock_cerr_mutex();
SHARE_EXPORT void unlock_cerr_mutex();
template<eLavel Level>
struct __logging_t
{
	const bool FLogging;
	__logging_t() :
	FLogging(Level >= FLAGS_minloglevel)
	{
		if (FLogging)
		{
			lock_cout_mutex();
			std::cout << (Level == INFO ?"INFO:":"WARN:" );
		}
	}
	~__logging_t()
	{
		if (FLogging)
		{
			std::cout << std::endl;
			unlock_cout_mutex();
		}
	}

	template<typename T>
	inline __logging_t <Level>& operator<<(const T& aStream)
	{
		if (FLogging)
			std::cout<<aStream;

		return *this;
	}
};
template<>
struct __logging_t<ERROR>
{
	const bool FLogging;
	__logging_t() :
			FLogging(ERROR >= FLAGS_minloglevel)
	{
		if (FLogging)
		{
			lock_cerr_mutex();
			std::cerr << "ERROR:";
		}
	}
	~__logging_t()
	{
		if (FLogging)
		{
			std::cerr << std::endl;
			unlock_cerr_mutex();
		}
	}

	template<typename T>
	inline __logging_t <ERROR>& operator<<(const T& aStream)
	{
		if (FLogging)
			std::cerr<<aStream;

		return *this;
	}
};
template<>
struct __logging_t<FATAL>
{
	const bool FLogging;
	__logging_t() :
			FLogging(FATAL >= FLAGS_minloglevel)
	{
		if (FLogging)
		{
			std::cerr << "FATAL:";
		}
	}
	~__logging_t()
	{
		if (FLogging)
		{
			using namespace std;
			cerr<<endl;
			log_terminate_handler();
		}
	}
	template<typename T>
	inline __logging_t <FATAL>& operator<<(const T& aStream)
	{
		if (FLogging)
				std::cerr<<aStream;

		return *this;
	}
};

struct __cout_gag_t
{
	template<class T>
	inline void operator^(T const&)
	{
	}
};
}
}
#define LOG(some) NSHARE::logging_impl::__logging_t<some>()<<__FILE__<<":"<<__LINE__<<" "

#define LOG_IF(severity, condition) \
		!(condition) ? (void) 0 : NSHARE::logging_impl::__cout_gag_t()^LOG(severity)
#define PLOG_IF(severity, condition) \
		 !(condition) ? (void) 0 : NSHARE::logging_impl::__cout_gag_t()^PLOG(severity)


#endif /* SHARE_COUT_LOG_H_ */
