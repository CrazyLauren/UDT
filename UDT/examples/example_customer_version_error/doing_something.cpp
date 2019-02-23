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

//!<It's some defenition for cross-platform code 
//of outputing to console.Do not take into account.
#ifdef _WIN32
#	include <windows.h>
::CRITICAL_SECTION _stream_mutex;
#	define STREAM_MUTEX_LOCK ::EnterCriticalSection(&_stream_mutex);
#	define STREAM_MUTEX_UNLOCK ::LeaveCriticalSection(&_stream_mutex);
#	ifdef uuid_t
#		undef uuid_t //"Fucking Microsoft! It's macro in lowercase."
#	endif
#else
#	include <pthread.h>
#	include <unistd.h>
pthread_mutex_t _stream_mutex;
#	define STREAM_MUTEX_LOCK pthread_mutex_lock( &_stream_mutex );
#	define STREAM_MUTEX_UNLOCK pthread_mutex_unlock( &_stream_mutex );
#	define Sleep(aVal) usleep((aVal)*1000)
#endif

using namespace NUDT;

extern int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	args_t const* _recv_arg=(args_t const*)aWHAT;

	//!<Now You can handle the received data.
	{
		const uint8_t* _it=_recv_arg->FBegin;
		for(int i=0;_it!=_recv_arg->FEnd;++i,++_it)
		{
			if (i%255 != *_it)
			{
				STREAM_MUTEX_LOCK
				std::cerr << "Fail data:"<<i<<"!="<<(unsigned)(*_it) << std::endl;
				STREAM_MUTEX_UNLOCK
				std::abort();
			}
		}
		
		//!<for optimization (decrease the number of operation 'copy') You can change the FBuffer field directly
	}
	
	STREAM_MUTEX_LOCK
	std::cout << "Message #"<<_recv_arg->FPacketNumber<<" ver "<<_recv_arg->FVersion<<" size "<<_recv_arg->FBuffer.size()<<" bytes received from "<<_recv_arg->FFrom<<" by "<<_recv_arg->FProtocolName<< std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	new_receiver_args_t* _recv_arg=(new_receiver_args_t*)aWHAT;
	
	STREAM_MUTEX_LOCK
	std::vector<new_receiver_args_t::what_t>::const_iterator _it(
			_recv_arg->FReceivers.begin()), _it_end(
			_recv_arg->FReceivers.end());
	for(;_it!=_it_end;++_it)
	{
		std::cout <<"*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*"<< std::endl;
		std::cout << "Now " << _it->FWho << " receive " << _it->FWhat.FRequired
				<< " from me by " << _it->FWhat.FProtocolName << " As its Request "
				<< _it->FRegExp << std::endl;
		std::cout <<"-------------------------------------"<< std::endl;
	}

	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	fail_sent_args_t* _recv_arg=(fail_sent_args_t*)aWHAT;

	STREAM_MUTEX_LOCK
	std::cerr<<"The packet "<<_recv_arg->FPacketNumber<<" ver "<<_recv_arg->FVersion<<" has not been delivered to ";
	
	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FFails.begin()),
			_it_end(_recv_arg->FFails.end());
	for (; _it != _it_end; ++_it)
	{
		std::cerr<<(*_it)<<", ";
	}
	
	std::cerr<<" by UDT kernel as ";
	CCustomer::sMPrintError(std::cerr,_recv_arg->FErrorCode);
	std::cerr<<std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}

#define PACKET_SIZE 200000

extern void doing_something()
{

	//!< Wait for connected to UDT
	for (; !CCustomer::sMGetInstance().MIsConnected(); Sleep(1000))
		;

	CCustomer::sMGetInstance().MJoin();
}

