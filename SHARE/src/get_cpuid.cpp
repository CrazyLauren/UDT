// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * get_cpuid.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.02.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <macro_attributes.h>
#include <get_cpuid.h>

#if defined __GNUC__ && (defined __i386__ || defined __x86_64__)
namespace NSHARE
{
int get_cpuid(int regs[], int h)
{
    __asm__ __volatile__(
#	if defined __x86_64__
                         "pushq %%rbx;\n"
#	else
                         "pushl %%ebx;\n"
#	endif
                         "cpuid;\n"
#	if defined __x86_64__
                         "popq %%rbx;\n"
#	else
                         "popl %%ebx;\n"
#	endif
                         : "=a"(regs[0]), [ebx] "=r"(regs[1]), "=c"(regs[2]), "=d"(regs[3])
                         : "a"(h));
    return 0;
}
}
#elif defined _MSC_VER
#include <intrin.h>
namespace NSHARE
{
int get_cpuid(int regs[], int h)
{
	__cpuid(regs,h);
	return 0;
}
}
#else
namespace NSHARE
{
int get_cpuid(int regs[], int h)
{
	return -1;
}
}
#endif

