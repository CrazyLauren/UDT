<<<<<<< HEAD
<<<<<<< HEAD:UDT/examples/example_customer_user_protocol/main.cpp
#include <customer.h>

#include <udt_example_protocol.h>

using namespace NUDT;
#define INDITIFICATION_NAME "uex@guex" ///<Indintification program name. Recommended using a short name
#define RECEIVE_MSG_TEST_FROM INDITIFICATION_NAME

extern int msg_test_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int sniffer_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_new_receiver(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_connect_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_customers_update_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);

extern void doing_something();
int main(int argc, char *argv[])
{
	const int _val=CCustomer::sMInit(argc, argv, INDITIFICATION_NAME,NSHARE::version_t(1,0), CONFIG_PATH);///< initialize UDT library

	if(_val!=0)
	{
		std::cerr<<"Cannot initialize library as "<<_val<<std::endl;
		return EXIT_FAILURE;
	}

	{	///< I want to receive msg E_MSG_TEST of protocol PROTOCOL_NAME
		// from RECEIVE_MSG_TEST_FROM and it will be  handled  by function msg_test_handler
		requirement_msg_info_t _msg;
		_msg.FRequired.FNumber = E_MSG_TEST;
		_msg.FProtocolName = PROTOCOL_NAME;

		callback_t _handler(msg_test_handler, NULL);

		CCustomer::sMGetInstance().MIWantReceivingMSG(
				RECEIVE_MSG_TEST_FROM, _msg, _handler);
	}
	{
		///< I want to sniff the msg number 0 (sent by example_customer)
		// between any customer of "guex" group
		//and it will be  handled  by function sniffer_handler
		callback_t _handler(sniffer_handler, NULL);
		CCustomer::sMGetInstance().MIWantReceivingMSG(
			"@guex", 0, _handler,requirement_msg_info_t::E_REGISTRATOR);
	}
	{
		///< When the UDT library will be connected to UDT kernel. The function
		//event_connect_handler is called.
		callback_t _handler_event_connect(event_connect_handler, NULL);
		CCustomer::value_t _event_connect(CCustomer::EVENT_CONNECTED,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}
	{
		///< When some consumers will start receiving data from me. The function
		//event_new_receiver is called.
		callback_t _handler_event_connect(event_new_receiver, NULL);
		CCustomer::value_t _event_connect(CCustomer::EVENT_NEW_RECEIVER,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}

	{
		///< When the sent packet is not delivered by UDT (usually It's happened when The UDT system
		// is overloaded or The receiver has been disconnected). The function
		//event_fail_sent_handler is called.
		callback_t _handler_fail_sent(event_fail_sent_handler, NULL);
		CCustomer::value_t _event_fail(CCustomer::EVENT_FAILED_SEND,
				_handler_fail_sent);
		CCustomer::sMGetInstance() += _event_fail;
	}
	{
		///< When the customer's list has been updated. The function
		//event_fail_sent_handler is called.

		callback_t _handler_cus_update(event_customers_update_handler, NULL);
		CCustomer::value_t _event_cust(CCustomer::EVENT_CUSTOMERS_UPDATED,
				_handler_cus_update);
		CCustomer::sMGetInstance() += _event_cust;
	}

	//Starting the 'main loop' of UDT library
	CCustomer::sMGetInstance().MOpen();

	//doing something
	doing_something();

	return EXIT_SUCCESS;
}
=======
=======
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
>>>>>>> caa8e6a... see changelog
#include <customer.h>

#include <udt_example_protocol.h>

using namespace NUDT;
#define INDITIFICATION_NAME "uex@guex" ///<Indintification program name. Recommended using a short name
#define RECEIVE_MSG_TEST_FROM INDITIFICATION_NAME

extern int msg_test_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int sniffer_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_new_receiver(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_connect_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_customers_update_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);

extern void doing_something();
int main(int argc, char const*argv[])
{
	const int _val=CCustomer::sMInit(argc, argv, INDITIFICATION_NAME,NSHARE::version_t(1,0), CONFIG_PATH);///< initialize UDT library

	if(_val!=0)
	{
		std::cerr<<"Cannot initialize library as "<<_val<<std::endl;
		return EXIT_FAILURE;
	}

	{	///< I want to receive msg E_MSG_TEST of protocol PROTOCOL_NAME
		// from RECEIVE_MSG_TEST_FROM and it will be  handled  by function msg_test_handler
		requirement_msg_info_t _msg;
		_msg.FRequired.FNumber = E_MSG_TEST;
		_msg.FProtocolName = PROTOCOL_NAME;

		callback_t _handler(msg_test_handler, NULL);

		CCustomer::sMGetInstance().MIWantReceivingMSG(
				RECEIVE_MSG_TEST_FROM, _msg, _handler);
	}
	{
		///< I want to sniff the msg number 0 (sent by example_customer)
		// between any customer of "guex" group
		//and it will be  handled  by function sniffer_handler
		callback_t _handler(sniffer_handler, NULL);
		CCustomer::sMGetInstance().MIWantReceivingMSG(
			"@guex", 0, _handler,requirement_msg_info_t::E_REGISTRATOR);
	}
	{
		///< When the UDT library will be connected to UDT kernel. The function
		//event_connect_handler is called.
		callback_t _handler_event_connect(event_connect_handler, NULL);
		CCustomer::value_t _event_connect(CCustomer::EVENT_CONNECTED,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}
	{
		///< When some consumers will start receiving data from me. The function
		//event_new_receiver is called.
		callback_t _handler_event_connect(event_new_receiver, NULL);
		CCustomer::value_t _event_connect(CCustomer::EVENT_RECEIVER_SUBSCRIBE,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}

	{
		///< When the sent packet is not delivered by UDT (usually It's happened when The UDT system
		// is overloaded or The receiver has been disconnected). The function
		//event_fail_sent_handler is called.
		callback_t _handler_fail_sent(event_fail_sent_handler, NULL);
		CCustomer::value_t _event_fail(CCustomer::EVENT_FAILED_SEND,
				_handler_fail_sent);
		CCustomer::sMGetInstance() += _event_fail;
	}
	{
		///< When the customer's list has been updated. The function
		//event_fail_sent_handler is called.

		callback_t _handler_cus_update(event_customers_update_handler, NULL);
		CCustomer::value_t _event_cust(CCustomer::EVENT_CUSTOMERS_UPDATED,
				_handler_cus_update);
		CCustomer::sMGetInstance() += _event_cust;
	}

	//Starting the 'main loop' of UDT library
	CCustomer::sMGetInstance().MOpen();

	//doing something
	doing_something();

	return EXIT_SUCCESS;
}
>>>>>>> 8543561... see ChangeLog.txt:UDT/examples_old/example_customer_user_protocol/main.cpp
