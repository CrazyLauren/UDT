/*
 * CTCPServerImpl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CTCPSERVERIMPL_H_
#define CTCPSERVERIMPL_H_
#include <Socket/CTcpImplBase.h>
namespace NSHARE
{
struct CTCPServer::loop_back_t: IIntrusived
{
	CLoopBack FLoop;
};
struct CTCPServer::CImpl:public CTcpImplBase, IIntrusived
{
	struct cl_t:client_t
	{
		diagnostic_io_t FDiagnostic;
		mutable unsigned FAgainError;
	};
	typedef std::map<CSocket, cl_t> clients_fd_t;
	typedef NSHARE::CSafeData<clients_fd_t>::RAccess<> const CRAccsess;
	typedef NSHARE::CSafeData<clients_fd_t>::WAccess<> CWAccsess;


	CImpl(CTCPServer * aThis);

	size_t MAvailable() const;
	bool MCanReceive() const;
	bool MCloseClient(const net_address& aTo);
	void MCloseAllClients();
	void MClose();

	ssize_t MReceiveData(recvs_t*aFrom, data_t*aBuf, const float aTime);
	bool MReceiveLoopBack();

	bool MOpenHostSocket();
	bool MOpenLoopSocket();

	cl_t MNewClient_t(net_address const &) const;
	void MMakeNonBlocking(CSocket& aSocket);
	void MCloseClient(CSocket& aSocket);
	bool MIsClient() const;
	int MWaitData(const float aTime) const;
	bool MOpen();
	static eCBRval sMConnect(void*, void*, void* pData);
	void MAccept();
	static eCBRval sMCleanupMutex(void*, void*, void* aP);

	sent_state_t MSend(const void* pData, size_t nSize);
	sent_state_t MSendTo(clients_fd_t::value_type const& aSock, const void* pData, size_t nSize);
	sent_state_t MSend(const void* pData, size_t nSize, const net_address&aTo);
	sent_state_t MSend(const void* pData, size_t nSize, NSHARE::CConfig const& aTo);
	net_address MGetAddress(const struct sockaddr_in&) const;

	cl_t MAddNewClient(CSocket& _sock, struct sockaddr_in& _addr);
	void MUnLockSelect();
	//int MReceiveFrom(data_t* aBuf, CSocket &);
	void MExpectConnection();
	void MReserveMemory(data_t* aBuf, CSelectSocket::socks_t const&);
	int MReceiveFromAllSocket(data_t* aBuf, CSelectSocket::socks_t &,
			recvs_t* aFrom);
	void MCalculateDataBegin(recvs_t*aFrom, data_t*aBuf);


	CSocket FHostSock;
	CSelectSocket FSelectSock;

	struct sockaddr_in FHostAddr;
	NSHARE::CSafeData<clients_fd_t> FClients;

	volatile bool FIsWorking;
	volatile bool FIsReceive;

	diagnostic_io_t FDiagnostic;

	CThread FThread;
	CMutex FMutex;		//fixme deprecated
	CCondvar FCond;		//fixme deprecated
	NSHARE::intrusive_ptr<loop_back_t> FLoopBack;
	CTCPServer *FThis;
	CSelectSocket::socks_t FTo;
};
}

#endif /* CTCPSERVERIMPL_H_ */
