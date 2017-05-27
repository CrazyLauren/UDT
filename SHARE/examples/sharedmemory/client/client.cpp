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
#include <share_socket.h>

using namespace NSHARE;

#define SEND_BUF_SIZE 100000

int main(int argc, char *argv[])
{
	init_trace(argc, argv);
	CText::sMUnitTest();
	CBuffer::sMUnitTest(NULL);

	size_t _buf_size = (argc > 1) ? atoi(argv[1]) : SEND_BUF_SIZE;

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

		if(!_buf.empty()&& _client.MSend(_buf, false, _flags)== CSharedMemoryClient::E_SENDED)
			++_flags;
	}
	NSHARE::usleep(10000);
	std::cerr << "closing" << std::endl;
	_client.MClose();
	std::cerr << "closed" << std::endl;

	return 0;
}

