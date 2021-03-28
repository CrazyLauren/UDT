// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CRtcSync.cpp
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
#include "CRtcSync.h"
namespace NUDT
{
CRtcSync::CRtcSync(IRtcApi* aRtc) :
		FMutexSync(NSHARE::CMutex::MUTEX_NORMAL), //
		FNextTime(time_info_t::END_OF_TIME), //
		FCurrentNumberOfSync(0), //
		FNumberOfIgnored(0),//
		FTimeCounter(0), //
		FRtc(NULL)
{
	aRtc->MSet(this); /// @warning virtual methods called !!
}
CRtcSync::~CRtcSync()
{
	MRemoveSyncRtc();
	/*	IRtcApi * _ptr = NULL;
	 do
	 {
	 {
	 NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	 _ptr = !FSyncDownData.empty() ? FSyncDownData.begin()->second->FApi : NULL;
	 }
	 if(_ptr != NULL)
	 _ptr->MRemoveSync();

	 }while(_ptr != NULL);*/
}

void CRtcSync::MSet(IRtcApi* aRtc)
{
	DCHECK(FRtc == aRtc);

	{
		NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
		FRtc = aRtc;
	}
	MAdd(this);
}
void CRtcSync::MRemove()
{
	DCHECK_NOTNULL(FRtc);
	MRemoveSyncRtc();

	MRemove(this);

	NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	FRtc = NULL;
}
IRtcApi* CRtcSync::MGetOwner() const
{
	return FRtc;
}

/** Remove synchronize node
 *
 * @param aSync Synchronize node
 */
void CRtcSync::MRemoveSyncRtc()
{
	IRtcSync::rtc_sync_array_t _array_me;

	{
		MGetSyncWithMe(&_array_me);

		IRtcSync::rtc_sync_array_t::iterator _jt = _array_me.begin(),
				_jt_end = _array_me.end();

		for (; _jt != _jt_end; ++_jt)
		{
			(*_jt)->MUnSyncWith(this);
		}
	}

	IRtcSync::rtc_sync_array_t _array;
	{
		MGetSyncWith(&_array);

		IRtcSync::rtc_sync_array_t::iterator _jt = _array.begin(),
				_jt_end = _array.end();

		for (; _jt != _jt_end; ++_jt)
		{
			MUnSyncWith((*_jt));
		}
	}

	IRtcSync::rtc_sync_array_t::iterator _it = _array.begin(),
			_it_end = _array.end();
	for (; _it != _it_end; ++_it)
	{

		IRtcSync::rtc_sync_array_t::iterator _jt = _array_me.begin(),
				_jt_end = _array_me.end();

		for (; _jt != _jt_end; ++_jt)
		{
			//todo check in
//			if(MIsIn((*_jt)->MGetOwner()->MGetName(),
//					(*_it)->MGetOwner()->MGetName()))
			(*_jt)->MSyncWith(*_it);
//			else
//				DCHECK(false) << (*_jt)->MGetOwner()->MGetName() << " "
//										<< (*_it)->MGetOwner()->MGetName();
		}
	}
}
bool CRtcSync::MAdd(IRtcSync* aRtc)
{
	rtc_unique_id_t const _id(aRtc->MGetOwner()->MGetId());
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);

