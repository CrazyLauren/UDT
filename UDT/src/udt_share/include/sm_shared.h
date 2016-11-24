/*
 * sm_shared.h
 *
 *  Created on: 07.07.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SM_SHARED_H_
#define SM_SHARED_H_
namespace NUDT
{
enum eDataType
{
	E_SM_INFO=1,
	E_SM_DATA,
	E_SM_DATA_INFO,
};
SHARED_PACKED( struct flag_mask_t
{
	uint32_t FCounter:30;
	uint32_t FType:2;//eDataType
});
COMPILE_ASSERT(sizeof(flag_mask_t) == sizeof(uint32_t), InvalidSizeOfFlasgMask);
inline void fill_header(NSHARE::CBuffer::pointer  aTo,user_data_info_t const& aData,NSHARE::CBuffer::offset_pointer_t aOffset)
{
	const size_t _name_from_len =0;//aData.FFrom.FName.length_code();//Name was deprecated
	const size_t _name_protocol =aData.FProtocol.length_code();
	const size_t _len_uuids =
			aData.FUUIDTo.MIs() && !aData.FUUIDTo.MGetConst().empty() ?
					sizeof(uint64_t) * aData.FUUIDTo.MGetConst().size() : 0;

	const size_t _begin_size = sizeof(user_data_header_t) //
	+ (_name_from_len ? _name_from_len + 1 : 0) //
			+ (_name_protocol ? _name_protocol + 1 : 0) //
			+ _len_uuids;

	CHECK_EQ(_begin_size,get_full_size(aData));


	char* const _ptr_head = (char*) (aTo);

	//fill dg
	user_data_header_t* _user_data = new (_ptr_head) user_data_header_t();
	_user_data->FUUIDsLen = _len_uuids;
	_user_data->FUUIDFrom = aData.FFrom.FUuid.FVal;

//	if (_name_from_len)
//		_user_data->FName = _name_from_len + 1;

	if (_name_protocol)
		_user_data->FProtocolName = _name_protocol + 1;

	_user_data->FNumber = aData.FPacketNumber;
	_user_data->FRawNumber=aData.FRawProtocolNumber;

	_user_data->FDataSize = aOffset;

	//fill dynamic data
	char* _p = (char*) (_user_data+1);
	if (_len_uuids)
	{
		memcpy(_p, &aData.FUUIDTo.MGetConst().front().FVal, _len_uuids);
		_p += _len_uuids;
	}
	if (_name_from_len)
	{
		memcpy(_p, aData.FFrom.FName.c_str(), _name_from_len);
		_p += _name_from_len;
		*_p++ = '\0';
	}
	if (_name_protocol)
	{
		memcpy(_p, aData.FProtocol.c_str(), _name_protocol);
		_p += _name_protocol;
		*_p++ = '\0';
	}

	CHECK_EQ((size_t)((char*)_p-(char*) aTo),_begin_size);
}
NSHARE::CBuffer::offset_pointer_t get_sm_user_data(
		NSHARE::CBuffer::const_pointer  aFrom, user_data_info_t* aTo)
{
	user_data_header_t const* _header =
			(user_data_header_t const*) aFrom;
	char const*const _user_data=(char*)(_header+1);
	uint64_t const *_begin_uuid = (uint64_t const *) _user_data;
	uint64_t const *_end_uuid = (uint64_t const *) (_user_data
			+ _header->FUUIDsLen);

	CHECK_EQ((unsigned )(_end_uuid - _begin_uuid),
			(_header->FUUIDsLen / sizeof(uint64_t)));

//	uint64_t const _my_uuid = CCustomer::sMGetInstance().MGetID().FId.FUuid.FVal;

	for (; _begin_uuid != _end_uuid; ++_begin_uuid)
		aTo->FUUIDTo.MGet().push_back(NSHARE::uuid_t(*_begin_uuid));

//	if (_header->FName)
//		aTo->FFrom.FName = NSHARE::CText(
//				(NSHARE::utf8 const*) (_user_data + _header->FUUIDsLen));
//	else
	aTo->FFrom.FName.clear();
	aTo->FFrom.FUuid = _header->FUUIDFrom;

	if(_header->FProtocolName)
		aTo->FProtocol = NSHARE::CText(
				(NSHARE::utf8 const*) (_user_data + _header->FUUIDsLen
				/*+ _header->FName*/));
	aTo->FPacketNumber = _header->FNumber;
	aTo->FRawProtocolNumber=_header->FRawNumber;
	LOG_IF(WARNING,!_header->FDataSize) << "No data sent by "
														<< aTo->FFrom;
	return _header->FDataSize;
}
}
#endif /* SM_SHARED_H_ */
