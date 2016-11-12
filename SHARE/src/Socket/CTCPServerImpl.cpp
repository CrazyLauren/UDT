/*
 * CTCPServerImpl.cpp
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
#include <print_socket_error.h>
namespace NSHARE
{
#define IMPL CTCPServer::CImpl
static inline char const* print_error()
{
	return print_socket_error().c_str();
}
CTCPServer::CImpl::CImpl(CTCPServer * aThis) :
		FMutex(CMutex::MUTEX_NORMAL), FLoopBack(new loop_back_t/*(this)*/), FThis(
				aThis)
{
	VLOG(2)<<"Create impl for "<<aThis;
	FThread += NSHARE::CB_t(sMConnect, this);
	FIsWorking = false;
}
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
	MReUsePort(FHostSock);
	MSettingSocket(FHostSock);

	if (MBindSocket(FHostSock.MGet(), FHostAddr) < 0)
	{
		net_address _addr;
		FThis->MGetInitParam(&_addr);
		LOG(ERROR)<<"Binding "<<_addr<<" failed "<<print_error();

		/*		if (errno!=EADDRINUSE)
		 return false;*/
		FHostSock.MClose();
		return false;
	}
	if(!FHostAddr.sin_port)
	{
		sockaddr_in addr;
		socklen_t len = sizeof(addr);
		if (getsockname(FHostSock.MGet(), (struct sockaddr*) &addr, &len) == 0)
		{
			LOG(WARNING)<<"Tcp serve is used random port = "<<addr.sin_port;
			FHostAddr.sin_port=addr.sin_port;
		}
		else
		LOG(DFATAL)<<"Cannot open tcp server wuth random port";

	}
	return true;
}

bool IMPL::MOpenLoopSocket()
{
	bool _rval = FLoopBack->FLoop.MOpen();
	CHECK(_rval);
	FSelectSock.MAddSocket(FLoopBack->FLoop.MGetSocket());
	return _rval;
}

bool IMPL::MOpen()
{
	if (FThis->MIsOpen())
	{
		LOG(WARNING)<< "The Port has been opened already.";
		return false;
	}

	if (!MOpenHostSocket())
	return false;

	VLOG(2) << "Starting loop back";
	MOpenLoopSocket();
	CRAII<CMutex> _mutex(FMutex);
	VLOG(2) << "Synchronized starting";
	FIsWorking = true;
	FThread.MCreate();
	FCond.MBroadcast();

	VLOG(1) << "The server has been opened successfully.";
	return true;
}
int IMPL::sMCleanupMutex(void*, void*, void* aP)
{
	CRAII<CMutex> *_block = reinterpret_cast<CRAII<CMutex>*>(aP);
	_block->MUnlock();
	return 0;
}
void IMPL::MMakeNonBlocking(CSocket& aSocket)
{
	CHECK(aSocket.MIsValid());
#if defined(unix) || defined(__QNX__) ||defined( __linux__)
	int flags = fcntl(aSocket.MGet(), F_GETFL, 0);
	if (flags < 0)
	{
		LOG(DFATAL)<<"Fail getting status flags of "<<aSocket<<"."<<print_error();
		return;
	}
	if (fcntl(aSocket.MGet(), F_SETFL, flags | O_NONBLOCK) < 0)
	{
		LOG(DFATAL)<<"Fail setting NONBLOCK flag to "<<aSocket<<"."<<print_error();
		return;
	}
#elif defined(_WIN32)
	unsigned long _on = 1;
	if (ioctlsocket(aSocket.MGet(), FIONBIO, &_on) < 0)
	{
		LOG(DFATAL)<<"Fail setting NONBLOCK flag to "<<aSocket;
		return;
	}
#else
#	error Target not supported
#endif

	VLOG(2) << aSocket << " is NON BLOCKING";
}

