/*
 *
 * uclock.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 02.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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
  long long	tv_sec; 	// seconds
  long  	tv_nsec;	// nanoseconds 
};
#endif
extern SHARE_EXPORT int clock_gettime(int X, struct timeval *tv);
extern SHARE_EXPORT void convert(struct timeval* aTo, float const aFrom);
extern SHARE_EXPORT double get_time();
//sec;
#if defined (__MINGW32__)
extern SHARE_EXPORT void get_time(struct timeval *ts);
#elif defined(__QNX__)|| defined(unix)
extern SHARE_EXPORT void get_time(struct timespec *ts);
#endif
extern SHARE_EXPORT void usleep(long unsigned aVal);
extern SHARE_EXPORT unsigned sleep(unsigned aVal);
extern SHARE_EXPORT uint64_t get_unix_time(); //ms
extern SHARE_EXPORT void add(struct timespec* aTo, double const& aFrom); //s
}
#endif /* UCLOCK_H_ */
