// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CNetBase.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
 
#include <Net.h>
#include <fcntl.h>
#include <Socket/CNetBase.h>

#if !defined(_WIN32)
#	include <sys/socket.h>
#	include <netinet/in.h>                     // sockaddr_in
#	include <arpa/inet.h>                      // htons, htonl
#	include <sys/sysctl.h>
#	include <sys/ioctl.h>
#	include <fcntl.h>
#		ifdef __QNX__
#			include <netinet/tcp_var.h>//only BSD
#		endif
#if 	defined( __linux__)
#			include <netinet/tcp.h>
#		endif
#else
#	include <ws2tcpip.h>
#		ifndef SIO_KEEPALIVE_VALS

struct tcp_keepalive
{
	ULONG onoff;
	ULONG keepalivetime;
	ULONG keepaliveinterval;
};
#		define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)
#	endif
#endif //#ifndef WIN32

#include <Socket/print_socket_error.h>

#ifdef _WIN32
typedef char raw_type_t;
#else
typedef void raw_type_t;
#endif
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

long const CNetBase::DEF_BUF_SIZE = 2 * std::numeric_limits<uint16_t>::max(); //2 max packet

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
CSocket CNetBase::MNewSocket(int aType,int aProtocol)
{
	return static_cast<CSocket::socket_t>(socket(AF_INET, aType,aProtocol));
}
net_address CNetBase::MGetLocalAddress(CSocket const& aSocket)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	socklen_t len = sizeof(addr);
	if (getsockname(aSocket.MGet(), (struct sockaddr*) &addr, &len) != 0)
		LOG(DFATAL)<<"Fetch of local address failed (getsockname())";

	return net_address(addr);
}
net_address CNetBase::MGetForeignAddress(CSocket const& aSocket)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	socklen_t len = sizeof(addr);
	if (getpeername(aSocket.MGet(), (struct sockaddr*) &addr, &len) != 0)
		LOG(DFATAL)<<"Fetch of local address failed (getsockname())";

	return net_address(addr);
}

int CNetBase::MSetLocalAddrAndPort(const CSocket& aSocket, const net_address& aAddr)
{
	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";

	sockaddr_in _addr;
	MSetAddress(aAddr, &_addr);

	VLOG(2)
			<< "try bind socket  " << aSocket << ", Port:" << _addr.sin_port;
	return bind(aSocket.MGet(), (sockaddr*) (((((&_addr))))), sizeof(_addr));
}
bool CNetBase::MSetMultiCastTTL(CSocket& aSocket,unsigned char aTTL)
{
	return setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEADDR, (raw_type_t *) &aTTL,
			sizeof(aTTL))==0;
}
bool CNetBase::MJoinGroup(const CSocket& aSocket, const net_address& aAddr)
{
	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";

	if (aAddr.FIp.MIs())
	{
		struct ip_mreq _mreq;

		_mreq.imr_multiaddr.s_addr = inet_addr(
				aAddr.FIp.MGetConst().c_str());
		_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if (setsockopt(aSocket.MGet(), IPPROTO_IP, IP_ADD_MEMBERSHIP,
				(raw_type_t *) &_mreq, sizeof(_mreq)) < 0)
		{
			LOG(DFATAL)<<"Multicast group join failed (setsockopt())";
			return false;
		}
	}else
	{
		LOG(DFATAL)<<"Invalid address";
		return false;
	}
	return true;
}
bool CNetBase::MLeaveGroup(const CSocket& aSocket, const net_address& aAddr)
{
	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";

	if (aAddr.FIp.MIs())
	{
		struct ip_mreq _mreq;

		_mreq.imr_multiaddr.s_addr = inet_addr(
				aAddr.FIp.MGetConst().c_str());
		_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if (setsockopt(aSocket.MGet(), IPPROTO_IP, IP_DROP_MEMBERSHIP,
				(raw_type_t *) &_mreq, sizeof(_mreq)) < 0)
		{
			LOG(DFATAL)<<"Multicast group leave failed (setsockopt())";
			return false;
		}
	}else
	{
		LOG(DFATAL)<<"Invalid address";
		return false;
	}
	return true;
}
bool CNetBase::MReUsePort(CSocket& aSocket)
{
	VLOG(2) << "Reuse port " << aSocket;

	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";
#ifdef _WIN32
	return MReUseAddr(aSocket);
#else
	int optval = 1;
	return setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEPORT, (raw_type_t*) &optval,
			sizeof optval)==0;
