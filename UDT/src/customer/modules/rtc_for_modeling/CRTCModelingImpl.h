/*
 * CRTCModelingImpl.h
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRTCMODELINGIMPL_H_
#define CRTCMODELINGIMPL_H_

#include <share/UType/CSharedMemory.h>
#include <udt/IRtc.h>
#include <CRTCForModeling.h>
#include <udt/CCustomer.h>

namespace NUDT
{
/** Realization API for time dispatcher
 * for  modeling
 *
 */
class CRTCModelingImpl: public IRtc
{
public:
	/** Initialize from exist dispatcher
	 *
	 * @param aInfo An info about dispatcher
	 * @param aMemory pointer to memory
	 */
	CRTCModelingImpl(rtc_info_t const& aInfo,
			NSHARE::CSharedMemory const& aMemory);
	virtual ~CRTCModelingImpl();

	/** Check for initialization
	 *
	 * @return true if initialized
	 */
	bool MIsInitialized() const;

	/** @copydoc IRtc::MJoinToRTC()
	 */
	bool MJoinToRTC();

	/** @copydoc IRtc::MIsJoinToRTC()
	*/
	bool MIsJoinToRTC() const;

	/** @copydoc IRtc::MLeaveFromRTC()
	*/
	bool MLeaveFromRTC();

	/** @copydoc IRtc::MGetCurrentTime()
	*/
	time_in_second_t MGetCurrentTime() const;

	/** @copydoc IRtc::MGetCurrentTimeMs()
	*/
	millisecond_t MGetCurrentTimeMs() const;

	/** @copydoc IRtc::MSleepUntil()
	*/
	time_in_second_t MSleepUntil(time_in_second_t aTime) const;

	/** @copydoc IRtc::MSleepUntil()
	*/
	millisecond_t MSleepUntil(millisecond_t aTime) const;

	/** @copydoc IRtc::MSetTimer()
	*/
	bool MSetTimer(time_in_second_t aFirstCall, time_in_second_t aIntervalCall,
			callback_rtc_t const& aHandler);

	/** @copydoc IRtc::MSetTimer()
	*/
	bool MSetTimer(millisecond_t aFirstCall, millisecond_t aIntervalCall,
			callback_rtc_t const& aHandler);

	/** @copydoc IRtc::MGetRTCInfo()
	 */
	rtc_info_t MGetRTCInfo() const;

	/** @copydoc IRtc::MNextTime()
	 */
	time_in_second_t MNextTime(time_in_second_t aTime);

	/** @copydoc IRtc::MNextTime()
	 */
	millisecond_t MNextTime(millisecond_t aTime=0);

	/** @copydoc IRtc::MGetPrecision()
	 */
	time_in_second_t MGetPrecision() const;

	/** @copydoc IRtc::MGetPrecisionMs()
	 */
	millisecond_t MGetPrecisionMs() const;

	/** @copydoc IRtc::MGetAmountOfJoined()
	*/
	int MGetAmountOfJoined() const;
private:
	//static const time_in_second_t MAX_TIME; //!< maximal time in seconds
	static const millisecond_t MAX_TIME_MS; //!< maximal time in milliseconds

	//static const time_in_second_t TO_DIPATCHER_TIME; //!< second to dispatcher time
	static const millisecond_t DIPATCHER_TIME_TO_TIME_MS; //!< millisecond to dispatcher time

	static const time_in_second_t PRECISION_IN_SECOND; //!< time precision in seconds
	static const millisecond_t PRECISION_IN_MILLISECOND; //!< time precision in milliseconds

	/** Timer info
	 *
	 */
	struct timer_info_t
	{
		/** Used unit of measure
		 *
		 */
		enum eTimeType
		{
			e_in_second=1,   //!< second
			e_in_millisecond,//!< millisecond
		};
		eTimeType FType;//!< unit of measure type
		unsigned FId;//!< unique event ID
		millisecond_t FNextCall;//!< Time of next call
		millisecond_t FIntervalCall;//!< periodicity of calling
		callback_rtc_t FWhatCall;//!< calling function
	};
	typedef std::list<timer_info_t> event_list_t; //!< sorted event list

	time_in_second_t MConvert(millisecond_t const& aWhat) const;
	millisecond_t MConvert(time_in_second_t const& aWhat) const;

	CRTCForModeling::rtc_time_t MGetNextTime(millisecond_t aTime) const;

	void MHandleTimers(time_info_t::rtc_time_t  const& aTime);
	void MGetUpcommingEventsList(millisecond_t aTime,event_list_t* _call);
	void MCallEvents(millisecond_t aTime,event_list_t const& _call,
			event_list_t* aNewEventList);
	void MPutTimer(const timer_info_t& _info);
	bool MSetTimerImpl(millisecond_t aFirstCall, millisecond_t aIntervalCall,
			callback_rtc_t const& aHandler, timer_info_t::eTimeType aType);
	int MCallEvent(const timer_info_t& aInfo,
			IRtc::millisecond_t* aNextCall);

	CRTCForModeling FDispatcher;
	rtc_info_t const FInfo;
	NSHARE::CSharedMemory const& FMemory;
	event_list_t FEventList;
	mutable NSHARE::CMutex FMutex;//!<used for lock timer
	unsigned FEventId;//!< Last event if
	CRTCForModeling::rtc_time_t FLastTime;//!<Last time
};

} /* namespace NUDT */

#endif /* CRTCMODELINGIMPL_H_ */
