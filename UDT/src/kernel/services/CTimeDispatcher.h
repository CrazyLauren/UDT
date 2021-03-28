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

#include <SHARE/UType/CIPCMutex.h>
#include <SHARE/UType/CSingleton.h>
#include <SHARE/UType/CSharedMemory.h>

#include <core/ICore.h>
#include <core/CDescriptors.h>

#include <CRTCForModeling.h>
#include <IRtcSync.h>
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

	struct rtc_data_t
	{
		NSHARE::intrusive_ptr<IRtcApi> FApi;
		NSHARE::intrusive_ptr<IRtcSync> FSyncApi;//!< Own sync object with hierarchical down RTC
	};
	//typedef CRTCForModeling> smart_rtc_modeling_t;
	typedef std::vector<rtc_data_t> array_of_rtc_t;//!< Array of RTC
	typedef NSHARE::CSafeData<array_of_rtc_t> safety_array_of_rtc_t;//!< thread safety type
	typedef std::map<NSHARE::uuid_t, real_time_clocks_t> rtc_info_from_t;//!< RTC info from
	typedef NSHARE::CSafeData<rtc_info_from_t> safety_rtc_info_from_t;

	typedef NSHARE::CSafeData<real_time_clocks_t> safety_rtc_info_t;//!< RTC info
	typedef int rtc_id_t;//!< is used to hold RTC id
	typedef std::vector<std::pair<IRtcSync *, IRtcSync*> > removed_sync_t;//!< first unsync from, second - who is unsync

	void MInit();
	bool MCreateSharedMemory(NSHARE::CText& aName);
	bool MCreateNewRTC(rtc_info_array_t& aNew);
	bool MRemoveRTC(rtc_info_array_t& aRemoved);

	bool MInformAboutRTC(const descriptor_t& aFor=CDescriptors::INVALID);
	bool MSubscribe();
	void MUnSubscribe();
	bool MRegistreNewRTC(rtc_info_t& aRtc);
	static NSHARE::eCBRval sMMainLoop(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	static NSHARE::eCBRval sMTestLoop(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	static void sMTestLoop(IRtcApi* aRTC);
	static int sMHandleCloseId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	static int sMHandleOpenId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	static int sMHandleNewRTC(CHardWorker* WHO, args_data_t* WHAT,
				void* YOU_DATA);

	void MHandleOpen(const descriptor_t& aFrom, const descriptor_info_t&);
	void MHandleClose(const descriptor_t& aFrom, const descriptor_info_t&);
	bool MHandleNewRTC(const real_time_clocks_t&);


	bool MCreateSharedMemoryIfNeed();
	void MStartTestIfNeed(IRtcApi *);
	bool MIsRTCExist(
			const NSHARE::CProgramName& aRtcGroup);
	void MStartRTCDispatcher(IRtcApi *);

	NSHARE::intrusive_ptr<IRtcApi> MCreateNewRTC(rtc_info_t& aRtc);

	bool MPushNewRTC(IRtcApi* );
	bool MGetNearestRTCFor(IRtcApi* ,
			array_of_rtc_t& aFrom,
			std::vector<array_of_rtc_t::iterator>* aToEqual,
			std::vector<array_of_rtc_t::iterator>* aToUp,
			std::vector<array_of_rtc_t::iterator>* aToDown) const;

	NSHARE::intrusive_ptr<IRtcApi> MPopRTC(const rtc_unique_id_t& aRtcGroup);

	void MGetDiff(const real_time_clocks_t& aRtc,
			rtc_info_array_t* aNew,
			rtc_info_array_t* aOld) const;
	void MStoreReceivedRTCInfo(const real_time_clocks_t& aRtc);
	void MPutToRTCList(rtc_info_t const& aRtc);
	bool MPopFromRTCList(rtc_info_t const& aRtc);
	unsigned MIsIn(NSHARE::CProgramName const& aWhat,
			NSHARE::CProgramName const& aIn) const;

	void MRemoveHierarchyDownRtc(IRtcSync *aSyncInfo,
			IRtcApi* aRtc,
			removed_sync_t* aRemoved);
	void MSyncHierarchyUpRtc(rtc_data_t& aRtcData,
			std::vector<array_of_rtc_t::iterator> const& aNearestUp,
			removed_sync_t* aRemoved);

	void MSyncHierarchyDownRtc(rtc_data_t& aRtcData,
			std::vector<array_of_rtc_t::iterator> const& aNearestDown);

	//void MDispatcher() const;

	NSHARE::CSharedMemory FMemory;//!< is used for communication

	safety_array_of_rtc_t FArrayOfRTC;
	safety_rtc_info_t FRtcList;//!< current list of avalaible RTC
	safety_rtc_info_from_t FReceivedRTCInfo;//!< Map of received RTC INFO

	CDataObject::value_t FHandlerOfOpen;//!< Handler of connection of new program
	CDataObject::value_t FHandlerOfClose;//!< Handler of disconnection of the program
	CDataObject::value_t FHandlerOfNewRTC;//!< Handler of new RTC

};

} /* namespace NUDT */

#endif /* CTIMEDISPATCHER_H_ */
