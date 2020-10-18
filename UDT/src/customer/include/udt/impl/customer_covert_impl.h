/*
 * customer_conver_impl.h
 *
 *  Created on: 06.09.2020
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2020  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CUSTOMER_CONVER_IMPL_H_
#define CUSTOMER_CONVER_IMPL_H_
#include <deftype>
#include <customer.h>
#include <UDT/customer/receive_data_info_c.h>
#include <UDT/customer/customer_c_type.h>

namespace NUDT
{
template<class TTo, class TFrom> void convert(TTo* aTo, TFrom const&);
template<> void convert<uint64_t, NSHARE::uuid_t>
(uint64_t *aTo, NSHARE::uuid_t const& aWhat)
{
	*aTo = aWhat.FVal;
}
template<> void convert<NSHARE::uuid_t, uint64_t>
(NSHARE::uuid_t *aTo, uint64_t const& aWhat)
{
	aTo->FVal = aWhat;
}
template<> void convert<NSHARE::version_t, version_c_t>
(NSHARE::version_t *aTo, version_c_t const& aWhat)
{
	*aTo = NSHARE::version_t(aWhat.major, aWhat.minor, aWhat.release);
}
template<> void convert<NSHARE::CBuffer, buffer_c_t>
(NSHARE::CBuffer *aTo, buffer_c_t const& aWhat)
{
	NSHARE::CBuffer _buffer(
			NUDT::CCustomer::sMGetInstance().MGetNewBuf(aWhat.size));
	if (!_buffer.empty())
	{
		memcpy(_buffer.ptr(), aWhat.ptr, aWhat.size);
	}
	else
		DLOG(ERROR) << "Requested size is " << aWhat.size;
	*aTo = _buffer;
}
template<> void convert<buffer_c_t, NSHARE::CBuffer>
(buffer_c_t *aTo, NSHARE::CBuffer const& aWhat)
{
	if (aWhat.empty())
	{
		aTo->ptr = NULL;
		aTo->size = 0;
	}
	else
	{
		aTo->ptr = (char const*) aWhat.ptr_const();
		aTo->size = aWhat.size();
	}
}

template<> void convert<version_c_t, NSHARE::version_t>
(version_c_t *aTo, NSHARE::version_t const& aWhat)
{
	aTo->major = aWhat.FMajor;
	aTo->minor = aWhat.FMinor;
	aTo->release = aWhat.FRelease;
}
template<> void convert<udt_customer_string_t, NSHARE::CText>
(udt_customer_string_t* aString, NSHARE::CText const& aWhat)
{
	NSHARE::CText::const_iterator _it = aWhat.begin(), _it_end(aWhat.end());

	char* aTo = (char*) (aString->string);
	unsigned i = 0;
	for (; _it != _it_end
			&& i < (CUSTOMER_C_MAX_STRING_LEN - 1)
			; ++_it, ++i)
		*aTo++ = *_it;
	*aTo++ = '\0';
	aString->len = i;

	CHECK(_it == _it_end) << aWhat;
}

template<> void convert<NSHARE::CText, udt_customer_string_t>(
		NSHARE::CText* aTo,
		udt_customer_string_t const & aWhat)
{
	unsigned const _str_len(
			(aWhat.len > (CUSTOMER_C_MAX_STRING_LEN - 1)) ?
															strlen(aWhat.string)
																	:
															aWhat.len
															);
	CHECK_LE(_str_len, CUSTOMER_C_MAX_STRING_LEN);
	*aTo = NSHARE::CText((NSHARE::utf8*) aWhat.string, _str_len);
}
template<> void convert<id_c_t, NUDT::id_t>
(id_c_t *aTo, NUDT::id_t const& aWhat)
{
	aTo->uuid = aWhat.FUuid.FVal;
	convert(&aTo->name, aWhat.FName);
}
template<> void convert<NUDT::id_t, id_c_t>
(NUDT::id_t *aTo,id_c_t  const& aWhat)
{
	aTo->FUuid.FVal = aWhat.uuid;
	convert(&aTo->FName, aWhat.name);
}
template<> void convert<program_id_c_t, NUDT::program_id_t>(program_id_c_t* aTo,
		NUDT::program_id_t const& aWhat)
{
	convert(&aTo->id, aWhat.FId);
	convert(&aTo->version, aWhat.FVersion);
	aTo->time = aWhat.FTime;
	aTo->pid = aWhat.FPid;
	convert((&aTo->path), aWhat.FPath);
	aTo->type = static_cast<unsigned>(aWhat.FType);
	aTo->endian = static_cast<unsigned>(aWhat.FEndian);
}
template<>
void convert<NUDT::program_id_t, program_id_c_t>(NUDT::program_id_t* aTo,
		program_id_c_t const& aWhat)
{
	convert(&aTo->FId, aWhat.id);
	convert(&aTo->FVersion, aWhat.version);
	aTo->FTime = aWhat.time;
	aTo->FPid = aWhat.pid;
	convert((&aTo->FPath), aWhat.path);
	aTo->FType = static_cast<NUDT::eProgramType>(aWhat.type);
	aTo->FEndian = static_cast<NSHARE::eEndian>(aWhat.endian);
}
template<> void convert<NUDT::required_header_t, required_header_c_t>
(NUDT::required_header_t *aTo, required_header_c_t const& aWhat)
{
	convert(&aTo->FVersion, aWhat.version);
	memcpy(aTo->FMessageHeader, aWhat.header, sizeof(aWhat.header));
}
template<> void convert<required_header_c_t, NUDT::required_header_t>
(required_header_c_t *aTo, NUDT::required_header_t const& aWhat)
{
	convert(&aTo->version, aWhat.FVersion);
	memcpy(aTo->header, aWhat.FMessageHeader, sizeof(aWhat.FMessageHeader));
}
template<> void convert<NUDT::requirement_msg_info_t, requirement_msg_info_c_t>
(NUDT::requirement_msg_info_t *aTo, requirement_msg_info_c_t const& aWhat)
{
	convert(&aTo->FProtocolName, aWhat.protocol_name);
	convert(&aTo->FRequired, aWhat.required);

	aTo->FFlags = aWhat.flags;
	convert(&aTo->FFrom, aWhat.rfrom);
}

template<> void convert<requirement_msg_info_c_t, NUDT::requirement_msg_info_t>
(requirement_msg_info_c_t *aTo, NUDT::requirement_msg_info_t const& aWhat)
{
	convert(&aTo->protocol_name, aWhat.FProtocolName);
	convert(&aTo->required, aWhat.FRequired);

	aTo->flags = aWhat.FFlags;
	convert(&aTo->rfrom, aWhat.FFrom);
}
template<> void convert<what_c_t, NUDT::subcribe_receiver_args_t::what_t>
(what_c_t *aTo, NUDT::subcribe_receiver_args_t::what_t const& aWhat)
{
	convert(&aTo->what, aWhat.FWhat);
	convert(&aTo->who, aWhat.FWho);
}
template<> void convert< NUDT::subcribe_receiver_args_t::what_t, what_c_t>
(NUDT::subcribe_receiver_args_t::what_t *aTo,what_c_t  const& aWhat)
{
	convert(&aTo->FWhat, aWhat.what);
	convert(&aTo->FWho, aWhat.who);
}

template<> void convert<subcribe_receiver_args_c_t,
		NUDT::subcribe_receiver_args_t>
(subcribe_receiver_args_c_t *aTo, NUDT::subcribe_receiver_args_t const& aWhat)
{
	NUDT::subcribe_receiver_args_t::receivers_t::const_iterator _it =
			aWhat.FReceivers.begin(), _it_end(aWhat.FReceivers.end());
	unsigned i = 0;
	for (; _it != _it_end && //
			i < sizeof(aTo->receivers) / sizeof(aTo->receivers[0]);
			++i, ++_it)
	{
		convert(&aTo->receivers[i],
				(NUDT::subcribe_receiver_args_t::what_t) *_it);
	}
	CHECK(_it == _it_end);
	aTo->size = i;
}
template<> void convert<NUDT::subcribe_receiver_args_t,
subcribe_receiver_args_c_t>
(NUDT::subcribe_receiver_args_t *aTo, subcribe_receiver_args_c_t const& aWhat)
{
	aTo->FReceivers.resize(aWhat.size);

	for (unsigned i = 0;
			i < aWhat.size;
			++i)
	{
		convert(&aTo->FReceivers[i],
				aWhat.receivers[i]);
	}
}
template<> void convert<customers_updated_args_c_t,
		NUDT::customers_updated_args_t>(customers_updated_args_c_t *aTo,
		NUDT::customers_updated_args_t const& aWhat)
{
	{
		std::set<NUDT::program_id_t>::const_iterator _it =
				aWhat.FDisconnected.begin(), _it_end(aWhat.FDisconnected.end());
		unsigned i = 0;
		for (; _it != _it_end && //
				i < sizeof(aTo->disconnected) / sizeof(aTo->disconnected[0]);
				++i, ++_it)
		{
			convert(&aTo->disconnected[i],
					(NUDT::program_id_t) *_it);
		}
		aTo->disconnected_size = i;
		CHECK(_it == _it_end);
	}
	{
		std::set<NUDT::program_id_t>::const_iterator _it =
				aWhat.FConnected.begin(), _it_end(aWhat.FConnected.end());
		unsigned i = 0;
		for (; _it != _it_end && //
				i < sizeof(aTo->connected) / sizeof(aTo->connected[0])
				; ++i, ++_it)
		{
			convert(&aTo->connected[i],
					(NUDT::program_id_t) *_it);
		}
		aTo->connected_size = i;
		CHECK(_it == _it_end);
	}
}

template<> void convert<NUDT::customers_updated_args_t,
		customers_updated_args_c_t>(NUDT::customers_updated_args_t *aTo,
		customers_updated_args_c_t const& aWhat)
{
	for (unsigned i = 0;
			i < aWhat.disconnected_size;
			++i)
	{
		program_id_t _id;
		convert(&_id,
				aWhat.disconnected[i]);
		aTo->FDisconnected.insert(_id);
	}
	for (unsigned i = 0;
			i < aWhat.connected_size;
			++i)
	{
		program_id_t _id;
		convert(&_id,
				aWhat.connected[i]);
		aTo->FConnected.insert(_id);
	}
}
template<>
void convert<received_data_c_t, NUDT::received_data_t>(received_data_c_t* aTo,
		NUDT::received_data_t const& aWhat)
{
	convert(&aTo->buffer, aWhat.FBuffer);

	aTo->header_begin = aWhat.FHeaderBegin;
	aTo->begin = aWhat.FBegin;
	aTo->end = aWhat.FEnd;
}

template<>
void convert<NUDT::received_data_t, received_data_c_t>(
		NUDT::received_data_t* aTo,
		received_data_c_t const& aWhat)
{
	convert(&aTo->FBuffer, aWhat.buffer);

	aTo->FHeaderBegin = aWhat.header_begin;
	aTo->FBegin = aWhat.begin;
	aTo->FEnd = aWhat.end;
}
template<>
void convert<received_message_info_c_t, NUDT::received_message_args_t>(
		received_message_info_c_t* aTo,
		NUDT::received_message_args_t const& aWhat)
{
	convert(&aTo->rfrom, aWhat.FFrom);
	convert(&aTo->protocol_name, aWhat.FProtocolName);
	aTo->packet_number = aWhat.FPacketNumber;
	convert(&aTo->header, aWhat.FHeader);
	{
		NUDT::received_message_args_t::uuids_t::const_iterator _it =
				aWhat.FTo.begin(), _it_end(aWhat.FTo.end());
		unsigned i = 0;
		for (; _it != _it_end && //
				i < sizeof(aTo->to) / sizeof(aTo->to[0])
				; ++i, ++_it)
		{
			convert(&aTo->to[i],
					(NSHARE::uuid_t) *_it);
		}
		aTo->to_size = i;
		CHECK(_it == _it_end);
	}
	aTo->occur_user_error = aWhat.FOccurUserError;
	aTo->endian = aWhat.FEndian;
	aTo->remain_CB = aWhat.FRemainCallbacks;
	aTo->cbs = aWhat.FCbs;
}
template<>
void convert<received_message_args_c_t, NUDT::received_message_args_t>(
		received_message_args_c_t* aTo,
		NUDT::received_message_args_t const& aWhat)
{
	convert(&aTo->info, aWhat);
	convert(&aTo->message, aWhat.FMessage);
}
template<>
void convert<NUDT::received_message_args_t, received_message_info_c_t>(
		NUDT::received_message_args_t* aTo,
		received_message_info_c_t const& aWhat)
{
	convert(&aTo->FFrom, aWhat.rfrom);
	convert(&aTo->FProtocolName, aWhat.protocol_name);
	aTo->FPacketNumber = aWhat.packet_number;
	convert(&aTo->FHeader, aWhat.header);
	{
		unsigned i = 0;
		for (; i < aWhat.to_size; ++i)
		{
			NSHARE::uuid_t _val;
			convert(&_val,
					aWhat.to[i]);
			aTo->FTo.push_back(_val);
		}
	}
	aTo->FOccurUserError = aWhat.occur_user_error;
	aTo->FEndian = aWhat.endian;
	aTo->FRemainCallbacks = aWhat.remain_CB;
	aTo->FCbs = aWhat.cbs;
}
template<>
void convert<NUDT::received_message_args_t, received_message_args_c_t>(
		NUDT::received_message_args_t* aTo,
		received_message_args_c_t const& aWhat)
{
	convert(aTo, aWhat.info);
	convert(&aTo->FMessage, aWhat.message);
}
template<>
void convert<fail_sent_args_c_t, NUDT::fail_sent_args_t>(
		fail_sent_args_c_t* aTo,
		NUDT::fail_sent_args_t const& aWhat)
{
	convert(&aTo->rfrom, aWhat.FFrom);
	convert(&aTo->protocol_name, aWhat.FProtocolName);
	aTo->packet_number = aWhat.FPacketNumber;
	convert(&aTo->header, aWhat.FHeader);
	{
		NUDT::received_message_args_t::uuids_t::const_iterator _it =
				aWhat.FSentTo.begin(), _it_end(aWhat.FSentTo.end());
		unsigned i = 0;
		for (; _it != _it_end && //
				i < sizeof(aTo->to) / sizeof(aTo->to[0])
				; ++i, ++_it)
		{
			convert(&aTo->to[i],
					(NSHARE::uuid_t) *_it);
		}
		aTo->to_size = i;
		CHECK(_it == _it_end);
	}
	{
		NUDT::received_message_args_t::uuids_t::const_iterator _it =
				aWhat.FFails.begin(), _it_end(aWhat.FFails.end());
		unsigned i = 0;
		for (; _it != _it_end && //
				i < sizeof(aTo->fails) / sizeof(aTo->fails[0])
				; ++i, ++_it)
		{
			convert(&aTo->fails[i],
					(NSHARE::uuid_t) *_it);
		}
		aTo->fails_size = i;
		CHECK(_it == _it_end);
	}
	aTo->error_code = aWhat.FErrorCode;
	aTo->user_code = aWhat.FUserCode;
}
template<>
void convert<NUDT::fail_sent_args_t, fail_sent_args_c_t>(
		NUDT::fail_sent_args_t* aTo,
		fail_sent_args_c_t const& aWhat)
{
	convert(&aTo->FFrom, aWhat.rfrom);
	convert(&aTo->FProtocolName, aWhat.protocol_name);
	aTo->FPacketNumber = aWhat.packet_number;
	convert(&aTo->FHeader, aWhat.header);
	{
		aTo->FSentTo.resize(aWhat.to_size);

		for (unsigned i = 0;
				i < aWhat.to_size
				; ++i)
		{
			convert(&aTo->FSentTo[i], aWhat.to[i]);
		}
	}
	{
		aTo->FFails.resize(aWhat.fails_size);

		for (unsigned i = 0;
				i < aWhat.fails_size
				; ++i)
		{
			convert(&aTo->FFails[i], aWhat.fails[i]);
		}
	}
	aTo->FErrorCode = aWhat.error_code;
	aTo->FUserCode = aWhat.user_code;
}
}
#endif /* CUSTOMER_CONVER_IMPL_H_ */
