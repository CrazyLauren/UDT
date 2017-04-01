/*
 * kernel_type.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 30.11.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef KERNEL_TYPE_H_
#define KERNEL_TYPE_H_

#include <share_socket.h>
#include <programm_id.h>
#include <internel_protocol.h>
#include <udt_types.h>
#include <shared_types.h>
namespace NUDT
{
typedef int descriptor_t;
typedef std::vector<descriptor_t> descriptors_t;
//convenient base class
template<class _T> struct data_from_id
{	// store a pair of values
	typedef data_from_id<_T> my_t;
	typedef descriptor_t _id_t;
	typedef _T _type_t;
	static const NSHARE::CText NAME;
	data_from_id() :
			FId(-1), FVal(_T())
	{	// construct from defaults
	}

	data_from_id(const descriptor_t& _Val1, const _T& _Val2) :
			FId(_Val1), FVal(_Val2)
	{	// construct from specified values
	}

	template<class _U>
	data_from_id(const data_from_id<_U>& _Right) :
			FId(_Right.FId), FVal(_Right.FVal)
	{
	}

	void swap(my_t& _Right)
	{
		std::swap(FId, _Right.FId);
		std::swap(FVal, _Right.FVal);
	}

	_id_t FId;
	_type_t FVal;
};

////////////////////////////////////////////
struct descriptor_info_t:kernel_link
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText CONNECTION_INFO;
	NSHARE::CConfig FInfo;

	//---
	//bool operator<(NSHARE::net_address const& aRht) const;
	bool operator<(descriptor_info_t const& aRht) const;
	bool operator==(descriptor_info_t const& aRht) const;
	NSHARE::CConfig MSerialize() const;
};
struct open_descriptor
{
	static const NSHARE::CText NAME;

	descriptor_t FId;
	descriptor_info_t FInfo;

	open_descriptor(descriptor_t aVal, descriptor_info_t const&aVal2) :
		FId(aVal), FInfo(aVal2)
	{

	}
};
struct close_descriptor
{
	static const NSHARE::CText NAME;

	descriptor_t FId;
	descriptor_info_t FInfo;

	close_descriptor(descriptor_t aVal, descriptor_info_t const&aVal2) :
		FId(aVal), FInfo(aVal2)
	{

	}
};

struct create_descriptor
{
	static const NSHARE::CText NAME;
	descriptor_t FId;

	create_descriptor(descriptor_t aVal) :
		FId(aVal)
	{

	}
};
struct destroy_descriptor
{
	static const NSHARE::CText NAME;
	descriptor_t FId;

	destroy_descriptor(descriptor_t aVal) :
		FId(aVal)
	{

	}

};
struct split_info
{
	static const NSHARE::CText NAME;
	static const NSHARE::CText TYPE;
	static const NSHARE::CText SIZE;
	enum eType
	{
		CAN_NOT_SPLIT=0x1<<0,//only whole block
		LIMITED=0x1<<1,
		IS_UNIQUE=0x1<<2,//deprecated

	};
	split_info();

	NSHARE::CFlags<eType> FType;
	size_t FMaxSize;
	void* FCbData;
	size_t (*pMCalculate)(user_data_t const&,void*);

	size_t MDataSize(user_data_t const& aData) const;

	split_info(NSHARE::CConfig const& aConf);
	NSHARE::CConfig MSerialize() const;
	bool MIsValid()const;
};
//received data
typedef data_from_id<program_id_t> new_id_t;
//typedef data_from_id<user_data_t> user_data_id_t;
typedef std::list<user_data_t> user_datas_t;
struct routing_user_data_t
{
	static const NSHARE::CText NAME;
	routing_user_data_t():
		FDesc(-1)//
	{
	}
	descriptor_t FDesc;
	user_datas_t FData;
};
typedef std::list<routing_user_data_t> output_user_data_t;

typedef data_from_id<demand_dgs_t> demands_id_t;//fixme rename
typedef data_from_id<kernel_infos_array_t> kernel_infos_array_id_t;
typedef data_from_id<demand_dgs_for_t> demand_dgs_for_by_id_t;
typedef data_from_id<fail_send_t> fail_send_by_id_t;



template<class _Ty2> inline
bool operator==(const data_from_id<_Ty2>& _Left,
		const data_from_id<_Ty2>& _Right)
{
	return (_Left.FId == _Right.FId && _Left.FVal == _Right.FVal);
}

template<class _Ty2> inline
bool operator!=(const data_from_id<_Ty2>& _Left,
		const data_from_id<_Ty2>& _Right)
{
	return (!(_Left == _Right));
}

template<class _Ty2> inline
bool operator<(const data_from_id<_Ty2>& _Left,
		const data_from_id<_Ty2>& _Right)
{
	return (_Left.FId < _Right.FId
			|| (!(_Right.FId < _Left.FId) && _Left.FVal < _Right.FVal));
}

template<class _Ty2> inline
bool operator>(const data_from_id<_Ty2>& _Left,
		const data_from_id<_Ty2>& _Right)
{
	return (_Right < _Left);
}

template<class _Ty2> inline
bool operator<=(const data_from_id<_Ty2>& _Left,
		const data_from_id<_Ty2>& _Right)
{
	return (!(_Right < _Left));
}

template<class _Ty2> inline
bool operator>=(const data_from_id<_Ty2>& _Left,
		const data_from_id<_Ty2>& _Right)
{
	return (!(_Left < _Right));
}

template<class _Ty2> inline data_from_id<_Ty2> make_data_from(
		descriptor_t _Val1, _Ty2 _Val2)
{
	return (data_from_id<_Ty2>(_Val1, _Val2));
}
}
namespace std
{

inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::create_descriptor const& aVal)
{
	return aStream << aVal.FId;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::destroy_descriptor const& aVal)
{
	return aStream << aVal.FId;
}
template<class _T>
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::data_from_id<_T> const& aVal)
{
	return aStream << "Id=" << aVal.FId<<"Route :"<<aVal.FRoute << "; " << aVal.FVal;
}
template<class _T>
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::routing_user_data_t const& aVal)
{
	return aStream << "Id=" << aVal.FDesc;//<< "; " << aVal.FData;//todo
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::descriptor_info_t const& aVal)
{
	aStream << aVal.FProgramm << std::endl;
	time_t const _time = aVal.FConnectTime/1000;//in ms
	aStream << "Connect Time:" << ctime(&_time)<< " ("
			<< _time << ")" << std::endl;

	aStream << "Info:" << aVal.FInfo;

	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::open_descriptor const& aVal)
{
	return aStream << aVal.FId;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::close_descriptor const& aVal)
{
	return aStream << aVal.FId;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::split_info const& aVal)
{
	return aStream << aVal.FMaxSize<<" Type="<<aVal.FType.MGetMask();
}


// pair TEMPLATE OPERATORS
template<class _T> inline
void swap(NUDT::data_from_id<_T>& aLft, NUDT::data_from_id<_T>& aRht)
{	// swap _Left and _Right pairs
	aLft.swap(aRht);
}
}
#endif /* KERNEL_TYPE_H_ */
