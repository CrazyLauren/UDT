// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * main.cpp
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
#include "api_example_customer.h"

using namespace NUDT;
namespace example_easy_program
{
int initialize_library(int argc, char const*argv[])
{
	/*! Algorithm: \n
	 * 1) Initialize UDT library
	 */
	const int _val = CCustomer::sMInit(argc, argv, INDITIFICATION_NAME,NSHARE::version_t(0,1));
	if (_val != 0)
	{
		std::cerr << "Cannot initialize library as " << _val << std::endl;
		return EXIT_FAILURE;
	}

	/*! 2) Say to the kernel that I want to receive the message
	 * number #MESSAGE_NUMBER of version 1.1
	 * from #INDITIFICATION_NAME and it will be  handled  by function #msg_test_handler
	 */
	CCustomer::sMGetInstance().MIWantReceivingMSG(
	INDITIFICATION_NAME, MESSAGE_NUMBER, msg_test_handler,
			NSHARE::version_t(1, 1));

	/*! 3) Subscribe to the event #NUDT::CCustomer::EVENT_CONNECTED to when the UDT library
	 *  will be connected to the kernel, the function
	 *  #event_connect_handler is called.
	 */
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_CONNECTED, event_connect_handler);
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_DISCONNECTED, event_disconnect_handler);

	/*! 4) Subscribe to the event #NUDT::CCustomer::EVENT_RECEIVER_SUBSCRIBE to
	 * when some program will start receiving data from me. The function
	 * #event_new_receiver is called.
	 */
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_RECEIVER_SUBSCRIBE, event_new_receiver);

	/*! 5) Subscribe to the event #NUDT::CCustomer::EVENT_FAILED_SEND to
	 *  when the sent packet is not delivered by UDT (usually It's happened when The UDT system
	 *  is overloaded or The receiver has been disconnected). The function
	 *  #event_fail_sent_handler is called.*/
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_FAILED_SEND, event_fail_sent_handler);

	/*! 6) Subscribe to the event #NUDT::CCustomer::EVENT_CUSTOMERS_UPDATED to
	 * when the program list has been updated. The function
	 * #event_customers_update_handler is called.*/
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_CUSTOMERS_UPDATED, event_customers_update_handler);

	/*! 7) Starting the 'main loop' of the UDT library */
	CCustomer::sMGetInstance().MOpen();

	/*! 8) Wait for me to connect to the kernel*/
	CCustomer::sMGetInstance().MWaitForEvent(CCustomer::EVENT_CONNECTED);

	return 0;
}

int main(int argc, char const*argv[])
{
	///1) Initialization library by initialize_library()
	int const _rval = initialize_library(argc, argv);
	if (_rval != 0)
		return _rval;

	///2) sending something by send_messages()
	send_messages();

	///3) wait for finished by NUDT::CCustomer::MJoin()
	CCustomer::sMGetInstance().MJoin();
	return EXIT_SUCCESS;
}
}
int main(int argc, char const*argv[])
{
	return example_easy_program::main(argc,argv);
}
