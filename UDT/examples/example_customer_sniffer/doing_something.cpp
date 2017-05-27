/*
 * doing_something.cpp
 *
 *  Created on: 29.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
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

extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	fail_sent_args_t* _recv_arg=(fail_sent_args_t*)aWHAT;


	std::cerr<<"The packet "<<_recv_arg->FPacketNumber<<" has not been delivered to ";
	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FFails.begin()),
			_it_end(_recv_arg->FFails.end());

	for (; _it != _it_end; ++_it)
	{
		std::cerr<<(*_it)<<", ";
	}
	std::cerr<<" From "<<_recv_arg->FFrom << " by UDT kernel as ";
	CCustomer::sMPrintError(std::cerr,_recv_arg->FErrorCode);

	if(_recv_arg->FErrorCode & CCustomer::E_USER_ERROR_EXIT)
			std::cerr<<" user's code="<<(unsigned)_recv_arg->FUserCode;//user's code from udt_example_protocol.h
	std::cerr<< std::endl;


	return 0;
}
extern void doing_something()
{
	//event loop
	CCustomer::sMGetInstance().MJoin();
}

