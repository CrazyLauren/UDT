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

#ifndef CCLIENT_H_
#define CCLIENT_H_


#include "customer_export.h"
#include <udt_types.h>

#ifdef uuid_t
#	error "Fucking programmer"

#endif
namespace NUDT
{
class CCustomer;

/*! \brief Information about requirement message
 *
 *\note
 * Non-POD type.
 */
struct requirement_msg_info_t
{
<<<<<<< HEAD
	NSHARE::CText FProtocolName; //Name of protocol
	required_header_t FRequired;
	enum eFLags{
		E_NO_FLAGS=0,
		E_REGISTRATOR=0x1<<0
	} FFlags;
=======
	/*! \brief Collection of bitwise flags for change
	 * subscription behavior
	 *
	 */
	enum eFLags
	{
		E_NO_FLAGS=0,
		E_REGISTRATOR=0x1<<0,//!< if it's set then message will be delivered to you
							 //!<only if there is at least one "non-registrator" (real)
							 //!<subscribed to the message
		E_INVERT_GROUP=0x1<<3,//<! if it's set then the order@com.ru.putin is not enter
							 //!<into the order@com.ru, but is enter into the order@com.ru.putin.vv
		E_NEAREST=0x1<<4,//<! if it's set then 	if there are next programs:
						 //!<order@com.ru.people,
						 //!<order@com.ru.putin.vv,
						 //!<order@com.ru.kremlin,
						 //!<than the order@com is included only order@com.ru.people
						 //!<and order@com.ru.kremlin
	};
>>>>>>> 3a2b21d... see changelog

	NSHARE::CText 		FProtocolName; //!<Type of message protocol
	required_header_t 	FRequired;//!< Header of requirement message
	unsigned 			FFlags;//!< subscription flags
	NSHARE::CText 		FFrom;//!< Name of program

	requirement_msg_info_t();
	bool operator==(requirement_msg_info_t const& aRht) const;
};

/*! \brief Storage of received data
 * which is contained the requirement message
 *
 *	The Pointers of type are pointed into FBuffer.
 *	The data from FHeaderBegin to FBegin is
 *	header. The message header can not exist or not equal of
 *	expected if:
 *	- it's message of child type;
 *	- the message header and data sent separately (see send API)
 *	(usually do that).
 *
 * \note
 *  FBuffer.end() can be not equal FEnd
 *  FBuffer.begin() can be not equal FBegin
 *  Non-POD type.
 */
struct received_data_t
{
	NSHARE::CBuffer FBuffer;//!< Contained received data

	const uint8_t* FHeaderBegin;//!< pointer to the message header
								//!<or NULL if message header is not exist
	const uint8_t* FBegin;//!<pointer to the message begin
	const uint8_t* FEnd;//!<pointer to end of message (equal std::vector::end())

	received_data_t();
};

/*! \brief information contained in aWHAT argument
 * of function which is handle received data ( see Receive API)
 *
 * You can send error to sender by
 * changing field FOccurUserError.
 *
 * \note
 *  Non-POD type.
 */
struct received_message_args_t
{
	typedef std::vector<NSHARE::uuid_t> uuids_t;

