/*
 * Net.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 06.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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
//typedef size_t ssize_t;
typedef int in_port_t;
#include <winsock2.h>
#endif//#include <deftype>

#include <deftype>
#include <Socket/ISocket.h>
#include <Socket/CSocket.h>
#include <Socket/diagnostic_io_t.h>
#include <Socket/CSelectSocket.h>
#include <Socket/INet.h>
#include <Socket/CNetBase.h>
#include <Socket/CUDP.h>
#include <Socket/CTCP.h>
#include <Socket/CTCPServer.h>
#include <Socket/net_function.h>

#endif /* NET_H_ */
