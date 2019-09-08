/*
 * udt_rtc.h
 *
 *  Created on: 05.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_RTC_H_
#define UDT_RTC_H_

namespace NUDT
{
/** Type of real-time clock which is used in kernel
 *
 */
enum eRTCType
{
	eRTC_TURN_OFF, /*!<The RTC of kernel is turn off.
	 Direct call of the similar system function is used.
	 Thus this is the faster RTC as
	 does not have any additional cost.
	 */
	eRTC_TURN_ON, /*!<The RTC of kernel is turn on.
	 It's realization own time counter,
	 without using counter of the OS.
	 Usually it's used in embedded computer
	 which has the specified RTC counter but
	 the driver to OS cannot be written.
	 */
	eRTC_MODELLING, /*!<The kernel is modeling the RTC,
	 Or course, it's not RTC in the truest
	 sense of the word, but if the program
	 isn't communicating with real hardware
	 this mode is equal of RTC.
	 */
	eRTC_DEFAULT = eRTC_TURN_OFF
};

/** Info about RTC
 *
 */
struct rtc_info_t
{
	eRTCType rtc_type; //!< Type of RTC
};
}

#endif /* UDT_RTC_H_ */
