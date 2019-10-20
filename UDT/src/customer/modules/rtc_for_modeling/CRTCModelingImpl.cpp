// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CRTCForModeling.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <CRTCModelingImpl.h>
#include <udt_rtc_types.h>

namespace NUDT
{
using namespace NSHARE;
/*const CRTCModelingImpl::time_in_second_t CRTCModelingImpl::TO_DIPATCHER_TIME = 1000.0
		* 1000.0
		/ time_info_t::E_TIME_PRECISION;*/
const CRTCModelingImpl::millisecond_t CRTCModelingImpl::DIPATCHER_TIME_TO_TIME_MS =
		time_info_t::E_TIME_PRECISION;

const CRTCModelingImpl::time_in_second_t CRTCModelingImpl::PRECISION_IN_SECOND =
		time_info_t::E_TIME_PRECISION / 1000.0 / 1000.0;
const CRTCModelingImpl::millisecond_t CRTCModelingImpl::PRECISION_IN_MILLISECOND =
		time_info_t::E_TIME_PRECISION;

/*
const CRTCModelingImpl::time_in_second_t CRTCModelingImpl::MAX_TIME =
		((time_in_second_t) std::numeric_limits<time_info_t::rtc_time_t>::max())/TO_DIPATCHER_TIME;
*/

const CRTCModelingImpl::millisecond_t CRTCModelingImpl::MAX_TIME_MS =
		((millisecond_t) std::numeric_limits<time_info_t::rtc_time_t>::max())
				* DIPATCHER_TIME_TO_TIME_MS;



CRTCModelingImpl::CRTCModelingImpl(rtc_info_t const& aInfo,
		NSHARE::CSharedMemory const& aMemory):
		FInfo(aInfo),//
		FMemory(aMemory),//
		FEventId(0),//
		FMutex(NSHARE::CMutex::MUTEX_RECURSIVE),//
		FLastTime(0)
{
	IAllocater::offset_pointer_t const _offset=aInfo.FOffset;

	void* _p_info=aMemory.MGetIfMalloced(_offset);

	if(_p_info!=NULL)
	{
		FDispatcher.MOpen(FMemory.MGetAllocator(),aInfo.FName,static_cast<time_info_t*>(_p_info));

		LOG_IF(DFATAL,!FDispatcher.MIsCreated())<<"Cannot open dispatcher ";
	}
	else
		LOG(DFATAL)<<"Cannot open memory into "<<_offset;
}

CRTCModelingImpl::~CRTCModelingImpl()
{
}
bool CRTCModelingImpl::MIsInitialized() const
{
	return FDispatcher.MIsCreated();
}
bool CRTCModelingImpl::MJoinToRTC()
{
	DCHECK(MIsInitialized());
	bool _is = FDispatcher.MJoinToRTC();
	if (_is)
	{
		FLastTime=FDispatcher.MGetCurrentTime();
	}
	return _is;
}
bool CRTCModelingImpl::MIsJoinToRTC() const
{
	DCHECK(MIsInitialized());
	return FDispatcher.MIsJoinToRTC();
}
bool CRTCModelingImpl::MLeaveFromRTC()
{
	DCHECK(MIsInitialized());
	bool _is=FDispatcher.MLeaveFromRTC();
	if(_is)
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
		FEventList.clear();
	}
	return _is;
}
CRTCModelingImpl::time_in_second_t CRTCModelingImpl::MGetCurrentTime() const
{
	DCHECK(MIsInitialized());
	return MConvert(MGetCurrentTimeMs());
}
CRTCModelingImpl::millisecond_t CRTCModelingImpl::MGetCurrentTimeMs() const
{
	DCHECK(MIsInitialized());

	return FDispatcher.MGetCurrentTime()*DIPATCHER_TIME_TO_TIME_MS;
}
CRTCModelingImpl::time_in_second_t CRTCModelingImpl::MSleepUntil(time_in_second_t aTime) const
{
	return MConvert(MSleepUntil(MConvert(aTime)));
}
CRTCModelingImpl::millisecond_t CRTCModelingImpl::MSleepUntil(millisecond_t aTime) const
{
	DCHECK(MIsInitialized());
	CHECK(false);
/*	aTime*=1000*1000/time_info_t::E_TIME_PRECISION;

	return FDispatcher.MNextTime(static_cast<CRTCForModeling::rtc_time_t>(aTime));*/
	return -1;
}
/** Gets next time to waking up
 *
 * @param aTime time of wake up
 * @return next time to waking up
 */
