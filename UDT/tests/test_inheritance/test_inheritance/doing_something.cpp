// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * doing_something.cpp
 *
 *  Created on: 12.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <customer.h>
#include <map>

#include <map>
#include <stdio.h>

#include <customer.h>

#include <protocol_inherited.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <pthread.h>
#	include <unistd.h>
#endif

#include "test_inheritance_api.h"

namespace test_inheritance
{
using namespace protocol_inherited;
using namespace NUDT;

static NSHARE::CMutex g_stream_mutex;///< A mutex for lock console output
static unsigned _amount_of_msg[E_MSG_LAST_NUMBER];///<Amount of received messages

/*!\brief function for common messages operation
 *
 *\param aMsg Number of message
 */
template<class TMsg>
static void receive_msg(received_data_t const& aMsg)
{
	typedef decltype(TMsg::FData) T;

	auto const _bytes= aMsg.FEnd-aMsg.FBegin;

	if (sizeof(T) != _bytes)
	{
		std::cerr << "Invalid size of message:" << sizeof(T) << "!=" << _bytes
				<< std::endl;
		throw std::invalid_argument("Invalid argument");
	}
	const T* _p = (T*)aMsg.FBegin;
	if (strcmp(_p->FMsg,_p->MString())!=0)
	{
		std::cerr << "Invalid message:" << _p->FMsg<< "!=" << _p->MString()
				<< std::endl;
		throw std::invalid_argument("Invalid argument");
	}

	++_amount_of_msg[TMsg::eType];
};
extern int msg_grand_child_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t const * _recv_arg=(received_message_args_t*)aWHAT;

	/*! 2) Handle the received data */
	receive_msg<grand_child_msg_t>(_recv_arg->FMessage);

	std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
			<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
			<< " bytes received from " << _recv_arg->FFrom << " by "
			<< _recv_arg->FProtocolName << std::endl;


	return 0;
}
extern int msg_child_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{

	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t const * _recv_arg=(received_message_args_t*)aWHAT;

	/*! 2) Handle the received data */
	receive_msg<child_msg_t>(_recv_arg->FMessage);

	std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
			<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
			<< " bytes received from " << _recv_arg->FFrom << " by "
			<< _recv_arg->FProtocolName << std::endl;


	return 0;
}
extern int msg_parent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t const * _recv_arg=(received_message_args_t*)aWHAT;

	/*! 2) Handle the received data */
	receive_msg<parent_msg_t>(_recv_arg->FMessage);

	std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
			<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
			<< " bytes received from " << _recv_arg->FFrom << " by "
			<< _recv_arg->FProtocolName << std::endl;


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
template<class T>
bool send_message()
{
	typedef decltype(T::FData) data_t;


	NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			sizeof(data_t));

	data_t* _p = (data_t *) _buf.ptr();
	strcpy( _p->FMsg,_p->MString());

	required_header_t const _header(msg_head_t((eMsgType)T::eType, sizeof(T)));

	return CCustomer::sMGetInstance().MSend(_header, PROTOCOL_NAME, _buf)>0;
}
extern void send_messages()
{
	NSHARE::sleep(1);

	getchar();

	send_message<child_msg_t>();


	std::cout<<"Press any key... "<<std::endl;
	getchar();
}

}

