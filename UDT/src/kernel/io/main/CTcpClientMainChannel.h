/*
 * CTcpClientMainChannel.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTCPCLIENTMAINCHANNEL_H_
#define CTCPCLIENTMAINCHANNEL_H_

#include "IMainChannel.h"
namespace NUDT
{
class CTcpClientMainChannel: public IMainChannel
{
public:
	static const NSHARE::CText MAX_BUFFER_SIZE_FOR_CHANNEL;
	static const NSHARE::CText MAX_SM_BUFFER;

	static const NSHARE::CText NAME;
	static const NSHARE::CText THREAD_PRIORITY;


	CTcpClientMainChannel();
	~CTcpClientMainChannel();
	void MInit();
	bool MOpen(ILink* aHandler, program_id_t const&,NSHARE::net_address const& aFor);
	bool MIsOpen(descriptor_t aFor) const;
	bool MClose(descriptor_t aFor);
	bool MCloseRequest(descriptor_t aFor);

	bool MSendToService(const data_t& aVal, descriptor_t);
	bool MSend(const data_t& aVal, descriptor_t);

	bool MSend(user_data_t const& aVal, descriptor_t);
	virtual NSHARE::ISocket* MGetSocket();
	const virtual NSHARE::ISocket* MGetSocket() const;

	bool MHandleServiceDG(const main_channel_param_t* aData, descriptor_t);
	bool MHandleServiceDG(main_channel_error_param_t const* aP, descriptor_t);
	bool MHandleServiceDG(request_main_channel_param_t const* aP, descriptor_t);
	bool MHandleServiceDG(close_main_channel_t const* aP, descriptor_t);
	bool MIsOveload(descriptor_t aFor) const;

	NSHARE::CConfig MSerialize() const;
private:
	class CMainClient;
	struct new_link_t
	{
		ILink* FLink;
		program_id_t FId;
		NSHARE::net_address FAddr;
	};

	typedef NSHARE::intrusive_ptr<CMainClient> smart_client_t;
	typedef std::map<descriptor_t, smart_client_t> client_by_id_t;
	typedef std::map<NSHARE::CSocket, smart_client_t> client_by_socket_t;
	struct link_data_t
	{
		client_by_socket_t FBySocket;
		client_by_id_t FById;
	};
	typedef NSHARE::CSafeData<link_data_t> safe_data_t;
	typedef safe_data_t::RAccess<> const r_access;
	typedef safe_data_t::WAccess<> w_access;

	typedef std::map<descriptor_t, new_link_t> new_links_t;


	template<class DG_T> void MFill(data_t*);
	template<class DG_T> void MFill(data_t*, DG_T const&,
			data_t::size_type aSize);
	template<class DG_T> void MProcess(const DG_T* aP, void*);

	static NSHARE::eCBRval sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);

	void MReceiver();
	void MReceive(NSHARE::CSelectSocket::socks_t const&);
	void MUnLockSelect();

	void MCloseImpl();
	bool MAddNewClient(link_data_t& _this_data,smart_client_t &_link);
	bool MEraseClient(link_data_t& _this_data,smart_client_t &_link);
	bool MOpenIfNeed();

	NSHARE::CSelectSocket FSelectSock;
	NSHARE::CLoopBack FLoopBack;

	safe_data_t FData;
	new_links_t FNewLinks;

	NSHARE::CMutex FMutexNewLinks;
	NSHARE::CMutex FOpenMutex;

};

} /* namespace NUDT */
#endif /* CTCPCLIENTMAINCHANNEL_H_ */
