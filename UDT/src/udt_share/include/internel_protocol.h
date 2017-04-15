/*
 * internel_protocol.h
 *
 *  Created on: 19.11.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef INTERNEL_PROTOCOL_H_
#define INTERNEL_PROTOCOL_H_
#include <crc8.h>
#include <crc16.h>
#include <time.h>
#include <programm_id.h>
#ifndef SHARED_PACKED
#include <macro_attributes.h>
#endif
namespace NUDT
{
enum  eMsgType
{
	E_PROTOCOL_MSG = 0,//
	E_REQUEST_INFO = 1,//
	E_INFO = 2,//
	E_KERNEL_INFO = 3,//kd from kernel to krenel
	E_ALL_CLIENTS = 4,//kd from kernel to cusomers
	E_ACCEPTED=5,

	//10-20 reserved for main channel
	E_MAIN_CHANNEL_PARAM = 10,//
	E_REQUEST_MAIN_CHANNEL_PARAM = 11,//
	E_CLOSE_MAIN_CHANNEL = 12,//
	E_MAIN_CHANNEL_ERROR = 13,//
	//20
	E_USER_DATA = 21,//
	E_CUSTOMER_FILTERS = 22,//
	E_USER_DATA_RECEIVED = 23,//deprecated
	E_USER_DATA_FAIL_SEND = 24,//
	E_CUSTOMERS_DEMANDS = 25,//
	//E_FAIL_SEND = 26,//
};
//enum eErrorCode
//{
//	E_NO_ERROR = 0,
//	E_SENT_ERROR = 1,
//};

namespace _impl
{
	template<uint8_t b>
	struct msb2lsb
	{
		enum
		{
			b1=(b&0xF0)>>4|(b&0x0F)<<4,
			b2=(b1&0xCC)>>2|(b1&0x33)<<2,
			result=(b2&0xAA)>>1|(b2&0x55)<<1
		};
	};
};
//32 byte
typedef NSHARE::crc8_t<0x97,0x01> udt_protocol_header_crc_t;
typedef NSHARE::crc16_t<0xC005, 0> udt_protocol_data_crc_t;
SHARED_PACKED(
struct head_t
{
	typedef udt_protocol_header_crc_t crc_head_t;
	typedef udt_protocol_data_crc_t crc_data_t;

	enum eBOM
	{
		E_HEAD_SIZE=8*4,
		E_ORDER_CHECK=_impl::msb2lsb<E_HEAD_SIZE>::result
	};
	enum eHeadFlags
	{
		E_ROUTE_OF_MSG = 0x1<<1,
		E_ERROR_OF_MSG= 0x1<<2,
	};
	//1-st word

	const uint8_t FHeadSize;
	const uint8_t FEndianness;
protected:
	uint16_t FTimeMs;//ms (for debug only)
public:
	//2-d word
	NSHARE::version_t const FVersion;//revision isn't used. It's reserv
protected:
	//3-d word
	uint32_t FDataSize;

	//4-d word
	uint32_t FTime;//for debug only

	//5-d,6-d word
	uint64_t FFromUUID;//can be problem with size(for debug only)

	//7-d word
	uint16_t FDataCrc;
public:
	uint8_t FType;//eMsgType
	uint8_t FFlags;//eHeadFlags
	//8-d word
protected:
	uint16_t FCounter;//for debug only
public:
	uint8_t :8;
	uint8_t FCrcHead;

	//fix //fucking msvc error c2503
	//uint8_t FDataBegin[0];
	inline uint8_t* MDataBegin()const{
		return (uint8_t*)(this+1);
	}
	inline bool MIs(eHeadFlags ) const;
	inline void MSet(unsigned ,bool aVal );
	inline bool MIsValidEndian() const;
	inline bool MIsValidBitEndian() const;

	inline uint32_t MGetDataSize() const;
	inline uint32_t MGetTime() const;
	inline uint64_t MGetFromUUID() const;
	inline uint16_t MGetCounter() const;
	inline uint16_t MGetTimeMs() const;
	inline uint16_t MGetDataCRC() const;

	inline void  MSetDataSize(uint32_t);
	inline void  MSetTime(uint32_t);
	inline void  MSetFromUUID(uint64_t);
	inline void  MSetCounter(uint16_t);
	inline void  MSetTimeMs(uint16_t);
	inline void  MSetDataCRC(uint16_t);

	template<class T> T MEndianCorrectValue(T aVal) const;
	head_t(uint8_t aMajor,uint8_t aMinor,eMsgType aType);
});
inline head_t::head_t(uint8_t aMajor, uint8_t aMinor, eMsgType aType) :
	FHeadSize(E_HEAD_SIZE),//
	FTimeMs(0),//
	FEndianness(NSHARE::E_SHARE_ENDIAN),//
	FVersion(aMajor, aMinor),//
	FDataSize(0),//
	FTime(0),//
	FFromUUID(0),//
	FDataCrc(0),//
	FType(aType),//
	FFlags(0),//
	FCounter(0),//
	FCrcHead(0)
{
}
inline bool head_t::MIs(eHeadFlags aFlag) const
{
	return (FFlags & aFlag)!=0;
}
inline void head_t::MSet(unsigned aFlag, bool aVal)
{
	FFlags = (aVal) ? (FFlags | aFlag) : (FFlags & (~aFlag));
}
template<class T>
inline T head_t::MEndianCorrectValue(T aVal) const
{
	return MIsValidEndian() ?aVal:NSHARE::swap_endian(aVal);
}
inline bool head_t::MIsValidBitEndian() const {
	return FHeadSize!= E_ORDER_CHECK;
}
template<>
inline uint8_t head_t::MEndianCorrectValue<uint8_t>(uint8_t aVal) const
{
	return aVal;
}
template<>
inline int8_t head_t::MEndianCorrectValue<int8_t>(int8_t aVal) const
{
	return aVal;
}
inline bool head_t::MIsValidEndian() const
{
	return FEndianness == NSHARE::E_SHARE_ENDIAN;
}
inline uint32_t head_t::MGetDataSize() const
{
	return MEndianCorrectValue(FDataSize);
}
inline uint32_t head_t::MGetTime() const
{
	return MEndianCorrectValue(FTime);
}
inline uint64_t head_t::MGetFromUUID() const
{
	return MEndianCorrectValue(FFromUUID);
}
inline uint16_t head_t::MGetCounter() const
{
	return MEndianCorrectValue(FCounter);
}
inline uint16_t head_t::MGetTimeMs() const
{
	return MEndianCorrectValue(FTimeMs);
}
inline uint16_t head_t::MGetDataCRC() const
{
	return MEndianCorrectValue(FDataCrc);
}
inline void  head_t::MSetDataSize(uint32_t aVal){
	FDataSize=aVal;
}
inline void  head_t::MSetTime(uint32_t aVal){
	FTime=aVal;
}
inline void  head_t::MSetFromUUID(uint64_t aVal){
	FFromUUID=aVal;
}
inline void  head_t::MSetCounter(uint16_t aVal){
	FCounter=aVal;
}
inline void  head_t::MSetTimeMs(uint16_t aVal){
	FTimeMs=aVal;
}
inline void  head_t::MSetDataCRC(uint16_t aVal) {
	FDataCrc = aVal;
}
COMPILE_ASSERT(sizeof(NSHARE::version_t) == 4, InvalidSizeOfVersion);
COMPILE_ASSERT(sizeof(head_t) == head_t::E_HEAD_SIZE, InvalidSizeOfHead);

//
//---------------------
//
template<class T, uint8_t aMajor, uint8_t aMinor, eMsgType aType>
struct dg_base_t: head_t
{
	enum
	{
		MSG_TYPE = aType, MAJOR = aMajor, MINOR = aMinor,
	};

	//static const NSHARE::version_t CURRENT_VERSION=NSHARE::version_t(aMajor,aMinor);
	typedef T dg_t;
	dg_base_t() :
			head_t(aMajor, aMinor, aType)
	{
		MSetDataSize(  sizeof(T) - FHeadSize);
	}
};
SHARED_PACKED(template<class T, uint8_t aMajor, uint8_t aMinor, eMsgType aType>
struct dg_json_base_t : dg_base_t<T, aMajor, aMinor, aType>
{

	uint16_t	FStrSize;
	uint16_t : 16;

	inline uint16_t MGetStrSize()const {
		return  head_t::MEndianCorrectValue(FStrSize);
	}
	inline void MSetStrSize(uint16_t aVal) {
		FStrSize = aVal;
	}
	inline NSHARE::utf8* MStrBegin()const { return (NSHARE::utf8*)(static_cast<T const *>(this) + 1); }
});
//
//---------------------
//
struct protocol_type_dg_t : dg_base_t<protocol_type_dg_t, 0, 1, E_PROTOCOL_MSG>
{

	uint32_t FProtocol;//eType

	inline eType MGetProtocol()const {
		return  (eType)MEndianCorrectValue(static_cast<uint32_t>(FProtocol));
	}
	inline void MSetProtocol(eType aVal) {
		FProtocol = aVal;
	}
};
COMPILE_ASSERT(sizeof(protocol_type_dg_t) == (sizeof(head_t)+4), InvalidSizeOfProtoclType);
//
//---------------------
//
//
//---------------------
//
SHARED_PACKED(struct requiest_info2_t : dg_json_base_t<requiest_info2_t, 0, 1, E_REQUEST_INFO>
{

});

//
//---------------------
//
SHARED_PACKED(struct clients_info2_t: dg_json_base_t<clients_info2_t, 0, 1, E_ALL_CLIENTS>
{
});

//
//
//---------------------
//
SHARED_PACKED(struct kernels_info_t: dg_json_base_t<kernels_info_t, 0, 1, E_KERNEL_INFO>
{

});
//
//---------------------
//
SHARED_PACKED(struct dg_info2_t: dg_json_base_t<dg_info2_t, 0, 1, E_INFO>
{
});
//
//---------------------
//
#define E_MAIN_CHANNEL_TCPSERVER "tcpser"
#define E_MAIN_CHANNEL_TCP "tcp"
#define E_MAIN_CHANNEL_UDP "udp"
#define E_MAIN_CHANNEL_SM "sm"
SHARED_PACKED(struct main_channel_param_t: dg_json_base_t<main_channel_param_t, 0, 1,
		E_MAIN_CHANNEL_PARAM>
{
/*	uint8_t FType[8]; //7 byte + 0
	union
	{
		udp_param_t FUdp;
		uint32_t FLimit;
		uint8_t FTReserved[64]; //reserved 64 byte
	};*/
});
COMPILE_ASSERT(sizeof(main_channel_param_t) == (sizeof(head_t) + 4), InvalidSizeOfMain);
//
//---------------------
//
SHARED_PACKED(struct request_main_channel_param_t: dg_base_t<main_channel_param_t, 0, 1,
		E_REQUEST_MAIN_CHANNEL_PARAM>
{
	uint8_t FType[8]; //7 byte + 0
});
COMPILE_ASSERT(sizeof(request_main_channel_param_t) == (sizeof(head_t) + 8), InvalidSizeOfRuquestMain);
//
//---------------------
//
SHARED_PACKED(struct main_channel_error_param_t: dg_base_t<main_channel_error_param_t, 0, 1,
		E_MAIN_CHANNEL_ERROR>
{
	enum eError
	{
		E_OK = 0,//
		E_NO_CHANNEL,//
		E_NOT_OPENED,//
		E_INVALID_NAME,//
		E_CANNOT_RESETTING,//
		E_UNKNOWN//
	};
	uint8_t FType[8]; //7 byte + 0
	uint8_t FError;//eError
	uint8_t :8;
	uint16_t :16;
});
COMPILE_ASSERT(sizeof(main_channel_error_param_t) == (sizeof(head_t) + 8+4), InvalidSizeOfMainChan);
//
//---------------------
//
SHARED_PACKED(struct close_main_channel_t: dg_base_t<close_main_channel_t, 0, 1,
		E_CLOSE_MAIN_CHANNEL>
{
	uint8_t FType[8]; //7 byte + 0
});
COMPILE_ASSERT(sizeof(close_main_channel_t) == (sizeof(head_t) + 8), InvalidSizeOfCloseMain);
//
//---------------------
//
SHARED_PACKED(struct user_data_flags
{
	uint8_t : 5;
	uint8_t FEndian : 2;
	uint8_t FIsLast : 1;
}
);
COMPILE_ASSERT(sizeof(user_data_flags) == (1), InvalidSizeOfUserDataFlags);
SHARED_PACKED(struct user_data_header_t
{
	user_data_header_t();

	uint64_t FUUIDFrom;


	user_data_flags FFlags;
	uint8_t FEventList;

	uint16_t FSplitCounter;

	uint32_t FRawNumber;


	uint16_t FSplitCoefficient;

	uint8_t FDestination;
	uint8_t FRouting;


	uint8_t FRegistrators;
	uint8_t FProtocolName; //(+ '\0')
	uint8_t FMinor;
	uint8_t FMajor;

	uint32_t FNumber;

	uint32_t FDataSize;
});
COMPILE_ASSERT(sizeof(user_data_header_t) == (8*4), InvalidSizeOfUserData);
inline user_data_header_t::user_data_header_t()
{
	memset(this,0,sizeof (*this));
}
SHARED_PACKED(struct user_data_dg_t: dg_base_t<user_data_dg_t, 0, 1, E_USER_DATA>
{
private:
	user_data_header_t FUserHeader;
public:
	uint8_t* MUserDataBegin()const{ return (uint8_t*)(this+1); }

	inline user_data_header_t MGetUserDataHeader() const;
	inline void  MSetUserDataHeader(user_data_header_t const&);
});
COMPILE_ASSERT(sizeof(user_data_dg_t) == (sizeof(head_t)
		+ 8*4), InvalidSizeOfDGUserData);

