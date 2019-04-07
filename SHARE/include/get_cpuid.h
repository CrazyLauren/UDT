/*
 * get_cpuid.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.02.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef GET_CPUID_H_
#define GET_CPUID_H_

namespace NSHARE
{
//linux and x86
#if (defined __GNUC__ && (defined __i386__ || defined __x86_64__)) \
	|| defined _MSC_VER //msvc
#	define SHARE_HAVE_CPUID 1 //have CPUID
#else
#	define SHARE_HAVE_CPUID 0 //have not CPUID
#endif

/*!\brief Возвращает информацию о поддерживаемых
 * в процессоре возможностях
 *
 *\return 0 - EOK
 *			-1 - error
 */
extern SHARE_EXPORT int get_cpuid(int regs[], int h);

}

#endif /* GET_CPUID_H_ */
