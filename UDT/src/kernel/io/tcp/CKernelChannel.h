/*
 * CKernelChannel.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 28.07.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CKERNELCHANNEL_H_
#define CKERNELCHANNEL_H_

namespace NUDT
{

//todo Only one receive thread for all Clients
//todo обдумать reopen и close когда и вкаких случаях
//их вызвать
struct CKernelIOByTCPClient::CKernelChannel: public ILinkBridge
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText LINK_TYPE;
	static const NSHARE::CText ADDR;
	enum eState //don't use pattern
	//as it's  difficult for the class
	{
		E_CLOSED,	//
		E_SERVICE_CONNECTED,	//
		E_SETTING,	//
		E_CONNECTED	//
	};


	CKernelChannel(CConfig const& aWhat,CKernelIOByTCPClient& aThis);
	void MClose();
	void MOpen(const void* aP);
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

	static int sMConnect(void* aWho, void* aWhat, void* aThis);
	static int sMDisconnect(void* aWho, void* aWhat, void* aThis);

	void MConnect(NSHARE::net_address* aVal);
	void MDisconnect(NSHARE::net_address* aVal);

	static int sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData);
	void MServiceReceiver();
	bool MAddNewLink();
	void MReceivingForNewLink(const data_t::const_iterator& aBegin,
			const data_t::const_iterator& aEnd);

	eState FState;	//for debuging only
	NSHARE::CB_t FCBServiceConnect;
	NSHARE::CB_t FCBServiceDisconncet;

	smart_link_t FLink;
	NSHARE::CTCP FTcp;
	CKernelIOByTCPClient& FThis;
	CConfig FConfig;
	NSHARE::CText const FLinkType;
	handler_t FConnectionHandler;
	descriptor_t Fd;
};
} /* namespace NUDT */
#endif /* CKERNELCHANNEL_H_ */
