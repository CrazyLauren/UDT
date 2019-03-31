/*
 * subscriber.cpp
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
#include <tasks.h>
#include "protocol.h"

using namespace NSHARE;
using namespace NUDT;

static NSHARE::CMutex g_mutex(NSHARE::CMutex::MUTEX_NORMAL);
static NSHARE::CCondvar g_convar;

extern NSHARE::CIPCSem g_mutex_stream;
extern std::string g_name;
#define LOCK_STREAM CRAII<CIPCSem> _block(g_mutex_stream); std::cout<<std::endl <<"Subscriber "<<g_name<<" say:"<<std::endl;

extern int g_argc;
extern char const** g_argv;
static bool g_is_working=false;
static unsigned g_amount_of_publisher=0;

extern std::vector<std::pair<ptrdiff_t,std::string> >g_child_pid;

static requirement_msg_info_t g_receive_what;
static std::set<NSHARE::uuid_t> g_current_publishers;

static int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA);
static int event_remove_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA);
static int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA);
static int event_customers_update_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA);
static bool check_for_valid_of_sender(const std::string& aSender);
static void testing();
static void start_publishers();
static void stop_publishers();
extern  int start_child(char  const* aName);

template<typename _Function>
static bool wait_for(_Function aF,double aTime=-1)
{
	CRAII<CMutex> _block(g_mutex);
	for (double const _cur_time=NSHARE::get_time(); aF();)
	{
		if(aTime<0)
			g_convar.MTimedwait(&g_mutex);
		else
			if (!g_convar.MTimedwait(&g_mutex,
				NSHARE::get_time() - _cur_time + aTime))
		{
			if((NSHARE::get_time() - _cur_time)>aTime)
				return false;
		}
	}
	return true;
}
static int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
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
}
static void send_cmd(msg_control_t::eCMD aCmd,NSHARE::uuid_t const & aTo =NSHARE::uuid_t())
{
	required_header_t _header;
	msg_head_t *_msg = (msg_head_t*)_header.FReserved;
	_msg->FType = E_MSG_CONTROL;

	NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			sizeof(msg_control_t));	//!< allocate the buffer for message without header

	if(_buf.empty())
	{
		LOCK_STREAM
		std::cerr << "Cannot open allocate memory"<<std::endl;
		exit(EXIT_FAILURE);
	}

	msg_control_t *const _data= (msg_control_t *)_buf.ptr();
	_data->FCommand=aCmd;

	int _num = aTo.MIsValid()?//
			CCustomer::sMGetInstance().MSend(_header,PROTOCOL_NAME, _buf,aTo)//Отправляем конкретному адресату
			:CCustomer::sMGetInstance().MSend(_header,PROTOCOL_NAME, _buf);//Отправляем всем
	if (_num > 0)	//Hurrah!!! The data has been sent
	{
		LOCK_STREAM
		std::cout <<"Send command "<<aCmd<< " packet#" << _num <<std::endl;
	}
	else //The buffer _buf is not freed as it's not sent.
	{
		LOCK_STREAM
		std::cout << "Send error  " << _num << std::endl;
	}
}
static bool test_stage_1();
static bool test_stage_2();
static bool test_stage_3();
static bool test_stage_4();
static void initialize(int argc, const char* aName, char const* argv[])
{
	const int _val = CCustomer::sMInit(argc, argv, aName,
			NSHARE::version_t(1, 0), "./example_customer.xml"); //!< initialize UDT library
	if (_val != 0)
	{
		LOCK_STREAM
		std::cerr << "Cannot initialize library as " << _val << std::endl;
		exit(EXIT_FAILURE);
	}
	{
		//!< When the UDT library will be connected to UDT kernel. The function
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
		//!< When some consumers will start receiving data from me. The function
		//event_new_receiver is called.
		callback_t _handler_event_connect(event_new_receiver, NULL);
		event_handler_info_t _event_connect(CCustomer::EVENT_RECEIVER_SUBSCRIBE,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}
	{
		//!< When some consumers will start receiving data from me. The function
		//event_remove_receiver is called.
		callback_t _handler_event_disconnect(event_remove_receiver, NULL);
		event_handler_info_t _event_disconnect(CCustomer::EVENT_RECEIVER_UNSUBSCRIBE,
				_handler_event_disconnect);
		CCustomer::sMGetInstance() += _event_disconnect;
	}
	{
		//!< When the customer's list has been updated. The function
		//event_customers_update_handler is called.

		callback_t _handler_cus_update(event_customers_update_handler, NULL);
		event_handler_info_t _event_cust(CCustomer::EVENT_CUSTOMERS_UPDATED,
				_handler_cus_update);
		CCustomer::sMGetInstance() += _event_cust;
	}

	//Starting the 'main loop' of UDT library
	CCustomer::sMGetInstance().MOpen();
	{
		LOCK_STREAM
		std::cout<<"Wait for connect ..."<<std::endl;
	}
}

void direct_test()
{
	CHECK_EQ(g_amount_of_publisher,0);
	CHECK_EQ(CCustomer::sMGetInstance().MGetMyWishForMSG().size(),0);

	((msg_head_t*) (g_receive_what.FRequired.FReserved))->FType = E_TEST_MSG;
	g_receive_what.FProtocolName = PROTOCOL_NAME;
	g_receive_what.FFlags = requirement_msg_info_t::E_NEAREST;
	g_receive_what.FFrom=g_child_pid.front().second;

	callback_t _handler(msg_test_handler, NULL);
	CCustomer::sMGetInstance().MIWantReceivingMSG(g_receive_what, _handler);
	start_publishers();
	testing();
	stop_publishers();
	CCustomer::sMGetInstance().MDoNotReceiveMSG(
			g_receive_what);
}

void reverse_test()
{
	CHECK_EQ(g_amount_of_publisher,0);
	CHECK_EQ(CCustomer::sMGetInstance().MGetMyWishForMSG().size(),0);

	std::reverse(g_child_pid.begin(), g_child_pid.end());

	callback_t _handler(msg_test_handler, NULL);
	((msg_head_t*) (g_receive_what.FRequired.FReserved))->FType = E_TEST_MSG;
	g_receive_what.FProtocolName = PROTOCOL_NAME;
	g_receive_what.FFlags = requirement_msg_info_t::E_NEAREST
			| requirement_msg_info_t::E_INVERT_GROUP;
	g_receive_what.FFrom=g_child_pid.front().second;



	CCustomer::sMGetInstance().MIWantReceivingMSG(
			g_receive_what, _handler);
	start_publishers();
	testing();
	stop_publishers();
	CCustomer::sMGetInstance().MDoNotReceiveMSG(g_receive_what);
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
extern void start_subscriber(int argc, char const*argv[],char const * aName)
{
	initialize(argc, aName, argv);

	direct_test();

	{
		LOCK_STREAM
		std::cout<<"Wait for child process finished"<<std::endl;
	}
	wait_for_process_finished();
	{
		LOCK_STREAM
		std::cout<<"All child process finished"<<std::endl;
	}
	reverse_test();

	CCustomer::sMGetInstance().MClose();
	{
		LOCK_STREAM
		std::cout<<"All test finished for:"<<g_name<<std::endl;
		std::cout<<"Press any key... "<<std::endl;
	}
	getchar();
}

static int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	subcribe_receiver_args_t* _recv_arg=(subcribe_receiver_args_t*)aWHAT;

	{
	LOCK_STREAM

	std::cout <<"subscribed: "<<std::endl;

	for(auto const& _it:_recv_arg->FReceivers)
	{
		{
			CRAII<CMutex> _block(g_mutex);
			++g_amount_of_publisher;
			g_convar.MBroadcast();
		}
		std::cout <<"-*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*"<< std::endl;
		std::cout << "Now " << _it.FWho << " receive " << _it.FWhat.FRequired
				<< " by " << _it.FWhat.FProtocolName << " As I am in "
				<< _it.FWhat.FFrom << std::endl;
		std::cout <<"-------------------------------------"<< std::endl;
	}
	}
	return 0;
}
static int event_remove_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	subcribe_receiver_args_t* _recv_arg=(subcribe_receiver_args_t*)aWHAT;
	{
		CRAII<CMutex> _block(g_mutex);
		--g_amount_of_publisher;
		g_convar.MBroadcast();
	}
	{
	LOCK_STREAM

	std::cout <<"unsubscribed: "<<std::endl;

	for(auto& _it:_recv_arg->FReceivers)
	{
		std::cout <<"*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*"<< std::endl;
		std::cout << "Now " << _it.FWho << " doesn't receive " << _it.FWhat.FRequired
				<< " by " << _it.FWhat.FProtocolName << " As I am in "
				<< _it.FWhat.FFrom << std::endl;
		std::cout <<"-------------------------------------"<< std::endl;

	}
	}
	return 0;
}
static int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	received_message_args_t const* _recv_arg=(received_message_args_t const*)aWHAT;
	const msg_test_t* _control=(const msg_test_t*)_recv_arg->FMessage.FBegin;

	LOCK_STREAM
	std::cout << "Receiving TEST MSG packet #"<<_recv_arg->FPacketNumber<<" bytes from "
			<<_recv_arg->FFrom<<" by "<<_recv_arg->FProtocolName<< std::endl;

	{
		CRAII<CMutex> _block(g_mutex);
		g_current_publishers.insert(_recv_arg->FFrom);
		g_convar.MBroadcast();
	}

	return 0;
}

static bool test_stage_1()
{
	{
		CRAII<CMutex> _block(g_mutex);
		g_current_publishers.clear();
	}
	send_cmd(msg_control_t::eSEND);
	{
		{
			CRAII<CMutex> _block(g_mutex);
			for (; g_current_publishers.empty();)
				g_convar.MTimedwait(&g_mutex);
		}
		NSHARE::sleep(1);
		{
			CRAII<CMutex> _block(g_mutex);
			for (auto _it : g_current_publishers)
			{
				const auto _id = CCustomer::sMGetInstance().MCustomer(_it).FId;
				if (_id.FName == g_child_pid.front().second)
				{
					LOCK_STREAM
					std::cout<< "Checked " << _id.FName << " == "
					<< g_child_pid.front().second << std::endl;
				}
				else
				{
					LOCK_STREAM
					std::cout<< "Checking failed " << _id.FName
					<< " != " << g_child_pid.front().second << std::endl;
					return false;
				}
			}
		}
	}
	return true;
}

static bool test_stage_2()
{
	if (g_child_pid.size() < 2)
	{
		LOCK_STREAM
		std::cout<< "amount of publisher less than 2" << std::endl;
		return false;
	}

	{
		CRAII<CMutex> _block(g_mutex);

		//Закрываем программы
		for (auto _it : g_current_publishers)
			send_cmd(msg_control_t::eFINISH,_it);
	}
	//ждём пока удалятся все отправители (event_customers_update_handler)
	if(!wait_for([]()
					{
						return !g_current_publishers.empty();
					}
					,5.0) )
	{
		LOCK_STREAM
		std::cerr<< "time out" << std::endl;
		return false;
	}

	send_cmd(msg_control_t::eSEND);


	if(!wait_for([]()
					{
						return g_current_publishers.empty();
					}
					,5.0) )
	{
		LOCK_STREAM
		std::cerr<< "time out" << std::endl;
		return false;
	}
	NSHARE::sleep(1);
	return check_for_valid_of_sender(g_child_pid[1].second);
}

static bool test_stage_3()
{
	if (g_child_pid.size() < 3)
	{
		LOCK_STREAM
		std::cout<< "amount of publisher less than 3" << std::endl;
		return false;
	}
	{//закрываем дальний
		auto const _list=CCustomer::sMGetInstance().MCustomers();

		for(auto const& _it:_list)
			if(g_child_pid.back().second==_it.FId.FName)
			{
				send_cmd(msg_control_t::eFINISH,_it.FId.FUuid);
			}
	}

	NSHARE::sleep(1);

	{
		CRAII<CMutex> _block(g_mutex);
		g_current_publishers.clear();
	}
	send_cmd(msg_control_t::eSEND);

	if(!wait_for([]()
					{
						return g_current_publishers.empty();
					}
					,5.0) )
	{
		LOCK_STREAM
		std::cerr<< "time out" << std::endl;
		return false;
	}
	NSHARE::sleep(1);
	return check_for_valid_of_sender(g_child_pid[1].second);
}
static bool test_stage_4()
{
	start_child(g_child_pid.front().second.c_str());

	const unsigned _old=g_amount_of_publisher;
	if(!wait_for([&]()
					{
						return _old==g_amount_of_publisher;
					}
					,5.0) )
	{
		LOCK_STREAM
		std::cerr<< "time out" << std::endl;
		return false;
	}

	{
		CRAII<CMutex> _block(g_mutex);
		g_current_publishers.clear();
	}
	send_cmd(msg_control_t::eSEND);
	if(!wait_for([]()
					{
						return g_current_publishers.empty();
					}
					,5.0) )
	{
		LOCK_STREAM
		std::cerr<< "time out" << std::endl;
		return false;
	}
	NSHARE::sleep(1);
	return check_for_valid_of_sender(g_child_pid[0].second);
}
static int event_customers_update_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	customers_updated_args_t const* _recv_arg=(customers_updated_args_t*)aWHAT;

	if(!_recv_arg->FDisconnected.empty())
	{
		CRAII<CMutex> _block(g_mutex);
		for (auto const& _it:_recv_arg->FDisconnected)
		{
			g_current_publishers.erase(_it.FId.FUuid);
		}
	}

	return 0;
}

static bool check_for_valid_of_sender(const std::string& aSender)
{
	CRAII<CMutex> _block(g_mutex);
	for (auto _it : g_current_publishers)
	{
		const auto _id = CCustomer::sMGetInstance().MCustomer(_it).FId;
		if (_id.FName == aSender)
		{
			LOCK_STREAM
			std::cout << "Checked " << _id.FName << " == "
			<< aSender << std::endl;
		}
		else
		{
			LOCK_STREAM
			std::cerr << "Checking failed " << _id.FName
			<< " != " << aSender << std::endl;
			return false;
		}
	}
	return true;
}
static void testing()
{
	{
		LOCK_STREAM
		std::cout<< std::endl<< std::endl << "Test 1 ..." << std::endl;
	}
	if (!test_stage_1())
		exit(EXIT_FAILURE);
	else
	{
		LOCK_STREAM
		std::cout << "Test 1 finished successfully" << std::endl;
	}
	{
		LOCK_STREAM
		std::cout << std::endl<< std::endl<< "Test 2 ..." << std::endl;
	}
	if (!test_stage_2())
		exit(EXIT_FAILURE);
	else
	{
		LOCK_STREAM
		std::cout << "Test 2 finished successfully" << std::endl;
	}
	{
		LOCK_STREAM
		std::cout << std::endl<< std::endl<< "Test 3 ..." << std::endl;
	}
	if (!test_stage_3())
		exit(EXIT_FAILURE);
	else
	{
		LOCK_STREAM
		std::cout << "Test 3 finished successfully" << std::endl;
	}
	{
		LOCK_STREAM
		std::cout << std::endl<< std::endl<< "Test 4 ..." << std::endl;
	}
	if (!test_stage_4())
		exit(EXIT_FAILURE);
	else
	{
		LOCK_STREAM
		std::cout << "Test 4 finished successfully" << std::endl;
	}
}
void start_publishers()
{
	for (auto& _v : g_child_pid)
		_v.first = start_child(_v.second.c_str());
	{
		LOCK_STREAM
		std::cout << g_name << std::endl;
		std::cout << "Child " << std::endl;
		std::cout << "  PID  " << "\t Name" << std::endl;
		for (auto& _v : g_child_pid)
			std::cout << _v.first << "\t" << _v.second << std::endl;
	}
	{
		CRAII<CMutex> _block(g_mutex);
		for (; g_amount_of_publisher != g_child_pid.size();)
			g_convar.MTimedwait(&g_mutex);
	}
}
void stop_publishers()
{
	send_cmd(msg_control_t::eFINISH);
	{
		CRAII<CMutex> _block(g_mutex);
		for (; g_amount_of_publisher;)
			g_convar.MTimedwait(&g_mutex);
	}
}

