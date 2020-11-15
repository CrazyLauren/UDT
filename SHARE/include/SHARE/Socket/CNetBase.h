/*
 * CNetBase.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CNETBASE_H_
#define CNETBASE_H_

#include <SHARE/Socket/socket_setting_t.h>

namespace NSHARE
{

/**
 *   Base class representing basic communication endpoint
 */
class SHARE_EXPORT CNetBase
{
public:

	/** For windows initialize WSA if need
	 *
	 *	@param aSetting Settings
	 */
	CNetBase();

	/** Create a new socket for protocol
	 *
	 *	@param aType A type
	 *	@param aProtocol A protocol
	 */
	static CSocket MNewSocket(int aType,int aProtocol);

	/**  Get the local net address
	 *
	 * @param aSocket A reference to socket
	 * @return local address of socket
	 */
	static net_address MGetLocalAddress(CSocket const& aSocket);

	/**  Get the foreign address.
	 *
	 * @param aSocket A reference to socket
	 * @return foreign address
	 */
	static net_address MGetForeignAddress(CSocket const& aSocket);

	/** Set the local port to the specified port and the local address
	 *   to any interface
	 *
	 * @param aSocket A reference to socket
	 * @param aAddr An address
	 */
	static int MSetLocalAddrAndPort(const CSocket& aSocket, const net_address& aAddr);

	/** Set the multicast TTL
	 *
	 * @param aTTL multicast TTL
	 * @param aSocket A reference to socket
	 */
	static bool MSetMultiCastTTL(CSocket& aSocket,unsigned char aTTL);

	 /**
	 *   Join the specified multicast group
	 *   @param aAddr multicast group address to join
	 *   @param aSocket A reference to socket
	 */
	static bool MJoinGroup(const CSocket& aSocket, const net_address& aAddr);
	/**
	 *  Leave the specified multicast group
	 *   @param aAddr multicast group address to join
	 *   @param aSocket A reference to socket
	 */
	static bool MLeaveGroup(const CSocket& aSocket, const net_address& aAddr);

	/** @brief Enables or disables the reuse of duplicate port bindings
	 * (for linux only)
	 *
	 *	@param aSocket A reference to socket
	 */
	static bool MReUsePort(CSocket& aSocket);

	/** @brief Enables or disables the reuse of duplicate addresses and port bindings
	 *
	 *	@param aSocket A reference to socket
	 */
	static bool MReUseAddr(CSocket& aSocket);

	/** Returns amount of available bytes to read
	 *
	 * @param aSocket A reference to socket
	 * @return amount of bytes
	 */
	static size_t MAvailable(CSocket const& aSocket);

	/** Converts NSHARE::net_address to sockaddr_in
	 *
	 *	@param aAddress A from
	 *	@param aSa [out] A to
	 */
	static void MSetAddress(net_address const& aAddress,
			struct sockaddr_in *aSa);

	/** Setting socket settings
	 *
	 *	@param aSetting Settings
	 */
	static void MSettingSocket(CSocket& aSocket,socket_setting_t const& aSetting);
	static bool MSettingSendBufSize(CSocket& aSocket,long aValue);
	static bool MSettingRecvBufSize(CSocket& aSocket,long aValue);

	/** Returns size of Send buffer
	 *
	 * @param aSocket A reference to socket
	 * @return amount of bytes or -1
	 */
	static size_t MGetSendBufSize(CSocket const& aSocket);

	/** Returns size of receive buffer
	 *
	 * @param aSocket A reference to socket
	 * @return amount of bytes or -1
	 */
	static size_t MGetRecvBufSize(CSocket const& aSocket);

	/** @brief Set up the buffer size
	 *
	 *	By default It's trying to set the buffer size
	 *	specified in the  #socket_setting_t.
	 *	If It cannot set the specified buffer size
	 *	it decrease the buffer size to 10 % in every
	 *	step until it sets the buffer size.
	 *
	 *	@param aSocket The socket
	 *	@param aSetting Settings
	 */
	static void MSettingBufSize(CSocket& aSocket,socket_setting_t const& aSetting);

	/** @brief Enable keep alive
	 *
	 *
	 *	@param aSocket The socket
	 *	@param aSetting Settings
	 */
	static void MSettingKeepAlive(CSocket& aSocket,socket_setting_t const& aSetting);

	/** Set the socket to nonblocking mode
	 * @param aSocket A reference to socket
	 * @return true if no error
	 */
	static bool MMakeNonBlocking(CSocket& aSocket);

	/** Make broadcast the UDP socket
	 *
	 * @param aSocket A reference to socket
	 * @return true if no error
	 */
	static bool MMakeAsBroadcast(CSocket& aSocket);

	/** Specified by which network interface the multicast data
	 * has to be sent in
	 *
	 * @param aSocket A reference to socket
	 * @param aSa A multicast address
	 * @return true if no error
	 */
	static bool MSendMultiCastMsgByInterface(CSocket& aSocket,struct sockaddr_in const & aSa);

	/** Set close-on-exist flags
	 *
	 */
	static bool MSetCloseOnExist(CSocket& aSocket);
};

} /* namespace NSHARE */
#endif /* CNETBASE_H_ */
