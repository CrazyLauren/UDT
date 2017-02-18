/*
 * shared_types.h
 *
 *  Created on: 30.03.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SHARED_TYPES_OF_SHARE_H_
#define SHARED_TYPES_OF_SHARE_H_
#include <udt_share_macros.h>
#include <programm_id.h>
#include <udt_types.h>
namespace NUDT
{
typedef uint32_t error_type;
typedef uint8_t user_error_type;

enum
{
	eUserErrorStartBits=(sizeof(error_type)-sizeof(user_error_type))*8
};
enum eError
{
	E_NO_ERROR=0x0,
	//Resreved=0x1<<1,
	E_CANNOT_READ_CONFIGURE=0x1<<2,
	E_CONFIGURE_IS_INVALID= 0x1<<3,
	E_NO_NAME= 0x1<<4,
	E_NOT_OPEN = 0x1<<5,
	E_NAME_IS_INVALID= 0x1<<6,
	E_NOT_CONNECTED_TO_KERNEL=0x1<<7,
	E_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE= 0x1<<8,

	//reserved=0x1<<9,
	E_HANDLER_IS_NOT_EXIST=0x1<<10,
	E_NO_ROUTE=0x1<<11,
	E_UNKNOWN_ERROR=0x1<<12,
	E_PARSER_IS_NOT_EXIST=0x1<<13,
	E_HANDLER_NO_MSG_OR_MORE_THAN_ONE=0x1<<14,
	E_SOCKET_CLOSED=0x1<<15,
	E_BUFFER_IS_FULL=0x1<<16,
	E_PACKET_LOST=0x1<<17,
	E_MERGE_ERROR=0x1<<19,
	E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE=0x1<<20,
	E_INCORRECT_USING_OF_UDT_1 = 0x1<<21,
//	Resreved = 0x1<<22,
	E_USER_ERROR_BEGIN=(0x1)<<(eUserErrorStartBits-1)
};
extern UDT_SHARE_EXPORT  error_type const USER_ERROR_MASK;

extern UDT_SHARE_EXPORT const NSHARE::CText RAW_PROTOCOL_NAME;
typedef std::vector<NSHARE::CText> customers_names_t;

struct UDT_SHARE_EXPORT uuids_t: std::vector<NSHARE::uuid_t>
{
	static const NSHARE::CText NAME;

	uuids_t()
	{

	}

	uuids_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
};
//
//-------------------------
//
struct UDT_SHARE_EXPORT split_packet_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText LAST;
	static const NSHARE::CText COUNTER;
	static const NSHARE::CText COEFFICIENT;

	split_packet_t():
		FIsLast(true),//
		FCounter(1),//
		FCoefficient(1)
	{

	}
	split_packet_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
	bool MIsSplited()const;
	size_t MGetAlignmentSize(size_t const&)const;

	bool FIsLast;
	uint32_t FCounter;
	uint16_t FCoefficient;
};
//
//-------------------------
//
struct UDT_SHARE_EXPORT demand_dg_t
{
	typedef uint32_t event_handler_t;

	static const NSHARE::CText NAME;
	static const NSHARE::CText HANDLER;
	static const NSHARE::CText KEY_FLAGS;
	static const uint32_t NO_HANDLER;

	enum
	{
		E_NO_DEMAND_FLAGS=0,
		E_REGISTRATOR=0x1<<0
	};
	required_header_t FWhat;
	//NSHARE::CText FNameRegExp;	//from
	NSHARE::CRegistration FNameFrom;
	NSHARE::smart_field_t<NSHARE::uuid_t> FUUIDFrom;
	NSHARE::CText FProtocol;
	uint32_t FHandler;
	uint32_t FFlags;

	demand_dg_t():
		FHandler(NO_HANDLER),//
		FFlags(E_NO_DEMAND_FLAGS)//
	{
		;
	}
	demand_dg_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
	bool operator==(demand_dg_t const& aRht) const;
	static std::pair<required_header_t,bool> sMParseHead(NSHARE::CConfig const& aConf);
};
//
//-------------------------
//
struct UDT_SHARE_EXPORT routing_t: uuids_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText FROM;
	id_t FFrom;//from.FName - Depreciated
	routing_t()
	{

	}
	routing_t(id_t const& aFrom, uuids_t const& aWhat) :
			uuids_t(aWhat),	//
			FFrom(aFrom)
	{

	}
	explicit routing_t(NSHARE::CConfig const& aConf);

	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
};
//
//-------------------------
//
struct UDT_SHARE_EXPORT user_data_info_t
{

	static const NSHARE::CText NAME;
	static const NSHARE::CText KEY_PACKET_NUMBER;
	static const NSHARE::CText KEY_PACKET_FROM;
	static const NSHARE::CText KEY_PACKET_TO;
	static const NSHARE::CText KEY_PACKET_PROTOCOL;
	static const NSHARE::CText KEY_RAW_PROTOCOL_NUM;
	user_data_info_t() :
			FPacketNumber(0),//
			FRawProtocolNumber(0)//
	{

	}
	user_data_info_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
	bool MIsRaw() const;

	//id_t FFrom;//from.FName - Depreciated
	uint32_t FPacketNumber;
	NSHARE::version_t FVersion;

	NSHARE::CText FProtocol;
	unsigned FRawProtocolNumber;//it's optimization

	std::vector<demand_dg_t::event_handler_t> FEventsList;
	uuids_t FDestination;
	routing_t FRouting;

	split_packet_t FSplit;
};
struct UDT_SHARE_EXPORT user_data_t
{
	user_data_info_t FDataId;
	NSHARE::CBuffer FData;
};

struct UDT_SHARE_EXPORT demand_dgs_t:std::vector<demand_dg_t>
{
	static const NSHARE::CText NAME;
	demand_dgs_t()
	{
	}

	demand_dgs_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
};
struct UDT_SHARE_EXPORT demand_dgs_for_t:std::map<id_t,demand_dgs_t>
{
	static const NSHARE::CText NAME;
	demand_dgs_for_t()
	{
	}

	demand_dgs_for_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
};
//
//-------------------------
//
struct UDT_SHARE_EXPORT kernel_link
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText LATENCY;
	static const NSHARE::CText TIME;
	static const NSHARE::CText LINK;

	program_id_t FProgramm;
	mutable uint64_t FConnectTime;//ms
	NSHARE::CText FTypeLink;
	mutable uint16_t FLatency;

	kernel_link() :
			FLatency(std::numeric_limits<uint16_t>::max())
	{
		FConnectTime = 0;
	}
	kernel_link(program_id_t const&,uint16_t  aLatency);

	kernel_link(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
	bool operator<(kernel_link const& aRht) const
	{
		return FProgramm < aRht.FProgramm;
	}
	bool operator==(kernel_link const& aRht) const
	{
		return FProgramm == aRht.FProgramm;
	}
	bool operator!=(kernel_link const& aRht) const
	{
		return !operator==(aRht);
	}
	bool operator==(program_id_t const& aRht) const
	{
		return FProgramm == aRht;
	}
	bool operator!=(program_id_t const& aRht) const
	{
		return !operator==(aRht);
	}
};
struct UDT_SHARE_EXPORT progs_id_t:std::set<program_id_t>
{
	static const NSHARE::CText NAME;
	progs_id_t()
	{

	};

	progs_id_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
};
typedef  std::set<kernel_link> kern_links_t;
struct UDT_SHARE_EXPORT kernel_infos_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText CLIENT;
	static const NSHARE::CText NUMBER;

	program_id_t FKernelInfo;
	mutable kern_links_t FCustomerInfo;
	uint32_t FIndexNumber;//The greate the number the later the object is created (using then there is loop)

	kernel_infos_t():FIndexNumber(0)
	{

	}

	explicit kernel_infos_t(program_id_t const& aId):
		FKernelInfo(aId)
	{
		FIndexNumber=0;
	}

	kernel_infos_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;

	bool operator==(kernel_infos_t const& aRht) const
	{
		return FKernelInfo == aRht.FKernelInfo && FCustomerInfo == aRht.FCustomerInfo;
	}
	bool operator==(program_id_t const& aRht) const
	{
		return FKernelInfo == aRht;
	}
	bool operator!=(kernel_infos_t const& aRht) const
	{
		return !operator==(aRht);
	}
	bool operator!=(program_id_t const& aRht) const
	{
		return !operator==(aRht);
	}
};
struct CKernelInfoLessCompare
{
	bool operator()(const NUDT::kernel_infos_t& a,
			const NUDT::kernel_infos_t& b) const
	{
		return (a.FKernelInfo.FId < b.FKernelInfo.FId);
	}
	bool operator()(const NUDT::kernel_infos_t& a,
			const NUDT::program_id_t& b) const
	{
		return (a.FKernelInfo.FId < b.FId);
	}
	bool operator()(const NUDT::program_id_t& a,
			const NUDT::kernel_infos_t& b) const
	{
		return (a.FId< b.FKernelInfo.FId);
	}
};
typedef std::set<kernel_infos_t,CKernelInfoLessCompare> kernel_list_t;
struct UDT_SHARE_EXPORT kernel_infos_array_t:kernel_list_t
{
	static const NSHARE::CText NAME;
	kernel_infos_array_t()
	{
	}

	kernel_infos_array_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
	kernel_list_t& MVec();
};
//
//-------------------------
//
typedef std::vector<std::pair<kernel_infos_t,bool> > k_diff_t;
struct  UDT_SHARE_EXPORT kernel_infos_diff_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText OPENED_KERNELS;
	static const NSHARE::CText CLOSED_KERNELS;

	k_diff_t FOpened;//if true  the kernel is opened
	k_diff_t FClosed;//if true  the kernel is closed
	NSHARE::CConfig MSerialize() const;

	bool MIsValid()const;
};

//
//-------------------------
//
struct UDT_SHARE_EXPORT fail_send_t:user_data_info_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText CODE;

	user_error_type MGetUserError() const;
	error_type MGetInnerError() const;//without user code
	void MSetUserError(user_error_type);
	void MSetError(error_type);

	fail_send_t() :
			FError(E_NO_ERROR)	//
	{

	}
	explicit fail_send_t(NSHARE::CConfig const& aConf);
	explicit fail_send_t(user_data_info_t const& aRht);
	explicit fail_send_t(user_data_info_t const& aRht, const uuids_t& aTo,error_type aError =E_NO_ERROR);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
	bool MIsError()const;

	NSHARE::CFlags<error_type,error_type> FError;
};
typedef std::vector<fail_send_t> fail_send_array_t;
//
//-------------------------
//
struct UDT_SHARE_EXPORT error_info_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText WHERE;
	static const NSHARE::CText CODE;

	eError FError;
	id_t FWhere;
	uuids_t FTo;

	error_info_t();
	error_info_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
};
//---------------

}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::uuids_t const& aVal)
{
	if (!aVal.empty())
		for (NUDT::uuids_t::const_iterator _it = aVal.begin();;)
		{
			aStream << _it->MToString();

			if (++_it != aVal.end())
				aStream << ";";
			else
				break;
		}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::routing_t const& aVal)
{
	aStream << "RoutingTo:" << static_cast<NUDT::uuids_t const&>(aVal)
			<< " FFrom" << aVal.FFrom;
	return aStream;
}

inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::customers_names_t const& aVal)
{
	if (!aVal.empty())
		for (NUDT::customers_names_t::const_iterator _it = aVal.begin();;)
		{
			aStream << *_it;

			if (++_it != aVal.end())
				aStream << ";";
			else
				break;
		}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::split_packet_t const& aVal)
{
	return aStream << "Count=" << aVal.FCounter << "; Coef="
			<< (unsigned)aVal.FCoefficient << " Last:" << aVal.FIsLast;
}

inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::user_data_info_t const& aVal)
{

	aStream <<"#" <<aVal.FPacketNumber;
	if (aVal.FVersion.MIsExist())
	{
		aStream << " v" << aVal.FVersion;
	}
	aStream<<" From:" << aVal.FRouting.FFrom << ";";
	//aStream << "To:" << aVal.FDestName;
	if (!aVal.FDestination.empty())
	{
		aStream << "dest(" << aVal.FDestination << ")" << ";";
	}
	else
		aStream << "();";

	if (!aVal.FRouting.empty())
	{
		aStream << " ==> " << aVal.FRouting << " <> " << ";";
	}

	if (!aVal.FEventsList.empty())
	{
		std::vector<NUDT::demand_dg_t::event_handler_t>::const_iterator _it =
				aVal.FEventsList.begin(), _it_end(aVal.FEventsList.end());
		aStream << " | ";
		for(;_it!=_it_end;++_it)
		{
			aStream << *_it << ",";
		}

		aStream << " | " << ";";
	}


	if(aVal.FSplit.MIsSplited())
			aStream << aVal.FSplit<<";";
	aStream << "Protocol :" << aVal.FProtocol;
	aStream << "RawNum :" << aVal.FRawProtocolNumber;

	//aStream <<  aVal.FData;

	return aStream;
}

inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::fail_send_t const& aVal)
{
	 aStream<<static_cast<NUDT::user_data_info_t const&>(aVal)<<" Error:"<<aVal.FError;
	 return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::user_data_t const& aVal)
{

	aStream << "From:" << aVal.FDataId << "; Data:" << aVal.FData.size()
			<< " bytes.";
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::demand_dg_t const& aVal)
{

	aStream << "From:" << aVal.FNameFrom << std::endl;
	aStream << "Protocol :" << aVal.FProtocol << std::endl;
	aStream << "Handler :" << aVal.FHandler << std::endl;
	aStream << "Flags :" << aVal.FFlags << std::endl;
	aStream << aVal.FWhat;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::demand_dgs_t const& aVal)
{
	for (NUDT::demand_dgs_t::const_iterator _it = aVal.begin();
			_it != aVal.end();)
	{
		aStream << (*_it);
		if (++_it != aVal.end())
			aStream << std::endl;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::kernel_link const& aVal)
{
	return aStream<<"Latency "<<aVal.FLatency<<"; Link"<<aVal.FTypeLink<<" "<<aVal.FProgramm;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::progs_id_t const& aVal)
{
	for (NUDT::progs_id_t::const_iterator _it = aVal.begin(); _it != aVal.end();
			)
	{
		aStream << (*_it);
		if (++_it != aVal.end())
			aStream << std::endl;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::kern_links_t const& aVal)
{
	for (NUDT::kern_links_t::const_iterator _it = aVal.begin(); _it != aVal.end();
			)
	{
		aStream << (*_it);
		if (++_it != aVal.end())
			aStream << std::endl;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::kernel_infos_t const& aVal)
{
	aStream << "Kernel:" << aVal.FKernelInfo << std::endl;

	return aStream << aVal.FCustomerInfo;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::kernel_infos_array_t const& aVal)
{
	for (NUDT::kernel_infos_array_t::const_iterator _it = aVal.begin();
			_it != aVal.end();)
	{
		aStream << (*_it);
		if (++_it != aVal.end())
			aStream << std::endl;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::demand_dgs_for_t const& aVal)
{
	for (NUDT::demand_dgs_for_t::const_iterator _it = aVal.begin();
			_it != aVal.end();)
	{
		aStream << (_it->first)<<"; dems: "<<_it->second;
		if (++_it != aVal.end())
			aStream << std::endl;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::error_info_t const& aVal)
{
	aStream << "Error:" << aVal.FError<<" Where:"<<aVal.FWhere<<" To:"<<aVal.FTo << std::endl;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::eError const& aVal)
{
	NSHARE::CFlags<NUDT::eError,NUDT::error_type> const _val(aVal);
	if (_val.MGetFlag(NUDT::E_CANNOT_READ_CONFIGURE))
	{
		aStream << " Cannot read configure,";
	}
	if (_val.MGetFlag(NUDT::E_CONFIGURE_IS_INVALID))
	{
		aStream << " Configure is invalid,";
	}

	if (_val.MGetFlag(NUDT::E_NO_NAME))
	{
		aStream << " Name is not exist,";
	}
	if (_val.MGetFlag(NUDT::E_NOT_OPEN))
	{
		aStream << " Not Opened,";
	}
	if (_val.MGetFlag(NUDT::E_NAME_IS_INVALID))
	{
		aStream << " Name is invalid,";
	}
	if (_val.MGetFlag(NUDT::E_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE))
	{
		aStream << " Cannot allocate buffer of requrement size,";
	}

	if (_val.MGetFlag(NUDT::E_HANDLER_IS_NOT_EXIST))
	{
		aStream << " Handler is not exist,";
	}

	if (_val.MGetFlag(NUDT::E_NO_ROUTE))
	{
		aStream << " No route,";
	}
	if (_val.MGetFlag(NUDT::E_UNKNOWN_ERROR))
	{
		aStream << " Unknown error,";
	}
	if (_val.MGetFlag(NUDT::E_PARSER_IS_NOT_EXIST))
	{
		aStream << " Parser is not exist,";
	}
	if (_val.MGetFlag(NUDT::E_HANDLER_NO_MSG_OR_MORE_THAN_ONE))
	{
		aStream << " No msg in the buffer or the number of msg is more than one,";
	}
	if (_val.MGetFlag(NUDT::E_SOCKET_CLOSED))
	{
		aStream << " Connection closed,";
	}
	if (_val.MGetFlag(NUDT::E_BUFFER_IS_FULL))
	{
		aStream << " The kernel buffer is full,";
	}
	if (_val.MGetFlag(NUDT::E_PACKET_LOST))
	{
		aStream << " The packet is lost,";
	}
	if (_val.MGetFlag(NUDT::E_MERGE_ERROR))
	{
		aStream << " Cannot merge msg,";
	}
	if (_val.MGetFlag(NUDT::E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE))
	{
		aStream << " Protocol version is not compatible,";
	}
	if (_val.MGetFlag(NUDT::E_INCORRECT_USING_OF_UDT_1))
	{
		aStream
				<< " There are two msg's handler with different type:  registrator and  'real'!!! Please, Refractoring your code or not sent this msg by uuid, ";
	}

	if (_val.MGetFlag(NUDT::E_USER_ERROR_BEGIN))
	{
		aStream << " User error code="
				<< ((_val.MGetMask() & NUDT::USER_ERROR_MASK)
						>> NUDT::eUserErrorStartBits) << ",";
	}


	return aStream;
}

}
#endif /* SHARED_TYPES_OF_SHARE_H_ */
