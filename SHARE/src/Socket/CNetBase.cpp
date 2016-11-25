/*
 * CNetBase.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 13.11.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
 
#include <Net.h>
#include <fcntl.h>

#if !defined(_WIN32)
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

#	ifndef SIO_KEEPALIVE_VALS
//FIXME
struct tcp_keepalive
{
	ULONG onoff;
	ULONG keepalivetime;
	ULONG keepaliveinterval;
};
#		define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)
#endif
#endif //#ifndef WIN32
#include <Socket/print_socket_error.h>
namespace NSHARE
{
#ifdef _WIN32
extern bool init_wsa()
{
	WSADATA _wsaData;
	static bool _is=WSAStartup(0x202, &_wsaData)==0;
	return _is;
}
#endif
CNetBase::CNetBase()
{
#ifdef _WIN32
	if (!init_wsa())
	{
		LOG(FATAL)<<"Cannot init wsa";
		throw std::runtime_error("cannot init wsa");
	}
#endif
}
long const CNetBase::DEF_BUF_SIZE = 2 * std::numeric_limits<uint16_t>::max(); //2 max packet
int CNetBase::MBindSocket(const CSocket& aSocket,
		const struct sockaddr_in& aAddr)
{
	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";
	VLOG(2)
			<< "try bind socket  " << aSocket << ", Port:" << aAddr.sin_port
					<< ":" << this;
	return bind(aSocket.MGet(), (sockaddr*) (((((&aAddr))))), sizeof(aAddr));
}
void CNetBase::MReUsePort(CSocket& aSocket)
{
	VLOG(2) << "Reuse port " << aSocket;

	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";
#ifdef _WIN32
	MReUseAddr(aSocket);
#else
	int optval = 1;
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEPORT, &optval,
			sizeof optval);
#endif
}

void CNetBase::MReUseAddr(CSocket& aSocket)
{
	VLOG(2) << "Reuse address " << aSocket;
	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";
	int optval = 1;

#ifdef _WIN32
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEADDR,
			reinterpret_cast<char*>(&optval), sizeof optval);
#else
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEADDR, &optval,
			sizeof optval);
#endif
}
size_t CNetBase::MAvailable(CSocket const& aSocket) const
{

#ifdef _WIN32
	unsigned long count_buf = 0;
	int _val = ioctlsocket(aSocket.MGet(), FIONREAD, &count_buf);
	LOG_IF(ERROR,_val<0) << "ioctlsocket error";
#else
	int count_buf = 0;
	int _val = ioctl(aSocket.MGet(), FIONREAD, &count_buf);
	LOG_IF(ERROR,_val<0) << print_socket_error();
	(void) _val;
	CHECK_GE(count_buf, 0);
#endif
	VLOG(2) << count_buf << " bytes available for reading from " << aSocket;
	return count_buf;
}
int CNetBase::MSetAddress(net_address const& aAddress,
		struct sockaddr_in *aSa)
{

	aSa->sin_family = AF_INET;
	if (!aAddress.ip.MGetConst().empty())
	{
		aSa->sin_addr.s_addr = inet_addr(aAddress.ip.MGetConst().c_str());
		if (aSa->sin_addr.s_addr == INADDR_NONE ) return -1;
	}
	else
		aSa->sin_addr.s_addr = htonl(INADDR_ANY );

	aSa->sin_port = htons(aAddress.port);

	return 0;
}
void CNetBase::MSettingSocket(CSocket& aSocket)
{
	VLOG(2) << "Set up " << aSocket;
	CHECK(aSocket.MIsValid());

	MSettingBufSize(aSocket);
	MSettingKeepAlive(aSocket);
	/*
	 #if defined( __linux__)
	 int on = 1;
	 setsockopt(aSocket.MGet(), SOL_SOCKET, SO_NOSIGPIPE, (void*) &on,
	 sizeof(on));
	 #endif
	 */
}
void CNetBase::MSettingKeepAlive(CSocket& aSocket)
{
	static const int _sec = 5;
	VLOG(2)
			<< "Enable the periodic " << _sec
					<< "sec transmission of messages on a connected socket.(KEEPALIVE)"
					<< aSocket;

	CHECK(aSocket.MIsValid());

#ifdef _WIN32
	struct tcp_keepalive _alive;
	_alive.onoff = 1;
	_alive.keepalivetime = 1000;
	_alive.keepaliveinterval = _sec * 1000;
	DWORD _ret;
	WSAIoctl(aSocket, SIO_KEEPALIVE_VALS, &_alive, sizeof(_alive), NULL, 0,
			&_ret, NULL, NULL);
	int on = 1;
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_KEEPALIVE, (const char*) &on,
			sizeof(on));
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__QNX__)
	int on = 1;
	struct timeval tval;
	int mib[4];
	int ival;
	mib[0] = CTL_NET;
	mib[1] = AF_INET;
	mib[2] = IPPROTO_TCP;
	mib[3] = TCPCTL_KEEPCNT;
	ival = 5;
	sysctl(mib, 4, NULL, NULL, &ival, sizeof(ival));
	mib[0] = CTL_NET;
	mib[1] = AF_INET;
	mib[2] = IPPROTO_TCP;
	mib[3] = TCPCTL_KEEPINTVL;
	ival = 1;
	sysctl(mib, 4, NULL, NULL, &ival, sizeof(ival));
	memset(&tval, 0, sizeof(tval));
	tval.tv_sec = _sec;
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_KEEPALIVE, (void*) &on,
			sizeof(on));
	setsockopt(aSocket.MGet(), IPPROTO_TCP, TCP_KEEPALIVE, (void*) &tval,
			sizeof(tval));
