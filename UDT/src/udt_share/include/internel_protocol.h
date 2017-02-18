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
	uint32_t const FHeadSize:8;
	uint32_t FTimeMs:16; //ms
	uint32_t const FEndianness:8;

	//2-d word
	NSHARE::version_t const FVersion;

	//3-d word
	uint32_t FDataSize;

	//4-d word
	uint32_t FTime;

	//5-d,6-d word
	uint64_t FFromUUID;//can be problem with size

	//7-d word
	uint16_t FDataCrc;
	uint16_t FType :8;//eMsgType
	uint16_t FFlags :8;//eHeadFlags
	//8-d word
	uint32_t FCounter:24;
	uint32_t FCrcHead:8;

	//fix //fucking msvc error c2503
	//uint8_t FDataBegin[0];
	inline uint8_t* MDataBegin()const{
		return (uint8_t*)(this+1);
	}
	inline bool MIs(eHeadFlags ) const;
	inline void MSet(unsigned ,bool aVal );

	head_t(uint8_t aMajor,uint8_t aMinor,eMsgType aType);
});
inline head_t::head_t(uint8_t aMajor, uint8_t aMinor, eMsgType aType) :
	FHeadSize(E_HEAD_SIZE),//
	FTimeMs(0),//
	FEndianness(E_ORDER_CHECK),//
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
		FDataSize = sizeof(T) - FHeadSize;
	}
};
//
//---------------------
//
struct protocol_type_dg_t: dg_base_t<protocol_type_dg_t, 0, 1, E_PROTOCOL_MSG>
{
	uint32_t FProtocol;//eType
};
COMPILE_ASSERT(sizeof(protocol_type_dg_t) == (sizeof(head_t)+4), InvalidSizeOfProtoclType);
//
//---------------------
//
SHARED_PACKED(struct info_t
{
	info_t();
	//0
	uint64_t FPid;
	//2
	uint64_t FFullUUID;//fixme depreceted
	//4
	uint16_t FNameLen;
	uint16_t FPathLen;
	//5
	uint16_t FType :8;//eType
	uint16_t FReserved1 :8;
	uint16_t FReserved2 :16;
	//6
	NSHARE::version_t FKernelVersion;
	//7
	uint32_t FStartTime;
	//8
	//uint8_t FDynamicBegin[0]; //FNameLen, FPathLen
	uint8_t* MDynamicBegin() const
	{
		return (uint8_t*)(this+1);
	}
});
COMPILE_ASSERT(sizeof(info_t) == 8 * 4, InvalidSizeOfInfo);
inline info_t::info_t()
{
	memset(this,0,sizeof (*this));
}
//
//---------------------
//
SHARED_PACKED(struct requiest_info_t : dg_base_t<requiest_info_t, 0, 1, E_REQUEST_INFO>
{
	info_t FInfo;
});
SHARED_PACKED(struct requiest_info2_t : dg_base_t<requiest_info2_t, 0, 1, E_REQUEST_INFO>
{
	//info_t FInfo;
	uint16_t	FStrSize;
	uint16_t	:16;
	inline NSHARE::utf8* MStrBegin()const{ return (NSHARE::utf8*)(this+1); }
});
//
//---------------------
//
SHARED_PACKED(struct clients_info_t: dg_base_t<clients_info_t, 0, 1, E_ALL_CLIENTS>
{
	uint16_t	FNumberOfInfo:16;//is used to save the number of info_t
	uint16_t	:16;

	//info_t FArrayInfo[0];
	uint8_t* MArrayInfo() const
		{
			return (uint8_t*)(this+1);
		}
});
SHARED_PACKED(struct clients_info2_t: dg_base_t<clients_info2_t, 0, 1, E_ALL_CLIENTS>
{
	uint16_t	FStrSize;
	uint16_t	:16;
	inline NSHARE::utf8* MStrBegin()const{ return (NSHARE::utf8*)(this+1); }
});
//
//
//---------------------
//
SHARED_PACKED(struct kernels_info_t: dg_base_t<kernels_info_t, 0, 1, E_KERNEL_INFO>
{
	uint16_t	FStrSize;
	uint16_t	:16;
	inline NSHARE::utf8* MStrBegin()const{ return (NSHARE::utf8*)(this+1); }

});
//
//---------------------
//
SHARED_PACKED(struct dg_info_t: dg_base_t<dg_info_t, 0, 1, E_INFO>
{
	info_t FInfo;
});
SHARED_PACKED(struct dg_info2_t: dg_base_t<dg_info2_t, 0, 1, E_INFO>
{
	uint16_t	FStrSize;
	uint16_t	:16;
	inline NSHARE::utf8* MStrBegin()const{ return (NSHARE::utf8*)(this+1); }
});
//
//---------------------
//
SHARED_PACKED(struct udp_param_t
{
	uint32_t FAddr; //if 0 - addr is not set
	uint32_t FPort;
});
COMPILE_ASSERT(sizeof(udp_param_t) == 2 * 4, InvalidSizeOfUdpParam);

