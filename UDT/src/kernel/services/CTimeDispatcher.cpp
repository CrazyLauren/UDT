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
#include "CRtcSync.h"

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

	real_time_clocks_t _new(_p->MChild(real_time_clocks_t::NAME));
	_new.FInfoFrom = get_my_id().FId.FUuid;

	{
		rtc_info_array_t::iterator _it = _new.FRtc.begin(), _it_end(
				_new.FRtc.end());
		rtc_info_t::unique_id_t _counter = 0;
		for (; _it != _it_end; ++_it)
		{
			_it->FId.FOwner = get_my_id().FId.FUuid;
			_it->FId.FId = ++_counter;
		}
	}

	_is = _new.MIsValid();

	NSHARE::CText _SharedMemoryName = _new.FShdMemName;

	_is = _is && MCreateSharedMemory(_SharedMemoryName);

	{
		safety_rtc_info_t::WAccess<> _access = FRtcList.MGetWAccess();
		real_time_clocks_t& _rtc(*_access);
		_rtc.FInfoFrom = get_my_id().FId.FUuid;
		_rtc.FShdMemName = _SharedMemoryName;
	}

	_is = _is && MSubscribe();

	MHandleNewRTC(_new);
	_new.FShdMemName = _SharedMemoryName;

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
		CDataObject::value_t _val(real_time_clocks_id_t::NAME, _cb);
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

	VLOG(2) << " RTC name " << _rtc.FShdMemName;

	if (!_rtc.FRtc.empty())
	{
		DCHECK(!_rtc.FShdMemName.empty());

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
	else
		FMemory.sMRemove(aName);

	LOG(INFO) << "Time dispatcher shared memory name:" << aName;

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
	reinterpret_cast<IRtcApi*>(aData)->MDispatcher();
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
		if ((*_it).FApi->MGetName() == aRtcGroup)
			break;
	return _it != _rtc.end();
}
/** Start RTC dispatcher
 *
 * @param _rval aDispatcher ID
 */
void CTimeDispatcher::MStartRTCDispatcher(IRtcApi * aRTC)
{
	/** Start main thread. */
	NSHARE::operation_t _op(CTimeDispatcher::sMMainLoop, aRTC,
			NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);
}
/** Create new RTC
 *
 * @param aRtc
 * @return
 */
NSHARE::intrusive_ptr<IRtcApi> CTimeDispatcher::MCreateNewRTC(rtc_info_t& aRtc)
{
	NSHARE::CProgramName const& aRtcGroup(aRtc.FName);

	CRTCForModeling* _rtc = new CRTCForModeling(FMemory.MGetAllocator(),
			aRtcGroup,
			aRtc.FId
			);
	aRtc.FOffset = _rtc->MGetOffset();

	return NSHARE::intrusive_ptr<IRtcApi>(_rtc);
}
unsigned CTimeDispatcher::MIsIn(NSHARE::CProgramName const& aWhat,
		NSHARE::CProgramName const& aIn) const
{
	return aIn.MIsName() ?
					aWhat.MIsForMe(aIn) :
					aWhat.MIsFrom(aIn.MGetAddress());
}
/** Gets nearest dependent RTC
 *
 * @param aRtc A RTC for search
 * @param aFrom Looking for in
 * @param aToEqual Nearest from up
 * @param aToUp Nearest from up
 * @param aToDown Nerarest from down
 * @return true exist nearest from up or down
 */
