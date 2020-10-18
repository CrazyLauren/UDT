/*
 * customer_c_type.h
 *
 *  Created on: 06.09.2020
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2020  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CUSTOMER_C_TYPE_H_
#define CUSTOMER_C_TYPE_H_

#include <UDT/customer_export.h>
#include <UDT/customer/receive_data_info_c.h>

#ifdef __cplusplus
#define CUSTOMER_C_H_EXTERN extern "C" CUSTOMER_EXPORT
#else
#define CUSTOMER_C_H_EXTERN extern CUSTOMER_EXPORT
#endif

/*!\brief C implementation off #NSHARE::CBuffer
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NSHARE::CBuffer cpp_type_t;
#endif
	char const* ptr; /*!< pointer to data */
	unsigned size; /*!< data size*/
} buffer_c_t;

/*!\brief C implementation off #NUDT::id_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::id_t cpp_type_t;
#endif
	uint64_t uuid;/*!<  An unique program ID */
	udt_customer_string_t name;/*!<  An program name (non unique) */
} id_c_t;

/*!\brief C implementation off #NUDT::program_id_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::program_id_t cpp_type_t;
#endif
	id_c_t id;/*!< A program identifier */
	version_c_t version;/*!< A program version */
	unsigned time;/*!< A program start time */
	uint64_t pid;/*!< A program process ID */
	udt_customer_string_t path;/*!< A path to the program */
	unsigned type;/*!< A type of program*/
	unsigned endian;/*!< A byte order used by program*/
} program_id_c_t;

/*!\brief Collection of bitwise flags for change
 * subscription behavior
 *
 */
enum eRequrementFLags
{
	E_NO_FLAGS = 0, ///< The flags isn't set
	E_REGISTRATOR = 0x1 << 0,/*!< if it's set then message will be delivered to you
	 only if there is at least one "non-registrator" (real)
	 subscribed to the message*/
	E_AS_INHERITANCE = 0x1 << 2,/*!< it the message header is a parent for the other
	 message header.*/
	E_INVERT_GROUP = 0x1 << 3,/*!< if it's set then the order@com.ru.putin is not enter
	 into the order@com.ru, but is enter into the order@com.ru.putin.vv*/
	E_NEAREST = 0x1 << 4, /*!< if it's set then 	if there are next programs:
	 order@com.ru.people,
	 order@com.ru.putin.vv,
	 order@com.ru.kremlin,
	 than the order@com is included only order@com.ru.people
	 and order@com.ru.kremlin*/
};
/*!\brief C implementation off #NUDT::requirement_msg_info_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::requirement_msg_info_t cpp_type_t;
#endif
	udt_customer_string_t protocol_name;/*!< Type of message protocol*/
	required_header_c_t required;/*!< Header of requirement message*/
	unsigned flags; /*!< Subscription flags #eRequrementFLags*/
	udt_customer_string_t rfrom;/*!< Header of requirement message*/
} requirement_msg_info_c_t;

/*!\brief C implementation off #NUDT::received_data_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::received_data_t cpp_type_t;
#endif
	buffer_c_t buffer; /*!< Contained received data @warning Data will be removed, auto call !!!!! */
	uint8_t const* header_begin;/*!< pointer to the message header
								or NULL if message header is not exist*/
	uint8_t const* begin;/*!< pointer to the message begin*/
	uint8_t const* end;/*!< Pointer to end of the message (equal std::vector::end()) */
} received_data_c_t;

/*!\brief C implementation off #NUDT::received_message_args_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::received_message_args_t cpp_type_t;
#endif
	received_message_info_c_t info;/*!< Info about data*/	
	received_data_c_t message;/*!< Received message */
} received_message_args_c_t;

/*!\brief C implementation off #NUDT::customers_updated_args_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::customers_updated_args_t cpp_type_t;
#endif
	program_id_c_t disconnected[CUSTOMER_C_MAX_PROGRAM_ID];/*!< list of connected program
												The CUSTOMER_C_MAX_PROGRAM_ID is CMake value,
	 	 	 	 	 	 	 	 	 	 	 	 see build documentation
	*/
	unsigned disconnected_size;/*!< Length of array in #disconnected*/
	program_id_c_t connected[CUSTOMER_C_MAX_PROGRAM_ID];/*!< list of disconnected program
												The CUSTOMER_C_MAX_PROGRAM_ID is CMake value,
	 	 	 	 	 	 	 	 	 	 	 	 see build documentation
	*/
	unsigned connected_size;/*!< Length of array in #connected*/
} customers_updated_args_c_t;

/*!\brief C implementation off #NUDT::subcribe_receiver_args_t::what_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::subcribe_receiver_args_t::what_t cpp_type_t;
#endif
	requirement_msg_info_c_t what;/*!< The request */
	uint64_t who;/*!< Who want to receive message */
} what_c_t;

/*!\brief C implementation off #NUDT::subcribe_receiver_args_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::subcribe_receiver_args_t cpp_type_t;
#endif
	what_c_t receivers[CUSTOMER_C_MAX_MESSAGE_SUBSCRIBERS]; /*!< list of requirement messages
													The CUSTOMER_C_MAX_MESSAGE_SUBSCRIBERS
													is CMake value, see build documentation*/
	unsigned size;/*!< Length of array in #receivers*/
} subcribe_receiver_args_c_t;

