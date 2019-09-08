// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTimeDispatcher.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <random_value.h>
#include <core/CDataObject.h>
#include <core/CConfigure.h>
#include "CTimeDispatcher.h"
#define NUM_THREAD 10
template<>
NUDT::CTimeDispatcher::singleton_pnt_t NUDT::CTimeDispatcher::singleton_t::sFSingleton =
NULL;
namespace NUDT
{
using namespace NSHARE;

const NSHARE::CText CTimeDispatcher::NAME = "time_dispatcher";
const NSHARE::CText CTimeDispatcher::SHARED_MEMORY_NAME = "name";

unsigned const CTimeDispatcher::SHARED_MEMORY_SIZE=1024;
bool CTimeDispatcher::TEST_IS_ON=false;

CTimeDispatcher::CTimeDispatcher()://
				ICore(NAME)
{
	MInit();
}

CTimeDispatcher::~CTimeDispatcher()
{

}
void CTimeDispatcher::MInit()
{
}

bool CTimeDispatcher::MStart()
{
	CConfig const* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);

	if (!_p->MGetIfSet(SHARED_MEMORY_NAME, FName))
	{
		//DLOG(FATAL)<<"Cannot open sm server as the name is not set. Value: "<<SHARED_MEMORY_SIZE;
	}
	LOG(INFO)<<"Time dispatcher shared memory name:"<<FName;

	if (MCreateSharedMemory())
	{
		MRegistreNewRTC();
	}
	else
		return false;
	return true;
}
/** Create share memory for RTC
 *
 * @return true if is created
 */
bool CTimeDispatcher::MCreateSharedMemory()
{
	FName.MMakeRandom(10);
	if (FMemory.MOpenOrCreate(FName, SHARED_MEMORY_SIZE)==CSharedMemory::E_NO_ERROR)
	{
		return true;
	}

	return false;
}
void CTimeDispatcher::MStop()
{
	VLOG(3)<<"Stopping CTimeDispatcher";

	{
		safety_array_of_rtc_t::WAccess<> _accesss =
				FArrayOfRTC.MGetWAccess();
		_accesss->clear();
	}
}
/** The static method of thread pool
 *
 * @param WHO
 * @param WHAT
 * @param aData
 * @return The operation has to be removed
 */
NSHARE::eCBRval CTimeDispatcher::sMMainLoop(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CRTCForModeling*>(aData)->MDispatcher();
	return E_CB_REMOVE;
}

/** Check if specified a RTC name exist
 *
 * @param aRtcGroup a RTC name that want to check
 * @return true if exist
 */
bool CTimeDispatcher::MIsRTCExist(
		const NSHARE::CProgramName& aRtcGroup)
{
	safety_array_of_rtc_t::RAccess<> const _accesss=FArrayOfRTC.MGetRAccess();
	array_of_rtc_t const& _rtc=*_accesss;
	array_of_rtc_t::const_iterator _it = _rtc.begin();
	for (; _it != _rtc.end(); ++_it)
		if ((*_it)->FName == aRtcGroup)
			break;
	return _it!=_rtc.end();
}
/** Start RTC dispatcher
 *
 * @param _rval aDispatcher ID
 */
void CTimeDispatcher::MStartRTCDispatcher(rtc_id_t _rval)
{
	const safety_array_of_rtc_t::RAccess<> _access = FArrayOfRTC.MGetRAccess();
	CRTCForModeling* const _p = (*_access)[_rval];
	/** Start main thread. */
	NSHARE::operation_t _op(CTimeDispatcher::sMMainLoop, _p,
			NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);
}
/** Push a new RTC
 *
 * @param aRtcGroup aRTC name
 * @return -1 if error otherwise RTC Id
 */
CTimeDispatcher::rtc_id_t CTimeDispatcher::MPushNewRTC(
		const NSHARE::CProgramName& aRtcGroup)
{
	rtc_id_t _rval = -1;
	safety_array_of_rtc_t::WAccess<> _accesss = FArrayOfRTC.MGetWAccess();

	array_of_rtc_t& _rtc = *_accesss;
	array_of_rtc_t::const_iterator _it = _rtc.begin();

	for (; _it != _rtc.end(); ++_it)
		if ((*_it)->FName == aRtcGroup)
			break;
	if (_it == _rtc.end())
	{
		_rtc.push_back(new CRTCForModeling(FMemory.MGetAllocator(), aRtcGroup));
		_rval = _rtc.size() - 1;
	}
	else
		LOG(DFATAL) << "The RTC " << aRtcGroup << " is exist";

	return _rval;
}