	NSHARE::uuid_t 		FFrom;//!<UUID of message sender
	NSHARE::CText 		FProtocolName;//!<Type of message protocol
	received_data_t 	FMessage;//!< Received message
	uint16_t 			FPacketNumber;//!<The packet (message) sequence number (continuous numbering
							//!<for all packets from sender)
							//!<two message can identical number only
							//!<if is sent in one packet (buffer), usually that two
							//!<messages in one buffer isn't sent (see send buffer method).
	required_header_t 	FHeader;//!< The message header
	uuids_t 			FTo;//!< List uuids of message receiver (sorted)
	mutable uint8_t 	FOccurUserError;//!<if the field will changed by you, when
										//!<the error with code  FOccurUserError
										//!<is sent to message sender (FFrom)
	unsigned 			FEndian;//!< Message byte order (see eEndian in endian_type.h)
};

/*! \brief Information about connected
 * (disconnected) program to kernel
 *
 * It's argument (aWHAT) of event: EVENT_CUSTOMERS_UPDATED
 *
 * \note
 *  Non-POD type.
 */
struct customers_updated_args_t
{
	std::set<program_id_t> FDisconnected;//!< list of connected program
	std::set<program_id_t> FConnected;//!< list of disconnected program
};
<<<<<<< HEAD
struct new_receiver_args_t
=======

/*! \brief Information about requirement message for
 *
 * It's argument (aWHAT) of event: EVENT_RECEIVER_SUBSCRIBE and
 * EVENT_RECEIVER_UNSUBSCRIBE
 *
 * \note
 *  Non-POD type.
 */
struct subcribe_receiver_args_t
>>>>>>> 3a2b21d... see changelog
{
	/*! \brief what requirement
	 *
	 */
	struct what_t
	{
		requirement_msg_info_t 	FWhat;//!< The request
		NSHARE::uuid_t 	FWho;//!< Who want to receive message
	};
	typedef std::vector<what_t> receivers_t;

	receivers_t FReceivers;//!< list of requirement messages
};

/*! \brief Information about not delivered message
 *
 */
struct fail_sent_args_t
{
	typedef uint32_t error_t;
	typedef std::vector<NSHARE::uuid_t> uuids_t;

	NSHARE::uuid_t 		FFrom;//!<see received_message_args_t
	NSHARE::CText 		FProtocolName;//!<see received_message_args_t
	uint16_t 			FPacketNumber;//!< see received_message_args_t

	required_header_t 	FHeader;//!< see received_message_args_t

	error_t 			FErrorCode;//!<A bitwise error code, see CCustomer structure
									//!< field E_*

	uint8_t 			FUserCode;//!<A user error or 0 (see FOccurUserError field of received_message_args_t)

	uuids_t 			FSentTo;//!< Where the data was sent
	uuids_t 			FFails;//!< Where the data was not delivered
};

/*! \brief type of callback function which used by customer
 *
 *	\param WHO - pointer to structure Customer
 *	\param WHAT - A pointer to a structure that describes
 *				 the event that caused the callback to be
 *				 invoked, or NULL if there isn't an event.
 *				 (see *_args_t structures)
 *	\param YOU_DATA -A pointer to data that you wanted to pass
 *					 as the second parameter(FYouData) callback_t structure.
 *
 *	\return by default Callback functions must return 0
 *			for detail see NSHARE::eCBRval
 */
typedef int (*signal_t)(CCustomer* WHO, void* WHAT, void* YOU_DATA);

/*! \brief Regular callback structure used in "Customer" structure
 *
 */
struct callback_t
{
	typedef signal_t TSignal;
	typedef TSignal pM;
	typedef void* arg_t;

	TSignal FSignal; //!< A pointer to the callback function
	void* 	FYouData;//!< A pointer to data that you
					//!<want to pass as the second parameter
					//!<to the callback function when it's invoked.

	callback_t();
	callback_t(TSignal const& aSignal, void * const aData);
	callback_t(callback_t const& aCB);
	callback_t& operator=(callback_t const& aCB);

	bool MIs() const;
	int operator ()(CCustomer* aWho, void * const aArgs) const;

	bool operator ==(callback_t const& rihgt) const;
};

/*! \brief Information about event handler
 *
 */
struct event_handler_info_t
{
	event_handler_info_t();
	event_handler_info_t (NSHARE::CText const& aKey,callback_t const& aCb);

