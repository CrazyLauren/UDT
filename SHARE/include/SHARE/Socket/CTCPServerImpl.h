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

#include <SHARE/Socket/CTCPSelectReceiver.h>

namespace NSHARE
{
struct CTCPServer::CImpl:public CTCPSelectReceiver
{
	CImpl(CTCPServer * aThis);

	/** Close the client by Its IP
	 *
	 * @param aIP  A client IP
	 * @return true if closed
	 */
	bool MCloseClient(const net_address& aIP);

	/** Add the new client
	 *
	 * @param aSocket A socket
	 * @param aAddress A IP address
	 */
	bool MAddClient(CSocket& aSocket, const net_address& aAddress);

	/** Returns info about socket
	 *
	 * @param aSocket The socket
	 * @return Info about socket
	 */
	client_t MGetSocketInfo(CSocket const& aSocket) const;

	/** Returns true if there are clients
	 *
	 * @return true if at least one client is exist
	 */
	bool MIsClients() const;

	bool MIsClient(const net_address& aIP) const;

	void MClose();

	/** Receives the data
	 *
	 * @param aFrom [out] From whom the data is received
	 * @param aBuf [out] Where the received data is passed
	 * @param aTime [out] Wait for time
	 * @return The amount of received bytes or -1
	 */
	ssize_t MReceiveData(recvs_t*aFrom, data_t*aBuf, const float aTime);

	bool MOpen();


	/** Send to all clients
	 *
	 * @param pData A pointer to data
	 * @param nSize A data size
	 * @return send state
	 */
	sent_state_t MSend(const void* pData, size_t nSize);

	/** Send to specified client
	 *
	 * @param pData A pointer to data
	 * @param nSize A data size
	 * @param aTo A ip address of client
	 * @return send state
	 */
	sent_state_t MSend(const void* pData, size_t nSize, const net_address&aTo);

	/** Send to specified client
	 *
	 * @param pData A pointer to data
	 * @param nSize A data size
	 * @param aTo A ip address of client or serialized object type of CTCPServer::client_t
	 * @return send state
	 */
	sent_state_t MSend(const void* pData, size_t nSize, NSHARE::CConfig const& aTo);

	bool MIsOpen() const;
	bool MSetAddress(const net_address& aParam);

	/** Returns diagnostic information about
	 * socket
	 *
	 * @return reference to object
	 */
	diagnostic_io_t const& MGetDiagnosticState() const;

	CTCPServer::settings_t const& MGetSetting() const;
	const CSocket& MGetSocket(void) const;

	/** @brief Returns info about connected clients
	 *
	 * @return list of connected clients
	 */
	list_of_clients MGetConnectedClientInfo() const;

	/** @brief Returns info about disconnected clients
	 *
	 * @return list of connected clients
	 */
	list_of_clients const&  MGetDisconnectedClientInfo() const;
protected:
	void MClientIsRemoved(CSocket& aSocket);
	void MClientIsAdded(CSocket& aSocket);
private:
	typedef std::map<CSocket, client_t> clients_fd_t;///< The list of clients
	typedef NSHARE::CSafeData<clients_fd_t>::RAccess<> const CRAccsess;///< Access for reading to #clients_fd_t type
	typedef NSHARE::CSafeData<clients_fd_t>::WAccess<> CWAccsess;///< Access for reading to #clients_fd_t type
	typedef std::map<net_address, clients_fd_t::const_iterator > client_by_ip_t;///< The list of clients sorted by IP


	sent_state_t MSendTo(clients_fd_t::value_type const& aSock, const void* pData, size_t nSize);

	void MStartConnectionThread();
	void MStopConnectionThread();
	void MWaitForConnectionThread();
	void MCloseHostSocket();
	static eCBRval sMListenThread(void*, void*, void* pData);
	void MListen();
	bool MListenSocket();
	bool MOpenHostSocket();

	bool MAddSocket(CSocket const& aSocket,client_t const& aClient);
	client_t MRemoveSocket(CSocket const& aSocket);
	bool MUpdateClientInfo(recvs_t*aFrom, data_t& aBuf,size_t aFirstByte, read_data_from_t const& aSockets);


	CThread FConnectThread;///< Thread for handle a new connection
	//CCondvar FConnectCondVar;///<Condition variable for blocking receiving thread
	CMutex FConnectMutex;///<Lock connect thread

	CTCPServer *FThis;

	NSHARE::CSafeData<clients_fd_t> FClients;///<The list of clients
	NSHARE::CSafeData<client_by_ip_t> FClientsByIP;///<The list of clients #FClients sorted by IP
	list_of_clients FInfoAboutOldClient;///<Info about the old client @todo RW lock
	atomic_t FIsAccepting;///< 0 - FConnectThread isn't working, 1 - isn't working

	CSocket FHostSock;///< A host socket
	settings_t FSettings;///<Settings
	mutable diagnostic_io_t FCurrentDiagnostic;///< A current diagnostic state
};
inline CTCPServer::settings_t const& CTCPServer::CImpl::MGetSetting() const
{
	return FSettings;
}
inline const CSocket& CTCPServer::CImpl::MGetSocket(void) const
{
	return FHostSock;
}
}

#endif /* CTCPSERVERIMPL_H_ */
