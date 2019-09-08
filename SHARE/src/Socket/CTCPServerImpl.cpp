// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTCPServerImpl.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <Net.h>
#include <Socket/CNetBase.h>
#include <Socket/CLoopBack.h>
#include <UType/CDenyCopying.h>
#include <UType/CThread.h>
#include <Socket/CTCPServerImpl.h>
#include <errno.h>
#if !defined(_WIN32)
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>                     // sockaddr_in
#include <arpa/inet.h>                      // htons, htonl
#include <sys/sysctl.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#	ifdef __QNX__
#include <netinet/tcp_var.h>//only BSD
#	endif
#else
#include <ws2tcpip.h>

#endif //#ifndef WIN32
#include <string.h>
#include <console.h>
#include <Socket/print_socket_error.h>
namespace NSHARE
{
#define IMPL CTCPServer::CImpl
static inline char const* print_error()
{
	return print_socket_error().c_str();
}
CTCPServer::CImpl::CImpl(CTCPServer * aThis) :
		FConnectMutex(CMutex::MUTEX_NORMAL),//
		FThis(aThis)
{
	VLOG(2) << "Create impl for " << aThis;
	FConnectThread += NSHARE::CB_t(sMListenThread, this);
	FIsAccepting = 0;
}
/** Open TCP server port
 *
 * @return true if is open
 */
bool IMPL::MOpenHostSocket()
{
	LOG_IF(WARNING, FHostSock.MIsValid())
	<< "Host socket is valid. Leak socket can occur!";
	FHostSock = MNewSocket();
	if (!FHostSock.MIsValid())
	{
		LOG(ERROR)<< FHostSock << print_error();
		return false;
	}
	MSettingSocket(FHostSock,MGetSetting().FSocketSetting);

	if (MSetLocalAddrAndPort(FHostSock.MGet(), FSettings.FServerAddress) < 0)
	{
		LOG(ERROR)<<"Binding "<<FSettings.FServerAddress<<" failed "<<print_error();

		FHostSock.MClose();
		return false;
	}
	if(FSettings.FServerAddress.FPort==net_address::RANDOM_NETWORK_PORT)
	{
		FSettings.FServerAddress=MGetLocalAddress(FHostSock.MGet());
		LOG(WARNING)<<"Tcp serve is used random port = "<<FSettings.FServerAddress.FPort;
	}
	return true;
}

bool IMPL::MIsOpen() const
{
	return FHostSock.MIsValid() && CTCPSelectReceiver::MIsOpen();//fixme
}

/** Starts listening for incoming client connections.
 *
 */
void IMPL::MStartConnectionThread()
{
	CRAII<CMutex> _mutex(FConnectMutex);
	FIsAccepting=1;
	FConnectThread.MCreate();
//	FConnectCondVar.MBroadcast();
}
bool IMPL::MOpen()
{
	if (MIsOpen())
	{
		LOG(WARNING)<< "The Port has been opened already.";
		return false;
	}

	if (!MOpenHostSocket())
		return false;

	VLOG(2) << "Starting loop back";
	MOpenSelectReceiver();

	MStartConnectionThread();
	return true;
}
/** Updates info about from whom the data has been received
 *
 * @param aFrom [out] Fill info to
 * @param aBuf [in] The received data (doesn't change, non-const as return iterator to data)
 * @param aFirstByte A index of first byte of received data
 * @param aSockets The sockets from whom the data has been received
 * @return true if no error
 */
bool IMPL::MUpdateClientInfo(recvs_t*aFrom, data_t& aBuf,size_t aFirstByte,read_data_from_t const& aSockets)
{
	read_data_from_t::const_iterator _it(aSockets.begin()),_it_end(aSockets.end());

	data_t::iterator _data_it = aBuf.begin()+aFirstByte;
	CRAccsess _r = FClients.MGetRAccess();

	for(;//
			_it!=_it_end//
					&& _data_it!=aBuf.end();//
			_data_it+=_it->FSize,//
					++_it//
			)
	{
		DCHECK_GT(_it->FSize, 0);
		DCHECK(_data_it<=aBuf.end()) << "WTF? invalid data iterator";

		clients_fd_t::const_iterator _cit = _r->find(_it->FSocket);

		DLOG_IF(FATAL,_cit==_r->end()) << "WTF? Cannot find socket:"<<_it->FSocket;

		if(_cit!=_r->end())
		{
			_cit->second.FDiagnostic+=_it->FDiagnostic;

			if(aFrom)
			{
				recvs_t::value_type _val;
				_val.FClient = _cit->second;
				_val.FBufBegin=_data_it;
				_val.FSize = _it->FSize;
				aFrom->push_back(_val);
			}
		}else
			return false;
	}
	return true;
}
ssize_t IMPL::MReceiveData(recvs_t*aFrom, data_t*aBuf,
		const float aTime)
{
	const size_t _size=aBuf->size();

	read_data_from_t _info;
	ssize_t const _recvd=CTCPSelectReceiver::MReceiveData(&_info,aBuf,aTime);
	VLOG(3)<<"Receive:"<<_info;

	if(!aBuf->empty() //
			&& !MUpdateClientInfo(aFrom,*aBuf,_size,_info))
		{
			aBuf->resize(_size);
			return -1;
		}
	return _recvd;
}
bool IMPL::MCloseClient(const net_address& aTo)
{
	VLOG(1) << "Close client " << aTo;
	LOG_IF(WARNING,!MIsClients()) << "No clients.";
	CSocket _tmp;
	{
		CRAccsess _r = FClients.MGetRAccess();
		client_by_ip_t const& _by_ip = FClientsByIP.MGetRAccess().MGet();
		client_by_ip_t::const_iterator _jt=_by_ip.find(aTo);

		if(_jt!=_by_ip.end())
		{
			_tmp=_jt->second->first;
		}
#ifndef NDEBUG
		else
		{
			for (clients_fd_t::const_iterator _it = _r->begin(); _it != _r->end();
					++_it)
			CHECK (!(_it->second == aTo));
		}
#endif

	}
	if (_tmp.MIsValid())
	{
		MRemoveClient(_tmp);
		return true;
	}

	LOG(INFO)<< "The client "<< (aTo)<<" is not exist.";
	return false;
}
CTCPServer::client_t IMPL::MRemoveSocket(CSocket const& aSocket)
{
	client_t _fd;

	{
		clients_fd_t& _r = FClients.MGetWAccess().MGet();

		clients_fd_t::iterator _it = _r.find(aSocket);
		if(_it!=_r.end())
		{
			_fd=_it->second;

			VLOG(2)<<"Remove client "<<_fd.MSerialize().MToJSON(true);

			{
				client_by_ip_t& _by_ip = FClientsByIP.MGetWAccess().MGet();
				if(_by_ip.erase(_fd.FAddr)==0)
				{
					DLOG(FATAL)<<"No address for socket :"<<aSocket<<" addr "<<_fd;
				}

			}
			_r.erase(_it);
		}
	}
	return _fd;
}
/** Handles removing socket from select list
 *
 * @param aSocket
 */
void IMPL::MClientIsRemoved(CSocket& aSocket)
{
	client_t _fd(MRemoveSocket(aSocket));

	aSocket.MClose();

	if(_fd.MIsValid())
	{
		VLOG(2) << "Calling the Event disconnected...";
		FThis->MCall(EVENT_DISCONNECTED, &_fd);
		VLOG(2) << "The 'event disconnected' has been called.";
		FInfoAboutOldClient.push_back(_fd);
	}
}
/** Stop accept thread
 *
 */
void IMPL::MStopConnectionThread()
{
	CRAII<CMutex> _mutex(FConnectMutex);
	FIsAccepting=0;
//	FConnectCondVar.MBroadcast();
}
/** Wait for accept thread stopped
 *
 */
void IMPL::MWaitForConnectionThread()
{
	VLOG(2) << "Wait for connect thread will be stopped.";

	if (FConnectThread.MCancel())
		FConnectThread.MJoin();
	else
		VLOG(2) << "Cancel failed";
}
/** Close host
 *
 */
void IMPL::MCloseHostSocket()
{
	VLOG(2) << "Try close host socket." << FHostSock;
	if (FHostSock.MIsValid())
	{
		FHostSock.MClose();
	}
	DCHECK(!FHostSock.MIsValid());
}
void IMPL::MClose()
{
	VLOG(1) << "Stopping server ";

	MStopConnectionThread();

	CTCPSelectReceiver::MClose();

	MCloseHostSocket();

	MWaitForConnectionThread();
	VLOG(2) << "Server stopped successfully.";
}
CTCPServer::sent_state_t IMPL::MSend(const void* pData, size_t nSize,
		const net_address&aTo)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to " << (aTo);
	DLOG_IF(WARNING,!MIsClients()) << "No clients.";