bool CTimeDispatcher::MGetNearestRTCFor(IRtcApi* aRtc,
		array_of_rtc_t& aFrom,
		std::vector<array_of_rtc_t::iterator>* aToEqual,
		std::vector<array_of_rtc_t::iterator>* aToUp,
		std::vector<array_of_rtc_t::iterator>* aToDown) const
		{
	typedef std::multimap<unsigned, array_of_rtc_t::iterator> priority_rtc_t;

	CProgramName const& _name(aRtc->MGetName());

	priority_rtc_t _prior_up;
	std::vector<array_of_rtc_t::iterator>& _nearest_up(*aToUp);

	priority_rtc_t _prior_down;
	std::vector<array_of_rtc_t::iterator>& _nearest_down(*aToDown);

	{
		array_of_rtc_t::iterator _it = aFrom.begin(), _it_end(aFrom.end());
		for (; _it != _it_end; ++_it)
		{
			DCHECK((*_it).FApi.MIs());
			//DCHECK_NE(aRtc->MGetId(), (*_it).FApi->MGetId());

			if (aRtc->MGetId() != (*_it).FApi->MGetId())
			{
				rtc_data_t const& _other_info(*_it);
				CProgramName const& _other(_other_info.FApi->MGetName());

				unsigned const _up = MIsIn(_name, _other);
				unsigned const _down = MIsIn(_other, _name);

				if (_up > 1)
				{
					_prior_up.insert(
							priority_rtc_t::value_type(_up,
									_it));
				}
				if (_down > 1)
				{
					_prior_down.insert(
							priority_rtc_t::value_type(_down,
									_it));
				}
				if(_up == 1 && _down == 1)
				{
					aToEqual->push_back(_it);
				}
				DCHECK(_down == 0 || _up == 0 || (_up == 1 && _down == 1));
			}
		}
	}

	if (!_prior_up.empty())
	{
		priority_rtc_t::const_iterator _jt = _prior_up.begin(), _jt_end(
				_prior_up.end());
		unsigned const _min_prior = _jt->first;
		for (; _jt != _jt_end && _min_prior == _jt->first; ++_jt)
			_nearest_up.push_back(_jt->second);
	}

	if (!_prior_down.empty())
	{
		priority_rtc_t::const_iterator _jt = _prior_down.begin(), _jt_end(
				_prior_down.end());
		unsigned const _min_prior = _jt->first;
		for (; _jt != _jt_end && _min_prior == _jt->first; ++_jt)
			_nearest_down.push_back(_jt->second);
	}
	return !_nearest_up.empty() || !_nearest_down.empty();
}
/** Remove Hierarchy down RTC from synchronize
 * Node
 *
 * @param aSyncInfo Synchronize node
 * @param aRtc aDown from
 * @param [out] aRemoved - Removed RTC
 */
void CTimeDispatcher::MRemoveHierarchyDownRtc(IRtcSync *aSyncInfo,
		IRtcApi* aRtc,
		removed_sync_t* aRemoved
		)
{
	DCHECK_NOTNULL(aSyncInfo);
	DCHECK_GE(aSyncInfo->MNumber(), 2);
	//IRtcApi * _other_rtc = _other_info.FApi.MGet();

	IRtcSync::rtc_sync_array_t _array;
	aSyncInfo->MGetSyncWithMe(&_array);
	DCHECK(!_array.empty());

	IRtcSync::rtc_sync_array_t::iterator _jt = _array.begin(),
			_jt_end = _array.end();

	for (; _jt != _jt_end; ++_jt)
	{
		IRtcApi* const _owner = (*_jt)->MGetOwner();
		DCHECK_NOTNULL(_owner);

		if (_owner != NULL //
				&& MIsIn(_owner->MGetName(), aRtc->MGetName())//
				&& !(_owner->MGetName() == aRtc->MGetName())//
		)
		{

			//(*_jt)->MUnSyncWith(aSyncInfo);//doesn't remove as will added late to aRtc
			aRemoved->push_back(removed_sync_t::value_type(aSyncInfo, *_jt));


		}
	}
}
/** Append synchronize for hierarchical up node
 *
 * @param aRtcData Added RTC
 * @param aNearestUp Nearest up node of RTC
 * @param aRemoved list of removed RTC
 */
void CTimeDispatcher::MSyncHierarchyUpRtc(rtc_data_t& aRtcData,
		std::vector<array_of_rtc_t::iterator> const& aNearestUp,
		removed_sync_t* aRemoved)
{
	///@warning you can change only data from pointer
	std::vector<array_of_rtc_t::iterator>::const_iterator _it =
			aNearestUp.begin(), _it_end(
			aNearestUp.end());

	for (; _it != _it_end; ++_it)
	{
		rtc_data_t & _other_info(*(*_it));

		if (!_other_info.FSyncApi.MIs())
		{
			_other_info.FSyncApi = new CRtcSync(_other_info.FApi.MGet());

			VLOG(1) << "Create synchronize node for "
								<< _other_info.FApi->MGetId() <<
								"(" << _other_info.FApi->MGetName()
								<< ")"
								;
		}
		else
		{
			MRemoveHierarchyDownRtc(_other_info.FSyncApi.MGet(),
					aRtcData.FApi.MGet(),
					aRemoved
					);
		}

		if(!aRtcData.FApi.MIs())
		{
			aRtcData.FSyncApi = new CRtcSync(aRtcData.FApi.MGet());

			VLOG(1) << "Create synchronize node for "
								<< aRtcData.FApi->MGetId() <<
								"(" << aRtcData.FApi->MGetName()
								<< ")"
								;
		}

		aRtcData.FSyncApi->MSyncWith(_other_info.FSyncApi.MGet());

		LOG(INFO) << "Create synchronize from "
								<< _other_info.FApi->MGetId() <<
								"(" << _other_info.FApi->MGetName()
								<< ") and "
								<< aRtcData.FApi->MGetId() <<
								"(" << aRtcData.FApi->MGetName() << ")"
								;
	}

}
/** Append synchronize for hierarchical don node
 *
 * @param aRtcData Added RTC
 * @param aNearestDown Nearest down node of RTC
 */
