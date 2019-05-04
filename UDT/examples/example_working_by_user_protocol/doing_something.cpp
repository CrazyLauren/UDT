// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
#include <stdexcept>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <pthread.h>
#	include <unistd.h>
#endif

namespace example_of_using_of_user_protocol
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
using namespace example_of_user_protocol;

extern int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t * _recv_arg = (received_message_args_t*) aWHAT;

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
			throw std::invalid_argument("Invalid argument");
		}
	}

	STREAM_MUTEX_LOCK
	std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
			<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
			<< " bytes received from " << _recv_arg->FFrom << " by "
			<< _recv_arg->FProtocolName << std::endl;
	STREAM_MUTEX_UNLOCK

	return 0;
}

extern int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::subcribe_receiver_args_t structure */
	subcribe_receiver_args_t const* _recv_arg=(subcribe_receiver_args_t const*)aWHAT;

	STREAM_MUTEX_LOCK

	/*! 2) Print information about new receivers and what messages they wanted to receive.*/
	std::vector<subcribe_receiver_args_t::what_t>::const_iterator _it(
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
extern int event_remove_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::subcribe_receiver_args_t structure */
	subcribe_receiver_args_t const* _recv_arg=(subcribe_receiver_args_t const*)aWHAT;

	/*! 2) Print information about removed receivers and what messages they doesn't
	 * wanted to receive.*/

	STREAM_MUTEX_LOCK
	std::vector<subcribe_receiver_args_t::what_t>::const_iterator _it(
			_recv_arg->FReceivers.begin()), _it_end(
			_recv_arg->FReceivers.end());
	for(;_it!=_it_end;++_it)
	{
		std::cout <<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<< std::endl;
		std::cout << "Now " << _it->FWho << " doesn't receive " << _it->FWhat.FRequired
				<< " by " << _it->FWhat.FProtocolName << " As its Request "
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

	/*! 2) Print information about for whom messages aren't delivered .*/
	STREAM_MUTEX_LOCK
	std::cerr<<"The packet "<<_recv_arg->FPacketNumber<<" has not been delivered to ";

	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FFails.begin()),
			_it_end(_recv_arg->FFails.end());
	for (; _it != _it_end; ++_it)
	{
		std::cerr<<(*_it)<<", ";
	}

	/*! 3) Print information about why the messages aren't delivered .*/
	CCustomer::sMPrintError(std::cerr,_recv_arg->FErrorCode);

	if(_recv_arg->FErrorCode & CCustomer::E_USER_ERROR_EXIST)
				std::cerr<<" user's code="<<(unsigned)_recv_arg->FUserCode;
	std::cerr<< std::endl;

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

void send_buffer_which_is_consist_msg()
{
	/// 1) the first variant of sending (with buffer parsing)
	{
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			MSG_TEST_SIZE); ///< allocate the buffer for full msg
		//filing the head of msg
		msg_head_t* _msg = (msg_head_t*) (_buf.ptr());
		_msg->FType = E_MSG_TEST;
		_msg->FSize = MSG_TEST_SIZE;
		NSHARE::CBuffer::iterator _it = _buf.begin() + sizeof(msg_head_t),
				_it_end = _buf.end();
		for (int i = 0; _it != _it_end; ++i, ++_it)
		{
			*_it = i % 255;
		}
		//send the data
		int _num = CCustomer::sMGetInstance().MSend(PROTOCOL_NAME, _buf);
		if (_num > 0)        //Hurrah!!! The data has been sent
		{
			//Warning!!! As The buffer is sent, it's freed. Thus calling _buf.size() return 0.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of "
					<< MSG_TEST_SIZE << " bytes." << std::endl;
			STREAM_MUTEX_UNLOCK
		}
		else        //The buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}
	}
}

void send_message()
{
	///Algorithm:
	///\warning The message data and header is sent separately
	{
		///1)  allocate the buffer for message without header
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			MSG_TEST_SIZE-sizeof(msg_head_t));

		{/// 2) Filling message into allocated buffer (without header)
			NSHARE::CBuffer::iterator _it = _buf.begin(), _it_end = _buf.end();
			for (int i = 0; _it != _it_end; ++i, ++_it)
			{
				*_it = i % 255;
			}
		}
		///3) Filing the header of message
		required_header_t const _header(msg_head_t(E_MSG_TEST, MSG_TEST_SIZE));

		///4) Send the message number #example_of_user_protocol::E_MSG_TEST of user protocol #example_of_user_protocol::PROTOCOL_NAME

		int _num = CCustomer::sMGetInstance().MSend(_header, PROTOCOL_NAME,
				_buf);

		if (_num > 0)	///5.a) If  NUDT::CCustomer::MSend() returns value greater zero when
						///The data has been sent successfully.
		{
			///\warning Warning!!! As The buffer is sent, it's freed.
			///Thus calling NSHARE::CBuffer::empty() return true.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of " << MSG_TEST_SIZE
					<< " bytes." << std::endl;
			STREAM_MUTEX_UNLOCK

		}
		else ///5.b) In the other case. NUDT::CCustomer::MSend() returns error code
			///It should be noted that the buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}
	}
}

extern void send_messages()
{
	///There two variant of sending:
	for (;;
			Sleep(1000)///Send message every 1 second
			)
	{

		/// The first variant of sending send the buffer
		/// which is consist the message
		send_buffer_which_is_consist_msg();

		/// The second variant of sending send the message
		send_message();

		/*! Notice what display the function #msg_test_handler
		 * for two variant of sending (size of data in #received_data_t::FBuffer)
		 */
	};

}
}
