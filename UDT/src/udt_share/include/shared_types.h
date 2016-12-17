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
struct UDT_SHARE_EXPORT user_data_info_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText KEY_PACKET_NUMBER;
	static const NSHARE::CText KEY_PACKET_FROM;
	static const NSHARE::CText KEY_PACKET_TO;
	static const NSHARE::CText KEY_PACKET_PROTOCOL;
	static const NSHARE::CText KEY_RAW_PROTOCOL_NUM;
	user_data_info_t() :
			FPacketNumber(0),FRawProtocolNumber(0)
	{

	}
	user_data_info_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;

	uint32_t FPacketNumber;
	id_t FFrom;//Warinig The Field FName is deprecated
	NSHARE::uuid_t FUuid;
	NSHARE::smart_field_t<uuids_t> FUUIDTo;//It's deprecated, reserved for callback list vector

	//customers_names_t FDestName;
	NSHARE::CText FProtocol;
	unsigned FRawProtocolNumber;//it's optimization

	split_packet_t FSplit;
};
struct UDT_SHARE_EXPORT user_data_t
{
	user_data_info_t FDataId;
	NSHARE::CBuffer FData;
};
//
//-------------------------
//
struct UDT_SHARE_EXPORT demand_dg_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText HANDLER;
	static const uint32_t NO_HANDLER;
	required_header_t FWhat;
	//NSHARE::CText FNameRegExp;	//from
	NSHARE::CRegistration FNameFrom;
	NSHARE::smart_field_t<NSHARE::uuid_t> FUUIDFrom;
	NSHARE::CText FProtocol;
	uint32_t FHandler;

	demand_dg_t():FHandler(NO_HANDLER)
	{
		;
	}
	demand_dg_t(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
	bool operator==(demand_dg_t const& aRht) const;
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
struct UDT_SHARE_EXPORT routing_t: uuids_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText FROM;
	id_t FFrom;
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
struct UDT_SHARE_EXPORT fail_send_t:user_data_info_t
{
	enum eError
	{
		E_THE_BUFFER_IS_SMALL=-1,
	};
	fail_send_t():FError(E_THE_BUFFER_IS_SMALL)
	{

	}
	explicit fail_send_t(NSHARE::CConfig const& aConf) :
			user_data_info_t(aConf),FError(E_THE_BUFFER_IS_SMALL)
	{
		;
	}
	explicit fail_send_t(user_data_info_t const& aRht) :
			user_data_info_t(aRht),FError(E_THE_BUFFER_IS_SMALL)
	{

	}
	eError FError;
};
typedef std::vector<fail_send_t> fail_send_array_t;
//
//-------------------------
//
enum eErrorCode
{
	E_NO_ERROR = 0,
	E_SENT_ERROR = 1,
	E_ROUTING_ERROR = 2,
};
struct UDT_SHARE_EXPORT error_info_t
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText WHERE;
	static const NSHARE::CText CODE;

	eErrorCode FError;
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

	aStream <<"#" <<aVal.FPacketNumber<<" From:" << aVal.FFrom << ";";
	//aStream << "To:" << aVal.FDestName;
	if (aVal.FUUIDTo.MIs())
	{
		aStream << "(" << aVal.FUUIDTo.MGetConst() << ")" << ";";
	}
	else
		aStream << "();";
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
	 aStream<<static_cast<NUDT::user_data_info_t const&>(aVal);
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

}
#endif /* SHARED_TYPES_OF_SHARE_H_ */
