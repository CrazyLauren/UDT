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

#include <share/Pipe.h>
#include <share/SerialPort.h>
#include <share/Net.h>
#include <share/Socket/CSocketFile.h>
#include <share/Socket/CLoopBack.h>
#include <share/Socket/CAsynchronousSocket.h>
#include <share/sm_socket.h>

#if (defined(unix)|| defined(__QNX__)) && !defined(__MINGW32__)
#include <share/Socket/CDex.h>
#include <share/Socket/CUnix.h>
#endif

#include <share/Socket/socket_parser.h>
#endif /* SHARE_SOCKET_H_ */
