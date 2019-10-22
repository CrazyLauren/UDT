// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTimeDispatcher.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/random_value.h>
#include <core/CDataObject.h>
#include <core/CConfigure.h>
#include "CTimeDispatcher.h"
#define NUM_THREAD 10
template<>
NUDT::CTimeDispatcher::singleton_pnt_t NUDT::CTimeDispatcher::singleton_t::sFSingleton =
NULL;
namespace NUDT
{
using namespace NSHARE;

const NSHARE::CText CTimeDispatcher::NAME = "time_dispatcher";
const NSHARE::CText CTimeDispatcher::SHARED_MEMORY_NAME = "name";

unsigned const CTimeDispatcher::SHARED_MEMORY_SIZE = 1024;
bool CTimeDispatcher::TEST_IS_ON = false;

CTimeDispatcher::CTimeDispatcher() : //
		ICore(NAME)
{
	MInit();
}

CTimeDispatcher::~CTimeDispatcher()
{

}
void CTimeDispatcher::MInit()
{
}

bool CTimeDispatcher::MStart()
{
	CConfig const* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);


	FRtc=real_time_clocks_t(_p->MChild(real_time_clocks_t::NAME));
	bool _is = FRtc.MIsValid();
	_is = _is && MCreateSharedMemory();
	_is = _is && MCreateNewRTC();
	_is = _is && MInformAboutRTC();
	_is = _is && MSubscribe();
	return _is;
}

/** Subscribe to the events
 *
 */
inline bool CTimeDispatcher::MSubscribe()
{
	{
		callback_data_t _cb(sMHandleOpenId, this);
		CDataObject::value_t _val(open_descriptor::NAME, _cb);
		FHandlerOfOpen = _val;
		CDataObject::sMGetInstance() += FHandlerOfOpen;
	}
	{
		callback_data_t _cb(sMHandleCloseId, this);
		CDataObject::value_t _val(close_descriptor::NAME, _cb);
		FHandlerOfClose = _val;
		CDataObject::sMGetInstance() += FHandlerOfClose;
	}
	return true;
}

/** Inform about RTC
 *
 *	@param aFor Who is to be informed?
 */
bool CTimeDispatcher::MInformAboutRTC(const descriptor_t& aFor)
{
	VLOG(2)<<"Inform about RTC :"<<aFor;

	if (!FRtc.empty())
	{
		if (aFor == CDescriptors::INVALID)
			CDataObject::sMGetInstance().MPush(FRtc, true);
		else

			CDataObject::sMGetInstance().MPush(make_data_to(aFor, FRtc), true);
	}
	return true;
}
/** Create share memory for RTC
 *
 * @return true if is created
 */
bool CTimeDispatcher::MCreateSharedMemory()
{
	if (FMemory.MIsOpened())
		return true;

	DLOG_IF(FATAL,FRtc.FShdMemName.empty())
													<< "Cannot open sm server as the name is not set. Value: "
													<< FRtc.FShdMemName;
	if (FRtc.FShdMemName.empty())
		FRtc.FShdMemName.MMakeRandom(10);

	LOG(INFO) << "Time dispatcher shared memory name:" << FRtc.FShdMemName;

	FMemory.sMRemove(FRtc.FShdMemName);
	if (FMemory.MOpenOrCreate(FRtc.FShdMemName, SHARED_MEMORY_SIZE)
			== CSharedMemory::E_NO_ERROR)
		return true;

	return false;
}
void CTimeDispatcher::MStop()
{
	VLOG(3) << "Stopping CTimeDispatcher";
	MUnSubscribe();
	{
		safety_array_of_rtc_t::WAccess<> _accesss =
				FArrayOfRTC.MGetWAccess();
		_accesss->clear();
	}
}
/** Unsubscribe from the events
 *
 */
void CTimeDispatcher::MUnSubscribe()
{
	CDataObject::sMGetInstance() -= FHandlerOfOpen;
	CDataObject::sMGetInstance() -= FHandlerOfClose;
}
/** The static method of thread pool
 *
 * @param WHO
 * @param WHAT
 * @param aData
 * @return The operation has to be removed
 */
NSHARE::eCBRval CTimeDispatcher::sMMainLoop(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CRTCForModeling*>(aData)->MDispatcher();
	return E_CB_REMOVE;
}

/** Check if specified a RTC name exist
 *
 * @param aRtcGroup a RTC name that want to check
 * @return true if exist
 */
bool CTimeDispatcher::MIsRTCExist(
		const NSHARE::CProgramName& aRtcGroup)
{
	safety_array_of_rtc_t::RAccess<> const _accesss = FArrayOfRTC.MGetRAccess();
	array_of_rtc_t const& _rtc = *_accesss;
	array_of_rtc_t::const_iterator _it = _rtc.begin();
	for (; _it != _rtc.end(); ++_it)
		if ((*_it)->MGetName() == aRtcGroup)
			break;
	return _it != _rtc.end();
}
/** Start RTC dispatcher
 *
 * @param _rval aDispatcher ID
 */
void CTimeDispatcher::MStartRTCDispatcher(rtc_id_t _rval)
{
	const safety_array_of_rtc_t::RAccess<> _access = FArrayOfRTC.MGetRAccess();
	CRTCForModeling* const _p = (*_access)[_rval].get();
	/** Start main thread. */
	NSHARE::operation_t _op(CTimeDispatcher::sMMainLoop, _p,
			NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);
}
/** Push a new RTC
 *
 * @param aRtcGroup aRTC name
 * @return -1 if error otherwise RTC Id
 */
