/*
 * win_processor_detection.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 26.05.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef WIN_PROCESSOR_DETECTION_H
#define WIN_PROCESSOR_DETECTION_H

/** This file uses preprocessor #defines to set  
* procesor family in gcc style.
*/
#if defined(_M_ARM)
#	define __aarch64__
#endif

#if defined(_M_IA64)
#	define __ia64__
#endif

#if defined(_M_IX86)
#	define __i386__
#endif

#if defined(_M_X64) || defined(_M_AMD64)
#	define __x86_64__
#endif

#endif // WIN_PROCESSOR_DETECTION_H