void IMPL::MExpectConnection()
{
	VLOG(0) << "Not connected,expect signal ";
	CRAII<CMutex> _mutex(FMutex);

	FThread.MSetCleanUp(NSHARE::CB_t(sMCleanupMutex, &_mutex));
	for (HANG_INIT; !MCanReceive() && FIsWorking;HANG_CHECK)
	{
		VLOG(0) << "Wait for.";
		FCond.MTimedwait(&FMutex);
		LOG_IF(ERROR,!MCanReceive()) << "No clients.";
	}

	FThread.MSetCleanUp(NSHARE::CB_t());
}
void IMPL::MReserveMemory(data_t* aBuf,
		CSelectSocket::socks_t const& _to)
{
	//const size_t _avalable = MAvailable(aSock);
	size_t _available = 0;
	CSelectSocket::socks_t::const_iterator _it = _to.begin();
	for (; _it != _to.end(); ++_it)
		_available += CNetBase::MAvailable(*_it);

	VLOG(2) << "Available for reading " << _available << ", The capacity is "
						<< aBuf->capacity();
	_available += aBuf->size();
	if (_available > aBuf->capacity())
		aBuf->reserve(_available);
}
int IMPL::MReceiveFromAllSocket(data_t* aBuf,
		CSelectSocket::socks_t & _to, recvs_t* aFrom)
{
	CSelectSocket::socks_t::iterator _it = _to.begin();
	int _recvd = 0;
	for (; _it != _to.end(); ++_it)
	{
		CHECK_NE(*_it, FLoopBack->FLoop.MGetSocket());
		int _size = MReadData(aBuf, *_it);
		if(_size<=0)
		{
			MCloseClient(*_it);
		}
		else if ( aFrom)
		{
			_recvd += _size;
			CRAccsess _r = FClients.MGetRAccess();

			clients_fd_t::const_iterator _cit = _r->find(*_it);
			CHECK(_cit!=_r->end()) << "WTF?";


			_cit->second.FDiagnostic.MRecv(_size);

			recvs_t::value_type _val;
			client_t const _saddr(_cit->second);

			_val.FClient = _saddr; //TODO
//			_val.FBufBegin=aBuf->end()-_size;//vector can be allocated to other heap
			_val.FSize = _size;
			//CHECK(_val.FBufBegin==(aBuf->begin()+_recvd-_size))<<"WTF? The container is damage.";
			aFrom->push_back(_val);
		}
	}
	FDiagnostic.MRecv(_recvd);

	return _recvd > 0 ? _recvd : -1;
}

