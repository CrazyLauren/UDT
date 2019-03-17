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
struct msg_parser_t
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
	 * subscription behaviour
	 *
	 */
	enum eFLags
	{
		E_NO_FLAGS=0,
		E_REGISTRATOR=0x1<<0,//!< if it's set then message will be delivered to you
							 //only if there is at least one "non-registrator" (real)
							 //subscribed to the message
		E_INVERT_GROUP=0x1<<3,//<! if it's set then the order@com.ru.putin is not enter
							 //into the order@com.ru, but is enter into the order@com.ru.putin.vv
		E_NEAREST=0x1<<4,//<! if it's set then 	if there are next programs:
						 //order@com.ru.people,
						 //order@com.ru.putin.vv,
						 //order@com.ru.kremlin,
						 //than the order@com is included only order@com.ru.people
						 //and order@com.ru.kremlin
	};
>>>>>>> 3a2b21d... see changelog

	NSHARE::CText FProtocolName; //!<Type of message protocol
	required_header_t FRequired;//!< Header of requirement message
	unsigned FFlags;//!< subscription flags
	NSHARE::CText FFrom;//!< Name of program

	msg_parser_t();
	bool operator==(msg_parser_t const& aRht) const;
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
								//or NULL if message header is not exist
	const uint8_t* FBegin;//pointer to the message begin
	const uint8_t* FEnd;//pointer to end of message (equal std::vector::end())

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
	NSHARE::uuid_t FFrom;//!<UUID of message sender
	NSHARE::CText FProtocolName;//!<Type of message protocol
	received_data_t FMessage;//!< Received message
	uint16_t FPacketNumber;//!<The packet (message) sequence number (continuous numbering
							//for all packets from sender)
							//two message can identical number only
							//if is sent in one packet (buffer), usually that two
							//messages in one buffer isn't sent (see send buffer method).
	required_header_t FHeader;//!< The message header
	std::vector<NSHARE::uuid_t> FTo;//!< List uuids of message receiver (sorted)
	mutable uint8_t FOccurUserError;//if the field will changed by you, when
	//the error with code  FOccurUserError is sent to message sender (FFrom)
	unsigned FEndian;// Message byte order (see eEndian in endian_type.h)
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
	struct what_t
	{
		msg_parser_t FWhat;//!< The request
		NSHARE::uuid_t FWho;//!< Who want to receive message
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

	NSHARE::uuid_t FFrom;//!<see received_message_args_t
	NSHARE::CText FProtocolName;//!<see received_message_args_t
	uint16_t FPacketNumber;//!< see received_message_args_t

	required_header_t FHeader;//!< see received_message_args_t

	error_t FErrorCode;//!<A bitwise error code, see CCustomer structure
					// field E_*

	uint8_t FUserCode;//!<A user error or 0 (see FOccurUserError field of received_message_args_t)

	std::vector<NSHARE::uuid_t> FSentTo;//!< Where the data was sent
	std::vector<NSHARE::uuid_t> FFails;//!< Where the data was not delivered
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
	void* FYouData;//!< A pointer to data that you
					//want to pass as the second parameter
					//to the callback function when it's invoked.

	callback_t();
	callback_t(TSignal const& aSignal, void * const aData);
	callback_t(callback_t const& aCB);
	callback_t& operator=(callback_t const& aCB);

	bool MIs() const;
	int operator ()(CCustomer* aWho, void * const aArgs) const;

	bool operator ==(callback_t const& rihgt) const;
};

/*! \brief Information about requested message and
 * it callback function
 *
 */
struct request_info_t
{
	msg_parser_t FWhat;//!<A requested message
	callback_t FHandler;//!<A callback function
};
class CUSTOMER_EXPORT CCustomer: public NSHARE::CSingleton<CCustomer>
{
public:
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

	typedef std::vector<NSHARE::CText> modules_t;
	typedef std::set<program_id_t> customers_t;

	enum eSendToFlags
	{
		E_NO_SEND_FLAGS = 0
	};
	enum eRecvFromFlags
	{
		E_NO_RECV_FLAGS = 0
	};

	//all error less zero and bitwise
	typedef uint32_t error_t;
	//error's
	//reserved
	static const error_t E_CANNOT_READ_CONFIGURE;//<<2
	static const error_t E_CONFIGURE_IS_INVALID;//<<3
	static const error_t E_NO_NAME;
	static const error_t E_NOT_OPEN;
	static const error_t E_NAME_IS_INVALID;
	static const error_t E_NOT_CONNECTED_TO_KERNEL;
	static const error_t E_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE;

	//reserved;
	static const error_t E_HANDLER_IS_NOT_EXIST;
	static const error_t E_NO_ROUTE;
	static const error_t E_UNKNOWN_ERROR;
	static const error_t E_PARSER_IS_NOT_EXIST;
	static const error_t E_HANDLER_NO_MSG_OR_MORE_THAN_ONE;
	static const error_t E_SOCKET_CLOSED;
	static const error_t E_BUFFER_IS_FULL;
	static const error_t E_PACKET_LOST;
	static const error_t E_DATA_TOO_LARGE;
	static const error_t E_MERGE_ERROR;
	static const error_t E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE;
	static std::ostream& sMPrintError(std::ostream&,error_t const&);
	//reserve

	static const error_t E_USER_ERROR_EXIST;//if user's error is exit then the bit is set
	static const unsigned FIRST_USER_ERROR_BIT;	//Number of the first bit of user's error
	//then 8 bit for user's code

	enum
	{
		E_KERNEL_IS_NOT_ANSWER		//deprecated
	};

	struct value_t
	{
		value_t()
		{

		}
		value_t (NSHARE::CText const& aKey,callback_t const& aCb):
			FKey(aKey),FCb(aCb)
		{

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

	///@brief free library
	static void sMFree();
	static const NSHARE::version_t& sMVersion();

	///@brief available modules in core
	///@param aModule Name of module
	bool MAvailable(const NSHARE::CText& aModule) const;
	///@brief if available default module
	bool MAvailable() const;
	///@brief getting all module at what the core is available
	modules_t MModules() const;

	bool MIsConnected() const;
	bool MIsOpened() const;

	///@brief Open channel
	///@return  true if opened successfully
	bool MOpen();
	///@brief close channel
	void MClose();

	///@brief waiting for ready to work
	void MWaitForEvent(NSHARE::CText const& aEvent,double aSec=-1);

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
	int MIWantReceivingMSG(const msg_parser_t& aMSGHeader,
			const callback_t& aHandler);
	int MIWantReceivingMSG(const NSHARE::CText& aFrom, const unsigned& aHeader,
			const callback_t& aCB, NSHARE::version_t const& =
					NSHARE::version_t(), msg_parser_t::eFLags const& =
					msg_parser_t::E_NO_FLAGS);

	int MDoNotReceiveMSG(const NSHARE::CText& aFrom,
			const unsigned& aNumber);
	int MDoNotReceiveMSG(const msg_parser_t& aNumber);

	std::vector<request_info_t> MGetMyWishForMSG() const;


	bool operator+=(value_t const & aVal);
	bool operator-=(value_t const & aVal);
	bool MAdd(value_t const & aVal, unsigned int aPrior = std::numeric_limits<unsigned int>::max());
	bool MErase(value_t const& aVal);

	bool MChangePrior(value_t const&aVal, unsigned int aPrior);
	bool MIs(value_t const& aVal) const;
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

inline msg_parser_t::msg_parser_t():FFlags(E_NO_FLAGS)
{

}
inline bool msg_parser_t::operator==(msg_parser_t const& aRht) const
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
} //
namespace std
{
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