#elif defined( __linux__)
	int on = 1;
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_KEEPALIVE, (void*) &on, sizeof(on));
	//TODO sysctl keepalive time
#warning  "Setting Time of Keep alive is not implemented"
#else
#error Target not supported
#endif
}
void CNetBase::MSettingBufSize(CSocket& aSocket)
{
	CHECK(aSocket.MIsValid());
	VLOG(2)
			<< "Setting the TCP  normal buffer size for output and input equal "
					<< DEF_BUF_SIZE;
	int const _protocol_size = std::numeric_limits<uint16_t>::max();
	{
		long _max = DEF_BUF_SIZE; //128 kb
		for (HANG_INIT;
				!MSettingSendBufSize(aSocket, _max) && _max > _protocol_size;
				HANG_CHECK)
		{
			LOG(WARNING)<<"Invalid max send buffer = "<<_max;
			_max =(long)(_max/ 1.1);
		}
	}
	{
		//try maximal
		long _max = (long)(1.5 * DEF_BUF_SIZE); //192 kb
		for (HANG_INIT;
				!MSettingRecvBufSize(aSocket, _max) && _max > _protocol_size;
				HANG_CHECK)
		{
			LOG(WARNING)<<"Invalid max recv buffer = "<<_max;
			_max =(long)(_max/ 1.1);
		}
	}
}
size_t CNetBase::MGetSendBufSize(CSocket& aSocket) const
{
	CHECK(aSocket.MIsValid());
	long aValue=0;
	socklen_t _len=sizeof(aValue);
	VLOG(2)
			<< "Getting the TCP  normal buffer size for output  equal ";
	int _rval = getsockopt(aSocket.MGet(), SOL_SOCKET, SO_SNDBUF,
			( char*) &aValue, &_len);
	VLOG_IF(1,_rval==0) << "The  buffer sizes  for output " << aValue;
	return aValue;
}
bool CNetBase::MSettingSendBufSize(CSocket& aSocket, long aValue)
{
	CHECK(aSocket.MIsValid());
	VLOG(2)
			<< "Setting the TCP  normal buffer size for output  equal "
					<< aValue;
	int _rval = setsockopt(aSocket.MGet(), SOL_SOCKET, SO_SNDBUF,
			(const char*) &aValue, sizeof(aValue));
	VLOG_IF(1,_rval==0) << "The normal buffer sizes  for output " << aValue;
	return _rval == 0;
}
size_t CNetBase::MGetRecvBufSize(CSocket& aSocket) const
{
	CHECK(aSocket.MIsValid());
	long aValue=0;
	socklen_t _len=sizeof(aValue);
	VLOG(2)
			<< "Getting the TCP  normal buffer size for output  equal ";
	int _rval = getsockopt(aSocket.MGet(), SOL_SOCKET, SO_RCVBUF,
			( char*) &aValue, &_len);
	VLOG_IF(1,_rval==0) << "The  buffer sizes  for output " << aValue;
	return aValue;
}
bool CNetBase::MSettingRecvBufSize(CSocket& aSocket, long aValue)
{
	CHECK(aSocket.MIsValid());
	VLOG(2)
			<< "Setting the TCP  normal buffer size for input equal " << aValue;
	int _rval = setsockopt(aSocket.MGet(), SOL_SOCKET, SO_RCVBUF,
			(const char*) &aValue, sizeof(aValue));
	VLOG_IF(1,_rval==0) << "The normal buffer sizes  for input " << aValue;
	return _rval == 0;
}

int CNetBase::MGetSendBufSize(CSocket const& aSocket) const
{
	CHECK(aSocket.MIsValid());
	long _size = 0;
	socklen_t _len = sizeof(_size);
	int _rval = getsockopt(aSocket.MGet(), SOL_SOCKET, SO_SNDBUF,
			(char*) &_size, &_len);

	VLOG_IF(1,_rval==0) << "The send buffer sizes  for output " << _size;
	if (_rval > 0)
	{
		CHECK(_size);
		return _size;
	}
	else
		return -1;
}
void CNetBase::MMakeNonBlocking(CSocket& aSocket)
{
	CHECK(aSocket.MIsValid());
#if defined(unix) || defined(__QNX__) ||defined( __linux__)
	int flags = fcntl(aSocket.MGet(), F_GETFL, 0);
	if (flags < 0)
	{
		LOG(DFATAL)<<"Fail getting status flags of "<<aSocket<<"."<<print_socket_error();
		return;
	}
	if (fcntl(aSocket.MGet(), F_SETFL, flags | O_NONBLOCK) < 0)
	{
		LOG(DFATAL)<<"Fail setting NONBLOCK flag to "<<aSocket<<"."<<print_socket_error();
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
} /* namespace NSHARE */
