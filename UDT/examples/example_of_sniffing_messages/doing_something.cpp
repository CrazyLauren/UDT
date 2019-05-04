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
#include <stdexcept>
#include <udt_example_protocol.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <pthread.h>
#	include <unistd.h>
#endif

#include "api_example_sniffer.h"

namespace example_of_sniffing_messages
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
extern int sniffer_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t const* _recv_arg =
			(received_message_args_t const*) aWHAT;

	/*! 2) Printing information about packet*/
	{
		STREAM_MUTEX_LOCK
		std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
				<< _recv_arg->FHeader.FVersion << " by "
				<< _recv_arg->FProtocolName << " size "
				<< _recv_arg->FMessage.FBuffer.size() << " bytes sniffed from "
				<< _recv_arg->FFrom << " to ";

		std::vector<NSHARE::uuid_t>::const_iterator _it =
				_recv_arg->FTo.begin(), _it_end = _recv_arg->FTo.end();

		for (; _it != _it_end; ++_it)
		{
			std::cout << *_it << ", ";
		}
		std::cout << std::endl;
		STREAM_MUTEX_UNLOCK
	}

	/*! 3) Handle the received data which is located
	 * from NUDT::received_data_t::FBegin
	 * to NUDT::received_data_t::FEnd*/
	{
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
	}
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
}
