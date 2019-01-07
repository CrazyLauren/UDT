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

namespace NSHARE
{
//#ifdef _WIN32
//extern bool init_wsa();
//#define SHARE_INIT_WSA if (!init_wsa())	throw std::runtime_error("cannot init wsa");
//#else
//#define SHARE_INIT_WSA
//#endif

class SHARE_EXPORT CNetBase
{
public:
	CNetBase();
	static long const DEF_BUF_SIZE;
	int MBindSocket(const CSocket&, const struct sockaddr_in&);
	void MReUsePort(CSocket&);
	void MReUseAddr(CSocket&);
	size_t MAvailable(CSocket const&) const;
	int MGetSendBufSize(CSocket const&) const;
	static int MSetAddress(net_address const& aAddress,
			struct sockaddr_in *aSa);

	void MSettingSocket(CSocket& aSocket);
	bool MSettingSendBufSize(CSocket& aSocket,long aValue);
	bool MSettingRecvBufSize(CSocket& aSocket,long aValue);
	size_t MGetSendBufSize(CSocket& aSocket) const;
	size_t MGetRecvBufSize(CSocket& aSocket) const;
	void MSettingBufSize(CSocket& aSocket);
	void MSettingKeepAlive(CSocket& aSocket);
	void MMakeNonBlocking(CSocket& aSocket);
};

} /* namespace NSHARE */
#endif /* CNETBASE_H_ */
