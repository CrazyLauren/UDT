/*
 * IRtcSync.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IRTCSYNC_H_
#define IRTCSYNC_H_
#include <udt_rtc_types.h>

namespace NUDT
{
class IApiSync;

class IRtcApi: public NSHARE::IIntrusived
{
public:
	virtual ~IRtcApi()
	{

	}

	/** Setup sync node
	 *
	 * @param aRtcSync
	 * @note has to call MSet method of IApiSync
	 */
	virtual void MSet(IApiSync* aRtcSync)=0;

	/** Remove synchronize method
	 *
	 * @return removed node
	 * @note has to call MRemove method of IApiSync
	 */
	virtual NSHARE::intrusive_ptr<IApiSync> MRemoveSync()=0;

	/** Gets Sync node
	 *
	 * @return sync node
	 */
	virtual NSHARE::intrusive_ptr<IApiSync> MGetSync() const=0;

	/** Gets unique ID of RTC
	 *
	 * @return unique id
	 */
	virtual rtc_unique_id_t MGetId() const =0;

	/** Get RTC type
	 *
	 * @return RTC type
	 */
	virtual eRTCType MGetType() const =0;

	/** Gets RTC name
	 *
	 * @return RTC name
	 */
	virtual NSHARE::CProgramName const& MGetName() const =0;

	/** Serialize data
	 *
	 * @return
	 */
	virtual NSHARE::CConfig MSerialize() const=0;

	/** Start RTC (blocking call)
	 *
	 * @return true if no error
	 */
	virtual bool MDispatcher() =0;

	/** Gets the number of joined
	 *
	 * @return number of joined or negative value if doesn't initialized
	 */
	virtual int MGetAmountOfJoined() const =0;

	/** Info about this RTC will be sync from the other
	 *
	 */
	virtual void MSyncStateIsUpdated() =0;
};
class IApiSync: public NSHARE::IIntrusived
{
	public:

	/** Set synchronized node
	 *
	 * @param aRtc RTC
	 */
	virtual void MSet(IRtcApi* aRtc) =0;

	/** Remove RTC from synchronize
	 *
	 */
	virtual void MRemove() =0;

	/** Gets RTC
	 *
	 * @return RTC or NULL
	 */
	virtual IRtcApi* MGetOwner() const =0;

	/** Force unlock locked thread
	 *
	 * @param  aRtc - RTC
	 * @return
	 */
	virtual bool MForceUnlock() =0;

	/** Locked RTC until time is occured
	 *
	 * @param aTime - It's call time
	 * @return Time different from the minimal
	 * time of the other RTC or time_info_t::END_OF_TIME if error is occured
	 */
	virtual time_info_t::rtc_time_t MSync(time_info_t::rtc_time_t const& aTime) =0;

};
class IRtcSync: public IApiSync
{
public:
	typedef std::vector<IRtcApi*> rtc_array_t;
	typedef std::vector<IRtcSync*> rtc_sync_array_t;

	virtual ~IRtcSync()
	{

	}

	virtual void MSyncWith(IRtcSync* aRtc)  =0;

	/** Unsync with the other RTC
	 *
	 * @param aRtc Sync interface
	 * @return true if can removed
	 */
	virtual bool MUnSyncWith(IRtcSync* aRtc)  =0;


	/** Store all RTC which is sync
	 *
	 * @param aTo store to
	 */
	virtual void MGetAllRtcOf(rtc_array_t* aTo) const =0;

	/** Store all RTC sync node
	 *
	 * @param aTo store to
	 */
	virtual void MGetSyncWith(rtc_sync_array_t* aTo) const =0;

	/** Store all RTC sync node
	 *
	 * @param aTo store to
	 */
	virtual void MGetSyncWithMe(rtc_sync_array_t* aTo) const =0;

	/** The number of RTC
	 *
	 */
	virtual unsigned MNumber() const =0;

	virtual bool MAdd(IRtcSync* aRtc) =0;
	virtual bool MRemove(IRtcSync* aRtc) =0;

	/** Locked RTC until time is occured
	 *
	 * @param aRtc - RTC
	 * @param aTime - It's call time
	 * @return Time different from the minimal
	 * time of the other RTC or time_info_t::END_OF_TIME if error is occured
	 */
	virtual time_info_t::rtc_time_t MSync(IRtcSync* aRtc,
			time_info_t::rtc_time_t const& aTime) =0;

};
}



#endif /* IRTCSYNC_H_ */
