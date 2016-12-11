/*
 * parcer-cpp.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef PARCER_CPP_H_
#define PARCER_CPP_H_

namespace NUDT
{
//Taken from http_parser.h
enum eHtppMethod
{
	E_METHOD_NOT_SET = -1,
	E_DELETE = 0,//
	E_GET, //
	E_HEAD, //
	E_POST, //
	E_PUT, //
	// pathological
	E_CONNECT,
	E_OPTIONS,
	E_TRACE,
	// WebDAV
	E_COPY,
	E_LOCK,
	E_MKCOL,
	E_MOVE,
	E_PROPFIND,
	E_PROPPATCH,
	E_SEARCH,
	E_UNLOCK,
	E_BIND,
	E_REBIND,
	E_UNBIND,
	E_ACL,
	// subversion
	E_REPORT,
	E_MKACTIVITY,
	E_CHECKOUT,
	E_MERGE,
	// upnp
	E_MSEARCH,
	E_NOTIFY,
	E_SUBSCRIBE,
	E_UNSUBSCRIBE,
	// RFC-5789
	E_PATCH,
	E_PURGE,
	// CalDAV
	E_MKCALENDAR,
	//RFC-2068
	E_LINK,
	E_UNLINK
};
enum eHtppError
{
	E_HTTP_ERROR_NOT_SET = -1,
	E_HTTP_OK = 0, E_INVALID_EOF_STATE = 11, //stream ended at an unexpected time
	E_HEADER_OVERFLOW, //too many header bytes seen; overflow detected
	E_CLOSED_CONNECTION, //data received after completed connection: close message
	E_INVALID_VERSION, //invalid HTTP version
	E_INVALID_STATUS, //nvalid HTTP status code
	E_INVALID_METHOD, //invalid HTTP method
	E_INVALID_URL, //
	E_INVALID_HOST, //
	E_INVALID_PORT, //
	E_INVALID_PATH, //
	E_INVALID_QUERY_STRING, //
	E_INVALID_FRAGMENT, //
	E_LF_EXPECTED, //
	E_INVALID_HEADER_TOKEN, //
	E_INVALID_CONTENT_LENGTH, //
	E_UNEXPECTED_CONTENT_LENGTH, //
	E_INVALID_CHUNK_SIZE, //
	E_INVALID_CONSTANT, //
	E_INVALID_INTERNAL_STATE, //
	E_STRICT, //
	E_PAUSED, //
	E_UNKNOWN, //
};
enum eStatusCode
{
	E_STARUS_UNINITIALIZED = 0,

	E_STARUS_CONTINUE_CODE = 100,
	E_STARUS_SWITCHING_PROTOCOLS = 101,

	E_STARUS_OK = 200,
	E_STARUS_CREATED = 201,
	E_STARUS_ACCEPTED = 202,
	E_STARUS_NON_AUTHORITATIVE_INFORMATION = 203,
	E_STARUS_NO_CONTENT = 204,
	E_STARUS_RESET_CONTENT = 205,
	E_STARUS_PARTIAL_CONTENT = 206,

	E_STARUS_MULTIPLE_CHOICES = 300,
	E_STARUS_MOVED_PERMANENTLY = 301,
	E_STARUS_FOUND = 302,
	E_STARUS_SEE_OTHER = 303,
	E_STARUS_NOT_MODIFIED = 304,
	E_STARUS_USE_PROXY = 305,
	E_STARUS_TEMPORARY_REDIRECT = 307,

	E_STARUS_BAD_REQUEST = 400,
	E_STARUS_UNAUTHORIZED = 401,
	E_STARUS_PAYMENT_REQUIRED = 402,
	E_STARUS_FORBIDDEN = 403,
	E_STARUS_NOT_FOUND = 404,
	E_STARUS_METHOD_NOT_ALLOWED = 405,
	E_STARUS_NOT_ACCEPTABLE = 406,
	E_STARUS_PROXY_AUTHENTICATION_REQUIRED = 407,
	E_STARUS_REQUEST_TIMEOUT = 408,
	E_STARUS_CONFLICT = 409,
	E_STARUS_GONE = 410,
	E_STARUS_LENGTH_REQUIRED = 411,
	E_STARUS_PRECONDITION_FAILED = 412,
	E_STARUS_REQUEST_ENTITY_TOO_LARGE = 413,
	E_STARUS_REQUEST_URI_TOO_LONG = 414,
	E_STARUS_UNSUPPORTED_MEDIA_TYPE = 415,
	E_STARUS_REQUEST_RANGE_NOT_SATISFIABLE = 416,
	E_STARUS_EXPECTATION_FAILED = 417,
	E_STARUS_IM_A_TEAPOT = 418,
	E_STARUS_UPGRADE_REQUIRED = 426,
	E_STARUS_PRECONDITION_REQUIRED = 428,
	E_STARUS_TOO_MANY_REQUESTS = 429,
	E_STARUS_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,

	E_STARUS_INTERNAL_SERVER_ERROR = 500,
	E_STARUS_NOT_IMPLEMENTED = 501,
	E_STARUS_BAD_GATEWAY = 502,
	E_STARUS_SERVICE_UNAVAILABLE = 503,
	E_STARUS_GATEWAY_TIMEOUT = 504,
	E_STARUS_HTTP_VERSION_NOT_SUPPORTED = 505,
	E_STARUS_NOT_EXTENDED = 510,
	E_STARUS_NETWORK_AUTHENTICATION_REQUIRED = 511
};
extern NSHARE::CText http_method_to_str(eHtppMethod);
extern NSHARE::CText http_error_str(eHtppError);
extern NSHARE::CText http_error_description(eHtppError);
extern NSHARE::CText http_status_str(eStatusCode);
struct CUrl
{
	struct qeury_t:std::map<NSHARE::CText,NSHARE::CText>
	{
		 ;
	};
	CUrl();
	CUrl(NSHARE::CText const& aFrom, bool aIsConnect = false,
			NSHARE::ICodeConv const& aType = NSHARE::CCodeUTF8());
	bool MParser(NSHARE::CText const& aFrom, bool aIsConnect = false,
			NSHARE::ICodeConv const& aType = NSHARE::CCodeUTF8());

	NSHARE::smart_field_t<NSHARE::CText> const& MSchema() const;
	NSHARE::smart_field_t<NSHARE::CText> const&MUser() const;
	NSHARE::smart_field_t<NSHARE::CText> const&MPassword() const;
	NSHARE::smart_field_t<NSHARE::CText> const&MHost() const;
	NSHARE::smart_field_t<unsigned> const&MPort() const;
	NSHARE::smart_field_t<NSHARE::CText> const&MPath() const;
	qeury_t const&MQuery() const;
	NSHARE::smart_field_t<NSHARE::CText> const&MFragment() const;
	std::ostream& MPrint(std::ostream&) const;
	bool MIsValid() const;
	static bool sMUintTest();
	void MReset();
private:
	NSHARE::smart_field_t<NSHARE::CText> FSchema;
	NSHARE::smart_field_t<NSHARE::CText> FHost;
	NSHARE::smart_field_t<unsigned> FPort;
	NSHARE::smart_field_t<NSHARE::CText> FPath;
	qeury_t FQuery;
	NSHARE::smart_field_t<NSHARE::CText> FFragment;
	NSHARE::smart_field_t<NSHARE::CText> FUser;
	NSHARE::smart_field_t<NSHARE::CText> FPassword;
	void MDecode(NSHARE::CText& aTo, NSHARE::CText::const_iterator aBegin,
			NSHARE::CText::const_iterator aEnd,
			NSHARE::ICodeConv const& aType) const;
	bool FIsValid;

};
}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::eHtppMethod const& aVal)
{
	return aStream << NUDT::http_method_to_str(aVal);
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::eHtppError const& aVal)
{
	return aStream << NUDT::http_error_str(aVal);
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::eStatusCode const& aVal)
{
	return aStream << NUDT::http_status_str(aVal);
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::CUrl const& aVal)
{
	aVal.MPrint(aStream);
	return aStream ;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::CUrl::qeury_t const& aVal)
{
	NUDT::CUrl::qeury_t::const_iterator _it =aVal.begin(), _it_end(
			aVal.end());
	for (; _it != _it_end; ++_it)
	{
		aStream << _it->first << " = " << _it->second << ", ";
	}
	return aStream ;
}
}
#endif /* PARCER_CPP_H_ */
