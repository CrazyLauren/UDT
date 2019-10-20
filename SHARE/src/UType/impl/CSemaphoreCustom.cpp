// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSemaphoreCustom.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#if !(defined (_WIN32) || (defined _WIN64)) && !defined (HAVE_POSIX_SEMAPHORES)

#include <deftype>
#include <assert.h>
#include <time.h>

#include <pthread.h>

namespace NSHARE
{
unsigned const ISemaphore::MAX_VALUE=std::numeric_limits<unsigned>::max()/2;;
struct CSemaphore::CImpl
{
	struct sema_t
	{
		unsigned int count;
		unsigned long numWaiting;
		pthread_mutex_t mutex; //FIXME CMutex
		pthread_cond_t nonZero;//FIXME CCondvar
	};

	mutable sema_t FSemaphore;

	bool FIsInited;
};
CSemaphore::CSemaphore(unsigned int value):FImpl(new CImpl)
{
	MInit(value);
	DCHECK(MIsInited());
}
CSemaphore::CSemaphore():FImpl(new CImpl)
{
	;
}
bool CSemaphore::MInit(unsigned int value)
{
	if (MIsInited())
	{
		LOG(DFATAL)<<"Cannot initialize semaphore as it has been created yet";
		return false;
	}
	bool _val=!::pthread_mutex_init(&FImpl->FSemaphore.mutex, 0);

	DCHECK(_val);

	if(_val)
	{
		_val=! ::pthread_cond_init(&FImpl->FSemaphore.nonZero, 0);
		if(_val)
		{
			DCHECK(_val);
			FImpl->FSemaphore.count = value;
			FImpl->FSemaphore.numWaiting = 0;
			FImpl->FIsInited=true;
		}
		else
			::pthread_mutex_destroy(&FImpl->FSemaphore.mutex);
	}

	return MIsInited();
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
		bool _val=!::pthread_cond_destroy(&FImpl->FSemaphore.nonZero);
		DCHECK(_val);
		_val=!::pthread_mutex_destroy(&FImpl->FSemaphore.mutex);
		DCHECK(_val);
		FImpl->FIsInited=!_val;
	}
}
bool CSemaphore::MWait()
{
	::pthread_mutex_lock(&FImpl->FSemaphore.mutex);

	FImpl->FSemaphore.numWaiting++;

	while (FImpl->FSemaphore.count == 0)
	{
		if (::pthread_cond_wait(&FImpl->FSemaphore.nonZero,
						&FImpl->FSemaphore.mutex))
		{
			::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
			MASSERT_1(false);
		}
	}

	FImpl->FSemaphore.numWaiting--;
	FImpl->FSemaphore.count--;

	::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
	return true;
}
bool CSemaphore::MWait(double const aTime)
{
	struct timespec tm;
	get_time(&tm);
	add(&tm, aTime);
	uint64_t const _wait_time=tm.tv_sec*(uint64_t)1000000000 +tm.tv_nsec;

	::pthread_mutex_lock(&FImpl->FSemaphore.mutex);


	FImpl->FSemaphore.numWaiting++;

	while (FImpl->FSemaphore.count == 0)
	{

		if (::pthread_cond_timedwait(&FImpl->FSemaphore.nonZero,
						&FImpl->FSemaphore.mutex,&tm)!=0)
		{
			::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
			MASSERT_1(false);
		}

		if(FImpl->FSemaphore.count != 0 && get_unix_time()> _wait_time)
			break;
	}

	FImpl->FSemaphore.numWaiting--;
	FImpl->FSemaphore.count--;

	::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
	return true;
}

bool CSemaphore::MTryWait()
{
	::pthread_mutex_lock(&FImpl->FSemaphore.mutex);

	if (FImpl->FSemaphore.count == 0)
	{
		::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
		return false;
	}
	else
	{
		FImpl->FSemaphore.count--;
		::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
		return true;
	}
}

bool CSemaphore::MPost()
{
	::pthread_mutex_lock(&FImpl->FSemaphore.mutex);

	if (FImpl->FSemaphore.numWaiting > 0)
	{
		if (::pthread_cond_signal(&FImpl->FSemaphore.nonZero))
		{
			::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);

			DCHECK(false);
		}
	}

	FImpl->FSemaphore.count++;
	::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);

	return true;
}

int CSemaphore::MValue() const
{
	::pthread_mutex_lock(&FImpl->FSemaphore.mutex);
	int value = FImpl->FSemaphore.count;
	::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
	return value;
}
bool CSemaphore::MIsInited() const
{
	return  FImpl->FIsInited;
}
}
#endif//#if !(defined (_WIN32) || (defined _WIN64)) && !defined (HAVE_POSIX_SEMAPHORES)