/** Create timer
 *
 * @param aRtcGroup for whom is RTC
 * @return -1 if error otherwise RTC Id
 */
CTimeDispatcher::rtc_id_t CTimeDispatcher::MRegistreNewRTC(NSHARE::CProgramName const& aRtcGroup)
{
	rtc_id_t const _rval= MPushNewRTC(aRtcGroup);

	if (_rval >= 0)
	{
		MStartRTCDispatcher(_rval);
		MStartTestIfNeed(_rval);
	}
	return _rval;
}
NSHARE::CConfig CTimeDispatcher::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	return _config;
}
/** For testing
 *
 * @param WHO
 * @param WHAT
 * @param aData
 * @return
 */
NSHARE::eCBRval CTimeDispatcher::sMTestLoop(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* aData)
{
	sMTestLoop(reinterpret_cast<CRTCForModeling*>(aData));
	return E_CB_REMOVE;
}
/** Start test if need
 *
 * @param aRtcID - RTC Id
 */
void CTimeDispatcher::MStartTestIfNeed(rtc_id_t aRtcID)
{
	if (TEST_IS_ON)
	{
		safety_array_of_rtc_t::RAccess<> const _access=FArrayOfRTC.MGetRAccess();
		CRTCForModeling* const _p=(*_access)[aRtcID];

		for (unsigned i = 0; i < NUM_THREAD; ++i)
		{
			NSHARE::operation_t _op(CTimeDispatcher::sMTestLoop, _p,
					NSHARE::operation_t::IO);
			CDataObject::sMGetInstance().MPutOperation(_op);
		}
	}
}
/** Test loop
 *
 *@param aRTC pointer to RTC that you  want to test
 */
void CTimeDispatcher::sMTestLoop(CRTCForModeling* aRTC)
{
	CHECK_NOTNULL(aRTC);

	aRTC->MJoinToRTCWorker();

	CRTCForModeling::rtc_time_t _cur_time=aRTC->MGetCurrentTime();
	CRTCForModeling::rtc_time_t _prev_next_time=_cur_time;
	CRTCForModeling::rtc_time_t _next_time=_cur_time+1;

	for (;_cur_time<(time_info_t::END_OF_TIME-1000000); //
			_prev_next_time = _next_time, //
			_next_time += (NSHARE::get_random_value_by_RNG() % 1000000) //
			)
	{
		DCHECK_GE(_next_time,_cur_time);
		for (; (_cur_time = aRTC->MNextTime(_next_time)) != _next_time;)
		{
			CHECK_LE(_cur_time, _next_time);
		}
	}
}
CRTCForModeling::CRTCForModeling(NSHARE::IAllocater* aAllocator,//
		NSHARE::CProgramName const& aName):
		FName(aName),//
		FAllocator(aAllocator),//
		FTimeInfo(NULL),//
		FIsDone(false)
{
	FTimeInfo=allocate_object<time_info_t>(*FAllocator);

	FTimeUpdated.MInit(FTimeInfo->FEventTimeChanged, sizeof(FTimeInfo->FEventTimeChanged),0);
	FHasToBeUpdated.MInit(FTimeInfo->FHasToBeUpdated, sizeof(FTimeInfo->FHasToBeUpdated));

	FSem.MInit(FTimeInfo->FMutex, sizeof(FTimeInfo->FMutex),1);

}

CRTCForModeling::~CRTCForModeling()
{
	MUnRegistration();
}
void CRTCForModeling::MUnRegistration()
{
	if (MIsCreated())
	{
		{
			FIsDone = true;
			VLOG(2) << "Wait for dispatcher stopped";
			CRAII<CMutex> _destroy_lock(FDestroyMutex);
		}

		FTimeUpdated.MUnlink();
		FHasToBeUpdated.MUnlink();
		FSem.MUnlink();

		FAllocator->MDeallocate(FTimeInfo, 0);
		FTimeInfo = NULL;
	}
}

bool CRTCForModeling::MIsCreated() const
{
	return FTimeInfo!=NULL;
}

