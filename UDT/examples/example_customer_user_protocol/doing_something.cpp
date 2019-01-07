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

#include <udt_example_protocol.h>
#ifdef _WIN32
#include <windows.h>
::CRITICAL_SECTION _stream_mutex;
#define STREAM_MUTEX_LOCK ::EnterCriticalSection(&_stream_mutex);
#define STREAM_MUTEX_UNLOCK ::LeaveCriticalSection(&_stream_mutex);
#ifdef uuid_t
#	undef uuid_t //"Fucking Microsoft! It's macro in lowercase."
#endif

#else
#include <pthread.h>
#include <unistd.h>
pthread_mutex_t _stream_mutex;
#define STREAM_MUTEX_LOCK pthread_mutex_lock( &_stream_mutex );
#define STREAM_MUTEX_UNLOCK pthread_mutex_unlock( &_stream_mutex );
#define Sleep(aVal) usleep((aVal)*1000)
#endif
using namespace NUDT;



extern int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	args_t const* _recv_arg=(args_t const*)aWHAT;
	const uint8_t* _it=_recv_arg->FHeaderBegin;
	_it+=sizeof(msg_head_t);
	if (_it != _recv_arg->FBegin)
	{
		STREAM_MUTEX_LOCK
		std::cerr << "Incorrect header size" << std::endl;
		STREAM_MUTEX_UNLOCK
		std::abort();
	}
	else
		for (int i = 0; _it != _recv_arg->FEnd; ++i, ++_it)
		{
			if (i % 255 != *_it)
			{
				STREAM_MUTEX_LOCK
				std::cerr << "Fail data:" << i << "!=" << (*_it) << std::endl;
				STREAM_MUTEX_UNLOCK
				std::abort();
			}
		}
	//for optimization (decrease the number of operation 'copy') You can change the FBuffer field.
	//Warning!!! The FBegin and FEnd fields always point to the received data
	//but sometimes The buffer FBuffer can be empty.
	STREAM_MUTEX_LOCK
	std::cout << "Receiving packet #"<<_recv_arg->FPacketNumber<<" size "<<_recv_arg->FBuffer.size()<<" bytes from "<<_recv_arg->FFrom<<" by "<<_recv_arg->FProtocolName<< std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int sniffer_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	args_t const* _recv_arg = (args_t const*)aWHAT;
	//!<Now You can handle the received data.

	STREAM_MUTEX_LOCK
	std::cout << "Message #" << _recv_arg->FPacketNumber<<" by " << _recv_arg->FProtocolName << " size "
			<< _recv_arg->FBuffer.size() << " bytes sniffed from "
			<< _recv_arg->FFrom <<" to ";
	std::vector<NSHARE::uuid_t>::const_iterator _it = _recv_arg->FTo.begin(),
			_it_end = _recv_arg->FTo.end();

	for(;_it!=_it_end;++_it)
	{
		std::cout << *_it<<", ";
	}
	std::cout << std::endl;
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
				<< " by " << _it->FWhat.FProtocolName << " As in "
				<< _it->FRegExp << std::endl;
		std::cout <<"-------------------------------------"<< std::endl;
	}

	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	STREAM_MUTEX_LOCK
	std::cout << "The udt library has been connected.."<< std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	fail_sent_args_t* _recv_arg=(fail_sent_args_t*)aWHAT;

	STREAM_MUTEX_LOCK
	std::cerr<<"The packet "<<_recv_arg->FPacketNumber<<" has not been delivered to ";
	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FFails.begin()),
			_it_end(_recv_arg->FFails.end());

	for (; _it != _it_end; ++_it)
	{
		std::cerr<<(*_it)<<", ";
	}
	std::cerr<<" by UDT."<<std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_customers_update_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	customers_updated_args_t const* _recv_arg=(customers_updated_args_t*)aWHAT;
	STREAM_MUTEX_LOCK
	if(!_recv_arg->FConnected.empty())
	{
		std::set<program_id_t>::const_iterator _it=_recv_arg->FConnected.begin(),_it_end(_recv_arg->FConnected.end());
		std::cout << "Connected customers:";
		for (; _it != _it_end; ++_it)
		{
			std::cout<<"\t" << (*_it) << std::endl;
		}
		std::cout << " to UDT." << std::endl;
	}
	if(!_recv_arg->FDisconnected.empty())
	{
		std::cout << "Disconnected customers:";
		std::set<program_id_t>::const_iterator _it =
				_recv_arg->FDisconnected.begin(), _it_end(
				_recv_arg->FDisconnected.end());
		for (; _it != _it_end; ++_it)
		{
			std::cout << (*_it)<< std::endl;
		}
		std::cout<<"\t"  << " from UDT." << std::endl;
	}
	STREAM_MUTEX_UNLOCK
	return 0;
}


extern void doing_something()
{
#ifdef _WIN32
	::InitializeCriticalSection(&_stream_mutex);
#else
	pthread_mutex_init(&_stream_mutex, NULL);
#endif
	for (; !CCustomer::sMGetInstance().MIsConnected(); Sleep(1000))
		;

	for (;; Sleep(1000))
	{
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
				PACKET_SIZE);	//!< allocate the buffer for msg
		for (;_buf.empty();Sleep(1))	//!< may be 'malloc' return NULL
		{
			std::cerr << "Cannot allocate the buffer. " << std::endl;
			_buf = CCustomer::sMGetInstance().MGetNewBuf(
							PACKET_SIZE);
			continue;
		}

		//filing the head of msg
		msg_head_t *_msg = (msg_head_t*) _buf.ptr();
		_msg->FType = E_MSG_TEST;
		_msg->FSize = PACKET_SIZE;
		NSHARE::CBuffer::iterator _it=_buf.begin()+sizeof(msg_head_t),_it_end=_buf.end();
		for(int i=0;_it!=_it_end;++i,++_it)
		{
			*_it=i%255;
		}
		//send the data
		int _num = CCustomer::sMGetInstance().MSend(PROTOCOL_NAME, _buf);
		if (_num > 0)	//Hurrah!!! The data has been sent
		{
			//Warning!!! As The buffer is sent, it's freed. Thus calling _buf.size() return 0.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of " << PACKET_SIZE
					<< " bytes." << std::endl;
			STREAM_MUTEX_UNLOCK

		}
		else //The buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}

	};

}

