/*
 * server.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 24.06.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren/UDT)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <iostream>
#include <share_socket.h>
#include <UType/CSharedMemory.h>
using namespace NSHARE;

using namespace std;



int main(int argc, char *argv[])
{
	init_trace(argc, argv);

	CSharedMemoryServer _server;
	if (!_server.MOpen("test", 1024 * 1024)) 
	{
		std::cerr << "Cannot open SM test" << std::endl;
		return EXIT_FAILURE;
	}


	for (;! _server.MIsClients(); NSHARE::sleep(1))
	{
		_server.MPrint(std::cout)<<std::endl;
	}
	double _time = NSHARE::get_time();
	double _start_time = NSHARE::get_time();

	unsigned long long _recv_count = 0;

	unsigned aFlags=1;
	unsigned _last_flags=0;
	for (;;)
	{
		NSHARE::CBuffer _data;
		_server.MReceiveData(_data,NULL,&aFlags);

		++_last_flags;
		if(_last_flags!=aFlags)
		{
			std::cerr<<"Invalid flags"<<std::endl;
		}

		_recv_count += _data.size();

		double _delta = NSHARE::get_time() - _time;
		if (_delta == 0.0)
			_delta = 0.0000000000001;

		double _speed = ((_data.size() / 1024.0 / 1024.0) / _delta);
		_time = NSHARE::get_time();

		static int _i = 0;
		if (!(++_i % 10000))
		{
			std::cout << "test1 <==" << (_recv_count / 1024 / 1024)
					<< " md; speed=" << _speed << " mb/s; Med="
					<< ((_recv_count / 1024.0 / 1024.0)
							/ (NSHARE::get_time() - _start_time))<<" mb/s." << std::endl;
		}

	}

	return 0;
}