void CRTCForModeling::MJoinToRTCWorker()
{
	DCHECK(MIsCreated());

	CRAII<CIPCSem> _lock(FSem);
	++FTimeInfo->FNumOfWorking;
}
void CRTCForModeling::MUpdateTime(rtc_time_t aNewTime) const
{
	DVLOG(3)<<"Update time "<<aNewTime;
	FTimeInfo->FTime=aNewTime;

	FTimeInfo->FTimeHW=get_unix_time();
	if (FTimeInfo->FTime >= FTimeInfo->FNextTimer)
	{
		FTimeInfo->FNextTimer=std::numeric_limits<CRTCForModeling::rtc_time_t>::max();
		FTimeInfo->FNumOfUnlocked=FTimeInfo->FNumOfWait;
		unsigned _post=FTimeInfo->FNumOfUnlocked;
		for (unsigned i = 0; i < _post; ++i)
			FTimeUpdated.MPost();
	}
}
CRTCForModeling::rtc_time_t CRTCForModeling::MNextTime(rtc_time_t aNewTime) const
{
	using namespace std;
	DCHECK(MIsCreated());

	rtc_time_t _rval = 0;
	bool _done=false;
	for(;!_done;)
	{
		CRAII<CIPCSem> _lock(FSem);
		if (FTimeInfo->FNumOfUnlocked==0)
		{
			DCHECK_LE(FTimeInfo->FTime, aNewTime);
			DCHECK_GT(FTimeInfo->FNumOfWorking, 0u);

			DVLOG(3) << "New time is " << aNewTime;

			FTimeInfo->FNextTimer = min(aNewTime, FTimeInfo->FNextTimer);
			++FTimeInfo->FNumOfWait;
			if (FTimeInfo->FNumOfWait == FTimeInfo->FNumOfWorking)
			{
				DVLOG(3) << "Current minimal value: "
									<< FTimeInfo->FNextTimer;
				FHasToBeUpdated.MSignal();
				++FTimeInfo->FTimeStep;
			}

			DVLOG(3) << "Request: " << aNewTime << "  minimal time:"
								<< FTimeInfo->FNextTimer;
			FSem.MPost();

			FTimeUpdated.MWait();

			DCHECK_GT(FTimeInfo->FNumOfUnlocked,0);
			DCHECK_GT(FTimeInfo->FNumOfWait,0);

			--FTimeInfo->FNumOfWait;
			--FTimeInfo->FNumOfUnlocked;

			FSem.MWait();

			DCHECK_LE(FTimeInfo->FTime, aNewTime);
			DCHECK_GT(FTimeInfo->FNumOfWorking, FTimeInfo->FNumOfWait);

			_rval = FTimeInfo->FTime;
			_done=true;
		}
		else
		{
			_done=!NSHARE::CThread::sMYield();
			DLOG_IF(FATAL,_done)<<"Yield operation is not supported";
		}
	}

	return _rval;
}
bool CRTCForModeling::MDispatcher() const
{
	CRAII<CMutex> _destroy_lock(FDestroyMutex);
	CRTCForModeling::rtc_time_t _next_time=0;

	CRAII<CIPCSem> _lock(FSem);
	if (!FIsDone)
		do
		{
			NSHARE::atomic_t _step;
			do
			{
				_step=FTimeInfo->FTimeStep;
				FHasToBeUpdated.MTimedwait(&FSem);
			} while (_step == FTimeInfo->FTimeStep);

			DCHECK_EQ(FTimeInfo->FNumOfUnlocked,0);

			MUpdateTime(FTimeInfo->FNextTimer);

			//FTimeInfo->FNumOfWait=0;
		} while (!FIsDone //
				&& FTimeInfo//
				&& FTimeInfo->FTime != time_info_t::END_OF_TIME);

	LOG(INFO)<<"Time is ended for dispatcher "<<FName;
	return true;
}
bool CRTCForModeling::MResetRTC() const
{
	if(MIsCreated() && FTimeInfo->FTime==time_info_t::END_OF_TIME)
	{
		LOG(INFO)<<"Restart dispatcher "<<FName;
		FTimeInfo->FTime=0;
		FTimeInfo->FTimeStep=0;
	}
	return true;
}
CRTCForModeling::rtc_time_t CRTCForModeling::MGetCurrentTime() const
{
	if (MIsCreated())
	{
		CRAII<CIPCSem> _lock(FSem);
		return FTimeInfo->FTime;
	}
	else
		return 0;
}
} /* namespace NUDT */
