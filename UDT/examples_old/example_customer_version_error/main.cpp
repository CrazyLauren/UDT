<<<<<<< HEAD:UDT/examples/example_customer_version_error/main.cpp
#include <customer.h>


using namespace NUDT;

#define INDITIFICATION_NAME "uex_v_fail@guex"
#define MESSAGE_NUMBER 1

extern int msg_test_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_new_receiver(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);


extern void doing_something();
int main(int argc, char *argv[])
{
	const int _val=CCustomer::sMInit(argc, argv, INDITIFICATION_NAME);///< initialize UDT library

	if(_val!=0)
	{
		std::cerr<<"Cannot initialize library as "<<_val<<std::endl;
		return EXIT_FAILURE;
	}

	{	///< I want to receive msg number MESSAGE_NUMBER version 1.3
		// from INDITIFICATION_NAME and it will be  handled  by function msg_test_handler
		callback_t _handler(msg_test_handler, NULL);
		CCustomer::sMGetInstance().MIWantReceivingMSG(
				"@guex", MESSAGE_NUMBER, _handler,requirement_msg_info_t::E_NO_FLAGS,NSHARE::version_t(1,3));
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

	//Starting the 'main loop' of UDT library
	CCustomer::sMGetInstance().MOpen();

	//doing something
	doing_something();

	return EXIT_SUCCESS;
}
=======
#include <customer.h>


using namespace NUDT;

#define INDITIFICATION_NAME "uex_v_fail@guex"
#define MESSAGE_NUMBER 1

extern int msg_test_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_new_receiver(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);


extern void doing_something();
int main(int argc, char const*argv[])
{
	const int _val=CCustomer::sMInit(argc, argv, INDITIFICATION_NAME);///< initialize UDT library

	if(_val!=0)
	{
		std::cerr<<"Cannot initialize library as "<<_val<<std::endl;
		return EXIT_FAILURE;
	}

	{	///< I want to receive msg number MESSAGE_NUMBER version 1.3
		// from INDITIFICATION_NAME and it will be  handled  by function msg_test_handler
		callback_t _handler(msg_test_handler, NULL);
		CCustomer::sMGetInstance().MIWantReceivingMSG(
				"@guex", MESSAGE_NUMBER, _handler,requirement_msg_info_t::E_NO_FLAGS,NSHARE::version_t(1,3));
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

	//Starting the 'main loop' of UDT library
	CCustomer::sMGetInstance().MOpen();

	//doing something
	doing_something();

	return EXIT_SUCCESS;
}
>>>>>>> 8543561... see ChangeLog.txt:UDT/examples_old/example_customer_version_error/main.cpp
