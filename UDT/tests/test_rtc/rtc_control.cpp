// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * rtc_control.cpp
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
#include <SHARE/tasks.h>
#include "api_test.h"

using namespace NSHARE;
using namespace NUDT;
namespace test_rtc
{
static std::vector<IRtc::millisecond_t> g_next_times;
static NSHARE::CMutex g_mutex(NSHARE::CMutex::MUTEX_NORMAL);
static NSHARE::CCondvar g_convar;
static IRtc * g_rtc=NULL;
static bool g_is_working=false;
static requirement_msg_info_t g_receive_what;
static const unsigned g_number_of_time=1500;
static void test();
static void doing_tests();
static int msg_next_time_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA);

template<typename _Function>
static bool wait_for(_Function aF,CMutex& aMutex,double aTime=-1)
{
	for (double const _cur_time=NSHARE::get_time(); aF();)
	{
		if(aTime<0)
			g_convar.MTimedwait(&aMutex);
		else
			if (!g_convar.MTimedwait(&aMutex,
				NSHARE::get_time() - _cur_time + aTime))
		{
			if((NSHARE::get_time() - _cur_time)>aTime)
				return false;
		}
	}
	return true;
}
template<typename _Function>
static bool wait_for(_Function aF,double aTime=-1)
{
	CRAII<CMutex> _block(g_mutex);
	wait_for(aF,g_mutex,aTime);
	return true;
}

/*static int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	CRAII<CMutex> _block(g_mutex);
	g_is_working=true;
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
}*/

