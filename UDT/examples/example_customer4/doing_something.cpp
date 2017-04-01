/*
 * test.cpp
 *
 *  Created on: 29.03.2016
 *      Author: Sergey
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
NSHARE::uuid_t g_sent_to(0);
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
	std::cout << "Message #"<<_recv_arg->FPacketNumber<<" size "<<_recv_arg->FBuffer.size()<<" bytes received from "<<_recv_arg->FFrom<<" by "<<_recv_arg->FProtocolName<< std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int group_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	args_t const* _recv_arg = (args_t const*)aWHAT;
	//!<Now You can handle the received data.
	
	STREAM_MUTEX_LOCK
		std::cout << "Message #" << _recv_arg->FPacketNumber << " size " << _recv_arg->FBuffer.size() << " bytes received from " << _recv_arg->FFrom << " by " << _recv_arg->FProtocolName << std::endl;
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
	if(!_recv_arg->FReceivers.empty())
	{
		std::cout <<"Now Sent to =====> "<<_recv_arg->FReceivers.back().FWho<< std::endl;
		g_sent_to=_recv_arg->FReceivers.back().FWho;
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
	g_sent_to=0;
	std::cerr<<" by UDT kernel as "<<_recv_arg->FErrorCode<<std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}

#define PACKET_SIZE 200000

extern void doing_something()
{
#ifdef _WIN32
	::InitializeCriticalSection(&_stream_mutex);
#else
	pthread_mutex_init(&_stream_mutex, NULL);
#endif

	//!< Wait for connected to UDT
	for (; !CCustomer::sMGetInstance().MIsConnected(); Sleep(1000))
		;

	for (;; Sleep(1000))
	{
		if(!g_sent_to.MIsValid())
			continue;

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
		int _num = CCustomer::sMGetInstance().MSend(0, _buf,g_sent_to);
		
		if (_num > 0)	//!<Hurrah!!! The data has been sent
		{
			//!<Warning!!! As The buffer is sent, it's freed. Thus calling _buf.size() return 0.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of " << PACKET_SIZE
					<< " bytes to "<<g_sent_to << std::endl;
			STREAM_MUTEX_UNLOCK

		}
		else //!<The buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}

	};

}