void CTimeDispatcher::MSyncHierarchyDownRtc(rtc_data_t& aRtcData,
		std::vector<array_of_rtc_t::iterator> const& aNearestDown)
{
	std::vector<array_of_rtc_t::iterator>::const_iterator _it =
			aNearestDown.begin(), _it_end(
			aNearestDown.end());

	for (; _it != _it_end; ++_it)
	{
		rtc_data_t & _other_info(*(*_it));
		IRtcApi * const _other_rtc(_other_info.FApi.MGet());

		if (!_other_info.FSyncApi.MIs())
		{
			_other_info.FSyncApi = new CRtcSync(_other_rtc);

			VLOG(1) << "Create synchronize node for "
								<< _other_info.FApi->MGetId() <<
								"(" << _other_info.FApi->MGetName()
								<< ")"
								;
		}

		if (!aRtcData.FSyncApi.MIs())
		{
			aRtcData.FSyncApi = new CRtcSync(aRtcData.FApi.MGet());

			VLOG(1) << "Create synchronize node for "
								<< aRtcData.FApi->MGetId() <<
								"(" << aRtcData.FApi->MGetName()
								<< ")"
								;
		}

		_other_info.FSyncApi->MSyncWith(aRtcData.FSyncApi.MGet());

		LOG(INFO) << "Create synchronize from "
								<< aRtcData.FApi->MGetId() <<
								"(" << aRtcData.FApi->MGetName()
								<< ") and "
								<< _other_rtc->MGetId() <<
								"(" << _other_rtc->MGetName() << ")"
								;
	}
}

/** Push a new RTC
 *
 * @param aRtc aRTC name
 * @return handler
 */
bool CTimeDispatcher::MPushNewRTC(
		IRtcApi* aRtc)
{
	safety_array_of_rtc_t::WAccess<> _accesss = FArrayOfRTC.MGetWAccess();

	array_of_rtc_t& _rtc = *_accesss;
	{
		array_of_rtc_t::value_type _data;
		_data.FApi = aRtc;
		_rtc.push_back(_data);
	}

	std::vector<array_of_rtc_t::iterator> _equal_of;
	std::vector<array_of_rtc_t::iterator> _nearest_up;
	std::vector<array_of_rtc_t::iterator> _nearest_down;

	MGetNearestRTCFor(aRtc, _rtc, &_equal_of, &_nearest_up, &_nearest_down);

	rtc_data_t & _rtc_info(_rtc.back());
	CProgramName const& _my_name(aRtc->MGetName());

	removed_sync_t _has_to_be_removed;

	if (_equal_of.empty())
	{
		if (!_nearest_up.empty())
		{

			MSyncHierarchyUpRtc(_rtc_info, _nearest_up, &_has_to_be_removed);

#ifndef NDEBUG
#	if __cplusplus >= 201103L
			for (auto const& _d : _has_to_be_removed)
			{
				DCHECK(
						std::find_if(_nearest_down.begin(),
								_nearest_down.end(),
								[&](array_of_rtc_t::iterator const& aRtcData)
								{
									return aRtcData->FApi.MGet() == _d.second->MGetOwner();
								}
								) != _nearest_down.end());
			}
#	endif
#endif
		}
		if (!_nearest_down.empty())
		{
			MSyncHierarchyDownRtc(_rtc_info, _nearest_down);
		}
	}else
	{
		/// @ todo Has equal RTC. Need synchronize its, before the other

	}


	if(!_has_to_be_removed.empty())
	{
		removed_sync_t::const_iterator _it(_has_to_be_removed.begin()),
				_it_end(_has_to_be_removed.end());

		for (; _it != _it_end; ++_it)
		{
			_it->second->MUnSyncWith(_it->first);

			IRtcApi* const _owner = _it->second->MGetOwner();
			IRtcApi* const _owner_from = _it->first->MGetOwner();

			LOG(INFO) << "Remove RTC sync of " << _owner->MGetName()
									<< "("
									<< _owner->MGetId()
									<< ") from "
									<< _owner_from->MGetName()
									<< "("
									<< _owner_from->MGetId()
									<< ") as it in "
									<< aRtc->MGetName()
									<< "("
									<< aRtc->MGetId()
									<< ")"
									;
		}
	}

	return true;
}

