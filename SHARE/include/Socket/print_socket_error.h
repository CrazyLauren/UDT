/*
 * print_socket_error.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 29.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef PRINT_SOCKET_ERROR_H_
#define PRINT_SOCKET_ERROR_H_

namespace NSHARE
{
inline std::string print_socket_error()
{
	std::string _val;
	char buffer[20];

#ifdef _WIN32
	sprintf(buffer,"%d",::WSAGetLastError());
	_val+="(";
	_val+=buffer;
	_val+=")";
#else
	sprintf(buffer,"%d",errno);
	_val+=strerror(errno);
	_val+="(";
	_val+=buffer;
	_val+=")";
#endif
	return _val;
}
}//namespace NSHARE


#endif /* PRINT_SOCKET_ERROR_H_ */
