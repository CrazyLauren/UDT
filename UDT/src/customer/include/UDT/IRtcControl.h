/*
 * IRtcControl.h
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ICRTCONTROL_H_
#define ICRTCONTROL_H_

#include "IModule.h"
#include <udt_rtc_types.h>
namespace NUDT
{
class IRtc;
/** API for control by RTC
 *
 */
class CUSTOMER_EXPORT IRtcControl:public IModule
{
public:
	typedef std::vector<IRtc*> array_of_RTC_t;//!< The array of pointer to RTC
	typedef unsigned rtc_id_t;//!< unique id of RTC
	typedef  time_info_t::rtc_time_t rtc_time_t;


	/** Gets ID by name
	 *
	 * @param aName a name of RTC
	 * @return ID of RTC
	 */
	virtual rtc_id_t MGetNameByRTC(name_rtc_t const& aName) const =0;

	/** Gets name by ID
	 *
	 * @param aName ID of RTC
	 * @return name of RTC
	 */
	virtual name_rtc_t MGetRTCByName(rtc_id_t const& aID) const =0;

	/** Get RTC By specified ID
	 *
	 * @param aID RTC id
	 * @return Pointer to RTC
	 */
	virtual IRtc* MGetRTC(name_rtc_t const& aID) const =0;

	/** Gets or Create new RTC
	 *
	 * @param aName Id of RTC
	 * @param aType Type of RTC (for creating only, can be ignored by Kernel)
	 * @return pointer to RTC or NULL if Kernel is disabled
	 * @warning blocking call!!!
	 * @warning Auto call MJoinToRTC()
	 */
	virtual IRtc* MGetOrCreateRTC(name_rtc_t const& aName,
			eRTCType const& aType) =0;

	/** Get RTC By specified ID
	 *
	 * @param aID RTC id
	 * @param aTime
	 * @return Pointer to RTC
	 */
	virtual IRtc* MWaitForRTC(name_rtc_t const& aID,
			double aTime = -1) const =0;


	/** Gets all RTC
	 *
	 * @return array of RTC
	 */
	virtual array_of_RTC_t MGetAllRTC() const=0;

protected:
	IRtcControl(const NSHARE::CText& type) :
		IModule(type)
	{
	}
};
}

#endif /* ICRTCONTROL_H_ */