#define E_MAIN_CHANNEL_TCPSERVER "tcpser"
#define E_MAIN_CHANNEL_TCP "tcp"
#define E_MAIN_CHANNEL_UDP "udp"
#define E_MAIN_CHANNEL_SM "sm"
SHARED_PACKED(struct main_channel_param_t: dg_base_t<main_channel_param_t, 0, 1,
		E_MAIN_CHANNEL_PARAM>
{
	uint8_t FType[8]; //7 byte + 0
	union
	{
		udp_param_t FUdp;
		uint32_t FLimit;
		uint8_t FTReserved[64]; //reserved 64 byte
	};
});
COMPILE_ASSERT(sizeof(main_channel_param_t) == (sizeof(head_t) + 8+64), InvalidSizeOfMain);
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
	uint32_t FError :32;//eError
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
SHARED_PACKED(struct user_data_header_t
{
	user_data_header_t();
	uint64_t FUUIDFrom;
	uint64_t FIsLast:1;
	uint64_t FSplitCounter:23;
	uint64_t FEventList:8;
	uint64_t FRawNumber:32;
	//uint64_t FUUIDTo; //if 0 is not exist

	uint16_t FSplitCoefficient;
	uint16_t FDestination:8;
	uint16_t FRouting:8;


	uint16_t FProtocolName; //(+ '\0')
	uint16_t FMinor:8;
	uint16_t FMajor:8;
	uint32_t FNumber:32;

	uint32_t FDataSize;
});
COMPILE_ASSERT(sizeof(user_data_header_t) == (8*4), InvalidSizeOfUserData);
inline user_data_header_t::user_data_header_t()
{
	memset(this,0,sizeof (*this));
}
SHARED_PACKED(struct user_data_dg_t: dg_base_t<user_data_dg_t, 0, 1, E_USER_DATA>
{
	user_data_header_t FUserHeader;
	uint8_t* MUserDataBegin()const{ return (uint8_t*)(this+1); }
	//uint8_t FUserDataBegin[0];
});
COMPILE_ASSERT(sizeof(user_data_dg_t) == (sizeof(head_t)
		+ sizeof(user_data_header_t)), InvalidSizeOfDGUserData);

SHARED_PACKED(struct user_data_received_t: dg_base_t<user_data_received_t, 0, 1, E_USER_DATA_RECEIVED>
{
	uint32_t FUserDgCounter;
});
COMPILE_ASSERT(sizeof(user_data_received_t) == (sizeof(head_t)+4), InvalidSizeOfReceivedDGUserData);


