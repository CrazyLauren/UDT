// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <customer.h>
#include <protocol_inherited.h>

#include "test_inheritance_api.h"
using namespace NUDT;

namespace test_inheritance
{
size_t g_buf_size=100000;
int main(int argc, char const*argv[])
{
	g_buf_size= (argc > 1) ? atoi(argv[1]) : g_buf_size;
	///1) Initialization library by initialize_library()

	int const _rval = initialize_library(argc, argv);
	if (_rval != 0)
		return _rval;

	///2) sending something by send_messages()
	send_messages();

	CCustomer::sMGetInstance().MClose();
		
	return EXIT_SUCCESS;
}
int initialize_library(int argc, char const*argv[])
{
	using namespace protocol_inherited;
	g_buf_size= (argc > 1) ? atoi(argv[1]) : g_buf_size;

	/*! Algorithm: \n
	 * 1) Initialize UDT library
	 */
	const int _val = CCustomer::sMInit(argc, argv, g_inditification_name,
			NSHARE::version_t(1, 0));
	if (_val != 0)
	{
		std::cerr << "Cannot initialize library as " << _val << std::endl;
		return EXIT_FAILURE;
	}

	/*! 2) Say to the kernel that I want to receive the messages number
	 * #test_inheritance::E_MSG_GRANDCHILD, #test_inheritance::E_MSG_CHILD, #test_inheritance::E_MSG_PARENT
	 * from #INDITIFICATION_NAME and it will be  handled  by function
	 * #msg_grand_child_handler, #msg_child_handler, #msg_parent_handler
	 */
	CCustomer::sMGetInstance().MIWantReceivingMSG( //
			requirement_msg_info_t(PROTOCOL_NAME,
					required_header_t(msg_head_t(E_MSG_GRANDCHILD, sizeof(grand_child_msg_t))),
					g_inditification_name), //
					msg_grand_child_handler);
	CCustomer::sMGetInstance().MIWantReceivingMSG( //
			requirement_msg_info_t(PROTOCOL_NAME,
					required_header_t(msg_head_t(E_MSG_CHILD, sizeof(child_msg_t))),
					g_inditification_name), //
					msg_child_handler);
	CCustomer::sMGetInstance().MIWantReceivingMSG( //
			requirement_msg_info_t(PROTOCOL_NAME,
					required_header_t(msg_head_t(E_MSG_PARENT, sizeof(parent_msg_t))),
					g_inditification_name), //
					msg_parent_handler);

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
	return test_inheritance::main(argc,argv);
}
