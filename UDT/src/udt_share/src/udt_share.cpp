/*
 * udt_share.cpp
 *
 *  Created on: 30.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
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
static programm_id_t g_id;

extern UDT_SHARE_EXPORT bool fill_dg_head(void* aWhat, size_t aFullSize,
		const programm_id_t& aFrom, bool aIsNeedCrc)
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
	_head->FTime = static_cast<uint32_t>(_time / 1000);
	_head->FTimeMs = static_cast<uint32_t>(_time) - _head->FTime;
	_head->FCounter = ++g_counter;
	_head->FFromUUID = aFrom.FId.FUuid.FVal;

	CHECK_GT(aFullSize, _head->FHeadSize);

	const size_t _data_size = aFullSize - _head->FHeadSize;
	if (_data_size)
	{
		//CHECK_LT(_data_size, std::numeric_limits<uint16_t>::max());
		_head->FDataSize = static_cast<uint32_t>(_data_size);
		if (!aIsNeedCrc)
			_head->FDataCrc = 0;
		else
		{
			_head->FDataCrc = head_t::crc_data_t::sMCalcCRCofBuf(
					_head->MDataBegin(),
					_head->MDataBegin() + _head->FDataSize);
			DCHECK_EQ(_head->FDataCrc,
					head_t::crc_data_t::sMCalcCRCofBuf(_head->MDataBegin(),
							_head->MDataBegin() + _head->FDataSize));
		}
	}
	else
	{
		VLOG(2) << "There is not data";
		_head->FDataCrc = 0;
		_head->FDataSize = 0;
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
extern UDT_SHARE_EXPORT bool deserialize(user_data_t& _user,
		const user_data_dg_t* aP, NSHARE::IAllocater* aBufferAlloc)
{
	VLOG(2) << "Handle user data " << *aP;
	CHECK_EQ(
			sizeof(user_data_dg_t) + aP->FUserHeader.FDataSize
					+ aP->FUserHeader.FUUIDsLen //+ aP->FUserHeader.FName
					+ aP->FUserHeader.FProtocolName,
			aP->FDataSize + aP->FHeadSize);

	//user_data_t _data;
	_user.FDataId.FFrom.FUuid = aP->FUserHeader.FUUIDFrom;
	_user.FDataId.FPacketNumber = aP->FUserHeader.FNumber;
	_user.FDataId.FRawProtocolNumber = aP->FUserHeader.FRawNumber;

	_user.FDataId.FSplit.FIsLast = aP->FUserHeader.FIsLast;
	_user.FDataId.FSplit.FCounter = aP->FUserHeader.FSplitCounter;
	_user.FDataId.FSplit.FCoefficient = aP->FUserHeader.FSplitCoefficient;

	size_t _offset = 0;
	if (aP->FUserHeader.FUUIDsLen)
	{
		uint64_t const *_begin_uuid = (uint64_t const *) aP->MUserDataBegin();
		uint64_t const *_end_uuid = (uint64_t const *) (aP->MUserDataBegin()
				+ aP->FUserHeader.FUUIDsLen);
		CHECK(
				(unsigned )(_end_uuid - _begin_uuid)
						== (aP->FUserHeader.FUUIDsLen / sizeof(uint64_t)));

		for (; _begin_uuid != _end_uuid; ++_begin_uuid)
			_user.FDataId.FUUIDTo.MGet().push_back(
					NSHARE::uuid_t(*_begin_uuid));
		_offset += aP->FUserHeader.FUUIDsLen;
	}
//	if (aP->FUserHeader.FName)
//	{
//		if (aP->FUserHeader.FUUIDsLen)
//			_user.FDataId.FFrom.FName = NSHARE::CText(
//					(const utf8*) (aP->MUserDataBegin()) + _offset);
//		//		else
//		//			_user.FDataId.FDestName.push_back(
//		//					NSHARE::CText((utf8 const*) aP->MUserDataBegin() + _offset));
//	}
	//_offset += aP->FUserHeader.FName;
	if (aP->FUserHeader.FProtocolName)
		_user.FDataId.FProtocol = NSHARE::CText(
				(const utf8*) (aP->MUserDataBegin()) + _offset);

	_offset += aP->FUserHeader.FProtocolName;
	//allocate memory
	//	data_t::const_iterator const _p(
	//			(data_t::value_type const*) ((utf8 const*) aP->MUserDataBegin()
	//					+ _offset));
	{
		const NSHARE::CBuffer::value_type* _data_begin =
				(const NSHARE::CBuffer::value_type*) (&aP->MUserDataBegin()[aP->FUserHeader.FUUIDsLen
						+ aP->FUserHeader.FProtocolName/* + aP->FUserHeader.FName*/]);
		VLOG_IF(5,aP->FUserHeader.FDataSize<100)
														<< NSHARE::print_buffer_t<
																NSHARE::CBuffer::value_type const*>(
																_data_begin,
																_data_begin
																		+ aP->FUserHeader.FDataSize);
		NSHARE::CBuffer _data(
				(!_user.FDataId.FSplit.MIsSplited()) ? aBufferAlloc : NULL,	//dead lock protection. using aBufferAlloc only for not split packet
				_data_begin,	//
				_data_begin + aP->FUserHeader.FDataSize);
		_data.MMoveTo(_user.FData);
	}
	_offset += aP->FUserHeader.FDataSize;
	//paranoid check
	CHECK_EQ(_user.FData.size(), aP->FUserHeader.FDataSize);
	//paranoid check
	CHECK_EQ(_offset, (aP->FUserHeader.FDataSize + aP->FUserHeader.FUUIDsLen
	/*+ aP->FUserHeader.FName*/+ aP->FUserHeader.FProtocolName));
	VLOG(5) << _user;
	return true;
}
extern size_t UDT_SHARE_EXPORT get_full_size(user_data_t const& _id)
{
	const size_t _name_protocol = _id.FDataId.FProtocol.length_code();
	const size_t _len_uuids =
			_id.FDataId.FUUIDTo.MIs()
					&& !_id.FDataId.FUUIDTo.MGetConst().empty() ?
					sizeof(uint64_t) * _id.FDataId.FUUIDTo.MGetConst().size() :
					0;
	const size_t _name_from_len = 0;// _id.FDataId.FFrom.FName.length_code(); //utf8 len
	const size_t _size = _id.FData.size();
	const size_t full_size = sizeof(user_data_dg_t) //
	+ (_name_from_len ? _name_from_len + 1 : 0) //
			+ (_name_protocol ? _name_protocol + 1 : 0) //
			+ _len_uuids //
			+ _size;
	return full_size;
}
extern size_t UDT_SHARE_EXPORT get_full_size(user_data_info_t const& aData)
{
	const size_t _name_from_len = 0; //aData.FFrom.FName.length_code();
	const size_t _name_protocol = aData.FProtocol.length_code();
	const size_t _len_uuids =
			aData.FUUIDTo.MIs() && !aData.FUUIDTo.MGetConst().empty() ?
					sizeof(uint64_t) * aData.FUUIDTo.MGetConst().size() : 0;

	return sizeof(user_data_header_t) + //
			(_name_from_len ? _name_from_len + 1 : 0) + //
			(_name_protocol ? _name_protocol + 1 : 0) + //
			_len_uuids;
}
extern UDT_SHARE_EXPORT bool serialize(NSHARE::CBuffer* _data,
		const user_data_info_t& _id, size_t aSize)
{
	uuids_t const& _uuids_to = _id.FUUIDTo.MGetConst();
	const size_t _name_protocol = _id.FProtocol.length_code();
	const size_t _uuids_size = _uuids_to.size() * sizeof(uint64_t);
	const size_t _name_from_len = 0; //_id.FFrom.FName.length_code(); //utf8 len
	const size_t full_size = sizeof(user_data_dg_t) //
	+ (_name_from_len ? _name_from_len + 1 : 0) //
			+ (_name_protocol ? _name_protocol + 1 : 0) //
			+ _uuids_size;

	//allocate memory
	_data->resize(full_size);
	char * const _begin = (char*) &_data->front();

	CHECK_EQ(full_size, _data->size());

	//fill dg
	user_data_dg_t * _user_data = new (_begin) user_data_dg_t();
	CHECK_NOTNULL(_user_data);
	_user_data->FUserHeader.FUUIDsLen = _uuids_size;

	_user_data->FUserHeader.FUUIDFrom = _id.FFrom.FUuid.FVal;

	_user_data->FUserHeader.FIsLast = _id.FSplit.FIsLast;
	_user_data->FUserHeader.FSplitCounter = _id.FSplit.FCounter;
	_user_data->FUserHeader.FSplitCoefficient = _id.FSplit.FCoefficient;

	_user_data->FUserHeader.FRawNumber = _id.FRawProtocolNumber;
//	if (_name_from_len)
//		_user_data->FUserHeader.FName = _name_from_len + 1;

	if (_name_protocol)
		_user_data->FUserHeader.FProtocolName = _name_protocol + 1;

	_user_data->FUserHeader.FNumber = _id.FPacketNumber;
	_user_data->FUserHeader.FDataSize = aSize;

	//fill dynamic data
	char* _p = (char*) _user_data->MUserDataBegin();

	if (_uuids_size)
	{
		uuids_t::const_iterator _it = _uuids_to.begin();

		for (; _it != _uuids_to.end(); ++_it)
		{
			memcpy(_p, &(*_it), sizeof(uint64_t));
			_p += sizeof(uint64_t);
		}
	}
	if (_name_from_len)
	{
		memcpy(_p, _id.FFrom.FName.c_str(), _name_from_len);
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

	//calc CRC and fill it
	fill_dg_head(_begin, full_size + aSize, get_my_id(), false);

	//paranoid check
	CHECK_EQ(full_size + aSize,
			(size_t ) (reinterpret_cast<user_data_dg_t*>(_begin)->FHeadSize
					+ reinterpret_cast<user_data_dg_t*>(_begin)->FDataSize));
	return true;
}
extern UDT_SHARE_EXPORT size_t serialize(NSHARE::CBuffer* _data,
		const user_data_t& _id, bool aIsNeedCrc)
{
	CHECK(_id.FDataId.FUUIDTo.MIs());
	//CHECK(!_id.FDataId.FFrom.FName.empty());

	uuids_t const& _uuids_to = _id.FDataId.FUUIDTo.MGetConst();
	const size_t _name_protocol = _id.FDataId.FProtocol.length_code();
	const size_t _uuids_size = _uuids_to.size() * sizeof(uint64_t);
	const size_t _name_from_len = 0; // _id.FDataId.FFrom.FName.length_code(); //utf8 len
	const size_t _size = _id.FData.size();
	const size_t full_size = sizeof(user_data_dg_t) //
	+ (_name_from_len ? _name_from_len + 1 : 0) //
			+ (_name_protocol ? _name_protocol + 1 : 0) //
			+ _uuids_size //
			+ _size;
	//allocate memory
	_data->resize(full_size);
	char * const _begin = (char*) &_data->front();

	CHECK_EQ(full_size, _data->size());

	//fill dg
	user_data_dg_t * _user_data = new (_begin) user_data_dg_t();
	CHECK_NOTNULL(_user_data);
	_user_data->FUserHeader.FUUIDsLen = _uuids_size;

	_user_data->FUserHeader.FUUIDFrom = _id.FDataId.FFrom.FUuid.FVal;

//		if (_name_from_len)
//			_user_data->FUserHeader.FName = _name_from_len + 1;

	if (_name_protocol)
		_user_data->FUserHeader.FProtocolName = _name_protocol + 1;

	_user_data->FUserHeader.FRawNumber = _id.FDataId.FRawProtocolNumber;
	_user_data->FUserHeader.FNumber = _id.FDataId.FPacketNumber;

	_user_data->FUserHeader.FDataSize = _size;

	//fill dynamic data
	char* _p = (char*) _user_data->MUserDataBegin();

	if (_uuids_size)
	{
		uuids_t::const_iterator _it = _uuids_to.begin();

		for (; _it != _uuids_to.end(); ++_it)
		{
			memcpy(_p, &(*_it), sizeof(uint64_t));
			_p += sizeof(uint64_t);
		}
	}
	if (_name_from_len)
	{
		memcpy(_p, _id.FDataId.FFrom.FName.c_str(), _name_from_len);
		_p += _name_from_len;
		*_p++ = '\0';
	}
	if (_name_protocol)
	{
		memcpy(_p, _id.FDataId.FProtocol.c_str(), _name_protocol);
		_p += _name_protocol;
		*_p++ = '\0';
	}
	memcpy(_p, _id.FData.ptr_const(), _size);
	_p += _size;

	//paranoid check
	CHECK_EQ(full_size, static_cast<size_t>(_p - _begin));

	//calc CRC and fill it
	fill_dg_head(_begin, full_size, get_my_id(), aIsNeedCrc);

	//paranoid check
	CHECK_EQ(full_size,
			(size_t )(reinterpret_cast<user_data_dg_t*>(_begin)->FHeadSize
					+ reinterpret_cast<user_data_dg_t*>(_begin)->FDataSize));

	VLOG(2) << "DG UserData " << *reinterpret_cast<user_data_dg_t*>(_begin);
	return full_size;
}


extern UDT_SHARE_EXPORT programm_id_t const& get_my_id()
{
	CHECK(g_is_inited);
	return g_id;
}
extern UDT_SHARE_EXPORT bool is_id_initialized()
{
	return g_is_inited;
}
extern UDT_SHARE_EXPORT int init_id(char const *aName, eType aType, const NSHARE::version_t& aVer)
{
	using namespace std;
	if (g_is_inited)
		return 0;
	VLOG(2)<<"Name="<<aName<<" aType="<<aType;
	if (!NSHARE::CRegistration::sMIsValid(aName) || !NSHARE::CRegistration(aName).MIsName())
	{
		LOG(ERROR)<<"Name '"<<aName<<"' is not valid";
		return -1;
	}

	g_is_inited = true;
	g_id.FKernelVersion = aVer;
	g_id.FTime = g_time;
	g_id.FId.FUuid = NSHARE::get_programm_uuid(NSHARE::CText(aName));
	g_id.FPid = NSHARE::CThread::sProcessId();
	g_id.FId.FName = aName;
	g_id.FPath = NSHARE::get_path(); //FIXME + programm name
	g_id.FType = aType;
	return 0;
}
}