	{
		client_by_ip_t const& _by_ip = FClientsByIP.MGetRAccess().MGet();
		client_by_ip_t::const_iterator _jt=_by_ip.find(aTo);
		if(_jt!=_by_ip.end())
		{
			return MSendTo(*_jt->second, pData, nSize);///todo проверить на взаимную блокировку
		}
	}
	LOG(ERROR)<< "The client "<< (aTo)<<" is not exist.";
	return sent_state_t(sent_state_t::E_INVALID_VALUE,0);
}
CTCPServer::sent_state_t IMPL::MSend(const void* pData, size_t nSize,
		NSHARE::CConfig const& aTo)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to " << aTo;

	net_address  _addr(aTo);
	if(aTo.MKey()==net_address::NAME)
	{
		_addr=net_address(aTo);
		LOG_IF(DFATAL,!_addr.MIsValid()) << "Invalid address:"<<aTo.MToJSON(true);
	}else if(aTo.MKey()==CTCPServer::client_t::NAME)
	{
		_addr=net_address(aTo.MChild(net_address::NAME));
		LOG_IF(DFATAL,!_addr.MIsValid()) << "Invalid address:"<<aTo.MToJSON(true);
	}else
	{
		LOG(DFATAL)<<"Cannot deserialize net address";
	}

	if (!_addr.MIsValid())
		return sent_state_t(sent_state_t::E_INVALID_VALUE,0);

	return MSend(pData,nSize,_addr);
}
CTCPServer::sent_state_t IMPL::MSend(const void* pData, size_t nSize)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to all clients";
	int _rval = 0;
	CRAccsess _r = FClients.MGetRAccess();
	for (clients_fd_t::const_iterator _it = _r->begin(); _it != _r->end();
			++_it)
	{
		VLOG(2) << "Send to  " << _it->second << " by " << _it->first;
		_rval *= MSendTo(*_it, pData, nSize).MIs()?1:0;
	}
	return _rval != 0?sent_state_t(sent_state_t::E_SENDED,nSize):sent_state_t(sent_state_t::E_ERROR,0);
}
CTCPServer::sent_state_t IMPL::MSendTo(clients_fd_t::value_type const& aVal, const void* pData,
		size_t nSize)
{
	return CTcpImplBase::MSendTo(aVal.first,pData,nSize,aVal.second.FDiagnostic,false);
}
/** Handles adding socket to select list
 *
 * @param aSocket
 */
