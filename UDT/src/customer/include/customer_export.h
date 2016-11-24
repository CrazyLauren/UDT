/*
 * customer_export.h
 *
 *  Created on: 02.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CUSTOMER_EXPORT_H_
#define CUSTOMER_EXPORT_H_

#if  defined( _WIN32 ) && defined(_MSC_VER) && !defined(CUSTOMER_STATIC)
#   ifdef CUSTOMER_EXPORTS
#       define CUSTOMER_EXPORT __declspec(dllexport)
#   else
#       define CUSTOMER_EXPORT __declspec(dllimport)
#   endif
#else
#       define CUSTOMER_EXPORT
#endif


#endif /* CUSTOMER_EXPORT_H_ */
