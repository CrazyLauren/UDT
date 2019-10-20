/*
 * Net.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 06.04.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef NET_H_
#define NET_H_

#if (defined(unix)|| defined(__QNX__)) && !defined(__MINGW32__)
#	include <netinet/in.h> // sockaddr_in
#	include <sys/time.h>
#else
#include <winsock2.h>
#endif//#include <deftype>

#include <deftype>
#include <share/Socket/ISocket.h>
#include <share/Socket/CSocket.h>
#include <share/Socket/CSelectSocket.h>
#include <share/Socket/net_address_t.h>
#include <share/Socket/CUDP.h>
#include <share/Socket/CTCP.h>
#include <share/Socket/CTCPServer.h>
#include <share/Socket/net_function.h>

#endif /* NET_H_ */
