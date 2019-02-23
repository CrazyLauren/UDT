/*
 * CSemaphore.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <deftype>

#include <assert.h>
#if (defined _WIN32 || defined _WIN64) //&& !defined (__MINGW32__)
#	include <windows.h>
#elif  defined (HAVE_POSIX_SEMAPHORES)
#	include <semaphore.h>
#else
#	include <pthread.h>
#endif

namespace NSHARE
{
struct CSemaphore::CImpl
{
#if (defined _WIN32 || defined _WIN64) //&& !defined (__MINGW32__)
	mutable HANDLE FSemaphore;

#elif defined(HAVE_POSIX_SEMAPHORES)
	mutable sem_t FSemaphore;
#else

	struct sema_t
	{
		unsigned int count;
		unsigned long numWaiting;
		pthread_mutex_t mutex; //FIXME CMutex
		pthread_cond_t nonZero;//FIXME CCondvar
	};

	mutable sema_t FSemaphore;

#endif
	bool FIsInited;
};
#if (defined _WIN32 || defined _WIN64) //&& !defined (__MINGW32__)
CSemaphore::CSemaphore(unsigned int value):FImpl(new CImpl)
{
	FImpl->FSemaphore = ::CreateSemaphore(0, value, value, 0);
	FImpl->FIsInited = FImpl->FSemaphore != 0;
	MASSERT_1(FImpl->FIsInited);
}

CSemaphore::~CSemaphore()
{
	if (FImpl->FIsInited)
	{
		bool _val = ::CloseHandle(FImpl->FSemaphore) != FALSE;
		MASSERT_1(_val);
	}
	delete FImpl;
}

void CSemaphore::MWait()
{
	bool _val = ::WaitForSingleObject(FImpl->FSemaphore, INFINITE) == WAIT_OBJECT_0;
	MASSERT_1(_val);
}

bool CSemaphore::MTryWait()
{
	return ::WaitForSingleObject(FImpl->FSemaphore, 0) == WAIT_OBJECT_0;
}

void CSemaphore::MPost()
{
	bool _val = ::ReleaseSemaphore(FImpl->FSemaphore, 1, 0)!=0;
	MASSERT_1(_val);
}

int CSemaphore::MValue() const
{
	LONG v = -1;
	bool _val = ::ReleaseSemaphore(FImpl->FSemaphore, 0, &v) && v > 0;
	MASSERT_1(_val)
	return v;
}
#elif defined(HAVE_POSIX_SEMAPHORES)
CSemaphore::CSemaphore(unsigned int value):FImpl(new CImpl)
{
	int error = ::sem_init(&FImpl->FSemaphore, 0, value);
	MASSERT_1(error >= 0);
	FImpl->FIsInited = error >= 0;
}

CSemaphore::~CSemaphore()
{
	if (FImpl->FIsInited)
	{
		bool _val =::sem_destroy(&FImpl->FSemaphore) == 0;
		MASSERT_1 (_val)
	}
	delete FImpl;
}
void CSemaphore::MWait()
{
	bool _val =!::sem_wait(&FImpl->FSemaphore);
	MASSERT_1 (_val)
}

bool CSemaphore::MTryWait()
{
	return sem_trywait(&FImpl->FSemaphore) == 0;
}

void CSemaphore::MPost()
{
	bool _val =!::sem_post(&FImpl->FSemaphore);
	MASSERT_1 (_val)
}

int CSemaphore::MValue() const
{
	int value;

	bool _val=!::sem_getvalue(&FImpl->FSemaphore, &value);
	MASSERT_1(_val);
	return value;
}
#else
CSemaphore::CSemaphore(unsigned int value):FImpl(new CImpl)
{
	bool _val=!::pthread_mutex_init(&FImpl->FSemaphore.mutex, 0);
	MASSERT_1(_val);
	_val=! ::pthread_cond_init(&FImpl->FSemaphore.nonZero, 0);
	MASSERT_1(_val);
	FImpl->FSemaphore.count = value;
	FImpl->FSemaphore.numWaiting = 0;
	FImpl->FIsInited=true;
}

CSemaphore::~CSemaphore()
{
	bool _val=!::pthread_cond_destroy(&FImpl->FSemaphore.nonZero);
	MASSERT_1(_val);
	_val=!::pthread_mutex_destroy(&FImpl->FSemaphore.mutex);
	MASSERT_1(_val);
	delete FImpl;
}

void CSemaphore::MWait()
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

void CSemaphore::MPost()
{
	::pthread_mutex_lock(&FImpl->FSemaphore.mutex);

	if (FImpl->FSemaphore.numWaiting > 0)
	{
		if (::pthread_cond_signal(&FImpl->FSemaphore.nonZero))
		{
			::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);

			MASSERT_1(false);
		}
	}

	FImpl->FSemaphore.count++;
	::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
}

int CSemaphore::MValue() const
{
	::pthread_mutex_lock(&FImpl->FSemaphore.mutex);
	int value = FImpl->FSemaphore.count;
	::pthread_mutex_unlock(&FImpl->FSemaphore.mutex);
	return value;
}
#endif//HAVE_POSIX_SEMAPHORES
bool CSemaphore::MIsInited() const
{
	return  FImpl->FIsInited;
}
}