inline user_data_header_t user_data_dg_t::MGetUserDataHeader() const
{
	user_data_header_t _copy(FUserHeader);
	_copy.FUUIDFrom=MEndianCorrectValue(_copy.FUUIDFrom);
	_copy.FSplitCounter=MEndianCorrectValue(_copy.FSplitCounter);
	_copy.FRawNumber=MEndianCorrectValue(_copy.FRawNumber);
	_copy.FSplitCoefficient=MEndianCorrectValue(_copy.FSplitCoefficient);
	_copy.FNumber=MEndianCorrectValue(_copy.FNumber);
	_copy.FDataSize=MEndianCorrectValue(_copy.FDataSize);
	if(!MIsValidEndian())//reverse fflags
	{
		uint8_t * _p=(uint8_t *)&_copy.FFlags;
		*_p = (*_p & 0xF0) >> 4 | (*_p & 0x0F) << 4;
		*_p = (*_p & 0xCC) >> 2 | (*_p & 0x33) << 2;
		*_p = (*_p & 0xAA) >> 1 | (*_p & 0x55) << 1;
	}
	return _copy;
}
inline void user_data_dg_t::MSetUserDataHeader(user_data_header_t const& aWhat)
{
	FUserHeader=aWhat;
}
SHARED_PACKED(struct user_data_received_t: dg_base_t<user_data_received_t, 0, 1, E_USER_DATA_RECEIVED>
{
	uint32_t FUserDgCounter;
});
COMPILE_ASSERT(sizeof(user_data_received_t) == (sizeof(head_t)+4), InvalidSizeOfReceivedDGUserData);


