// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * uuid.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 30.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/random_value.h>

namespace NSHARE
{
static uint64_t uuid_generate_time(CText const& aText = CText())
{
	generate_seed_if_need();

	uint8_t out[8];
	uint64_t time = NSHARE::get_unix_time();
	out[0] = (uint8_t) (time >> 56) | (uint8_t) (time);
	out[1] = (uint8_t) (time >> 48) ^ (uint8_t) (time >> 8);
	out[2] = rand() & 0xFF;
	out[3] = rand() & 0xFF;

	unsigned _pid = NSHARE::CThread::sMPid(); //getpid();
	out[4] = rand() & 0xFF;
	out[5] = rand() & 0xFF;
	out[6] = ((_pid >> 8) & 0xFF) ^ 0x0A;
	out[7] = (_pid & 0xFF) | 0x10;

	for (CText::const_iterator _it = aText.begin(); _it != aText.end(); ++_it)
	{
		const unsigned _pos = rand() % 4 + 2;
		out[_pos] = out[_pos] ^ (*_it & 0xFF);
	}
	if(is_RNG_available())
    {
	    ///< Don't use srand to avoid change default seek
        //srand(get_random_value_by_RNG());
        for (unsigned i = 0; i < 6; ++i)
        {
            out[i] = out[i] ^ (get_random_value_by_RNG() & 0xFF);
        }
    }
	memcpy(&time, out, sizeof(time));
	return time;
}
static uuid_t g_uuid;
uuid_t get_uuid(CText const& aVal)
{
	uuid_t _rval;
	for(HANG_INIT;(_rval.FVal = uuid_generate_time(aVal))==0;HANG_CHECK)
		;
	return _rval;
}

const CText uuid_t::NAME="uuid";
uuid_t::uuid_t(NSHARE::CConfig const& aConf) :
		FVal(0)
{
	if(!aConf.MValue().empty())
		FVal = aConf.MValue<uint64_t>(0);
	VLOG(4)<<"Value = "<<FVal;
}
CConfig uuid_t::MSerialize() const
{
	return CConfig(NAME,FVal);
}
CText uuid_t::MToString() const
{
	CText _text;
	uint8_t const *_this = reinterpret_cast<uint8_t const *>(&FVal);
#ifdef	SHARE_LITTLEENDIAN
	_text.MPrintf("%02x-%02x-"
			"%02x-%02x-"
			"%02x-%02x-"
			"%02x-%02x", _this[7], _this[6], _this[5], _this[4], _this[3],
			_this[2], _this[1], _this[0]);
#else
	_text.MPrintf("%02x-%02x-"
			"%02x-%02x-"
			"%02x-%02x-"
			"%02x-%02x", _this[0], _this[1], _this[2], _this[3], _this[4],
			_this[5], _this[6], _this[7]);
#endif
	return _text;
}
bool uuid_t::MIsValid() const
{
	return FVal != 0;
}
bool uuid_t::MFromString(CText const& aText)
{
	uint8_t *_this = reinterpret_cast<uint8_t *>(&FVal);
	uint32_t _data[8];
	memset(_data, 0, sizeof(_data));
	int count = sscanf(aText.c_str(),
			"%*c%*c%02x%*c" //
					"%*c%*c%02x%*c"//
					"%*c%*c%02x%*c"//
					"%*c%*c%02x%*c"//
					"%*c%*c%02x%*c"//
					"%*c%*c%02x%*c"//
					"%*c%*c%02x%*c"//
					"%*c%*c%02x"//
			, &_data[7], &_data[6], &_data[5], &_data[4], &_data[3], &_data[2],
			&_data[1], &_data[0]);
	if (count == 8)
	{

		for (int i = 0; i < 8; ++i)
#ifdef	SHARE_LITTLEENDIAN
			_this[i] = _data[i];
#else
			_this[7-i] = _data[i];
#endif
	}
	return count == 8;
}
uuid_t get_programm_uuid(CText const& aVal)
{
	static bool g_is_uuid = false;
	if (!g_is_uuid)
	{
		CText _text = aVal;
		if (aVal.empty())
			_text.MMakeRandom(8);
		g_is_uuid = true;
		volatile uuid_t _uuid = get_uuid(_text);
		g_uuid.FVal = _uuid.FVal;
	}
	return g_uuid;
}
uint32_t uuid_t::MGetHash() const
{
	uint64_t _val = FVal;
	_val = (~_val) + (_val << 18);
	_val = _val ^ (_val >> 31);
	_val *= 21;
	_val = _val ^ (_val >> 11);
	_val = _val + (_val << 6);
	_val = _val ^ (_val >> 22);
	return (uint32_t)_val;
}
}
;