SHARED_PACKED(struct user_data_fail_send_t: dg_base_t<user_data_fail_send_t, 0, 1, E_USER_DATA_FAIL_SEND>
{
	uint32_t FStrSize;
	uint8_t* MStrBegin()const{ return (uint8_t*)(this+1); }
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
SHARED_PACKED(struct customer_filter_t
{
	customer_filter_t();
	NSHARE::version_t FVersion;
	uint8_t FNumber[32];
	uint16_t FFrom;
	uint16_t FProtocolName;

	//uint8_t FBegin[0];
});
COMPILE_ASSERT(sizeof(customer_filter_t) == (10*4), InvalidSizeOfCustomFilter);
inline customer_filter_t::customer_filter_t()
{
	memset(this,0,sizeof (*this));
}
SHARED_PACKED(struct custom_filters_dg_t: dg_base_t<custom_filters_dg_t, 0, 1,
		E_CUSTOMER_FILTERS>
{
	//customer_filter_t FFilters[0];
	uint8_t* MFilters() const
	{
		return (uint8_t*)(this+1);
	}
});
COMPILE_ASSERT(sizeof(custom_filters_dg_t) == sizeof(head_t), InvalidSizeOfDGUserData);


SHARED_PACKED(struct custom_filters_dg2_t: dg_base_t<custom_filters_dg2_t, 0, 1,
		E_CUSTOMER_FILTERS>
{
	uint16_t	FStrSize;
	uint16_t	:16;
	inline NSHARE::utf8* MStrBegin()const{ return (NSHARE::utf8*)(this+1); }
});

//
//---------------------
//

SHARED_PACKED(struct customers_demands_t: dg_base_t<customers_demands_t, 0, 1, E_CUSTOMERS_DEMANDS>
{
	uint16_t	FStrSize;
	uint16_t	:16;
	inline NSHARE::utf8* MStrBegin()const{ return (NSHARE::utf8*)(this+1); }
});
COMPILE_ASSERT(sizeof(customers_demands_t) == (sizeof(head_t)+4), InvalidSizeOfCUSTOMERS_DEMANDS);


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
	time_t const _time = aVal.FTime;
	aStream << "Time:" << ctime(&_time) << "." << aVal.FTimeMs << " ("
			<< aVal.FTime << ")" << std::endl;
	aStream << "Version:" << aVal.FVersion << std::endl;
	aStream << "Counter:" << aVal.FCounter << std::endl;
	aStream << "UUID from:" << aVal.FFromUUID << std::endl;

	aStream << "Crc:" << (unsigned) aVal.FCrcHead << std::endl;
	aStream << "Head Size:" << (unsigned) aVal.FHeadSize << std::endl;

	aStream << "Data Size:" << (unsigned) aVal.FDataSize << std::endl;
	aStream << "Flags:" << (unsigned) aVal.FFlags << std::endl;
	aStream << "Data CRC:" << (unsigned) aVal.FDataCrc;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,NUDT::protocol_type_dg_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<"Protocol " <<aVal.FProtocol;
	return aStream;
}


inline std::ostream& operator<<(std::ostream & aStream, NUDT::info_t const& aVal)
{
	using namespace NUDT;
	aStream << "Type :" << static_cast<eType const>(aVal.FType) << std::endl;
	time_t const _time = aVal.FStartTime;
	aStream << "StartTime:" << ctime(&_time) << " ("
			<< aVal.FStartTime << ")" << std::endl;
	aStream << "Kernel Version:" << aVal.FKernelVersion << std::endl;
	NSHARE::uuid_t _uuid;
	_uuid.FVal = aVal.FFullUUID;
	aStream << "FullUUID:" << _uuid.MToString() << "(" << aVal.FFullUUID << ")"
			<< std::endl;

	aStream << "PID:" << aVal.FPid << std::endl;
	aStream << "NameLen:" << aVal.FNameLen << std::endl;
	aStream << "PathLen:" << aVal.FPathLen;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::requiest_info_t const& aVal)
{
	aStream << aVal.FInfo << std::endl;
	size_t _full_size = sizeof(aVal.FInfo) + aVal.FInfo.FNameLen
			+ aVal.FInfo.FPathLen;
	if (aVal.FDataSize == _full_size)
	{
		NSHARE::CText _name((NSHARE::utf8 const*) aVal.FInfo.MDynamicBegin());
		NSHARE::CText _path(
				(NSHARE::utf8 const*) (aVal.FInfo.MDynamicBegin()
						+ aVal.FInfo.FNameLen));

		aStream << "Name:" << _name << std::endl;
		aStream << "Path:" << _path;
	}
	else
	{
		LOG(ERROR) << "Invalid DG SIZE!!!" << aVal.FDataSize << "!="
				<< _full_size;
		aStream << "Invalid DG SIZE!!!";
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::requiest_info2_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<NSHARE::CText (aVal.MStrBegin())  << std::endl;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::dg_info_t const& aVal)
{
	using namespace NUDT;
	aStream << aVal.FInfo << std::endl;
	size_t _full_size = sizeof(info_t) + aVal.FInfo.FNameLen
			+ aVal.FInfo.FPathLen;
	if (aVal.FDataSize == _full_size)
	{
		NSHARE::CText _name((NSHARE::utf8 const*) aVal.FInfo.MDynamicBegin());
		NSHARE::CText _path(
				(NSHARE::utf8 const*) (aVal.FInfo.MDynamicBegin()
						+ aVal.FInfo.FNameLen));

		aStream << "Name:" << _name << std::endl;
		aStream << "Path:" << _path;
	}
	else
	{
		LOG(ERROR) << "Invalid DG SIZE!!!" << aVal.FDataSize << "!="
				<< _full_size;
		aStream << "Invalid DG SIZE!!!";
	}
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

inline std::ostream& operator<<(std::ostream & aStream, NUDT::clients_info_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal);
	aStream <<"The Number of infos " <<aVal.FNumberOfInfo;
	return aStream;
}

inline std::ostream& operator<<(std::ostream & aStream, NUDT::udp_param_t const& aVal)
{
	return aStream << "Ip:" << aVal.FAddr << ";Port:" << aVal.FPort;
}
;
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::main_channel_param_t const& aVal)
{
	using namespace NUDT;
	aStream << static_cast<head_t const&>(aVal) << std::endl
			<< (char*) aVal.FType << std::endl;
	if (strcmp((char const*) aVal.FType, E_MAIN_CHANNEL_UDP) == 0)
		aStream << aVal.FUdp;
	else if (strcmp((char const*) aVal.FType, E_MAIN_CHANNEL_SM) == 0)
		aStream << "sm main channel";
	else if (strcmp((char const*) aVal.FType, E_MAIN_CHANNEL_TCP) == 0)
{
	aStream << "tcp main channel";
	aStream << "Mtu" << aVal.FLimit;
}
	else if (strcmp((char const*) aVal.FType, E_MAIN_CHANNEL_TCPSERVER) == 0)
{
	aStream << "tcpserver main channel";
	aStream << "Mtu" << aVal.FLimit;
}
	else
		LOG(DFATAL) << "Unknown type of main channel"
				<< (char const*) aVal.FType;
	return aStream;
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
		NUDT::user_data_header_t const& aVal)
{
	using namespace NUDT;
	return aStream << "Number:"<<aVal.FNumber<<" Data size:" << aVal.FDataSize << std::endl  << "RawNumber:" << aVal.FRawNumber<< std::endl//"Name:" << aVal.FName<< std::endl
			<< "UUID From:" << aVal.FUUIDFrom << std::endl << " Destination len:"
			<< (int)aVal.FDestination<<", Routing:"<<(int)aVal.FRouting<<", Events:"<<(int)aVal.FEventList<< std::endl <<"ProtocolLen:"
			<< aVal.FProtocolName<<" Packet:"<<aVal.FSplitCounter<<" Coefficient:"<<aVal.FSplitCoefficient<<" IsLast"<<(bool)aVal.FIsLast<<" Minor:"<<aVal.FMinor<<" Major:"<<aVal.FMajor;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::user_data_received_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal) << std::endl<<" Kernel has received "<<aVal.FUserDgCounter;
}
inline std::ostream& operator<<(std::ostream & aStream, NUDT::user_data_dg_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal) << std::endl
			<<  aVal.FUserHeader;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::custom_filters_dg_t const& aVal)
{
	using namespace NUDT;
	return aStream << static_cast<head_t const&>(aVal);
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
