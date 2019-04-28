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
#	include <windows.h>
#else
#	include <pthread.h>
#	include <unistd.h>
#endif

#include "api_example_customer.h"

namespace example_1
{
/*!\name It's some definition for cross-platform code
 * of outputing to console.Do not take into account.
 *
 *\{
 */
#ifdef _WIN32
struct _stream_block
{
	_stream_block()
	{
		::InitializeCriticalSection(&_stream_mutex);
	}
	::CRITICAL_SECTION _stream_mutex;
} _st_block;
#	define STREAM_MUTEX_LOCK ::EnterCriticalSection(&_st_block._stream_mutex);
#	define STREAM_MUTEX_UNLOCK ::LeaveCriticalSection(&_st_block._stream_mutex);
#	ifdef uuid_t
#		undef uuid_t //"Fucking Microsoft! It's macro in lowercase."
#	endif
#else
struct _stream_block
{
	_stream_block()
	{
		pthread_mutex_init(&_stream_mutex, NULL);
	}
	pthread_mutex_t _stream_mutex;
}_st_block;
#	define STREAM_MUTEX_LOCK pthread_mutex_lock( &_st_block._stream_mutex );
#	define STREAM_MUTEX_UNLOCK pthread_mutex_unlock( &_st_block._stream_mutex );
#	define Sleep(aVal) usleep((aVal)*1000)
#endif
/// \}

using namespace NUDT;

extern int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t * _recv_arg=(received_message_args_t*)aWHAT;

	/*! 2) Handle the received data which is located
	 * from NUDT::received_data_t::FBegin
	 * to NUDT::received_data_t::FEnd*/
	const uint8_t* _it = _recv_arg->FMessage.FBegin;
	for (int i = 0; _it != _recv_arg->FMessage.FEnd; ++i, ++_it)
	{
		if (i % 255 != *_it)
		{
			STREAM_MUTEX_LOCK
			std::cerr << "Fail data:" << i << "!=" << (unsigned) (*_it)
					<< std::endl;
			STREAM_MUTEX_UNLOCK
			std::abort();
		}
	}

	STREAM_MUTEX_LOCK
	std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
			<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
			<< " bytes received from " << _recv_arg->FFrom << " by "
			<< _recv_arg->FProtocolName << std::endl;
	STREAM_MUTEX_UNLOCK

	/*!For optimization (decrease the number of operation 'copy')
	 * You can change the FBuffer field directly
	 */
	NSHARE::CBuffer _copy=_recv_arg->FMessage.FBuffer;
	_copy.pop_front();

	return 0;
}

extern int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
<<<<<<< HEAD
	new_receiver_args_t* _recv_arg=(new_receiver_args_t*)aWHAT;
	
	STREAM_MUTEX_LOCK
	std::vector<new_receiver_args_t::what_t>::const_iterator _it(
=======
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::subcribe_receiver_args_t structure */
	subcribe_receiver_args_t const* _recv_arg=(subcribe_receiver_args_t const*)aWHAT;
	
	STREAM_MUTEX_LOCK

	/*! 2) Print information about new receivers and what messages they wanted to receive.*/
	std::vector<subcribe_receiver_args_t::what_t>::const_iterator _it(
>>>>>>> 8543561... see ChangeLog.txt
			_recv_arg->FReceivers.begin()), _it_end(
			_recv_arg->FReceivers.end());
	for(;_it!=_it_end;++_it)
	{
		std::cout <<"*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*"<< std::endl;
		std::cout << "Now " << _it->FWho << " receive " << _it->FWhat.FRequired
				<< " from me by " << _it->FWhat.FProtocolName << " As its Request "
				<< _it->FWhat.FFrom << std::endl;
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
extern int event_disconnect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	STREAM_MUTEX_LOCK
	std::cout << "The udt library has been disconnected.."<< std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::fail_sent_args_t structure */
	fail_sent_args_t const* _recv_arg=(fail_sent_args_t const*)aWHAT;

	/*! 2) Print information about not delivered messages.*/
	STREAM_MUTEX_LOCK
	std::cerr<<"The packet "<<_recv_arg->FPacketNumber<<" has not been delivered to ";
	
	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FFails.begin()),
			_it_end(_recv_arg->FFails.end());
	for (; _it != _it_end; ++_it)
	{
		std::cerr<<(*_it)<<", ";
	}
	
	std::cerr<<" by UDT kernel as "<<_recv_arg->FErrorCode<<std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_customers_update_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::customers_updated_args_t structure */
	customers_updated_args_t const* _recv_arg=(customers_updated_args_t*)aWHAT;
	
	/*! 2) Print information about connected programs.*/
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
	/*! 3) Print information about disconnected programs.*/
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

extern void send_messages()
{
	///Algorithm:
	for (;;
			Sleep(1000)///Send message every 1 second
			)
	{

		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
				PACKET_SIZE);	///1) allocate the buffer for msg
				
		{/// 2) Filling message into allocated buffer
			NSHARE::CBuffer::iterator _it=_buf.begin(),_it_end=_buf.end();
			for(int i=0;_it!=_it_end;++i,++_it)
			{
				*_it=i%255;
			}
		}
		
		/*! 3) Send the message number #MESSAGE_NUMBER ver 1.2 (If Somebody want to receive
		 *the message number #MESSAGE_NUMBER from us, It call method
		 *NUDT::CCustomer::MIWantReceivingMSG and
		 *specify receiving the message number #MESSAGE_NUMBER from us.)
		 */
		int _num = CCustomer::sMGetInstance().MSend(MESSAGE_NUMBER, _buf,NSHARE::version_t(1,2));

		if (_num > 0)	///4.a) If  NUDT::CCustomer::MSend() returns value greater zero when
						///The data has been sent successfully.
		{
			///\warning Warning!!! As The buffer is sent, it's freed.
			///Thus calling NSHARE::CBuffer::empty() return true.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of " << PACKET_SIZE
					<< " bytes." << std::endl;
			STREAM_MUTEX_UNLOCK

		}
		else ///4.b) In the other case. NUDT::CCustomer::MSend() returns error code
			///It should be noted that the buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}
	};
}
}
