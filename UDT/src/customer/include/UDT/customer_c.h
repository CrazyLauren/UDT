/*
 * customer_c.h
 *
 *  Created on: 06.09.2020
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2020  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CUSTOMER_C_H_
#define CUSTOMER_C_H_

#include <UDT/customer/customer_c_type.h>
#include <UDT/customer_export.h>

#ifdef __cplusplus
#define CUSTOMER_C_H_EXTERN extern "C" CUSTOMER_EXPORT
#else
#define CUSTOMER_C_H_EXTERN extern CUSTOMER_EXPORT
#endif

/*!\brief C implementation off #NUDT::CCustomer::sMInit
 *
 */
CUSTOMER_C_H_EXTERN int init_udt_library(int argc,
		char const* argv[],
		char const* aName,
		version_c_t* aVersion, //or null
		char const* aConfPath //or null
		);
		
/*!\brief C implementation off #NUDT::CCustomer::sMInit
 *
 */
CUSTOMER_C_H_EXTERN int init_udt_library_default(
		char const* aName, int aMajorVersion, int aMinorVersion
		);
/*!\brief C implementation off #NUDT::CCustomer::sMFree
 *
 */
CUSTOMER_C_H_EXTERN void free_udt_library();

/*!\brief C implementation off #NUDT::CCustomer::sMVersion
 *
 */
CUSTOMER_C_H_EXTERN int library_version(version_c_t* aTo);

/*!\brief C implementation off #NUDT::CCustomer::MIsConnected
 *
 */
CUSTOMER_C_H_EXTERN int udt_is_connected();

/*!\brief C implementation off #NUDT::CCustomer::MIsOpened
 *
 */
CUSTOMER_C_H_EXTERN int udt_is_opened();

/*!\brief C implementation off #NUDT::CCustomer::MOpen
 *
 */
CUSTOMER_C_H_EXTERN int udt_open();

/*!\brief C implementation off #NUDT::CCustomer::MClose
 *
 */
CUSTOMER_C_H_EXTERN int udt_close();

/*!\brief C implementation off #NUDT::CCustomer::MWaitForEvent
 *
 */
CUSTOMER_C_H_EXTERN int udt_wait_for_event(char const* aEvent, double aSec);

/*!\brief C implementation off #NUDT::CCustomer::MGetID
 *
 */
CUSTOMER_C_H_EXTERN int udt_get_id(program_id_c_t *aTo);

/*!\brief C implementation off #NUDT::CCustomer::MCustomers
 *
 *@return The number of program_id_c_t in array
 * 		  or <= -2 if The number of program greater then array size (-value is
 * 		  requirement  array size)
 * 		  or -1 the other error
 */
CUSTOMER_C_H_EXTERN int udt_customers(program_id_c_t* aToArray,
		unsigned aSize);

/*!\brief C implementation off #NUDT::CCustomer::MSend
 *
 */
CUSTOMER_C_H_EXTERN int udt_send(char const* aProtocolName,
		void* aBuffer, unsigned aSize,
		unsigned aFlags //zero
		);
/*!\brief C implementation off #NUDT::CCustomer::MSend
 *
 */
CUSTOMER_C_H_EXTERN int udt_send_raw(required_header_c_t aNumber,
		char const* aProtocolName,
		void const* aBuffer, unsigned aSize,
		unsigned aFlags //zero
		);
/*!\brief C implementation off #NUDT::CCustomer::MSend
 *
 */
CUSTOMER_C_H_EXTERN int udt_send_to(char const* aProtocolName,
		void const* aBuffer, unsigned aSize,
		uint64_t aTo,
		unsigned aFlags //zero
		);
/*!\brief C implementation off #NUDT::CCustomer::MSend
 *
 */
CUSTOMER_C_H_EXTERN int udt_send_raw_to(required_header_c_t aNumber,
		char const* aProtocolName,
		void const* aBuffer, unsigned aSize,
		uint64_t aTo,
		unsigned aFlags //zero
		);

