/*
 * CCustomer.h
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCUSTOMER_H_
#define CCUSTOMER_H_

#include <UDT/config/config.h>
#include "customer_export.h"
#include <UDT/udt_types.h>
#include <UDT/programm_id.h>

#ifdef uuid_t
#	error "Opa!!!"
#endif
namespace NUDT
{
class CCustomer;
class IRtc;

/*!\brief Information about requirement message
 *
 *\note
 * Non-POD type.
 *
 *\see requirement_msg_info_c_t
 *\see NUDT::CCustomer
 */
struct CUSTOMER_EXPORT requirement_msg_info_t
{
	/*!\brief Collection of bitwise flags for change
	 * subscription behavior
	 *
	 */
	enum eFLags
	{
		E_NO_FLAGS=0,///< The flags isn't set
		E_REGISTRATOR=0x1<<0,/*!< if it's set then message will be delivered to you
							  only if there is at least one "non-registrator" (real)
							  subscribed to the message*/
		E_AS_INHERITANCE=0x1<<2,/*!< it the message header is a parent for the other
								message header.*/
		E_INVERT_GROUP=0x1<<3,/*!< if it's set then the order@com.ru.putin is not enter
							 into the order@com.ru, but is enter into the order@com.ru.putin.vv*/
		E_NEAREST=0x1<<4,/*!< if it's set then 	if there are next programs:
						 order@com.ru.people,
						 order@com.ru.putin.vv,
						 order@com.ru.kremlin,
						 than the order@com is included only order@com.ru.people
						 and order@com.ru.kremlin*/
	};

	NSHARE::CText 		FProtocolName; ///<Type of message protocol
	required_header_t 	FRequired;///< Header of requirement message
	unsigned 			FFlags;///< subscription flags
	NSHARE::CText 		FFrom;///< A requirement message from (for detail see NUDT::CCustomer)

	/*!\brief To constructor may be passed to be copied
	 * a protocol name, a message header, from receive the message and flags
	 *
	 *\param aProtocol A type of message protocol
	 *\param aWhat A message header (number)
	 *\param aFrom A from receive the message (for detail see NUDT::CCustomer)
	 *\param aFlags A bitwise flags #eFLags
	 */
	requirement_msg_info_t(NSHARE::CText const& aProtocol,required_header_t const& aWhat,
			NSHARE::CText const& aFrom,unsigned aFlags=E_NO_FLAGS);

	/*!\brief The default constructor creates fields using their
	 *  respective default constructors.
	 */
	requirement_msg_info_t();

	/*!\brief Two @c requirement_msg_info_t are equal if their members are equal.
	 *
	 *\param aRht What to compare object with
	 *\return true if the objects are equal
	 */
	bool operator==(requirement_msg_info_t const& aRht) const;

	static const NSHARE::CText NAME;//!<Serialize key
	static const NSHARE::CText KEY_PROTOCOL_NAME;//!<Serialize key of #FProtocolName
	static const NSHARE::CText KEY_FLAGS;//!<Serialize key of #FFlags
	static const NSHARE::CText KEY_FROM;//!<Serialize key of #FFrom
};

/*!\brief Storage of received data
 * which is contained the requirement message
 *
 * The Pointers of type are pointed into FBuffer.
 * The data from FHeaderBegin to FBegin is
 * header. The message header can not exist or not equal of
 * expected if:\n
 * - it's message of child type;\n
 * - the message header and data sent separately (see send API)
 * (usually do that).
 * @note
 *  FBuffer.end() can be not equal #FEnd @n
 *  FBuffer.begin() can be not equal #FBegin @n
 *  Non-POD type.
 *  @bug The value (#FEnd-#FBegin) can be @b greater
 *  than the message data size if the message has been inherited.
 *  Usually (Otherwise it doesn't work)
 *  inherited only a @b fixed @b size @b message therefore
 *  if you want to check the message data size using
 *  operator @a">=" instead of @a"==" for
 *  compatibility.
 */
struct CUSTOMER_EXPORT received_data_t
{
	NSHARE::CBuffer FBuffer;///< Contained received data

	const uint8_t* FHeaderBegin;/*!< pointer to the message header
								or NULL if message header is not exist*/
	const uint8_t* FBegin;///<pointer to the message begin
	const uint8_t* FEnd;/*!< Pointer to end of the message (equal std::vector::end())*/

	/*!\brief The default constructor creates fields using their
	 *  respective default constructors (For it's NULL).
	 */
	received_data_t();

	static const NSHARE::CText NAME;//!<Serialize key
	static const NSHARE::CText KEY_HEADER_BEGIN;//!<Serialize key of #FHeaderBegin
};

/*!\brief information about received data
 *
 * You can send error to sender by
 * changing field FOccurUserError.
 *
 *\note
 *  Non-POD type.
 */
struct CUSTOMER_EXPORT received_message_info_t
{
	typedef std::vector<NSHARE::uuid_t> uuids_t;///< Information about programs uuid

	NSHARE::uuid_t 		FFrom;///<UUID of message sender
	NSHARE::CText 		FProtocolName;///<Type of message protocol
	uint16_t 			FPacketNumber;/*!<The packet (message) sequence number (continuous numbering
	 	 	 	 	 	 	for all packets from sender)
							two message can identical number only
							if is sent in one packet (buffer), usually that two
							messages in one buffer isn't sent (see send buffer method).*/
	required_header_t 	FHeader;///< The message header
	uuids_t 			FTo;///< List uuids of message receiver (sorted)
	mutable uint8_t 	FOccurUserError;/*!<if the field will changed by you, when
										the error with code  FOccurUserError
										is sent to message sender (FFrom)*/
	unsigned 			FEndian;///< Message byte order (see eEndian in endian_type.h)
	unsigned 			FRemainCallbacks;///!<Amount of CB which of called after It CB
	unsigned 			FCbs;///!<Amount of CB which is called
	unsigned 			FFlags;///< subscription flags #requirement_msg_info_t::eFlags


	static const NSHARE::CText NAME;//!<Serialize key
	static const NSHARE::CText KEY_FROM;//!<Serialize key of #FFrom
	static const NSHARE::CText KEY_PROTOCOL_NAME;//!<Serialize key of #FProtocolName
	static const NSHARE::CText KEY_PACKET_NUMBER;//!<Serialize key of #FPacketNumber
	static const NSHARE::CText KEY_TO;//!<Serialize key of #FTo
	static const NSHARE::CText KEY_OCCUR_USER_ERROR;//!<Serialize key of #FOccurUserError
	static const NSHARE::CText KEY_ENDIAN;//!<Serialize key of #FEndian
	static const NSHARE::CText KEY_REMAIN_CALLBACKS;//!<Serialize key of #FRemainCallbacks
	static const NSHARE::CText KEY_CBS;//!<Serialize key of #FCbs
	static const NSHARE::CText KEY_FLAGS;//!<Serialize key of #FFlags
};

/*!\brief information contained in aWHAT argument
 * of function which is handle received data ( see Receive API)
 *
 * You can send error to sender by
 * changing field FOccurUserError.
 *
 *\note
 *  Non-POD type.
 */
