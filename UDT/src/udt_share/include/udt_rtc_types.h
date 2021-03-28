/*
 * udt_rtc_types.h
 *
 *  Created on: 05.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_RTC_TYPES_H_
#define UDT_RTC_TYPES_H_

#include <UDT/udt_share_macros.h>
#include <UDT/udt_rtc.h>
namespace NUDT
{
/** Info about current time
 *
 */
struct UDT_SHARE_EXPORT time_info_t
{
	enum
	{
		E_TIME_PRECISION = 1, //!< the minimal rtc step in microseconds
	};

	typedef uint64_t rtc_time_t; //!< Used to hold time (precision is #E_TIME_PRECISION)
	static const uint64_t END_OF_TIME; //!< End of time value
	static const NSHARE::CText NAME;	///< A serialization key

	static const NSHARE::CText KEY_NUM_OF_WORKING;///< A serialization key FNumOfWorking
	static const NSHARE::CText KEY_NUM_OF_WAIT;	///< A serialization key FNumOfWait
	static const NSHARE::CText KEY_NUM_OF_LOCKED;///< A serialization key FNumOfLocked

	static const NSHARE::CText KEY_NUM_OF_UNLOCKED;	///< A serialization key FNumOfUnlocked
	static const NSHARE::CText KEY_TIME_STEP;///< A serialization key FTimeStep

	static const NSHARE::CText KEY_TIME;	///< A serialization key FTime
	static const NSHARE::CText KEY_TIME_HW;	///< A serialization key FTimeHW
	static const NSHARE::CText KEY_TIME_NEXT;///< A serialization key FNextTimer

	static const NSHARE::CText KEY_OWNER;	///< A serialization key FOwner
	static const NSHARE::CText KEY_ID;///< A serialization key FId

	time_info_t();
	/** Serialize data
	 *
	 * @return
	 */
	NSHARE::CConfig MSerialize() const;

	//rtc_info_t FRtc; //!< Info about used RTC
	NSHARE::atomic_t FNumOfWorking; //!< The number of thread which is working
	NSHARE::atomic_t FNumOfWait; //!< The number of thread wait for time updated
	NSHARE::atomic_t FNumOfLocked; //!< The number of locked thread
	NSHARE::atomic_t FNumOfUnlocked; //!< The number of thread which has to be unlocked
	NSHARE::atomic_t FTimeStep; //!< Current step of time
	rtc_time_t FTime; //!< current time in milliseconds
	rtc_time_t FTimeHW; //!< real time (hardware time) of hold #time value
	rtc_time_t FNextTimer; //!< when the next timer will to expire
	uint64_t 	FOwner;//!< Owner ID
	rtc_info_t::unique_id_t FId;//!< Owner unique ID
	uint8_t FEventTimeChanged[NSHARE::CIPCSignalEvent::eReguredBufSize]; //!< used to wait for time changed
	uint8_t FMutex[NSHARE::CIPCMutex::eReguredBufSize]; //!< used to lock this object
	uint8_t FHasToBeUpdated[NSHARE::CIPCSignalEvent::eReguredBufSize]; //!< used to wait for need to update time
};
inline time_info_t::time_info_t()
{
	FNextTimer = END_OF_TIME;
	FNumOfWorking = 0;
	FNumOfUnlocked = 0;
	FTimeStep = 0;
	FNumOfWait = 0;
	FNumOfLocked = 0;
	FTime = 0;
	FTimeHW = 0;
	FOwner = 0;
	FId = 0;
	memset(FEventTimeChanged, 0,sizeof(FEventTimeChanged));
	memset(FMutex, 0,sizeof(FMutex));
	memset(FHasToBeUpdated, 0,sizeof(FHasToBeUpdated));
}
}
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		NUDT::time_info_t const& aInfo)
{
	using namespace NUDT;
	aStream << " Cur time=" << aInfo.FTime
			<< " Next time=" << aInfo.FNextTimer
			<< " unlocked=" << aInfo.FNumOfUnlocked
			<< " locked=" << aInfo.FNumOfLocked
			<< " wait=" << aInfo.FNumOfWait
			<< " rtc users=" << aInfo.FNumOfWorking
			<< " step=" << aInfo.FTimeStep
			<< " HW time=" << aInfo.FTimeHW
			<< " owner=" << aInfo.FOwner
			<< " id=" << aInfo.FId
			/* <<aInfo.FRtc*/;
	return aStream;
}
}
#endif /* UDT_RTC_TYPES_H_ */