/** Remove RTC
 *
 * @param aRtcGroup aRTC name
 * @return handler
 */
NSHARE::intrusive_ptr<IRtcApi> CTimeDispatcher::MPopRTC(
		const rtc_unique_id_t& aRtcId)
{
	VLOG(2)<<"Pop RTC "<<aRtcId;
	rtc_data_t _info;
	{
		safety_array_of_rtc_t::WAccess<> _accesss =
				FArrayOfRTC.MGetWAccess();
		array_of_rtc_t& _rtc = *_accesss;

		array_of_rtc_t::iterator _it = _rtc.begin();
		for (; _it != _rtc.end(); ++_it)
			if ((*_it).FApi->MGetId() == aRtcId)
				break;

		if (_it != _rtc.end())
		{
			_info = *_it;
			_rtc.erase(_it);
		}
	}
	if (_info.FApi.MIs())
	{
		VLOG(2)<<"RTC is founded: "<<aRtcId;

		NSHARE::intrusive_ptr<IApiSync> const _sync_api =
				_info.FApi->MGetSync();
		if (_sync_api.MIs())
		{
			VLOG(2)<<"Sync node is exist !!! in "<<aRtcId;

			DCHECK_EQ(_info.FSyncApi.MGet(), _sync_api.MGet());

			_info.FApi->MRemoveSync();

			DCHECK_EQ(_info.FSyncApi->MNumber(), 0);
			_info.FSyncApi = NULL;
		}
	}

	return _info.FApi;
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
		NSHARE::intrusive_ptr<IRtcApi> _rtc(MPopRTC(_jt->FId));
		if (_rtc)
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
	NSHARE::intrusive_ptr<IRtcApi> const _rval(MCreateNewRTC(aRtc));

	if (_rval.MIs() && MPushNewRTC(_rval.MGet()))
	{
		MPutToRTCList(aRtc);

		MStartRTCDispatcher(_rval.MGet());
		MStartTestIfNeed(_rval.MGet());
	}
	return _rval.MIs();
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
		if (aRtc.FId == _it->FId)
			break;

	DCHECK(_it != _rtc.FRtc.end());
	if (_it == _rtc.FRtc.end())
		return false;

	_rtc.FRtc.erase(_it);
	return true;
}

NSHARE::CConfig CTimeDispatcher::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	_config.MAdd(FMemory.MSerialize());
	{
		safety_array_of_rtc_t::RAccess<> const _accesss =
				FArrayOfRTC.MGetRAccess();
		array_of_rtc_t const& _rtc = *_accesss;
		array_of_rtc_t::const_iterator _it = _rtc.begin();
		for (; _it != _rtc.end(); ++_it)
		{
			NSHARE::CConfig _info((*_it).FApi->MSerialize());
			_info.MAdd<unsigned>(rtc_info_t::KEY_RTC_TYPE, eRTC_MODELING);
			//(*_it)->
			_config.MAdd(_info);
		}
	}
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
	sMTestLoop(reinterpret_cast<IRtcApi*>(aData));
	return E_CB_REMOVE;
}
/** Start test if need
 *
 * @param aRtcID - RTC Id
 */
void CTimeDispatcher::MStartTestIfNeed(IRtcApi * _p)
{
	if (TEST_IS_ON)
	{
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
void CTimeDispatcher::sMTestLoop(IRtcApi* aRTC)
{
	CHECK_NOTNULL(aRTC);

	/*	aRTC->MJoinToRTC();

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
	 }*/
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
		if (aNew != NULL)
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

			rtc_info_array_t::iterator _jt(_new_impl.begin()), _jt_end(
					_new_impl.end());

			for (; _jt != _jt_end && !(*_jt == *_it); ++_jt)
				;

			if (_jt != _jt_end)
				_new_impl.erase(_jt);
			else if (aRemoved != NULL)
			{
				aRemoved->push_back(*_it);
			}
		}
		if (aNew != NULL)
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

	if (_is_added || _is_removed)
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
	CHECK_EQ(real_time_clocks_id_t::NAME, WHAT->FType);
	const real_time_clocks_id_t* _p =
			reinterpret_cast<real_time_clocks_id_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleNewRTC(_p->FVal);
	return 0;
}

} /* namespace NUDT */
