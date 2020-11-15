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

	bool _is = true;
	{
		safety_rtc_info_t::WAccess<> _access =FRtcList.MGetWAccess();
		real_time_clocks_t& _rtc(*_access);
		_rtc.FInfoFrom = get_my_id().FId.FUuid;
	}

	real_time_clocks_t _new(_p->MChild(real_time_clocks_t::NAME));
	_new.FInfoFrom = get_my_id().FId.FUuid;

	{
		rtc_info_array_t::iterator _it = _new.FRtc.begin(), _it_end(
				_new.FRtc.end());
		rtc_info_t::unique_id_t _counter = 0;
		for (; _it != _it_end; ++_it)
		{
			_it->FOwner = get_my_id().FId.FUuid;
			_it->FUniqueId = ++_counter;
		}
	}

	_is = _new.MIsValid();

	_is = _is && MCreateSharedMemory(_new.FShdMemName);
	_is = _is && MSubscribe();

	MHandleNewRTC(_new);


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
	{
		callback_data_t _cb(sMHandleNewRTC, this);
		CDataObject::value_t _val(real_time_clocks_t::NAME, _cb);
		FHandlerOfNewRTC = _val;
		CDataObject::sMGetInstance() += FHandlerOfNewRTC;
	}
	return true;
}

/** Inform about RTC
 *
 *	@param aFor Who is to be informed?
 */
bool CTimeDispatcher::MInformAboutRTC(const descriptor_t& aFor)
{
	VLOG(2) << "Inform about RTC :" << aFor;

	safety_rtc_info_t::RAccess<> _access = FRtcList.MGetRAccess();
	real_time_clocks_t const& _rtc(*_access);

	if (!_rtc.FRtc.empty())
	{
		if (aFor == CDescriptors::INVALID)
			CDataObject::sMGetInstance().MPush(_rtc, true);
		else

			CDataObject::sMGetInstance().MPush(make_data_to(aFor, _rtc), true);
	}
	return true;
}
/** Create share memory for RTC
 *
 *	@param aName - name of shared memory
 * 	@return true if is created
 */
