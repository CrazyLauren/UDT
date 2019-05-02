// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * random_value.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.02.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <stdio.h>
#include <limits>
#include <stdexcept>
#include <random_value.h>
#include <get_cpuid.h>

namespace NSHARE
{
namespace
{
static bool has_rdrand_impl()
{
	// CPUID.01H:ECX.RDRAND[bit 30] == 1
	//see __cpuid of msdn
	int regs[4];
	if (get_cpuid(regs, 1) < 0)
		return false;

	return ((regs[2] & (1 << 30)) != 0);
}
}
static bool has_rdrand(void)
{
	static bool const _is=has_rdrand_impl();
	return _is;
}

/* get_rdrand_seed - GCC x86 and X64 */

#if defined __GNUC__ && (defined __i386__ || defined __x86_64__)

#	define HAVE_RDRAND 1

static uint32_t get_rdrand_seed(void)
{
    DVLOG(3)<<"get_rdrand_seed";
    uint32_t _eax=std::numeric_limits<uint32_t>::max();
    // rdrand eax
    __asm__ __volatile__("1: .byte 0x0F\n"
                         "   .byte 0xC7\n"
                         "   .byte 0xF0\n"
                         "   jnc 1b;\n"
                         : "=a" (_eax));
    return _eax;
}

#endif

#ifdef _MSC_VER

#	if _MSC_VER >= 1700
#		define HAVE_RDRAND 1

// get_rdrand_seed - Visual Studio 2012 and above
static uint32_t get_rdrand_seed(void)
{
    DVLOG(3)<<"get_rdrand_seed";
    uint32_t r=std::numeric_limits<uint32_t>::max();
    while (_rdrand32_step(&r) == 0);
    return r;
}

#	elif defined _M_IX86
#		define HAVE_RDRAND 1

// get_rdrand_seed - Visual Studio 2010 and below - x86 only
static uint32_t get_rdrand_seed(void)
{
	DVLOG(3)<<"get_rdrand_seed";
	uint32_t _eax=std::numeric_limits<uint32_t>::max();
retry:
	// rdrand eax
	__asm _emit 0x0F __asm _emit 0xC7 __asm _emit 0xF0
	__asm jnc retry
	__asm mov _eax, eax
	return _eax;
}

#	endif//#if _MSC_VER >= 1700
#endif //#ifdef _MSC_VER
}


#if defined (__APPLE__) || defined(__unix__) || defined(__linux__) || defined(__FreeBSD__)|| defined(__QNX__)

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#define HAVE_DEV_RANDOM 1
namespace NSHARE
{
static const char *dev_random_file = "/dev/urandom";

static bool has_dev_urandom(void)
{
    struct stat buf;
    if (stat(dev_random_file, &buf)) {
        return false;
    }
    return ((buf.st_mode & S_IFCHR) != 0);
}


static uint32_t get_dev_random_seed(void)
{
	DVLOG(3)<<"get_dev_random_seed";
	uint32_t r=std::numeric_limits<uint32_t>::max();
	ssize_t nread=0;
    int fd = open(dev_random_file, O_RDONLY);
    if (fd < 0)
    	goto error;


    nread = read(fd, &r, sizeof(r));
    if (nread != sizeof(r))
    	goto error;

    close(fd);
    return r;

 error:
	LOG(DFATAL)<<"cannot generate random value by  "<<dev_random_file<<" as "<<strerror(errno);
	if (fd >= 0)
		 close(fd);
	 return std::numeric_limits<uint32_t>::max();
}
}
#endif


#ifndef HAVE_RDRAND //не использовать  CRYPTGENRANDOM если есть RDRAND
#ifdef HAVE_LIBADVAPI32
#	define HAVE_CRYPTGENRANDOM 1

#	include <windows.h>
#	include <wincrypt.h>
namespace NSHARE
{
static uint32_t get_cryptgenrandom_seed(void)
{
    HCRYPTPROV hProvider = 0;
    uint32_t r=std::numeric_limits<uint32_t>::max();

    DVLOG(3)<<"get_cryptgenrandom_seed";

    if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
    	goto error;

    if (!CryptGenRandom(hProvider, sizeof(r), (BYTE*)&r))
    	goto error;

    return r;

error:
	if(hProvider!=0)
		CryptReleaseContext(hProvider, 0);

	LOG(DFATAL)<<"CryptGenRandom fail";

	return std::numeric_limits<uint32_t>::max();
}
}
#endif
#endif// HAVE_RDRAND

namespace NSHARE
{
bool is_RNG_available()
{
	bool _is=false;
#if defined HAVE_RDRAND
	_is=has_rdrand();
    if(_is) return true;
#endif
#if defined HAVE_DEV_RANDOM
    _is=has_dev_urandom();
    if(_is) return true;
#endif
#if defined HAVE_CRYPTGENRANDOM
    return true;
#endif
    return _is;
}
unsigned int get_random_value_by_RNG(void)
{
#if defined HAVE_RDRAND
    if (has_rdrand()) return get_rdrand_seed();
#endif
#if defined HAVE_DEV_RANDOM
    if (has_dev_urandom()) return get_dev_random_seed();
#endif
#if defined HAVE_CRYPTGENRANDOM
    return get_cryptgenrandom_seed();
#endif
    throw std::invalid_argument("cannot generate random value");
    return 0;
}

void generate_seed_if_need(bool aForce)
{
	static bool _is=false;
	if(!_is || aForce)
	{
		_is=true;

		uint32_t _rand=NSHARE::get_unix_time()%std::numeric_limits<uint32_t>::max();
		if(is_RNG_available())
			_rand=get_random_value_by_RNG();

		DVLOG(3)<<"Generated new seed ="<<_rand;
		srand(_rand);
	}
}

};
