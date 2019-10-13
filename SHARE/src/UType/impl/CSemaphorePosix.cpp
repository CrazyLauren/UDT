// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSemaphorePosix.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#if defined (HAVE_POSIX_SEMAPHORES)
#include <deftype>
#include <assert.h>
#include <semaphore.h>

namespace NSHARE
{
unsigned const ISemaphore::MAX_VALUE=SEM_VALUE_MAX;
struct CSemaphore::CImpl
{
	mutable sem_t FSemaphore;
	bool FIsInited;
};
CSemaphore::CSemaphore(unsigned int value):FImpl(new CImpl)
{
	MInit(value);
	DCHECK(MIsInited());
}
bool CSemaphore::MInit(unsigned int value)
{
	if (MIsInited())
	{
		LOG(DFATAL)<<"Cannot initialize semaphore as it has been created yet";
		return false;
	}
	int error = ::sem_init(&FImpl->FSemaphore, 0, value);
	DCHECK_GE(error , 0);
	FImpl->FIsInited = error >= 0;
	return MIsInited();
}
CSemaphore::CSemaphore():FImpl(new CImpl)
{
	;
}
CSemaphore::~CSemaphore()
{
	MFree();
	delete FImpl;
}
void CSemaphore::MFree()
{
	if (MIsInited())
	{
		bool _val =::sem_destroy(&FImpl->FSemaphore) == 0;
		DCHECK(_val);
		FImpl->FIsInited=!_val;
	}
}
bool CSemaphore::MWait()
{
	bool _val =!::sem_wait(&FImpl->FSemaphore);
	DCHECK (_val);
	return _val;
}
bool CSemaphore::MWait(double const aTime)
{
	struct timespec tm;
	get_time(&tm);
	add(&tm, aTime);
	bool _val =!::sem_timedwait(&FImpl->FSemaphore, &tm);
	return _val;
}
bool CSemaphore::MTryWait()
{
	return sem_trywait(&FImpl->FSemaphore) == 0;
}

bool CSemaphore::MPost()
{
	bool _val =!::sem_post(&FImpl->FSemaphore);
	DCHECK  (_val);
	return _val;
}

int CSemaphore::MValue() const
{
	int value;

	bool _val=!::sem_getvalue(&FImpl->FSemaphore, &value);
	DCHECK (_val);
	return value;
}
bool CSemaphore::MIsInited() const
{
	return  FImpl->FIsInited;
}
}
#endif //#if defined (HAVE_POSIX_SEMAPHORES)


