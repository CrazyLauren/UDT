// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CBarrier.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
namespace NSHARE
{
struct CBarrier::CImpl {
    CImpl(int aVal):
    	FMutex(CMutex::MUTEX_RECURSIVE), //
		FBarrier(aVal),//
		FNumThread(0),//
		FStateFlag(0),//
		FReleaseFlag(0) {

    }

    CCondvar FCond;
    CMutex    FMutex;
    volatile int       FBarrier;
    volatile int       FNumThread;
    volatile int       FStateFlag;
    volatile int       FReleaseFlag;
};

CBarrier::CBarrier(int aNum)
{
	FPImpl = new CImpl(aNum);
}

CBarrier::~CBarrier()
{
	delete FPImpl;
}
void CBarrier::MReset()
{
    FPImpl->FNumThread = 0;
    FPImpl->FStateFlag = 0;
}
bool CBarrier::MWait(unsigned  aNum)
{
    if(aNum != 0) FPImpl->FBarrier = aNum;

	CRAII<CMutex> lock(FPImpl->FMutex);
	int const my_phase = FPImpl->FStateFlag;
	++FPImpl->FNumThread;

	if (FPImpl->FNumThread == FPImpl->FBarrier)
	{             // Последний
		FPImpl->FNumThread = 0;

		FPImpl->FStateFlag = 1 - my_phase;
		FPImpl->FReleaseFlag=FPImpl->FStateFlag;

		FPImpl->FCond.MBroadcast();
	}
	else
	{
		while (FPImpl->FStateFlag == my_phase)
		{
			FPImpl->FCond.MTimedwait(&FPImpl->FMutex);
		}
	}
	return FPImpl->FReleaseFlag==FPImpl->FStateFlag;
}
bool CBarrier::MShutDown ()
{
    CRAII<CMutex> lock(FPImpl->FMutex);
    int const _phase = FPImpl->FStateFlag;

    FPImpl->FNumThread = 0;

    FPImpl->FReleaseFlag=_phase;
    FPImpl->FStateFlag = 1 - _phase;

    FPImpl->FCond.MBroadcast();
    return true;
}
} /* namespace NSHARE */
