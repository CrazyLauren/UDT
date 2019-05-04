/*
 * api_example_quick_answer.h
 *
 *  Created on: 29.04.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef EXAMPLE_QUICK_ANSWER_API_H_
#define EXAMPLE_QUICK_ANSWER_API_H_

namespace example_quick_answer
{
#define INDITIFICATION_NAME "uex_quick@guex" ///<Identification program name which will see another program
#define MESSAGE_NUMBER 1 				///<The unique number of message used for sending(receiving) the message
#define PACKET_SIZE 200000 				///<The size of message #MESSAGE_NUMBER

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

/*\brief Easiest example of sending(publishing) messages
 *
 */
extern void send_messages();

/*\brief Main function of test
 *
 */
int main(int argc, char const*argv[]);
}

#endif /* EXAMPLE_QUICK_ANSWER_API_H_ */
