#include <customer.h>


using namespace NUDT;

#define INDITIFICATION_NAME "uex_snif@guex"

extern int sniffer_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);
extern int event_fail_sent_handler(CCustomer* WHO, void* WHAT, void* YOU_DATA);

extern void doing_something();
int main(int argc, char *argv[])
{
	const int _val=CCustomer::sMInit(argc, argv, INDITIFICATION_NAME);//!< initialize UDT library

	if(_val!=0)
	{
		std::cerr<<"Cannot initialize library as "<<_val<<std::endl;
		return EXIT_FAILURE;
	}

	{	
		//!< I want to sniff the msg number 0 (sent by example_customer)
		// between any customer of "guex" group
		//and it will be  handled  by function sniffer_handler
		callback_t _handler(sniffer_handler, NULL);
		CCustomer::sMGetInstance().MIWantReceivingMSG(
			"@guex", 0, _handler,msg_parser_t::E_REGISTRATOR);
	}
	{
		//!< When the sniffed packet is not delivered by UDT (usually It's happened when The UDT system
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
