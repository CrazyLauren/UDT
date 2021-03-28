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

#include <deftype>
#include <SHARE/revision.h>
#include <SHARE/random_value.h>
#include <CRTCFactory.h>
#include <UDT/IRtc.h>
#include <CDataObject.h>

#include "CRTCForModelingModule.h"

namespace NUDT
{
using namespace NSHARE;
NSHARE::CText const CRTCForModelingModule::NAME = "modeling";
rtc_info_t::unique_id_t CRTCForModelingModule::sFIdCounter = 0;

CRTCForModelingModule::CRTCForModelingModule() :
		IRtcControl(NAME, eRTC_MODELING), //
		FCommonMutex(NSHARE::CMutex::MUTEX_NORMAL),
		FPCustomer(NULL) //
{
	{
		callback_data_t _callbak(sMReceiveRTC, this);
		FHandler = CDataObject::value_t(new_real_time_clocks_t::NAME, _callbak);
	}
	FOwnTimeClocks.FInfoFrom = get_my_id().FId.FUuid;
}

/** Push new RTC
 *
 * @param aRtc a new RTC
 * @return index of array
 */
IRtcControl::rtc_id_t CRTCForModelingModule::MPushRTC(CRTCModelingImpl* aRtc)
{
	protected_RTC_array_t::WAccess<> _access = FRTCArray.MGetWAccess();
	_access->FRtc.push_back(array_of_RTC_t::value_type(aRtc));
	return static_cast<IRtcControl::rtc_id_t>(_access->FRtc.size() - 1);
}
/** Open share memory for RTC
 *
 * @return true if is created
 */
bool CRTCForModelingModule::MOpenSharedMemory()
{
	if (FMemory.MIsOpened())
	{
		LOG(DFATAL) << "Shared memory has been opened early.";
		return false;
	}

	DLOG_IF(FATAL,FRealTimeClocks.FShdMemName.empty())
																<< "Cannot open sm server as the name is not set. Value: "
																<< FRealTimeClocks.FShdMemName;
	if (FRealTimeClocks.FShdMemName.empty())
		return false;

	LOG(INFO) << "Time dispatcher shared memory name:"
							<< FRealTimeClocks.FShdMemName;

	if (FMemory.MOpen(FRealTimeClocks.FShdMemName)
			== CSharedMemory::E_NO_ERROR)
		return true;

	return false;
}
CRTCForModelingModule::rtc_id_t CRTCForModelingModule::MGetNameByRTC(
		name_rtc_t const& aName) const
		{
	protected_RTC_array_t::RAccess<> const _access(FRTCArray.MGetRAccess());
	array_of_RTC_t const& _array(_access->FRtc);

	for (unsigned i = 0; i < _array.size(); ++i)
		if (_array[i]->MGetRTCInfo().FName == aName)
			return i;
	return std::numeric_limits<rtc_id_t>::max();
}

CRTCForModelingModule::array_of_RTC_t CRTCForModelingModule::MGetAllRTC() const
{
	protected_RTC_array_t::RAccess<> const _access(FRTCArray.MGetRAccess());
	return _access->FRtc;
}
bool CRTCForModelingModule::MIsRTC() const
{
	protected_RTC_array_t::RAccess<> const _access(FRTCArray.MGetRAccess());
	return !_access->FRtc.empty();
}
name_rtc_t CRTCForModelingModule::MGetRTCByName(
		rtc_id_t const& aID) const
		{
	protected_RTC_array_t::RAccess<> const _access(FRTCArray.MGetRAccess());
	DCHECK_LT(aID, _access->FRtc.size());

	return (*_access).FRtc[aID]->MGetRTCInfo().FName;
}

IRtc* CRTCForModelingModule::MGetRTC(name_rtc_t const& aID) const
		{
	protected_RTC_array_t::RAccess<> const _access(FRTCArray.MGetRAccess());
	array_of_RTC_t const& _array(_access->FRtc);

	for (array_of_RTC_t::const_iterator _it = _array.begin();
			_it != _array.end(); ++_it)
		if ((*_it)->MGetRTCInfo().FName == aID)
		{
			VLOG(2) << "Rtc is founded " << aID;
			return _it->MGet();
		}

	LOG(INFO) << "No RTC named " << aID;
	return NULL;
}
void CRTCForModelingModule::MInit(ICustomer * aP)
{
	VLOG(2) << " Initialize RTC for modeling ...";
	FPCustomer = aP;

}
bool CRTCForModelingModule::MOpen(const NSHARE::CThread::param_t*)
{
	VLOG(2) << " Open RTC for modeling ...";

	CDataObject::sMGetInstance() += FHandler;
	return true;
}
void CRTCForModelingModule::MJoin()
{
	;
}
bool CRTCForModelingModule::MIsOpened() const
{
	return FMemory.MIsOpened();
}
void CRTCForModelingModule::MClose()
{
	CDataObject::sMGetInstance() -= FHandler;
}
int CRTCForModelingModule::sMReceiveRTC(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	DCHECK_EQ(aWhat->FType, new_real_time_clocks_t::NAME);
	reinterpret_cast<CRTCForModelingModule*>(aData)->MUpdateRTCInfo(
			reinterpret_cast<new_real_time_clocks_t*>(aWhat->FPointToData)->FData);
	return E_CB_SAFE_IT;
}

/** Create RTC
 *
 * @param aRtc list of RTC
 * @return true if is changed
 */
bool CRTCForModelingModule::MCreateRTC(real_time_clocks_t const& aRtc)
{
	//rtc_id_t _rval = std::numeric_limits<rtc_id_t>::max();
	rtc_info_array_t::const_iterator _it = aRtc.FRtc.begin();
	bool _is = false;
	for (; _it != aRtc.FRtc.end(); ++_it)
	{
		if (_it->FRTCType == eRTC_MODELING)
		{
			CRTCModelingImpl* _p = new CRTCModelingImpl(*_it, FMemory);
			if (_p->MIsInitialized())
			{
				VLOG(1) << "Push RTC " << (*_it);

				/*_rval = */MPushRTC(_p);
				_is = true;
			}
			else
				delete _p;
		}
	}
	return _is;
}
IRtc* CRTCForModelingModule::MGetOrCreateRTC(name_rtc_t const& aID)
{
	IRtc* _val = MGetRTC(aID);
	if(_val != NULL)
		return _val;

	return MCreateRTC(aID);
}
IRtc* CRTCForModelingModule::MCreateRTC(name_rtc_t const& aID)
{
	IRtc* _val =NULL;
	{
		rtc_info_t _info;
		_info.FName = aID;
		_info.FRTCType = FRtcType;
		_info.FId.FOwner = get_my_id().FId.FUuid;
		_info.FId.FId = ++sFIdCounter;

		NSHARE::CRAII<NSHARE::CMutex> _lock(FCommonMutex);
		{
			rtc_info_array_t::iterator _it = FOwnTimeClocks.FRtc.begin(),
					_it_end(FOwnTimeClocks.FRtc.end());

			for (; _it != _it_end //
			&& _it->FName == aID
					; ++_it)
				;

			if (_it != _it_end)
				return NULL;
		}
		FOwnTimeClocks.MInsert(_info);
		update_own_real_time_clocks_t _cval;
		_cval.FNewRTC = FOwnTimeClocks;

		CDataObject::sMGetInstance().MPush(_cval);
	}
	_val = MWaitForRTC(aID);
	DCHECK_NOTNULL(_val);
	VLOG(2)<<"Join to created RTC";
	return _val;
}
bool CRTCForModelingModule::MRemoveRTC(name_rtc_t const& aName)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FCommonMutex);

	rtc_info_array_t::iterator _it = FOwnTimeClocks.FRtc.begin(),
			_it_end(FOwnTimeClocks.FRtc.end());
	for (; _it != _it_end //
	&& !(_it->FName == aName)
			; ++_it)
		;
	if (_it != _it_end)
	{
		LOG(INFO)<<"Remove RTC "<<aName;
		FOwnTimeClocks.FRtc.erase(_it);

		update_own_real_time_clocks_t _cval;
		_cval.FNewRTC = FOwnTimeClocks;

		CDataObject::sMGetInstance().MPush(_cval);
		return true;
	}
	LOG(INFO)<<"Cannot remove RTC "<<aName;
	return false;
}
IRtc* CRTCForModelingModule::MWaitForRTC(name_rtc_t const& aID,
		double aTime) const
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FCommonMutex);
	IRtc* _val = NULL;

	if (aTime < 0)
		for (; (_val = MGetRTC(aID)) == NULL;)
			FWaitForEvent.MTimedwait(&FCommonMutex, aTime);
	else
	{
		double const _cur_time = NSHARE::get_time();
		double _time = _cur_time;
		for (; (_val = MGetRTC(aID)) == NULL //
				&& aTime > (_time - _cur_time);)
		{
			FWaitForEvent.MTimedwait(&FCommonMutex,
					aTime - (_time - _cur_time));
			double _time = NSHARE::get_time();
		}
	}

	return _val;
}

