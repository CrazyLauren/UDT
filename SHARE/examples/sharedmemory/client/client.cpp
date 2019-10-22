// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * client.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  (https://github.com/CrazyLauren/UDT)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <iostream>
#include <SHARE/share_socket.h>
#include <SHARE/unit_tests.h>

using namespace NSHARE;

#define SEND_BUF_SIZE 100000

int main(int argc, char *argv[])
{
	init_trace(argc, argv);
	bool const _is=NSHARE::unit_testing();
	if(!_is)
		std::cerr << "testing error" << std::endl;

	size_t _buf_size = SEND_BUF_SIZE;
	if (argc > 1)
	{
		std::string const _str(argv[1]);
		std::stringstream _ss(_str);
		_ss >> _buf_size;
	}
	CSharedMemoryClient _client;

	for (; !_client.MOpen("test"); NSHARE::usleep(100000))
	{
		std::cerr << "Cannot open test SM" << std::endl;
	}
	std::cout << "Opened" << std::endl;
	for(; !_client.MConnect();)
		std::cerr << "Cannot connect to server 'test'" << std::endl;
	std::cout << "Connected" << std::endl;
	std::cout << "Buf suze=" << _buf_size << " bytes" << std::endl;

	unsigned _flags=1;
	for (;;)
	{
		NSHARE::CBuffer _buf= _client.MAllocate(_buf_size);

		if(!_buf.empty()&& _client.MSend(_buf, false, _flags)== sent_state_t::E_SENDED)
			++_flags;
	}
	NSHARE::usleep(10000);
	std::cerr << "closing" << std::endl;
	_client.MClose();
	std::cerr << "closed" << std::endl;

	return 0;
}

