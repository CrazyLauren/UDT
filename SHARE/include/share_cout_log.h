/*
 *
 * share_cout_log.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 03.03.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_COUT_LOG_H_
#define SHARE_COUT_LOG_H_


#		undef VLOG
#		undef LOG
#		undef LOG_IF
#		undef DVLOG
#		undef DLOG
#		undef DLOG_IF
#		define LOG(some)  (some)==0?std::cout:std::cerr
#		define VLOG(some)  LOG(some)
#		define LOG_IF(some,other) LOG(some)
#		define DVLOG(some) LOG(some)
#		define DLOG(some) LOG(some)
#		define DLOG_IF(some,other) LOG(some)
#		define LOG_ASSERT(some) assert(some)
#		define INFO 0
#		define WARNING 0
#		define ERROR 1
#		define FATAL 1

#endif /* SHARE_COUT_LOG_H_ */