#endif
}

bool CNetBase::MReUseAddr(CSocket& aSocket)
{
	VLOG(2) << "Reuse address " << aSocket;
	LOG_IF(ERROR, !aSocket.MIsValid()) << "Invalid socket";
	int optval = 1;

#ifdef _WIN32
	return setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEADDR,
			(raw_type_t*)&optval, sizeof optval)==0;
#elif defined(SO_REUSEPORT) && !defined(__linux__)
	return setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEPORT,(raw_type_t*) &optval,
			sizeof optval)==0;
#else
	return setsockopt(aSocket.MGet(), SOL_SOCKET, SO_REUSEADDR,(raw_type_t*) &optval,
			sizeof optval)==0;
#endif
}
size_t CNetBase::MAvailable(CSocket const& aSocket)
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
	memset(aSa, 0, sizeof(*aSa));

	aSa->sin_family = AF_INET;
	if (aAddress.FIp.MIs() && !aAddress.FIp.MGetConst().empty() && aAddress.FIp.MGetConst()!=net_address::ALL_NETWORKS)
	{
		aSa->sin_addr.s_addr = inet_addr(aAddress.FIp.MGetConst().c_str());
		if (aSa->sin_addr.s_addr == INADDR_NONE && aAddress.FIp.MGetConst()!=net_address::BROAD_CAST_ADDR) return -1;
	}
	else
		aSa->sin_addr.s_addr = htonl(INADDR_ANY );

	aSa->sin_port = htons(aAddress.FPort);

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
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_KEEPALIVE, (const raw_type_t*) &on,
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
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_KEEPALIVE, (raw_type_t*) &on,
			sizeof(on));
	setsockopt(aSocket.MGet(), IPPROTO_TCP, TCP_KEEPALIVE, (raw_type_t*) &tval,
			sizeof(tval));
#elif defined(__linux__)
	int on = 1;
	setsockopt(aSocket.MGet(), SOL_SOCKET, SO_KEEPALIVE, (raw_type_t*) &on, sizeof(on));

#	ifdef TCP_KEEPCNT
	int keepcnt = 3;
	int keepidle = 1;
	int keepintvl = _sec;

	setsockopt(aSocket.MGet(), IPPROTO_TCP, TCP_KEEPCNT, &keepcnt,
			sizeof(keepcnt));
	setsockopt(aSocket.MGet(), IPPROTO_TCP, TCP_KEEPIDLE, &keepidle,
			sizeof(keepidle));
	setsockopt(aSocket.MGet(), IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl,
			sizeof(keepintvl));
#	else
#		warning  "Setting Time of Keep alive is not implemented"
#	endif //#	ifdef TCP_KEEPCNT

#else
#	error Target not supported
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
size_t CNetBase::MGetSendBufSize(CSocket& aSocket)
{
	CHECK(aSocket.MIsValid());
	long aValue=0;
	socklen_t _len=sizeof(aValue);
	VLOG(2)
			<< "Getting the TCP  normal buffer size for output  equal ";
	int _rval = getsockopt(aSocket.MGet(), SOL_SOCKET, SO_SNDBUF,
			( raw_type_t*) &aValue, &_len);
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
			(const raw_type_t*) &aValue, sizeof(aValue));
	VLOG_IF(1,_rval==0) << "The normal buffer sizes  for output " << aValue;
	return _rval == 0;
}
size_t CNetBase::MGetRecvBufSize(CSocket& aSocket)
{
	CHECK(aSocket.MIsValid());
	long aValue=0;
	socklen_t _len=sizeof(aValue);
	VLOG(2)
			<< "Getting the TCP  normal buffer size for output  equal ";
	int _rval = getsockopt(aSocket.MGet(), SOL_SOCKET, SO_RCVBUF,
			( raw_type_t*) &aValue, &_len);
	VLOG_IF(1,_rval==0) << "The  buffer sizes  for output " << aValue;
	return aValue;
}
bool CNetBase::MSettingRecvBufSize(CSocket& aSocket, long aValue)
{
	CHECK(aSocket.MIsValid());
	VLOG(2)
			<< "Setting the TCP  normal buffer size for input equal " << aValue;
	int _rval = setsockopt(aSocket.MGet(), SOL_SOCKET, SO_RCVBUF,
			(const raw_type_t*) &aValue, sizeof(aValue));
	VLOG_IF(1,_rval==0) << "The normal buffer sizes  for input " << aValue;
	return _rval == 0;
}

