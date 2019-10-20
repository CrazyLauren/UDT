/*
 * api_example_customer_for_user_protocol.h
 *
 *  Created on: 29.04.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef EXAMPLE_CUSTOMER_API_H_
#define EXAMPLE_CUSTOMER_API_H_

namespace example_of_using_of_user_protocol
{
#define INDITIFICATION_NAME "uex2@guex" ///<Identification program name which will see another program

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

/*!\brief Handling information about changing program composition
 *
 *\param WHO A pointer to the structure Customer which is invoked function
 *\param WHAT A pointer to the structure #NUDT::customers_updated_args_t
 *\param YOU_DATA A pointer to data that you has been wanted to pass
 *				  as the third parameter (NULL In this case).
 *
 *\return 0
 */
extern int event_customers_update_handler(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*\brief Easiest example of sending(publishing) messages
 *
 */
extern void send_messages();

/*\brief Main function of test
 *
 */
int main(int argc, char const*argv[]);

/*\brief Recommended method of a message sending
 * by user protocol
 *
 */
void send_message();

/*\brief The buffer will be passed to the kernel
 * and when it parses the buffer for making
 * messages. Into the buffer can be more than one
 * message.
 *
 *\warning Non recommended method of a message sending
 *by user protocol as the method has been added
 *for compatibility with existing source code.
 */
void send_buffer_which_is_consist_msg();
}

#endif /* EXAMPLE_CUSTOMER_API_H_ */
