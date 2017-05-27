/*
 * CHttpRequest.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.05.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <core/kernel_type.h>
#include "CHttpRequest.h"
#include "http_parser.h"

namespace NUDT
{
template<>
const NSHARE::CText http_request_id_t::NAME = "HttpRequestId";
using namespace NSHARE;
CHttpRequest::CHttpRequest() :
		FMethod(E_METHOD_NOT_SET), FError(E_HTTP_ERROR_NOT_SET), FParser(NULL), FParserSettings(
				NULL)
{
	MInitParser();
}
CHttpRequest::CHttpRequest(CHttpRequest const& aRht)
{
	operator =(aRht);
}
CHttpRequest& CHttpRequest::operator =(CHttpRequest const& aRht)
{
	if (aRht.FShouldKeepAlive.MIs())
		FShouldKeepAlive = aRht.FShouldKeepAlive;
	if (aRht.FURl.MIs())
		FURl = aRht.FURl;

	FHeader = aRht.FHeader;
	FVersion = aRht.FVersion;
	FMethod = aRht.FMethod;
	FError = aRht.FError;
	FBuffer = aRht.FBuffer;
	FCurentUrl = aRht.FCurentUrl;
	FCurentHeaderField = aRht.FCurentHeaderField;
	FCurentHeaderValue = aRht.FCurentHeaderValue;
	MInitParser();
	return *this;
}

void CHttpRequest::MInitParser()
{
	FParser = new http_parser;
	http_parser_init(FParser, HTTP_REQUEST);
	FParser->data = this;
	FParserSettings = new http_parser_settings;
	http_parser_settings_init(FParserSettings);
	FParserSettings->on_message_begin = sMMessageBegin;
	FParserSettings->on_url = sMUrl;
	FParserSettings->on_header_field = sMHeaderField;
	FParserSettings->on_header_value = sMHeaderValue;
	FParserSettings->on_headers_complete = sMHeadersComplete;
	FParserSettings->on_body = sMBody;
	FParserSettings->on_message_complete = sMMessageComplete;
	FCode = new CCodeUTF8();
}
CHttpRequest::~CHttpRequest()
{
	MFreeParser();
}
void CHttpRequest::MFreeParser()
{
	delete FParserSettings;
	delete FParser;
	delete FCode;
	FParserSettings = NULL;
	FParser = NULL;
	FCode = NULL;
}
unsigned CHttpRequest::MParse(void const* aPtr, size_t aSize)
{
	CHECK_NOTNULL(FParser);
	CHECK_NOTNULL(FParserSettings);
	size_t const _size = http_parser_execute(FParser, FParserSettings,
			(const char*) aPtr, aSize);
	return _size;
}
NSHARE::smart_field_t<bool> const& CHttpRequest::MShouldKeepAlive() const
{
	return FShouldKeepAlive;
}
NSHARE::smart_field_t<CUrl> const& CHttpRequest::MUrl() const
{
	return FURl;
}
CHttpRequest::header_t const& CHttpRequest::MHeader() const
{
	return FHeader;
}

const NSHARE::version_t &CHttpRequest::MHttpVersion() const
{
	return FVersion;
}
eHtppMethod CHttpRequest::MMethod() const
{
	return FMethod;
}
eHtppError CHttpRequest::MError() const
{
	return FError;
}
NSHARE::CBuffer const& CHttpRequest::MBody() const
{
	return FBuffer;
}
NSHARE::ICodeConv const* CHttpRequest::MGetCode()
{
	return FCode;
}
int CHttpRequest::sMMessageBegin(http_parser *parser)
{
	CHttpRequest *_this = static_cast<CHttpRequest *>(parser->data);
	CHECK_NOTNULL(_this);
	_this->FCurentUrl.clear();
	_this->FCurentHeaderField.clear();
	_this->FCurentHeaderValue.clear();
	return 0;
}
int CHttpRequest::sMUrl(http_parser *parser, const char *at, size_t length)
{
	CHttpRequest *_this = static_cast<CHttpRequest *>(parser->data);
	CHECK_NOTNULL(_this);
	VLOG(2) << "Append to url:" << NSHARE::CText(at, length);
	_this->FCurentUrl.append(at, length);
	return 0;
}
int CHttpRequest::sMHeaderField(http_parser *parser, const char *at,
		size_t length)
{
	CHttpRequest *_this = static_cast<CHttpRequest *>(parser->data);
	CHECK_NOTNULL(_this);
	//the callback has to be called after sMHeaderValue
	if (!_this->FCurentHeaderField.empty()
			&& !_this->FCurentHeaderValue.empty())
	{
		VLOG(2) << "Insert " << _this->FCurentHeaderField << "="
							<< _this->FCurentHeaderValue;
		//insert value into map
		_this->FHeader[_this->FCurentHeaderField.MToLowerCase()] =
				_this->FCurentHeaderValue;
		//Clean up
		_this->FCurentHeaderField.clear();
		_this->FCurentHeaderValue.clear();
	}
	if (length)
	{
		VLOG(2) << "Append to head field :" << NSHARE::CText(at, length);
		_this->FCurentHeaderField.append(at, length);
	}
	return 0;
}
int CHttpRequest::sMHeaderValue(http_parser *parser, const char *at,
		size_t length)
{
	CHttpRequest *_this = static_cast<CHttpRequest *>(parser->data);
	CHECK_NOTNULL(_this);
	VLOG(2) << "Append to head value :" << NSHARE::CText(at, length);
	_this->FCurentHeaderValue.append(at, length);
	return 0;
}
int CHttpRequest::sMHeadersComplete(http_parser *parser)
{
	CHttpRequest *_this = static_cast<CHttpRequest *>(parser->data);
	CHECK_NOTNULL(_this);
	_this->FMethod = static_cast<eHtppMethod>(parser->method);
	_this->FVersion.FMajor = parser->http_major;
	_this->FVersion.FMinor = parser->http_minor;
	VLOG(2) << "Version:" << _this->FVersion;

	//	charset
	{
		header_t::const_iterator _it = _this->FHeader.find(CText("charset"));
		if (_it != _this->FHeader.end())
		{
			CText _tmp = _it->second;
			_tmp.MToLowerCase();
			if (_tmp == "utf-8")
			{
				delete _this->FCode;
				_this->FCode = new CCodeUTF8;
			}
			else
			{
				LOG(DFATAL) << "Unknown code:" << _tmp;
			}
		}
	}
	//
	if (!_this->FCurentUrl.empty())
		_this->FURl.MGet().MParser(_this->FCurentUrl,
				parser->method == HTTP_CONNECT, *_this->MGetCode());

	//bug fix I don't why last sMHeaderField does not call.
	sMHeaderField(parser, NULL, 0);
	CHECK(_this->FCurentHeaderField.empty());
	CHECK(_this->FCurentHeaderValue.empty());
	//
	_this->FShouldKeepAlive.MGet() = http_should_keep_alive(parser)!=0;
	if (_this->FVersion.FMajor == 1 && _this->FVersion.FMinor == 0)
	{
		header_t::const_iterator _it = _this->FHeader.find(CText("connection"));
		if (_it != _this->FHeader.end())
		{
			VLOG(2) << "Connection: " << _it->second;
			CText _tmp = _it->second;
			_tmp.MToLowerCase();
			if (_tmp == "keep-alive")
			{
				_this->FShouldKeepAlive.MGet() = true;
			}
		}
	}

	return 0;
}
int CHttpRequest::sMBody(http_parser *parser, const char *at, size_t length)
{
	CHttpRequest *_this = static_cast<CHttpRequest *>(parser->data);
	CHECK_NOTNULL(_this);

	CBuffer::value_type const* _data_begin = (CBuffer::value_type const*) at;
	CBuffer::const_iterator _it(_data_begin);
	CBuffer::const_iterator _end = _it + length;
	_this->FBuffer.insert(_this->FBuffer.end(), _it, _end);
	return 0;
}
int CHttpRequest::sMMessageComplete(http_parser *parser)
{
	CHttpRequest *_this = static_cast<CHttpRequest *>(parser->data);
	CHECK_NOTNULL(_this);
	_this->FError = static_cast<eHtppError>(parser->http_errno);
	VLOG(2) << "Msg complete";
	//cleanup
	_this->FCurentUrl.clear();
	_this->FCurentHeaderField.clear();
	_this->FCurentHeaderValue.clear();
	return 0;
}
std::ostream& CHttpRequest::MPrint(std::ostream& aStream) const
{
	if (FError == E_HTTP_ERROR_NOT_SET)
		aStream << "Not Set";
	else
	{
		if (MShouldKeepAlive().MIs())
			aStream << MShouldKeepAlive() << std::endl;
		if (MUrl().MIs())
			aStream << MUrl() << std::endl;
		aStream << MHttpVersion() << std::endl;
		aStream << MMethod() << std::endl;
		aStream << MError() << std::endl;
		header_t const& _header = MHeader();
		header_t::const_iterator _it = _header.begin();
		for (; _it != _header.end();)
		{
			aStream << _it->first << " = " << _it->second;
			++_it;
			if (_it != _header.end())
				aStream << std::endl;
		}

	}
	return aStream;
}
bool CHttpRequest::sMUintTest()
{
	char _buf[] =
			"GET /favicon.ico HTTP/1.1\r\n"
					"Host: 0.0.0.0=5000\r\n"
					"User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9) Gecko/2008061015 Firefox/3.0\r\n"
					"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
					"Accept-Language: en-us,en;q=0.5\r\n"
					"Accept-Encoding: gzip,deflate\r\n"
					"Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
					"Keep-Alive: 300\r\n"
					"Connection: keep-alive\r\n"
					"\r\n";
	CHttpRequest _request;
	_request.MParse(_buf, sizeof(_buf));
	char _buf2[] =
			"GET /test HTTP/1.1\r\n"
					"User-Agent: curl/7.18.0 (i486-pc-linux-gnu) libcurl/7.18.0 OpenSSL/0.9.8g zlib/1.2.3.3 libidn/1.1\r\n"
					"Host: 0.0.0.0=5000\r\n"
					"Accept: */*\r\n"
					"\r\n";
	CHttpRequest _request2;
	_request2.MParse(_buf2, sizeof(_buf2));
	return _request.MError() == E_HTTP_OK && _request2.MError() == E_HTTP_OK;
}
} /* namespace NSHARE */
