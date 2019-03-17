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
	received_message_args_t const* _recv_arg=(received_message_args_t const*)aWHAT;

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
	_recv_arg->FOccurUserError=10;//triggering error. see event_fail_sent_handler
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
	if(!_recv_arg->FReceivers.empty())
	{
		std::cout <<"Now Sent to =====> "<<_recv_arg->FReceivers.back().FWho<< std::endl;
	}
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
	std::cerr << " by UDT kernel as ";
	CCustomer::sMPrintError(std::cerr,_recv_arg->FErrorCode);

	if(_recv_arg->FErrorCode & CCustomer::E_USER_ERROR_EXIST)
			std::cerr<<" user's code="<<(unsigned)_recv_arg->FUserCode;//user's code see _recv_arg->FOccurUserError
	std::cerr<< std::endl;

	STREAM_MUTEX_UNLOCK
	return 0;
}
#define PACKET_SIZE 100000
#define MESSAGE_NUMBER 1

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
				
		for (;_buf.empty();Sleep(1))	//!< may be 'malloc' return NULL, trying again
		{
			std::cerr << "Cannot allocate the buffer. " << std::endl;
			_buf = CCustomer::sMGetInstance().MGetNewBuf(
							PACKET_SIZE);
		}

		
		{//!< Filling message
			NSHARE::CBuffer::iterator _it=_buf.begin(),_it_end=_buf.end();
			for(int i=0;_it!=_it_end;++i,++_it)
			{
				*_it=i%255;
			}
		}
		
		//!< Send the message number 0 to uuid 
		int _num = CCustomer::sMGetInstance().MSend(MESSAGE_NUMBER, _buf,NSHARE::version_t(1,2));
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

