/*
 * parcer-cpp.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "parser-cpp.h"
#include "http_parser.h"

COMPILE_ASSERT((unsigned )NUDT::E_DELETE == (unsigned )HTTP_DELETE,
		IvalideE_DELETE);
COMPILE_ASSERT((unsigned )NUDT::E_GET == HTTP_GET, IvalideGET);
COMPILE_ASSERT((unsigned )NUDT::E_HEAD == HTTP_HEAD, IvalideHEAD);
COMPILE_ASSERT((unsigned )NUDT::E_POST == HTTP_POST, IvalidePOST);
COMPILE_ASSERT((unsigned )NUDT::E_PUT == HTTP_PUT, IvalidePUT);
COMPILE_ASSERT((unsigned )NUDT::E_CONNECT == HTTP_CONNECT, IvalideCONNECT);
COMPILE_ASSERT((unsigned )NUDT::E_OPTIONS == HTTP_OPTIONS, IvalideOPTIONS);
COMPILE_ASSERT((unsigned )NUDT::E_TRACE == HTTP_TRACE, IvalideTRACE);
COMPILE_ASSERT((unsigned )NUDT::E_COPY == HTTP_COPY, IvalideCOPY);
COMPILE_ASSERT((unsigned )NUDT::E_LOCK == HTTP_LOCK, IvalideLOCK);
COMPILE_ASSERT((unsigned )NUDT::E_MKCOL == HTTP_MKCOL, IvalideKCOL);
COMPILE_ASSERT((unsigned )NUDT::E_MOVE == HTTP_MOVE, IvalideMOVE);
COMPILE_ASSERT((unsigned )NUDT::E_PROPFIND == HTTP_PROPFIND, IvalidePROPFIND);
COMPILE_ASSERT((unsigned )NUDT::E_PROPPATCH == HTTP_PROPPATCH, IvalidePROPPATCH);
COMPILE_ASSERT((unsigned )NUDT::E_SEARCH == HTTP_SEARCH, IvalideSEARCH);
COMPILE_ASSERT((unsigned )NUDT::E_UNLOCK == HTTP_UNLOCK, IvalideUNLOCK);
COMPILE_ASSERT((unsigned )NUDT::E_BIND == HTTP_BIND, IvalideBIND);
COMPILE_ASSERT((unsigned )NUDT::E_REBIND == HTTP_REBIND, IvalideREBIND);
COMPILE_ASSERT((unsigned )NUDT::E_UNBIND == HTTP_UNBIND, IvalideUNBIND);
COMPILE_ASSERT((unsigned )NUDT::E_ACL == HTTP_ACL, IvalideACL);
COMPILE_ASSERT((unsigned )NUDT::E_REPORT == HTTP_REPORT, IvalideREPORT);
COMPILE_ASSERT((unsigned )NUDT::E_MKACTIVITY == HTTP_MKACTIVITY,
		IvalideMKACTIVITY);
COMPILE_ASSERT((unsigned )NUDT::E_CHECKOUT == HTTP_CHECKOUT, IvalideE_CHECKOUT);
COMPILE_ASSERT((unsigned )NUDT::E_MERGE == HTTP_MERGE, IvalideMERGE);
COMPILE_ASSERT((unsigned )NUDT::E_MSEARCH == HTTP_MSEARCH, IvalideMSEARCH);
COMPILE_ASSERT((unsigned )NUDT::E_NOTIFY == HTTP_NOTIFY, IvalideNOTIFY);
COMPILE_ASSERT((unsigned )NUDT::E_SUBSCRIBE == HTTP_SUBSCRIBE, IvalideSUBSCRIBE);
COMPILE_ASSERT((unsigned )NUDT::E_UNSUBSCRIBE == HTTP_UNSUBSCRIBE,
		IvalideE_UNSUBSCRIBE);
COMPILE_ASSERT((unsigned )NUDT::E_PATCH == HTTP_PATCH, IvalidePATCH);
COMPILE_ASSERT((unsigned )NUDT::E_PURGE == HTTP_PURGE, IvalidePURGE);
COMPILE_ASSERT((unsigned )NUDT::E_MKCALENDAR == HTTP_MKCALENDAR,
		IvalideMKCALENDAR);
COMPILE_ASSERT((unsigned )NUDT::E_LINK == HTTP_LINK, IvalideLINK);
COMPILE_ASSERT((unsigned )NUDT::E_UNLINK == HTTP_UNLINK, IvalideUNLINK);

COMPILE_ASSERT((unsigned )NUDT::E_HTTP_OK == HPE_OK, IvalideHPE_OK);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_EOF_STATE == HPE_INVALID_EOF_STATE,
		IvalideEOF_STATE);
COMPILE_ASSERT((unsigned )NUDT::E_HEADER_OVERFLOW == HPE_HEADER_OVERFLOW,
		IvalideHEADER_OVERFLOW);
COMPILE_ASSERT((unsigned )NUDT::E_CLOSED_CONNECTION == HPE_CLOSED_CONNECTION,
		IvalideCLOSED_CONNECTION);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_VERSION == HPE_INVALID_VERSION,
		IvalideINVALID_VERSION);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_STATUS == HPE_INVALID_STATUS,
		IvalideINVALID_STATUS);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_METHOD == HPE_INVALID_METHOD,
		IvalideNVALID_METHOD);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_URL == HPE_INVALID_URL,
		IvalideINVALID_URL);

COMPILE_ASSERT((unsigned )NUDT::E_INVALID_HOST == HPE_INVALID_HOST,
		IvalideINVALID_HOST);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_PORT == HPE_INVALID_PORT,
		IvalideINVALID_PORT);

COMPILE_ASSERT((unsigned )NUDT::E_INVALID_PATH == HPE_INVALID_PATH,
		IvalideINVALID_PATH);
COMPILE_ASSERT(
		(unsigned )NUDT::E_INVALID_QUERY_STRING == HPE_INVALID_QUERY_STRING,
		IvalideINVALID_QUERY_STRING);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_FRAGMENT == HPE_INVALID_FRAGMENT,
		IvalideINVALID_FRAGMENT);

COMPILE_ASSERT((unsigned )NUDT::E_LF_EXPECTED == HPE_LF_EXPECTED,
		IvalideLF_EXPECTED);
COMPILE_ASSERT(
		(unsigned )NUDT::E_INVALID_HEADER_TOKEN == HPE_INVALID_HEADER_TOKEN,
		IvalideINVALID_HEADER_TOKEN);
COMPILE_ASSERT(
		(unsigned )NUDT::E_INVALID_CONTENT_LENGTH == HPE_INVALID_CONTENT_LENGTH,
		IvalideINVALID_CONTENT_LENGTH);
COMPILE_ASSERT(
		(unsigned )NUDT::E_UNEXPECTED_CONTENT_LENGTH
				== HPE_UNEXPECTED_CONTENT_LENGTH,
		IvalideUNEXPECTED_CONTENT_LENGTH);
COMPILE_ASSERT((unsigned )NUDT::E_INVALID_CONSTANT == HPE_INVALID_CONSTANT,
		IvalideINVALID_CONSTANT);

COMPILE_ASSERT(
		(unsigned )NUDT::E_INVALID_INTERNAL_STATE == HPE_INVALID_INTERNAL_STATE,
		IvalideINVALID_INTERNAL_STATE);
COMPILE_ASSERT((unsigned )NUDT::E_STRICT == HPE_STRICT, IvalideSTRICT);
COMPILE_ASSERT((unsigned )NUDT::E_PAUSED == HPE_PAUSED, IvalidePAUSED);
COMPILE_ASSERT((unsigned )NUDT::E_UNKNOWN == HPE_UNKNOWN, IvalideUNKNOWN);

namespace NUDT
{
using namespace NSHARE;
extern NSHARE::CText http_method_to_str(eHtppMethod aVal)
{
	if (aVal >= 0)
		return NSHARE::CText(http_method_str(static_cast<http_method>(aVal)));
	else
		return NSHARE::CText("Not set");
}
extern NSHARE::CText http_error_str(eHtppError aError)
{
	if (aError >= 0)
		return NSHARE::CText(http_errno_name(static_cast<http_errno>(aError)));
	else
		return NSHARE::CText("Not set");
}
extern NSHARE::CText http_error_description(eHtppError aError)
{
	if (aError >= 0)
		return NSHARE::CText(
				http_errno_description(static_cast<http_errno>(aError)));
	else
		return NSHARE::CText("Not set");
}
extern NSHARE::CText http_status_str(eStatusCode c)
{
	switch (c)
	{
	case E_STARUS_UNINITIALIZED:
		return "Uninitialized";
	case E_STARUS_CONTINUE_CODE:
		return "Continue";
	case E_STARUS_SWITCHING_PROTOCOLS:
		return "Switching Protocols";
	case E_STARUS_OK:
		return "OK";
	case E_STARUS_CREATED:
		return "Created";
	case E_STARUS_ACCEPTED:
		return "Accepted";
	case E_STARUS_NON_AUTHORITATIVE_INFORMATION:
		return "Non Authoritative Information";
	case E_STARUS_NO_CONTENT:
		return "No Content";
	case E_STARUS_RESET_CONTENT:
		return "Reset Content";
	case E_STARUS_PARTIAL_CONTENT:
		return "Partial Content";
	case E_STARUS_MULTIPLE_CHOICES:
		return "Multiple Choices";
	case E_STARUS_MOVED_PERMANENTLY:
		return "Moved Permanently";
	case E_STARUS_FOUND:
		return "Found";
	case E_STARUS_SEE_OTHER:
		return "See Other";
	case E_STARUS_NOT_MODIFIED:
		return "Not Modified";
	case E_STARUS_USE_PROXY:
		return "Use Proxy";
	case E_STARUS_TEMPORARY_REDIRECT:
		return "Temporary Redirect";
	case E_STARUS_BAD_REQUEST:
		return "Bad Request";
	case E_STARUS_UNAUTHORIZED:
		return "Unauthorized";
	case E_STARUS_PAYMENT_REQUIRED:
		return "Payment Required";
	case E_STARUS_FORBIDDEN:
		return "Forbidden";
	case E_STARUS_NOT_FOUND:
		return "Not Found";
	case E_STARUS_METHOD_NOT_ALLOWED:
		return "Method Not Allowed";
	case E_STARUS_NOT_ACCEPTABLE:
		return "Not Acceptable";
	case E_STARUS_PROXY_AUTHENTICATION_REQUIRED:
		return "Proxy Authentication Required";
	case E_STARUS_REQUEST_TIMEOUT:
		return "Request Timeout";
	case E_STARUS_CONFLICT:
		return "Conflict";
	case E_STARUS_GONE:
		return "Gone";
	case E_STARUS_LENGTH_REQUIRED:
		return "Length Required";
	case E_STARUS_PRECONDITION_FAILED:
		return "Precondition Failed";
	case E_STARUS_REQUEST_ENTITY_TOO_LARGE:
		return "Request Entity Too Large";
	case E_STARUS_REQUEST_URI_TOO_LONG:
		return "Request-URI Too Long";
	case E_STARUS_UNSUPPORTED_MEDIA_TYPE:
		return "Unsupported Media Type";
	case E_STARUS_REQUEST_RANGE_NOT_SATISFIABLE:
		return "Requested Range Not Satisfiable";
	case E_STARUS_EXPECTATION_FAILED:
		return "Expectation Failed";
	case E_STARUS_IM_A_TEAPOT:
		return "I'm a teapot";
	case E_STARUS_UPGRADE_REQUIRED:
		return "Upgrade Required";
	case E_STARUS_PRECONDITION_REQUIRED:
		return "Precondition Required";
	case E_STARUS_TOO_MANY_REQUESTS:
		return "Too Many Requests";
	case E_STARUS_REQUEST_HEADER_FIELDS_TOO_LARGE:
		return "Request Header Fields Too Large";
	case E_STARUS_INTERNAL_SERVER_ERROR:
		return "Internal Server Error";
	case E_STARUS_NOT_IMPLEMENTED:
		return "Not Implemented";
	case E_STARUS_BAD_GATEWAY:
		return "Bad Gateway";
	case E_STARUS_SERVICE_UNAVAILABLE:
		return "Service Unavailable";
	case E_STARUS_GATEWAY_TIMEOUT:
		return "Gateway Timeout";
	case E_STARUS_HTTP_VERSION_NOT_SUPPORTED:
		return "HTTP Version Not Supported";
	case E_STARUS_NOT_EXTENDED:
		return "Not Extended";
	case E_STARUS_NETWORK_AUTHENTICATION_REQUIRED:
		return "Network Authentication Required";
	default:
		return "Unknown";
	}
}
CUrl::CUrl() :
		FIsValid(false)
{
	;
}
CUrl::CUrl(NSHARE::CText const& aFrom, bool aIsConnect, ICodeConv const& aType)
{
	MParser(aFrom, aIsConnect, aType);
}
void CUrl::MDecode(CText& aTo, CText::const_iterator aBegin,
		CText::const_iterator aEnd, ICodeConv const& aType) const
{
	std::string _str;
	for (; aBegin != aEnd; ++aBegin)
	{
		if (*aBegin == '%')
		{
			int _val = 0;
			++aBegin; //%
			CText _tmp(aBegin, aBegin + 2); //hex value
			++aBegin;
			sscanf(_tmp.c_str(), "%x", &_val);
			_str += _val;

		}
		else
		{
			_str += *aBegin;
		}
	}
	aTo.assign(_str, 0, CText::npos, aType);
}
void CUrl::MReset()
{
	FSchema.MUnSet();
	FHost.MUnSet();
	FPort.MUnSet();
	FPath.MUnSet();
	FQuery.clear();
	FFragment.MUnSet();
	FUser.MUnSet();
	FPassword.MUnSet();
	FIsValid=false;
}
bool CUrl::MParser(NSHARE::CText const& aFrom, bool aIsConnect,
		ICodeConv const& aType)
{
	VLOG(2) << "Parser URL " << aFrom;
	MReset();

	http_parser_url _url;
	http_parser_url_init(&_url);
	int _error = http_parser_parse_url(aFrom.c_str(), aFrom.length_code(),
			aIsConnect ? 1 : 0, &_url);
	FIsValid = _error == 0;
	VLOG(2) << "Parser state " << _error;
	if (!FIsValid)
		return false;
	for (unsigned i = 0; i < UF_MAX; ++i)
		if ((_url.field_set & (1 << i)) != 0)
		{
			VLOG(2) << "Data " << i << " off:" << _url.field_data[i].off << //
								" len:" << _url.field_data[i].len;
			NSHARE::CText _text;
			MDecode(_text, aFrom.begin() + _url.field_data[i].off,
					aFrom.begin() + _url.field_data[i].off
							+ _url.field_data[i].len, aType);
			switch (i)
			{
			case UF_SCHEMA:
				FSchema = _text;
				break;
			case UF_HOST:
				FHost = _text;
				break;
			case UF_PORT:
			{
				unsigned _port = 0;
				if (NSHARE::from_string(_text, _port))
					FPort.MGet() = _port;
				VLOG(2) << "Port:" << _port;
				break;
			}
			case UF_PATH:
				FPath = _text;
				break;
			case UF_QUERY:
			{
				if (!_text.empty())
				{
					NSHARE::CText::size_type _query_begin = 0;
					do
					{
						NSHARE::CText::size_type _pos_ampersand =
								_text.find_first_of('&', _query_begin);
						NSHARE::CText::size_type const _pos_semicolon =
								_text.find_first_of(';', _query_begin);
						NSHARE::CText::size_type const _query_end = std::min(
								_pos_ampersand, _pos_semicolon);

						NSHARE::CText::size_type const _pos_equal =
								_text.find_first_of('=', _query_begin);
						NSHARE::CText::size_type const _pos_val = std::min(
								_pos_equal, _query_end);

						NSHARE::CText const _var = _text.substr(_query_begin,
								_pos_val == NSHARE::CText::npos ?
										NSHARE::CText::npos :
										_pos_val - _query_begin);
						NSHARE::CText const _val =
								_pos_val == NSHARE::CText::npos ?
										NSHARE::CText() :
										_text.substr(_pos_val + 1,
												_query_end
														== NSHARE::CText::npos ?
														NSHARE::CText::npos :
														_query_end - _pos_val
																- 1);

						VLOG(2)<<_var<<" = "<<_val;

						CHECK(FQuery.find(_var)==FQuery.end());
						FQuery[_var]=_val;

						_query_begin =
								_query_end == NSHARE::CText::npos ?
										NSHARE::CText::npos : _query_end + 1;
					} while (_query_begin!=NSHARE::CText::npos);
				}
			}
				break;
			case UF_FRAGMENT:
				FFragment = _text;
				break;
			case UF_USERINFO:
			{
				NSHARE::CText::size_type _pos = _text.find_first_of(':');
				FUser.MGet().assign(_text, 0, _pos);
				if (_pos != NSHARE::CText::npos)
					FPassword.MGet().assign(_text, _pos + 1,
							NSHARE::CText::npos);
			}
				break;
			case UF_MAX:
				CHECK(false);
				break;
			}
		}
	return FIsValid;
}
NSHARE::smart_field_t<NSHARE::CText> const& CUrl::MSchema() const
{
	return FSchema;
}
NSHARE::smart_field_t<NSHARE::CText> const&CUrl::MHost() const
{
	return FHost;
}
NSHARE::smart_field_t<unsigned> const&CUrl::MPort() const
{
	return FPort;
}
NSHARE::smart_field_t<NSHARE::CText> const&CUrl::MPath() const
{
	return FPath;
}
CUrl::qeury_t const&CUrl::MQuery() const
{
	return FQuery;
}
NSHARE::smart_field_t<NSHARE::CText> const&CUrl::MFragment() const
{
	return FFragment;
}
NSHARE::smart_field_t<NSHARE::CText> const&CUrl::MUser() const
{
	return FUser;
}
NSHARE::smart_field_t<NSHARE::CText> const&CUrl::MPassword() const
{
	return FPassword;
}
bool CUrl::MIsValid() const
{
	return FIsValid;
}
std::ostream& CUrl::MPrint(std::ostream& aStream) const
{
	if (!MIsValid())
		aStream << "Is not valid";
	else
	{
		if (MSchema().MIs())
			aStream << MSchema();
		if (MUser().MIs())
			aStream << std::endl << MUser();
		if (MPassword().MIs())
			aStream << std::endl << MPassword();
		if (MHost().MIs())
			aStream << std::endl << MHost();
		if (MPort().MIs())
			aStream << std::endl << MPort();
		if (MPath().MIs())
			aStream << std::endl << MPath();
		if (!MQuery().empty())
		{
			qeury_t::const_iterator _it = MQuery().begin(), _it_end(
					MQuery().end());
			aStream << std::endl;
			for (; _it != _it_end; ++_it)
			{
				aStream << _it->first << " = " << _it->second << ", ";
			}

		}
		if (MFragment().MIs())
			aStream << std::endl << MFragment();
	}
	return aStream;
}
bool CUrl::sMUintTest()
{
	CUrl _url(
			"http://user:pass@foo.com:8080/exmaple/index.html?bar=1&for=2;hello=3#some",
			0);
	qeury_t _qeury;
	_qeury["bar"] = "1";
	_qeury["for"] = "2";
	_qeury["hello"] = "3";
	CHECK(_url.MIsValid());

	CHECK_EQ(_url.MSchema().MGetConst(), "http");
	CHECK_EQ(_url.MUser().MGetConst(), "user");
	CHECK_EQ(_url.MPassword().MGetConst(), "pass");
	CHECK_EQ(_url.MHost().MGetConst(), "foo.com");
	CHECK_EQ(_url.MPort().MGetConst(), 8080);
	CHECK_EQ(_url.MPath().MGetConst(), "/exmaple/index.html");
	CHECK(_url.MQuery() == _qeury);
	CHECK_EQ(_url.MFragment().MGetConst(), "some");

	_url.MParser("foo.com:8080", 1);
	CHECK(_url.MIsValid());
	CHECK_EQ(_url.MHost().MGetConst(), "foo.com");
	CHECK_EQ(_url.MPort().MGetConst(), 8080);

	_url.MParser(
			"http://user:pass@foo.com:8080/%D0%DA%D0%B8%D1%80?bar=1&for=2;hello=3#some",
			0);
	CHECK(_url.MIsValid());

	CHECK_EQ(_url.MSchema().MGetConst(), "http");
	CHECK_EQ(_url.MUser().MGetConst(), "user");
	CHECK_EQ(_url.MPassword().MGetConst(), "pass");
	CHECK_EQ(_url.MHost().MGetConst(), "foo.com");
	CHECK_EQ(_url.MPort().MGetConst(), 8080);
	CHECK_EQ(_url.MPath().MGetConst(), "/Кир");
	CHECK(_url.MQuery()== _qeury)
;
			CHECK_EQ(_url.MFragment().MGetConst(), "some");
	return _url.MIsValid();
}
}

