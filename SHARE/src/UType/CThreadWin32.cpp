// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CThreadWin32.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#if   defined(_WIN32)
#include <deftype>
//#include <boost/thread/thread.hpp>
#include <SHARE/uclock.h>
#include <SHARE/UType/CThread.h>
#include <string>
#include <process.h>
#include <stdlib.h>
#include <winbase.h>

#if __cplusplus >=201103L
#	include <thread>
#endif
struct _thread_canceled_t{};
namespace NSHARE
{
struct CThread::CImpl
{
	typedef SECURITY_ATTRIBUTES attr_t;

	CImpl(CThread& aThis) :
			FThis(aThis)
	{
		FID=0;
		FPThread=INVALID_HANDLE_VALUE;
	}
	~CImpl()
	{

	}
	static unsigned WINAPI sMThreadFunc(void* p);

	inline void MStart(const param_t& aParam);
	void MWaitForCreated();
private:
	void MSetCpuNum();
	void MSetPriority();

	unsigned int   FID;
	HANDLE FPThread;
	CThread& FThis;
	NSHARE::CMutex FMutex;
	NSHARE::CCondvar FCond;
	friend class CThread;
};
void CThread::CImpl::MWaitForCreated()
{
	for (; !FThis.MIsRunning(); sMYield())
		;
}
void CThread::CImpl::MSetCpuNum()
{
	if (sMNumberOfProcessor() == 1)
		return;

    if (!FThis.MIsRunning())
       return;
    if (FPThread == INVALID_HANDLE_VALUE)
       return ;	

    DWORD affinityMask = 0x1 << FThis.FParam.priority;
	SetThreadAffinityMask(FPThread, affinityMask);
}
void CThread::CImpl::MSetPriority()
{
	int prio = THREAD_PRIORITY_NORMAL;
	switch (FThis.MGetPriority())
	{
	case CThread::THREAD_PRIORITY_MAX:
		prio = THREAD_PRIORITY_HIGHEST;
		break;
	case CThread::THREAD_PRIORITY_HIGH:
		prio = THREAD_PRIORITY_ABOVE_NORMAL;
		break;
	case CThread::THREAD_PRIORITY_DEFAULT:
	case CThread::THREAD_PRIORITY_NOMINAL:
		prio = THREAD_PRIORITY_NORMAL;
		break;
	case CThread::THREAD_PRIORITY_LOW:
		prio = THREAD_PRIORITY_BELOW_NORMAL;
		break;
	case CThread::THREAD_PRIORITY_MIN:
		prio = THREAD_PRIORITY_IDLE;
		break;
	case CThread::THREAD_PRIORITY_REAL_TIME:
		prio = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	case CThread::THREAD_PRIORITY_AS_IDLE:
		LOG(DFATAL)<<"Cannot set idle priority thread";
		break;
	}
	SetThreadPriority(FPThread, prio);
}

inline void CThread::CImpl::MStart(const param_t& aP)
{
	FPThread = (HANDLE) _beginthreadex(NULL, 0, CThread::CImpl::sMThreadFunc,
			this, 0, &FID);

	if(FPThread != INVALID_HANDLE_VALUE)
		FThis.MSetRunnig(false);
	else
		ResumeThread(FPThread);
}

unsigned CThread::CImpl::sMThreadFunc(void* aData)
{
	CThread::CImpl* _pThis = static_cast<CThread::CImpl*>(aData);
	_pThis->MSetPriority();
	_pThis->MSetCpuNum();
	_pThis->FThis.MSetRunnig(true);
	try
	{
		_pThis->FThis.MRun();
		//todo
	} catch (_thread_canceled_t const&)
	{
		_pThis->FThis.MCancelCleanUp();
	}
	catch (std::exception const& aExcept)
	{
		LOG(DFATAL) << "Unhandled exception."<< aExcept.what();
	}
	catch(...)
	{
		LOG(DFATAL)<<"Unknown unhandled exception.";
	}

	if (_pThis->FPThread != INVALID_HANDLE_VALUE)
	{
		HANDLE _tmp=_pThis->FPThread;
		_pThis->FPThread=INVALID_HANDLE_VALUE;
		CloseHandle(_tmp);
	}

	{
		CRAII<CMutex> _lock(_pThis->FMutex);
		_pThis->FThis.MSetRunnig(false);
		_pThis->FCond.MBroadcast();
	}


	VLOG(2)<<"Thread closed...";
	_endthreadex(0);
	return 0;
}

CThread::CThread() :
		FImpl(new CImpl(*this)),//
		FCleanUp(this)
{
	MInit();
}
CThread::CThread(const param_t& aParam) :
		FImpl(new CImpl(*this)), //
		FCleanUp(this), //
		FParam(aParam)
{
	DCHECK(FParam.MIsValid());
	MInit();
}
CThread::CThread(const NSHARE::CConfig& aConf) :
		FImpl(new CImpl(*this)), //
		FCleanUp(this), //
		FParam(param_t(aConf.MChild(param_t::NAME)))
{
	DCHECK(FParam.MIsValid());
	MInit();
}
CThread::CThread(CB_t const& aCB, const param_t& aParam) :
		FImpl(new CImpl(*this)), //
		FCleanUp(this), //
		FParam(aParam)
{
	DCHECK(FParam.MIsValid());
	if (aCB.MIs())
		MAdd(aCB);
	MInit();
}
CThread::CThread(CB_t const& aCB, const NSHARE::CConfig& aConf) :
		FImpl(new CImpl(*this)), //
		FCleanUp(this), //
		FParam(param_t(aConf.MChild(param_t::NAME)))
{
	DCHECK(FParam.MIsValid());
	if (aCB.MIs())
		MAdd (aCB);
	MInit();
}

inline void CThread::MInit()
{
	FIsDetached = false;
	MSetRunnig(false);
}
CThread::~CThread()
{
	VLOG(3) << "Destruct CThread: " << this;
	if (MIsRunning())
	{
		VLOG(1) << "Thread still running." << (*this);
		if (MCancel())
			MJoin();
		else
			MKill();
		MSetRunnig(false);
	}
	delete FImpl;
	FImpl = NULL;
}

bool CThread::MCreate(const param_t* aParam)
{
	VLOG(2) << "Create thread. Param=" << aParam << ":" << this;
	if (MIsRunning())
	{
		LOG(ERROR)<< "The Thread already running." << (*this);
		MCancel();
	}
	if(aParam)
		FParam=*aParam;

	FImpl->MStart(FParam);
	FImpl->MWaitForCreated();
	return MIsRunning();
}

bool CThread::MTestCancel()
{
	LOG_IF(DFATAL,!MIsRunning())<<"The thread is not running.";
	if (sMThreadId() != MThreadId())
	{
		LOG(DFATAL)<< "Test cancel. The current thread ID and thread ID not equal.";

		return false;
	}
	//TODO
	//if(WaitForSingleObject(pd->cancelEvent.get(),0) != WAIT_OBJECT_0) return true;
	return false;
}
bool CThread::MCancel()
{
	VLOG(2) << "Canceling thread";
	if(!MIsRunning())
		return false;

	HANDLE const _tmp=FImpl->FPThread;
	//FImpl->FPThread=INVALID_HANDLE_VALUE;
	//bool const _rval =CloseHandle(_tmp)!=FALSE;
//	if(!_rval)
//	{
//		LOG(ERROR)<<"Unknown error";
//		FImpl->FPThread = _tmp;
//		FIsRunning=false;
//	}

	//return _rval;
	return true;
}
void CThread::MDetach()
{
	FIsDetached=true;
}
bool CThread::MJoin(uint64_t aTime)
{
	CRAII<CMutex> _lock(FImpl->FMutex);
	 if (!MIsRunning() ||FIsDetached)
		 return false;
	 DWORD exit_code = -1;
	 GetExitCodeThread(FImpl->FPThread, &exit_code);
	 if (exit_code != STILL_ACTIVE)
		 return false;
	 FImpl->FCond.MTimedwait(&FImpl->FMutex);
	/*
	{
		CRAII<CMutex> _lock(FImpl->FMutex);
		if (!MIsRunning() || FIsDetached)
			return false;
	}
	BOOL b_ret;
	DWORD dw_ret;

	dw_ret = WaitForSingleObject(FImpl->FPThread, INFINITE);
	if (dw_ret == WAIT_OBJECT_0)
	{
		CRAII<CMutex> _lock(FImpl->FMutex);
		FIsRunning = false;
		return true;
	}
	else
	{
		DWORD exit_code = -1;
		GetExitCodeThread(FImpl->FPThread, &exit_code);

		CHECK_NE(STILL_ACTIVE, exit_code);
	}
	*/
	return true;
}

void CThread::MRun(void)
{
	LOG_IF(DFATAL,!MIsRunning())<<"The thread is not running.";
	if(MIsRunning())
	{
		int const _number=MCall(NULL);
		DCHECK_GT(_number,0);
	}
}

bool CThread::MSignal(int signal)
{
	LOG_IF(DFATAL,!MIsRunning())<<"The thread is not running.";
	if (MIsRunning())
	{
		MCancelCleanUp();
		FIsRunning = false;
		return TerminateThread(FImpl->FPThread, signal) != 0;
	}
	return false;
}

void CThread::MKill()
{
	LOG_IF(DFATAL,!MIsRunning())<<"The thread is not running.";
	if(MIsRunning())
	{
		MCancelCleanUp();
		FIsRunning = false;
		TerminateThread(FImpl->FPThread,-1);
	}
}
bool CThread::MSetPriority(eThreadPriority priority)
{
	FParam.priority = priority;
	if (!MIsRunning())
		return true;
	FImpl->MSetPriority();
	return true;
}
bool CThread::MSetPolicy(eSched aPolicy)
{
	if (!MIsRunning())
	{
		FParam.scheduling = aPolicy;
		return true;
	}
	else
		LOG(WARNING)<<"RT Setting Policy is not supported";
	return false;
}
unsigned CThread::sMNumberOfProcessor()
{
	static unsigned _num=0;
	if (!_num)
	{
		SYSTEM_INFO sysinfo;
		::GetSystemInfo(&sysinfo);
		_num=sysinfo.dwNumberOfProcessors;
	}
	return _num;
}
bool CThread::sMYield()
{
#if __cplusplus >=201103L && !defined(__MINGW32__)
	std::this_thread::yield();
#else
	if (!SwitchToThread())
	{
		Sleep(0);
	}
#endif
	return true;
}
CThread::process_id_t CThread::MThreadId()const
{
	LOG_IF(DFATAL,!MIsRunning())<<"The thread is not running.";
	if (MIsRunning())
		return FImpl->FID;
	return 0;
}
CThread::process_id_t CThread::sMThreadId()
{
	return GetCurrentThreadId();
}
CThread::process_id_t CThread::sProcessId()
{
	return GetCurrentProcessId();
}

std::ostream& CThread::MPrint(std::ostream& aVal)const
{
	if (!MIsRunning())
	{
		aVal << "Thread is not running.";
		return aVal;
	}
	aVal << "Thread ID:" << MThreadId() << ". Priority=";

	aVal <<MGetPriority();

	if (FParam.cpu_number >= 0)
		aVal << ". CPU:" << FParam.cpu_number;

	return aVal;
}
} //namespace NSHARE
#endif//#if   defined(_WIN32)