SHARED_PACKED(struct user_data_fail_send_t: dg_json_base_t<user_data_fail_send_t, 0, 1, E_USER_DATA_FAIL_SEND>
{
});
COMPILE_ASSERT(sizeof(user_data_fail_send_t) == (sizeof(head_t)+4), InvalidSizeOfReceivedDGFailedSend);

//SHARED_PACKED(struct fail_send_dg_t: dg_base_t<fail_send_dg_t, 0, 1, E_FAIL_SEND>
//{
//	uint32_t FStrSize;
//	uint8_t* MStrBegin()const{ return (uint8_t*)(this+1); }
//});
//COMPILE_ASSERT(sizeof(fail_send_dg_t) == (sizeof(head_t)+4), InvalidSizeOfDGFailedSend);

//
//---------------------
//


SHARED_PACKED(struct custom_filters_dg2_t: dg_json_base_t<custom_filters_dg2_t, 0, 1,
		E_CUSTOMER_FILTERS>
{
});
COMPILE_ASSERT(sizeof(custom_filters_dg2_t) == (sizeof(head_t) + 4), InvalidSizeOfReceivedDGFilterDg);

//
//---------------------
//

SHARED_PACKED(struct customers_demands_t: dg_json_base_t<customers_demands_t, 0, 1, E_CUSTOMERS_DEMANDS>
{
});
COMPILE_ASSERT(sizeof(customers_demands_t) == (sizeof(head_t)+4), InvalidSizeOfCUSTOMERS_DEMANDS);

