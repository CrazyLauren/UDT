/*
 * CCheckNoMultiThread.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 14.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#include <deftype>
#include <UType/CCheckNoMultiThread.h>
#include <boost/version.hpp>
#include <boost/interprocess/detail/atomic.hpp>
namespace NSHARE
{
#if (BOOST_VERSION / 100000 >=1) &&(BOOST_VERSION / 100 % 1000<=47)
using namespace boost::interprocess::detail;
#else
using namespace boost::interprocess::ipcdetail;
#endif
static NSHARE::CMutex _lock;
CCheckNoMultiThread::CCheckNoMultiThread(unsigned& aThread, char const* aFile,
		const unsigned & aLine) :
		FThread(aThread),//
		FFile(aFile),//
		FLine(aLine)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(_lock);
	unsigned const _cur=CThread::sMThreadId();
	LOG_IF(FATAL,FThread!=0) << "Multithreading fail in begin of "
														<< FFile << ":" << FLine
														<< " current Thread="
														<< _cur
														<< " doing Thread="
														<< FThread;
	FThread=_cur;
}

CCheckNoMultiThread::~CCheckNoMultiThread()
{
	NSHARE::CRAII<NSHARE::CMutex> _block(_lock);
	unsigned const _cur=CThread::sMThreadId();
	LOG_IF(FATAL, _cur!=FThread) << "Multithreading fail in end of "
														<< FFile << ":" << FLine
														<< " current Thread="
														<< _cur
														<< " doing Thread="
														<< FThread;
	FThread=0;
}

} /* namespace NSHARE */
