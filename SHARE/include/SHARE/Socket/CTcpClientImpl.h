/*
 * CTcpClientImpl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CTCPCLIENTIMPL_H_
#define CTCPCLIENTIMPL_H_
#include <SHARE/Socket/CTcpImplBase.h>
namespace NSHARE
{
//Early there were two implementation. One of was client,the other was server.
//Now In "CTCP" class there is only client  implementation.
//"CTCP::CClientImpl" class  is remained by historical reason.
struct CTCP::CClientImpl:public CTcpImplBase, public IIntrusived
{

	CClientImpl(CTCP& aTcp,settings_t const& aSetting);
	~CClientImpl();

	CTCP::settings_t const& MGetInitParam() const;
	bool MClientConnect();

	/** Send data
	 *
	 * @param pData a pointer to data
	 * @param nSize the size data
	 * @return
	 */
	CTCP::sent_state_t MSend(const void* pData, size_t nSize);
	ssize_t MReceiveData(data_t* aBuf, const float aTime);

	void MSetAddress(net_address const& aAddr);

	void MClose();

	bool MOpen();
	bool MIsOpen() const;

	static void sMCleanupConnection(void *aP);
	//int MReadData(data_t* aBuf);
	void MDisconnect();


	CTCP& FTcp;
	CTCP::settings_t FSettings;

	CMutex FConnectMutex;
	CSocket FSock;
	struct sockaddr_in FAddr;

	diagnostic_io_t FDiagnostic;

	CMutex FReceiveThreadMutex; ///<It's used for lock receive operation
	atomic_t FIsDoing; ///< True if the TCP client has to try to connect
	volatile bool FIsConnected; ///< True if the TCP client is connected to server
	atomic_t FIsReceiving;///< If true then object is locked on recv (see #MClose)
};
}



#endif /* CTCPCLIENTIMPL_H_ */
