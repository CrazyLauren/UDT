/*
 *
 * share_socket.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 24.06.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARE_SOCKET_H_
#define SHARE_SOCKET_H_

#include <Pipe.h>
#include <SerialPort.h>
#include <Net.h>
#include <Socket/CSocketFile.h>
#include <Socket/CLoopBack.h>
#include <Socket/CAsynchronousSocket.h>
#include <sm_socket.h>

#if (defined(unix)|| defined(__QNX__)) && !defined(__MINGW32__)
#include <Socket/CDex.h>
#include <Socket/CUnix.h>
#endif
#endif /* SHARE_SOCKET_H_ */
