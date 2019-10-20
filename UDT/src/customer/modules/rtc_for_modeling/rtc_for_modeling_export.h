/*
 * rtc_for_modeling_export.h
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef RTC_FOR_MODELING_EXPORT_H_
#define RTC_FOR_MODELING_EXPORT_H_

#if  defined( _WIN32 ) && defined(_MSC_VER) && !defined(RTC_FOR_MODELING_STATIC)
#   ifdef RTC_FOR_MODELING_EXPORTS
#       define RTC_FOR_MODELING_EXPORT __declspec(dllexport)
#   else
#       define RTC_FOR_MODELING_EXPORT __declspec(dllimport)
#   endif
#else
#       define RTC_FOR_MODELING_EXPORT
#endif


#endif /* RTC_FOR_MODELING_EXPORT_H_ */
