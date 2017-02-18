/*
 * test.cpp
 *
 *  Created on: 29.03.2016
 *      Author: Sergey
 */
#include <customer.h>
#include <map>

using namespace NUDT;
extern int sniffer_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	args_t const* _recv_arg = (args_t const*)aWHAT;
	//!<Now You can handle the received data.
	std::cout<< std::endl << "Message #" << _recv_arg->FPacketNumber<<" by " << _recv_arg->FProtocolName << " size "
			<< _recv_arg->FBuffer.size() << " bytes sniffed from "
			<< _recv_arg->FFrom <<" to ";
			
	std::vector<NSHARE::uuid_t>::const_iterator _it = _recv_arg->FTo.begin(),
			_it_end = _recv_arg->FTo.end();
	for(;_it!=_it_end;++_it)
	{
		std::cout << *_it<<", ";
	}
	std::cout << std::endl;
	return 0;
}


extern void doing_something()
{
	//event loop
	CCustomer::sMGetInstance().MJoin();
}

