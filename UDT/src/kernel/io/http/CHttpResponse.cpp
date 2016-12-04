/*
 * CHttpResponse.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 16.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <algorithm>
#include "parser-cpp.h"
#include "CHttpResponse.h"

namespace NUDT
{
using namespace NSHARE;
static char const g_header_tokens[] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00..0f
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 10..1f
		0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, // 20..2f
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, // 30..3f
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40..4f
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, // 50..5f
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 60..6f
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, // 70..7f
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80..8f
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 90..9f
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // a0..af
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // b0..bf
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // c0..cf
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // d0..df
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // e0..ef
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // f0..ff
		};
/// Is the character a token
inline bool is_token_char(unsigned c)
{
	if (c > std::numeric_limits<unsigned char>::max())
		return false;
	return (g_header_tokens[c] == 1);
}

/// Is the character a non-token
inline bool is_not_token_char(unsigned c)
{
	return !g_header_tokens[c];
}
static CText const g_length_key = "content-length";
CHttpResponse::CHttpResponse()
{
	MSetVersion();

}

CHttpResponse::~CHttpResponse()
{
}
void CHttpResponse::MSetVersion(NSHARE::version_t const & version)
{
	FVersion = version;
}
bool CHttpResponse::MAppendHeader(NSHARE::CText const & key,
		NSHARE::CText const & val)
{
	if (std::find_if(key.begin(), key.end(), is_not_token_char) != key.end())
		return false;
	VLOG(2) << "Append new header " << key << " = " << val;
	header_array_t::iterator _it = FHeaders.find(key);
	if (_it == FHeaders.end())
		FHeaders[key] = val;
	else
		_it->second += ", " + val;
	return true;
}
void CHttpResponse::MRemoveHeader(NSHARE::CText const & key)
{
	FHeaders.erase(key);
}
void CHttpResponse::MSetBody(NSHARE::CText const & value)
{
	if (value.size() == 0)
	{
		MRemoveHeader(g_length_key);
		FBody.clear();
		return;
	}
	FBody = value;
	NSHARE::CText _length;
	NSHARE::num_to_str(FBody.length_code(), _length);
	FHeaders[g_length_key] = _length;
}
void CHttpResponse::MSetStatus(eStatusCode code, NSHARE::CText const & msg)
{
	FCode = code;
	if (msg.empty())
		FCodeMsg = http_status_str(code);
	else
		FCodeMsg = msg;
}
void CHttpResponse::MRawHeaders(NSHARE::CText& aTo) const
{
	header_array_t::const_iterator _it = FHeaders.begin();
	std::stringstream _buf;
	for (; _it != FHeaders.end(); ++_it)
	{
		_buf << _it->first << ": " << _it->second << "\r\n";
	}
	aTo = _buf.str();
}
NSHARE::CBuffer CHttpResponse::MRaw(NSHARE::CBuffer aBuf/*,NSHARE::ICodeConv*/)
{
	std::stringstream _buf;
	{
		NSHARE::CText _text_response;
		_text_response.MPrintf("HTTP/%d.%d %d %s\r\n", FVersion.FMajor,
				FVersion.FMinor, FCode, FCodeMsg.c_str());
		_buf << _text_response;
	}
	{
		header_array_t::iterator _it = FHeaders.find(g_length_key);
		if (_it != FHeaders.end())
		{
		}
		else if (!FBody.empty())
		{
			LOG(WARNING)<<g_length_key<<" is not setting.Chucked is not supported";
			CText _num;
			_num.MPrintf("%d",FBody.length_code());
			MAppendHeader(g_length_key,_num);
		}
#ifdef _WIN32
		time_t const _time = time(NULL);
		struct tm _ts;
		_ts = *localtime(&_time);
		NSHARE::CText _data(asctime(&_ts));
#else
		time_t const _time=time(NULL);
		struct tm _ts;
		localtime_r(&_time,&_ts);
		char _str_buf[128];
		NSHARE::CText _data(asctime_r(&_ts,_str_buf));
#endif
		_data.erase(_data.find_last_of('\n'));
		MAppendHeader("data",_data);
	}
	CText _head;
	MRawHeaders(_head);
	_buf << _head << "\r\n";
	_buf << FBody;
	std::string _str = _buf.str();
	NSHARE::CBuffer::const_iterator _it(
			(NSHARE::CBuffer::const_pointer) _str.c_str());

	aBuf.insert(aBuf.end(), _it, _it + _str.length());

	return aBuf;
}

} /* namespace NUDT */