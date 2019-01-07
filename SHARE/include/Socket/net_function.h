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
namespace NNet
{
struct interface__addr_t
{
	NSHARE::CText IP;
	NSHARE::CText Mask;
	NSHARE::CText Name;
};
typedef std::list<interface__addr_t> interfaces_addr_t;
extern int ping(NSHARE::CText aTarget, double aTime = DEF_WAIT,
		unsigned aNumber = DEF_COUNT);
extern NSHARE::CText broadcast(NSHARE::CText const& aIP,
		NSHARE::CText const& aMask);
extern uint32_t broadcast(uint32_t aIP, uint32_t aMask);
extern int addresses(uint32_t aIP, uint32_t aMask, NSHARE::Strings* aTo);
extern int addresses(NSHARE::CText const& aIP, NSHARE::CText const& aMask,
		NSHARE::Strings* aTo);
extern unsigned looking_for(NSHARE::CText const& aIP,
		NSHARE::CText const& aMask, NSHARE::Strings* aTo, double aTime =
				DEF_WAIT, unsigned aNumber = DEF_COUNT);
extern unsigned looking_for(NSHARE::Strings* aTo,
		interfaces_addr_t const& aAddr, unsigned aMaxCountAddrOfSubNet =
				MAX_ADDRS_OF_SUBNET, double aTime = DEF_WAIT, unsigned aNumber =
				DEF_COUNT);
extern unsigned looking_for(NSHARE::Strings* aTo,
		unsigned aMaxCountAddrOfSubNet = MAX_ADDRS_OF_SUBNET, double aTime =
				DEF_WAIT, unsigned aNumber = DEF_COUNT);
extern int loocal_addr(interfaces_addr_t* aTo);
extern bool is_in(NSHARE::CText const& aIP, NSHARE::CText const& aSubNet,
		NSHARE::CText const& aMask);
}
}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::NNet::interface__addr_t& aIfaddr)
{
	aStream << aIfaddr.Name << ": ip=" << aIfaddr.IP << ", mask="
			<< aIfaddr.Mask;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::NNet::interfaces_addr_t& aAdr)
{
	for (NSHARE::NNet::interfaces_addr_t::const_iterator _it = aAdr.begin();
			_it != aAdr.end(); ++_it)
		aStream << *_it << ";\n";
	return aStream;
}

}
#endif /* NET_FUNCTION_H_ */
