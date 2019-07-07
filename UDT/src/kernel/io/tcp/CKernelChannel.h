/*
 * CKernelChannel.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CKERNELCHANNEL_H_
#define CKERNELCHANNEL_H_

namespace NUDT
{

/** @brief Class realization TCP client channel for one client
 *
 *	@todo обдумать reopen и close когда и вкаких случаях их вызвать
 *	@todo Only one receive thread for all Clients
 */
struct CKernelIOByTCPClient::CKernelChannel: public ILinkBridge
{
	static const NSHARE::CText NAME; ///< A serialization key

	/** @brief The Current state of connection
	 *
	 *	@note For debug only
	 */
	enum eState
	{
		E_CLOSED,				///< No connection
		E_SERVICE_CONNECTED,	///< Connected only service channel
		E_CONNECTED				///< The channel is established
	};

	/** Create a new channel
	 *
	 * @param aSetting - A channel setting
	 * @param aThis - A reference to base class
	 */
	CKernelChannel(network_channel_t const& aSetting,
			CKernelIOByTCPClient& aThis);

	~CKernelChannel();
	/** Disconnect (Close) from server
	 *
	 */
	void MClose();

	/** Start TCP client
	 *
	 */
	void MOpen();

	/** Determine if the settings is corresponding to this client
	 *
	 * @param aSetting
	 * @return true is corresponding
	 */
	bool MIs(network_channel_t const& aSetting) const;

	descriptor_t MGetDescriptor() const;
	virtual bool MCloseRequest(descriptor_t aId);
	virtual bool MInfo(NSHARE::CConfig & aTo);
	NSHARE::net_address MGetAddr()const;
	virtual bool MConfig(NSHARE::CConfig & aTo);

	bool MSend(const data_t& aVal);
	template<class T>
	inline bool MSend(const T& _id, const routing_t& aRoute,error_info_t const& aError)
	{
		if(FLink)
			return FLink->MSend(_id,aRoute,aError);
		return false;
	}
	inline bool MSend(const user_data_t& _id)
	{
		if(FLink)
			return FLink->MSend(_id);
		return false;
	}
	void MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig const& MBufSettingFor(
			NSHARE::CConfig const& aFrom) const;
private:
	typedef NSHARE::intrusive_ptr<IConnectionHandler> handler_t;

	template<class _T> inline void MFill(data_t*, const _T&);

	static NSHARE::eCBRval sMConnect(void* aWho, void* aWhat, void* aThis);
	static NSHARE::eCBRval sMDisconnect(void* aWho, void* aWhat, void* aThis);

	void MConnect(NSHARE::net_address* aVal);
	void MDisconnect(NSHARE::net_address* aVal);

	static NSHARE::eCBRval sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData);
	void MServiceReceiver();
	bool MAddNewLink();
	void MReceivingForNewLink(const data_t::const_iterator& aBegin,
			const data_t::const_iterator& aEnd);

	eState FState;	///< Current state (for debug only)
	NSHARE::CB_t FCBServiceConnect;
	NSHARE::CB_t FCBServiceDisconncet;

	smart_link_t FLink;
	NSHARE::CTCP FTcp;
	CKernelIOByTCPClient& FThis;	///<A reference to base class
	network_channel_t const FSetting;
	handler_t FConnectionHandler;
	descriptor_t Fd;
	NSHARE::CMutex FLockToRemove;/*!< The mutex is protected against to remove
	 Object before receive thread is finished (sMReceiver) */
};
inline descriptor_t CKernelIOByTCPClient::CKernelChannel::MGetDescriptor() const
{
	if (FLink)
		return FLink->MGetID();
	else
		return Fd;
}
} /* namespace NUDT */
#endif /* CKERNELCHANNEL_H_ */
