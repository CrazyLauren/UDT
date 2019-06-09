// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * net_function.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.03.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#include <deftype>
#include <Net.h>
//#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <limits.h>


#ifdef _WIN32
#	include <ws2tcpip.h>
#else
#include <arpa/inet.h>                      // htons, htonl
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <ifaddrs.h>
#include <netdb.h>

#include <errno.h>

#ifndef EOK
#	define EOK 0
#endif

#ifdef __QNX__
#	define icmphdr icmp
#endif
#endif

namespace NSHARE
{
extern bool is_ip_valid(const char* aIp,bool aBroadcast)
{
	unsigned  a,b,c,d;
	return aIp!=NULL//
			&& sscanf(aIp,"%d.%d.%d.%d", &a, &b, &c, &d) == 4//
			&& (a<255 ||	(aBroadcast && (a==255) &&	(b==255) && (c==255) && (d==255)))//
			&& (b<255 || 	(aBroadcast &&				(b==255) && (c==255) && (d==255)))//
			&& (c<255 || 	(aBroadcast &&							(c==255) && (d==255)))//
			&& (d<255 || 	(aBroadcast &&										 d==255))//
			;
}
network_ip_t get_ip(uint32_t aIp)
{
	struct in_addr _addr;
	_addr.s_addr = htonl(aIp);
	return  get_ip(_addr);
}
network_ip_t get_ip(struct in_addr const& aIp)
{
#ifdef HAVE_INET_PTON
	char _buf[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, aIp, _buf, sizeof(_buf)) != NULL)
	{
		return (const char*)_buf;
	}
	return network_ip_t();
#else
	return  inet_ntoa(aIp);
#endif
}
extern uint32_t get_ip(network_ip_t const& aIP)
{
	return inet_addr( aIP.c_str());
}
bool is_mask_valid(uint32_t aVal)
{
	aVal ^= 0xffffffff;
	for (; aVal & 0x1; aVal >>= 1)
	;
	return aVal == 0;
}

bool is_broadcast_ip(network_ip_t const& aIp)
{
	union
	{
		uint8_t _byte[4];
		uint32_t _net_ip;
	};
	_net_ip=get_ip(aIp.c_str());
	return _byte[0]==0xFF//
			||_byte[1]==0xFF//
			||_byte[2]==0xFF//
			||_byte[3]==0xFF//
			;
}
NSHARE::network_ip_t broadcast(NSHARE::network_ip_t const& aIP, NSHARE::network_ip_t const& aMask)
{
	uint32_t const _ip=inet_addr( aIP.c_str());
	uint32_t const _mask=inet_addr( aMask.c_str());

	struct in_addr _addr;
	_addr.s_addr = broadcast(_ip, _mask);


	return get_ip(_addr);
}
uint32_t broadcast(uint32_t aIP, uint32_t aMask)
{
	DCHECK(is_mask_valid(ntohl(aMask)));

	uint32_t const _bits = aMask ^ 0xffffffff;
	uint32_t const _bcast = aIP | _bits;

	return _bcast;
}
int addresses(uint32_t aIP, uint32_t aMask, NSHARE::Strings* aTo)
{
	if (!is_mask_valid(ntohl(aMask)))
	{
		LOG(DFATAL) << "Invalid network mask";
		return -1;
	}
	uint32_t const _bits = ntohl(aMask ^ 0xffffffff);
	uint32_t const _base = ntohl(aIP & aMask);

	uint32_t _ip;
	for (uint32_t i = 1; i <= _bits; ++i)
	{
		_ip = _base | i;
		if (((uint8_t) _ip) == 0x0 || ((uint8_t) _ip) == 0xFF)
			continue;
		struct in_addr _addr;
		_addr.s_addr = htonl(_ip);
		aTo->push_back(get_ip(_addr));
	}
	return _bits;
}
int addresses(NSHARE::network_ip_t const& aIP, NSHARE::network_ip_t const& aMask,
		NSHARE::Strings* aTo)
{
	return addresses(inet_addr( aIP.c_str()),inet_addr( aMask.c_str()), aTo);
}

bool is_in(NSHARE::network_ip_t const& aIP, NSHARE::network_ip_t const& aSubNet,
		NSHARE::network_ip_t const& aMask)
{
	struct in_addr _ip_subnet;
	_ip_subnet.s_addr=inet_addr( aSubNet.c_str());

	struct in_addr _mask;
	_mask.s_addr=inet_addr( aMask.c_str());

	if (!is_mask_valid(ntohl(_mask.s_addr)))
	{
		DLOG(ERROR)<<"Invalid mask "<<aMask;
		return false;
	}
	struct in_addr _ip;
	_ip.s_addr=inet_addr( aIP.c_str());

	uint32_t const _bits = ntohl(_mask.s_addr ^ 0xffffffff);
	uint32_t const _base = ntohl(_ip_subnet.s_addr & _mask.s_addr);
	return (_base < ntohl(_ip.s_addr)) && (ntohl(_ip.s_addr) <= (_base | _bits));
}
}
#ifdef HAVE_GETIFADDRS
#define	DEFAULT_DATA_LEN	(64-ICMP_MINLEN)	/* default data length */
#define SEQUENCE 12
#define DEF_TIME 0.1
#define DEF_NUMBER 1

