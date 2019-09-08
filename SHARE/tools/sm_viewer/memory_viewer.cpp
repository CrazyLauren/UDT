// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * memory_viewer.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.12.2016
 *      Author:  (https://github.com/CrazyLauren/UDT)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <iostream>
#include <UType/CCommonAllocater.h>
#include <UType/CSharedMemory.h>
#include <share_socket.h>

using namespace NSHARE;

int main(int argc, char *argv[])
{
	init_trace(argc, argv);

	CSharedMemory _memory;
	std::cout << "View SM:" << argv[1] << std::endl;
	CSharedMemory::eError const _error = _memory.MOpen(argv[1], false);

	if (_error !=CSharedMemory::E_NO_ERROR)
		std::cerr << "Cannot open " << argv[1] <<"error"<< _error <<std::endl;
	else
	{
		CConfig _conf(_memory.MSerialize());
		std::cout << "serializing" << std::endl;		

		std::cout << "data output" << std::endl;
		std::stringstream output_stream;
		_conf.MToJSON(output_stream, true);
		std::cout << output_stream.str() << std::endl;
		std::fstream _stream;
		std::cout << "saving to ./mem.json" << std::endl;
		_stream.open("./mem.json",
				std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
		_stream << output_stream.str();
		_stream.close();
		std::cout << "finish" << std::endl;
	}
	return 0;
}