#if   !defined(_WIN32)
#include <deftype>
#include <SHARE/uclock.h>
#include <SHARE/UType/CThread.h>
#endif
namespace NSHARE
{
	const NSHARE::CText CThread::NAME="thread";
	const NSHARE::CText CThread::RUN="thrun";
	const NSHARE::CText CThread::THREAD_ID="thid";
	const NSHARE::CText CThread::DETACHED="detach";

	NSHARE::CConfig CThread::MSerialize() const
	{
		CConfig _conf(NAME);
		_conf.MAdd(RUN,MIsRunning());
		_conf.MAdd(MGetSetting().MSerialize());
		if(MIsRunning())
		{
			_conf.MAdd(THREAD_ID,MThreadId());
			_conf.MAdd(DETACHED,MIsDetached());
		}
		return _conf;
	}
	void CThread::MCancelCleanUp()
	{
		DLOG_IF(WARNING,!MIsRunning())<<"The thread is not running.";
		if(MIsRunning())
		{
			FCleanUp.MCall(NULL);
		}
	}
	bool CThread::MSetProcessor(unsigned int cpunum)
	{
		VLOG(2) << "Selecting  " << cpunum << " processor";
		if (cpunum > sMNumberOfProcessor() || MIsRunning())
		{
			LOG(ERROR)<< "Selecting processor error. Invalid number of CPU or the Thread is running.";
			return false;
		}

		FParam.cpu_number = cpunum;
		return true;
	}
	const NSHARE::CText CThread::param_t::NAME="thset";
	const NSHARE::CText CThread::param_t::PRIORITY="thpior";
	const NSHARE::CText CThread::param_t::SCHEDULING="thsched";
	const NSHARE::CText CThread::param_t::CPU_NUMBER="thcpu";

