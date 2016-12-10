/*
 *
 * profiler_vtune.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.12.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef PROFILER_VTUNE_H
#define PROFILER_VTUNE_H

#include <ittnotify.h>

#	define ProfileScopeEvent(x) 
#	define THREAD_NAME(n,i)     
#	define PROFILE_INIT()       
#	define PROFILE_PAUSE()      __itt_pause()
#	define PROFILE_RESUME()     __itt_resume()

#endif //PROFILER_VTUNE_H