void IMPL::MCalculateDataBegin(recvs_t*aFrom, data_t*aBuf)
{
	recvs_t::reverse_iterator _it = aFrom->rbegin();
	data_t::iterator _data_it = aBuf->end();
	for (; _it != aFrom->rend(); ++_it)
	{
		//_data_it!=aBuf->begin()
		CHECK(_data_it>=aBuf->begin()) << "WTF? invalid data iterator";
		CHECK_GT(_it->FSize, 0);
		_data_it -= _it->FSize;
		_it->FBufBegin = _data_it;
	}
}
bool IMPL::MCloseClient(const net_address& aTo)
{
	VLOG(1) << "Close client " << aTo;
	LOG_IF(WARNING,!FThis->MIsClients()) << "No clients.";
	CSocket _tmp;
	{
		CRAccsess _r = FClients.MGetRAccess();
		for (clients_fd_t::const_iterator _it = _r->begin(); _it != _r->end();
				++_it)
			if (_it->second == aTo)
			{
				VLOG(2) << "The socket of " << (aTo) << " is " << _it->first;
				_tmp = _it->first;
				break;
			}
	}
	if (_tmp.MIsValid())
	{
		MCloseClient(_tmp);
		return true;
	}

	LOG(INFO)<< "The client "<< (aTo)<<" is not exist.";
	return false;
}
void IMPL::MCloseClient(CSocket& aSocket)
{
	VLOG(1) << "Close socket " << aSocket;
	LOG_IF(WARNING, !aSocket.MIsValid()) << "Socket is invalid";
	if (!aSocket.MIsValid())
		return;

	FSelectSock.MRemoveSocket(aSocket);

	LOG_IF(WARNING, !FLoopBack->FLoop.MGetSocket().MIsValid())
																	<< "Very interesting!, The Inside loop socket is closed, but  it is impossible";
	CHECK_NE(FLoopBack->FLoop.MGetSocket(), aSocket);
	client_t _p;
	{
		CWAccsess _r = FClients.MGetWAccess();
		_p = (*_r)[aSocket];
		_r->erase(aSocket);
	}

	FThis->MCall(EVENT_DISCONNECTED, &_p);
	aSocket.MClose();
	LOG_IF(ERROR,!MCanReceive() && FThis->MIsClients())
																<< "The socket and clients list is not equal.";
}
void IMPL::MCloseAllClients()
{
	clients_fd_t _fd;
	{
		CRAccsess _r = FClients.MGetRAccess();
		_fd=_r.MGet();
	}
	clients_fd_t::iterator _it = _fd.begin();
	for (; _it != _fd.end(); ++_it)
	{
		CSocket _tmp = _it->first;
		MCloseClient(_tmp);
	}
}
void IMPL::MClose()
{
	VLOG(1) << "Close all sockets ";
	FIsWorking = false;

	{
		CRAII<CMutex> _mutex(FMutex);
		FCond.MBroadcast();
	}
	MCloseAllClients();
	VLOG(2) << "Try close host socket." << FHostSock;
	if (FHostSock.MIsValid())
	{
		//#if defined(_WIN32)
		//		closesocket(FHostSock.MGet());
		//#else
		FHostSock.MClose();
		//#endif
	}
	if (FLoopBack->FLoop.MGetSocket().MIsValid())
		FSelectSock.MRemoveSocket(FLoopBack->FLoop.MGetSocket());
	FLoopBack->FLoop.MClose();

	usleep(1); //waitfor receive end;

	CHECK(!FHostSock.MIsValid());
	CHECK(!FSelectSock.MIsSetUp());
	CHECK(FSelectSock.MGetSockets().empty());

	VLOG(2) << "Wait for connect thread will be stopped.";
	if (FThread.MCancel())
		FThread.MJoin();
	else
		VLOG(2) << "Cancel failed";
	VLOG(2) << "Server stopped successfully.";
}
CTCPServer::sent_state_t IMPL::MSend(const void* pData, size_t nSize,
		const net_address&aTo)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to " << (aTo);
	LOG_IF(WARNING,!FThis->MIsClients()) << "No clients.";
	CRAccsess _r = FClients.MGetRAccess();
	for (clients_fd_t::const_iterator _it = _r->begin(); _it != _r->end();
			++_it)
		if (_it->second == aTo)
		{
			VLOG(2) << "The socket of " << (aTo) << " is " << _it->first;
			return MSendTo(*_it, pData, nSize);
		}
	LOG(ERROR)<< "The client "<< (aTo)<<" is not exist.";
	return sent_state_t(E_INVALID_VALUE,0);
}
CTCPServer::sent_state_t IMPL::MSend(const void* pData, size_t nSize,
		NSHARE::CConfig const& aTo)
{
	VLOG(1) << "Send " << pData << " size=" << nSize << " to " << aTo;
	LOG_IF(WARNING,!FThis->MIsClients()) << "No clients.";
	CRAccsess _r = FClients.MGetRAccess();
	net_address _addr(aTo);
	LOG_IF(DFATAL,!_addr.MIsValid()) << "Invalide type of smart_addr";
	if (!_addr.MIsValid())
		return sent_state_t(E_INVALID_VALUE,0);
	for (clients_fd_t::const_iterator _it = _r->begin(); _it != _r->end();
			++_it)
		if (_it->second.FAddr == _addr)
		{
			VLOG(2) << "The socket of " << aTo << " is " << _it->first;
			return MSendTo(*_it, pData, nSize);
		}
	LOG(ERROR)<< "The client "<< aTo<<" is not exist.";
	return sent_state_t(E_INVALID_VALUE,0);
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
	return _rval != 0?sent_state_t(E_SENDED,nSize):sent_state_t(E_ERROR,0);
}
CTCPServer::sent_state_t IMPL::MSendTo(clients_fd_t::value_type const& aVal, const void* pData,
		size_t nSize)
{
	CTCPServer::sent_state_t const _state(CTcpImplBase::MSendTo(aVal.first,pData,nSize,aVal.second.FDiagnostic,false,aVal.second.FAgainError));
	if(_state.FError==E_SENDED)
		FDiagnostic.MSend(nSize);
	return _state;
}
int IMPL::sMConnect(void*, void*, void* pData)
{
	reinterpret_cast<CImpl*>(pData)->MAccept();
	return 0;
}
#ifdef _WIN32
static const char* inet_ntop(int af, const void* src, char* dst, int cnt)
{

	struct sockaddr_in srcaddr;

	memset(&srcaddr, 0, sizeof(struct sockaddr_in));
	memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

	srcaddr.sin_family = af;
	if (WSAAddressToString((struct sockaddr*) &srcaddr,
			sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0)
	{
		DWORD rv = WSAGetLastError();
		LOG(ERROR)<<"WSAAddressToString() : "<< rv;
		return NULL;
	}
	return dst;
}
#endif
net_address IMPL::MGetAddress(
		const struct sockaddr_in& aAddr) const
{
	char buf[INET6_ADDRSTRLEN + 20];
	net_address _addr;
	_addr.port = ntohs(aAddr.sin_port);
	if (inet_ntop(aAddr.sin_family, &aAddr.sin_addr, buf, sizeof(buf)) != NULL)
		_addr.ip = buf;
	else
		LOG(ERROR)<< "Invalid address " << print_error();

	LOG_IF(DFATAL,
			(aAddr.sin_family != AF_INET && aAddr.sin_family != AF_INET6))
																							<< "Unknown type of Address network "
																							<< aAddr.sin_family;

	VLOG(2) << "Net addr " << _addr;
	return _addr;
}
IMPL::cl_t IMPL::MNewClient_t(
		net_address const & _net_addr) const
{
	cl_t _val;
	_val.FTime = ::time(NULL);
	_val.FAddr = _net_addr;
	return _val;
}
IMPL::cl_t IMPL::MAddNewClient(CSocket& _sock,
		struct sockaddr_in& _addr)
{
	//VLOG(2) << " Is loop " << _is_loop << ", after " << FIsLoopConnected;
	MMakeNonBlocking(_sock);
	MSettingSocket(_sock);
	FSelectSock.MAddSocket(_sock);

	net_address _net_addr = MGetAddress(_addr);
	VLOG(2) << "Address " << _net_addr;
	cl_t _ptr = MNewClient_t(_net_addr);
	{
		CWAccsess _r = FClients.MGetWAccess();
		clients_fd_t::iterator _it = _r->find(_sock);
		LOG_IF(ERROR, _it!= _r->end()) << "WTF? The socket " << _sock
												<< " is exist in the clients list. The socket is at "
												<< _it->second;
		_r->insert(std::make_pair(_sock, _ptr));
	}
	return _ptr;
}

void IMPL::MUnLockSelect()
{
	static const NSHARE::CBuffer _test(4,0);
	sent_state_t _val = FLoopBack->FLoop.MSend(_test);

	VLOG_IF(1,_val.MIs()) << "Select unlocked successfully. ";
	LOG_IF(WARNING,!_val.MIs()) << "Cannot unlock select. ";
}
void IMPL::MAccept()
{
	VLOG(2) << "Accepting";
	{
		CRAII<CMutex> _mutex(FMutex);
	}
	VLOG(2) << "After start synchronize.";
	if (listen(FHostSock, 1) < 0)
	{
		LOG(ERROR)<< "Listening error" << print_error();
		MClose();
		return;
	}
	LOG_IF(ERROR,MIsClient()) << "The client list is not empty";

	for (HANG_INIT; FIsWorking;HANG_CHECK)
	{
		struct sockaddr_in _addr;
		socklen_t addrlen = sizeof(_addr);

		VLOG(2) << "Accepting";
		CSocket _sock = accept(FHostSock, (struct sockaddr *) &(_addr),
		&addrlen);
		VLOG(2) << "New client: " << _sock << "; host " << FHostSock;
		if (!FHostSock.MIsValid())
		{
			VLOG(2) << "Ivalid host socket.May be server is stopped.";
			continue;
		}

		LOG_IF(ERROR,!_sock.MIsValid()) << "Invalid Socket ";

		if (_sock.MIsValid())
		{
			net_address _net_addr = MGetAddress(_addr);
			LOG(INFO)<< "New Client form " << _net_addr<<"; Socket: "<<_sock;

			client_t _ptr = MAddNewClient(_sock, _addr);
			MUnLockSelect();

			VLOG(2) << "Calling the Event connected...";
			FThis->MCall(EVENT_CONNECTED, &_ptr);
			VLOG(2) << "The 'event connected' has been called.";
		}

	}
}
ssize_t IMPL::MReceiveData(recvs_t*aFrom, data_t*aBuf,
		const float aTime)
{
	if (!FIsWorking)
	{
		LOG(ERROR)<< "The server is not working";
		return 0;
	}
	VLOG(2) << "Receive data to " << aBuf << ", max time " << aTime;
	FIsReceive = true;
	int _recvd = 0;
	for (HANG_INIT; FIsWorking && /*_recvd <= 0*/_recvd == 0; )
	{
		if (MCanReceive())
		{
			VLOG(2) << "It connected.";

			CSelectSocket::socks_t _to;
			int _val = FSelectSock.MWaitData(_to, aTime);
			VLOG(2) << "Wait status " << _val;

			if (_val == 0) //timeout
			{
				VLOG(2) << "Timeout";
				return 0;
			}
			else if (_val < 0)
			{
				LOG(ERROR) << "Unknown error:" << print_error();
				continue;
			}
			else if (_val > 0)
			{
				CSelectSocket::socks_t::iterator _it = _to.begin();
				CHECK(FLoopBack.MIs());
				for (; _it != _to.end(); ++_it)
				if(FLoopBack->FLoop.MGetSocket() == *_it) //looking for loopback socket
				{
					VLOG(1) << "It's internal msg";

					FLoopBack->FLoop.MReadAll();
					_to.erase(_it);
					break;
				}

				if(_to.empty())
				{
					VLOG(2)<<"Only internal MSG has been received.";
					continue;
				}
				{
					//the client can be closed
					CRAccsess _r = FClients.MGetRAccess();
					clients_fd_t::const_iterator const _end=_r->end();
					_it = _to.begin();
					for (; _it != _to.end(); )
					{
						if(_r->find(*_it) == _end)
						{
							VLOG(1) <<*_it<< " has been closed already.";
							_it=_to.erase(_it);
						}
						else ++_it;
					}
				}
				MReserveMemory(aBuf,_to);
				int _rval=MReceiveFromAllSocket(aBuf,_to,aFrom);
				_recvd=_rval;
				if(_rval>0)
				{
//					_recvd+=_rval;

					//vector can be allocated to other heap thus
					//FBufBegin is to calculated after data reading
					MCalculateDataBegin(aFrom,aBuf);
				}
				VLOG(1) << "Resultant:Reads " << _recvd << " bytes";
			}
		}
		else
		{
			MExpectConnection();
			HANG_CHECK;
		}
	}
	FIsReceive = false;
	VLOG(2) << "End Receive";
	return _recvd;
}
size_t IMPL::MAvailable() const
{
	size_t _rval = 0;
	CRAccsess _r = FClients.MGetRAccess();
	for (clients_fd_t::const_iterator _it = _r->begin(); _it != _r->end();
			++_it)
		_rval += CNetBase::MAvailable(_it->first);
	VLOG(2) << _rval << " bytes available for reading from all clients";
	return _rval;
}

}


