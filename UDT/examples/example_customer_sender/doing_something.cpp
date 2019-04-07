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
#include <stdio.h>

///<It's some defenition for cross-platform code 
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
	
	std::cerr<<" by UDT kernel as ";
	CCustomer::sMPrintError(std::cerr,_recv_arg->FErrorCode);
	std::cerr<<std::endl;
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
		
		std::cout << "New customers in UDT:";
		for (; _it != _it_end; ++_it)
		{
			std::cout<<"\t" << (*_it) << std::endl;
		}
		std::cout <<std::endl;
	}
	if(!_recv_arg->FDisconnected.empty())
	{
		std::cout << "Some customers droped:";
		std::set<program_id_t>::const_iterator _it =
				_recv_arg->FDisconnected.begin(), _it_end(
				_recv_arg->FDisconnected.end());
		for (; _it != _it_end; ++_it)
		{
			std::cout << (*_it)<< std::endl;
		}
		std::cout<< std::endl;
	}
	STREAM_MUTEX_UNLOCK
	
	return 0;
}

#define PACKET_SIZE 200000
#define MESSAGE_NUMBER 1

extern void doing_something()
{
#ifdef _WIN32
	::InitializeCriticalSection(&_stream_mutex);
#else
	pthread_mutex_init(&_stream_mutex, NULL);
#endif

	///< Wait for connected to UDT
	for (; !CCustomer::sMGetInstance().MIsConnected(); Sleep(1000))
		;

	for (;; )
	{
		{
			STREAM_MUTEX_LOCK
			std::cout << "Press any key to send message" << std::endl;
			STREAM_MUTEX_UNLOCK
			getchar();
		}

		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
				PACKET_SIZE);	///< allocate the buffer for msg
				
		for (;_buf.empty();Sleep(1))	///< may be 'malloc' return NULL, trying again
		{
			std::cerr << "Cannot allocate the buffer. " << std::endl;
			_buf = CCustomer::sMGetInstance().MGetNewBuf(
							PACKET_SIZE);
		}

		
		{///< Filling message
			NSHARE::CBuffer::iterator _it=_buf.begin(),_it_end=_buf.end();
			for(int i=0;_it!=_it_end;++i,++_it)
			{
				*_it=i%255;
			}
		}
		
		///< Send the message number 0 ver 1.2 (It's not necessary to specify the Receiver  
		//as If Somebody want to receive the message number 0 from us, It call method MIWantReceivingMSG and
		//specify receiving the message number 0 from us.)
		int _num = CCustomer::sMGetInstance().MSend(MESSAGE_NUMBER, _buf,NSHARE::version_t(1,2));
		
		if (_num > 0)	///<Hurrah!!! The data has been sent
		{
			///<Warning!!! As The buffer is sent, it's freed. Thus calling _buf.size() return 0.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of " << PACKET_SIZE
					<< " bytes." << std::endl;
			STREAM_MUTEX_UNLOCK

		}
		else ///<The buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}

	};

}

