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
#include <winsock2.h>                     // sockaddr_in
#else
#include <arpa/inet.h>                      // htons, htonl
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <ifaddrs.h>
#include <netdb.h>

#include <errno.h>

#ifndef EOK
#define EOK 0
#endif

#ifdef __QNX__
#	define icmphdr icmp
#endif

#define	DEFAULT_DATA_LEN	(64-ICMP_MINLEN)	/* default data length */
#define SEQUENCE 12
#define DEF_TIME 0.1
#define DEF_NUMBER 1

size_t const MAX_PACKET = 512;

static int input(int const aSocket, double aTime);
static uint16_t in_cksum(uint16_t *addr, size_t len);
bool check_mask(uint32_t aVal)
{

	aVal = ntohl(aVal);
	aVal ^= 0xffffffff;
	for (; aVal & 0x1; aVal >>= 1)
	;
	return aVal == 0;

}
namespace NSHARE
{
	namespace NNet
	{
		int ping(NSHARE::CText aTarget, double aTime, unsigned aNumber)
		{
			int const _socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
			if (_socket <= 0)
			return -1;
			struct icmp *_icmp;
			uint8_t _buf[MAX_PACKET];
			_icmp = (struct icmp *) _buf;
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

		NSHARE::CText broadcast(NSHARE::CText const& aIP, NSHARE::CText const& aMask)
		{
			char _str[INET_ADDRSTRLEN];
			struct in_addr _ip;
			if (inet_pton(AF_INET, aIP.c_str(), &_ip) <= 0)
			return "";
			struct in_addr _mask;
			if (inet_pton(AF_INET, aMask.c_str(), &_mask) <= 0)
			return "";
			in_addr_t _addr = broadcast(_ip.s_addr, _mask.s_addr);
			inet_ntop(AF_INET, &_addr, _str, INET_ADDRSTRLEN);
			return _str;
		}
		uint32_t broadcast(uint32_t aIP, uint32_t aMask)
		{
			if (!check_mask(aMask))
			{
				errno= EINVAL;
				return ULONG_MAX;
			}
			uint32_t _bits = aMask ^ 0xffffffff;
			uint32_t _bcast = aIP | _bits;

			return _bcast;
		}
		int addresses(uint32_t aIP, uint32_t aMask, NSHARE::Strings* aTo)
		{
			if (!check_mask(aMask))
			{
				errno= EINVAL;
				return -1;
			}
			uint32_t const _bits = ntohl(aMask ^ 0xffffffff);
			uint32_t const _base = ntohl(aIP & aMask);

			char _str[INET_ADDRSTRLEN];
			uint32_t _ip;
			for (uint32_t i = 1; i <= _bits; ++i)
			{
				_ip = _base | i;
				if (((uint8_t) _ip) == 0x0 || ((uint8_t) _ip) == 0xFF)
				continue;
				in_addr_t _addr = htonl(_ip);
				inet_ntop(AF_INET, &_addr, _str, INET_ADDRSTRLEN);

				aTo->push_back(_str);
			}
			return _bits;
		}
		int addresses(NSHARE::CText const& aIP, NSHARE::CText const& aMask,
				NSHARE::Strings* aTo)
		{
			struct in_addr _ip;
			if (inet_pton(AF_INET, aIP.c_str(), &_ip) <= 0)
			return -1;
			struct in_addr _mask;
			if (inet_pton(AF_INET, aMask.c_str(), &_mask) <= 0)
			return -1;

			return addresses(_ip.s_addr, _mask.s_addr, aTo);
		}
		unsigned looking_for(NSHARE::CText const& aIP, NSHARE::CText const& aMask,
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
		int loocal_addr(interfaces_addr_t* aTo)
		{
			struct ifaddrs *_ifaddr = NULL, *_ifa = NULL;
			int _rc = 0;
			char _buf[INET6_ADDRSTRLEN];
			interfaces_addr_t::value_type _temp;

			_rc = getifaddrs(&_ifaddr);
			if (_rc != -1)
			{

				for (_ifa = _ifaddr; _ifa != NULL; _ifa = _ifa->ifa_next)
				{
					if (_ifa->ifa_addr == NULL)
					continue;
					int _family = _ifa->ifa_addr->sa_family;
					void *_haddr = NULL;
					if (_family == AF_INET)
					_haddr = &((struct sockaddr_in *) _ifa->ifa_addr)->sin_addr;
					else if (_family == AF_INET6)
					_haddr = &((struct sockaddr_in6 *) _ifa->ifa_addr)->sin6_addr;
					else
					continue;

					if (inet_ntop(_family, _haddr, _buf, sizeof(_buf)) == NULL)
					continue;
					_temp.IP = _buf;
					if (_ifa->ifa_name)
					_temp.Name = _ifa->ifa_name;
					else
					_temp.Name = "";

					_temp.Mask = "";
					if (_ifa->ifa_netmask != NULL)
					{
						_haddr = NULL;
						if (_family == AF_INET)
						_haddr =
						&((struct sockaddr_in *) _ifa->ifa_netmask)->sin_addr;
						else if (_family == AF_INET6)
						_haddr =
						&((struct sockaddr_in6 *) _ifa->ifa_netmask)->sin6_addr;

						if (_haddr
								&& inet_ntop(_ifa->ifa_netmask->sa_family, _haddr, _buf,
										sizeof(_buf)) != NULL)
						_temp.Mask = _buf;

					}
					aTo->push_back(_temp);
				}
			}
			freeifaddrs(_ifaddr);
			return aTo->size();
		}
		unsigned looking_for(NSHARE::Strings* aTo, unsigned aMaxCountAddrOfSubNet,
				double aTime, unsigned aNumber)
		{
			interfaces_addr_t _ifaddr;
			loocal_addr(&_ifaddr);

			return looking_for(aTo, _ifaddr, aMaxCountAddrOfSubNet, aTime, aNumber);
		}
		unsigned looking_for(NSHARE::Strings* aTo, interfaces_addr_t const& aAddr,
				unsigned aMaxCountAddrOfSubNet, double aTime, unsigned aNumber)
		{
			MASSERT_1( aMaxCountAddrOfSubNet);

			uint32_t _pow_of_two = 1;
			for (; _pow_of_two < aMaxCountAddrOfSubNet && _pow_of_two < LONG_MAX;
					_pow_of_two *= 2)
			;
			aMaxCountAddrOfSubNet = _pow_of_two;

			--aMaxCountAddrOfSubNet; //�� 0

			for (interfaces_addr_t::const_iterator _it = aAddr.begin();
					_it != aAddr.end(); ++_it)
			{
				if (_it->Mask == "") //localhost
				continue;
				struct in_addr _mask;
				if (inet_pton(AF_INET, _it->Mask.c_str(), &_mask) > 0)
				{
					uint32_t _new_mask = (htonl(aMaxCountAddrOfSubNet ^ 0xffffffff))
					| _mask.s_addr;
					char _str[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &_new_mask, _str, INET_ADDRSTRLEN);
					looking_for(_it->IP, _str, aTo, aTime, aNumber);
				}
			}
			return aTo->size();
		}
		bool is_in(NSHARE::CText const& aIP, NSHARE::CText const& aSubNet,
				NSHARE::CText const& aMask)
		{
			struct in_addr _ip_subnet;
			if (inet_pton(AF_INET, aSubNet.c_str(), &_ip_subnet) <= 0)
			return false;
			struct in_addr _mask;
			if (inet_pton(AF_INET, aMask.c_str(), &_mask) <= 0)
			return false;

			if (!check_mask(_mask.s_addr))
			{
				errno= EINVAL;
				return false;
			}
			struct in_addr _ip;
			if (inet_pton(AF_INET, aIP.c_str(), &_ip) <= 0)
			return false;

			uint32_t const _bits = ntohl(_mask.s_addr ^ 0xffffffff);
			uint32_t const _base = ntohl(_ip_subnet.s_addr & _mask.s_addr);
			return (_base < ntohl(_ip.s_addr)) && (ntohl(_ip.s_addr) <= (_base | _bits));
		}
	} //namespace
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
#endif //#ifndef _WIN32
namespace NSHARE
{
const CText net_address::NAME="addr";
const CText net_address::PORT="port";
const CText net_address::IP="ip";

net_address::net_address(NSHARE::CConfig const& aConf):port(-1)
{
	aConf.MGetIfSet(IP,ip);
	aConf.MGetIfSet(PORT,port);
}
CConfig net_address::MSerialize() const
{
	CConfig _conf(NAME);
	if (MIsValid())
	{
		_conf.MUpdateIfSet(IP, ip);
		_conf.MSet(PORT,port);
	}
	return _conf;
}
void net_address::MSetIP(uint32_t aIp) {
	struct in_addr _addr; //fixme inet_net_ntop()
	_addr.s_addr = htonl(aIp);
	ip = inet_ntoa(_addr);
}
net_address::net_address(uint32_t aIp, uint32_t aPort)
{
	MSetIP(aIp);
	port = aPort;
}
}