//
//---------------------
//

SHARED_PACKED(struct accept_info_t: dg_json_base_t<accept_info_t, 0, 1, E_ACCEPTED>
{
});
}//namespace NUDT



//
//---------------------
//

namespace std
{
inline std::ostream& operator<<(std::ostream & aStream, NUDT::eMsgType const& aVal)
{
	using namespace NUDT;
	switch (aVal)
	{
		case E_PROTOCOL_MSG:
			aStream << "Protocol MSG";
			break;
		case E_REQUEST_INFO:
			aStream << "Request info to new abonet";
			break;

		case E_INFO:
			aStream << "Answer to \"Request info to new abonet\"";
			break;
		case E_KERNEL_INFO:
			aStream << "Sinc info for kernels.";
			break;
		case E_ALL_CLIENTS:
			aStream << "Id of all avalible  programs for clients.";
			break;
		case E_ACCEPTED:
			aStream << "Accepted";
			break;
		case E_MAIN_CHANNEL_PARAM:
			aStream << "Main channel parametrs";
			break;

		case E_REQUEST_MAIN_CHANNEL_PARAM:
			aStream << "Request main channel parametrs";
			break;

		case E_CLOSE_MAIN_CHANNEL:
			aStream << "Close main channel parametrs";
			break;
		case E_MAIN_CHANNEL_ERROR:
			aStream << "Answer to main channel parametrs";
			break;

		case E_USER_DATA:
			aStream << "User data.";
			break;

		case E_CUSTOMER_FILTERS:
			aStream << "Reciving list.";
			break;

		case E_USER_DATA_RECEIVED:
			aStream << "User data has been received.";
			break;

		case E_USER_DATA_FAIL_SEND:
			aStream << "User data is not sent.";
			break;

		case E_CUSTOMERS_DEMANDS:
			aStream << "Demands.";
			break;

	};
	return aStream << "(" << static_cast<int>(aVal) << ")";
}
;
inline std::ostream& operator<<(std::ostream & aStream,NUDT:: head_t const& aVal)
{
	using namespace NUDT;
	aStream << "Type :" << static_cast<eMsgType const>(aVal.FType) << std::endl;
	time_t const _time = aVal.MGetTime();
	aStream << "Time:" << ctime(&_time) << "." << aVal.MGetTimeMs() << " ("
			<< aVal.MGetTime() << ")" << std::endl;
	aStream << "Version:" << aVal.FVersion << std::endl;
	aStream << "Counter:" << aVal.MGetCounter() << std::endl;
	aStream << "UUID from:" << aVal.MGetFromUUID()<< std::endl;

	aStream << "Crc:" << (unsigned) aVal.FCrcHead << std::endl;
	aStream << "Head Size:" << (unsigned) aVal.FHeadSize << std::endl;

	aStream << "Data Size:" << (unsigned) aVal.MGetDataSize() << std::endl;
	aStream << "Flags:" << (unsigned) aVal.FFlags << std::endl;
	aStream << "Endian:" << (unsigned)aVal.FEndianness << std::endl;
	aStream << "Data CRC:" << (unsigned) aVal.MGetDataCRC();

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,NUDT::protocol_type_dg_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<"Protocol " <<aVal.MGetProtocol();
	return aStream;
}

inline std::ostream& operator<<(std::ostream & aStream, NUDT::requiest_info2_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;

	return aStream;
}

inline std::ostream& operator<<(std::ostream & aStream, NUDT::dg_info2_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::clients_info2_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::accept_info_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::kernels_info_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::customers_demands_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;
	return aStream;
}



inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::main_channel_param_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal) << std::endl
		<< aVal.FType;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::request_main_channel_param_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal) << std::endl
			<< (char const*) aVal.FType;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::main_channel_error_param_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal) << std::endl
			<< (char const*) aVal.FType << std::endl
			<< static_cast<unsigned>(aVal.FError);
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::close_main_channel_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal) << std::endl
			<< (char const*) aVal.FType;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::user_data_received_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal) << std::endl<<" Kernel has received "<<aVal.FUserDgCounter;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::user_data_header_t const& aVal)
{
	using namespace NUDT;
	return aStream << "Number:"<<aVal.FNumber<<" Data size:" << aVal.FDataSize << std::endl  << "RawNumber:" << aVal.FRawNumber<< std::endl//"Name:" << aVal.FName<< std::endl
			<< "UUID From:" << aVal.FUUIDFrom << std::endl << " Destination len:"
			<< (int)aVal.FDestination<<", Routing:"<<(int)aVal.FRouting<<", Events:"<<(int)aVal.FEventList<< std::endl <<"ProtocolLen:"
			<< aVal.FProtocolName<<" Packet:"<<aVal.FSplitCounter<<" Coefficient:"<<aVal.FSplitCoefficient<<" IsLast"<<(bool)aVal.FFlags.FIsLast<<" Minor:"<<aVal.FMinor<<" Major:"<<aVal.FMajor<<" FRegistrators:"<<aVal.FRegistrators;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::user_data_dg_t const& aVal)
{
	return aStream << static_cast<NUDT::head_t const&>(aVal) << std::endl
			<<  aVal.MGetUserDataHeader();
}

inline std::ostream& operator<<(std::ostream & aStream, NUDT::custom_filters_dg2_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;
	return aStream;
}
} //namespace std
#endif /* INTERNEL_PROTOCOL_H_ */