/*!\brief C implementation off #NUDT::CCustomer::MIWantReceivingMSG
 *
 */
CUSTOMER_C_H_EXTERN int udt_receiving_msg(requirement_msg_info_c_t aMSG,
		callback_received_data_t aHandler
		);

/*!\brief C implementation off #NUDT::CCustomer::MDoNotReceiveMSG
 *
 */
CUSTOMER_C_H_EXTERN int udt_unreceiving_msg(unsigned aHandler,
		request_info_c_t* aReuqest);

/*!\brief C implementation off #NUDT::CCustomer::MGetMyWishForMSG
 *
 *
 * @return The number of request_info_c_t in array
 * 		  or <= -2 if The number of program greater then array size (-value is
 * 		  requirement  array size)
 * 		  or -1 the other error
 */
CUSTOMER_C_H_EXTERN int udt_list_of_receiving(request_info_c_t* aToArray,
		unsigned aSize);

/*!\brief C implementation off #NUDT::CCustomer::MAdd
 *
 */
CUSTOMER_C_H_EXTERN int udt_add_event_connect(callback_connect_t aEvent,
		unsigned aPrior);
CUSTOMER_C_H_EXTERN int udt_add_event_disconnect(callback_connect_t aEvent,
		unsigned aPrior);

CUSTOMER_C_H_EXTERN int udt_add_event_receiver_subscribe(
		callback_subcribe_receiver_t aEvent, unsigned aPrior);
CUSTOMER_C_H_EXTERN int udt_add_event_receiver_unsubscribe(
		callback_subcribe_receiver_t aEvent, unsigned aPrior);

CUSTOMER_C_H_EXTERN int udt_add_event_fail_sent(
		callback_fail_sent_t aEvent, unsigned aPrior);
CUSTOMER_C_H_EXTERN int udt_add_event_customer_update(
		callback_customers_updated_t aEvent, unsigned aPrior);

/*!\brief C implementation off #NUDT::CCustomer::MErase
 *
 */
CUSTOMER_C_H_EXTERN int udt_erase_event_connect(callback_connect_t aEvent);
CUSTOMER_C_H_EXTERN int udt_erase_event_disconnect(
		callback_connect_t aEvent);

CUSTOMER_C_H_EXTERN int udt_erase_event_receiver_subscribe(
		callback_subcribe_receiver_t aEvent);
CUSTOMER_C_H_EXTERN int udt_erase_event_receiver_unsubscribe(
		callback_subcribe_receiver_t aEvent);

CUSTOMER_C_H_EXTERN int udt_erase_event_fail_sent(
		callback_fail_sent_t aEvent);
CUSTOMER_C_H_EXTERN int udt_erase_event_customer_update(
		callback_customers_updated_t aEvent);

/*!\brief C implementation off #NUDT::CCustomer::MJoin
 *
 */
CUSTOMER_C_H_EXTERN int udt_join();

/*!\brief C implementation off #IRtc::time_in_second_t
 *
 */
typedef double rtc_c_time_in_second_t;

/*!\brief C implementation off #IRtc::millisecond_t
 *
 */
typedef uint64_t rtc_c_millisecond_t; //!< time in miliseconds

/*!\brief C implementation off #UDT::rtc_info_t
 *
 */
typedef struct
{
	unsigned rtc_type;/*!< Type of RTC*/
	udt_customer_string_t name;/*!< The unique name of RTC*/
	rtc_c_time_in_second_t precision;/*!< The RTC precision */
	rtc_c_millisecond_t precision_ms;/*!< The RTC precision */
} rtc_info_c_t;

/*!\brief C implementation off #IRtc::MGetRTCInfo
 *
 *@return The number of rtc_info_c_t in array
 * 		  or <= -2 if The number of program greater then array size (-value is
 * 		  requirement  array size)
 * 		  or -1 the other error
 */
