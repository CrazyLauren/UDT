// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CRTCForModelingModule.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRTCFORMODELINGMODULE_H_
#define CRTCFORMODELINGMODULE_H_

#include <SHARE/UType/CSingleton.h>
#include <SHARE/UType/CSharedMemory.h>
#include <UDT/IRtcControl.h>
#include <rtc_for_modeling_export.h>
#include <CDataObject.h>
#include "CRTCModelingImpl.h"

namespace NUDT
{
/** RTC realized for modeling
 *
 */
class RTC_FOR_MODELING_EXPORT CRTCForModelingModule: public IRtcControl,
												NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;

	CRTCForModelingModule();

	/** Gets ID by name
	 *
	 * @param aName a name of RTC
	 * @return ID of RTC
	 */
	virtual rtc_id_t MGetNameByRTC(name_rtc_t const& aName) const;

	/** @copydoc IRtcControl::MGetAllRTC
	 *
	 */
	virtual array_of_RTC_t MGetAllRTC() const;

	/** Gets name by ID
	 *
	 * @param aName ID of RTC
	 * @return name of RTC
	 */
	virtual name_rtc_t MGetRTCByName(rtc_id_t const& aID) const;

	virtual IRtc* MGetRTC(name_rtc_t const& aID) const;

	virtual IRtc* MWaitForRTC(name_rtc_t const& aID,
			double aTime = -1) const;

	virtual IRtc* MGetOrCreateRTC(name_rtc_t const& aName);
	virtual IRtc* MCreateRTC(name_rtc_t const& aName);

	void MWaitForRTC(
			wait_for_t* aWaitFor
			) const;

	void MUnWait(
			name_rtc_t const& aWhat
				) const;
	bool MRemoveRTC(name_rtc_t const& aName);

	/** Initialize module
	 *
	 */
	void MInit(ICustomer *);

	/** Open module (can start thread)
	 *
	 */
	bool MOpen(const NSHARE::CThread::param_t* = NULL);

	/** Wait for closed
	 *
	 */
	void MJoin();

	/** Returns true if is opened
	 *
	 * @return true if opened
	 */
	bool MIsOpened() const;

	/** Close module
	 *
	 */
	void MClose();

	/** Checks for existing RTC
	 *
	 * @return true if exist
	 */
	bool MIsRTC() const;
private:
	static rtc_info_t::unique_id_t sFIdCounter;

	/** State of RTC
	 *
	 */
	struct rtc_state_t
	{
		array_of_RTC_t FRtc;//!< Pointer to RTC
	};
	typedef NSHARE::CSafeData<rtc_state_t> protected_RTC_array_t;//!< RW lock for  #rtc_state_t
	typedef std::map<name_rtc_t, NSHARE::intrusive_ptr<wait_for_t> > wait_for_store_t; //!< wait for store

	static int sMReceiveRTC(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	void MUpdateRTCInfo(real_time_clocks_t const&);
	bool MOpenSharedMemory();
	rtc_id_t MPushRTC(CRTCModelingImpl*);

	bool MCreateRTC(real_time_clocks_t const& aRtc);
	bool MUpdateRTC();
	void MUnlockWaitedRTC();


	protected_RTC_array_t FRTCArray;//!< List of available RTC
	CDataObject::value_t 	FHandler;//!< Handler for new RTC info
	real_time_clocks_t FRealTimeClocks;//!< Info about RTCs
	real_time_clocks_t FOwnTimeClocks;//!< Info about Own RTC
	mutable NSHARE::CMutex FCommonMutex;//!< Mutex for common purpose
	NSHARE::CSharedMemory FMemory;//!< is used for communication
	ICustomer* FPCustomer;//!< Pointer to customer
	mutable wait_for_store_t FWaitFor;//!< Store expected RTC by user

	mutable NSHARE::CCondvar FWaitForEvent;
};
}

#endif /* CRTCFORMODELINGMODULE_H_ */