struct CUSTOMER_EXPORT received_message_args_t:received_message_info_t
{
	received_data_t 	FMessage;///< Received message


	static const NSHARE::CText NAME;//!<Serialize key
};

/*!\brief Information about connected
 * (disconnected) program to kernel
 *
 * It's argument (aWHAT) of event: EVENT_CUSTOMERS_UPDATED
 *
 *\note
 *  Non-POD type.
 */
struct CUSTOMER_EXPORT customers_updated_args_t
{
	std::set<program_id_t> FDisconnected;///< list of connected program
	std::set<program_id_t> FConnected;///< list of disconnected program

	static const NSHARE::CText NAME;//!<Serialize key
	static const NSHARE::CText KEY_DISCONNECTED;//!<Serialize key of #FDisconnected
	static const NSHARE::CText KEY_CONNECTED;//!<Serialize key of #FConnected
};

/*!\brief Information about requirement message for
 *
 * It's argument (aWHAT) of event: EVENT_RECEIVER_SUBSCRIBE and
 * EVENT_RECEIVER_UNSUBSCRIBE
 *
 *\note
 *  Non-POD type.
 */
struct CUSTOMER_EXPORT subcribe_receiver_args_t
{
	/*!\brief what requirement
	 *
	 */
	struct what_t
	{
		requirement_msg_info_t 	FWhat;///< The request
		NSHARE::uuid_t 	FWho;///< Who want to receive message

		static const NSHARE::CText KEY_WHO;//!<Serialize key of #FWho
		static const NSHARE::CText NAME;//!<Serialize key
	};
	typedef std::vector<what_t> receivers_t;///<list of requirement messages

	receivers_t FReceivers;///< list of requirement messages
	static const NSHARE::CText NAME;//!<Serialize key
	static const NSHARE::CText KEY_RECEIVERS;//!<Serialize key of #FReceivers
};

/*!\brief Information about not delivered message
 *
 */
struct CUSTOMER_EXPORT fail_sent_args_t
{
	typedef uint32_t error_t;///< A bitwise error type
	typedef std::vector<NSHARE::uuid_t> uuids_t;///< Information about programs uuid

	NSHARE::uuid_t 		FFrom;///<see received_message_args_t
	NSHARE::CText 		FProtocolName;///<see received_message_args_t
	uint16_t 			FPacketNumber;///< see received_message_args_t

	required_header_t 	FHeader;///< see received_message_args_t

	error_t 			FErrorCode;///<A bitwise error code, see CCustomer structure fields E_*

	uint8_t 			FUserCode;///<A user error or 0 (see received_message_args_t::FOccurUserError)

	uuids_t 			FSentTo;///< Where the data was sent
	uuids_t 			FFails;///< Where the data was not delivered


	static const NSHARE::CText NAME;//!<Serialize key
	static const NSHARE::CText KEY_FROM;//!<Serialize key of #FFrom
	static const NSHARE::CText KEY_PROTOCOL_NAME;//!<Serialize key of #FProtocolName
	static const NSHARE::CText KEY_PACKET_NUMBER;//!<Serialize key of #FPacketNumber
	static const NSHARE::CText KEY_ERROR_CODE;//!<Serialize key of #FErrorCode
	static const NSHARE::CText KEY_USER_ERROR;//!<Serialize key of #FUserError
	static const NSHARE::CText KEY_SENT_TO;//!<Serialize key of #FSentTo
	static const NSHARE::CText KEY_FAILS;//!<Serialize key of #FFails
};

/*!\brief type of callback function which used in CCustomer
 *
 *
 *\param WHO - pointer to structure Customer
 *\param WHAT - A pointer to a structure that describes
 *				 the event that caused the callback to be
 *				 invoked, or NULL if there isn't an event.
 *				 The format of the data varies with event type.
 *				 (see *_args_t structures)
 *\param YOU_DATA -A pointer to data that you wanted to pass
 *					 as the third parameter(FYouData) callback_t structure.
 *
 *\return by default Callback functions must return 0
 *			for detail see NSHARE::eCBRval
 *
 *\see callback_t#operator()()
 */