CUSTOMER_C_H_EXTERN int rtc_c_get_rtc_info(rtc_info_c_t* aTo,
		unsigned aSize);

/*!\brief C implementation off #IRtc::MGetRTCInfo
 *
 */
CUSTOMER_C_H_EXTERN int rtc_c_get_rtc_info_for(rtc_info_c_t* aTo,
		unsigned aId);

/*!\brief C implementation off #IRtc::MJoinToRTC
 *
 *\return RTC ID or -1 if error
 */
CUSTOMER_C_H_EXTERN int rtc_c_join(udt_customer_string_t aRtcName);

/*!\brief C implementation off #IRtc::MIsJoinToRTC
 *
 *\return 0 - False, 1 - True else error
 */
CUSTOMER_C_H_EXTERN int rtc_c_is_joined(unsigned aId);

/*!\brief C implementation off #IRtc::MLeaveFromRTC
 *
 */
CUSTOMER_C_H_EXTERN int rtc_c_leave_from_rtc(unsigned aId);

/*!\brief C implementation off #IRtc::MGetCurrentTime
 *
 *\return time or negative value if error is occured
 */
CUSTOMER_C_H_EXTERN rtc_c_time_in_second_t rtc_c_get_current_time(
		unsigned aId);

/*!\brief C implementation off #IRtc::MGetCurrentTimeMs
 *
 *\return time
 */
CUSTOMER_C_H_EXTERN rtc_c_millisecond_t rtc_c_get_current_time_ms(
		unsigned aId);

/*!\brief C implementation off #IRtc::MSleepUntil
 *
 */
CUSTOMER_C_H_EXTERN rtc_c_time_in_second_t rtc_c_sleep_until(
		unsigned aId, rtc_c_time_in_second_t aTime);

/*!\brief C implementation off #IRtc::MSleepUntil
 *
 */
CUSTOMER_C_H_EXTERN rtc_c_millisecond_t rtc_c_sleep_until_ms(
		unsigned aId, rtc_c_millisecond_t aTime);

/*!\brief C implementation off #NUDT::callback_rtc_t
 *
 */
typedef struct
{
	signal_rtc_t signal;//!< A pointer to the callback function @warning doesn't change first argument
	void* you_data;/*!<A pointer to data that you
				 want to pass as the second parameter
				 to the callback function when it's invoked.*/
} callback_rtc_c_t;

/*!\brief C implementation off #IRtc::MSetTimer
 *
 *\return 0 - False, 1 - error, -1 the other error
 */
CUSTOMER_C_H_EXTERN int rtc_c_set_timer(
		unsigned aId, rtc_c_time_in_second_t aFirstCall,
		rtc_c_time_in_second_t aIntervalCall,
		callback_rtc_c_t aCb
		);

/*!\brief C implementation off #IRtc::MSetTimer
 *
 *\return 0 - False, 1 - error, -1 the other error
 */
CUSTOMER_C_H_EXTERN int rtc_c_set_timer_ms(
		unsigned aId, rtc_c_millisecond_t aFirstCall,
		rtc_c_millisecond_t aIntervalCall,
		callback_rtc_c_t aCb
		);

/*!\brief C implementation off #IRtc::MNextTime
 *
 */
CUSTOMER_C_H_EXTERN rtc_c_time_in_second_t rtc_c_next_time(
		unsigned aId, rtc_c_time_in_second_t aTime);

/*!\brief C implementation off #IRtc::MNextTime
 *
 */
CUSTOMER_C_H_EXTERN rtc_c_millisecond_t rtc_c_next_time_ms(
		unsigned aId, rtc_c_millisecond_t aTime);

/*!\brief C implementation off #IRtc::MGetAmountOfJoined
 *
 */
CUSTOMER_C_H_EXTERN int rtc_c_amount_of_joined(
		unsigned aId);

#endif /* CUSTOMER_C_H_ */
