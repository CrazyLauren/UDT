// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * udt_share.cpp
 *
 *  Created on: 30.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <fdir.h>
#include <programm_id.h>
#include <time.h>
#include <internel_protocol.h>
#include <shared_types.h>
#include <udt_share.h>
#include <CParserFactory.h>

namespace NUDT
{
using namespace NSHARE;
static uint32_t g_counter = 0;
static time_t const g_time = ::time(NULL);
static bool g_is_inited = false;
static program_id_t g_id;

extern UDT_SHARE_EXPORT bool fill_dg_head(void* aWhat, size_t aFullSize,
		const program_id_t& aFrom, bool aIsNeedCrc)
{
	CHECK_EQ(
			head_t::crc_head_t::sMCalcNextCRC(head_t::crc_head_t::Polynom,
					head_t::crc_head_t::Polynom), 0);
	CHECK_GE(aFullSize, sizeof(head_t));
	head_t * _head = reinterpret_cast<head_t *>(aWhat);
	CHECK(_head->FVersion.MIsExist());
	CHECK_EQ((unsigned )_head->FHeadSize, sizeof(head_t));

	uint64_t const _time = NSHARE::get_unix_time();
	CHECK_LE((int long long)(_time/1000-time(0)),1)
			<< "Invalid rval of get_unix_time(): " << _time << "; time:"
			<< time(NULL);
	_head->MSetTime (static_cast<uint32_t>(_time / 1000));
	_head->MSetTimeMs (static_cast<uint32_t>(_time) - _head->MGetTime());
	_head->MSetCounter (++g_counter);
	_head->MSetFromUUID ( aFrom.FId.FUuid.FVal);

	CHECK_GT(aFullSize, _head->FHeadSize);

	const size_t _data_size = aFullSize - _head->FHeadSize;
	if (_data_size)
	{
		//CHECK_LT(_data_size, std::numeric_limits<uint16_t>::max());
		_head->MSetDataSize( static_cast<uint32_t>(_data_size));
		if (!aIsNeedCrc)
			_head->MSetDataCRC(0);
		else
		{
			_head->MSetDataCRC (head_t::crc_data_t::sMCalcCRCofBuf(
					_head->MDataBegin(),
					_head->MDataBegin() + _head->MGetDataSize()));
			DCHECK_EQ(_head->MGetDataCRC(),
					head_t::crc_data_t::sMCalcCRCofBuf(_head->MDataBegin(),
							_head->MDataBegin() + _head->MGetDataSize()));
		}
	}
	else
	{
		VLOG(2) << "There is not data";
		_head->MSetDataCRC( 0);
		_head->MSetDataSize( 0);
	}
	uint8_t* _head_end = (uint8_t*) _head + _head->FHeadSize - 1; //-1 without crc
	VLOG(2) << "Crc begin " << (int) _head->FCrcHead;
	_head->FCrcHead = head_t::crc_head_t::sMCalcCRCofBuf((uint8_t*) _head,
			_head_end);
	VLOG(2) << "Crc head " << (int) _head->FCrcHead;
	CHECK_EQ(
			(unsigned )head_t::crc_head_t::sMCalcCRCofBuf((uint8_t* )_head,
					(uint8_t* )_head + _head->FHeadSize),
			head_t::crc_head_t::sMCheckingConstant());
	return true;
}

static std::pair<size_t,size_t> deserialize_dg_head(user_data_info_t &_user,NSHARE::CBuffer::const_pointer  aFrom,const user_data_header_t& _from,bool aIsValidEndian)
{
	VLOG(2) << "Handle user data " << _from;
	const size_t _events_size = _from.FEventList
			* sizeof(demand_dg_t::event_handler_t);
	const size_t _dest_size = _from.FDestination * sizeof(uuids_t::value_type);
	const size_t _registrator_size = _from.FRegistrators
			* sizeof(uuids_t::value_type);
	const size_t _routing_size = _from.FRouting * sizeof(uuids_t::value_type);
	//user_data_t _data;
	_user.FRouting.FFrom.FUuid = _from.FUUIDFrom;
	_user.FPacketNumber = _from.FNumber;
	_user.FDataOffset = _from.FDataOffset;
	_user.FSplit.FIsLast = _from.FFlags.FIsLast;
	_user.FSplit.FCounter = _from.FSplitCounter;
	_user.FSplit.FCoefficient = _from.FSplitCoefficient;
	_user.FWhat.FVersion.FMajor = _from.FMajor;
	_user.FWhat.FVersion.FMinor = _from.FMinor;
	memcpy(_user.FWhat.FMessageHeader,&_from.FType,sizeof(_user.FWhat.FMessageHeader));

	_user.FEndian = (NSHARE::eEndian) (_from.FFlags.FEndian);
	size_t _offset = sizeof(user_data_header_t);
	if (_events_size)
	{
		const demand_dg_t::event_handler_t* _p_begin =
				(const demand_dg_t::event_handler_t*) ((aFrom + _offset));
		const demand_dg_t::event_handler_t* _p_end = _p_begin
				+ _from.FEventList;
		for (; _p_begin != _p_end; ++_p_begin)
			_user.FEventsList.push_back(aIsValidEndian?*_p_begin:swap_endian(*_p_begin));
	}
	_offset += _events_size;
	typedef uint64_t _uuid_t;
	if (_dest_size)
	{
		const _uuid_t* _p_begin =
				(const _uuid_t*) ((aFrom + _offset));
		const _uuid_t* _p_end = _p_begin + _from.FDestination;
		for (; _p_begin != _p_end; ++_p_begin)
			_user.FDestination.push_back(uuids_t::value_type (aIsValidEndian?*_p_begin:swap_endian(*_p_begin)));
	}
	_offset += _dest_size;
	if (_routing_size)
	{
		const _uuid_t* _p_begin =
				(const _uuid_t*) ((aFrom + _offset));
		const _uuid_t* _p_end = _p_begin + _from.FRouting;
		for (; _p_begin != _p_end; ++_p_begin)
			_user.FRouting.push_back(uuids_t::value_type (aIsValidEndian?*_p_begin:swap_endian(*_p_begin)));
	}
	_offset += _routing_size;
	if (_registrator_size)
	{
		const _uuid_t* _p_begin =
				(const _uuid_t*) ((aFrom + _offset));
		const _uuid_t* _p_end = _p_begin + _from.FRegistrators;
		for (; _p_begin != _p_end; ++_p_begin)
			_user.FRegistrators.push_back(uuids_t::value_type (aIsValidEndian?*_p_begin:swap_endian(*_p_begin)));
	}
	_offset += _registrator_size;
	//	if (_from.FName)
	//	{
	//		if (_from.FUUIDsLen)
	//			_user.FDataId.FFrom.FName = NSHARE::CText(
	//					(const utf8*) (aP->MUserDataBegin()) + _offset);
	//		//		else
	//		//			_user.FDataId.FDestName.push_back(
	//		//					NSHARE::CText((utf8 const*) aFrom + _offset));
	//	}
	//_offset += _from.FName;
	if (_from.FProtocolName)
	{
		_user.FProtocol = NSHARE::CText((const utf8*) ((aFrom + _offset)));
	}
	_offset += _from.FProtocolName;
	return std::pair<size_t, size_t>(_offset, _from.FDataSize);
}

extern UDT_SHARE_EXPORT std::pair<size_t,size_t> deserialize_dg_head(
		user_data_info_t &_user,NSHARE::CBuffer::const_pointer  aFrom)
{
	user_data_header_t const& _from =
			*(user_data_header_t const*) aFrom;

	return deserialize_dg_head( _user,aFrom, _from,true);
}
extern UDT_SHARE_EXPORT bool deserialize(user_data_t& _user,
		const user_data_dg_t* aP, NSHARE::IAllocater* aBufferAlloc)
{
//	CHECK_EQ(sizeof(user_data_dg_t) + _from.FDataSize //
//			+ _events_size//
//			+ _dest_size//
//			+ _routing_size//
//			//+ _from.FName
//			+ _from.FProtocolName//
//			,//==
//			aP->FDataSize + aP->FHeadSize);
	const user_data_header_t _header = aP->MGetUserDataHeader();
	NSHARE::CBuffer::const_pointer const  _begin=(NSHARE::CBuffer::const_pointer)aP;
	const size_t _size_dg_header=sizeof(user_data_dg_t)-sizeof(user_data_header_t);
	size_t _offset = _size_dg_header;

	

	const std::pair<size_t, size_t> _sizes = deserialize_dg_head(_user.FDataId,
			_begin+_offset,_header,aP->MIsValidEndian());

	_offset +=_sizes.first;
	//paranoid check
	CHECK_EQ(_sizes.first + _sizes.second+_size_dg_header,
			aP->MGetDataSize()+ aP->FHeadSize);

	LOG_IF(WARNING,!_sizes.second) << "No data sent by "
														<< _user.FDataId.FRouting.FFrom;

	if(_sizes.second)
	{
		VLOG_IF(5, _header.FDataSize<100)
														<< NSHARE::print_buffer_t<
														NSHARE::CBuffer::const_pointer>(
																		_begin+_offset,//
																		_begin+_offset//
																		+ _header.FDataSize);
		const bool _use_allocator=!_user.FDataId.FSplit.MIsSplited();//dead lock protection. using aBufferAlloc only for not split packet
		
		NSHARE::CBuffer _data(				
						_begin+_offset,	//
						_begin+_offset + _header.FDataSize,
			    (_use_allocator)?-1:0,
			    (_use_allocator)? aBufferAlloc : NULL
				     );
		_data.MMoveTo(_user.FData);
	}
	_offset +=_sizes.second;

	//paranoid check
	CHECK_EQ(_user.FData.size(), _header.FDataSize);
	CHECK_EQ(_offset, aP->MGetDataSize() +aP->FHeadSize);


	VLOG(5) << _user;
	return true;
}
extern size_t UDT_SHARE_EXPORT get_full_size(user_data_t const& _id)
{
	return _id.FData.size()+get_full_size(_id.FDataId);
}

extern size_t UDT_SHARE_EXPORT fill_header(NSHARE::CBuffer::pointer _begin ,
		const user_data_info_t& _id, size_t aSize)
{
	VLOG(3)<<"Serialize size="<<aSize<<" "<<_id;
	std::vector<demand_dg_t::event_handler_t> const& _events = _id.FEventsList;
	uuids_t const& _dest = _id.FDestination;
	uuids_t const& _routing = _id.FRouting;
	uuids_t const& _reg = _id.FRegistrators;

	const size_t _name_protocol = _id.FProtocol.length_code();

	const size_t _events_size = _events.size()
			* sizeof(demand_dg_t::event_handler_t);
	const size_t _dest_size = _dest.size() * sizeof(uuids_t::value_type);
	const size_t _reg_size = _reg.size() * sizeof(uuids_t::value_type);
	const size_t _routing_size = _routing.size() * sizeof(uuids_t::value_type);

	const size_t _name_from_len = 0; //_id.FFrom.FName.length_code(); //utf8 len
	const size_t full_size = sizeof(user_data_header_t) //
	+ (_name_from_len ? _name_from_len + 1 : 0) //
			+ (_name_protocol ? _name_protocol + 1 : 0) //
			+ _events_size+_dest_size+_reg_size+_routing_size;

	if(!_begin)
		return full_size;//calculate size

	//fill dg
	user_data_header_t * _user_data = new (_begin) user_data_header_t();
	CHECK_NOTNULL(_user_data);
	_user_data->FEventList =static_cast<uint8_t>(_events.size());
	_user_data->FDestination = static_cast<uint8_t>(_dest.size());
	_user_data->FRouting = static_cast<uint8_t>(_routing.size());
	_user_data->FRegistrators = static_cast<uint8_t>(_reg.size());


	_user_data->FUUIDFrom = _id.FRouting.FFrom.FUuid.FVal;

	_user_data->FFlags.FIsLast = _id.FSplit.FIsLast;
	_user_data->FSplitCounter = _id.FSplit.FCounter;
	_user_data->FSplitCoefficient = _id.FSplit.FCoefficient;

	_user_data->FDataOffset = static_cast<uint32_t>(_id.FDataOffset);
	_user_data->FMajor=_id.FWhat.FVersion.FMajor;
	_user_data->FMinor=_id.FWhat.FVersion.FMinor;
	memcpy(&_user_data->FType,_id.FWhat.FMessageHeader,sizeof(_id.FWhat.FMessageHeader));


	_user_data->FFlags.FEndian=_id.FEndian;
//	if (_name_from_len)
//		_user_data->FName = _name_from_len + 1;

	if (_name_protocol)
		_user_data->FProtocolName = static_cast<uint8_t>(_name_protocol) + 1;

	_user_data->FNumber = _id.FPacketNumber;
	_user_data->FDataSize = static_cast<uint32_t>(aSize);

	//fill dynamic data
	NSHARE::CBuffer::pointer _p = _begin;
	_p+=sizeof(user_data_header_t);

	if (_events_size)
	{
		const size_t _it_size=sizeof(demand_dg_t::event_handler_t);

		std::vector<demand_dg_t::event_handler_t>::const_iterator _it =
				_events.begin(), _it_end(_events.end());

		for (; _it != _it_end; ++_it)
		{
			memcpy(_p, &(*_it), _it_size);
			_p += _it_size;
		}
	}
	if (_dest_size)
	{
		const size_t _it_size=sizeof(uuids_t::value_type);

		uuids_t::const_iterator _it =
				_dest.begin(), _it_end(_dest.end());

		for (; _it != _it_end; ++_it)
		{
			memcpy(_p, &(*_it), _it_size);
			_p += _it_size;
		}
	}
	if (_routing_size)
	{
		const size_t _it_size=sizeof(uuids_t::value_type);

		uuids_t::const_iterator _it =
				_routing.begin(), _it_end(_routing.end());

		for (; _it != _it_end; ++_it)
		{
			memcpy(_p, &(*_it), _it_size);
			_p += _it_size;
		}
	}
	if (_reg_size)
	{
		const size_t _it_size=sizeof(uuids_t::value_type);

		uuids_t::const_iterator _it =
				_reg.begin(), _it_end(_reg.end());

		for (; _it != _it_end; ++_it)
		{
			memcpy(_p, &(*_it), _it_size);
			_p += _it_size;
		}
	}
	if (_name_from_len)
	{
		memcpy(_p, _id.FRouting.FFrom.FName.c_str(), _name_from_len);
		_p += _name_from_len;
		*_p++ = '\0';
	}
	if (_name_protocol)
	{
		memcpy(_p, _id.FProtocol.c_str(), _name_protocol);
		_p += _name_protocol;
		*_p++ = '\0';
	}

	//paranoid check
	CHECK_EQ(full_size, static_cast<size_t>(_p - _begin));
	VLOG(4)<<"Data "<<*_user_data;
	return full_size;
}
extern size_t UDT_SHARE_EXPORT get_full_size(user_data_info_t const& aData)
{
	return fill_header(NULL,aData,0);
}

extern UDT_SHARE_EXPORT bool serialize(NSHARE::CBuffer* _data,
		const user_data_info_t& _id, size_t aSize)
{
	const size_t _size_dg_header=sizeof(user_data_dg_t)-sizeof(user_data_header_t);
	const size_t full_size =get_full_size(_id)+_size_dg_header;
	//allocate memory
	_data->resize(full_size);
	CHECK_EQ(full_size, _data->size());

	NSHARE::CBuffer::pointer const _begin =(NSHARE::CBuffer::pointer) &_data->front();
	NSHARE::CBuffer::pointer _p=_begin;
	new(_p) user_data_dg_t;
	_p+=_size_dg_header;
	_p+=fill_header(_p,_id,aSize);

	//paranoid check
	CHECK_EQ(full_size, static_cast<size_t>(_p - _begin));

	//calc CRC and fill it
	fill_dg_head(_begin, full_size + aSize, get_my_id(), false);

	//paranoid check
	CHECK_EQ(full_size + aSize,
			(size_t ) (reinterpret_cast<user_data_dg_t*>(_begin)->FHeadSize
					+ reinterpret_cast<user_data_dg_t*>(_begin)->MGetDataSize()));
	return true;
}

extern UDT_SHARE_EXPORT size_t serialize(NSHARE::CBuffer* _data,
		const user_data_t& _id, bool aIsNeedCrc)
{
	const size_t _size = _id.FData.size();
	const size_t full_size = get_full_size(_id) + _size;
	//allocate memory
	_data->resize(full_size);
	CHECK_EQ(full_size, _data->size());

	NSHARE::CBuffer::pointer const _begin = (NSHARE::CBuffer::pointer) &_data->front();
	NSHARE::CBuffer::pointer _p=_begin;
	new(_p) user_data_dg_t;
	_p+=sizeof(user_data_dg_t)-sizeof(user_data_header_t);
	_p+=fill_header(_p,_id.FDataId,_size);
	_p+=_size;

	//paranoid check
	CHECK_EQ(full_size, static_cast<size_t>(_p - _begin));

	//calc CRC and fill it
	fill_dg_head(_begin, full_size, get_my_id(), aIsNeedCrc);

	//paranoid check
	CHECK_EQ(full_size,
			(size_t )(reinterpret_cast<user_data_dg_t*>(_begin)->FHeadSize
					+ reinterpret_cast<user_data_dg_t*>(_begin)->MGetDataSize()));

	VLOG(2) << "DG UserData " << *reinterpret_cast<user_data_dg_t*>(_begin);
	return full_size;
}


extern UDT_SHARE_EXPORT program_id_t const& get_my_id()
{
	CHECK(g_is_inited);
	return g_id;
}
extern UDT_SHARE_EXPORT bool is_id_initialized()
{
	return g_is_inited;
}
extern UDT_SHARE_EXPORT int init_id(char const *aName, eProgramType aType, const NSHARE::version_t& aVer)
{
	using namespace std;
	if (g_is_inited)
		return 0;
	VLOG(2)<<"Name="<<aName<<" aType="<<aType;
	if (!NSHARE::CProgramName::sMIsValid(aName) || !NSHARE::CProgramName(aName).MIsName())
	{
		LOG(ERROR)<<"Name '"<<aName<<"' is not valid";
		return -1;
	}

	g_is_inited = true;
	g_id.FVersion = aVer;
	g_id.FTime = static_cast<unsigned>(g_time);
	g_id.FId.FUuid = NSHARE::get_programm_uuid(NSHARE::CText(aName));
	g_id.FPid = NSHARE::CThread::sProcessId();
	g_id.FId.FName = aName;
	g_id.FPath = NSHARE::get_path(); //FIXME + programm name
	g_id.FType = aType;
	return 0;
}
}