/*!\brief C implementation off #NUDT::fail_sent_args_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::fail_sent_args_t cpp_type_t;
#endif
	uint64_t rfrom;/*!< see #received_message_args_c_t */
	udt_customer_string_t protocol_name;/*!< see #received_message_args_c_t */
	uint16_t packet_number;/*!< see #received_message_args_c_t */

	required_header_c_t header;/*!< see #received_message_args_c_t */

	uint32_t error_code;/*!< see A bitwise error code,
							see CCustomer structure fields E_* */

	uint8_t user_code;/*!< A user error or 0
						(see #received_message_args_t::FOccurUserError)*/

	uint64_t to[CUSTOMER_C_MAX_UUID_TO_SIZE];/*!< Where the data was sent*/
	unsigned to_size;/*!< Length of array in #to*/
	uint64_t fails[CUSTOMER_C_MAX_UUID_TO_SIZE];/*!< Where the data was not delivered*/
	unsigned fails_size;/*!< Length of array in #fails*/
} fail_sent_args_c_t;

/** Cb of empty event
 *
 */
typedef struct
{
} empty_event_c_t;

/*!\brief C implementation of #NUDT::signal_t for event
 * without arguments
 *
 */
typedef int (*signal_empty_t)(empty_event_c_t*, void*);

/*!\brief C implementation of #NUDT::signal_rtc_t
 *
 *
 */

typedef int (*signal_rtc_t)(void*, void*, void*);

/** #CCustomer::EVENT_CUSTOMERS_UPDATED
 *
 * @{
 */
/*!\brief C implementation of #NUDT::signal_t for
 * #CCustomer::EVENT_CUSTOMERS_UPDATED
 *
 */
typedef int (*signal_customers_updated_t)(customers_updated_args_c_t*, void*);

/*!\brief C implementation of #NUDT::callback_t
 *
 */
typedef struct
{
	signal_customers_updated_t signal;/*!< A pointer to the callback function*/
	void* you_data;/*!<A pointer to data that you
					 want to pass as the second parameter
					 to the callback function when it's invoked.*/
} callback_customers_updated_t;
/** @}
 */

/** #CCustomer::EVENT_FAILED_SEND
 *
 * @{
 */
/*!\brief C implementation off #NUDT::signal_t for #CCustomer::EVENT_FAILED_SEND
 *
 */
typedef int (*signal_fail_send_t)(fail_sent_args_c_t*, void*);

/*!\brief C implementation of #NUDT::callback_t
 *
 */
typedef struct
{
	signal_fail_send_t signal;/*!< A pointer to the callback function*/
	void* you_data;/*!<A pointer to data that you
					 want to pass as the second parameter
					 to the callback function when it's invoked.*/
} callback_fail_sent_t;
/** @}
 */

/** #CCustomer::EVENT_RECEIVER_SUBSCRIBE #CCustomer::EVENT_RECEIVER_UNSUBSCRIBE
 *
 * @{
 */
/*!\brief C implementation of #NUDT::signal_t
 * for #CCustomer::EVENT_RECEIVER_SUBSCRIBE #CCustomer::EVENT_RECEIVER_UNSUBSCRIBE
 *
 */
typedef int (*signal_subcribe_receiver_t)(subcribe_receiver_args_c_t*, void*);
/*!\brief C implementation off #NUDT::callback_t
 *
 */
typedef struct
{
	signal_subcribe_receiver_t signal;/*!< A pointer to the callback function*/
	void* you_data;/*!<A pointer to data that you
					 want to pass as the second parameter
					 to the callback function when it's invoked.*/
} callback_subcribe_receiver_t;
/** @}
 */

/** #CCustomer::EVENT_CONNECTED #CCustomer::EVENT_DISCONNECTED
 *
 * @{
 */

/*!\brief C implementation of #NUDT::callback_t
 * for #CCustomer::EVENT_CONNECTED #CCustomer::EVENT_DISCONNECTED
 *
 */
typedef struct
{
	signal_empty_t signal;/*!< A pointer to the callback function*/
	void* you_data;/*!<A pointer to data that you
					 want to pass as the second parameter
					 to the callback function when it's invoked.*/
} callback_connect_t;
/** @}
 */

/*!\brief C implementation off #NUDT::signal_t for
 * received data CB
 *
 */
typedef int (*signal_received_t)(received_message_args_c_t*, void*);

/*!\brief C implementation off #NUDT::callback_t
 *
 */
typedef struct
{
	signal_received_t signal;/*!< A pointer to the callback function*/
	void* you_data;/*!<A pointer to data that you
					 want to pass as the second parameter
					 to the callback function when it's invoked.*/
} callback_received_data_t;

/*!\brief C implementation off #NUDT::request_info_t
 *
 */
typedef struct
{
#ifdef __cplusplus
	typedef NUDT::fail_sent_args_t cpp_type_t;
#endif
	requirement_msg_info_c_t what;/*!< A requested message*/
	callback_received_data_t handler;/*!< A callback function*/
} request_info_c_t;

#endif /* CUSTOMER_C_TYPE_H_ */