typedef int (*signal_t)(CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*!\brief Regular callback structure used in "Customer" structure
 *
 *\see signal_t
 */
struct callback_t
{
	signal_t FSignal; ///< A pointer to the callback function
	void* 	FYouData;/*!<A pointer to data that you
					 want to pass as the third parameter
					 to the callback function when it's invoked.*/

	/*!\brief The default constructor initializes
	 * @c FSignal and @c FYouData to NULL.
	 */
	callback_t();

	/*!\brief Only a pointer to the callback function
	 * will be passed to constructor.
	 *
	 * To the third parameter to the callback function will
	 * be passed NULL value.
	 *
	 *\param aSignal - A pointer to the callback function
	 */
	callback_t(signal_t const& aSignal);

	/*!\brief Two objects may be passed to a @c callback_t
	 * constructor to be copied
	 */
	callback_t(signal_t const& aSignal, void * const aData);

	/*!\brief There is also a copy constructor for
	 * the @c callback_t class itself.
	 */
	callback_t(callback_t const& aCB);

	/*!\brief callback_t assignment operator.
	 *
	 *\param  aCB  A callback_t of identical element.
	 *
	 *\return reference to this
	 */
	callback_t& operator=(callback_t const& aCB);

	/*!\brief Return true if A pointer to the callback
	 *function is exist
	 *
	 *\return true if @c FSignal is not NULL
	 */
	bool MIs() const;

	/*!\brief Invoke the callback and pass to it
	 *first argument aWho, second argument aArgs, third
	 *argument FYouData.
	 *
	 *\param aWho Who is invoking the callback
	 *\param aArgs a pointer to structure which provides information about related
	 *to the event callback being invoked. The format of the data varies with event type.
	 */
	int operator ()(CCustomer* aWho, void * const aArgs) const;

	/*!\brief Two @c callback_t are equal if their members are equal.
	 *
	 *\param aRht What to compare object with
	 *\return true if the objects are equal
	 */
	bool operator ==(callback_t const& aRht) const;
};

/*!\brief Information about event handler
 *
 */
struct event_handler_info_t
{
    /*!\brief The default constructor creates fields using their
     * respective default constructors.
     */
	event_handler_info_t();

	/*!\brief Two objects may be passed to a constructor to be copied.
	 *
	 *\param aKey - aName of event
	 *\param aCb - event handler
	 */
	event_handler_info_t (NSHARE::CText const& aKey,callback_t const& aCb);

	/*!\brief Three objects may be passed to a constructor to be copied.
	 *
	 *\param aKey - aName of event
	 *\param aSignal - A pointer to the callback function
	 *\param aData - A pointer to data that you
					 want to pass as the third parameter
					 to the callback function when it's invoked.
	 */
	event_handler_info_t(NSHARE::CText const& aKey, signal_t const& aSignal,
			void * const aData=NULL);

	NSHARE::CText 	FKey;///< A Name of Event
	callback_t 		FCb;/*!<A callback function that will handle
							the event when it occurs*/
};
/*!\brief Information about requested message and
 * it callback function
 *
 */
struct CUSTOMER_EXPORT request_info_t
{
	requirement_msg_info_t 	FWhat;///<A requested message
	callback_t 		FHandler;///<A callback function
};

/*!\brief A main class used for communication
 *
 *		The data is exchanged asynchronously with callback
 *	function by publisher subscriber pattern. i.e
 *	Senders of messages, called "PUBLISHERS",
 *	are not have a program code of sending the messages
 *	directly to specific receivers, called "SUBSCRIBERS",
 *	Instead of this, PUBLISHERS (Senders) categorize
 *	published messages without knowledge of	which
 *	SUBSCRIBERS (Receivers). Similarly, SUBSCRIBERS
 *	(Receivers) request (subscribes) one or more messages
 *	and receive only those messages	that are of want,
 *	without the knowledge of which	PUBLISHER (Sender).
 *	SUBSCRIBERS can request message from
 *	specific PUBLISHERS by it name or from specific group
 *	of PUBLISHERS. Thus any program name involved in
 *	data exchange is consists two part:
 *	- non-unique name
 *	- groups
 *	and is defined as string is made up of a lowercase
 *	name ( Latin letters a to z, digits 0 to 9,
 *	!#$%&'+-/=?^_`{|}~), an @ symbol, then a lowercase group
 *	which is defined using a hierarchical
 *	naming pattern, with some levels in the hierarchy
 *	separated by periods (."dot").
 *	For example: non_unique_name@group1.group2.group3
 *	If no group: non_unique_name
 *	Respectively message request can be:
 *		 non_unique_name 	- for receive message only from "non_unique_name"
 *		 @group1.group2 	- for receive message from any program which is
 *		 					part of	group "group1.group2": for example
 *		 				   	rand@group1.group2.group3
 *		 name@group1		- for receive message only from "non_unique_name"
 *		 					which is part of group "group1".
 *
 *		If errors occurred during the data transfer,
 *	than an event EVENT_FAILED_SEND were occurs
 *	in PUBLISHERS and SUBSCRIBERS. Its event is
 *	contained information about error and message
 *	type.
 *		For convenience, the message always
 *	is delivered in true byte order, whatever the byte
 *	order of the message when it was sending.
 *		Exactly one object is used for communication, It's
 *	created by sMInit method and freed by sMFree method.
 *	To organize communication used so-called "modules",
 *	which is realizing data communication with kernel.
 *		What "module" will be used, is specified
 *	in the configuration file.
 *		Before used library It can be initialized be the method sMInit
 *	and create channel to kernel by method MOpen.
 *		\warning To publisher the message is needed allocate
 *	memory by calling MGetNewBuf() method, and
 *	then creating message into the allocated memory.
 *	It is not recommended to violate this principle
 *	publishing message
 *
 *
 */
class CUSTOMER_EXPORT CCustomer: public NSHARE::CSingleton<CCustomer>
{
public:
	static const NSHARE::CText ENV_CONFIG_PATH;/*!< environment name used for specification
												path to the config file*/

	/// \name Keys using in configure file
	/// \{

	static const NSHARE::CText MODULES;/*!< A name of key for specification
										list of the additional loaded libraries*/
	static const NSHARE::CText MODULES_PATH;/*!< A name of key for specification
											where looking for additional libraries*/
	static const NSHARE::CText DOING_MODULE;/*!< A name of key in the config file for specification
											used "manager" if not set is used manager from DEFAULT_IO_MANAGER*/
	static const NSHARE::CText DEFAULT_IO_MANAGER;///< A name of default "manager"
	static const NSHARE::CText RRD_NAME;/*!< A name of key  for specification
										the program name in the system if not set used from parameter aName of sMinit
										if it is not set too that used the real name of program*/
	static const NSHARE::CText THREAD_PRIORITY;/*!< A name of key  for specification
												a thread priority of event callback*/
	static const NSHARE::CText NUMBER_OF_THREAD;/*!< A name of key for specification
												amount of thread is used for event callback*/
	/// \}

	static const NSHARE::CText RAW_PROTOCOL;///< A name of predefined protocol

	/// \name Keys of available events
	/// \{
	static const NSHARE::CText EVENT_RAW_DATA;/*!< It's invoked when any data is received.
												Each callback is passed  received_message_args_t structure*/
	static const NSHARE::CText EVENT_CONNECTED;/*!< It's invoked when you connected to the kernel.
												 The callback is called without additional arguments.*/
	static const NSHARE::CText EVENT_DISCONNECTED;/*!< It's invoked when you disconnected from the kernel.
												  The callback is called without additional arguments.*/
	static const NSHARE::CText EVENT_CUSTOMERS_UPDATED;/*!< It's invoked when some programs are connected
														 or disconnected from the kernel.
														 Each callback is passed  customers_updated_args_t structure.*/
	static const NSHARE::CText EVENT_FAILED_SEND;/*!< It's invoked when the message from you cannot be delivered
												 to receiver (subscriber) or the message to you cannot be delivered
												 from sender (publisher).
												 Each callback is passed  fail_sent_args_t structure.*/
	static const NSHARE::CText EVENT_RECEIVER_SUBSCRIBE;/*!< It's invoked when a receiver (subscriber)
														 wanted to receive the message from you (publisher).
														 Each callback is passed  subcribe_receiver_args_t structure.*/

	static const NSHARE::CText EVENT_RECEIVER_UNSUBSCRIBE;/*!<It's invoked when a receiver (subscriber)
														 did not want to receive the message from you (publisher).
														 Each callback is passed  subcribe_receiver_args_t structure.*/
	static const NSHARE::CText EVENT_UPDATE_RTC_INFO;/*!< It's invoked when you updated RTC info.
												 The callback is called without additional arguments.*/
	/// \}

	typedef std::vector<NSHARE::CText> modules_t;///< An information about modules
	typedef std::set<program_id_t> customers_t;///< An information about program

	typedef std::vector<NSHARE::intrusive_ptr<IRtc> > rtc_list_t;
	/*!\brief Collection of bitwise flags for change
	 * sending behavior
	 *
	 */
	enum eSendToFlags
	{
		E_NO_SEND_FLAGS = 0,///< No send flags
	};

	/*!\name The bitwise error number constants of an argument
	 *fail_sent_args_t::FErrorCode.
	 *
	 *For printing error calling sMPrintError Method
	 *\{
	 */
	typedef fail_sent_args_t::error_t error_t;///< Type of bitwise error

	static const error_t E_HANDLER_IS_NOT_EXIST;/*!< Cannot find a callback function for handling a message.
												Usually It occured during disconnecting of the message receiver.*/
	static const error_t E_NO_ROUTE;/*!< Not route to receiver (subscriber).
									Usually It occured during disconnect of the message receiver or problems with net.*/
	static const error_t E_UNKNOWN_ERROR;///< Wtf?
	static const error_t E_PARSER_IS_NOT_EXIST;/*!< The parser of the raw buffer for specified protocol
												 is not loaded (exist) to kernel.
												 For fix: load the parser of the raw buffer for specified protocol
												to the kernel.*/
	static const error_t E_CANNOT_PARSE_BUFFER;///< Cannot parse the sent buffer
	static const error_t E_SOCKET_CLOSED;///<The receiver has been disconnected yet
	static const error_t E_BUFFER_IS_FULL;/*!<The block of memory which kernel has been allocated to receiver is full.
											For fix error: change size of memory that the allocated to receiver or
											increase the performance of the message receiver*/
	static const error_t E_PACKET_LOST;/*!< The packet has been lost during delivering of message.
										Sometimes it happens in the bad net*/
	static const error_t E_DATA_TOO_LARGE;/*!< In the kernel can be set the max message size which is transmitted by net (line)
										  The error is occured if sender is exceed the limit and the packet division mode of
										  the kernel is not set.*/
	static const error_t E_MERGE_ERROR;/*!< The error is occured when if in the packet division mode of the kernel, it cannot
										merge the splitted message.*/
	static const error_t E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE;/*!< The version of the sent message and the requirement
																message is not compatible*/
	static const error_t E_USER_ERROR_EXIST;/*!<If user's error is set ( see FOccurUserError of received_message_args_t)
											The error is occured*/
	static const unsigned MAX_SIZE_USER_ERROR;///< The amount of bits are used for save user's error


	/*!\brief Printing bitwise error code to stream
	 *
	*\param aStream where to print
	*\param aError error bitwise code
	*\return aStream
	 */
	static std::ostream& sMPrintError(std::ostream& aStream,error_t const& aError);
	/// \}

	/*!\brief An error code which is returned by function
	 *
	 */
	enum eAPIError
	{
		ERROR_CANNOT_READ_CONFIGURE=-255,///< Cannot found (read) configuration file
		ERROR_CONFIGURE_IS_INVALID,///< Some value in configuration file is not valid
		ERROR_NO_NAME,///< The name of program is not set
		ERROR_NOT_OPEN,///< The library is not "opened".
		ERROR_NAME_IS_INVALID,///< The name of program is not valid
		ERROR_NOT_CONNECTED_TO_KERNEL,///< The library is not connected to the kernel
		ERROR_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE,/*!<Cannot allocate requirement size the buffer
													 in the shared memory (to fix it change the size of
													 created by the kernel shared memory)*/
		ERROR_UNEXPECETED,///< Wtf?
		ERROR_HANDLER_IS_NOT_EXIST,///< Cannot find a callback function for handling a message.
	};

	/*!\brief Initialize library
	 *
	 *	Initialize the library and create singleton for this type
	 *
	*\param argc The number of entries in the argv array
	*\param argv An array of pointers to strings that contain the arguments to the program
	*\param aName The desired name of program
	*\param aProgramName - Name of "exe" file
	*\param aLogOption - option of logging
	*\param aVersion Version of program
	*\param aConfPath Path to the configuration file if it not set
	 *					The file is read from environment specificated in ENV_CONFIG_PATH.
	*\return 0 - EOK
	 *			else bitwise error code
	 */
	static int sMInit(int argc, char const* argv[], char const* aName,NSHARE::version_t const&  aVersion=NSHARE::version_t(),
	    const NSHARE::CText& aConfPath=
            UDT_CONFIG_DEFAULT_PATH
	        "default_customer_config.json");

	static int sMInit(char const* aProgramName,
			char const* aName,
			char const* aLogOption = NULL,
			NSHARE::version_t const&  aVersion=NSHARE::version_t(),
	    const NSHARE::CText& aConfPath=
            UDT_CONFIG_DEFAULT_PATH
	        "default_customer_config.json");
	/*!\brief Initialize library
	 *
	 *	Initialize the library and create singleton for this type
	 *
	*\param argc The number of entries in the argv array
	*\param argv An array of pointers to strings that contain the arguments to the program
	*\param aName The desired name of program
	*\param aProgrammVersion Version of program
	*\param aConfPath configuration
	*\return 0 - EOK
	 *			else bitwise error code
	 */
	static int sMInit(int argc, char const* argv[], char const* aName,NSHARE::version_t const& aProgrammVersion,	const NSHARE::CConfig& aConfPath);

	/*!\brief deinitialization library
	 *
	 *	Remove singelton and free all used resources.
	 */
	static void sMFree();

	/*!\brief Get information about library version
	 *
	 *\return Library version
	 */
	static const NSHARE::version_t& sMVersion();

	/*!\brief Check for available module
	 *
	*\param aModule Name of module
	*\return true if exist
	 */
	bool MAvailable(const NSHARE::CText& aModule) const;

	/*!\brief Get list of available modules
	 *
	*\return modules
	 */
	modules_t MModules() const;

	/**\brief Check if you connect to kernel
	 *
	*\return true if connected
	 */
	bool MIsConnected() const;

	/**\brief Check if you create channel to library
	 *
	*\return true if created
	 */
	bool MIsOpened() const;

	/**\brief Open channel to kernel
	 *
	 *	After the channel to kernel is opened, the
	 *	connection to kernel begins. When the connection
	 *	is established the event EVENT_CONNECTED is occured.
	 *
	*\return true if opened successfully
	 */
	bool MOpen();

	/**\brief Close channel to kernel
	 *
	 */
	void MClose();

	/*!\brief wait for some event is occured (and handled)
	 *
	 * For events #EVENT_CONNECTED and #EVENT_DISCONNECTED
	 * connection is checked. That is, if it's connected and the
	 * MWaitForEvent method with #EVENT_CONNECTED is called
	 * when returned error #ERROR_NOT_CONNECTED_TO_KERNEL.
	 *
	 *\param aEvent - waited for event
	 *\param aSec - time out if value < 0 than wait for infinitely
	 *
	 *\warning Non-recommended to use the method for any events
	 *	associated with receiving data
	 *
	 *\return < 0 if the error is occured:\n
	 *		#ERROR_UNEXPECETED if the event has not registered.
	 */
	int MWaitForEvent(NSHARE::CText const& aEvent,double aSec=-1);

	/*!\brief wait for some events is occured (and handled)
	 *
	 * For events #EVENT_CONNECTED and #EVENT_DISCONNECTED
	 * connection is checked. That is, if it's connected and the
	 * MWaitForEvent method with #EVENT_CONNECTED is called
	 * when returned error #ERROR_NOT_CONNECTED_TO_KERNEL.
	 *
	 *\param aEvent - waited for event
	 *\param aSec - time out if value < 0 than wait for infinitely
	 *
	 *\warning Non-recommended to use the method for any events
	 *	associated with receiving data
	 *
	 *\return < 0 if the error is occured:\n
	 *		#ERROR_UNEXPECETED if the event has not registered.
	 */
	int MWaitForEvent(NSHARE::Strings const& aEvent,double aSec=-1);

	/*!\brief Get information about you
	 *
	 * You uuid and name is contained in the field  of program_id_t::FId.
	 *
	 *\return Information about you
	 */
	const program_id_t& MGetID() const;

	/*!\brief Get information about all program
	 *
	 *\return information about all program
	 */
	customers_t MCustomers() const;

	/*!\brief Get information about program by uuid
	 *
	 *\param aUUID uuid of program
	 *\return A program info if the program is exist\n
	 *		  Invalid object of type program_id_t (see program_id_t::MIsValid())\n
	 *		  if program is not exist
	 */
	program_id_t MCustomer(NSHARE::uuid_t const& aUUID) const;

	/*!\brief Publication(Send) the buffer which is contained one or more
	 * message
	 *
	 * When the kernel received the buffer, it try parses the buffer for
	 * generate the message. For parsing the kernel  is used the outer library
	 * which is loaded in concordance with the configuration file.
	 * The message in the buffer has to be consist of the head and data,
	 * following each other. If the library corresponded to the protocol
	 * is not exist when the error #E_PARSER_IS_NOT_EXIST is occured
	 *
	 *
	 *\param  aProtocolName A Type of the packet protocol
	 *\param  aBuffer A pointer to the buffer that you want to parse and send
	 *\param  aSize The size of the buffer, in bytes
	 *\param  aFlags A combination of the send flag
	 *\warning 	The method is non-recommended to use because some features
	 *			can be not available for this method (e.g. inheritance)
	 *			Usually it is used for compatibility with old version of program
	 *\return >0 if the buffer is sent successfully. The value is equal the packet\n
	 *			 number of received_message_args_t::FPacketNumber
	 *		  0 if loopback mode (the data is sent to youself)\n
	 *		  <0 if error is occured (see bitwise error codes E_*)
	 */
	int MSend(NSHARE::CText aProtocolName, void const* aBuffer, size_t aSize, eSendToFlags aFlags= E_NO_SEND_FLAGS);

	/*!\overload
	 *
	 *\param aBuffer A pointer to the buffer that you want to parse and send. (It will moved!)
	 *\param  aProtocolName A Type of the packet protocol
	 *\param  aFlags A combination of the send flag
	 *\warning  aBuffer object will be empty if the buffer is sent to the kernel successfully
	*/
	int MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer, eSendToFlags aFlags= E_NO_SEND_FLAGS);

	/*!\brief Send to the specified receiver the buffer
	 * which is contained one or more message
	 *
	 * When the kernel received the buffer, it try parses the buffer for
	 * generate the message. For parsing the kernel  is used the outer library
	 * which is loaded in concordance with the configuration file.
	 * The message in the buffer has to be consist of the head and data,
	 * following each other. If the library corresponded to the protocol
	 * is not exist when the error #E_PARSER_IS_NOT_EXIST is occured
	 *
	 *
	 *\param  aProtocolName A Type of the packet protocol
	 *\param  aBuffer A pointer to the buffer that you want to parse and send
	 *\param  aSize The size of the buffer, in bytes
	 *\param  aTo where to send
	 *\param  aFlags A combination of the send flag
	 *\warning 	The method is non-recommended to use because some features
	 *			can be not available for this method (e.g. inheritance)
	 *			Usually it is used for compatibility with old version of program
	 *\return >0 if the buffer is sent successfully. The value is equal the packet
	 *			 number of received_message_args_t::FPacketNumber; \n
	 *		  0 if loopback mode (the data is sent to youself) \n
	 *		  <0 if error is occured (see bitwise error codes E_*)
	 */
	int MSend(NSHARE::CText aProtocolName, void const* aBuffer, size_t aSize,
			const NSHARE::uuid_t& aTo, eSendToFlags aFlags= E_NO_SEND_FLAGS);

	/*!\overload
	 *
	 *\param aBuffer A pointer to the buffer that you want to parse and send. (It will moved!)
	 *\param  aProtocolName A Type of the packet protocol
	 *\param  aFlags A combination of the send flag
	 *\param  aTo where to send
	 *\warning  aBuffer object will be empty if the buffer is sent to the kernel successfully
	*/
	int MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
			const NSHARE::uuid_t& aTo, eSendToFlags aFlags= E_NO_SEND_FLAGS);

	/*!\brief Publication(Send) the message by inner protocol (#RAW_PROTOCOL)
	 *
	 * It's the easiest way to publication(send) message. For this
	 * method of publication isn't required to develop the library
	 * for parsing the buffer. But it has some limitation:\n
	 * - in the raw protocol is not implemented auto swapping the byte order
	 * of messages;\n
	 * - for see data in the web GUI you has to be implemented method of
	 * serialize data to json;\n
	 * and of course, you cannot use inheritance.
	 *
	 *\param  aNumber A unique number of the message
	 *\param  aMsg A pointer to the message
	 *\param  aVersion A version of the message
	 *\param  aFlags A combination of the send flag
	 *
	 *\return >0 if the buffer is sent successfully. The value is equal the packet
	 *			 number of received_message_args_t::FPacketNumber \n
	 *		  0 if loopback mode (the data is sent to youself)\n
	 *		  <0 if error is occured (see bitwise error codes E_*)
	 *\warning  aMsg object will be empty if the buffer is sent to the kernel successfully
	 */
	int MSend(unsigned aNumber, NSHARE::CBuffer & aMsg,
			NSHARE::version_t const& aVersion= NSHARE::version_t(), eSendToFlags aFlags=
					E_NO_SEND_FLAGS);
	/*!\overload
	 *
	 * Send the message to the specified receiver
	 *
	 *\param  aNumber A unique number of the message
	 *\param  aTo where to send
	 *\param  aMsg A pointer to the message
	 *\param  aVersion A version of the message
	 *\param  aFlags A combination of the send flag
	 *\warning  aMsg object will be empty if the buffer is sent to the kernel successfully
	*/
	int MSend(unsigned aNumber, NSHARE::CBuffer & aMsg,
			const NSHARE::uuid_t& aTo, NSHARE::version_t const& aVersion=
					NSHARE::version_t(), eSendToFlags aFlags= E_NO_SEND_FLAGS);

	/*!\brief Publication(Send) the message of the special protocol
	 *
	 * It's balance between easy of use, performance and
	 * available features. For this method isn't required
	 * to develop the library of parsing the buffer if you
	 * don't want to use some features. But when you want
	 * to use the features for which you need to develop
	 * the library, you can do it without changing the existing
	 * source code.
	 * For which features are requirement the library you can see
	 * in NUDT::IExtParser.
	 *
	 *\param  aNumber A unique header of the message
	 *\param  aProtocolName A Type of the message protocol
	 *\param  aMsg A pointer to the message
	 *\param  aFlags A combination of the send flag
	 *
	 *\return >0 if the buffer is sent successfully. The value is equal the packet
	 *			 number of received_message_args_t::FPacketNumber\n
	 *		  0 if loopback mode (the data is sent to youself)\n
	 *		  <0 if error is occured (see bitwise error codes E_*)
	 *\warning It's the recommended way to send a message.
	 *\warning aMsg object will be empty if the buffer is sent to the kernel successfully
	 */
	int MSend(required_header_t const& aNumber, NSHARE::CText aProtocolName,
			NSHARE::CBuffer & aMsg, eSendToFlags aFlags= E_NO_SEND_FLAGS);

	/*!\overload
	 *
	 * Send the message to the specified receiver
	 *
	 *\param  aNumber A unique header of the message
	 *\param  aProtocolName A Type of the message protocol
	 *\param  aTo where to send
	 *\param  aMsg A pointer to the message
	 *\param  aFlags A combination of the send flag
	 *\warning  aBuffer object will be empty if the buffer is sent to the kernel successfully
	*/
	int MSend(required_header_t const& aNumber, NSHARE::CText aProtocolName,
			NSHARE::CBuffer & aMsg, const NSHARE::uuid_t& aTo, eSendToFlags aFlags=
					E_NO_SEND_FLAGS);

	/*!\brief Subscribe to the message (Registration the message handler)
	 *
	 * When the message will be received, the callback function
	 * is called.
	 *
	 *\param aMSGHeader what is requirement to receive and from
	 *\param aHandler A pointer to the function for handling message
	 *
	 *\return <0 if the error is occured \n
	 *			else unique handler ID (can be used in #MDoNotReceiveMSG)
	 *\see received_data_t
	 *\see CCustomer
	 *
	 */
	int MIWantReceivingMSG(const requirement_msg_info_t& aMSGHeader,
			const callback_t& aHandler);

	/*!\overload
	 *\param aFrom From whom to receive the message
	 *\param aNumber A what is requirement to receive
	 *\param aHandler A pointer to the function for handling message
	 *\param aVersion A requirement version of the message
	 *\param aFlags A flags for change subscription behavior (requirement_msg_info_t::eFLags)
	 */
	int MIWantReceivingMSG(const NSHARE::CText& aFrom, const unsigned& aNumber,
			const callback_t& aHandler, NSHARE::version_t const& aVersion=
					NSHARE::version_t(), requirement_msg_info_t::eFLags const& aFlags =
					requirement_msg_info_t::E_NO_FLAGS);

	/** Inform about subscribing to
	 *
	 *
	 * When somebody is subscribe to message  , the callback function
	 * is called.
	 *
	 * @param aProtocol - protocol name
	 * @param aMsg - message type
	 *\return <0 if the error is occured \n
	 *			else unique handler ID (can be used in #MDoesNotInformAboutSubscribe)
	 */
	int MInformAboutSubscribe(const NSHARE::CText& aProtocol,
				required_header_t const& aMsg,
				const callback_t& aHandler);

	/** Doesn't inform about subscribe
	 *
	 * @param aId handler ID (retrun value of #MInformAboutSubscribe or #MInformAboutUnSubscribe)
	 * @return true if removed
	 */
	bool MDoesNotInformAboutSubscribing(const NSHARE::CText& aProtocol,
			required_header_t const& aMsg,unsigned aId);

	/** Inform about subscribing to
	 *
	 *
	 * When somebody is subscribe to message  , the callback function
	 * is called.
	 *
	 * @param aProtocol - protocol name
	 * @param aMsg - message type
	 *\return <0 if the error is occured \n
	 *			else unique handler ID (can be used in #MDoesNotInformAboutUnSubscribe)
	 */
	int MInformAboutUnSubscribe(const NSHARE::CText& aProtocol,
				required_header_t const& aMsg,
				const callback_t& aHandler);


	/*!\brief Unsubscribe to the message (Remove the message handler)
	 *
	 *
	 *\param aFrom From whom to receive the message
	 *\param aNumber A what is requirement to receive
	 *
	 *\return <0 if the error is occured \n
	 *			else handler ID
	 *\see MIWantReceivingMSG
	 *
	 */
	int MDoNotReceiveMSG(const NSHARE::CText& aFrom, const unsigned& aNumber);

	/*!\overload
	 *
	 *\param aHandlerId Handler ID which was returned by #MIWantReceivingMSG function
	 *\param aTo If not null, then store request to aTo
	 *
	 *\return <0 if the error is occured \n
	 *			else handler ID
	 *			if aTo is not NULL then also return request info
	 *\see MIWantReceivingMSG
	 */
	int MDoNotReceiveMSG(unsigned aHandlerId, request_info_t* aTo =NULL);

	/*!\overload
	 *
	 *\param aMSGHeader what is requirement to receive and from
	 *\param aTo If not null, then store request to aTo
	 *\return <0 if the error is occured \n
	 *			else handler ID
	 *			if aTo is not NULL then also return request info
	 *\see MIWantReceivingMSG
	 */
	int MDoNotReceiveMSG(const requirement_msg_info_t& aMSGHeader,callback_t * aTo =NULL);

	/*!\brief Return information about all requested messages
	 *
	 *\return all requested messages
	 */
	std::vector<request_info_t> MGetMyWishForMSG() const;


	/*!\brief Add event handler
	 *
	 *\param aVal Event handler and type
	 *\return true if successfully
	 */
	bool operator+=(event_handler_info_t const & aVal);

	/*!\brief Remove event handler
	 *
	 *\param aVal Event handler and type which is removed
	 *\return true if successfully
	 */
	bool operator-=(event_handler_info_t const & aVal);

	/*!\brief Add event handler
	 *
	 *\param aVal Event handler and type
	 *\param aPrior Priority of event calling
	 *(Than the value is less by that the event is called earlier)
	 *\return true if successfully
	 */
	bool MAdd(event_handler_info_t const & aVal, unsigned int aPrior = std::numeric_limits<unsigned int>::max());


	/*!\brief Remove event handler
	 *
	 *\param aVal Event handler and type which is removed
	 *\return true if successfully
	 */
	bool MErase(event_handler_info_t const& aVal);

	/*!\brief Change priority of event handler
	 *
	 *\param aVal Event handler and type
	 *\param aPrior Priority of event calling
	 *(Than the value is less by that the event is called earlier)
	 *\return true if successfully
	 */
	bool MChangePrior(event_handler_info_t const&aVal, unsigned int aPrior);

	/*!\brief Test the event handler to see it's exist
	 *
	 *\param aVal Event handler and type
	 *\return true it's exist
	 */
	bool MIs(event_handler_info_t const& aVal) const;

	/*!\brief Test the key event to
	 * see there is at least handler of event
	 *
	 *\param aVal Event key
	 *\return true it's exist
	 */
	bool MIsKey(NSHARE::CText const& aVal) const;

	/*!\brief Print information about the events handler to stream
	 *
	 *\param aStream A output steam
	 *\return A output steam
	 */
	std::ostream& MPrintEvents(std::ostream & aStream) const;

	/*!\brief Return true if no event handler
	 *
	 *\return Return true if no event handler
	 */
	bool MEmpty  ()const;

	/*!\brief Allocate a new buffer(block of memory)
	 *
	 *The function allocates a buffer of size bytes.
	 *If shared memory is available then the buffer is
	 *allocated from it, else the buffer is allocated
	 *from the heap.
	 *
	 *\param aSize The number of bytes to allocate
	 *
	 *\return Return true if no event handler
	 */
	NSHARE::CBuffer MGetNewBuf(std::size_t aSize) const;

	/*!\brief The function blocks the calling thread
	 *until opened channel to kernel.
	 *
	 */
	void MJoin();

	/** Gets RTC for working
	 *
	 * @param aName Name of RTC
	 * @return pointer to RTC or NULL
	 * @warning Call this method as little as possible.
	 * 			It's looking for the specified RTC.
	 */
	IRtc* MGetRTC(NSHARE::CText const& aName) const;

	/** Gets or Create new RTC
	 *
	 * @param aName Id of RTC
	 * @param aType Type of RTC (for creating only, can be ignored by Kernel)
	 * @param aModuleName Name of RTC Module
	 * @return pointer to RTC or NULL if Kernel is disabled
	 * @warning blocking call!!!
	 */
	virtual IRtc* MGetOrCreateRTC(NSHARE::CText const& aName,
			unsigned const& aType,
			NSHARE::CText const& aModuleName = NSHARE::CText());

	/** Creates new RTC
	 *
	 * @param aName Id of RTC
	 * @param aType Type of RTC (for creating only, can be ignored by Kernel)
	 * @param aModuleName Name of RTC Module
	 * @return pointer to RTC or NULL if Kernel is disabled
	 * @warning blocking call!!!
	 */
	virtual IRtc* MCreateRTC(NSHARE::CText const& aName,
			unsigned const& aType,
			NSHARE::CText const& aModuleName = NSHARE::CText());

	/** Remove RTC if You is owner
	 *
	 * @param aName RTC name
	 * @return true if remove
	 */
	virtual bool MRemoveRTC(NSHARE::CText const& aName);

	/** Gets list of available RTC
	 *
	 * @param aModuleName Name of RTC Module
	 * @return list of RTC
	 */
	rtc_list_t MGetListOfRTC(NSHARE::CText const& aModuleName
			= NSHARE::CText()) const;

	/** Wait for RTC created
	 *
	 * @param aID RTC id
	 * @param aTime
	 * @return Pointer to RTC or NULL
	 */
	IRtc* MWaitForRTCCreated(NSHARE::CText const& aID,
			double aTime = -1) const;

	/** Force  stop to wait for RTC create
	 *
	 * @param aID RTC id
	 * @return true - is stopped
	 */
	bool MForceUnWaitForRTCCreated(NSHARE::CText const& aID) const;

	struct _pimpl;//!< Realization

	_pimpl& MGetImpl() const;//!< Return pointer to realization (for inner use)