	NSHARE::CText 	FKey;//!< A Name of Event
	callback_t 		FCb;//!<A callback function that will handle
					//!<the event when it occurs
};
/*! \brief Information about requested message and
 * it callback function
 *
 */
struct request_info_t
{
	requirement_msg_info_t 	FWhat;//!<A requested message
	callback_t 		FHandler;//!<A callback function
};

/*! \brief A main class used for communication
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
<<<<<<< HEAD
	static const NSHARE::CText DEFAULT_IO_MANAGER;
	static const NSHARE::CText RAW_PROTOCOL;
	static const NSHARE::CText ENV_CONFIG_PATH;
	static const NSHARE::CText CONFIG_PATH;
	static const NSHARE::CText MODULES;
	static const NSHARE::CText MODULES_PATH;
	static const NSHARE::CText DOING_MODULE;
	static const NSHARE::CText RRD_NAME;
	static const NSHARE::CText THREAD_PRIORITY;
	static const NSHARE::CText NUMBER_OF_THREAD;

	//events
	//for receive dg from fixed Custom, using its name as key
	static const NSHARE::CText EVENT_RAW_DATA;
	static const NSHARE::CText EVENT_CONNECTED;
	static const NSHARE::CText EVENT_DISCONNECTED;
	static const NSHARE::CText EVENT_CUSTOMERS_UPDATED;
	static const NSHARE::CText EVENT_FAILED_SEND;
	static const NSHARE::CText EVENT_NEW_RECEIVER;
	static const NSHARE::CText EVENT_READY;
=======
	static const NSHARE::CText ENV_CONFIG_PATH;//!< environment name used for specification
												//!<path to the config file

	/// \name Keys using in configure file
	/// \{
	static const NSHARE::CText MODULES;//!< A name of key for specification
										//!< list of the additional loaded libraries
	static const NSHARE::CText MODULES_PATH;//!< A name of key for specification
											//!< where looking for additional libraries
	static const NSHARE::CText DOING_MODULE;//!< A name of key in the config file for specification
											//!<used "manager" if not set is used manager from DEFAULT_IO_MANAGER
	static const NSHARE::CText DEFAULT_IO_MANAGER;//!< A name of default "manager"
	static const NSHARE::CText RRD_NAME;//!< A name of key  for specification
										//!<the program name in the system if not set used from parameter aName of sMinit
										//!<if it is not set too that used the real name of program
	static const NSHARE::CText THREAD_PRIORITY;//!< A name of key  for specification
												//!<a thread priority of event callback
	static const NSHARE::CText NUMBER_OF_THREAD;//!< A name of key for specification
												//!<amount of thread is used for event callback
	/// \}

	static const NSHARE::CText RAW_PROTOCOL;//!< A name of predefined protocol

	/// \name Keys of available events
	/// \{
	static const NSHARE::CText EVENT_RAW_DATA;//!< It's invoked when any data is received.
												//!<Each callback is passed  received_message_args_t structure
	static const NSHARE::CText EVENT_CONNECTED;//!< It's invoked when you connected to the kernel.
												//!<The callback is called without additional arguments.
	static const NSHARE::CText EVENT_DISCONNECTED;//!< It's invoked when you disconnected from the kernel.
												 //!<The callback is called without additional arguments.
	static const NSHARE::CText EVENT_CUSTOMERS_UPDATED;//!< It's invoked when some programs are connected
														//or disconnected from the kernel.
														//Each callback is passed  customers_updated_args_t structure.
	static const NSHARE::CText EVENT_FAILED_SEND;//!< It's invoked when the message from you cannot be delivered
												//!<to receiver (subscriber) or the message to you cannot be delivered
												//!< from sender (publisher).
												//!<Each callback is passed  fail_sent_args_t structure.
	static const NSHARE::CText EVENT_RECEIVER_SUBSCRIBE;//!< It's invoked when a receiver (subscriber)
														//!<wanted to receive the message from you (publisher).
														//!<Each callback is passed  subcribe_receiver_args_t structure.

	static const NSHARE::CText EVENT_RECEIVER_UNSUBSCRIBE;//!< It's invoked when a receiver (subscriber)
														//!<did not want to receive the message from you (publisher).
														//!<Each callback is passed  subcribe_receiver_args_t structure.
	/// \}
>>>>>>> bd5a830... before fixing

	typedef std::vector<NSHARE::CText> modules_t;
	typedef std::set<program_id_t> customers_t;

	/*! \brief Collection of bitwise flags for change
	 * sending behavior
	 *
	 */
	enum eSendToFlags
	{
		E_NO_SEND_FLAGS = 0
	};

