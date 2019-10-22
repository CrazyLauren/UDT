/*
 * net_function.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.03.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef NET_FUNCTION_H_
#define NET_FUNCTION_H_

#define DEF_WAIT 0.1
#define DEF_COUNT 1
#define MAX_ADDRS_OF_SUBNET 512

namespace NSHARE
{

/** Nets interface info
 *
 */
struct net_interface_addr_t
{
	network_ip_t FIp;///< IP of network
	network_ip_t FMask;///< Mask of network
	NSHARE::CText FName;///< The interface name
};
typedef std::vector<net_interface_addr_t> interfaces_addr_t;

/** Returns list of network interface address
 *
 * @param aTo A save result to
 * @return A number of interfaces
 */
extern int loocal_addr_ip4(interfaces_addr_t* aTo);

/** Returns a broadcast address for network
 *
 * @param aIP an ip in  network byte order
 * @param aMask a mask in network byte order
 * @return broadcast address in network byte order
 */
extern uint32_t broadcast(uint32_t aIP, uint32_t aMask);

/** Returns a broadcast address for network
 *
 * @param aIP an ip in  network byte order
 * @param aMask a mask in network byte order
 * @return broadcast address in network byte order
 */
extern NSHARE::network_ip_t broadcast(network_ip_t const& aIP,
		network_ip_t const& aMask);

/** Returns List of all IP of network
 *
 * @param aIP an ip in  network byte order
 * @param aMask a mask in network byte order
 * @param aTo A save result to
 * @return A number of ip or -1 if error
 */
extern int addresses(uint32_t aIP, uint32_t aMask, NSHARE::Strings* aTo);

/** Returns List of all IP of network
 *
 * @param aIP an IP
 * @param aMask a mask
 * @param aTo A save result to
 * @return A number of ip or -1 if error
 */
extern int addresses(network_ip_t const& aIP, network_ip_t const& aMask,
		NSHARE::Strings* aTo);

/** Convert ip in network byte order to CTest
 *
 * @param aIp IP in host byte order
 * @return ip
 */
extern network_ip_t get_ip(uint32_t aIp);

/** Convert ip in network byte order to CTest
 *
 * @param aIp IP in net byte order
 * @return ip
 */
extern network_ip_t get_ip(struct in_addr const& aIp);


/** Convert ip in CTest to network byte order
 *
 * @param aIp IP
 * @return ip in network byte order
 */
extern uint32_t get_ip(network_ip_t const& aIp);

/** Checks is ip valid
 *
 * @param aIp IP
 * @param aBroadcast if true The IP 127.0.0.255 is valid,
 * 					the other case isn't valid
 *
 * @return true if valid
 */
extern bool is_ip_valid(const char* aIp,bool aBroadcast=true);

/** Checks is mask valid
 *
 * @param aMask Mask in host byte order
 * @return true if valid
 */
extern bool is_mask_valid(uint32_t aMask);

/** Returns true if ip is a broadcast IP
 *
 * @param aIp IP
 * @return true if broadcast
 */
extern bool is_broadcast_ip(network_ip_t const& aIp);

/** Checks if ip in net
 *
 * @param aIP ip for check
 * @param aSubNet sub network
 * @param aMask mask of network
 * @return true if aIP in network
 */
extern bool is_in(NSHARE::network_ip_t const& aIP, NSHARE::network_ip_t const& aSubNet,
		NSHARE::network_ip_t const& aMask);

extern int ping(NSHARE::network_ip_t aTarget, double aTime = DEF_WAIT,
		unsigned aNumber = DEF_COUNT);



extern unsigned looking_for(NSHARE::network_ip_t const& aIP,
		NSHARE::network_ip_t const& aMask, NSHARE::Strings* aTo, double aTime =
				DEF_WAIT, unsigned aNumber = DEF_COUNT);
extern unsigned looking_for(NSHARE::Strings* aTo,
		interfaces_addr_t const& aAddr, unsigned aMaxCountAddrOfSubNet =
				MAX_ADDRS_OF_SUBNET, double aTime = DEF_WAIT, unsigned aNumber =
				DEF_COUNT);
extern unsigned looking_for(NSHARE::Strings* aTo,
		unsigned aMaxCountAddrOfSubNet = MAX_ADDRS_OF_SUBNET, double aTime =
				DEF_WAIT, unsigned aNumber = DEF_COUNT);

}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::net_interface_addr_t& aIfaddr)
{
	aStream << aIfaddr.FName << ": ip=" << aIfaddr.FIp << ", mask="
			<< aIfaddr.FMask;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::interfaces_addr_t& aAdr)
{
	NSHARE::interfaces_addr_t::const_iterator _it = aAdr.begin(),_it_end(aAdr.end());
	if(!aAdr.empty())
		do
		{
			aStream << *_it;
			if (++_it != _it_end)
				aStream << ", ";
		}while (_it != _it_end);
	else
		aStream << "no network interfaces";

	return aStream;
}

}
#endif /* NET_FUNCTION_H_ */
