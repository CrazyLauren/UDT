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
const NSHARE::CText rtc_info_t::NAME = "rtc_info";
const NSHARE::CText rtc_info_t::KEY_RTC_TYPE = "rtc_type";
const NSHARE::CText rtc_info_t::KEY_NAME = "name";
const NSHARE::CText rtc_info_t::KEY_NULL_OFFSET = "offset";

rtc_info_t::rtc_info_t():
		FRTCType(eRTC_DEFAULT),//
		FOffset(NSHARE::IAllocater::NULL_OFFSET)
{
	;
}
rtc_info_t::rtc_info_t(NSHARE::CConfig const& aConf) :
		FRTCType(eRTC_DEFAULT),//
		FOffset(NSHARE::IAllocater::NULL_OFFSET)
{
	CText _name;
	if(aConf.MGetIfSet(KEY_NAME,_name))
			FName.MSetRaw(_name);

	int _val=-1;

	if(aConf.MGetIfSet(KEY_RTC_TYPE, _val))
		FRTCType=static_cast<eRTCType>(_val);

	aConf.MGetIfSet(KEY_NULL_OFFSET, FOffset);
}
NSHARE::CConfig rtc_info_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet(KEY_NULL_OFFSET, FOffset);
	_conf.MSet(KEY_NAME, FName.MGetRawName());
	_conf.MSet<int>(KEY_RTC_TYPE, FRTCType);
	return _conf;
}
bool rtc_info_t::MIsValid() const
{
	return FName.MIsValid();
}
bool rtc_info_t::operator==(rtc_info_t const& aInfo) const
{
	return aInfo.FName==FName//
			&&aInfo.FRTCType==FRTCType//
			&&aInfo.FOffset==FOffset;
}
const NSHARE::CText real_time_clocks_t::NAME = "rtcs";
const NSHARE::CText real_time_clocks_t::SHARED_MEMORY_NAME = "shm_name";
real_time_clocks_t::real_time_clocks_t(NSHARE::CConfig const& aConf)
{
	VLOG(2) << "Create real_time_clocks_t from " << aConf;

	aConf.MGetIfSet(SHARED_MEMORY_NAME, FShdMemName);

	ConfigSet _set = aConf.MChildren(rtc_info_t::NAME);
	ConfigSet::const_iterator _it = _set.begin();
	for (; _it != _set.end(); ++_it)
	{
		VLOG(5) << "Push info " << *_it;
		push_back(rtc_info_t(*_it));
	}
}
NSHARE::CConfig real_time_clocks_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MSet(SHARED_MEMORY_NAME,FShdMemName);

	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		_conf.MAdd(_it->MSerialize());
	}

	return _conf;
}
bool real_time_clocks_t::MIsValid() const
{
	const_iterator _it(begin()), _end(end());
	for (; _it != _end; ++_it)
	{
		if (!_it->MIsValid())
			return false;
	}
	return true;
}
};


