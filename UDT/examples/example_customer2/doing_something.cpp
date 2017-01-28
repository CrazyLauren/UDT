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
				std::cerr << "Fail data:"<<i<<"!="<<(*_it) << std::endl;
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
extern int msg_test2_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
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
		std::cout << "Now " << _it->FWho << " receive " << _it->FWhat.FRequired
				<< " from me by " << _it->FWhat.FProtocolName << " As its Request "
				<< _it->FRegExp << std::endl;
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
	
	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FTo.begin()),
			_it_end(_recv_arg->FTo.end());
	for (; _it != _it_end; ++_it)
	{
		std::cerr<<(*_it)<<", ";
	}
	
	std::cerr<<" by UDT kernel."<<std::endl;
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
		
		//!< Send the message number 0 (It's not necessary to specify the Receiver  
		//as If Somebody want to receive the message number 0 from us, It call method MIWantReceivingMSG and
		//specify receiving the message number 0 from us.)
		int _num = CCustomer::sMGetInstance().MSend(0, _buf);
		
		if (_num > 0)	//!<Hurrah!!! The data has been sent
		{
			//!<Warning!!! As The buffer is sent, it's freed. Thus calling _buf.size() return 0.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of " << PACKET_SIZE
					<< " bytes." << std::endl;
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