size_t const MAX_PACKET = 512;

static int input(int const aSocket, double aTime);
static uint16_t in_cksum(uint16_t *addr, size_t len);

namespace NSHARE
{

		int ping(NSHARE::network_ip_t aTarget, double aTime, unsigned aNumber)
		{
			int const _socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
			if (_socket <= 0)
			return -1;
			struct icmp *_icmp;
			uint8_t _buf[MAX_PACKET];
			_icmp = (struct icmp *) _buf;
			DCHECK_POINTER_ALIGN(_icmp);
			_icmp->icmp_type = ICMP_ECHO;
			_icmp->icmp_code = 0;
			_icmp->icmp_cksum = 0;
			_icmp->icmp_seq = SEQUENCE;
			_icmp->icmp_id = getpid();
			_icmp->icmp_cksum = in_cksum((uint16_t *) _buf, DEFAULT_DATA_LEN);

			struct sockaddr_in _to;
			_to.sin_family = AF_INET;
			_to.sin_addr.s_addr = inet_addr(aTarget.c_str());
			unsigned _count = 0;
			for (unsigned i = 0; i < aNumber; ++i)
			{
				int _rval = sendto(_socket, (uint8_t *) _buf, DEFAULT_DATA_LEN, 0,
						(struct sockaddr*) &_to,
						(socklen_t) sizeof(struct sockaddr_in));
				if (_rval < 0 || _rval != DEFAULT_DATA_LEN)
				{
					close(_socket);
					return -1;
				}
				if (input(_socket, aTime) == EOK)
				++_count;
			}
			close(_socket);
			return _count;
		}