	/// \name The bitwise error number constants.
	/// if some function return negative value less than -1.
	/// The return value is contained bitwise error code.
	/// For printing error calling sMPrintError Method
	/// \{
	typedef uint32_t error_t;
	//reserved
	static const error_t E_CANNOT_READ_CONFIGURE;//!< Cannot found (read) configuration file
	static const error_t E_CONFIGURE_IS_INVALID;//!< Some value in configuration file is not valid
	static const error_t E_NO_NAME;//!< The name of program is not set
	static const error_t E_NOT_OPEN;//!< The library is not "opened".
	static const error_t E_NAME_IS_INVALID;//!< The name of program is not valid
	static const error_t E_NOT_CONNECTED_TO_KERNEL;//!< The library is not connected to the kernel
	static const error_t E_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE;//!<Cannot allocate requirement size the buffer
																	  //!< in the shared memory (to fix it change the size of
																		//!< created by the kernel shared memory)
	static const error_t E_HANDLER_IS_NOT_EXIST;//!< Cannot find a callback function for handling a message.
												//!<Usually It occured during disconnecting of the message receiver.
	static const error_t E_NO_ROUTE;//!< Not route to receiver (subscriber).
									//!<Usually It occured during disconnect of the message receiver or problems with net.
	static const error_t E_UNKNOWN_ERROR;//!< Wtf?
	static const error_t E_PARSER_IS_NOT_EXIST;//!< The parser of the raw buffer for specified protocol
												//!< is not loaded (exist) to kernel.
												//!< For fix: load the parser of the raw buffer for specified protocol
												//!<to the kernel.
	static const error_t E_HANDLER_NO_MSG_OR_MORE_THAN_ONE;//!< No message in the buffer or the number of message is more than one (deprecated)
	static const error_t E_SOCKET_CLOSED;//!<The receiver has been disconnected yet
	static const error_t E_BUFFER_IS_FULL;//!<The block of memory which kernel has been allocated to receiver is full.
											//!<For fix error: change size of memory that the allocated to receiver or
											//!<increase the performance of the message receiver
	static const error_t E_PACKET_LOST;//!< The packet has been lost during delivering of message.
										//Sometimes it happens in the bad net
	static const error_t E_DATA_TOO_LARGE;//!< In the kernel can be set the max message size which is transmitted by net (line)
										  //!<The error is occured if sender is exceed the limit and the packet division mode of
											//!<the kernel is not set.
	static const error_t E_MERGE_ERROR;//!< The error is occured when if in the packet division mode of the kernel, it cannot
										//!<merge the splitted message.
	static const error_t E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE;//!< The version of the sent message and the requirement
																//!<message is not compatible
	static const error_t E_USER_ERROR_EXIST;//!<If user's error is set ( see FOccurUserError of received_message_args_t)
											//!<The error is occured
	static const unsigned MAX_SIZE_USER_ERROR;//!< The amount of bits are used for save user's error


	/*! \brief Printing bitwise error code to stream
	 *
	 *	\param aStream where to print
	 *	\param aError error bitwise code
	 *	\return aStream
	 */
	static std::ostream& sMPrintError(std::ostream& aStream,error_t const& aError);
	/// \}