		sync_data_t::iterator _it = FSyncDownData.find(_id);
		DCHECK(_it == FSyncDownData.end());
		FSyncDownData[_id] = NSHARE::intrusive_ptr<rtc_sync_data_t>(
				new rtc_sync_data_t(aRtc));
	}

	MGetOwner() ->MSyncStateIsUpdated();
	return true;
}
bool CRtcSync::MRemove(IRtcSync* aRtc)
{
	rtc_unique_id_t const _id(aRtc->MGetOwner()->MGetId());
	bool _is =false;
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
		sync_data_t::iterator _it = FSyncDownData.find(_id);
		DCHECK(_it != FSyncDownData.end());
		if (_it != FSyncDownData.end())
		{
			_is = true;
			DCHECK(FSyncDownData.size() > 1 || (
					_it->second->FSyncApi == this //
					&& aRtc == this
					)
					);

			NSHARE::intrusive_ptr<rtc_sync_data_t> _rsd = _it->second;
			FSyncDownData.erase(_it);

			_rsd->FCondvar.MBroadcast();
			_rsd->FIsDone = true;

			//Unlock the other blocked thread
			if (!_rsd->FSync //
			&& (FCurrentNumberOfSync + FNumberOfIgnored) == FSyncDownData.size() //
			&& !FSyncDownData.empty()
					)
			{
				DCHECK(FSyncDownData.begin()->second->FSync);
				FSyncDownData.begin()->second->FCondvar.MBroadcast();
			}
		}
	}
	if(_is)
		MGetOwner() ->MSyncStateIsUpdated();
	return true;
}
void CRtcSync::MSyncWith(IRtcSync* aRtc)
{
	bool const _is = aRtc->MAdd(this);
	if (_is)
	{
		VLOG(1) << "Add sync interface " << aRtc->MGetOwner()->MGetId();
		NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
		FSyncUp.push_back(aRtc);
	}
}
/** Check is need sync
 *
 * @return true if need
 * @warning doesn't thread safety
 */
bool  CRtcSync::MIsNeedSyncImpl() const
{
	DCHECK_GE(FSyncDownData.size(), 1);
	return !(FSyncUp.empty() && FSyncDownData.size() == 1/*the first is this*/);
}
bool CRtcSync::MUnSyncWith(IRtcSync* aRtc)
{
	bool const _is = aRtc->MRemove(this);
	if (_is)
	{
		VLOG(1) << "Remove sync interface " << aRtc->MGetOwner()->MGetId();

		NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);

		rtc_sync_array_t::iterator _it = FSyncUp.begin();
		for (; _it != FSyncUp.end() //
			&& *_it != aRtc; ++_it)
			;
		DCHECK(_it != FSyncUp.end());
		FSyncUp.erase(_it);
		return MIsNeedSyncImpl();
	}
	return false;
}

time_info_t::rtc_time_t CRtcSync::MSync(IRtcSync* aRtc,
		time_info_t::rtc_time_t const& aTime)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	NSHARE::intrusive_ptr<rtc_sync_data_t> _rtc;
	{

		sync_data_t::iterator _it = FSyncDownData.find(
				aRtc->MGetOwner()->MGetId());
		DCHECK(_it != FSyncDownData.end());
		_rtc = _it->second;
	}
	if (!_rtc.MIs() || _rtc->FIsDone || FSyncDownData.size() == 1)
		return time_info_t::END_OF_TIME;
	DCHECK_GE(FSyncDownData.size(), 1);

	_rtc->FSync = true;

	_rtc->FTime = aTime;

	bool _is_lock = false;
	min_time_t _min_time;
	do
	{
		++FCurrentNumberOfSync;

		unsigned const _prev_counter = FTimeCounter;
		for (; _prev_counter == FTimeCounter //
		&& (FCurrentNumberOfSync + FNumberOfIgnored) != FSyncDownData.size() //
		&& !_rtc->FIsDone
				;)
			_rtc->FCondvar.MTimedwait(&FMutexSync);

		//todo force unlock
		--FCurrentNumberOfSync;

		if (_prev_counter == FTimeCounter
				&& !_rtc->FIsDone
				)
		{
			_min_time = MGetMinTime();
			FNextTime = _min_time.first;
			++FTimeCounter;

			MUnlockThread(_min_time.first, aRtc);

			_is_lock = _min_time.first != aTime;
		}
		else
			_is_lock = false;

	} while (_is_lock && !_rtc->FIsDone);

	_rtc->FSync = false;

	//fixme replace to diff FNextTime
	return !_rtc->FIsDone? _min_time.second: time_info_t::END_OF_TIME;
}

