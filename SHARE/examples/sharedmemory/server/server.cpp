// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * server.cpp
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
#include <stdexcept>
#include <SHARE/share_socket.h>
#include <SHARE/UType/CSharedMemory.h>
using namespace NSHARE;

using namespace std;



int main(int argc, char *argv[])
{
	init_trace(argc, argv);

	//CSharedMemoryServer::sMRemove("test");
	CSharedMemoryServer _server;
	size_t  _mem_size = 10*1024*1024;
	if (argc > 1)
	{
		std::string const _str(argv[1]);
		std::stringstream _ss(_str);
		_ss >> _mem_size;
	}
	if (!_server.MOpen("test", _mem_size))
	{
		std::cerr << "Cannot open SM test" << std::endl;
		return EXIT_FAILURE;
	}


	for (;! _server.MIsClients(); NSHARE::sleep(1))
	{
		std::cout<<_server<<std::endl;
	}
	double _time = NSHARE::get_time();
	double const _start_time = NSHARE::get_time();

	unsigned long long _recv_count = 0;

	unsigned aFlags=1;
	unsigned _last_flags=0;
	unsigned _i = 0;
	for (;;)
	{
		NSHARE::CBuffer _data;
		_server.MReceiveData(_data,NULL,&aFlags);

		++_last_flags;
		if(_last_flags!=aFlags)
		{
			std::cerr<<"WTF? Invalid flag or The several clients is working."<<std::endl;
			throw std::invalid_argument("Invalid argument");
		}
		NSHARE::CBuffer::size_type const _data_size = _data.size();
		_recv_count += _data_size;

		double const _current_time = NSHARE::get_time();
		double _delta = _current_time - _time;
		if (_delta == 0.0)
			_delta = 0.0000000000001;

		double const _speed = ((_data_size / 1024.0 / 1024.0) / _delta);
		_time = NSHARE::get_time();

		if ((++_i % 1000)==0)
		{
			std::cout << "test1 <==" << (_recv_count / 1024 / 1024)
					<< " md; speed=" << _speed << " mb/s; Med="
					<< ((_recv_count / 1024.0 / 1024.0)
							/ (_current_time - _start_time))<<" mb/s." << std::endl;
		}

	}

	return 0;
}
