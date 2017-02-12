#include <customer.h>


using namespace NUDT;

#define INDITIFICATION_NAME "uex2@guex"

extern int msg_test_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int group_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_new_receiver(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_connect_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_customers_update_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);

extern void doing_something();
int main(int argc, char *argv[])
{
	const int _val=CCustomer::sMInit(argc, argv, INDITIFICATION_NAME);//!< initialize UDT library

	if(_val!=0)
	{
		std::cerr<<"Cannot initialize library as "<<_val<<std::endl;
		return EXIT_FAILURE;
	}

	{	//!< I want to receive msg number 0
		// from INDITIFICATION_NAME and it will be  handled  by function msg_test_handler
		callback_t _handler(msg_test_handler, NULL);
		CCustomer::sMGetInstance().MIWantReceivingMSG(
				"uex2@guex", 0, _handler);
	}
	{	
		//!< I want to receive msg number 0
		// from any customer of "guex" group  and it will be  handled  by function group_handler
		callback_t _handler(group_handler, NULL);
		CCustomer::sMGetInstance().MIWantReceivingMSG(
			"@guex", 0, _handler);
	}
	{
		//!< When the UDT library will be connected to UDT kernel. The function
		//event_connect_handler is called.
		callback_t _handler_event_connect(event_connect_handler, NULL);
		CCustomer::value_t _event_connect(CCustomer::EVENT_CONNECTED,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}
	{
		//!< When some consumers will start receiving data from me. The function
		//event_new_receiver is called.
		callback_t _handler_event_connect(event_new_receiver, NULL);
		CCustomer::value_t _event_connect(CCustomer::EVENT_NEW_RECEIVER,
				_handler_event_connect);
		CCustomer::sMGetInstance() += _event_connect;
	}

	{
		//!< When the sent packet is not delivered by UDT (usually It's happened when The UDT system
		// is overloaded or The receiver has been disconnected). The function
		//event_fail_sent_handler is called.
		callback_t _handler_fail_sent(event_fail_sent_handler, NULL);
		CCustomer::value_t _event_fail(CCustomer::EVENT_FAILED_SEND,
				_handler_fail_sent);
		CCustomer::sMGetInstance() += _event_fail;
	}
	{
		//!< When the customer's list has been updated. The function
		//event_customers_update_handler is called.

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