int CNetBase::MGetSendBufSize(CSocket const& aSocket)
{
	CHECK(aSocket.MIsValid());
	long _size = 0;
	socklen_t _len = sizeof(_size);
	int _rval = getsockopt(aSocket.MGet(), SOL_SOCKET, SO_SNDBUF,
			(raw_type_t*) &_size, &_len);

	VLOG_IF(1,_rval==0) << "The send buffer sizes  for output " << _size;
	if (_rval > 0)
	{
		CHECK(_size);
		return _size;
	}
	else
		return -1;
}
bool CNetBase::MMakeAsBroadcast(CSocket& aSocket)
{
	int _on = 1;
	VLOG(2)
			<< "Setting the broadcast addr ";

	int const _rval=setsockopt(aSocket.MGet(), SOL_SOCKET, SO_BROADCAST,
			reinterpret_cast<raw_type_t*>(&_on), sizeof _on);
	if(_rval!=0)
	{
		LOG(DFATAL)<<"Fail setting broadcast flag to "<<aSocket<<"."<<print_socket_error();
	}
	return _rval==0;

}
bool CNetBase::MSendMultiCastMsgByInterface(CSocket& aSocket,struct sockaddr_in const & aSa)
{
	struct sockaddr_in const* const addr4=(struct sockaddr_in const*) &aSa;
	struct sockaddr_in6 const* const addr6=(struct sockaddr_in6 const*) &aSa;

	int _rval = -1;

	VLOG(2)
			<< "Setting the Multicast addr ";

	  if (aSa.sin_family == AF_INET)
	  {
		  _rval=setsockopt(aSocket.MGet(),
                  IPPROTO_IP,
                  IP_MULTICAST_IF,
                  (raw_type_t*) &addr4->sin_addr,
                  sizeof(addr4->sin_addr));

	  } else if (aSa.sin_family == AF_INET6)
	  {
		  _rval=setsockopt(aSocket.MGet(),
	                   IPPROTO_IPV6,
	                   IPV6_MULTICAST_IF,
	                   (raw_type_t*) &addr6->sin6_scope_id,
	                   sizeof(addr6->sin6_scope_id));
	  }
	if(_rval!=0)
	{
		LOG(DFATAL)<<"Fail setting multicast to "<<aSocket<<"."<<print_socket_error();
	}
	return _rval==0;

}
bool CNetBase::MMakeNonBlocking(CSocket& aSocket)
{
	CHECK(aSocket.MIsValid());
#if defined(unix) || defined(__QNX__) ||defined( __linux__)
	int flags = fcntl(aSocket.MGet(), F_GETFL, 0);
	if (flags < 0)
	{
		LOG(DFATAL)<<"Fail getting status flags of "<<aSocket<<"."<<print_socket_error();
		return false;
	}
	if (fcntl(aSocket.MGet(), F_SETFL, flags | O_NONBLOCK) < 0)
	{
		LOG(DFATAL)<<"Fail setting NONBLOCK flag to "<<aSocket<<"."<<print_socket_error();
		return false;
	}
#elif defined(_WIN32)
	unsigned long _on = 1;
	if (ioctlsocket(aSocket.MGet(), FIONBIO, &_on) < 0)
	{
		LOG(DFATAL)<<"Fail setting NONBLOCK flag to "<<aSocket;
		return false;
	}
#else
#	error Target not supported
#endif

	VLOG(2) << aSocket << " is NON BLOCKING";
	return true;
}
} /* namespace NSHARE */