static bool test_integer_timers();
static bool test_double_timers();
static void initialize(int argc, const char* aName, char const* argv[])
{
	const int _val = CCustomer::sMInit(argc, argv, aName,
			NSHARE::version_t(1, 0), "./default_customer_config.xml"); ///< initialize UDT library
	if (_val != 0)
	{
		LOCK_STREAM
		std::cerr << "Cannot initialize library as " << _val << std::endl;
		exit(EXIT_FAILURE);
	}
/*	{
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
	}*/

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
		//event_remove_receiver is called.
		callback_t _handler_event_disconnect(event_remove_receiver, NULL);
		event_handler_info_t _event_disconnect(CCustomer::EVENT_RECEIVER_UNSUBSCRIBE,
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
void subscribe_to_msg()
{
	((msg_head_t*) ((g_receive_what.FRequired.FMessageHeader)))->FType =
			E_MSG_RTC_NEXT_TIME;
	g_receive_what.FProtocolName = PROTOCOL_NAME;
	g_receive_what.FFrom = g_publisher_name;
	callback_t _handler(msg_next_time_handler, NULL);
	CCustomer::sMGetInstance().MIWantReceivingMSG(g_receive_what, _handler);
}


void wait_for_process_finished()
{
	NSHARE::CText const _name=NSHARE::process_name(NSHARE::CThread::sMPid());
	id_of_all_process_t _list;
	NSHARE::pid_list(&_list);

	_list.erase(
			std::remove_if(_list.begin(), _list.end(),
					[&](CThread::process_id_t aVal)
					{
						return NSHARE::process_name(aVal)!=_name;
					}), _list.end());

	do
	{
		_list.erase(
				std::remove_if(_list.begin(), _list.end(),
						[&](CThread::process_id_t aVal)
						{
							return !NSHARE::is_process_exist(aVal);
						}), _list.end());

	} while (_list.size() != 1 && NSHARE::sleep(1));


}
void start_rtc_control(int argc, char const*argv[],char const * aName)
{
	initialize(argc, aName, argv);

	test();

	{
		LOCK_STREAM
		std::cout<<"Wait for child process finished"<<std::endl;
	}
	wait_for_process_finished();
	{
		LOCK_STREAM
		std::cout<<"All child process finished"<<std::endl;
	}

	CCustomer::sMGetInstance().MClose();
	{
		LOCK_STREAM
		std::cout<<"All test finished for:"<<g_name<<std::endl;
		std::cout<<"Press any key... "<<std::endl;
	}
	getchar();
}

void test()
{
	CHECK_EQ(CCustomer::sMGetInstance().MGetMyWishForMSG().size(),0);

	subscribe_to_msg();
	CCustomer::sMGetInstance().MWaitForEvent(CCustomer::EVENT_UPDATE_RTC_INFO);
	{
		LOCK_STREAM
		std::cout<<"RTC info updated ..."<<std::endl;
	}

	g_rtc=CCustomer::sMGetInstance().MGetRTC(g_rtc_name);
	CHECK_NOTNULL(g_rtc);

	start_publishers();

	doing_tests();
	stop_publishers();
	CCustomer::sMGetInstance().MDoNotReceiveMSG(
			g_receive_what);
}
static void doing_tests()
{
	{
		LOCK_STREAM
		std::cout<< std::endl<< std::endl << "Test 1 ..." << std::endl;
	}
	if (!test_integer_timers())
		exit(EXIT_FAILURE);
	else
	{
		LOCK_STREAM
		std::cout << "Test 1 finished successfully" << std::endl;
	}
	if (!test_double_timers())
		exit(EXIT_FAILURE);
	else
	{
		LOCK_STREAM
		std::cout << "Test 2 finished successfully" << std::endl;
	}

	{
		LOCK_STREAM
		std::cout << "-------end of testing--------"<< std::endl;
		std::cout << "Press any key ..."<< std::endl;
		getchar();
	}
}
static bool test_integer_timers()
{

	double const _time_wait_for=5;
	send_cmd(msg_control_t::eSTART_TIMER_TEST);

	bool _is=true;
	_is=_is&&g_rtc->MJoinToRTC();

	if(_is)
		send_cmd(msg_control_t::eJOIN);

	NSHARE::sleep(1);
	send_cmd(msg_control_t::eGO_TIMER_TEST_1);

	IRtc::millisecond_t const _precision= g_rtc->MGetPrecisionMs();

	decltype(g_next_times) _next_timers;

	auto _less_eq_then = [&](auto aVal,auto aMin)
	{	return aVal <= (aMin + _precision);};

	for (unsigned i=0;i<g_number_of_time;++i)
	{

		{
			CRAII<CMutex> _block(g_mutex);
			bool const _is = wait_for([& ]()
			{
				return g_child_pid.size() != g_next_times.size();
			}
					, g_mutex,_time_wait_for);

			LOG(INFO) << "Timeout:" << _is;
			if (_is)
			{
				_next_timers.clear();

				DCHECK_EQ(g_child_pid.size(), g_next_times.size());

				decltype(g_next_times)::value_type const _min = g_next_times.front();

				for (auto const _val : g_next_times)
					if (_less_eq_then(_val, _min))
						_next_timers.push_back(_val);
					else
						break;

				for (auto _it = g_next_times.begin();		//
				_it != g_next_times.end()		//
				&& _less_eq_then(*_it, _min)		//
				; _it = g_next_times.erase(_it)		//
						)
					;
				LOG(INFO) << "Next time size:" << g_next_times.size();
			}
			else
			{
				LOCK_STREAM
				std::cerr<< "No data" << std::endl;
				return false;
			}
		}
		IRtc::millisecond_t const _current_time = g_rtc->MNextTime();

		for (auto const _event : _next_timers)
		{
			if (!_less_eq_then(_event,_current_time))
			{
				LOCK_STREAM
				std::cerr<< "time is not valid  "<<_current_time<<" event time:"<<_event << std::endl;
				return false;
			}
		}
	}
	{
		{
			LOCK_STREAM
			std::cout<< "No error for unsigned test " << std::endl;
		}
		send_cmd(msg_control_t::eUNJOIN);
		send_cmd(msg_control_t::eSTOP_TIMER_TEST);
		NSHARE::sleep(1);

		if(g_rtc->MGetAmountOfJoined()!=1)
		{
			LOCK_STREAM
			std::cerr<< "Amount of joined "<< g_rtc->MGetAmountOfJoined()<< std::endl;

			return false;
		}
	}
	{
		bool _is = g_rtc->MLeaveFromRTC();
		if (_is)
		{
			if(g_rtc->MGetCurrentTimeMs()!=0)
			{
				LOCK_STREAM
				std::cerr<< "Invalid current time is "<< g_rtc->MGetCurrentTimeMs()<< std::endl;
				return false;
			}
			g_next_times.clear();
		}
		return _is;
	}
}
static bool test_double_timers()
{

	double const _time_wait_for=5;
	send_cmd(msg_control_t::eSTART_TIMER_TEST);

	bool _is=true;
	_is=_is&&g_rtc->MJoinToRTC();

	if(_is)
		send_cmd(msg_control_t::eJOIN);

	NSHARE::sleep(1);
	send_cmd(msg_control_t::eGO_TIMER_TEST_2);

	IRtc::millisecond_t const _precision= g_rtc->MGetPrecisionMs();

	decltype(g_next_times) _next_timers;

	auto _less_eq_then = [&](auto aVal,auto aMin)
	{	return aVal <= (aMin + _precision);};

	for (unsigned i=0;i<g_number_of_time;++i)
	{

		{
			CRAII<CMutex> _block(g_mutex);
			bool const _is = wait_for([& ]()
			{
				return g_child_pid.size() != g_next_times.size();
			}
					, g_mutex,_time_wait_for);

			LOG(INFO) << "Timeout:" << _is;
			if (_is)
			{
				_next_timers.clear();

				DCHECK_EQ(g_child_pid.size(), g_next_times.size());

				decltype(g_next_times)::value_type const _min = g_next_times.front();

				for (auto const _val : g_next_times)
					if (_less_eq_then(_val, _min))
						_next_timers.push_back(_val);
					else
						break;

				for (auto _it = g_next_times.begin();		//
				_it != g_next_times.end()		//
				&& _less_eq_then(*_it, _min)		//
				; _it = g_next_times.erase(_it)		//
						)
					;
				LOG(INFO) << "Next time size:" << g_next_times.size();
			}
			else
			{
				LOCK_STREAM
				std::cerr<< "No data" << std::endl;
				return false;
			}
		}
		IRtc::millisecond_t const _current_time = g_rtc->MNextTime();

		for (auto const _event : _next_timers)
		{
			if (!_less_eq_then(_event,_current_time))
			{
				LOCK_STREAM
				std::cerr<< "time is not valid  "<<_current_time<<" event time:"<<_event << std::endl;
				return false;
			}
		}
	}
	{
		{
			LOCK_STREAM
			std::cout<< "No error for unsigned test " << std::endl;
		}
		send_cmd(msg_control_t::eUNJOIN);
		send_cmd(msg_control_t::eSTOP_TIMER_TEST);
		NSHARE::sleep(1);

		if(g_rtc->MGetAmountOfJoined()!=1)
		{
			LOCK_STREAM
			std::cerr<< "Amount of joined "<< g_rtc->MGetAmountOfJoined()<< std::endl;

			return false;
		}
	}
	{
		bool _is = g_rtc->MLeaveFromRTC();
		if (_is)
		{
			LOCK_STREAM
			std::cout<< "Current time is "<< g_rtc->MGetCurrentTimeMs()<< std::endl;

		}
		return _is;
	}
}

int msg_next_time_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	received_message_args_t const* _recv_arg=(received_message_args_t const*)aWHAT;
	const msg_next_time_t* _control=(const msg_next_time_t*)_recv_arg->FMessage.FBegin;

	{
		CRAII<CMutex> _block(g_mutex);
		auto _to= std::lower_bound(g_next_times.begin(),
				g_next_times.end(), _control->FNextTime);

		g_next_times.insert(_to, _control->FNextTime);
		LOG(INFO)<<"Put to next time:"<<g_next_times.size();

		g_convar.MBroadcast();
	}

	return 0;
}
}
