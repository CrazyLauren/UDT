// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CRTCForModeling.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRTCFORMODELING_H_
#define CRTCFORMODELING_H_

#include <SHARE/UType/CIPCMutex.h>
#include <SHARE/UType/CIPCSem.h>
#include <udt_rtc_types.h>
namespace NUDT
{
/** RTC realized for modeling
 *
 */
class CRTCForModeling: NSHARE::CDenyCopying
{
public:
	typedef time_info_t::rtc_time_t rtc_time_t;//!< time in

	/** default constructor
	 *
	 */
	CRTCForModeling();

	/** Create the new RTC
	 *
	 * @param aAllocator  An allocator of shared memory
	 * @param aName  The unique name of RTC
	 */
	CRTCForModeling(NSHARE::IAllocater* aAllocator,
			NSHARE::CProgramName const& aName);

	/** Open the exist RTC
	 *
	 * @param aAllocator  An allocator of shared memory
	 * @param aOffset  Offset to exist RTC
	 * @param aP  Pointer to time info
	 */
	CRTCForModeling(NSHARE::IAllocater* aAllocator,
			NSHARE::CProgramName const& aName,
			time_info_t* aP);

	~CRTCForModeling();

	/** Open the exist RTC
	 *
	 * @param aAllocator  An allocator of shared memory
	 * @param aOffset  Offset to exist RTC
	 * @param aP  Pointer to time info
	 */
	bool MOpen(NSHARE::IAllocater* aAllocator,
			NSHARE::CProgramName const& aName,
			time_info_t* aP);

	/** Destroy RTC
	 *
	 */
	bool MDestroyRTC();

	/** Add a new worker which is used
	 * the RTC
	 *
	 */
	bool MJoinToRTC();

	/** Gets is joined to RTC
	 *
	 *
	 * @return true if joined
	 */
	bool MIsJoinToRTC() const;

	/** Wait for specified time to expire or
	 * expire time of the other program
	 *
	 * @param aNewTime expired time
	 * @return current time
	 */
	rtc_time_t MNextTime(rtc_time_t aNewTime) const;

	/** Check for create time info
	 *
	 * @return true if time info has created
	 */
	bool MIsCreated() const;

	/** UnRegistration RTC
	 *
	 */
	bool MLeaveFromRTC();

	/** Perpetual loop of update
	 * time until finish
	 *
	 *	@return true if no error
	 */
	bool MDispatcher();

	/** Force stopping  dispatcher
	 *
	 *	@return true if no error
	 */
	bool MForceStopDispatcher();

	/** Reset RTC counter
	 *
	 */
	bool MResetRTC() const;

	/** Gets current time
	 *
	 * @return current time
	 */
	rtc_time_t MGetCurrentTime() const;

	/** Gets name of RTC
	 *
	 * @return name of RTC
	 */
	NSHARE::CProgramName const& MGetName() const;


	/** Gets offset to time info
	 *
	 * @return offset
	 */
	NSHARE::IAllocater::offset_pointer_t MGetOffset() const;

	/** Gets amount of joined
	 *
	 * @return number of joined or -1
	 */
	int MGetAmountOfJoined() const;
private:

	/** Updates time and notifies
	 * time receiver
	 *
	 * @warning Thread unsafety operation
	 * @param aNewTime a new time
	 */
	void MUpdateTime(uint64_t aNewTime) const;
	bool MInitializeSignals();
	void MDeInitializeSignals();
	void MUnlockWaiter() const;