CTimeDispatcher::rtc_id_t CTimeDispatcher::MPushNewRTC(
		const NSHARE::CProgramName& aRtcGroup)
{
	rtc_id_t _rval = -1;
	safety_array_of_rtc_t::WAccess<> _accesss = FArrayOfRTC.MGetWAccess();

	array_of_rtc_t& _rtc = *_accesss;
	array_of_rtc_t::const_iterator _it = _rtc.begin();

	for (; _it != _rtc.end(); ++_it)
		if ((*_it)->MGetName() == aRtcGroup)
			break;
	if (_it == _rtc.end())
	{
		array_of_rtc_t::value_type _ptr(new CRTCForModeling(FMemory.MGetAllocator(), aRtcGroup));
		_rtc.push_back(_ptr);
		_rval = _rtc.size() - 1;
	}
	else
		LOG(DFATAL) << "The RTC " << aRtcGroup << " is exist";

	return _rval;
}

/** Create RTC
 *
 * @return true if no error
 */
bool CTimeDispatcher::MCreateNewRTC()
{
	real_time_clocks_t::iterator _it(FRtc.begin()), _it_end(FRtc.end());
	for (; _it != _it_end; ++_it)
	{
		_it->FOffset=MRegistreNewRTC(_it->FName);
	}
	return true;
}
/** Create timer
 *
 * @param aRtcGroup for whom is RTC
 * @return -1 if error otherwise RTC Id
 */
NSHARE::IAllocater::offset_pointer_t CTimeDispatcher::MRegistreNewRTC(
		NSHARE::CProgramName const& aRtcGroup)
{
	NSHARE::IAllocater::offset_pointer_t _offset=NSHARE::IAllocater::NULL_OFFSET;
	rtc_id_t const _rval = MPushNewRTC(aRtcGroup);

	if (_rval >= 0)
	{
		MStartRTCDispatcher(_rval);
		MStartTestIfNeed(_rval);
		{
			safety_array_of_rtc_t::RAccess<> const _access =
						FArrayOfRTC.MGetRAccess();
			array_of_rtc_t const& _d=*_access;
			_offset=_d[_rval]->MGetOffset();
		}
	}
	return _offset;
}
NSHARE::CConfig CTimeDispatcher::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	return _config;
}
/** For testing
 *
 * @param WHO
 * @param WHAT
 * @param aData
 * @return
 */
NSHARE::eCBRval CTimeDispatcher::sMTestLoop(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* aData)
{
	sMTestLoop(reinterpret_cast<CRTCForModeling*>(aData));
	return E_CB_REMOVE;
}
/** Start test if need
 *
 * @param aRtcID - RTC Id
 */
void CTimeDispatcher::MStartTestIfNeed(rtc_id_t aRtcID)
{
	if (TEST_IS_ON)
	{
		safety_array_of_rtc_t::RAccess<> const _access =
				FArrayOfRTC.MGetRAccess();
		CRTCForModeling* const _p = (*_access)[aRtcID].get();

		for (unsigned i = 0; i < NUM_THREAD; ++i)
		{
			NSHARE::operation_t _op(CTimeDispatcher::sMTestLoop, _p,
					NSHARE::operation_t::IO);
			CDataObject::sMGetInstance().MPutOperation(_op);
		}
	}
}
/** Test loop
 *
 *@param aRTC pointer to RTC that you  want to test
 */
void CTimeDispatcher::sMTestLoop(CRTCForModeling* aRTC)
{
	CHECK_NOTNULL(aRTC);

	aRTC->MJoinToRTC();

	CRTCForModeling::rtc_time_t _cur_time = aRTC->MGetCurrentTime();
	CRTCForModeling::rtc_time_t _prev_next_time = _cur_time;
	CRTCForModeling::rtc_time_t _next_time = _cur_time + 1;

	for (; _cur_time < (time_info_t::END_OF_TIME - 1000000); //
			_prev_next_time = _next_time, //
			_next_time += (NSHARE::get_random_value_by_RNG() % 1000000) //
			)
	{
		DCHECK_GE(_next_time, _cur_time);
		for (; (_cur_time = aRTC->MNextTime(_next_time)) != _next_time;)
		{
			CHECK_LE(_cur_time, _next_time);
		}
	}
}

/** Handle new program
 *
 * @param aFrom The id of new program
 * @param aInfo The info of new program
 */
void CTimeDispatcher::MHandleOpen(const descriptor_t& aFrom,
		const descriptor_info_t& aInfo)
{
	if(aInfo.FProgramm.FType==E_CONSUMER)
	{
		MInformAboutRTC(aFrom);
	}
}
/** Close new program
 *
 * @param aFrom The id of new program
 * @param aInfo The info of new program
 */
void CTimeDispatcher::MHandleClose(const descriptor_t& aFrom,
		const descriptor_info_t& aInfo)
{
	if(aInfo.FProgramm.FType==E_CONSUMER)
	{
		;
	}
}
int CTimeDispatcher::sMHandleOpenId(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CTimeDispatcher* _this = reinterpret_cast<CTimeDispatcher*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(open_descriptor::NAME, WHAT->FType);
	const open_descriptor* _p =
			reinterpret_cast<const open_descriptor*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleOpen(_p->FId, _p->FInfo);
	return 0;
}
int CTimeDispatcher::sMHandleCloseId(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CTimeDispatcher* _this = reinterpret_cast<CTimeDispatcher*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(close_descriptor::NAME, WHAT->FType);
	const close_descriptor* _p =
			reinterpret_cast<close_descriptor*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleClose(_p->FId, _p->FInfo);
	return 0;
}

} /* namespace NUDT */