void CRTCForModelingModule::MWaitForRTC(
		wait_for_t* aWaitFor
		) const
{
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FCommonMutex);
		FWaitFor[aWaitFor->FID] = NSHARE::intrusive_ptr<wait_for_t>(aWaitFor);
	}
	if (aWaitFor->FRtc == NULL)
	{
		IRtc* _rtc = MGetRTC(aWaitFor->FID);
		if(_rtc != NULL)
			aWaitFor->FRtc = _rtc;
	}
}

void CRTCForModelingModule::MUnWait(
		name_rtc_t const& aWhat
		) const
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FCommonMutex);
	wait_for_store_t::iterator _it = FWaitFor.find(aWhat);

	if(_it != FWaitFor.end())
	{
		NSHARE::intrusive_ptr<wait_for_t> _wf= _it->second;
		FWaitFor.erase(_it);
		_wf->FCondvar.MBroadcast();
	}
}


/** Update RTC
 *
 *@param aOldRTC update RTC
 * @return true if is changed
 */
bool CRTCForModelingModule::MUpdateRTC()
{
	real_time_clocks_t _rtc(FRealTimeClocks);
	bool _is_removed = false;
	{
		protected_RTC_array_t::WAccess<> _access = FRTCArray.MGetWAccess();
		rtc_state_t & _state = *_access;

		array_of_RTC_t::iterator _jt = _state.FRtc.begin();
		for (; _jt != _state.FRtc.end();)
		{
			rtc_info_t _info = (*_jt)->MGetRTCInfo();

			rtc_info_array_t::iterator _it = _rtc.FRtc.begin();
			for (; _it != _rtc.FRtc.end() && !(_info == *_it);)
				if (_it->FRTCType != eRTC_MODELING)
					_it = _rtc.FRtc.erase(_it);//!< doesn't our type of RTC (no more handle it )
				else
					++_it;

			if (_it == _rtc.FRtc.end())
			{
				VLOG(1) << "Remove RTC " << _info.FName;
				//remove
				_jt = _state.FRtc.erase(_jt);
				_is_removed = true;
			}
			else
				++_jt;
		}
	}
	bool const _is_created = MCreateRTC(_rtc);
	return _is_removed || _is_created;
}
/** Update info about RTC
 *
 * @param aRTC a new RTC
 */
