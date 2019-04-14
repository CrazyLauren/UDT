#include <customer.h>
#include <udt_example_protocol.h>
#include "api_example_customer_for_user_protocol.h"

namespace example_for_user_protocol
{
using namespace NUDT;
int initialize_library(int argc, char const*argv[])
{
	/*! Algorithm: \n
	 * 1) Initialize UDT library
	 */
	const int _val = CCustomer::sMInit(argc, argv, INDITIFICATION_NAME,
			NSHARE::version_t(1, 0), CONFIG_PATH);
	if (_val != 0)
	{
		std::cerr << "Cannot initialize library as " << _val << std::endl;
		return EXIT_FAILURE;
	}

	/*! 2) Say to the kernel that I want to receive the message
	 * number #E_MSG_TEST
	 * from #INDITIFICATION_NAME and it will be  handled
	 * by function #msg_test_handler
	 */
	CCustomer::sMGetInstance().MIWantReceivingMSG( //
			requirement_msg_info_t(PROTOCOL_NAME,
					required_header_t(msg_head_t(E_MSG_TEST, PACKET_SIZE)),
					INDITIFICATION_NAME), //
			msg_test_handler);

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

/*extern int sniffer_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	received_message_args_t const* _recv_arg = (received_message_args_t const*)aWHAT;
	///<Now You can handle the received data.

	STREAM_MUTEX_LOCK
	std::cout << "Message #" << _recv_arg->FPacketNumber<<" by " << _recv_arg->FProtocolName << " size "
			<< _recv_arg->FBuffer.size() << " bytes sniffed from "
			<< _recv_arg->FFrom <<" to ";
	std::vector<NSHARE::uuid_t>::const_iterator _it = _recv_arg->FTo.begin(),
			_it_end = _recv_arg->FTo.end();

	for(;_it!=_it_end;++_it)
	{
		std::cout << *_it<<", ";
	}
	std::cout << std::endl;
	STREAM_MUTEX_UNLOCK
		return 0;
}*/
int main(int argc, char const*argv[])
{

	//todo в отдельный тест
	/*
	 {
	 ///< I want to sniff the msg number 0 (sent by example_customer)
	 // between any customer of "guex" group
	 //and it will be  handled  by function sniffer_handler
	 callback_t _handler(sniffer_handler, NULL);
	 CCustomer::sMGetInstance().MIWantReceivingMSG(
	 "@guex", 0, _handler,requirement_msg_info_t::E_REGISTRATOR);
	 }
	 */
	return example_for_user_protocol::main(argc, argv);
}