	CThread::param_t::param_t():
		priority(THREAD_PRIORITY_DEFAULT),
		scheduling(FIFO),
		cpu_number(-1)
	{
		;
	}
	CThread::param_t::param_t(NSHARE::CConfig const& aConf):
			//see MIsValid()
				priority(THREAD_PRIORITY_AS_IDLE),
				scheduling((eSched)0),
				cpu_number(CThread::sMNumberOfProcessor())
	{
		aConf.MGetIfSet(PRIORITY, priority);
		unsigned _val=0;
		if(aConf.MGetIfSet<unsigned>(SCHEDULING, _val))
			scheduling=(eSched)_val;
		aConf.MGetIfSet(CPU_NUMBER, cpu_number);
	}
	bool CThread::param_t::MIsValid() const
	{
		return priority<THREAD_PRIORITY_AS_IDLE&&//
				scheduling>0&&//
				cpu_number<(int)CThread::sMNumberOfProcessor();
	}
	NSHARE::CConfig CThread::param_t::MSerialize() const
	{
		DCHECK(MIsValid());

		NSHARE::CConfig _conf(NAME);
		_conf.MAdd(PRIORITY, priority);
		_conf.MAdd<unsigned>(SCHEDULING, scheduling);
		_conf.MAdd(CPU_NUMBER, cpu_number);

		return _conf;
	}
}

