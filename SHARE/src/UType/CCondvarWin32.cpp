// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CWin32Condvar.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#if defined(_WIN32)
#include <deftype>
#include <windows.h>
#include <winerror.h>

#define InterlockedGet(x) InterlockedExchangeAdd(x,0)
namespace NSHARE
{
struct CCondvar::CImpl
{

	int FWaitNum; //waiters
	int FReleaseCount; //number of release thread
	int FWaitGenNum; // current "generation" - it don't allow
	//one thread to steal all the "releases" from the brodcast
	HANDLE FSignalEvent;
	CMutex FMutex;

	CImpl()
	{
		FWaitNum = 0;
		FReleaseCount = 0;
		FWaitGenNum = 0;
		FSignalEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		LOG_IF(FATAL,!FSignalEvent) << "CreateEvent failed. signalEvent:"
											<< FSignalEvent << " error:"
											<< GetLastError();
	}
	~CImpl()
	{
		::CloseHandle(FSignalEvent);
	}
	bool MWait(CMutex* _mutex, unsigned msec = INFINITE);
	bool MBroadcast(void);
	bool MSignal(void);
};

bool CCondvar::CImpl::MWait(CMutex* _mutex, unsigned msec)
{
	int _generation;
	{
		CRAII<CMutex> _block(FMutex);
		++FWaitNum;
		_generation = FWaitGenNum;
	}
	_mutex->MUnlock();
	DWORD ret;
	bool _is_timeout = false;
	for (HANG_INIT;;HANG_CHECK)
	{
		ret = ::WaitForSingleObject(FSignalEvent, msec);
		DVLOG(2) << "Ret = " << ret;

		CRAII<CMutex> _block(FMutex);
		{
			bool _wait_done=FReleaseCount>0 &&FWaitGenNum!=_generation;
			_is_timeout= ret == WAIT_TIMEOUT;

			LOG_IF(DFATAL,((ret!= NO_ERROR) && !_is_timeout))<<" Error in WaitForSingleObject "<<ret;

			if(_wait_done ||_is_timeout)
			{
				VLOG(2)<<"The condvar is unlocked. Number of Release "<<FReleaseCount//
						<<"; current 'generation' "<<FWaitGenNum//
						<<"; Gen="<<_generation;
				--FWaitNum;

				if(!_is_timeout)
				{
					--FReleaseCount;
					if(FReleaseCount == 0)
						ResetEvent(FSignalEvent);
				}
				break;
			}
			else
			{
				VLOG(1)<<"Does not for me. Number of Release "<<FReleaseCount	//
				<<"; current 'generation' "<<FWaitGenNum//
				<<"; Gen="<<_generation<<"; ret="<<ret<<"; msec="<<msec;
			}
		}
	}
	_mutex->MLock();
/*	bool _last;
	{
		CRAII<CMutex> _block(FMutex);
		--FWaitNum;
		--FReleaseCount;
		_last = FReleaseCount == 0;
	}
	if (_last)
		ResetEvent(FSignalEvent);*/
	return !_is_timeout;
}
bool CCondvar::CImpl::MSignal()
{
	CRAII<CMutex> _block(FMutex);
	if (FWaitNum > FReleaseCount)
	{
		VLOG(1) << "Do signal...";
		SetEvent(FSignalEvent);
		++FReleaseCount;
		++FWaitGenNum;
		return true;
	}
	VLOG(1) << "There is not expecting thread.";
	return false;
}
bool CCondvar::CImpl::MBroadcast(void)
{
	CRAII<CMutex> _block(FMutex);
	if (FWaitNum > 0)
	{
		VLOG(1) << "Do broadcast...";
		SetEvent(FSignalEvent);
		FReleaseCount = FWaitNum;
		++FWaitGenNum;
		return true;
	}
	VLOG(1) << "There is not expecting thread.";
	return false;
}
CCondvar::CCondvar()
{
	FPImpl = new CImpl();
}

CCondvar::~CCondvar()
{
	delete FPImpl;
}
bool  CCondvar::MTimedwait(IMutex *aMutex)
{
	DCHECK(dynamic_cast<CMutex*>(aMutex)!=NULL);
	return FPImpl->MWait(static_cast<CMutex *>(aMutex));
}
bool CCondvar::MTimedwait(IMutex *aMutex, double const aTime)
{
	DCHECK(dynamic_cast<CMutex*>(aMutex)!=NULL);
	//LOG_IF(DFATAL,aTime<0) << "Time is negative.";
	unsigned int const _time=(aTime<=0?INFINITE:(unsigned int)(aTime * 1000));
	return FPImpl->MWait(static_cast<CMutex *>(aMutex), _time);
}

bool CCondvar::MSignal()
{
	DCHECK_NOTNULL(FPImpl);
	return FPImpl->MSignal();
}

bool CCondvar::MBroadcast()
{
	DCHECK_NOTNULL(FPImpl);
	return FPImpl->MBroadcast();
}

}

#endif
