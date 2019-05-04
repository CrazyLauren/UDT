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
#include <map>
#include <stdio.h>

#include <customer.h>
#include <udt_example_protocol.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <pthread.h>
#	include <unistd.h>
#endif

#include "api_example_sendto.h"

namespace example_of_sendto_msg
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

NSHARE::uuid_t g_sent_to(0);///< A uuid of receiver

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

	/*! 3) Keep information about uuid of the receiver, that to send message to it only.*/
	if(!_recv_arg->FReceivers.empty())
	{
		g_sent_to=_recv_arg->FReceivers.back().FWho;
		std::cout <<"Now The message sends to =====> "<<g_sent_to<< std::endl;
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

	/*! 3) Remove information about uuid of the receiver if to it the message is sent.*/
	if (g_sent_to.MIsValid())
	{
		_it = _recv_arg->FReceivers.begin();
		for (; _it != _it_end; ++_it)
		{
			if (g_sent_to == _it->FWho)
			{
				std::cout <<"Now the message doesn't send to <===== "<<g_sent_to<< std::endl;
				g_sent_to = 0;
				break;
			}
		}
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

	/*! 2) Print information about for whom the messages aren't delivered .*/
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

	/*! 4) Remove information about uuid of the receiver if to it the message isn't delivered.*/

	if (g_sent_to.MIsValid())
	{
		_it=_recv_arg->FFails.begin();
		for (; _it != _it_end; ++_it)
		{
			if (g_sent_to == *_it)
			{
				std::cerr <<"Now the message doesn't send to <===== "<<g_sent_to<< std::endl;
				g_sent_to = 0;
				break;
			}
		}
	}

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

	for (;; )
	{
		///1) Wait for user Press any key and no receiver of message

		STREAM_MUTEX_LOCK
		std::cout << "Wait for message receiver " << std::endl;
		STREAM_MUTEX_UNLOCK

		for (; !g_sent_to.MIsValid(); Sleep(1000))
		{
			STREAM_MUTEX_LOCK
			std::cout << ".";
			STREAM_MUTEX_UNLOCK
		}

		STREAM_MUTEX_LOCK
		std::cout << std::endl << "Press any key to send message to "
				<< g_sent_to << std::endl;
		STREAM_MUTEX_UNLOCK
		getchar();

		if(!g_sent_to.MIsValid())
			continue;

		///2)  allocate the buffer for message without header
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			MSG_TEST_SIZE-sizeof(msg_head_t));

		/// 3) Filling message into allocated buffer (without header)
		{
			NSHARE::CBuffer::iterator _it=_buf.begin(),_it_end=_buf.end();
			for(int i=0;_it!=_it_end;++i,++_it)
			{
				*_it=i%255;
			}
		}
		///4) Filing the header of message #example_of_user_protocol::E_MSG_TEST of version 1.2
		required_header_t const _header(
				msg_head_t(example_of_user_protocol::E_MSG_TEST,
					MSG_TEST_SIZE),
						NSHARE::version_t(1,2));

		///5) Send the message number #example_of_user_protocol::E_MSG_TEST of user protocol #example_of_user_protocol::PROTOCOL_NAME

		int _num = CCustomer::sMGetInstance().MSend(_header,PROTOCOL_NAME, _buf,g_sent_to);
		
		if (_num > 0)	///5.a) If  NUDT::CCustomer::MSend() returns value greater zero when
						///The data has been sent successfully.
		{
			///\warning Warning!!! As The buffer is sent, it's freed.
			///Thus calling NSHARE::CBuffer::empty() return true.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << std::endl;
			STREAM_MUTEX_UNLOCK

		}
		else ///5.b) In the other case. NUDT::CCustomer::MSend() returns error code
			///It should be noted that the buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}

	};

}
}
