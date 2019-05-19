/*
 * no_route_test_api.h
 *
 *  Created on: 29.04.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef NO_ROUTE_TEST_API_H_
#define NO_ROUTE_TEST_API_H_

namespace test_errors
{
#define INDITIFICATION_NAME "no_route@guex" ///<Identification program name which will see another program
#define MESSAGE_NUMBER 6
#define MESSAGE_SIZE 10000

extern NSHARE::version_t g_requrement_msg_version;

/*!\brief Initialize library, add event handler,
 * subscription for a message (add handlers for
 * a message which will be received)
 *
 *\param argc - argc of main function
 *\param argv - argv of main function
 *
 *\return EXIT_FAILURE if the error is occured
 */
extern int initialize_library(int argc, char const*argv[]);

/*!\brief The function is called  for handling received message
 * of number #MESSAGE_NUMBER
 *
 * It's calculated speed of sending and receiving messages
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT A pointer to the structure #NUDT::received_message_args_t
 *			  in which is contained the message.
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *
 *\return 0
 */
extern int msg_handler(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*!\brief The function is called  for handling received message
 * of number #example_of_user_protocol::E_MSG_TEST
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT A pointer to the structure #NUDT::received_message_args_t
 *			  in which is contained the message.
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *
 *\return 0
 */
extern int msg_test_handler(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*! \brief The function is called when some program
 * wants to receive some message from me
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT A pointer to the structure #NUDT::subcribe_receiver_args_t
 *			  which is contained information about subscription.
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *\return 0
 */
extern int event_new_receiver(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*! \brief The function is called when some program doesn't more
 * want to receive some message from me
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT A pointer to the structure #NUDT::subcribe_receiver_args_t
 *			  which is contained information about subscription.
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *\return 0
 */
extern int event_remove_receiver(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*! \brief Handling connection to the kernel event
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT NULL
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *\return 0
 */
extern int event_connect_handler(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*! \brief Handling disconnection to the kernel event
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT NULL
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *\return 0
 */
extern int event_disconnect_handler(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*!\brief Handling the messages which isn't delivered to the
 * receivers
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT A pointer to the structure #NUDT::fail_sent_args_t
 *			  which is consisted information about error and message
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *
 *\return 0
 */
extern int event_fail_sent_handler(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*\brief Testing handling of invalid uuid of program
 *
 */
extern bool test_invalid_uuid();

/*\brief Testing handling of invalid message version
 *
 */
extern bool test_invalid_msg_version();

/*\brief Testing error during message parsing
 *
 */
extern bool test_parsing_error();

/*\brief Testing error which occured if parse for the protocol is not exist
 *
 */
extern bool test_no_parser_error();

/*\brief Doing tests
 *
 */
extern void doing_tests();

/*\brief Main function of test
 *
 */
int main(int argc, char const*argv[]);

}

#endif /* NO_ROUTE_TEST_API_H_ */
