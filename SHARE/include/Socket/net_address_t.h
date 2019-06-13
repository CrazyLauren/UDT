/*
 * net_address_t.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef NET_ADDRESS_T_H_
#define NET_ADDRESS_T_H_

struct sockaddr_in;
namespace NSHARE
{
class CConfig;
typedef CText network_ip_t;///< type of network ip
typedef int network_port_t;///< type of network

typedef std::set<network_ip_t,CStringFastLessCompare> network_ips_t;///< info about ip addresses
extern SHARE_EXPORT network_port_t INVALID_NETWORK_PORT;///< A value for check if port valid

/** Information network ip and port
 *
 * @note Ip address can be not set
 * @warning Only IPv4 is supported
 *
 */
struct SHARE_EXPORT net_address
{
	struct CFastLessCompare;

	static const CText NAME;///<A serialization key
	static const CText PORT;///< A key of type #FPort
	static const CText IP;///< A key of type #FIp

	static const CText BROAD_CAST_SUBADDR;///< broadcast address prefix
	static const CText BROAD_CAST_ADDR;///< broadcast address
	static const CText ALL_NETWORKS;///< address for listens on all networks
	static const CText LOCAL_HOST;///< ip address of local host

	/** @brief default constructor
	 *
	 */
	net_address();

	/*! @brief Deserialize object
	 *
	 * 	To check the result of deserialization,
	 * 	used the MIsValid().
	 *	@param aConf Serialized object
	 */
	net_address(NSHARE::CConfig const& aConf);

	/*! @brief Only a port number will be passed to constructor.
	 *
	 *	@param aPort - A port number
	 */
	explicit net_address(network_port_t aPort);

	/** @brief The port number and ip address in
	 * host-byte order will be passed to constructor.
	 *
	 *	@param  aPort port number (host-byte order)
	 *	@param  aIp IP (host-byte order)	 *
	 */
	net_address(uint32_t aIp, network_port_t aPort);

	/** @brief Construct from sockaddr_in
	 *
	 *@param  aAddr IP and port in net-byte order
	 */
	net_address(struct sockaddr_in const& aAddr);

	/** @brief The port number and ip address
	 *  will be passed to constructor.
	 *
	 *	@param  aPort port number (host-byte order)
	 *	@param  aIp IP
	 */
	net_address(const char* aIP, network_port_t aPort);

	/** @brief The port number and ip address
	 *  will be passed to constructor.
	 *
	 *	@param  aPort port number (host-byte order)
	 *	@param  aIp IP
	 */
	net_address(network_ip_t const& aIP, network_port_t aPort) ;

	/** @brief Compare addresses
	 *
	 */
	bool operator==(net_address const& aRht) const;

	/** @brief Compare addresses
	 *
	 */
	bool operator!=(net_address const& aRht) const
	{
		return !operator==(aRht);
	}
	/** @brief Compare ports
	 *
	 */
	bool operator==(network_port_t const& aRht) const;

	/** @brief Less compare for using in containers
	 *
	 */
	bool operator<(net_address const& aRht) const;

	/** @brief Less compare for using in containers
	 *
	 */
	bool operator<(network_port_t const& aRht) const
	{
		return FPort < aRht;
	}

	/** @brief Set up IP
	 *
	 * @param aIP IP in host byte order
	 */
	void MSetIP(uint32_t aIp);

	/** @brief Set(Unset) ip
	 *
	 *  if ip is empty it will unset
	 *
	 * @return true if no error
	 */
	bool MSetIP(network_ip_t const& aIp);

	/*! @brief Checks object for valid
	 *
	 * Usually It's used after deserializing object
	 * @return true if it's valid.
	 */
	bool MIsValid() const;

	/*! @brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	CConfig MSerialize() const;

	/** @brief Checks is ip valid
	 *
	 * @return true if valid
	 */
	bool MIsIPValid() const;

	/** @brief Checks is network port valid
	 *
	 * @return true if valid
	 */
	bool MIsPortValid() const;

	NSHARE::smart_field_t<network_ip_t> FIp;///< Network ip
	network_port_t FPort;///< Network port
};

struct net_address::CFastLessCompare
{
	bool operator()(const net_address& a, const net_address& b) const
	{
		if (a.FPort != b.FPort)
			return a.FPort < b.FPort;
		else if (!a.FIp.MIs())
			return !b.FIp.MIs();
		else if (!b.FIp.MIs())
			return true;
		else
			return NSHARE::CStringFastLessCompare()(a.FIp.MGetConst(), b.FIp.MGetConst());
	}
};
typedef std::set<net_address,net_address::CFastLessCompare> net_addresses_t; ///< list of addresses

} //namespace USHARE
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::net_address& aIP)
{
	if (aIP.FIp.MGetConst() != "")
		aStream << "IP=" << aIP.FIp.MGetConst() << "; ";
	return aStream << "Port=" << aIP.FPort;
}
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::net_addresses_t& aIPs)
{
	NSHARE::net_addresses_t::const_iterator _it(aIPs.begin()), _it_end(
			aIPs.end());

	if (!aIPs.empty())
		do
		{
			aStream << *_it;
			if (++_it != _it_end)
				aStream << ", ";
		} while (_it != _it_end);
	else
		aStream << "no addresses";

	return aStream;
}
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::network_ips_t& aIPs)
{
	NSHARE::network_ips_t::const_iterator _it(aIPs.begin()), _it_end(
			aIPs.end());

	if (!aIPs.empty())
		do
		{
			aStream << *_it;
			if (++_it != _it_end)
				aStream << ", ";
		} while (_it != _it_end);
	else
		aStream << "no addresses";

	return aStream;
}

template<class _Elem, class _Traits>
inline basic_istream<_Elem, _Traits>& operator>>(
		basic_istream<_Elem, _Traits>& aStream, NSHARE::net_address& _Str)
{
	//todo
	return (aStream);
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
#endif /* NET_ADDRESS_T_H_ */
