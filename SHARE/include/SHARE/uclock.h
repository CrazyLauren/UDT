/*
 *
 * uclock.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.04.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
 
#ifndef UCLOCK_H_
#define UCLOCK_H_

namespace NSHARE
{
#ifdef _MSC_VER
struct timespec
{
  long long	tv_sec; 	///< seconds
  long  	tv_nsec;	///< nanoseconds
};
#endif
/**\brief realization clock_gettime for windows
 *
 */
extern SHARE_EXPORT int clock_gettime(int X, struct timeval *tv);
/**\brief convert from time in second to struct timeval
 */
extern SHARE_EXPORT void convert(struct timeval* aTo, float const aFrom);

/**\brief возвращет текущее время в секундах
 *\return current time in seconds
 */
extern SHARE_EXPORT double get_time();

/**\brief возвращет текущее время в timeval
 *\param ts сюда сохраняется результат
 */
#if defined (__MINGW32__)
extern SHARE_EXPORT void get_time(struct timeval *ts);
#elif defined(__QNX__)|| defined(unix)
extern SHARE_EXPORT void get_time(struct timespec *ts);
#endif

/**\brief кросплатформенная реализация usleep
 */
extern SHARE_EXPORT bool usleep(long unsigned aVal);
/**\brief кросплатформенная реализация sleep
 */
extern SHARE_EXPORT bool sleep(unsigned aVal);
/**\brief возвращет текущее время в ms
 *\return current time in ms
 */
extern SHARE_EXPORT uint64_t get_unix_time();
/**\brief изменяет время на aTo
 */
extern SHARE_EXPORT void add(struct timespec* aTo, double const& aFrom); //s
}
#endif /* UCLOCK_H_ */
