<<<<<<< HEAD:UDT/examples/example_parser_error/main.cpp
#include <customer.h>

#include <udt_example_protocol.h>

using namespace NUDT;
#define INDITIFICATION_NAME "uex_fail@guex"

#define RECEIVE_MSG_TEST_FROM INDITIFICATION_NAME

extern int msg_test_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_new_receiver(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);

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
	CCustomer::sMGetInstance().MOpen();

	//doing something
	doing_something();

	return EXIT_SUCCESS;
}
=======
#include <customer.h>

#include <udt_example_protocol.h>

using namespace NUDT;
#define INDITIFICATION_NAME "uex_fail@guex"

#define RECEIVE_MSG_TEST_FROM INDITIFICATION_NAME

extern int msg_test_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_new_receiver(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);

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
	CCustomer::sMGetInstance().MOpen();

	//doing something
	doing_something();

	return EXIT_SUCCESS;
}
>>>>>>> 8543561... see ChangeLog.txt:UDT/examples_old/example_parser_error/main.cpp
