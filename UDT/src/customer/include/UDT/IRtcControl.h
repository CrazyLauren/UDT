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
	typedef std::vector<NSHARE::intrusive_ptr<IRtc> > array_of_RTC_t;//!< The array of pointer to RTC
	typedef unsigned rtc_id_t;//!< unique id of RTC
	typedef  time_info_t::rtc_time_t rtc_time_t;
	struct wait_for_t;



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
	 */
	virtual IRtc* MGetOrCreateRTC(name_rtc_t const& aName) =0;

	/** Creates new RTC
	 *
	 * @param aName Id of RTC
	 * @param aType Type of RTC (for creating only, can be ignored by Kernel)
	 * @return pointer to RTC or NULL if Kernel is disabled
	 * @warning blocking call!!!
	 */
	virtual IRtc* MCreateRTC(name_rtc_t const& aName) =0;

	/** Remove RTC if You is owner
	 *
	 * @param aName RTC name
	 * @return true if remove
	 */
	virtual bool MRemoveRTC(name_rtc_t const& aName) =0;

	/** Wait for RTC created
	 *
	 * @param aWaitFor Struct for wait for
	 */
	virtual void MWaitForRTC(
			wait_for_t* aWaitFor
			) const =0;

	virtual void MUnWait(
				name_rtc_t const& aWhat
				) const =0;

	/** Gets all RTC
	 *
	 * @return array of RTC
	 */
	virtual array_of_RTC_t MGetAllRTC() const=0;

	inline eRTCType const& MGetRTCType() const
	{
		return FRtcType;
	}
protected:
	IRtcControl(const NSHARE::CText& aModuleName, eRTCType const& aType):
		IModule(aModuleName),
		FRtcType(aType)
	{
	}
	eRTCType const FRtcType;
};

/** Wait for RTC structure
 *
 */
struct IRtcControl::wait_for_t: NSHARE::IIntrusived
{
	NSHARE::CMutex FLockMutex;
	NSHARE::CCondvar FCondvar;

	IRtc* FRtc;//!< Out variable

	name_rtc_t const FID;
	wait_for_t(name_rtc_t const& aID):
		FLockMutex(NSHARE::CMutex::MUTEX_NORMAL),//
				FRtc(NULL),//
				FID(aID)
	{

	}
};
}

#endif /* ICRTCONTROL_H_ */
