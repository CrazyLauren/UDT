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
#include <Socket/CTcpImplBase.h>
namespace NSHARE
{
//Early there were two implementation. One of was client,the other was server.
//Now In "CTCP" class there is only client  implementation.
//"CTCP::CClientImpl" class  is remained by historical reason.
struct CTCP::CClientImpl:public CTcpImplBase
{

	CClientImpl(CTCP& aTcp);
	~CClientImpl();

	net_address MGetInitParam() const;
	bool MClientConnect();

	CTCP::sent_state_t MSend(const void* pData, size_t nSize);
	ssize_t MReceiveData(data_t* aBuf, const float aTime);

	bool MIsClient() const;


	void MClose();

	static void sMCleanupConnection(void *aP);
	//int MReadData(data_t* aBuf);
	void MCloseImpl();


	CTCP& FTcp;
	volatile bool FIsReceive;
	CMutex FMutex;
	CSocket FSock;
	struct sockaddr_in FAddr;

	diagnostic_io_t FDiagnostic;
	unsigned FConnectionCount;
	unsigned FAgainError;
	uint8_t _buf[1];

};
}



#endif /* CTCPCLIENTIMPL_H_ */
