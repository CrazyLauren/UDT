// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * net_address_t.cpp
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
#ifdef _WIN32
#	include <ws2tcpip.h>
#else
#	include <arpa/inet.h>// htons, htonl
#endif
namespace NSHARE
{
const CText net_address::NAME="addr";
const CText net_address::PORT="port";
const CText net_address::IP="ip";
const CText net_address::BROAD_CAST_SUBADDR = "255";
const CText net_address::BROAD_CAST_ADDR = "255.255.255.255";
const CText net_address::ALL_NETWORKS = "0.0.0.0";
const CText net_address::LOCAL_HOST = "127.0.0.1";

network_port_t INVALID_NETWORK_PORT=-1;

net_address::net_address(uint32_t aIp, network_port_t aPort)
{
	MSetIP(aIp);
	FPort = aPort;
}
net_address::net_address() :
		FPort(INVALID_NETWORK_PORT)
{
}
net_address::net_address(network_ip_t const& aIP, network_port_t aPort) ://
		FIp(aIP), //
		FPort(aPort)
{
	DCHECK(is_ip_valid(aIP.c_str()));
}
net_address::net_address(const char* aIP, network_port_t aPort) ://
		FIp(aIP), //
		FPort(aPort)
{
	DCHECK_NOTNULL(aIP);
	DCHECK(is_ip_valid(aIP));
}
net_address::net_address(struct sockaddr_in const& aAddr)
{
	FIp = get_ip(aAddr.sin_addr);
	FPort = ntohs(aAddr.sin_port);
}
net_address::net_address(NSHARE::CConfig const& aConf)://
		FPort(INVALID_NETWORK_PORT)
{
	aConf.MGetIfSet(IP,FIp);
	aConf.MGetIfSet(PORT,FPort);
}
net_address::net_address(network_port_t aPort) ://
			FPort(aPort)
{
}
bool net_address::operator==(net_address const& aRht) const
{
	if (FIp.MIs() && aRht.FIp.MIs())
		return aRht.FIp == FIp && FPort == aRht.FPort;
	return operator==(aRht.FPort);
}
bool  net_address::operator==(network_port_t const& aRht) const
{
	return FPort == aRht;
}
bool net_address::operator<(net_address const& aRht) const
{
	if (aRht.FIp != FIp)
		return aRht.FIp < FIp;
	return FPort < aRht.FPort;
}
CConfig net_address::MSerialize() const
{
	CConfig _conf(NAME);
	if (MIsValid())
	{
		if(FIp.MIs())
			_conf.MSet(IP,FIp.MGetConst());
		_conf.MSet(PORT,FPort);
	}
	return _conf;
}
bool net_address::MIsValid() const
{
	return MIsPortValid();///< Required only that the port will be valid
}
bool net_address::MIsPortValid() const
{
	return FPort !=INVALID_NETWORK_PORT;
}
void net_address::MSetIP(uint32_t aIp)
{
	FIp = get_ip(aIp);
}
bool net_address::MSetIP(network_ip_t const& aIp)
{
	if (aIp.empty()) FIp.MUnSet();

	DCHECK(is_ip_valid(aIp.c_str()));

	FIp.MSet(aIp);

	return true;

}

bool net_address::MIsIPValid() const
{
	if(!FIp.MIs())
		return false;

	return is_ip_valid(FIp.MGetConst().c_str());

}
}