private:

	CCustomer();

	~CCustomer();

	_pimpl* FImpl;
};

inline requirement_msg_info_t::requirement_msg_info_t(
		NSHARE::CText const& aProtocol, required_header_t const& aWhat,
		NSHARE::CText const& aFrom, unsigned aFlags):
				FProtocolName(aProtocol),//
				FRequired(aWhat),//
				FFrom(aFrom),//
				FFlags(aFlags)
{
	;
}
inline requirement_msg_info_t::requirement_msg_info_t()://
		FFlags(E_NO_FLAGS)
{

}
inline bool requirement_msg_info_t::operator==(requirement_msg_info_t const& aRht) const
{
	return FProtocolName==aRht.FProtocolName//
			&& FFlags==aRht.FFlags//
			&& FRequired==aRht.FRequired//
			&& FFrom==aRht.FFrom//
			;
}
inline received_data_t::received_data_t():
		FHeaderBegin(NULL),//
		FBegin(NULL),//
		FEnd(NULL)
{
	;
}
inline callback_t::callback_t() :
		FSignal(NULL), FYouData(NULL)
{
	;
}
inline callback_t::callback_t(signal_t const& aSignal):
		FSignal(aSignal),//
		FYouData(NULL)
{
	;
}
inline callback_t::callback_t(signal_t const& aSignal, void * const aData) :
		FSignal(aSignal), FYouData(aData)
{
	;
}
inline callback_t::callback_t(callback_t const& aCB) :
		FSignal(aCB.FSignal), FYouData(aCB.FYouData)
{
	;
}
inline callback_t& callback_t::operator=(callback_t const& aCB)
{
	FSignal = aCB.FSignal;
	FYouData = aCB.FYouData;
	return *this;
}
inline bool callback_t::MIs()const
{
	return FSignal!=NULL;
}
inline int callback_t::operator ()(CCustomer* aWho, void * const aArgs) const
{
	if (FSignal)
		return (*FSignal)(aWho, aArgs, FYouData);
	return -1;
}
inline bool callback_t::operator ==(callback_t const& rihgt) const
{
	return FSignal == rihgt.FSignal && FYouData == rihgt.FYouData;
}
inline event_handler_info_t::event_handler_info_t()
{

}
inline event_handler_info_t::event_handler_info_t (NSHARE::CText const& aKey,callback_t const& aCb):
	FKey(aKey),//
	FCb(aCb)
{

}
inline event_handler_info_t::event_handler_info_t(NSHARE::CText const& aKey,
		signal_t const& aSignal, void * const aData):
		FKey(aKey),//
		FCb(aSignal,aData)
{
	;
}
} //

