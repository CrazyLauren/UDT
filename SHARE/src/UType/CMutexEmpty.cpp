// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CMutexEmpty.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
namespace NSHARE
{
CMutexEmpty::CMutexEmpty(eMutexType aType) :
		FFlags(aType)
{
	FThreadID = 0;
}
bool CMutexEmpty::MLock(void)
{
	if (FThreadID)
	{
		DCHECK_EQ(FThreadID, CThread::sMThreadId());
		DCHECK_EQ(FFlags , CMutex::MUTEX_RECURSIVE);
	}

	FThreadID = CThread::sMThreadId();
	return true;
}
bool CMutexEmpty::MUnlock(void)
{
	DCHECK_EQ(FThreadID, CThread::sMThreadId());
	FThreadID = 0;
	return true;
}
bool CMutexEmpty::MCanLock(void)
{
	if(FThreadID)
		return false;
	return MLock();
}
unsigned CMutexEmpty::MThread() const
{
	return FThreadID;
}
}

