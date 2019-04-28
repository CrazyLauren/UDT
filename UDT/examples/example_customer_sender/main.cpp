#include <customer.h>
#include "api_example_sender.h"

using namespace NUDT;

namespace example_customer_sender
{
int initialize_library(int argc, char const*argv[])
{
	/*! Algorithm: \n
	 * 1) Initialize UDT library
	 */
	const int _val = CCustomer::sMInit(argc, argv, INDITIFICATION_NAME,
			NSHARE::version_t(1, 0));
	if (_val != 0)
	{
		std::cerr << "Cannot initialize library as " << _val << std::endl;
		return EXIT_FAILURE;
	}


	/*! 2) Subscribe to the event #NUDT::CCustomer::EVENT_CONNECTED to when the UDT library
	 *  will be connected to the kernel, the function
	 *  #event_connect_handler is called.
	 */
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_CONNECTED, event_connect_handler);
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_DISCONNECTED, event_disconnect_handler);

	/*! 3) Subscribe to the event #NUDT::CCustomer::EVENT_RECEIVER_SUBSCRIBE to
	 * when some program will started to receive data from me. The function
	 * #event_new_receiver is called.
	 */
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_RECEIVER_SUBSCRIBE, event_new_receiver);

	/*! 4) Subscribe to the event #NUDT::CCustomer::EVENT_RECEIVER_UNSUBSCRIBE to
	 * when some program will finished to receive data from me. The function
	 * #event_remove_receiver is called.
	 */
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_RECEIVER_UNSUBSCRIBE, event_remove_receiver);

	/*! 6) Subscribe to the event #NUDT::CCustomer::EVENT_FAILED_SEND to
	 *  when the sent packet is not delivered by UDT (usually It's happened when The UDT system
	 *  is overloaded or The receiver has been disconnected). The function
	 *  #event_fail_sent_handler is called.*/
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_FAILED_SEND, event_fail_sent_handler);

	/*! 7) Subscribe to the event #NUDT::CCustomer::EVENT_CUSTOMERS_UPDATED to
	 * when the program list has been updated. The function
	 * #event_customers_update_handler is called.*/
	CCustomer::sMGetInstance() += event_handler_info_t(
			CCustomer::EVENT_CUSTOMERS_UPDATED, event_customers_update_handler);

	/*! 8) Starting the 'main loop' of the UDT library */
	CCustomer::sMGetInstance().MOpen();

	/*! 9) Wait for me to connect to the kernel*/
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
int main(int argc, char const *argv[])
{
	return example_customer_sender::main(argc, argv);
}
