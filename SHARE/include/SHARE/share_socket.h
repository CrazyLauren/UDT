/*
 *
 * share_socket.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_SOCKET_H_
#define SHARE_SOCKET_H_

#include <SHARE/Pipe.h>
#include <SHARE/SerialPort.h>
#include <SHARE/Net.h>
#include <SHARE/Socket/CSocketFile.h>
#include <SHARE/Socket/CLoopBack.h>
#include <SHARE/Socket/CAsynchronousSocket.h>
#include <SHARE/sm_socket.h>

#if (defined(unix)|| defined(__QNX__)) && !defined(__MINGW32__)
#include <SHARE/Socket/CDex.h>
#include <SHARE/Socket/CUnix.h>
#endif

#include <SHARE/Socket/socket_parser.h>
#endif /* SHARE_SOCKET_H_ */
