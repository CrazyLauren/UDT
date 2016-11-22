/*
 * CTcpClientImpl.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 07.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <Net.h>
#include <UType/CDenyCopying.h>
#include <UType/CThread.h>
#include <Socket/CTcpClientImpl.h>
#include <errno.h>
#include <console.h>
#if !defined(_WIN32)
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>                     // sockaddr_in
#include <arpa/inet.h>                      // htons, htonl
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#	ifdef __QNX__
#include <netinet/tcp_var.h>//only BSD
#	endif
#else
#include <ws2tcpip.h>
#endif
//#include <unistd.h>
#include <string.h>
#include <Socket/print_socket_error.h>
namespace NSHARE
{
#define IMPL CTCP::CClientImpl

CTCP::CClientImpl::CClientImpl(CTCP& aTcp) :
		FTcp(aTcp), FIsReceive(false)
{
	VLOG(2) << "Construct CClientImpl :" << this;
	memset(_buf, 0, sizeof(_buf));
	memset(&FAddr, 0, sizeof(FAddr));
	FConnectionCount = 0;
	FAgainError = 0;
	FSock = MNewSocket();
	if (!FSock.MIsValid())
	{
		LOG(ERROR)<<"Cannot create socket.";
	}
	else
	MSettingSocket(FSock);
}
CTCP::CClientImpl::~CClientImpl()
{
	VLOG(2) << "Disstruct CClientImpl :" << this;
}
net_address IMPL::MGetInitParam() const
{
	return net_address(inet_ntoa(FAddr.sin_addr),ntohs(FAddr.sin_port));
}
bool IMPL::MClientConnect()
{
	VLOG(2)<<"Connect to "<<MGetInitParam();

	CRAII<CMutex> _mutex(FMutex);
	if (FTcp.MIsConnected())
	{
		VLOG(2) << "Has been connected";
		return true;
	}
	FSock.MClose();
	for(HANG_INIT;!FSock.MIsValid();HANG_CHECK)
	{
		VLOG(2) << "The socket is invalid";
		FSock = MNewSocket();
		MSettingSocket(FSock);
	}
	LOG_IF(DFATAL,!MIsClient())<<"Invalid address ";
	FTcp.FIsConnected = false;
	if (connect(FSock.MGet(), (struct sockaddr *) &(FAddr),
					sizeof(FAddr)) >=0)
	{
		VLOG(2) << "Connected to "<<MGetInitParam();
		FTcp.FIsConnected = true;

		net_address _addr=MGetInitParam();
		FTcp.MCall(EVENT_CONNECTED, &_addr);
		++FConnectionCount;
		return true;
	}
	VLOG(2)<<"Connection error "<<print_socket_error();
	return false;
}
void IMPL::sMCleanupConnection(void *aP)
{
	CSocket* _sock = reinterpret_cast<CSocket*>(aP);
	if (_sock && _sock->MIsValid())
	{
		_sock->MClose();
	}
}
ssize_t IMPL::MReceiveData(data_t* aBuf, const float aTime)
{
	VLOG(2) << "Start receive.";
	FIsReceive = true;
	int _recvd = 0;
	for (HANG_INIT; FTcp.FIsWorking && _recvd == 0; )
	{
		VLOG(2) << "Receive data to " << aBuf;
		if (FTcp.MIsConnected())
		{
			VLOG(2) << "Connected already";
			bool _is_error = false;
			_recvd = recv(FSock.MGet(), (char*) &_recvd, 1, MSG_PEEK);
#ifdef _WIN32
			//WTF? Fucking windows. If the received msg is more than the buffer size,
			//it generates the WSAEMSGSIZE error. But The buffer is always small as
			//it is 1 byte!!!!!
			if (_recvd >= 0
					|| (_recvd == SOCKET_ERROR && WSAEMSGSIZE == ::WSAGetLastError()))
#else
			if (_recvd >= 0)
#endif
			_recvd=MReadData(aBuf,FSock);

			if (_recvd <= 0)
			_is_error = true;

			if (_is_error)
			{
				VLOG(2) << "Error during receive " << print_socket_error();
				MCloseImpl();
			}
		}
		else
		{
			MClientConnect();
			HANG_CHECK;
		}
	}
	FIsReceive = false;
	VLOG(2) << "End receive.";
	if(_recvd>0)FDiagnostic.MRecv(_recvd);
	return _recvd;
}

CTCP::sent_state_t IMPL::MSend(const void* pData, size_t nSize)
{
	VLOG(2) << "Send " << pData <<" "<< nSize<<" bytes to " << FSock;
	if (!FTcp.MIsConnected())
	{
		VLOG(2) << "Is not connected.Receive= " << FIsReceive;
		if (FIsReceive) //if receive method does not work, can connect
		return sent_state_t(E_ERROR,0);
		else if (!MClientConnect())
		return sent_state_t(E_ERROR,0);
	}
	return MSendTo(FSock,pData,nSize,FDiagnostic,true,FAgainError);
}


bool IMPL::MIsClient() const
{
	return FAddr.sin_addr.s_addr != INADDR_ANY;
}

void IMPL::MCloseImpl()
{
	CRAII < CMutex > _mutex(FMutex);
	VLOG(2) << "Closing the socket";
	FTcp.FIsConnected = false;
	net_address _addr = MGetInitParam();
	FTcp.MCall(EVENT_DISCONNECTED, &_addr);
	FSock.MClose();
}

void IMPL::MClose()
{
	VLOG(2) << "Close client" << MGetInitParam() << " " << FSock;
	MCloseImpl();
}}