bool CTimeDispatcher::MCreateSharedMemory(NSHARE::CText& aName)
{
	if (FMemory.MIsOpened())
		return true;

	DLOG_IF(FATAL,aName.empty())
								<< "Cannot open sm server as the name is not set. Value: "
								<< aName;
	if (aName.empty())
	{
		aName.MMakeRandom(10);
	}

	LOG(INFO) << "Time dispatcher shared memory name:" << aName;

	FMemory.sMRemove(aName);
	if (FMemory.MOpenOrCreate(aName, SHARED_MEMORY_SIZE)
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
	CDataObject::sMGetInstance() -= FHandlerOfNewRTC;
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
void CTimeDispatcher::MStartRTCDispatcher(smart_rtc_modeling_t const& aRTC)
{
	CRTCForModeling* const _p = aRTC.get();
	/** Start main thread. */
	NSHARE::operation_t _op(CTimeDispatcher::sMMainLoop, _p,
			NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);
}
/** Push a new RTC
 *
 * @param aRtcGroup aRTC name
 * @return handler
 */
CTimeDispatcher::smart_rtc_modeling_t CTimeDispatcher::MPushNewRTC(
		const NSHARE::CProgramName& aRtcGroup)
{
	smart_rtc_modeling_t _rval;
	safety_array_of_rtc_t::WAccess<> _accesss = FArrayOfRTC.MGetWAccess();

	array_of_rtc_t& _rtc = *_accesss;
	array_of_rtc_t::const_iterator _it = _rtc.begin();

	for (; _it != _rtc.end(); ++_it)
		if ((*_it)->MGetName() == aRtcGroup)
			break;
	if (_it == _rtc.end())
	{
		array_of_rtc_t::value_type _ptr(
				new CRTCForModeling(FMemory.MGetAllocator(), aRtcGroup));
		_rtc.push_back(_ptr);
		_rval = _rtc.back();
	}
	else//@todo The rtc name can be equal, add compare by name nad ID of owner
		LOG(DFATAL) << "The RTC " << aRtcGroup << " is exist";

	return _rval;
}
/** Remove RTC
 *
 * @param aRtcGroup aRTC name
 * @return handler
 */
CTimeDispatcher::smart_rtc_modeling_t CTimeDispatcher::MPopRTC(
		const NSHARE::CProgramName& aRtcGroup)
{
	smart_rtc_modeling_t _rval;
	//remove
	{
		safety_array_of_rtc_t::WAccess<> _accesss =
				FArrayOfRTC.MGetWAccess();
		array_of_rtc_t& _rtc = *_accesss;

		array_of_rtc_t::iterator _it = _rtc.begin();
		for (; _it != _rtc.end(); ++_it)
			if ((*_it)->MGetName() == aRtcGroup)
				break;
		if (_it != _rtc.end())
		{
            _rval = *_it;
			_rtc.erase(_it);
		}
	}

	return _rval;
}
/** Create RTC
 *
 * @return true if created
 */
bool CTimeDispatcher::MCreateNewRTC(rtc_info_array_t& aNew)
{
	rtc_info_array_t::iterator _it(aNew.begin()), _it_end(
			aNew.end());
	bool _is = false;
	for (; _it != _it_end; ++_it)
	{
		bool const _is_reg = MRegistreNewRTC(*_it);
		DCHECK(_is_reg);
		_is = _is || _is_reg;
	}

	return _is;
}
/** Remove RTC
 *
 * @return true if created
 */
bool CTimeDispatcher::MRemoveRTC(rtc_info_array_t& aRemove)
{
	rtc_info_array_t::iterator _jt(aRemove.begin()), _jt_end(
			aRemove.end());
	bool _is = false;

	for (; _jt != _jt_end; ++_jt)
	{
		smart_rtc_modeling_t _rtc(MPopRTC(_jt->FName));
		if(_rtc)
		{
			MPopFromRTCList(*_jt);
			_is = true;
		}
		//dispatcher will stopped atomatically
		// @ fixme blocking call start in new thread !!!
	}

	return _is;
}

/** Create timer
 *
 * @param aRtcGroup for whom is RTC
 * @return false if error otherwise true
 */
bool CTimeDispatcher::MRegistreNewRTC(rtc_info_t& aRtc)
{
	NSHARE::CProgramName const& aRtcGroup(aRtc.FName);
	smart_rtc_modeling_t const _rval = MPushNewRTC(aRtcGroup);

	if (_rval)
	{
		aRtc.FOffset = _rval->MGetOffset();

		MPutToRTCList(aRtc);

		MStartRTCDispatcher(_rval);
		MStartTestIfNeed(_rval);
	}
	return _rval.get() !=NULL;
}
/** Put to List
 *
 * @param aRtc RTC info
 */
void CTimeDispatcher::MPutToRTCList(rtc_info_t const& aRtc)
{
	safety_rtc_info_t::WAccess<> _access = FRtcList.MGetWAccess();
	real_time_clocks_t& _rtc(*_access);
	_rtc.FRtc.push_back(aRtc);
}
/** Remove from list of RTC
 *
 * @param aRtc RTC info
 * @return true if removed
 */
bool CTimeDispatcher::MPopFromRTCList(rtc_info_t const& aRtc)
{

	safety_rtc_info_t::WAccess<> _access = FRtcList.MGetWAccess();
	real_time_clocks_t& _rtc(*_access);

	rtc_info_array_t::iterator _it = _rtc.FRtc.begin();
	for (; _it != _rtc.FRtc.end(); ++_it)
		if (aRtc.FName == _it->FName //
		&& aRtc.FOwner == _it->FOwner)
			break;

	DCHECK(_it != _rtc.FRtc.end());
	if(_it == _rtc.FRtc.end())
		return false;

	_rtc.FRtc.erase(_it);
	return true;
}



NSHARE::CConfig CTimeDispatcher::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	//@ todo
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
void CTimeDispatcher::MStartTestIfNeed(smart_rtc_modeling_t const& aRtcID)
{
	if (TEST_IS_ON)
	{
		CRTCForModeling* const _p = aRtcID.get();

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
	if (aInfo.FProgramm.FType == E_CONSUMER)
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
	if (aInfo.FProgramm.FType == E_CONSUMER)
	{
		; //todo remove it from RTC
		; //todo remove RTC if it's inner
	}
}

/** Get different of RTC
 *
 *
 * @param aRtc - New RTC
 * @param [out] aNew - new RTC
 * @param [out] aRemoved - removed RTC
 */
void CTimeDispatcher::MGetDiff(const real_time_clocks_t& aRtc,
		rtc_info_array_t* aNew, rtc_info_array_t* aRemoved) const
{
	safety_rtc_info_from_t::RAccess<> _access = FReceivedRTCInfo.MGetRAccess();
	rtc_info_from_t const& _rtc(*_access);
	rtc_info_from_t::const_iterator _it = _rtc.find(aRtc.FInfoFrom);

	if (_it == _rtc.end())
	{
		if(aNew != NULL)
			*aNew = aRtc.FRtc;
	}
	else
	{
		real_time_clocks_t const& _old(_it->second);

		rtc_info_array_t _new_impl(aRtc.FRtc);

		rtc_info_array_t::const_iterator _it(_old.FRtc.begin()), _it_end(
				_old.FRtc.end());

		for (; _it != _it_end; ++_it)
		{

			rtc_info_array_t::const_iterator _jt(_new_impl.begin()), _jt_end(
					_new_impl.end());

			for (; _jt != _jt_end && !(*_jt == *_it); ++_jt)
				;

			if (_jt != _jt_end)
				_new_impl.erase(_jt);
			else if(aRemoved != NULL)
			{
				aRemoved->push_back(*_it);
			}
		}
		if(aNew != NULL)
			aNew->swap(_new_impl);
	}
}
/** Create new RTC
 *
 * @param aRtc The new RTC info
 */
bool CTimeDispatcher::MHandleNewRTC(const real_time_clocks_t& aRtc)
{
	rtc_info_array_t _new;
	rtc_info_array_t _removed;

	MGetDiff(aRtc, &_new, &_removed);

	bool const _is_added = MCreateNewRTC(_new);
	bool const _is_removed = MRemoveRTC(_removed);

	MStoreReceivedRTCInfo(aRtc);

	if(_is_added || _is_removed)
		MInformAboutRTC();
	return true;
}
/** Store received RTC Info
 *
 * @param aRtc
 */
void CTimeDispatcher::MStoreReceivedRTCInfo(const real_time_clocks_t& aRtc)
{
	safety_rtc_info_from_t::WAccess<> _access = FReceivedRTCInfo.MGetWAccess();
	rtc_info_from_t& _rtc(*_access);
	_rtc[aRtc.FInfoFrom] = aRtc;
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
int CTimeDispatcher::sMHandleNewRTC(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CTimeDispatcher* _this = reinterpret_cast<CTimeDispatcher*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(real_time_clocks_t::NAME, WHAT->FType);
	const real_time_clocks_t* _p =
			reinterpret_cast<real_time_clocks_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleNewRTC(*_p);
	return 0;
}

} /* namespace NUDT */
