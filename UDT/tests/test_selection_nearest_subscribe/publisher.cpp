// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * publisher.cpp
 *
 *  Created on: 20.01.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <customer.h>
#include "protocol.h"

using namespace NSHARE;
using namespace NUDT;
namespace test_selection
{
static NSHARE::CMutex g_mutex(NSHARE::CMutex::MUTEX_NORMAL);
static NSHARE::CCondvar g_convar;

extern NSHARE::CIPCSem g_mutex_stream;
extern std::string g_name;
#define LOCK_STREAM CRAII<CIPCSem> _block(g_mutex_stream); std::cout<<std::endl <<"Publisher "<<g_name<<" say:"<<std::endl;

static bool g_is_working = false;

extern std::string const g_subscriber_name;

static int msg_control_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA);
static int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA);
static int event_remove_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA);

static int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	CRAII<CMutex> _block(g_mutex);
	g_is_working = true;

	{
		LOCK_STREAM
		std::cout << "The udt library has been connected.."<< std::endl;
	}
	g_convar.MBroadcast();
	return 0;
}
static int event_disconnect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	{
		LOCK_STREAM
		std::cout << "The udt library has been disconnected.."<< std::endl;
	}
	CRAII<CMutex> _block(g_mutex);
	g_convar.MBroadcast();
	g_is_working=false;
	return 0;
}

static void initialize(int argc, const char* aName, char const* argv[])
{
	const int _val = CCustomer::sMInit(argc, argv, aName,
			NSHARE::version_t(1, 0), "./default_customer_config.json"); ///< initialize UDT library
	if (_val != 0)
	{
		LOCK_STREAM
		std::cerr << "Cannot initialize library as " << _val << std::endl;
		exit(EXIT_FAILURE);
	}
	{
		///< When the UDT library will be connected to UDT kernel. The function
		//event_connect_handler is called.
		callback_t _handler_event_connect(event_connect_handler, NULL);
		event_handler_info_t _event_connect(CCustomer::EVENT_CONNECTED,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}
	{
		callback_t _handler_event_connect(event_disconnect_handler, NULL);
		event_handler_info_t _event_connect(CCustomer::EVENT_DISCONNECTED,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}
	{
		requirement_msg_info_t _msg;
		((msg_head_t*) _msg.FRequired.FMessageHeader)->FType = E_MSG_CONTROL;
		_msg.FProtocolName = PROTOCOL_NAME;
		_msg.FFrom = g_subscriber_name;

		callback_t _handler(msg_control_handler, NULL);

		CCustomer::sMGetInstance().MIWantReceivingMSG(_msg, _handler);
	}
	{
		///< When some consumers will start receiving data from me. The function
		//event_new_receiver is called.
		callback_t _handler_event_connect(event_new_receiver, NULL);
		event_handler_info_t _event_connect(CCustomer::EVENT_RECEIVER_SUBSCRIBE,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}
	{
		///< When some consumers will start receiving data from me. The function
		//event_new_receiver is called.
		callback_t _handler_event_disconnect(event_remove_receiver, NULL);
		event_handler_info_t _event_disconnect(
				CCustomer::EVENT_RECEIVER_UNSUBSCRIBE,
				_handler_event_disconnect);
		CCustomer::sMGetInstance() += _event_disconnect;
	}

	//Starting the 'main loop' of UDT library
	CCustomer::sMGetInstance().MOpen();
	{
		LOCK_STREAM
		std::cout<<"Wait for connect ..."<<std::endl;
	}
	CCustomer::sMGetInstance().MWaitForEvent(CCustomer::EVENT_CONNECTED);
	{
		LOCK_STREAM
		std::cout<<"Connected ..."<<std::endl;
	}
}

extern void start_publisher(int argc, char const *argv[], char const * aName)
{
	initialize(argc, aName, argv);
	{
		CRAII<CMutex> _block(g_mutex);
		for (; g_is_working;)
			g_convar.MTimedwait(&g_mutex);
	}
	CCustomer::sMGetInstance().MClose();
	{
		LOCK_STREAM
		std::cout <<"by"<<std::endl;
	}
}
static int msg_control_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	received_message_args_t const* _recv_arg =
			(received_message_args_t const*) aWHAT;
	const msg_control_t* _control =
			(const msg_control_t*) _recv_arg->FMessage.FBegin;

	{
		LOCK_STREAM
		std::cout << "Receiving packet #"<<_recv_arg->FPacketNumber<<" bytes from "
		<<_recv_arg->FFrom<<" by "<<_recv_arg->FProtocolName<<" command "<<_control->FCommand<< std::endl;
	}
	if (_control->FCommand.MGetFlag(msg_control_t::eFINISH))
	{
		CRAII<CMutex> _block(g_mutex);
		g_is_working = false;
		g_convar.MBroadcast();
	}
	if (g_is_working)
	{
		if (_control->FCommand.MGetFlag(msg_control_t::eSEND))
		{
			required_header_t _header;
			msg_head_t *_msg = (msg_head_t*) _header.FMessageHeader;
			_msg->FType = E_TEST_MSG;

			NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
					sizeof(msg_test_t));///< allocate the buffer for message without header

			int _num = CCustomer::sMGetInstance().MSend(_header, PROTOCOL_NAME,
					_buf);
			if (_num > 0)	//Hurrah!!! The data has been sent
			{
				LOCK_STREAM
				std::cout <<"Send test msg packet#" << _num <<std::endl;
			}
			else //The buffer _buf is not freed as it's not sent.
			{
				LOCK_STREAM
				std::cout << "Send error  " << _num << std::endl;
			}
		}
	}
	return 0;
}
static int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	subcribe_receiver_args_t* _recv_arg = (subcribe_receiver_args_t*) aWHAT;

	LOCK_STREAM

std	::cout << "subscribed: " << std::endl;

	for (auto& _it : _recv_arg->FReceivers)
	{
		std::cout << "-*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*" << std::endl;
		std::cout << "Now " << _it.FWho << " receive " << _it.FWhat.FRequired
				<< " by " << _it.FWhat.FProtocolName << " As I am in "
				<< _it.FWhat.FFrom << std::endl;
		std::cout << "-------------------------------------" << std::endl;
	}

	return 0;
}
static int event_remove_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	subcribe_receiver_args_t* _recv_arg = (subcribe_receiver_args_t*) aWHAT;

	LOCK_STREAM

std	::cout << "unsubscribed: " << std::endl;

	for (auto& _it : _recv_arg->FReceivers)
	{
		std::cout << "*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*" << std::endl;
		std::cout << "Now " << _it.FWho << " doesn't receive "
				<< _it.FWhat.FRequired << " by " << _it.FWhat.FProtocolName
				<< " As I am in " << _it.FWhat.FFrom << std::endl;
		std::cout << "-------------------------------------" << std::endl;

	}
	return 0;
}
}