#define ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(aType)\
		namespace NUDT{CUSTOMER_EXPORT NUDT:: aType deserialize_##aType(NSHARE::CConfig const& aConf);\
		CUSTOMER_EXPORT NSHARE::CConfig serialize_##aType(NUDT:: aType const& aObject);}\
		namespace NSHARE\
		{ template<> inline NUDT:: aType deserialize<NUDT:: aType>(NSHARE::CConfig const& aConf)\
			{return NUDT::deserialize_##aType(aConf);}\
			template<> inline NSHARE::CConfig serialize<NUDT:: aType>(NUDT:: aType const& aObject)\
			{ return NUDT::serialize_##aType(aObject);}\
		}\
	/*END*/

ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(requirement_msg_info_t)
ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(received_data_t)
ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(received_message_info_t)
ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(received_message_args_t)
ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(customers_updated_args_t)
ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(subcribe_receiver_args_t)
namespace NUDT
{
CUSTOMER_EXPORT NUDT::subcribe_receiver_args_t::what_t deserialize_subcribe_receiver_args_t_what_t(
		NSHARE::CConfig const& aConf);
CUSTOMER_EXPORT NSHARE::CConfig serialize_deserialize_subcribe_receiver_args_t_what_t(
		subcribe_receiver_args_t::what_t const& aObject);
}
namespace NSHARE
{
template<> inline NUDT::subcribe_receiver_args_t::what_t deserialize<
		NUDT::subcribe_receiver_args_t::what_t>(NSHARE::CConfig const& aConf)
{
	return NUDT::deserialize_subcribe_receiver_args_t_what_t(aConf);
}
template<> inline NSHARE::CConfig serialize<
		NUDT::subcribe_receiver_args_t::what_t>(
		NUDT::subcribe_receiver_args_t::what_t const& aObject)
{
	return NUDT::serialize_deserialize_subcribe_receiver_args_t_what_t(aObject);
}
}
ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(fail_sent_args_t)
ADD_CUSTOMER_SERIALIZE_DESERIALIZE_FOR(event_handler_info_t)

namespace std
{
CUSTOMER_EXPORT std::ostream& operator<<(std::ostream & aStream,
		NUDT::requirement_msg_info_t::eFLags const& aFlags);

inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::requirement_msg_info_t const& aMSG)
{
	using namespace NUDT;

	aStream<<"Name of program:  "<<aMSG.FFrom<<std::endl;
	aStream<<"Message protocol:  "<<aMSG.FProtocolName<<std::endl;
	aStream<<"Flags:  "<<(requirement_msg_info_t::eFLags)aMSG.FFlags<<std::endl;
	aStream<<"Message header:"<<std::endl;
	aStream<<aMSG.FRequired;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::received_data_t const& aData)
{
	using namespace NUDT;
	using namespace NSHARE;
	if(aData.FBuffer.empty())
		aStream<<"The buffer is empty"<<std::endl;
	else
	{
		const uint8_t* const _p_begin=(const uint8_t*)aData.FBuffer.ptr_const();

		aStream<<"Header index: "<<(aData.FHeaderBegin==NULL?0u:(aData.FHeaderBegin-_p_begin))<<std::endl;
		aStream<<"First message byte index: "<<(aData.FBegin==NULL?0u:(aData.FBegin-_p_begin))<<std::endl;
		aStream<<"Last message byte index: "<<(aData.FEnd==NULL?0u:(aData.FEnd-_p_begin))<<std::endl;
		aStream<<"Data :"<<std::endl;
		aStream<<aData.FBuffer;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::received_message_info_t const& aMSG)
{
	using namespace NUDT;

	aStream << "Sender:  " << aMSG.FFrom << std::endl;
	aStream << "The message protocol:  " << aMSG.FProtocolName << std::endl;
	aStream << "The packet number:" << aMSG.FPacketNumber << std::endl;

	if (!aMSG.FTo.empty())
	{
		received_message_info_t::uuids_t::const_iterator _it = aMSG.FTo.begin(),
				_it_end(aMSG.FTo.end());
		aStream << "Sent to:  ";
		for (; _it != _it_end; ++_it)
			aStream << (*_it) << " ";
		aStream << std::endl;
	}

	if (aMSG.FEndian != NSHARE::E_SHARE_ENDIAN)
		aStream << "The byte order is not valid" << std::endl;
	else
		aStream << "The byte order is valid" << std::endl;

	aStream << "Remain CB "<< aMSG.FRemainCallbacks<< std::endl;
	aStream << "Amount Of CB "<< aMSG.FCbs<< std::endl;

	aStream << "The message header:" << std::endl;

	aStream << aMSG.FHeader << std::endl;

	aStream << "The message data:" << std::endl;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::received_message_args_t const& aMSG)
{
	using namespace NUDT;

	aStream << static_cast<received_message_info_t const> (aMSG);
	aStream << aMSG.FMessage << std::endl;



	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::customers_updated_args_t const& aUpdate)
{
	using namespace NUDT;

	if(!aUpdate.FConnected.empty())
	{
		std::set<program_id_t>::const_iterator _it=aUpdate.FConnected.begin(),
				_it_end(aUpdate.FConnected.end());

		aStream << "New customers in UDT:";
		for (; _it != _it_end; ++_it)
		{
			aStream<< (*_it) <<std::endl;
		}
		aStream <<std::endl;
	}
	if(!aUpdate.FDisconnected.empty())
	{
		aStream << "Some customers droped:";
		std::set<program_id_t>::const_iterator _it =
				aUpdate.FDisconnected.begin(), _it_end(
						aUpdate.FDisconnected.end());
		for (; _it != _it_end; ++_it)
		{
			aStream << (*_it)<< std::endl;
		}
		aStream<< std::endl;
	}

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::subcribe_receiver_args_t::what_t const& aWhat)
{
	aStream <<"Requirement of " <<aWhat.FWhat << std::endl;
	aStream <<"From " <<aWhat.FWho << std::endl;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::subcribe_receiver_args_t const& aReceivers)
{
	using namespace NUDT;
	if(!aReceivers.FReceivers.empty())
	{
		subcribe_receiver_args_t::receivers_t::const_iterator
			_it = aReceivers.FReceivers.begin(),
			_it_end(aReceivers.FReceivers.end());

		for(;_it!=_it_end;++_it)
			aStream <<(*_it);
	}else
		aStream <<"No requirements"<<std::endl;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::fail_sent_args_t const& aMSG)
{
	using namespace NUDT;

	aStream << "Sender:  " << aMSG.FFrom << std::endl;
	aStream << "The message protocol:  " << aMSG.FProtocolName << std::endl;
	aStream << "The packet number:" << aMSG.FPacketNumber << std::endl;
	aStream << "Error:";

	CCustomer::sMPrintError(aStream, aMSG.FErrorCode);

	if(aMSG.FErrorCode!=0)
		aStream << "User error:"<<aMSG.FErrorCode<<std::endl;

	aStream << "The message header:" << std::endl;
	aStream << aMSG.FHeader << std::endl;

	if (!aMSG.FSentTo.empty())
	{
		received_message_args_t::uuids_t::const_iterator _it = aMSG.FSentTo.begin(),
				_it_end(aMSG.FSentTo.end());
		aStream << "Sent to:  ";
		for (; _it != _it_end; ++_it)
			aStream << (*_it) << " ";
		aStream << std::endl;
	}
	if (!aMSG.FFails.empty())
	{
		received_message_args_t::uuids_t::const_iterator _it = aMSG.FSentTo.begin(),
				_it_end(aMSG.FSentTo.end());
		aStream << "Don't delivered to:  ";
		for (; _it != _it_end; ++_it)
			aStream << (*_it) << " ";
		aStream << std::endl;
	}

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::callback_t const& aCb)
{
	aStream.setf(ios::hex, ios::basefield);
	aStream << "Pointer to data:" << aCb.FYouData << "; Pointer to cb handler: "
			<< aCb.FSignal;
	aStream.unsetf(ios::hex);
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::event_handler_info_t const& aCb)
{
	aStream <<"Key: " <<aCb.FKey << std::endl;
	aStream <<"Callback: " <<aCb.FCb << std::endl;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::request_info_t const& aRequest)
{
	aStream <<"What : " <<aRequest.FWhat << std::endl;
	aStream <<"Callback: " <<aRequest.FHandler << std::endl;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::CCustomer::eAPIError const& aError)
{
	using namespace NUDT;
	switch(aError)
	{
	case CCustomer::ERROR_CANNOT_READ_CONFIGURE:
		aStream << " Cannot read configure";
		break;

	case CCustomer::ERROR_CONFIGURE_IS_INVALID:
		aStream << " Configure is invalid";
		break;

	case CCustomer::ERROR_NO_NAME:
		aStream << " Name is not exist";
		break;

	case CCustomer::ERROR_NOT_OPEN:
		aStream << " Not Opened";
		break;

	case CCustomer::ERROR_NAME_IS_INVALID:
		aStream << " Name is invalid";
		break;

	case CCustomer::ERROR_NOT_CONNECTED_TO_KERNEL:
		aStream << " Not connected to kernel";
		break;

	case CCustomer::ERROR_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE:
		aStream << " Cannot allocate buffer of requirement size";
		break;

	case CCustomer::ERROR_UNEXPECETED:
		aStream << " unexpected error";
		break;

	case CCustomer::ERROR_HANDLER_IS_NOT_EXIST:
		aStream << " handler is not exist";
		break;

	default:
		aStream << " Unknown";
		break;
	};

	return aStream;
}
}
#endif /* CCUSTOMER_H_ */