void IMPL::MClientIsAdded(CSocket& aSocket)
{
	client_t _fd(MGetSocketInfo(aSocket));

	if(_fd.MIsValid())
	{
		VLOG(2) << "Calling the Event connected...";
		FThis->MCall(EVENT_CONNECTED, &_fd);
		VLOG(2) << "The 'event connected' has been called.";
	}
}

/** Returns info about socket
 *
 * @param aSocket Info about socket
 * @return Info about socket
 */
CTCPServer::client_t IMPL::MGetSocketInfo(CSocket const& aSocket) const
{
	clients_fd_t const& _r = FClients.MGetRAccess().MGet();
	clients_fd_t::const_iterator _it = _r.find(aSocket);

	if(_it!=_r.end())
		return _it->second;

	return client_t();
}
bool IMPL::MAddSocket(CSocket const& aSocket,client_t const& aClient)
{
	clients_fd_t& _r = FClients.MGetWAccess().MGet();
	clients_fd_t::iterator _it = _r.find(aSocket);

	if(_it!= _r.end())
	{
		LOG(ERROR) << "WTF? The socket " << aSocket<< " is exist in the clients list. The socket is at "<< _it->second;
		return false;
	}
	clients_fd_t::const_iterator const _gt=
			_r.insert(_it,clients_fd_t::value_type(aSocket, aClient));
	{
		client_by_ip_t& _by_ip = FClientsByIP.MGetWAccess().MGet();
		_by_ip[aClient.FAddr]=_gt;
	}
	return true;
}
bool IMPL::MAddClient(CSocket& aSocket,const net_address& aAddress)
{
	client_t const _fd(aAddress);

	MSettingSocket(aSocket,MGetSetting().FSocketSetting);
	MMakeNonBlocking(aSocket);

	if(MAddSocket(aSocket,_fd))
	{
		if(CTCPSelectReceiver::MAddClient(aSocket))
		{
			VLOG(1)<<"Add new client "<<aAddress<<" socket "<<aSocket;
		}
		else
		{
			DLOG(FATAL)<<"Cannot add client "<<aAddress;

			MRemoveSocket(aSocket);
			return false;
		}

		return true;
	}
	else
	{
		DLOG(FATAL)<<"Cannot add client";
		return false;
	}

}
eCBRval IMPL::sMListenThread(void*, void*, void* pData)
{
	reinterpret_cast<CImpl*>(pData)->MListen();
	return E_CB_SAFE_IT;
}

