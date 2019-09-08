/*
 * CTimeDispatcher.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTIMEDISPATCHER_H_
#define CTIMEDISPATCHER_H_

#include <UType/CSingleton.h>
#include <UType/CSharedMemory.h>

#include <core/ICore.h>
#include <udt_rtc_types.h>
namespace NUDT
{
/** RTC realized for modeling
 *
 */
class CRTCForModeling: NSHARE::CDenyCopying
{
public:
	typedef  time_info_t::rtc_time_t rtc_time_t;

	/** Create the new RTC
	 *
	 * @param aAllocator  An allocator of shared memory
	 * @param aName  The unique name of RTC
	 */
	CRTCForModeling(NSHARE::IAllocater* aAllocator,
			NSHARE::CProgramName const& aName);

	~CRTCForModeling();


	/** Add a new worker which is used
	 * the RTC
	 *
	 */
	void MJoinToRTCWorker();

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
	void MUnRegistration();

	/** Perpetual loop of update
	 * time until finish
	 *
	 *	@return true if no error
	 */
	bool MDispatcher() const;

	/** Reset RTC counter
	 *
	 */
	bool MResetRTC() const;

	/** Gets current time
	 *
	 * @return current time
	 */
	rtc_time_t MGetCurrentTime() const;

	NSHARE::CProgramName const FName;//!< The unique name of RTC
private:

	/** Updates time and notifies
	 * time receiver
	 *
	 * @warning Thread unsafety operation
	 * @param aNewTime a new time
	 */
	void MUpdateTime(uint64_t aNewTime) const;

	NSHARE::IAllocater* FAllocator;//!<Pointer to allocator
	time_info_t* FTimeInfo;//!< Info about time
	mutable NSHARE::CIPCSem FTimeUpdated; //!< Event to signal the time is changed
	mutable NSHARE::CIPCSignalEvent FHasToBeUpdated;//!< Event to update time
	mutable NSHARE::CIPCSem  FSem;//!< Mutex for lock change of #FTimeInfo
	mutable NSHARE::CMutex FDestroyMutex;/*!< #MDispatcher has the infinite loop therefore
	 	 	 	 	 	 	 	 	 it need to wait until it's finished working
	 	 	 	 	 	 	 	 	 before the object will destroyed */
	bool FIsDone;//!< If true then dispatcher is working

};
/** @brief This class is controlling scheduling (time partitions).
 * It will actually be activated during modeling.
 * But also can be used for time synchronization.
 */
class CTimeDispatcher: public ICore, public NSHARE::CSingleton<CTimeDispatcher>
{
public:
	static const NSHARE::CText NAME;///< A serialization key
	static const NSHARE::CText SHARED_MEMORY_NAME; ///< A serialization key of shared memory unique name

	static const unsigned SHARED_MEMORY_SIZE;//!< Size of shared memory

	static bool TEST_IS_ON;//!< Is start test thread
	CTimeDispatcher();
	~CTimeDispatcher();

	NSHARE::CConfig MSerialize() const;

	/*! @brief Start working (has to be non-blocking)
	 *
	 *	@return true if started successfully
	 */
	bool MStart();
	void MStop();
private:
	typedef std::vector<CRTCForModeling* > array_of_rtc_t;//!< Array of RTC
	typedef NSHARE::CSafeData<array_of_rtc_t> safety_array_of_rtc_t;//!< thread safety type
	typedef int rtc_id_t;//!< is used to hold RTC id

	void MInit();
	bool MCreateSharedMemory();
	rtc_id_t MRegistreNewRTC(NSHARE::CProgramName const& aRtcGroup=NSHARE::CProgramName());
	static NSHARE::eCBRval sMMainLoop(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	static NSHARE::eCBRval sMTestLoop(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	static void sMTestLoop(CRTCForModeling* aRTC);


	void MStartTestIfNeed(rtc_id_t );
	bool MIsRTCExist(
			const NSHARE::CProgramName& aRtcGroup);
	void MStartRTCDispatcher(rtc_id_t _rval);
	rtc_id_t MPushNewRTC(const NSHARE::CProgramName& aRtcGroup);

	//void MDispatcher() const;

	NSHARE::CSharedMemory FMemory;//!< is used for communication
	NSHARE::CText FName;//!< shared memory name
	safety_array_of_rtc_t FArrayOfRTC;

};

} /* namespace NUDT */

#endif /* CTIMEDISPATCHER_H_ */