	NSHARE::CProgramName FName; //!< The unique name of RTC
	NSHARE::IAllocater* FAllocator; //!<Pointer to allocator
	time_info_t* FTimeInfo; //!< Info about time
	mutable NSHARE::CIPCSem FTimeUpdated; //!< Event to signal the time is changed
	mutable NSHARE::CIPCSignalEvent FHasToBeUpdated; //!< Event to update time
	mutable NSHARE::CIPCMutex FSem; //!< Mutex for lock change of #FTimeInfo
	mutable NSHARE::CMutex FDestroyMutex;/*!< #MDispatcher has the infinite loop therefore
	 it need to wait until it's finished working
	 before the object will destroyed */
	mutable NSHARE::CMutex FLeaveMutex;/*!< Mutex for wait for leave operation finished
	 	 	 	 	 	 	 	 	 	 	 (#MNextTime operation stoped)
	 	 	 	 	 	 	 	 	 	 	 @warning Can died lock with #FSem
	 	 	 	 	 	 	 	 	 	 	 */
	bool FIsDone; //!< If true then dispatcher is working
	bool FIsJoin;//!<If true then joined
	bool FIsICreateInfo;//!< true I'm create time_info
};


inline CRTCForModeling::CRTCForModeling() :
		FAllocator(NULL), //
		FTimeInfo(NULL), //
		FIsDone(true),//
		FIsJoin(false),//
		FIsICreateInfo(false)
{

}
inline CRTCForModeling::CRTCForModeling(NSHARE::IAllocater* aAllocator, //
		NSHARE::CProgramName const& aName) :
		FAllocator(NULL), //
		FTimeInfo(NULL), //
		FIsDone(true),//
		FIsJoin(false),//
		FIsICreateInfo(false)
{
	DCHECK(aName.MIsValid());

	time_info_t* _p = NSHARE::allocate_object<time_info_t>(*aAllocator);
	if(_p==NULL ||  !MOpen(aAllocator,aName,_p))
	{
		aAllocator->MDeallocate(_p, 0);
		LOG(DFATAL)<<"Cannot create RTC "<<aName;
	}else
		FIsICreateInfo=true;
}
inline CRTCForModeling::CRTCForModeling(NSHARE::IAllocater* aAllocator,
		NSHARE::CProgramName const& aName,
		time_info_t* aP):
				FAllocator(NULL), //
				FTimeInfo(NULL), //
				FIsDone(true),//
				FIsJoin(false),//
				FIsICreateInfo(false)
{
	MOpen(aAllocator,aName,aP);
}
inline bool CRTCForModeling::MOpen(NSHARE::IAllocater* aAllocator,
		NSHARE::CProgramName const& aName,
		time_info_t* aP)
{
	DCHECK_NOTNULL(aP);
	if(aP==NULL)
		return false;
	FName=aName;
	FAllocator=aAllocator;
	FTimeInfo=aP;

	bool _is=MInitializeSignals();
	if(!_is)
	{
		FAllocator=NULL;
		FName=NSHARE::CProgramName();
		FTimeInfo=NULL;
	}
	return _is;
}
inline bool CRTCForModeling::MInitializeSignals()
{
	bool _is=true;
	_is=_is&&FTimeUpdated.MInit(FTimeInfo->FEventTimeChanged,
			sizeof(FTimeInfo->FEventTimeChanged), 0);
	_is=_is&&FHasToBeUpdated.MInit(FTimeInfo->FHasToBeUpdated,
			sizeof(FTimeInfo->FHasToBeUpdated));
	_is=_is&&FSem.MInit(FTimeInfo->FMutex, sizeof(FTimeInfo->FMutex));

	if(!_is)
		MDeInitializeSignals();

	return _is;
}
inline void CRTCForModeling::MDeInitializeSignals()
{
	FSem.MFree();
	FHasToBeUpdated.MFree();
	FTimeUpdated.MFree();
}
inline CRTCForModeling::~CRTCForModeling()
{
	MForceStopDispatcher();
	MLeaveFromRTC();
	MDestroyRTC();
}
inline bool CRTCForModeling::MLeaveFromRTC()
{
	if (!MIsCreated())
		return false;
	NSHARE::CRAII < NSHARE::CIPCMutex > _lock(FSem);

	if (!MIsJoinToRTC())
		return false;

	FIsJoin=false;

	DVLOG(1) << "Leaving from RTC: " << FName << ", Num of working"
			<< FTimeInfo->FNumOfWorking;
	MUnlockWaiter();

	{
		NSHARE::CRAII < NSHARE::CMutex > _leave_lock(FLeaveMutex);
		--FTimeInfo->FNumOfWorking;
	}

	if(FTimeInfo->FNumOfWorking==0)
	{
		MResetRTC();
	}
	return true;
}
inline int CRTCForModeling::MGetAmountOfJoined() const
{
	if (!MIsCreated())
		return -1;
	return FTimeInfo->FNumOfWorking;
}
inline bool CRTCForModeling::MDestroyRTC()
{
	if (!MIsCreated())
		return false;
	MDeInitializeSignals();
	{
		if(FIsICreateInfo)
			FAllocator->MDeallocate(FTimeInfo, 0);

		FAllocator=NULL;
		FName=NSHARE::CProgramName();
		FTimeInfo=NULL;
	}
	return true;
}
inline bool CRTCForModeling::MIsCreated() const
{
	return FTimeInfo != NULL;
}
inline bool CRTCForModeling::MIsJoinToRTC() const
{
	return FIsJoin;
}
inline bool CRTCForModeling::MJoinToRTC()
{
	DCHECK(MIsCreated());

	if(!MIsCreated())
		return false;

	NSHARE::CRAII < NSHARE::CIPCMutex > _lock(FSem);

	if(MIsJoinToRTC())
		return false;

	FIsJoin=true;

	++FTimeInfo->FNumOfWorking;
	DVLOG(1) << "Join to RTC: " << FName << ", Num of working"
				<< FTimeInfo->FNumOfWorking;
	return true;
}
inline void CRTCForModeling::MUpdateTime(rtc_time_t aNewTime) const
{
	DVLOG(3) << "Update time " << aNewTime;
	FTimeInfo->FTime = aNewTime;

	FTimeInfo->FTimeHW =NSHARE:: get_unix_time();
	if (FTimeInfo->FTime >= FTimeInfo->FNextTimer)
	{
		FTimeInfo->FNextTimer =
				std::numeric_limits<CRTCForModeling::rtc_time_t>::max();
		MUnlockWaiter();
	}
}
inline void CRTCForModeling::MUnlockWaiter() const
{
	FTimeInfo->FNumOfUnlocked = FTimeInfo->FNumOfWait;
	unsigned _post = FTimeInfo->FNumOfUnlocked;
	for (unsigned i = 0; i < _post; ++i)
		FTimeUpdated.MPost();
}
inline CRTCForModeling::rtc_time_t CRTCForModeling::MNextTime(
		rtc_time_t aNewTime) const
{
	using namespace std;
	DCHECK(MIsCreated());

	NSHARE::CRAII < NSHARE::CMutex > _leave_lock(FLeaveMutex);
	if(!MIsJoinToRTC())
	{
		LOG(DFATAL)<<"Not joined to RTC";

		return false;
	}
	rtc_time_t _rval = 0;
	bool _done = false;
	for (; !_done;)
	{
		if (FTimeInfo->FNumOfUnlocked == 0)
		{
            NSHARE::CRAII < NSHARE::CIPCMutex > _lock(FSem);
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
			FSem.MUnlock();

			rtc_time_t _time= 0;
			do
			{
				_time = FTimeInfo->FTime;

				for(HANG_INIT;FTimeInfo->FNumOfUnlocked!=0;
						NSHARE::CThread::sMYield(),HANG_CHECK)
					;//!<Wait for the other thread unlocked

				FTimeUpdated.MWait();

				DCHECK_GT(FTimeInfo->FNumOfUnlocked, 0);
				--FTimeInfo->FNumOfUnlocked;

			} while (_time == FTimeInfo->FTime && MIsJoinToRTC());

			DCHECK_GT(FTimeInfo->FNumOfWait, 0);

			--FTimeInfo->FNumOfWait;
#ifndef NDEBUG
			if(MIsJoinToRTC())/*!< @Warning The  FSem is locked by #MJoinToRTC method
			 	 	 	 	 	 And it wait for we unlock #FLeaveMutex
			 	 	 	 	 	 Thus all operation thread safety*/
				FSem.MLock();

			DCHECK_LE(FTimeInfo->FTime, aNewTime);
			DCHECK_GT(FTimeInfo->FNumOfWorking, FTimeInfo->FNumOfWait)
			    <<"time= "<<_time<<" current="<< FTimeInfo->FTime<<" Join:"
			    <<MIsJoinToRTC();
#endif
			_rval = FTimeInfo->FTime;
			_done = true;
		}
		else
		{
			_done = !NSHARE::CThread::sMYield();
			DLOG_IF(FATAL, _done) << "Yield operation is not supported";
		}
	}

	return _rval;
}
inline bool CRTCForModeling::MForceStopDispatcher()
{
	if (!FIsDone)
	{
		FIsDone = true;
		VLOG(2) << "Wait for dispatcher stopped";
		FHasToBeUpdated.MSignal();
		NSHARE::CRAII<NSHARE::CMutex> _destroy_lock(FDestroyMutex);
	}
	return true;
}
inline bool CRTCForModeling::MDispatcher()
{
	FIsDone = false;
	NSHARE::CRAII < NSHARE::CMutex > _destroy_lock(FDestroyMutex);
	CRTCForModeling::rtc_time_t _next_time = 0;

	NSHARE::CRAII <NSHARE:: CIPCMutex > _lock(FSem);
	if (!FIsDone)
		do
		{
			NSHARE::atomic_t _step;
			do
			{
				_step = FTimeInfo->FTimeStep;
				FHasToBeUpdated.MTimedwait(&FSem);
			} while (_step == FTimeInfo->FTimeStep && !FIsDone);

			DCHECK_EQ(FTimeInfo->FNumOfUnlocked, 0);

			MUpdateTime(FTimeInfo->FNextTimer);

			//FTimeInfo->FNumOfWait=0;
		} while (!FIsDone //
		&& FTimeInfo //
				&& FTimeInfo->FTime != time_info_t::END_OF_TIME);

	FIsDone=true;
	LOG(INFO) << "Time is ended for dispatcher " << FName;
	return true;
}
inline bool CRTCForModeling::MResetRTC() const
{
	if (MIsCreated()
			&& (FTimeInfo->FTime == time_info_t::END_OF_TIME
					|| FTimeInfo->FNumOfWorking == 0))
	{
		LOG(INFO) << "Restart dispatcher " << FName;
		FTimeInfo->FTime = 0;
		FTimeInfo->FTimeStep = 0;
		FTimeInfo->FNextTimer =
				std::numeric_limits<CRTCForModeling::rtc_time_t>::max();
		FTimeInfo->FTimeHW=0;
	}
	return true;
}
inline CRTCForModeling::rtc_time_t CRTCForModeling::MGetCurrentTime() const
{
	if (MIsCreated())
	{
		NSHARE::CRAII < NSHARE::CIPCMutex > _lock(FSem);
		return FTimeInfo->FTime;
	}
	else
		return 0;
}
inline NSHARE::CProgramName const& CRTCForModeling::MGetName() const
{
	return FName;
}
inline NSHARE::IAllocater::offset_pointer_t CRTCForModeling::MGetOffset() const
{
	return FAllocator->MOffset(FTimeInfo);
}
}

#endif /* CRTCFORMODELING_H_ */
