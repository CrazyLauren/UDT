/*
 * INet.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 26.03.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef INET_H_
#define INET_H_

namespace NSHARE
{
class CConfig;
struct SHARE_EXPORT net_address
{
	struct CFastLessCompare;
	static const CText PORT;
	static const CText IP;
	net_address() :
			port(-1)
	{
	}
	net_address(NSHARE::CConfig const& aConf);
	explicit net_address(uint32_t aPort) :
			port(aPort)
	{
	}
	// struct in_addr in ( host-byte order), port ( host-byte order)
	net_address(uint32_t aIp, uint32_t aPort);

	net_address(const char* aIP, uint32_t aPort) :
			ip(aIP), port(aPort)
	{
	}
	net_address(String const& aIP, uint32_t aPort) :
			ip(aIP), port(aPort)
	{
	}
	bool operator==(net_address const& aRht) const
	{
		if (ip.MIs() && aRht.ip.MIs())
			return aRht.ip == ip && port == aRht.port;
		return operator==(aRht.port);
	}
	bool operator!=(net_address const& aRht) const
	{
		return !operator==(aRht);
	}
	bool operator==(in_port_t const& aRht) const
	{
		return port == aRht;
	}
	bool operator<(net_address const& aRht) const
	{
		if (aRht.ip != ip)
			return aRht.ip < ip;
		return port < aRht.port;
	}
	bool operator<(in_port_t const& aRht) const
	{
		return port < aRht;
	}

	bool MIsValid() const
	{
		return port >= 0;
	}
	CConfig MSerialize() const;
	NSHARE::smart_field_t<CText> ip;
	int port;
};
struct net_address::CFastLessCompare
{
	bool operator()(const net_address& a, const net_address& b) const
	{
		if (a.port != b.port)
			return a.port < b.port;
		else if (!a.ip.MIs())
			return !b.ip.MIs();
		else if (!b.ip.MIs())
			return true;
		else
			return NSHARE::CStringFastLessCompare()(a.ip.MGetConst(), b.ip.MGetConst());
	}
};

} //namespace USHARE
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::net_address& aIP)
{
	if (aIP.ip.MGetConst() != "")
		aStream << "IP=" << aIP.ip.MGetConst() << "; ";
	return aStream << "Port=" << aIP.port;
}
template<>
struct less<NSHARE::net_address>: public binary_function<NSHARE::net_address, NSHARE::net_address, bool>
{
	bool operator()(const NSHARE::net_address& __x, const NSHARE::net_address& __y) const
	{
		NSHARE:: net_address::CFastLessCompare _comp;
		return _comp(__x,__y);
	}
};
}
#endif /* INET_H_ */