CRTCForModeling::rtc_time_t CRTCModelingImpl::MGetNextTime(millisecond_t aTime) const
{
	using namespace std;

	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
	if (FEventList.empty() && aTime ==0)
	{
		VLOG(1) << "Nobody expect time";
		return  time_info_t::END_OF_TIME;
	}

	time_info_t::rtc_time_t  const _next_time_with =
			aTime == 0 || aTime >= MAX_TIME_MS ?
						time_info_t::END_OF_TIME :
						static_cast<CRTCForModeling::rtc_time_t>(aTime
								/ DIPATCHER_TIME_TO_TIME_MS);

	time_info_t::rtc_time_t const _next_time_event =
			FEventList.empty() || FEventList.front().FNextCall >= MAX_TIME_MS ?
					time_info_t::END_OF_TIME :
					static_cast<CRTCForModeling::rtc_time_t>(FEventList.front().FNextCall
							/DIPATCHER_TIME_TO_TIME_MS);

	return min(_next_time_with,	_next_time_event);
}
/** Get list of upcomming events
 *
 * @param aCall store result to
 * @param aTime time
 */
void CRTCModelingImpl::MGetUpcommingEventsList(millisecond_t aTime,event_list_t* aCall)
{
	VLOG(2)<<"Get list of upcomming events for time  "<<aTime;
	event_list_t& _call(*aCall);
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);

	event_list_t::iterator _it = FEventList.begin();
	for (; _it != FEventList.end() //
			&& _it->FNextCall < aTime;)
	{
		VLOG(3)<<"Put event for time  "<<_it->FNextCall;
		_call.splice(_call.end(), FEventList, _it++);//only post increment
	}
}
/** Call event
 *
 * @param aInfo What is called
 * @param [out] aNextCall pointer to next call time
 * @return
 */
int CRTCModelingImpl::MCallEvent(const timer_info_t& aInfo,
		millisecond_t* aNextCall)
{
	VLOG(1)<<"Call event "<<aInfo.FNextCall;
	if (aInfo.FType == timer_info_t::e_in_millisecond)
	{
		millisecond_t _next_time_ev = aInfo.FNextCall;
		const int _rval = aInfo.FWhatCall(this, &_next_time_ev);
		*aNextCall=_next_time_ev;
		return _rval;
	}else
	{
		time_in_second_t _time=MConvert(aInfo.FNextCall);
		const int _rval = aInfo.FWhatCall(this, &_time);
		*aNextCall=MConvert(_time);
		return _rval;
	}
}

/** Call events
 *
 * @param aTime Current time
 * @param aCall What is it called
 * @param aNewEventList A new events
 */
void CRTCModelingImpl::MCallEvents(millisecond_t aTime, event_list_t const& aCall,
		event_list_t* aNewEventList)
{
	VLOG_IF(1,aCall.empty())<<"No events for call in "<<aTime<<" seconds.";

	event_list_t& _new_event_list(*aNewEventList);

	event_list_t::const_iterator _it = aCall.begin();
	for (; _it != aCall.end(); ++_it)
	{
		timer_info_t _info = *_it;

		millisecond_t _next_time_ev = 0;
		const int _rval = MCallEvent(_info, &_next_time_ev);

		switch (_rval)
		{
		case NSHARE::E_CB_REMOVE:
		{
			VLOG(1) << "Remove callback ";
			break;
		}
		case NSHARE::E_CB_SAFE_IT:
			{
			VLOG(1) << "Warning!!! callback isn't removed ";
			bool _is = false;
			if ((_next_time_ev - aTime) > PRECISION_IN_MILLISECOND)
			{
				_info.FNextCall = _next_time_ev;

				VLOG(1) << "Next time by argument "<<_info.FNextCall;
				_new_event_list.push_back(_info);
			}
			else
			if (_info.FIntervalCall > 0)
			{
				_info.FNextCall += _info.FIntervalCall;

				VLOG(1) << "Next time by interval "<<_info.FNextCall;
				_new_event_list.push_back(_info);
			}
			else
			{
				LOG(DFATAL)
										<< "The interval time less 0 and argument is not"
												"changed but function is return 0 - repeat timer. "
												"Change return value of function to -1";
			}

			break;
		}
		case NSHARE::E_CB_BLOCING_OTHER:
			{
			LOG(DFATAL) << "Blocking other is ignored";
			break;
		}
		default:
			{
			LOG(DFATAL) << "unknown return value " << _rval;
			break;
		}
		}
	}
}

/** Call all event take into account time precision
 *
 * @param aTime current time
 */
void CRTCModelingImpl::MHandleTimers(time_info_t::rtc_time_t const& aTime)
{
	millisecond_t const _time_d = aTime *DIPATCHER_TIME_TO_TIME_MS;

	millisecond_t const _next_precision_time = _time_d
			+ PRECISION_IN_MILLISECOND;
	VLOG(2)<<"Time  take into account precision is "<<_next_precision_time;

	event_list_t _new_event_list;
	event_list_t _call;

	MGetUpcommingEventsList(_next_precision_time,&_call);
	MCallEvents(_time_d,_call,  &_new_event_list);

	{
		event_list_t::const_iterator _it = _new_event_list.begin();
		for (; _it != _new_event_list.end(); ++_it)
		{
			MPutTimer(*_it);
		}
	}
}
CRTCModelingImpl::time_in_second_t CRTCModelingImpl::MNextTime(
		time_in_second_t aTime)
{
	return MConvert(MNextTime(MConvert(aTime)));
}