		unsigned looking_for(NSHARE::network_ip_t const& aIP, NSHARE::network_ip_t const& aMask,
				NSHARE::Strings* aTo, double aTime, unsigned aNumber)
		{
			//FIXME обяединить с поиском ip и ping. разбить на два потока- отправки и чтения
			//итого время будет =aNumber*0.001+aTime
			NSHARE::Strings _addr;
			if (!addresses(aIP, aMask, &_addr))
			return 0;
			NSHARE::Strings::iterator _it = _addr.begin();
			for (; _it != _addr.end(); ++_it)
			if ((*_it != aIP) && ping(*_it, aTime, aNumber) > 0)
			aTo->push_back(*_it);

			return aTo->size();
		}
		int loocal_addr_ip4(interfaces_addr_t* aTo)
		{
			struct ifaddrs *_ifaddr = NULL,//
					*_ifa = NULL;
			int _rc = 0;

			interfaces_addr_t::value_type _temp;

			_rc = getifaddrs(&_ifaddr);
			if (_rc != -1)
			{

				for (_ifa = _ifaddr; _ifa != NULL; _ifa = _ifa->ifa_next)
				{
					if (_ifa->ifa_addr == NULL)
						continue;

					int const _family = _ifa->ifa_addr->sa_family;

					if (_family != AF_INET)
						continue;

					_temp.FIp = get_ip(((struct sockaddr_in *) _ifa->ifa_addr)->sin_addr);

					if (_ifa->ifa_name)
						_temp.FName = _ifa->ifa_name;
					else
						_temp.FName = "";

					_temp.FMask = "";
					if (_ifa->ifa_netmask != NULL)
					{
						_temp.FMask = get_ip(((struct sockaddr_in *) _ifa->ifa_netmask)->sin_addr);
					}
					VLOG(2)<<"Push interface :"<<_temp;
					if(_temp.FIp!=net_address::ALL_NETWORKS && _temp.FIp!=net_address::LOCAL_HOST)
						aTo->push_back(_temp);
				}
			}else
			{
				LOG(DFATAL)<<"Cannot call getifaddrs error"<<errno<<"("<<strerror(errno)<<")";
			}
			freeifaddrs(_ifaddr);
			return aTo->size();
		}
		unsigned looking_for(NSHARE::Strings* aTo, unsigned aMaxCountAddrOfSubNet,
				double aTime, unsigned aNumber)
		{
			interfaces_addr_t _ifaddr;
			loocal_addr_ip4(&_ifaddr);

			return looking_for(aTo, _ifaddr, aMaxCountAddrOfSubNet, aTime, aNumber);
		}
		unsigned looking_for(NSHARE::Strings* aTo, interfaces_addr_t const& aAddr,
				unsigned aMaxCountAddrOfSubNet, double aTime, unsigned aNumber)
		{
			MASSERT_1( aMaxCountAddrOfSubNet);

			uint32_t _pow_of_two = 1;
			for (; _pow_of_two < aMaxCountAddrOfSubNet &&//
			_pow_of_two < std::numeric_limits<uint32_t>::max()/2;
					_pow_of_two *= 2)
			;
			aMaxCountAddrOfSubNet = _pow_of_two;

			--aMaxCountAddrOfSubNet; //�� 0

			for (interfaces_addr_t::const_iterator _it = aAddr.begin();
					_it != aAddr.end(); ++_it)
			{
				if (_it->FMask == "") //localhost
				continue;
				struct in_addr _mask;
				if (inet_pton(AF_INET, _it->FMask.c_str(), &_mask) > 0)
				{
					uint32_t _new_mask = (htonl(aMaxCountAddrOfSubNet ^ 0xffffffff))
					| _mask.s_addr;
					char _str[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &_new_mask, _str, INET_ADDRSTRLEN);
					looking_for(_it->FIp, _str, aTo, aTime, aNumber);
				}
			}
			return aTo->size();
		}
}
int input(int const aSocket, double aTime)
{
	uint8_t _buf[MAX_PACKET];
	fd_set _rfds;
	FD_ZERO(&_rfds);
	FD_SET(aSocket, &_rfds);

	for (; aTime > 0.0;)
	{
		struct timeval tv;
		NSHARE::convert(&tv, aTime);
		double const _tbeg = NSHARE::get_time();

		int const _rval = select(aSocket + 1, &_rfds, NULL, NULL, &tv);
		aTime -= (NSHARE::get_time() - _tbeg);
		if (_rval <= 0)
		return -1;
		else if (FD_ISSET(aSocket, &_rfds))
		{
			int const _n = recvfrom(aSocket, (uint8_t *) _buf, MAX_PACKET, 0,
					NULL, NULL);

			if (_n <= 0)
			return -1;
			else if (_n < (int) sizeof(struct ip))
			return -1;

			struct ip *_ip = (struct ip *) _buf;
			DCHECK_POINTER_ALIGN(_ip);
			int _hlen = _ip->ip_hl << 2;

			if (_n < (_hlen + ICMP_MINLEN))
			return -1;
			struct icmp* _icmp = (struct icmp *) (_buf + _hlen);

			if (_icmp->icmp_type == ICMP_TIMXCEED
					&& _icmp->icmp_code == ICMP_TIMXCEED_INTRANS)
			{
				if (_n < ICMP_ADVLEN(_icmp))
				return -1;

				struct ip* _ip2 = (struct ip *) (_buf + _hlen + ICMP_MINLEN);
				int _hlen2 = _ip2->ip_hl << 2;
				struct icmp* _icmp2 = (struct icmp *) (_buf + _hlen2
						+ ICMP_MINLEN + _hlen);
				if (_icmp2->icmp_id == getpid() && _icmp2->icmp_seq == SEQUENCE)
				continue;
			}
			else if (_icmp->icmp_type == ICMP_ECHOREPLY
					&& _icmp->icmp_seq == SEQUENCE)
			return EOK;

		}
	}
	return -1;
}
uint16_t in_cksum(uint16_t *addr, size_t len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while (nleft > 1)
	{
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1)
	{
		u_short u = 0;

		*(u_char *) (&u) = *(u_char *) w;
		sum += u;
	}

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
	sum += (sum >> 16); /* add carry */
	answer = ~sum; /* truncate to 16 bits */
	return (answer);
}
#elif defined(HAVE_IPHLPAPI_H)
#include <iphlpapi.h>

#	ifdef HAVE_WINERROR_H
#		include <winerror.h>
#	else
# 		error The header winerror.h is reuqurement
#	endif
namespace NSHARE
{
int loocal_addr_ip4(interfaces_addr_t* aTo)
{
	DCHECK_NOTNULL(aTo);

	void* buf = NULL;
	ULONG bufSz = 0;

	if (GetAdaptersInfo(NULL, &bufSz) == ERROR_BUFFER_OVERFLOW)
		buf = malloc(bufSz);

	DWORD const _error=GetAdaptersInfo((IP_ADAPTER_INFO*) buf, &bufSz);
	if (_error== NO_ERROR)
	{
<<<<<<< HEAD
		_conf.MUpdateIfSet(IP, ip);
		_conf.MSet(PORT,port);
=======
		IP_ADAPTER_INFO* _p = (IP_ADAPTER_INFO*) buf;
		for (; _p != NULL; _p = _p->Next)
		{
			net_interface_addr_t _addr;
			_addr.FName = _p->AdapterName;

			IP_ADDR_STRING* _addr_win=&_p->IpAddressList;
			for(;_addr_win!=NULL;_addr_win=_addr_win->Next)
			{
				_addr.FIp = _addr_win->IpAddress.String;
				_addr.FMask = _addr_win->IpMask.String;

				if (_addr.FIp != net_address::ALL_NETWORKS && _addr.FIp!=net_address::LOCAL_HOST)
					aTo->push_back(_addr);
			}
		}

	}else
	{
		LOG(DFATAL)<<"Cannot call GetAdaptersInfo error:"<<_error;
>>>>>>> 80c7e21... See ChangeLog.txt
	}
	free(buf);
	return aTo->size();
}
}
#else
# 	error function loocal_addr_ip4 is not implemented for current platform
#endif //#ifndef _WIN32

