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
 
#include <share/Net.h>
#include <fcntl.h>
#include <share/Socket/CNetBase.h>

#if !defined(HAVE_WINSOCK_H)
#	include <sys/socket.h>
#	include <netinet/in.h>                     // sockaddr_in
#	include <arpa/inet.h>                      // htons, htonl
#	include <sys/sysctl.h>
#	include <sys/ioctl.h>
#	include <fcntl.h>
#	ifdef __QNX__
#		include <netinet/tcp_var.h>//only BSD
#	endif
#	if defined( __linux__)
#		include <netinet/tcp.h>
#	endif
#elif defined(HAVE_WINSOCK_H)

#	include <ws2tcpip.h>
#
#	ifndef SIO_KEEPALIVE_VALS
struct tcp_keepalive
{
	ULONG onoff;
	ULONG keepalivetime;
	ULONG keepaliveinterval;
};
#	define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)

#	endif
#else
#	error The target is not supported
#endif //#ifndef WIN32

#include "print_socket_error.h"

#ifdef HAVE_WINSOCK_H
typedef char raw_type_t;
#else
typedef void raw_type_t;
#endif
namespace NSHARE
{
#ifdef HAVE_WINSOCK_H
extern bool init_wsa()
{
	WSADATA _wsaData;
	static bool _is=WSAStartup(0x202, &_wsaData)==0;
	return _is;
}
#endif

long const socket_setting_t::DEF_BUF_SIZE = 2 * std::numeric_limits<uint16_t>::max(); //2 max packet
unsigned const socket_setting_t::DEF_KEEPALIVE_TIME = 5;
unsigned const socket_setting_t::DEF_KEEPALIVE_COUNT = 3;


CNetBase::CNetBase()
{
#ifdef HAVE_WINSOCK_H
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
#ifdef HAVE_WINSOCK_H
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

#ifdef HAVE_WINSOCK_H
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

#ifdef HAVE_WINSOCK_H
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
void CNetBase::MSetAddress(net_address const& aAddress,
		struct sockaddr_in *aSa)
{
	memset(aSa, 0, sizeof(*aSa));

	aSa->sin_family = AF_INET;
	if (aAddress.FIp.MIs() && !aAddress.FIp.MGetConst().empty() && aAddress.FIp.MGetConst()!=net_address::ALL_NETWORKS)
	{
		aSa->sin_addr.s_addr = inet_addr(aAddress.FIp.MGetConst().c_str());
		if (aSa->sin_addr.s_addr == INADDR_NONE && aAddress.FIp.MGetConst()!=net_address::BROAD_CAST_ADDR)
			return;
	}
	else
		aSa->sin_addr.s_addr = htonl(INADDR_ANY );

	aSa->sin_port = htons(aAddress.FPort);
}
void CNetBase::MSettingSocket(CSocket& aSocket,socket_setting_t const& aSetting)
{
	VLOG(2) << "Set up " << aSocket;
	CHECK(aSocket.MIsValid());

	if(aSetting.FFlags.MGetFlag(socket_setting_t::E_REUSE_PORT))
			MReUsePort(aSocket);

	if(aSetting.FFlags.MGetFlag(socket_setting_t::E_SET_BUF_SIZE))
		MSettingBufSize(aSocket,aSetting);
	if(aSetting.FFlags.MGetFlag(socket_setting_t::E_KEEPALIVE))
		MSettingKeepAlive(aSocket,aSetting);
}
void CNetBase::MSettingKeepAlive(CSocket& aSocket,socket_setting_t const& aSetting)
{
	const int _sec = aSetting.FKeepAliveTime;
	VLOG(2)
			<< "Enable the periodic " << _sec
					<< "sec transmission of messages on a connected socket.(KEEPALIVE)"
					<< aSocket;

	DCHECK(aSocket.MIsValid());

#ifdef HAVE_WINSOCK_H
	struct tcp_keepalive _alive;
	_alive.onoff = 1;
	_alive.keepalivetime =_sec* 1000;
	_alive.keepaliveinterval = _sec * 1000;
	DWORD _ret;
	if(WSAIoctl(aSocket, SIO_KEEPALIVE_VALS, &_alive, sizeof(_alive), NULL, 0,
			&_ret, NULL, NULL)!=0)
	{
		DLOG(ERROR)<<"Failed to set SIO_KEEPALIVE_VALS on fd:"<<aSocket;
	}
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
	ival = aSetting.FKeepAliveCount;
	sysctl(mib, 4, NULL, NULL, &ival, sizeof(ival));
	mib[0] = CTL_NET;
	mib[1] = AF_INET;
	mib[2] = IPPROTO_TCP;
	mib[3] = TCPCTL_KEEPINTVL;
	ival = _sec*2;
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
	int keepcnt = aSetting.FKeepAliveCount;
	int keepidle = _sec;
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
void CNetBase::MSettingBufSize(CSocket& aSocket,socket_setting_t const& aSetting)
{
	CHECK(aSocket.MIsValid());
	VLOG(2)
			<< "Setting the TCP  normal buffer size for output and input equal "
					<< aSetting.FSendBufferSize;
	unsigned const _protocol_size = std::numeric_limits<uint16_t>::max();
	{
		unsigned _max = aSetting.FSendBufferSize;
		for (;
				!MSettingSendBufSize(aSocket, _max) && _max > _protocol_size;
				)
		{
			LOG(WARNING)<<"Invalid max send buffer = "<<_max;
			_max =(long)(_max/ 1.1);
		}
	}
	{
		//try maximal
		unsigned _max = aSetting.FRecvBufferSize;
		for (;
				!MSettingRecvBufSize(aSocket, _max) && _max > _protocol_size;
				)
		{
			LOG(WARNING)<<"Invalid max recv buffer = "<<_max;
			_max =(long)(_max/ 1.1);
		}
	}
}
size_t CNetBase::MGetSendBufSize(CSocket const& aSocket)
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
size_t CNetBase::MGetRecvBufSize(CSocket const& aSocket)
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
#elif defined(HAVE_WINSOCK_H)
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
const CText socket_setting_t::NAME="socket";
const CText socket_setting_t::KEY_SEND_BUFFER_SIZE="send_buf";
const CText socket_setting_t::KEY_RECV_BUFFER_SIZE="recv_buf";
const CText socket_setting_t::KEY_KEEPALIVE_TIME="ka_time";
const CText socket_setting_t::KEY_KEEP_ALIVE_COUNT="ka_count";

const CText socket_setting_t::ENABLE_KEEPALIVE="ka_on";
const CText socket_setting_t::SET_BUF_SIZE="buf_size_on";
const CText socket_setting_t::ENABLE_REUSE_PORT="reuse_on";

socket_setting_t::socket_setting_t():
		FSendBufferSize(DEF_BUF_SIZE),//
		FRecvBufferSize((long)(1.5*DEF_BUF_SIZE)),//
		FKeepAliveTime(DEF_KEEPALIVE_TIME),//
		FKeepAliveCount(DEF_KEEPALIVE_COUNT),//
		FFlags(E_DEFAULT_FLAGS)
{

}
socket_setting_t::socket_setting_t(NSHARE::CConfig const& aConf) :
				FSendBufferSize(DEF_BUF_SIZE),//
				FRecvBufferSize((long)(1.5*DEF_BUF_SIZE)),//
				FKeepAliveTime(DEF_KEEPALIVE_TIME),//
				FKeepAliveCount(DEF_KEEPALIVE_COUNT),//
				FFlags(E_DEFAULT_FLAGS)
{
	FFlags.MSetFlag(E_KEEPALIVE,
			aConf.MValue(ENABLE_KEEPALIVE,
					FFlags.MGetFlag(E_KEEPALIVE)));
	FFlags.MSetFlag(E_SET_BUF_SIZE,aConf.MValue(SET_BUF_SIZE, FFlags.MGetFlag(E_SET_BUF_SIZE)));
	FFlags.MSetFlag(E_REUSE_PORT,aConf.MValue(ENABLE_REUSE_PORT, FFlags.MGetFlag(E_REUSE_PORT)));

	aConf.MGetIfSet(KEY_SEND_BUFFER_SIZE, FSendBufferSize);
	aConf.MGetIfSet(KEY_RECV_BUFFER_SIZE, FRecvBufferSize);
	aConf.MGetIfSet(KEY_KEEPALIVE_TIME, FKeepAliveTime);
	aConf.MGetIfSet(KEY_KEEP_ALIVE_COUNT, FKeepAliveCount);

}
bool socket_setting_t::MIsValid() const
{
	return FSendBufferSize >= std::numeric_limits<uint16_t>::max()&&//
			FRecvBufferSize >= std::numeric_limits<uint16_t>::max()&&//
			FKeepAliveTime<2*60*60&&//
			FKeepAliveCount<100//
			;
}
CConfig socket_setting_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(ENABLE_KEEPALIVE, FFlags.MGetFlag(E_KEEPALIVE));
	_conf.MAdd(SET_BUF_SIZE, FFlags.MGetFlag(E_SET_BUF_SIZE));
	_conf.MAdd(ENABLE_REUSE_PORT, FFlags.MGetFlag(E_REUSE_PORT));

	_conf.MAdd(KEY_SEND_BUFFER_SIZE,FSendBufferSize);
	_conf.MAdd(KEY_RECV_BUFFER_SIZE,FRecvBufferSize);
	_conf.MAdd(KEY_KEEPALIVE_TIME,FKeepAliveTime);
	_conf.MAdd(KEY_KEEP_ALIVE_COUNT,FKeepAliveCount);

	return _conf;
}

} /* namespace NSHARE */
