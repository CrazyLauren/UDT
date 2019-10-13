// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSemaphoreWin32.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#if (defined _WIN32 || defined _WIN64)

#include <deftype>
#include <assert.h>
#include <windows.h>

namespace NSHARE
{
unsigned const ISemaphore::MAX_VALUE=std::numeric_limits<long>::max()/2;

struct CSemaphore::CImpl
{
	mutable HANDLE FSemaphore;
	bool FIsInited;
};
CSemaphore::CSemaphore(unsigned int value):FImpl(new CImpl)
{
	MInit(value);
	DCHECK(MIsInited());
}
CSemaphore::CSemaphore():FImpl(new CImpl)
{
}

CSemaphore::~CSemaphore()
{
	MFree();
	delete FImpl;
}
bool CSemaphore::MInit(unsigned int value)
{
	if (MIsInited())
	{
		LOG(DFATAL)<<"Cannot initialize semaphore as it has been created yet";
		return false;
	}
	FImpl->FSemaphore = ::CreateSemaphore(0, value, MAX_VALUE, 0);
	FImpl->FIsInited = FImpl->FSemaphore != 0;
	return MIsInited();
}
bool CSemaphore::MWait()
{
	bool _val = ::WaitForSingleObject(FImpl->FSemaphore, INFINITE) == WAIT_OBJECT_0;
	DCHECK(_val);
	return _val;
}
bool CSemaphore::MWait(double const aTime)
{
	DCHECK_NOTNULL(FImpl);
	unsigned msec = static_cast<unsigned>(aTime*1000);
	bool _val = ::WaitForSingleObject(FImpl->FSemaphore, msec) == WAIT_OBJECT_0;
	return _val;
}
bool CSemaphore::MTryWait()
{
	return ::WaitForSingleObject(FImpl->FSemaphore, 0) == WAIT_OBJECT_0;
}

bool CSemaphore::MPost()
{
	bool _val = ::ReleaseSemaphore(FImpl->FSemaphore, 1, 0)!=0;
	DCHECK(_val);
	return _val;
}

int CSemaphore::MValue() const
{
	LONG v = -1;
	bool _val = ::ReleaseSemaphore(FImpl->FSemaphore, 0, &v) && v > 0;
	DCHECK(_val);
	return v;
}
bool CSemaphore::MIsInited() const
{
	return  FImpl->FIsInited;
}

void CSemaphore::MFree()
{
	if (MIsInited())
	{
		bool _val = ::CloseHandle(FImpl->FSemaphore) != FALSE;
		DCHECK(_val);
		FImpl->FIsInited=!_val;
	}
}
}
#endif // #if (defined _WIN32 || defined _WIN64)
