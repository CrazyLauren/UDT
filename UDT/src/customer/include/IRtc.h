/*
 * IRtc.h
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ICRTC_H_
#define ICRTC_H_

#include <udt_rtc.h>
namespace NUDT
{
struct callback_t;
/** API for working with real time clock
 *
 * In practice, the time of system can differ from
 * "real" time. The kernel have inner module "RTC",
 * which is used for modeling purpose or when the
 * "RTC" is not  available in the hardware.
 * If the kernel "RTC" is turned off, calls of this
 * method is equal of calling the #SHARE::get_time()
 * function. Which in turn equals to call gettimeofday
 * of UNIX systems.
 * Thus, instead of using standard function
 * for gets the current system time (i.e. time(),
 * gettimeofday, clock_gettime (CLOCK_REALTIME))
 * it recommends to use interface.
 * This can save a lot of time in the future.
 *
 * For using the RTC You has to be connected to it by calling
 * #IRtc::MJoinToRTCWorker method.
 */
class IRtc:public NSHARE::IFactory
{
public:
	typedef NSHARE::CProgramName program_group_t; //!< Type of program group

	/** Registry in RTC controller
	 *
	 *
	 * @return true if noe error
	 */
	virtual bool MJoinToRTC() =0;

	/** UnRegistration in RTC controller
	 *
	 * * @return true if noe error
	 */
	virtual bool MLeaveFromRTC() =0;

	/** @brief Gets the current time
	 *
	 * @param aGroup A time of group (by default common time)
	 * @return current time in seconds
	 * 		   (precision - milliseconds)
	 */
	virtual double MGetCurrentTime(program_group_t const& aGroup =
			program_group_t()) const=0;

	/** @brief Suspend a thread until the specified
	 * time comes
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method. If aTime is less or equal  of zero
	 * then it waits for the time is changed.
	 * In modeling purpose it's equal to send
	 * to modeling controller the time
	 * of "next call".
	 * @param aTime A time (absolute)
	 * @param aGroup A time of group (by default common time)
	 * @return current time in seconds
	 */
	virtual double MSleepUntil(double aTime, program_group_t const& aGroup =
			program_group_t()) const =0;

	/** @brief Creates a timer
	 *
	 * The methods creates timer using the kernel "RTC"
	 * if is turn on or CLOCK_REALTIME if is turn off.
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method.
	 *
	 * @param aGroup A time of group (by default common time)
	 * @param aFirstCall A first expiry time (absolute)
	 * @param aIntervalCall A repetition interval or -1
	 * @param aHandler A pointer to the function for handling timer
	 */
	virtual void MSetTimer(double aFirstCall, double aIntervalCall,
			callback_t const& aHandler, program_group_t const& aGroup =
					program_group_t())=0;

	/** Returns info about used RTC
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method.
	 * @param aGroup A time of group (by default common time)
	 * @return Info about RTC
	 */
	virtual rtc_info_t MGetRTCInfo(program_group_t const& aGroup =
			program_group_t()) const=0;

protected:
	IRtc(const NSHARE::CText& type) :
		NSHARE::IFactory(type)
	{
	}
};
}

#endif /* ICRTC_H_ */