/** Listen the socket
 *
 * @return true if no error
 */
bool IMPL::MListenSocket()
{
	if (listen(FHostSock, FSettings.FListenQueue) < 0)
	{
		LOG(ERROR) << "Listening error" << print_error();
		return false;
	}
	return true;
}

/** Listener a new connection
 *
 */
void IMPL::MListen()
{
	VLOG(2) << "Accepting";
	{
		CRAII<CMutex> _mutex(FConnectMutex);// wait for opened
	}
	VLOG(2) << "After start synchronize.";

	if(!MListenSocket())
	{
		MClose();
		DCHECK(false);
		return;
	}

	for (HANG_INIT; FIsAccepting.MIsOne();HANG_CHECK)
	{
		struct sockaddr_in _addr;
		socklen_t addrlen = sizeof(_addr);

		VLOG(2) << "Accepting";
		CSocket _sock (static_cast<CSocket::socket_t>(accept(FHostSock, (struct sockaddr *) &(_addr),
								&addrlen)));
		VLOG(2) << "New client: " << _sock << "; host " << FHostSock;

		CRAII<CMutex> _mutex(FConnectMutex);
		if (FIsAccepting.MIsOne()) ///< As accept is lock thread checking the current state
		{

			LOG_IF(ERROR,!_sock.MIsValid()) << "Invalid Socket ";

			if (_sock.MIsValid())
			{
				net_address const _net_addr(_addr);

				LOG(INFO)<< "New Client form " << _net_addr<<"; Socket: "<<_sock;
				MAddClient(_sock, _addr);
			}

		}else
			VLOG(2) << "May be server is stopped.";
	}
}

bool IMPL::MIsClients() const
{
	return !FClients.MGetRAccess().MGet().empty();
}
bool IMPL::MIsClient(const net_address& aIP) const
{
	client_by_ip_t const& _by_ip = FClientsByIP.MGetRAccess().MGet();
	client_by_ip_t::const_iterator _jt=_by_ip.find(aIP);
	return _jt!=_by_ip.end();
}
bool IMPL::MSetAddress(const net_address& aParam)
{
	if(MIsOpen())
		return false;///FIXME change address
	FSettings.FServerAddress=aParam;
	return true;
}
diagnostic_io_t const& IMPL::MGetDiagnosticState() const
{
	diagnostic_io_t _rval;
	{
		CRAccsess const _r = FClients.MGetRAccess();

		clients_fd_t::const_iterator _it = _r->begin(),_it_end(_r->end());
		for (;_it!=_it_end;++_it)
		{
			_rval+=_it->second.FDiagnostic;
		}
	}
	{
		list_of_clients::const_iterator _it=FInfoAboutOldClient.begin(),
				_it_end=FInfoAboutOldClient.end();
		for(;_it!=_it_end;++_it)
			_rval+=_it->FDiagnostic;
	}
	FCurrentDiagnostic=_rval;
	return FCurrentDiagnostic;
}

CTCPServer::list_of_clients IMPL::MGetConnectedClientInfo() const
{
	list_of_clients _rval;
	CRAccsess const _r = FClients.MGetRAccess();

	clients_fd_t::const_iterator _it = _r->begin(), _it_end(_r->end());
	for (;_it!=_it_end;++_it)
	{
		_rval.push_back(_it->second);
	}
	return _rval;
}
CTCPServer::list_of_clients const&  IMPL::MGetDisconnectedClientInfo() const
{
	return FInfoAboutOldClient;
}
}

