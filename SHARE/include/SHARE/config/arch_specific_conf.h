/*
 *
 * arch_specific_conf.h
 *
 * Copyright © 2020  https://github.com/CrazyLauren
 *
 *  Created on: 29.02.2020
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ARCH_SPECIFIC_CONF_H
#define ARCH_SPECIFIC_CONF_H

#include <SHARE/config/config.h>

#ifdef ENABLE_VTUNE
#	include <profiler_vtune.h>
#else
#	define ProfileScopeEvent(x)
#	define THREAD_NAME(n,i)
#	define PROFILE_INIT()
#	define PROFILE_PAUSE()
#	define PROFILE_RESUME()
#endif

#ifndef MSVC
#	include <SHARE/win_processor_detection.h>
#endif

/**\brief define standard headers
 *\{
 */
#include <set>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <deque>
#include <stdint.h>
#include <cassert>
#include <exception>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdexcept>
#include <sstream>
//#include <limits.h>
#include <limits>
#include <functional>
#include <ctype.h> //isdigit ...
#include <algorithm>
//\}

#if (__cplusplus>= 201103L) || (_MSC_VER >= 1800) //fucking msvc
#	include <unordered_set>
#	include <unordered_map>
#	define HASH_SET std::unordered_set
#	define HASH_MAP std::unordered_map
#	define HASH_MULTISET std::unordered_multiset
#	define HASH_MULTIMAP std::unordered_multimap
#	define HASH std::hash
#else
#	include<boost/unordered_set.hpp>
#	include<boost/unordered_map.hpp>
#	define HASH_SET boost::unordered_set
#	define HASH_MAP boost::unordered_map
#	define HASH_MULTISET boost::unordered_multiset
#	define HASH_MULTIMAP boost::unordered_multimap
#endif

/**\brief define shared ptr
 *\{
 */
#include SHARED_PTR_HEADER
#define SHARED_PTR SHARED_PTR_NAMESPACE::shared_ptr
//\}

/**\brief define system headers
 *\{
 */
#ifdef _WIN32
#	ifdef _MSC_VER
#		ifndef NOMINMAX
#			define NOMINMAX
#		endif
#		include <SHARE/win_unistd.h>
#		pragma warning(disable : 4290)
#		pragma warning(disable : 4996)
#		pragma warning(disable : 4355)
#		pragma warning(disable : 4251)
#	else
#		include <unistd.h> //mingw,cygwin
#	endif

#	include <winsock2.h>
#	include <windows.h>

#	ifdef _MSC_VER
#		include <memory>
#		undef uuid_t
#	endif
#else
#	include <unistd.h>
#	ifdef  __QNX__
#		include <sys/neutrino.h>
#	endif
#endif
//\}

#if   defined(__QNX__)||defined(unix) ||  defined(__MINGW32__)
#	include <pthread.h>                        // POSIX threads support
#endif


#ifdef  __QNX__
#	include <sys/time.h>
#else
#	include <ctime>
#endif
#endif //ARCH_SPECIFIC_CONF_H