CRTCModelingImpl::millisecond_t CRTCModelingImpl::MNextTime(
		millisecond_t aTime)
{
	if(!MIsJoinToRTC())
		return 0;
	time_info_t::rtc_time_t const _next_time=MGetNextTime(aTime);
	VLOG(2)<<"Next time is "<<_next_time;
	time_info_t::rtc_time_t const _time = FDispatcher.MNextTime(
			_next_time);
	VLOG(2)<<"Current time is "<<_time;

	if(!MIsJoinToRTC())
		return 0;

	MHandleTimers(_time);

	FLastTime=_time;
	return _time*DIPATCHER_TIME_TO_TIME_MS;
}

/** Put timer
 *
 * @param aInfo info about
 */
void CRTCModelingImpl::MPutTimer(const timer_info_t& aInfo)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
	if (aInfo.FNextCall < MAX_TIME_MS)
	{
		event_list_t::iterator _it = FEventList.begin();
		for (; _it != FEventList.end() //
		&& _it->FNextCall <= aInfo.FNextCall; ++_it)
			;
		VLOG_IF(2,_it != FEventList.end()) << "Add event before "
													<< _it->FNextCall
													<< " Time:"
													<< aInfo.FNextCall;
		VLOG_IF(2,_it == FEventList.end()) << "Add event Time:"
															<< aInfo.FNextCall;
		FEventList.insert(_it, aInfo);
	}
	else
	{
		VLOG(1)<<"Add event to the end of time";
		FEventList.push_back(aInfo);
		FEventList.back().FNextCall = MAX_TIME_MS;
	}
}

bool CRTCModelingImpl::MSetTimer(time_in_second_t aFirstCall, time_in_second_t aIntervalCall,
		callback_rtc_t const& aHandler)
{
	return MSetTimerImpl(MConvert(aFirstCall), MConvert(aIntervalCall),
			aHandler, timer_info_t::e_in_second);
}
bool CRTCModelingImpl::MSetTimer(millisecond_t aFirstCall, millisecond_t aIntervalCall,
		callback_rtc_t const& aHandler)
{
	return MSetTimerImpl(aFirstCall, aIntervalCall, aHandler,
			timer_info_t::e_in_millisecond);
}
bool CRTCModelingImpl::MSetTimerImpl(millisecond_t aFirstCall, millisecond_t aIntervalCall,
		callback_rtc_t const& aHandler, timer_info_t::eTimeType aType)
{
	DCHECK(MIsInitialized());

	if(!MIsInitialized())
	{
		LOG(DFATAL)<<"RTC isn't initialized";
		return false;
	}

	if (aIntervalCall != 0 && (aIntervalCall < MGetPrecisionMs()))
	{
		LOG(DFATAL)<<"Interval time less than "<<MGetPrecisionMs();
		return false;
	}

	if (FLastTime*DIPATCHER_TIME_TO_TIME_MS >= aFirstCall)
	{
		LOG(DFATAL)<<"Timer time is past "<<FLastTime;
		return false;
	}

	timer_info_t _info;
	_info.FType=aType;
	_info.FNextCall=aFirstCall;
	_info.FIntervalCall =aIntervalCall;

	_info.FWhatCall=aHandler;
	_info.FId=++FEventId;

	MPutTimer(_info);
	return true;
}
rtc_info_t CRTCModelingImpl::MGetRTCInfo() const
{
	DCHECK(MIsInitialized());
	return FInfo;
}
CRTCModelingImpl::time_in_second_t CRTCModelingImpl::MGetPrecision() const
{
	return PRECISION_IN_SECOND;
}
CRTCModelingImpl::millisecond_t CRTCModelingImpl::MGetPrecisionMs() const
{
	return PRECISION_IN_MILLISECOND;
}
int CRTCModelingImpl::MGetAmountOfJoined() const
{
	DCHECK(MIsInitialized());
	return FDispatcher.MGetAmountOfJoined();
}
/** Convert from millisecond to second
 *
 * @param aWhat time in millisecond
 * @return time in second
 */
CRTCModelingImpl::time_in_second_t CRTCModelingImpl::MConvert(millisecond_t const& aWhat) const
{
	return static_cast<CRTCModelingImpl::time_in_second_t>(aWhat) / 1000.0
			/ 1000.0;
}
/** Convert from second to millisecond
 *
 * @param aWhat time in second
 * @return time in millisecond
 */
CRTCModelingImpl::millisecond_t CRTCModelingImpl::MConvert(time_in_second_t const& aWhat) const
{
	return aWhat<=0?0:
			static_cast<CRTCModelingImpl::millisecond_t>(aWhat*1000.0*1000.0);
}

} /* namespace NUDT */
