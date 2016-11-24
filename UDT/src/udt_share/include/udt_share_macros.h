/*
 * udt_share_macros.h
 *
 *  Created on: 23.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_SHARE_MACROS_H_
#define UDT_SHARE_MACROS_H_

#if  defined( _WIN32 ) && defined(_MSC_VER) && !defined(UDT_SHARE_STATIC)
#   ifdef UDT_SHARE_EXPORTS
#       define UDT_SHARE_EXPORT __declspec(dllexport)
#   else
#       define UDT_SHARE_EXPORT __declspec(dllimport)
#   endif
#else
#       define UDT_SHARE_EXPORT
#endif


#endif /* UDT_SHARE_MACROS_H_ */
