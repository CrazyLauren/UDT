/*
 * CTcpServerMainChannel.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTCPMAINCHANNEL_H_
#define CTCPMAINCHANNEL_H_

#include "IMainChannel.h"
//#include "CSendBuffering.h"
namespace NUDT
{
class CTcpServerMainChannel: public IMainChannel
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText THREAD_PRIORITY;
	static const NSHARE::CText PORT;

	CTcpServerMainChannel();
	virtual ~CTcpServerMainChannel();
	void MInit();
	bool MOpen(ILink* aHandler,program_id_t const&,NSHARE::net_address const& aFor);
	bool MIsOpen(descriptor_t aFor) const;
	bool MClose(descriptor_t aFor);
	bool MCloseRequest(descriptor_t aFor);

	bool MSendToService(const data_t& aVal, descriptor_t);
	bool MSend(const data_t& aVal, descriptor_t);

	bool MSend(user_data_t const& aVal, descriptor_t);
	virtual NSHARE::ISocket* MGetSocket();
	const virtual NSHARE::ISocket* MGetSocket() const;

	bool MHandleServiceDG(const main_channel_param_t* aData, descriptor_t);
	bool MHandleServiceDG(main_channel_error_param_t const* aP,
			descriptor_t);
	bool MHandleServiceDG(request_main_channel_param_t const* aP,
			descriptor_t);
	bool MHandleServiceDG(close_main_channel_t const* aP, descriptor_t);
	bool MIsOveload(descriptor_t aFor) const;
	NSHARE::CConfig MSerialize() const;
private:
	class CMainClient;
	class CMainClientConnector;


	typedef COverload<NSHARE::net_address> net_overload_t;

	typedef NSHARE::intrusive_ptr<CMainClient> smart_client_t;
	typedef std::map<NSHARE::net_address,smart_client_t,NSHARE::net_address::CFastLessCompare> client_by_ip_t;
	typedef std::map<descriptor_t,smart_client_t > client_by_id_t;

	//typedef std::map<descriptor_t, CSendBuffering> buffers_t;

	struct send_buffers_t
	{
		//buffers_t FBuffer;
		NSHARE::CMutex FMutexSend;
		//NSHARE::CCondvar FCond;
		uint32_t FSendSMBuffer;
	};

	struct link_data_t
	{
		client_by_ip_t FByIp;
		client_by_id_t FById;
	};
	typedef NSHARE::CSafeData<link_data_t> safe_data_t;
	typedef safe_data_t::RAccess<> const r_access;
	typedef safe_data_t::WAccess<> w_access;

	bool MSendImpl(const void* pData, size_t nSize, const NSHARE::net_address&);
	//void MSendImpl();
	static int sMSend(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT,	void* YOU_DATA);
	static NSHARE::eCBRval sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	void MReceiver();
	bool MSendSetting(ILink* aHandler, descriptor_t aFor);
	bool MCheckingForOverload(NSHARE::CTCPServer::recvs_t const& _from) const;
	void MReceivingData(NSHARE::CTCPServer::recvs_t const& _from,std::vector<NSHARE::net_address>& _need_close,std::list<smart_client_t>& _new_clients);
	void MClosing(std::vector<NSHARE::net_address> const&);
	void MAddingNew(std::list<smart_client_t> const&);
	//bool MSendProtectionAndLimitation(send_buffer_t &,user_data_t const& aVal, descriptor_t);

	template<class DG_T> void MFill(data_t*,ILink* aHandler);
	template<class DG_T> void MFill(data_t*);
	template<class T>
	inline void MFill(data_t* _buf, const T& _id);

	static NSHARE::eCBRval sMConnect(void* aWho, void* aWhat, void* aThis);
	static NSHARE::eCBRval sMDisconnect(void* aWho, void* aWhat, void* aThis);
	split_info MGetLimits(size_t aSize,ILink* aHandler) const;
	bool MOpenIfNeed();

	NSHARE::CTCPServer FServer;

	safe_data_t FData;

	send_buffers_t FBuffers;

	mutable net_overload_t FOverload;
	mutable bool FIsOverload;
	NSHARE::intrusive_ptr<CMainClientConnector> FConnector;
	NSHARE::net_address FAddr;
	NSHARE::CMutex FOpenMutex;
};
}
#endif /* CTCPMAINCHANNEL_H_ */
