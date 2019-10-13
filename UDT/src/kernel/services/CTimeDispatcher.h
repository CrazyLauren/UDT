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

#include <UType/CIPCMutex.h>
#include <UType/CSingleton.h>
#include <UType/CSharedMemory.h>

#include <core/ICore.h>
#include <core/CDescriptors.h>

#include <CRTCForModeling.h>
namespace NUDT
{

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

	/** @copydoc ICore::MStop()
	 *
	 */
	void MStop();
private:
	typedef std::vector<SHARED_PTR<CRTCForModeling> > array_of_rtc_t;//!< Array of RTC
	typedef NSHARE::CSafeData<array_of_rtc_t> safety_array_of_rtc_t;//!< thread safety type
	typedef int rtc_id_t;//!< is used to hold RTC id

	void MInit();
	bool MCreateSharedMemory();
	bool MCreateNewRTC();
	bool MInformAboutRTC(const descriptor_t& aFor=CDescriptors::INVALID);
	bool MSubscribe();
	void MUnSubscribe();
	NSHARE::IAllocater::offset_pointer_t MRegistreNewRTC(NSHARE::CProgramName const& aRtcGroup=NSHARE::CProgramName());
	static NSHARE::eCBRval sMMainLoop(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	static NSHARE::eCBRval sMTestLoop(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	static void sMTestLoop(CRTCForModeling* aRTC);
	static int sMHandleCloseId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	static int sMHandleOpenId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	void MHandleOpen(const descriptor_t& aFrom, const descriptor_info_t&);
	void MHandleClose(const descriptor_t& aFrom, const descriptor_info_t&);

	bool MCreateSharedMemoryIfNeed();
	void MStartTestIfNeed(rtc_id_t );
	bool MIsRTCExist(
			const NSHARE::CProgramName& aRtcGroup);
	void MStartRTCDispatcher(rtc_id_t _rval);
	rtc_id_t MPushNewRTC(const NSHARE::CProgramName& aRtcGroup);

	//void MDispatcher() const;

	NSHARE::CSharedMemory FMemory;//!< is used for communication
	real_time_clocks_t FRtc;//!< list of RTC
	safety_array_of_rtc_t FArrayOfRTC;
	CDataObject::value_t FHandlerOfOpen;//!< Handler of connection of new program
	CDataObject::value_t FHandlerOfClose;//!< Handler of disconnection of the program

};

} /* namespace NUDT */

#endif /* CTIMEDISPATCHER_H_ */
