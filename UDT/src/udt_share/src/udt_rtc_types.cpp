/*
 * udt_rtc_types.cpp
 *
 *  Created on: 05.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <UDT/udt_rtc.h>
#include <shared_types.h>

namespace NUDT
{
using namespace NSHARE;

//---------------------------
const NSHARE::CText rtc_unique_id_t::NAME = "rtc_id";
const NSHARE::CText rtc_unique_id_t::KEY_UNIQUE_ID = "uid";

const rtc_unique_id_t::unique_id_t rtc_unique_id_t::NO_ID = 0;

rtc_unique_id_t::rtc_unique_id_t() :
		FId(NO_ID)
{
	;
}
rtc_unique_id_t::rtc_unique_id_t(NSHARE::CConfig const& aConf) :
		FOwner(aConf.MChild(NSHARE::uuid_t::NAME)), //
		FId(NO_ID)
{
	aConf.MGetIfSet(KEY_UNIQUE_ID, FId);
}
NSHARE::CConfig rtc_unique_id_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet(KEY_UNIQUE_ID, FId);
	_conf.MAdd(FOwner.MSerialize());
	return _conf;
}
bool rtc_unique_id_t::MIsValid() const
{
	return FOwner.MIsValid() //
	&& FId != NO_ID;
}
bool rtc_unique_id_t::operator<(rtc_unique_id_t const& aInfo) const
		{
	return (FOwner == aInfo.FOwner) ? FId < aInfo.FId : FOwner < aInfo.FOwner;
}

bool rtc_unique_id_t::operator==(rtc_unique_id_t const& aInfo) const
		{
	return FOwner == aInfo.FOwner && FId == aInfo.FId;
}

bool rtc_unique_id_t::operator!=(rtc_unique_id_t const& aInfo) const
{
	return !operator ==(aInfo);
}
//---------------------------
const NSHARE::CText rtc_info_t::NAME = "rtc_info";
const NSHARE::CText rtc_info_t::KEY_RTC_TYPE = "rtc_type";
const NSHARE::CText rtc_info_t::KEY_NAME = "name";
const NSHARE::CText rtc_info_t::KEY_NULL_OFFSET = "offset";

rtc_info_t::rtc_info_t() :
		FRTCType(eRTC_DEFAULT), //
		FOffset(NSHARE::IAllocater::NULL_OFFSET)
{
	;
}
rtc_info_t::rtc_info_t(NSHARE::CConfig const& aConf) :
		FId(aConf.MChild(rtc_unique_id_t::NAME)), //
		FRTCType(eRTC_DEFAULT), //
		FOffset(NSHARE::IAllocater::NULL_OFFSET)
{
	CText _name;
	if (aConf.MGetIfSet(KEY_NAME, _name))
		FName.MSetRaw(_name);

	int _val = -1;

	if (aConf.MGetIfSet(KEY_RTC_TYPE, _val))
		FRTCType = static_cast<eRTCType>(_val);

	aConf.MGetIfSet(KEY_NULL_OFFSET, FOffset);
}
NSHARE::CConfig rtc_info_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(FId.MSerialize());
	_conf.MSet(KEY_NULL_OFFSET, FOffset);
	_conf.MSet(KEY_NAME, FName.MGetRawName());
	_conf.MSet<int>(KEY_RTC_TYPE, FRTCType);
	return _conf;
}
bool rtc_info_t::MIsValid() const
{
	return FName.MIsValid() && FId.MIsValid();
}
bool rtc_info_t::operator==(rtc_info_t const& aInfo) const
		{
	return aInfo.FName == FName //
	&& aInfo.FRTCType == FRTCType //
	&& aInfo.FOffset == FOffset //
	&& FId == aInfo.FId //
	;
}
const NSHARE::CText real_time_clocks_t::NAME = "rtcs";
const NSHARE::CText real_time_clocks_t::SHARED_MEMORY_NAME = "shm_name";
const NSHARE::CText real_time_clocks_t::KEY_INFO_FROM = "key_from";
real_time_clocks_t::real_time_clocks_t(NSHARE::CConfig const& aConf)
{
	VLOG(2) << "Create real_time_clocks_t from " << aConf;

	aConf.MGetIfSet(SHARED_MEMORY_NAME, FShdMemName);
	aConf.MGetIfSet(KEY_INFO_FROM, FInfoFrom);

	ConfigSet _set = aConf.MChildren(rtc_info_t::NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push info " << *_it;
		MInsert(rtc_info_t(*_it));
	}
}
static bool less_compare(rtc_info_t const& aLft, rtc_info_t const& aRgt)
{
	return aLft.FId < aRgt.FId;
}
void real_time_clocks_t::MInsert(rtc_info_t const& aRtc)
{
	FRtc.insert(std::upper_bound(FRtc.begin(),
			FRtc.end(), aRtc, less_compare),
			aRtc
			);
}

NSHARE::CConfig real_time_clocks_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet(SHARED_MEMORY_NAME, FShdMemName);
	_conf.MSet(KEY_INFO_FROM, FInfoFrom.MSerialize());

	rtc_info_array_t::const_iterator _it(FRtc.begin()), _end(FRtc.end());
	for (; _it != _end; ++_it)
	{
		_conf.MAdd(_it->MSerialize());
	}

	return _conf;
}
bool real_time_clocks_t::MIsValid() const
{
	rtc_info_array_t::const_iterator _it(FRtc.begin()), _end(FRtc.end());
	for (; _it != _end; ++_it)
	{
		if (!_it->MIsValid())
			return false;
	}
	return FInfoFrom.MIsValid();
}
}
;

