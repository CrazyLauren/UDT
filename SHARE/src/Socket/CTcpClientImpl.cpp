// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTcpClientImpl.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <SHARE/Net.h>
#include <SHARE/Socket/CNetBase.h>
#include <SHARE/UType/CDenyCopying.h>
#include <SHARE/UType/CThread.h>
#include <SHARE/Socket/CTcpClientImpl.h>
#include <errno.h>
#include <SHARE/console.h>
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
#include "print_socket_error.h"
namespace NSHARE
{
#define IMPL CTCP::CClientImpl

CTCP::CClientImpl::CClientImpl(CTCP& aTcp,settings_t const& aSetting):
		FTcp(aTcp), //
		FSettings(aSetting),//
		FConnectMutex(CMutex::MUTEX_NORMAL),//
		FIsConnected(false)
{
	VLOG(2) << "Construct CClientImpl :" << this;
	memset(&FAddr, 0, sizeof(FAddr));
	FIsDoing=0;
	FIsReceiving=0;
}
CTCP::CClientImpl::~CClientImpl()
{
	VLOG(2) << "Waitfor Close Receive thread";
	CRAII<CMutex> _lock(FReceiveThreadMutex);
}
void IMPL::MSetAddress(net_address const& aAddr)
{
	FSettings.FServerAddress=aAddr;

	if (FSettings.FServerAddress.FIp.MGetConst().empty())
		FSettings.FServerAddress.FIp=net_address::LOCAL_HOST;
}
CTCP::settings_t const& IMPL::MGetInitParam() const
{
	return FSettings;
}
bool IMPL::MClientConnect()
{
	VLOG(2)<<"Connect to "<<MGetInitParam().FServerAddress;

	CRAII<CMutex> _mutex(FConnectMutex);
	if (FIsConnected)
	{
		VLOG(2) << "Has been connected";
		return true;
	}
	FSock.MClose();
	FSock = MNewSocket();

	if(FSock.MIsValid())
	{
		MSettingSocket(FSock,FSettings.FSocketSetting);

		LOG_IF(DFATAL,FAddr.sin_addr.s_addr == INADDR_ANY)<<"Invalid address ";
		FIsConnected = false;
		if (connect(FSock.MGet(), (struct sockaddr *) &(FAddr),
						sizeof(FAddr)) >=0)
		{
			VLOG(2) << "Connected to "<<MGetInitParam().FServerAddress;
			FIsConnected = true;

			net_address _addr=MGetInitParam().FServerAddress;
			FTcp.MCall(EVENT_CONNECTED, &_addr);
			return true;
		}
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
	CRAII<CMutex> _lock(FReceiveThreadMutex);

	FIsReceiving = 1;
	int _recvd = 0;
	for (; FIsDoing.MIsOne() && _recvd == 0; )
	{
		VLOG(2) << "Receive data to " << aBuf;
		if (FIsConnected)
		{
			VLOG(2) << "Connected already";
			bool _is_error = false;
			_recvd = recv(FSock.MGet(), (char*) &_recvd, 1, MSG_PEEK);
#ifdef _WIN32
			//WTF? Fucking windows. If the received msg is more than the buffer size,
			//it generates the WSAEMSGSIZE error. But The buffer is always small as
			//it is 1 byte!!!!!
			if (_recvd > 0
					|| (_recvd == SOCKET_ERROR && WSAEMSGSIZE == ::WSAGetLastError()))
#else
			if (_recvd > 0)
#endif
				_recvd=MReadData(aBuf,FSock,FDiagnostic);

			if (_recvd <= 0)
				_is_error = true;

			VLOG_IF(0,_recvd==0)<<"The connection is closed";

			if (_is_error)
			{
				VLOG(2) << "Error during receive " << print_socket_error();
				MDisconnect();
			}
		}
		else
		{
			MClientConnect();
		}
	}
	FIsReceiving = 0;
	VLOG(2) << "End receive.";
	return _recvd;
}
bool IMPL::MIsOpen() const
{
	return FIsDoing.MIsOne();
}
bool IMPL::MOpen()
{
	if (MIsOpen())
	{
		LOG(WARNING)<< "The Port has been opened already.";
		return false;
	}
	if(!FSettings.FServerAddress.MIsAddressValid())
	{
		LOG(DFATAL)<<"Invalid server address: "<<FSettings.FServerAddress;
		return false;
	}

	CNetBase::MSetAddress(FSettings.FServerAddress, &FAddr);
	FIsDoing = 1;
	return MIsOpen();
}

CTCP::sent_state_t IMPL::MSend(const void* pData, size_t nSize)
{
	VLOG(2) << "Send " << pData <<" "<< nSize<<" bytes to " << FSock;
	if (!FIsConnected)
	{
		VLOG(2) << "Is not connected.Receive= " << FIsReceiving.MIsOne();
		sent_state_t const _error(sent_state_t::E_ERROR,nSize);
		FDiagnostic.MSend(_error);
		return _error;
	}else
		return MSendTo(FSock,pData,nSize,FDiagnostic,true);
}
void IMPL::MDisconnect()
{
	CRAII < CMutex > _mutex(FConnectMutex);
	VLOG(2) << "Closing the socket";

	FSock.MClose();
	if(FIsConnected)
	{
		VLOG(2) << "Event Disconnect";
		FIsConnected = false;
		net_address _addr = MGetInitParam().FServerAddress;
		FTcp.MCall(EVENT_DISCONNECTED, &_addr);
	}
}

void IMPL::MClose()
{
	VLOG(2) << "Close client" << MGetInitParam().FServerAddress << " " << FSock;
	if(MIsOpen())
	{
		FIsDoing = 0;
#ifndef WIN32
		FSock.MShutdown();
#endif
		FSock.MClose();
	}
}}