time_info_t::rtc_time_t CRtcSync::MSync(
		time_info_t::rtc_time_t const& aTime)
{
	using namespace std;

	min_time_t _min_time;
	{//before sync down
		NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
		if (!FSyncDownData.empty())
		{
			_block.MUnlock();

			_min_time.first = MSync(this, aTime);
		}
	}
	//then sync up
	{
		/** If last Up sync less then our time
		 *
		 */
		NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
		if (!FSyncUp.empty())
		{
			CHECK_EQ(FSyncUp.size(), 1); //todo

			IRtcSync* const _rtc = FSyncUp.front();
			_block.MUnlock();

			_min_time.second = _rtc->MSync(this, aTime);
		}
	}
	return min(_min_time.first,_min_time.second  );
}
bool CRtcSync::MForceUnlock()
{
	NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	if (!FSyncDownData.empty())
	{
		sync_data_t::iterator _it = FSyncDownData.begin();
		DCHECK_EQ(_it->second->FSyncApi, this);

		if (_it == FSyncDownData.end())
			return false;

		_it->second->FCondvar.MBroadcast();
	}
	return false;
}
unsigned CRtcSync::MUnlockThread(time_info_t::rtc_time_t const& aTime,
		IRtcSync const* aRtc
		) const
		{
	unsigned _number = 0;
	sync_data_t::const_iterator _it = FSyncDownData.begin();
	for (; _it != FSyncDownData.end(); ++_it)
	{
		if (aRtc != _it->second->FSyncApi && _it->second->MGetTime() == aTime)
		{
			++_number;
			_it->second->FCondvar.MSignal();
		}
	}
	return _number;
}

CRtcSync::min_time_t CRtcSync::MGetMinTime() const
{
	time_info_t::rtc_time_t _min_time = time_info_t::END_OF_TIME;
	time_info_t::rtc_time_t _next_min_time = time_info_t::END_OF_TIME;
	{
		sync_data_t::const_iterator _it = FSyncDownData.begin();

		for (; _it != FSyncDownData.end(); ++_it)
		{
			using namespace std;
			time_info_t::rtc_time_t const _min(_it->second->MGetTime());

			_next_min_time = _min > _min_time ? _min: _next_min_time;

			_min_time = min(_min_time, _min);


		}
	}
	return min_time_t(_min_time, _next_min_time);
}
void CRtcSync::MGetSyncWith(rtc_sync_array_t* aTo) const
{
	NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	aTo->insert(aTo->end(), FSyncUp.begin(), FSyncUp.end());
}
void CRtcSync::MGetSyncWithMe(rtc_sync_array_t* aTo) const
		{
	DCHECK_NOTNULL(aTo);

	NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	if (!FSyncDownData.empty())
	{
		DCHECK(FSyncDownData.begin()->second->FSyncApi == this);

		sync_data_t::const_iterator _it = ++FSyncDownData.begin(); //the first is this

		for (; _it != FSyncDownData.end(); ++_it)
		{
			aTo->push_back(_it->second->FSyncApi);
		}
	}
}
unsigned CRtcSync::MNumber() const
{
	NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	return (unsigned)FSyncDownData.size();
}
void CRtcSync::MGetAllRtcOf(rtc_array_t* aTo) const
		{
	DCHECK_NOTNULL(aTo);

	NSHARE::CRAII<NSHARE::CMutex> _block(FMutexSync);
	{
		sync_data_t::const_iterator _it = FSyncDownData.begin();

		for (; _it != FSyncDownData.end(); ++_it)
		{
			aTo->push_back(_it->second->FSyncApi->MGetOwner());
		}
	}

	{
		rtc_sync_array_t::const_iterator _it = FSyncUp.begin();

		for (; _it != FSyncUp.end(); ++_it)
		{
			aTo->push_back((*_it)->MGetOwner());
		}
	}
}

}
