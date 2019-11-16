// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <customer.h>
#include <udt_example_protocol.h>

#include "no_route_test_api.h"
using namespace NUDT;

namespace test_errors
{
size_t g_buf_size=100000;
int main(int argc, char const*argv[])
{
	g_buf_size= (argc > 1) ? atoi(argv[1]) : g_buf_size;
	///1) Initialization library by initialize_library()

	int const _rval = initialize_library(argc, argv);
	if (_rval != 0)
		return _rval;

	///2) sending something by doing_tests()
	bool const _is=doing_tests();

	CCustomer::sMGetInstance().MClose();
		
	return _is?EXIT_SUCCESS:EXIT_FAILURE;
}
int initialize_library(int argc, char const*argv[])
{
	using namespace example_of_user_protocol;
	g_buf_size= (argc > 1) ? atoi(argv[1]) : g_buf_size;

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

	/*! 2) Say to the kernel that I want to receive the message
	 * number #MESSAGE_NUMBER
	 * from #INDITIFICATION_NAME and it will be  handled  by function #msg_speed_handler
	 * and
	 * number #example_of_user_protocol::E_MSG_TEST
	 * from #INDITIFICATION_NAME and it will be  handled
	 * by function #msg_test_handler
	 */
	CCustomer::sMGetInstance().MIWantReceivingMSG(
	INDITIFICATION_NAME, MESSAGE_NUMBER, msg_handler,g_requrement_msg_version);

	CCustomer::sMGetInstance().MIWantReceivingMSG( //
			requirement_msg_info_t(PROTOCOL_NAME,
					required_header_t(msg_head_t(example_of_user_protocol::E_MSG_TEST, MSG_TEST_SIZE)),
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

	/*! 6) Starting the 'main loop' of the UDT library */
	CCustomer::sMGetInstance().MOpen();

	/*! 7) Wait for me to connect to the kernel*/
	CCustomer::sMGetInstance().MWaitForEvent(CCustomer::EVENT_CONNECTED);

	return 0;
}
}
int main(int argc, char const*argv[])
{
	return test_errors::main(argc,argv);
}
