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

    /**
     * @brief It's used for force leave from rtc dispatcher
     */
    enum eWaitState
    {
        eStateNoWait,
        eStateWaitNextTime/*,
        eStateWaitReadTime,*/
    };
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
//	bool MHasShouldContinue(rtc_time_t const& aTime) const;
//    bool MIsWaitingOther(rtc_time_t const& aTime) const;
	bool MCheckForTimeHasCome() const;

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
	mutable unsigned FMyStepNumber;//!< Number of step which i did
    mutable NSHARE::atomic_t FIsWaitForNextStepState;//!< state of wait for next step
};


inline CRTCForModeling::CRTCForModeling() :
		FAllocator(NULL), //
		FTimeInfo(NULL), //
		FIsDone(true),//
		FIsJoin(false),//
		FIsICreateInfo(false),//
        FMyStepNumber(0)
{
    FIsWaitForNextStepState=eStateNoWait;
}
inline CRTCForModeling::CRTCForModeling(NSHARE::IAllocater* aAllocator, //
		NSHARE::CProgramName const& aName) :
		FAllocator(NULL), //
		FTimeInfo(NULL), //
		FIsDone(true),//
		FIsJoin(false),//
		FIsICreateInfo(false),//
        FMyStepNumber(0)
{
    FIsWaitForNextStepState=eStateNoWait;
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
				FIsICreateInfo(false),//
                FMyStepNumber(0)
{
    FIsWaitForNextStepState=eStateNoWait;
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

    if (!MIsJoinToRTC())
        return false;

    bool _is_lock=FLeaveMutex.MCanLock();
    FSem.MLock();
    FIsJoin = false;
    eWaitState const _was_wait_state=(eWaitState)FIsWaitForNextStepState.MValue();

    if(!_is_lock)
    {
        ///< Into #MNextTime Method

        ///< Try Again as on IPC Mutex
        _is_lock=FLeaveMutex.MCanLock();
        if(!_is_lock)
        {
            for(;FTimeInfo->FNumOfUnlocked!=0;)
            {
                FSem.MUnlock();
                NSHARE::CThread::sMYield();
                FSem.MLock();
            }
            _is_lock=FLeaveMutex.MCanLock();
            if(!_is_lock)
            {

                ///< It's not locked, No problem can not lock
                for (; FIsWaitForNextStepState==eStateWaitNextTime;)
                {
                    ///< Try force unlock
                    CHECK_EQ(FTimeInfo->FNumOfUnlocked,0);
                    ++FTimeInfo->FNumOfUnlocked;
                    FTimeUpdated.MPost();
                    for (; FTimeInfo->FNumOfUnlocked != 0//
                                && FIsWaitForNextStepState!=/*eStateWaitReadTime*/eStateNoWait//
                               && FTimeInfo->FNumOfLocked!=0;
                               )
                    {
                        CHECK(FTimeInfo->FNumOfUnlocked==1 //
                                  ||FTimeInfo->FNumOfUnlocked==0)<<FTimeInfo->FNumOfUnlocked;
                        NSHARE::CThread::sMYield();
                    }
                    if(FTimeInfo->FNumOfUnlocked == 1
                        && FTimeInfo->FNumOfLocked==0
                     )
                    {
                        ///< No any wait thread
                        FTimeUpdated.MWait();
                        FTimeInfo->FNumOfUnlocked=0;
                    }
                }

            }
        }
    }
    DVLOG(1) << "Leaving from RTC: " << FName << ", Num of working"
                 << FTimeInfo->FNumOfWorking;

    --FTimeInfo->FNumOfWorking;
    switch(_was_wait_state)
    {
        case eStateNoWait:break;
        case eStateWaitNextTime:
        {
            /** Next mutex can lock dispatcher thus we should revert all change
             * as it doesn't know about we force unlock one of dipsacther user
             */
            if(FIsWaitForNextStepState==eStateNoWait/*eStateWaitReadTime*/)
            {
                //FIsWaitForNextStepState=eStateNoWait;
                ///< Revert change
                if(FTimeInfo-> FNumOfWait!=0)
                    --FTimeInfo->FNumOfWait;
            }

            break;
        }
/*        case eStateWaitReadTime:
        {
            *//** Next mutex can lock dispatcher thus we should decrease the number of
             * locked thread force as we change the number of working
             *//*
             CHECK_EQ(FIsWaitForNextStepState,eStateWaitReadTime);

            break;
        }*/

    }

    if (FTimeInfo->FNumOfWorking == 0)
        MResetRTC();
    else
    {
        MCheckForTimeHasCome();
    }



    FMyStepNumber=0;
    FSem.MUnlock();

    if(!_is_lock)
        ///< Wait end of working #MNextTime
        FLeaveMutex.MLock();
    FLeaveMutex.MUnlock();
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

	NSHARE::CRAII < NSHARE::CMutex > _leave_lock(FLeaveMutex);
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
    DCHECK_EQ(FTimeInfo->FNumOfUnlocked,0);

    DCHECK_EQ(FTimeInfo->FNumOfUnlocked,0);
	FTimeInfo->FNumOfUnlocked = FTimeInfo->FNumOfLocked;
	unsigned _post = FTimeInfo->FNumOfUnlocked;
	for (unsigned i = 0; i < _post; ++i)
		FTimeUpdated.MPost();
}
/*inline bool CRTCForModeling::MIsWaitingOther(rtc_time_t const& aTime) const
{
    if(aTime == FTimeInfo->FTime && MIsJoinToRTC())//!< Force compare to avoid dead lock then it's forced unlocked
    {
        NSHARE::CRAII<NSHARE::CIPCMutex> _lock(FSem);

        if( aTime == FTimeInfo->FTime//
            &&FTimeInfo->FNumOfUnlocked!=0//
            && MIsJoinToRTC()//
            )
        {
            CHECK_NE(FTimeInfo->FNextTimer,
                std::numeric_limits<CRTCForModeling::rtc_time_t>::max())
                <<"Time ="<<aTime
                <<*FTimeInfo
                <<"My step="<<FMyStepNumber;
            return true;
        };
    }
    return false;
};
*//** It has to continue wait
 *
 * @param aTime Wait time
 * @return
 *//*
inline bool CRTCForModeling::MHasShouldContinue(rtc_time_t const& aTime) const
{

	///< Wait for the other thread unlocked
	bool _is;
	for(HANG_INIT; (_is=MIsWaitingOther(aTime))//
			 ;
			NSHARE::CThread::sMYield(),HANG_CHECK)
;
	return _is;
}*/

inline bool CRTCForModeling::MCheckForTimeHasCome() const
{
	if (FTimeInfo->FNumOfWait == FTimeInfo->FNumOfWorking//
	 	                    && FTimeInfo->FNumOfWorking>0
	 	                    )
	{
		DVLOG(3) << "Current minimal value: " << FTimeInfo->FNextTimer;

		CHECK_GT(FTimeInfo->FNextTimer,FTimeInfo->FTime)<<"Step ="<<FTimeInfo->FTimeStep;


		FHasToBeUpdated.MSignal();
		++FTimeInfo->FTimeStep;
        FTimeInfo-> FNumOfWait=0;
		return true;
	}
	return false;
}

inline CRTCForModeling::rtc_time_t CRTCForModeling::MNextTime(
		rtc_time_t aNewTime) const
{
	using namespace std;
	DCHECK(MIsCreated());

	NSHARE::CRAII < NSHARE::CMutex > _leave_lock(FLeaveMutex);

    LOG_IF(DFATAL,!MIsJoinToRTC())<<"Not joined to RTC";

	rtc_time_t _rval = 0;
	bool _done = false;
	for (; !_done && MIsJoinToRTC();)
	{
		if (FTimeInfo->FNumOfUnlocked == 0 )
		{
            rtc_time_t _time= 0;
            NSHARE::atomic_t  _prev_step;
            NSHARE::atomic_t  _start_num;
            {
                NSHARE::CRAII<NSHARE::CIPCMutex> _lock(FSem);
                DCHECK_LE(FTimeInfo->FTime, aNewTime);
                DCHECK_GT(FTimeInfo->FNumOfWorking, 0u);

                DVLOG(3) << "New time is " << aNewTime;

                if(!MIsJoinToRTC())
                {
                	_rval = aNewTime;
                	break;
                }

                _prev_step=FTimeInfo->FTimeStep;
                _start_num=FTimeInfo->FNumOfWorking;

                FTimeInfo->FNextTimer = min(aNewTime, FTimeInfo->FNextTimer);
                FIsWaitForNextStepState=eStateWaitNextTime;
                ++FTimeInfo->FNumOfLocked;
                ++FTimeInfo->FNumOfWait;

				MCheckForTimeHasCome();

                DVLOG(3) << "Request: " << aNewTime << "  minimal time:"
                         << FTimeInfo->FNextTimer;
                _time = FTimeInfo->FTime;
            }

			do
			{
				FTimeUpdated.MWait();

                {
                    --FTimeInfo->FNumOfUnlocked;
                    CHECK_LT(FTimeInfo->FNumOfUnlocked, std::numeric_limits<unsigned short>::max());
                }
			} //while (MHasShouldContinue(_time));
            while (_time == FTimeInfo->FTime && MIsJoinToRTC());

			DCHECK_GT(FTimeInfo->FNumOfLocked, 0);

            --FTimeInfo->FNumOfLocked;
            //FIsWaitForNextStepState=eStateWaitReadTime;
            {
               // NSHARE::CRAII<NSHARE::CIPCMutex> _lock(FSem);//!< Can be removed
                if(MIsJoinToRTC())
                {
                    CHECK(_prev_step == (FTimeInfo->FTimeStep - 1))
                    << "Prev step=" << _prev_step << "Cut step=" << FTimeInfo->FTimeStep
                      << " before=" << _start_num
                      << "My step=" << FMyStepNumber << *FTimeInfo;

                    DCHECK_LE(FTimeInfo->FTime, aNewTime) << "My step=" << FMyStepNumber
                                                          << *FTimeInfo;
                    DCHECK(FTimeInfo->FNumOfWorking > FTimeInfo->FNumOfLocked)
                    << "time= " << _time << " current=" << FTimeInfo->FTime << " Join:"
                    << MIsJoinToRTC();

                    _rval = FTimeInfo->FTime;
                    _done = true;
                }
                else
                {
                    _rval = aNewTime;
                    _done = true;
                }
                FIsWaitForNextStepState=eStateNoWait;
            }

		}
		else
		{
			_done = !NSHARE::CThread::sMYield();
			DLOG_IF(FATAL, _done) << "Yield operation is not supported";
		}
	}
    ++FMyStepNumber;
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
            DCHECK_EQ(FTimeInfo->FNumOfLocked, FTimeInfo->FNumOfWorking);
            DCHECK_EQ(FTimeInfo->FNumOfWait,0);

			MUpdateTime(FTimeInfo->FNextTimer);



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
