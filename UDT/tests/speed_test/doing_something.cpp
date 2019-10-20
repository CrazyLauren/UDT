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
#include <deftype>
#include <customer.h>

#include "speed_test_api.h"

namespace test_of_speed
{
using namespace NUDT;

static double g_test_working_time = 60;///< Time of working test
static NSHARE::CMutex g_stream_mutex;///< A mutex for lock console output
static double g_time = 0.0;///< Current time
static double g_start_time = 0.0;///< Start time of the test
static double g_last_print_time = 0.0;///< Current time

static unsigned long long g_recv_bytes = 0;///< Amount of received bytes
static unsigned g_amount_of_messages = 0;///< Amount of received messages
static unsigned g_amount_of_doesnt_send = 0;///< Amount of not sent message
static unsigned g_amount_of_doesnt_allocated = 0;///< Amount of not allocated buffers

extern int msg_speed_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t const* const _recv_arg = (received_message_args_t const*) aWHAT;

	/*! 2) Update information about amount of received bytes */
	NSHARE::CBuffer::size_type const _data_size = _recv_arg->FMessage.FBuffer.size();
	g_recv_bytes += _data_size;

	/*! 3) Calculating current time */
	double const _current_time = NSHARE::get_time();

	/*! 3) Print result every 2 seconds */
	if ((_current_time - g_last_print_time) >= 2.)
	{
		g_last_print_time = _current_time;

		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		std::cout << "Receive <==" << (g_recv_bytes / 1024 / 1024)
				<< " md; Average speed ="
				<< ((g_recv_bytes / 1024.0 / 1024.0)
						/ (_current_time - g_start_time)) << " mb/s."
				<< std::endl;
		std::cout << "Messages=" << g_amount_of_messages << "; fail sent="
				<< g_amount_of_doesnt_send << "; fail allocated="
				<< g_amount_of_doesnt_allocated << std::endl;

	}
	++g_amount_of_messages;
	return 0;
}
extern int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::subcribe_receiver_args_t structure */
	subcribe_receiver_args_t const* _recv_arg=(subcribe_receiver_args_t const*)aWHAT;


	/*! 2) Print information about new receivers and what messages they wanted to receive.*/
	for(auto const&_it:_recv_arg->FReceivers)
	{
		std::cout <<"*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*"<< std::endl;
		std::cout << "Now " << _it.FWho << " receive " << _it.FWhat.FRequired
				<< " from me by " << _it.FWhat.FProtocolName << " As its Request "
				<< _it.FWhat.FFrom << std::endl;
		std::cout <<"-------------------------------------"<< std::endl;
	}
	return 0;
}
extern int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	std::cout << "The udt library has been connected.." << std::endl;
	return 0;
}
extern int event_disconnect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	std::cout << "The udt library has been disconnected.."<< std::endl;
	return 0;
}
extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::fail_sent_args_t structure */
	fail_sent_args_t const* _recv_arg = (fail_sent_args_t const*) aWHAT;

	/*! 2) Print information about for whom messages aren't delivered .*/
	std::cerr << "The packet " << _recv_arg->FPacketNumber
			<< " has not been delivered to ";

	for (auto const& _it :_recv_arg->FFails)
	{
		std::cerr << (_it) << ", ";
	}

	/*! 3) Print information about why the messages aren't delivered .*/
	CCustomer::sMPrintError(std::cerr, _recv_arg->FErrorCode);

	if (_recv_arg->FErrorCode & CCustomer::E_USER_ERROR_EXIST)
		std::cerr << " user's code=" << (unsigned) _recv_arg->FUserCode;
	std::cerr << std::endl;

	return 0;
}
extern int event_customers_update_handler(CCustomer* WHO, void* aWHAT,
		void* YOU_DATA)
{
	customers_updated_args_t const* _recv_arg =
			(customers_updated_args_t*) aWHAT;

	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
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

	return 0;
}
extern void send_messages()
{
	///Algorithm:
	g_time = NSHARE::get_time();
	g_start_time = NSHARE::get_time();
	g_last_print_time= NSHARE::get_time();
	for (unsigned i=0;
		(i%10000)!=0//Check only every 10000 sent message
		|| (NSHARE::get_time()-g_start_time)<g_test_working_time//stop after timeout
		;++i)
	{
		///1) allocate the buffer for msg
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
				g_buf_size);

		/*! 2) Send the message number #MESSAGE_NUMBER */
		if (!_buf.empty())
		{
			if (CCustomer::sMGetInstance().MSend(MESSAGE_NUMBER, _buf) < 0)
				++g_amount_of_doesnt_send;
		}
		else
			++g_amount_of_doesnt_allocated;
	};

	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		std::cout << "Receive <==" << (g_recv_bytes / 1024 / 1024)
			<< " md; Average speed ="
			<< ((g_recv_bytes / 1024.0 / 1024.0)
				/ (NSHARE::get_time() - g_start_time)) << " mb/s."
			<< std::endl;
		std::cout << "Messages=" << g_amount_of_messages << "; fail sent="
			<< g_amount_of_doesnt_send << "; fail allocated="
			<< g_amount_of_doesnt_allocated << std::endl;
		std::cout << "Press any key... " << std::endl;
		getchar();
	}
}
}

