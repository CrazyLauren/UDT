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
#	include <windows.h>
#else
#	include <pthread.h>
#	include <unistd.h>
#endif

#include "api_example_receiver.h"

namespace example_customer_receiver
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

extern int group_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t const* _recv_arg = (received_message_args_t const*) aWHAT;

	/*! 2) Handle the received data which is located
	 * from NUDT::received_data_t::FBegin
	 * to NUDT::received_data_t::FEnd*/
	if (NSHARE::E_SHARE_ENDIAN == _recv_arg->FEndian)
	{
		STREAM_MUTEX_LOCK
		std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
				<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
				<< " bytes received from " << _recv_arg->FFrom << " by "
				<< _recv_arg->FProtocolName << std::endl;
		STREAM_MUTEX_UNLOCK

		const uint8_t* _it = _recv_arg->FMessage.FBegin;
		for (int i = 0; _it != _recv_arg->FMessage.FEnd; ++i, ++_it)
		{
			if (i % 255 != *_it)
			{
				STREAM_MUTEX_LOCK
				std::cerr << "Fail data:" << i << "!=" << (unsigned) (*_it)
						<< std::endl;
				STREAM_MUTEX_UNLOCK
				throw;
			}
		}
	}
	else
	{
		STREAM_MUTEX_LOCK
		std::cerr << "The byte endian of Message #" << _recv_arg->FPacketNumber
				<< " ver " << _recv_arg->FHeader.FVersion << " size "
				<< _recv_arg->FMessage.FBuffer.size() << " bytes received from "
				<< _recv_arg->FFrom << " by " << _recv_arg->FProtocolName
				<< " is not match" << std::endl;
		STREAM_MUTEX_UNLOCK

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
}