	/*! \brief Initialize library
	 *
	 *	Initialize the library and create singleton for this type
	 *
	 *	\param argc The number of entries in the argv array
	 *	\param argv An array of pointers to strings that contain the arguments to the program
	 *	\param aName The desired name of program
	 *	\param aVersion Version of program
	 *	\param aConfPath Path to the configuration file if it not set
	 *					The file is read from environment specificated in ENV_CONFIG_PATH.
	 *	\return 0 - EOK
	 *			else bitwise error code
	 */
	static int sMInit(int argc, char const* argv[], char const* aName,NSHARE::version_t const&  aVersion=NSHARE::version_t(),const NSHARE::CText& aConfPath="");

<<<<<<< HEAD
		}
		NSHARE::CText FKey;
		callback_t FCb;
	};
	///@brief initialization library
	///@param argc - The number of entries in the argv array
	///@param argv - An array of pointers to strings that contain the arguments to the program
	///@param aName - Name of Customer
	///@param aConf - Config
	static int sMInit(int argc, char* argv[], char const* aName,NSHARE::version_t const& =NSHARE::version_t(),const NSHARE::CText& aConfPath="");
	static int sMInit(int argc, char* argv[], char const* aName,NSHARE::version_t const& aProgrammVersion,	const NSHARE::CConfig& aConf);
=======
	/*! \brief Initialize library
	 *
	 *	Initialize the library and create singleton for this type
	 *
	 *	\param argc The number of entries in the argv array
	 *	\param argv An array of pointers to strings that contain the arguments to the program
	 *	\param aName The desired name of program
	 *	\param aVersion Version of program
	 *	\param aConfPath configuration
	 *	\return 0 - EOK
	 *			else bitwise error code
	 */
	static int sMInit(int argc, char const* argv[], char const* aName,NSHARE::version_t const& aProgrammVersion,	const NSHARE::CConfig& aConf);
>>>>>>> bd5a830... before fixing

	/*! \brief deinitialization library
	 *
	 *	Remove sigelton and free all used resources.
	 */
	static void sMFree();

	/*! \brief Get information about library version
	 *
	 *	\return Library version
	 */
	static const NSHARE::version_t& sMVersion();

	/*! \brief Check for available module
	 *
	 *	\param aModule Name of module
	 *	\return true if exist
	 */
	bool MAvailable(const NSHARE::CText& aModule) const;

	/*! \brief Check for available used module (DOING_MODULE)
	 *
	 *	\return true if exist
	 */
	bool MAvailable() const;

	/*! \brief Get list of available modules
	 *
	 *	\return modules
	 */
	modules_t MModules() const;

	/* \brief Check if you connect to kernel
	 *
	 *	\return true if connected
	 */
	bool MIsConnected() const;

	/* \brief Check if you create channel to library
	 *
	 *	\return true if created
	 */
	bool MIsOpened() const;

	/* \brief Open channel to kernel
	 *
	 *	After the channel to kernel is opened, the
	 *	connection to kernel begins. When the connection
	 *	is established the event EVENT_CONNECTED is occured.
	 *
	 *	\return true if opened successfully
	 */
	bool MOpen();

	/* \brief Close channel to kernel
	 *
	 */
	void MClose();

	/*! \brief wait for some event is occured
	 *
	 *	\param aEvemt - waited for event
	 *	\param aSec - time out if value < 0 than wait for infinitely
	 *	\warning Non-recommended to use the method for any events
	 *	with the exception of EVENT_CONNECTED, EVENT_DISCONNECTED
	 *
	 *	\return -1 if error occured
	 */
	int MWaitForEvent(NSHARE::CText const& aEvent,double aSec=-1);

	///@brief Return current ID
	const program_id_t& MGetID() const;
	///@brief Return all registered ID
	customers_t MCustomers() const;
	program_id_t MCustomer(NSHARE::uuid_t const& aUUID) const;


	///@brief Send data to customer
	///@param aProtocolName The fixed protocol name that refers to the  sending buffer. there is default Protocol - raw
	///@param aNumber The number off sending buffer.
	///If you don't want to create own protocol, using send with aNumber argument. The protocol will  created automatically as "raw"
	///protocol with number aNumber
	///@param aBuffer Pointer to the data
	///@param aSize size of data
	///@param aTo Name of customer
	///@return  <0 if error, 0 -if loopback, else ID of sent packet
	int MSend(NSHARE::CText aProtocolName, void* aBuffer, size_t aSize, eSendToFlags = E_NO_SEND_FLAGS);
	int MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer, eSendToFlags = E_NO_SEND_FLAGS);

	int MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
			const NSHARE::uuid_t& aTo, eSendToFlags = E_NO_SEND_FLAGS);
	int MSend(NSHARE::CText aProtocolName, void* aBuffer, size_t aSize,
			const NSHARE::uuid_t& aTo, eSendToFlags = E_NO_SEND_FLAGS);

