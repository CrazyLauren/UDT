/*
 * CRtcSync.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CRTCSYNC_H_
#define CRTCSYNC_H_

#include <IRtcSync.h>

namespace NUDT
{
/** The first RTC node always is owner of sync interface
 *
 */
class CRtcSync: public IRtcSync
{
public:
	CRtcSync(IRtcApi* aRtc);
	~CRtcSync();

	void MSet(IRtcApi* aRtc);
	void MRemove();
	IRtcApi* MGetOwner() const;


	bool MForceUnlock();
	time_info_t::rtc_time_t MSync(time_info_t::rtc_time_t const& aTime);


	void MSyncWith(IRtcSync* aRtc);
	bool MUnSyncWith(IRtcSync* aRtc);
	void MGetAllRtcOf(rtc_array_t* aTo) const;

	void MGetSyncWith(rtc_sync_array_t* aTo) const;
	void MGetSyncWithMe(rtc_sync_array_t* aTo) const;
	unsigned MNumber() const;

	bool MAdd(IRtcSync* aRtc);
	bool MRemove(IRtcSync* aRtc);

	time_info_t::rtc_time_t MSync(IRtcSync* aRtc,
			time_info_t::rtc_time_t const& aTime);

private:
	struct rtc_sync_data_t: NSHARE::IIntrusived
	{
		rtc_sync_data_t(IRtcSync* aApi):
			FTime(time_info_t::END_OF_TIME),//
			FIsDone(false),//
			FSync(false),//
			FSyncApi(aApi),//
			FIsIgnored(aApi->MGetOwner()->MGetType() != eRTC_MODELING)
		{

		}
		inline time_info_t::rtc_time_t  MGetTime() const
		{
			return FTime;
		}

		NSHARE::CCondvar FCondvar;

		time_info_t::rtc_time_t FTime;
		bool FIsDone;
		bool FSync;
		bool FIsIgnored;
		IRtcSync* FSyncApi;//!< Not intrusive as have pointer to self
	};
	typedef std::map<rtc_unique_id_t, NSHARE::intrusive_ptr<rtc_sync_data_t> > sync_data_t;
	typedef std::pair<time_info_t::rtc_time_t, time_info_t::rtc_time_t> min_time_t;//!< the first is min, second second min

	min_time_t MGetMinTime() const;
	unsigned MUnlockThread(time_info_t::rtc_time_t const& aTime,
			IRtcSync const* aRtc
			) const;
	void MRemoveSyncRtc();
	bool MIsNeedSyncImpl() const;

	time_info_t::rtc_time_t FNextTime;
	sync_data_t FSyncDownData;//!< Sync data. @warning the first is always this data

	mutable NSHARE::CMutex FMutexSync;
	unsigned FCurrentNumberOfSync;
	unsigned FNumberOfIgnored;
	unsigned FTimeCounter;
	IRtcApi* FRtc;
	rtc_sync_array_t FSyncUp;
};

}



#endif /* CRTCSYNC_H_ */
