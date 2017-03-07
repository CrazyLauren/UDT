/*
 * CCustomer.h
 *
 *  Created on: 19.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCLIENT_H_
#define CCLIENT_H_


#include "customer_export.h"
#include <udt_types.h>

#ifdef uuid_t
#error "Fucking programmer"
#endif
namespace NUDT
{
class CCustomer;
struct msg_parser_t
{
	NSHARE::CText FProtocolName; //Name of protocol
	required_header_t FRequired;
	enum eFLags{
		E_NO_FLAGS=0,
		E_REGISTRATOR=0x1<<0
	} FFlags;

	msg_parser_t():FFlags(E_NO_FLAGS)
	{

	}
};
struct args_t
{
	NSHARE::uuid_t FFrom;
	NSHARE::CText FProtocolName;
	const uint8_t* FBegin;//pointer to the data
	const uint8_t* FEnd;//equal vector::end()
	NSHARE::CBuffer FBuffer;//If buffer is exist The fields FBegin,FEnd
							//are equal to FBuffer.begin(),FBuffer.end().
	unsigned FPacketNumber;
	unsigned FRawProtocolNumber;//If using Raw protocol The field is the number of packet
	std::vector<NSHARE::uuid_t> FTo;
	NSHARE::version_t FVersion;
};

struct customers_updated_args_t
{
	std::set<program_id_t> FDisconnected;
	std::set<program_id_t> FConnected;
};
struct new_receiver_args_t
{
	struct what_t
	{
		msg_parser_t FWhat;
		NSHARE::uuid_t FWho;
		NSHARE::CText FRegExp;//see the first argument of MSettingDgParserFor
	};
	typedef std::vector<what_t> receivers_t;

	receivers_t FReceivers;
};
typedef int (*signal_t)(CCustomer* WHO, void* WHAT, void* YOU_DATA);
struct callback_t
{
	typedef signal_t TSignal ;
	typedef TSignal pM;
	typedef void* arg_t;
	callback_t() :
			FSignal(NULL), FYouData(NULL)
	{
		;
	}
	callback_t(TSignal const& aSignal, void * const aData) :
			FSignal(aSignal), FYouData(aData)
	{
		;
	}
	callback_t(callback_t const& aCB) :
			FSignal(aCB.FSignal), FYouData(aCB.FYouData)
	{
		;
	}
	callback_t& operator=(callback_t const& aCB)
	{
		FSignal=aCB.FSignal;
		FYouData=aCB.FYouData;
		return *this;
	}

	TSignal FSignal;
	void* FYouData;
	bool MIs()const
	{
		return FSignal!=NULL;
	}
	int operator ()(CCustomer* aWho, void * const aArgs) const
	{
		if (FSignal)
			return (*FSignal)(aWho, aArgs, FYouData);
		return -1;
	}

	bool operator ==(callback_t const& rihgt) const
	{
		return FSignal == rihgt.FSignal && FYouData == rihgt.FYouData;
	}
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
	static const error_t E_MERGE_ERROR;
	static const error_t E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE;
	static std::ostream& sMPrintError(std::ostream&,error_t const&);
	//reserve

	static const error_t E_USER_ERROR_EXIT;//if user's error is exit then the bit is set
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

	int MSend(unsigned aNumber, NSHARE::CBuffer & aBuffer,
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
	int MIWantReceivingMSG(const NSHARE::CText& aFrom,
			const unsigned& aMSGNumber, const callback_t& aHandler,
			msg_parser_t::eFLags const& = msg_parser_t::E_NO_FLAGS,
			NSHARE::version_t const& = NSHARE::version_t());
	int MIWantReceivingMSG(const NSHARE::CText& aFrom,
			const msg_parser_t& aMSGHeader, const callback_t& aHandler);

	int MDoNotReceiveMSG(const NSHARE::CText& aFrom,
			const unsigned& aNumber);
	int MDoNotReceiveMSG(const NSHARE::CText& aFrom,
			const msg_parser_t& aNumber);



	bool operator+=(value_t const & aVal);
	bool operator-=(value_t const & aVal);
	bool MAdd(value_t const & aVal, unsigned int aPrior = std::numeric_limits<unsigned int>::max());
	bool MErase(value_t const& aVal);

	bool MChangePrior(value_t const&aVal, unsigned int aPrior);
	bool MIs(value_t const& aVal) const;
	bool MIsKey(NSHARE::CText const& aVal) const;
	std::ostream& MPrintEvents(std::ostream & aStream) const;

	bool MEmpty  ()const;
	NSHARE::CBuffer MGetNewBuf(unsigned aSize) const;

	int MSettingDgParserFor(const NSHARE::CText& aFrom,
			const msg_parser_t& aHeader, const callback_t& aCB);
	int MRemoveDgParserFor(const NSHARE::CText& aFrom,
			const msg_parser_t& aNumber);

	int MSettingDgParserFor(const NSHARE::CText& aFrom,
			const unsigned& aHeader, const callback_t& aCB,NSHARE::version_t const& =NSHARE::version_t(),msg_parser_t::eFLags const& =msg_parser_t::E_NO_FLAGS);
	int MRemoveDgParserFor(const NSHARE::CText& aFrom,
			const unsigned& aNumber);
	void MJoin();
private:

	CCustomer();
	int MInitialize(NSHARE::CText const& aProgram, NSHARE::CText const& aName,NSHARE::version_t const&);

	~CCustomer();

	struct _pimpl;
	_pimpl* FImpl;
};
struct fail_sent_args_t
{
	NSHARE::uuid_t FFrom;
	NSHARE::CText FProtocolName;
	unsigned FPacketNumber;
	unsigned FRawProtocolNumber;//If using Raw protocol The field is the number of packet
	NSHARE::version_t FVersion;

	CCustomer::error_t FErrorCode;//see CCustomer
	uint8_t FUserCode;
	std::vector<NSHARE::uuid_t> FTo;
	std::vector<NSHARE::uuid_t> FFails;
};
inline int CCustomer::MIWantReceivingMSG(const NSHARE::CText& aFrom,
		const msg_parser_t& aHeader, const callback_t& aCB){
	return MSettingDgParserFor(aFrom,aHeader,aCB);
}
inline int CCustomer::MDoNotReceiveMSG(const NSHARE::CText& aFrom,
		const msg_parser_t& aNumber){
	return MRemoveDgParserFor(aFrom,aNumber);
}

inline int CCustomer::MIWantReceivingMSG(const NSHARE::CText& aFrom,
		const unsigned& aHeader, const callback_t& aCB,msg_parser_t::eFLags const& aFlags,NSHARE::version_t const& aVal){
	return MSettingDgParserFor(aFrom,aHeader,aCB,aVal,aFlags);
}
inline int CCustomer::MDoNotReceiveMSG(const NSHARE::CText& aFrom,
		const unsigned& aNumber){
	return MRemoveDgParserFor(aFrom,aNumber);
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