<<<<<<< HEAD
	int MSend(unsigned aNumber, NSHARE::CBuffer & aBuffer,
=======
	///@brief Send message to customer
	///@param aNumber The number off sending buffer.
	///@param aPacket Pointer to the packet
	///@param aTo Name of customer
	///@return  <0 if error, 0 -if loopback, else ID of sent packet
	int MSend(unsigned aNumber, NSHARE::CBuffer & aPacket,
>>>>>>> 3a2b21d... see changelog
			NSHARE::version_t const& = NSHARE::version_t(), eSendToFlags =
					E_NO_SEND_FLAGS);
	int MSend(unsigned aNumber, NSHARE::CBuffer & aBuffer,
			const NSHARE::uuid_t& aTo, NSHARE::version_t const& =
					NSHARE::version_t(), eSendToFlags = E_NO_SEND_FLAGS);

	///@brief Add a parser callback entry to a handle list
	///@param aFrom Name of Parsing  customer
	///@param aHeader Parsing a header type
	///@param aCB Callback handler
	///@return  <0 if error, else handler ID
	int MIWantReceivingMSG(const requirement_msg_info_t& aMSGHeader,
			const callback_t& aHandler);
	int MIWantReceivingMSG(const NSHARE::CText& aFrom, const unsigned& aHeader,
			const callback_t& aCB, NSHARE::version_t const& =
					NSHARE::version_t(), requirement_msg_info_t::eFLags const& =
					requirement_msg_info_t::E_NO_FLAGS);

	int MDoNotReceiveMSG(const NSHARE::CText& aFrom,
			const unsigned& aNumber);
	int MDoNotReceiveMSG(const requirement_msg_info_t& aNumber);

	std::vector<request_info_t> MGetMyWishForMSG() const;


	bool operator+=(event_handler_info_t const & aVal);
	bool operator-=(event_handler_info_t const & aVal);
	bool MAdd(event_handler_info_t const & aVal, unsigned int aPrior = std::numeric_limits<unsigned int>::max());
	bool MErase(event_handler_info_t const& aVal);

	bool MChangePrior(event_handler_info_t const&aVal, unsigned int aPrior);
	bool MIs(event_handler_info_t const& aVal) const;
	bool MIsKey(NSHARE::CText const& aVal) const;
	std::ostream& MPrintEvents(std::ostream & aStream) const;

	bool MEmpty  ()const;
	NSHARE::CBuffer MGetNewBuf(std::size_t aSize) const;


	void MJoin();
private:

	CCustomer();
	int MInitialize(NSHARE::CText const& aProgram, NSHARE::CText const& aName,NSHARE::version_t const&);

	~CCustomer();

	struct _pimpl;
	_pimpl* FImpl;
};

inline requirement_msg_info_t::requirement_msg_info_t():FFlags(E_NO_FLAGS)
{

}
inline bool requirement_msg_info_t::operator==(requirement_msg_info_t const& aRht) const
{
	return FProtocolName==aRht.FProtocolName//
			&& FFlags==aRht.FFlags//
			&& FRequired==aRht.FRequired//
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
inline callback_t::callback_t(TSignal const& aSignal, void * const aData) :
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
	FKey(aKey),FCb(aCb)
{

}
} //
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
		NUDT::callback_t const& aCb)
{
	aStream.setf(ios::hex, ios::basefield);
	aStream << "Pointer to data:" << aCb.FYouData << "; Pointer to cb handler: "
			<< aCb.FSignal;
	aStream.unsetf(ios::hex);
	return aStream;
}
}
#endif /* CCLIENT_H_ */
