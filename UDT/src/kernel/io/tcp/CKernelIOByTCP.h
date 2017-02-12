/*
 * CControlByTCP.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.12.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CIOTCPIMPL_H_
#define CIOTCPIMPL_H_

#include "ITcpIOManager.h"
#include <io/ILink.h>
namespace NUDT
{
using namespace NSHARE;
class CKernelIOByTCP:public ITcpIOManager
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText TIMEOUT;
	static const NSHARE::CText PORT;
	static const NSHARE::CText LINKS;
	CKernelIOByTCP();
	~CKernelIOByTCP();


	void MInit(CKernelIo *);
	bool MOpen(const void* = NULL);
	bool MIsOpen() const;
	void MClose();
	void MClose(const descriptor_t& aFrom);
	void MCloseRequest(const descriptor_t& aFrom);
	descriptors_t MGetDescriptors() const;
	bool MIs(descriptor_t)const;

	bool MSend(const data_t& aVal, descriptor_t const&);
	bool MSend(const program_id_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const kernel_infos_array_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());

	bool MSend(const fail_send_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const user_data_t& aVal, descriptor_t const& aTo);
	bool MSend(const demand_dgs_for_t& aVal, descriptor_t const& aFrom, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());

	void MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd, const descriptor_t& aFrom);

	bool MSendService(const data_t& aVal,
			const NSHARE::net_address&);

	void MCleanUpNewLinkages();
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig const&  MBufSettingFor(const descriptor_t&,
			NSHARE::CConfig const& aFrom) const;
private:
	class CConnectionHandler;
	class CServerBridge;
	struct channel_t
	{
		smart_link_t FLink;
		NSHARE::intrusive_ptr<CServerBridge> FBridge;
	};
	typedef std::map<descriptor_t,channel_t> channels_t;
	typedef std::map<NSHARE::net_address, descriptor_t,
			NSHARE::net_address::CFastLessCompare> ips_t;

	typedef NSHARE::intrusive_ptr<CConnectionHandler> smart_new_client_t;
	typedef std::map<NSHARE::net_address, smart_new_client_t,
			NSHARE::net_address::CFastLessCompare> new_channels_t;
	typedef std::map<NSHARE::net_address, NSHARE::CText,
			NSHARE::net_address::CFastLessCompare> last_sampling_link_type_t;

	void MInitTcp();

	static int sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	void MServiceReceiver();
	void MReceivedServiceDataImpl(const NSHARE::ISocket::data_t& aData,
			const CTCPServer::recvs_t& aFrom);

	bool MHandleNewLinkage(CTCPServer::client_t const& aAb,
			ISocket::data_t::const_iterator aBegin,
			ISocket::data_t::const_iterator aEnd);
	//void MDisconnectNew(NSHARE::net_address const& aAddr);

	static int sMConnect(void* aWho, void* aWhat, void* aThis);
	void MConnect(CTCPServer::client_t* aVal);
	void MConnectedStatusPoll(CTCPServer::client_t* aVal);
	static int sMDisconnect(void* aWho, void* aWhat, void* aThis);
	void MRefuseClient(const NSHARE::net_address& _addr);
	void MDisconnectImpl(const NSHARE::net_address& _addr);


	void MAddNewActiveLink(channel_t const&);
	bool MAcceptLink(const smart_new_client_t& aNewLink);
	void MReceiveImpl(const smart_link_t& _link,
			const data_t::const_iterator& aBegin,
			const data_t::const_iterator& aEnd);
	template<class _T> inline void MFill(data_t*, const _T&);
	template<class T> inline bool MSendImpl(const T& _id,
			descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError);
	inline bool MSendImpl(const user_data_t& _id, descriptor_t const& aTo);

	//if from disconnect function, It's fix to don't create new method

	NSHARE::CTCPServer FTcpServiceSocket;
	NSHARE::CB_t FCBServiceConnect;
	NSHARE::CB_t FCBServiceDisconncet;
	CKernelIo * FIo;

	channels_t FServiceChannels;
	new_channels_t FNewChannels;
	ips_t FActiveLinks;
	mutable NSHARE::CMutex FMutex;
	NSHARE::CConfig FConfig;
	unsigned FTimeout;
	last_sampling_link_type_t FSampling;
};
class CKernelIOByTCPRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CKernelIOByTCPRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
}
#endif /* CIOTCPIMPL_H_ */
