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
#include <SHARE/random_value.h>
#include <customer.h>
#include <UDT/IRtc.h>
#include <thread>
#include "api_test.h"

using namespace NSHARE;
using namespace NUDT;
namespace test_rtc_create
{
static NSHARE::CMutex g_mutex(NSHARE::CMutex::MUTEX_NORMAL);
static NSHARE::CCondvar g_convar;

static NSHARE::CText g_rtc_full_name;
static bool g_is_working = true;
static bool g_is_rtc_working = false;


static int msg_control_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA);
static void timer_test_1();

static int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	CRAII<CMutex> _block(g_mutex);

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
	g_is_rtc_working=false;
	return 0;
}
static void subscribe_to_msg()
{
	requirement_msg_info_t _msg;
	((msg_head_t*) _msg.FRequired.FMessageHeader)->FType = E_MSG_RTC_CONTROL;
	_msg.FProtocolName = PROTOCOL_NAME;
	_msg.FFrom = g_subscriber_name;

	callback_t _handler(msg_control_handler, NULL);

	CCustomer::sMGetInstance().MIWantReceivingMSG(_msg, _handler);
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

	//Starting the 'main loop' of UDT library
	CCustomer::sMGetInstance().MOpen();
	{
		LOCK_STREAM
		std::cout<<"Wait for connect ..."<<std::endl;
	}
	NSHARE::Strings _events={CCustomer::EVENT_CONNECTED,CCustomer::EVENT_UPDATE_RTC_INFO};

	CCustomer::sMGetInstance().MWaitForEvent(_events);
	{
		LOCK_STREAM
		std::cout<<"Connected and RTC info updated ..."<<std::endl;
	}
	subscribe_to_msg();
}

void start_rtc_user(int argc, char const *argv[], char const * aName)
{
	{
		LOCK_STREAM
		std::cout<<"\t started:"<<NSHARE::CThread::sProcessId()<<" ("<<g_name<<")"<<std::endl;
	}
	initialize(argc, aName, argv);
	{
		CRAII<CMutex> _block(g_mutex);
		for (; g_is_working;)
			g_convar.MTimedwait(&g_mutex);
	}
    {
        LOCK_STREAM
        std::cout <<"close socket"<<std::endl;
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
        {
            LOCK_STREAM
            std::cout << "Finish now"<< std::endl;
        }
		CRAII<CMutex> _block(g_mutex);
		g_is_working = false;
		g_is_rtc_working=false;
		g_convar.MBroadcast();
	}else if (_control->FCommand.MGetFlag(msg_control_t::eSTOP_TIMER_TEST))
	{
		g_is_rtc_working=false;
		{
			LOCK_STREAM
			std::cout << "Timer test stopped " << _control->FName << std::endl;
		}
	}else if (_control->FCommand.MGetFlag(msg_control_t::eGO_TIMER_TEST_1))
	{
		g_rtc_full_name = _control->FName;
		{
			LOCK_STREAM
			std::cout << "Start test 1 for RTC: " << g_rtc_full_name<< std::endl;
		}
		g_is_rtc_working = true;
		std::thread _thread(timer_test_1);
		_thread.detach();
	}
	return 0;
}
int timer_integer_impl(void* WHO, void* WHAT, void* YOU_DATA)
{
	IRtc * _rtc =(IRtc *)YOU_DATA;
	IRtc::millisecond_t & _time=*((IRtc::millisecond_t*)WHAT);
	LOG(INFO)<<"Timer is waked up "<<_time;

	IRtc::millisecond_t _div=0;
	while ((_div = (NSHARE::get_random_value_by_RNG() % 1000000))
			<= _rtc->MGetPrecisionMs())
		;

	_time+=_div;

	send_nextime(_time);

	return NSHARE::E_CB_SAFE_IT;
}
int timer_double_impl(void* WHO, void* WHAT, void* YOU_DATA)
{
	IRtc * _rtc =(IRtc *)YOU_DATA;

	IRtc::time_in_second_t & _time=*((IRtc::time_in_second_t*)WHAT);
	LOG(INFO)<<"Timer is waked up "<<_time;

	IRtc::time_in_second_t _div=0;
	while ((_div = (NSHARE::get_random_value_by_RNG() % 1000000)/1000.0/1000.0)
			<= _rtc->MGetPrecision())
		;

	_time+=_div;

	send_nextime(_time);

	return NSHARE::E_CB_SAFE_IT;
}
/** Thread loop for test integer timer
 *
 */
void timer_test_1()
{
	IRtc * _rtc = CCustomer::sMGetInstance().MWaitForRTCCreated(g_rtc_full_name);
	CHECK_NOTNULL(_rtc);
	const bool _is = _rtc->MJoinToRTC();
	CHECK(_is);

	IRtc::millisecond_t const _next_time = (IRtc::millisecond_t) (g_start_time
			* 1000.0 * 1000.0);
	send_nextime(_next_time);
	_rtc->MSetTimer(_next_time, 1, callback_rtc_t(timer_integer_impl, _rtc));

	for (; g_is_rtc_working && _rtc != NULL && _rtc->MIsJoinToRTC();)
	{
		_rtc->MNextTime(-1.);
	}
	if(_rtc != NULL)
		_rtc->MLeaveFromRTC();
	{
		LOCK_STREAM
		std::cout << "Stop loop "<< std::endl;
	}
}
}