void CRTCForModelingModule::MUpdateRTCInfo(real_time_clocks_t const& aRTC)
{
	VLOG(1) << "Update RTC " << aRTC;
	bool _is_changed = false;
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FCommonMutex);
		real_time_clocks_t const _old = FRealTimeClocks;
		FRealTimeClocks = aRTC;

		//if (_old.FShdMemName != FRealTimeClocks.FShdMemName)
		{
			if (!FMemory.MIsOpened() && !MOpenSharedMemory())
			{
				LOG(DFATAL) << "Cannot open new shared memory:"
										<< FRealTimeClocks.FShdMemName;
			}
			else if (!MIsRTC())
				_is_changed = MCreateRTC(FRealTimeClocks);
			else
				_is_changed = MUpdateRTC();

			if (_is_changed)
			{
				CDataObject::sMGetInstance().MPush(
						real_time_clocks_updated_t());
				FWaitForEvent.MBroadcast();
			}
		}
	}

	if(_is_changed)
		MUnlockWaitedRTC();
}

/** Unlock blocked thread which is expected specified RTC
 *
 */
void CRTCForModelingModule::MUnlockWaitedRTC()
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FCommonMutex);
	wait_for_store_t::iterator _it(FWaitFor.begin());
	for (; _it != FWaitFor.end(); ++_it)
	{
		{// may be founded yet
			NSHARE::CRAII<NSHARE::CMutex> _lock(
					_it->second->FLockMutex);
			if (_it->second->FRtc != NULL)
				continue;
		}
		IRtc* _rtc = MGetRTC(_it->first);

		if (_rtc != NULL)
		{
			NSHARE::CRAII<NSHARE::CMutex> _lock(
					_it->second->FLockMutex);

			if (_it->second->FRtc != NULL)// may be founded yet
			{
				_it->second->FRtc = _rtc;
				_it->second->FCondvar.MBroadcast();
			}
		}
	}

}
} /* namespace NUDT */

