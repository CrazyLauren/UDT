/*
 * CHttpRequest.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.05.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CHTTPREQUEST_H_
#define CHTTPREQUEST_H_

#include "parser-cpp.h"
class http_parser;
class http_parser_settings;
namespace NUDT
{
class CHttpRequest
{
public:
	typedef std::map<NSHARE::CText, NSHARE::CText,
			NSHARE::CStringFastLessCompare> header_t;
	CHttpRequest();
	CHttpRequest( CHttpRequest const& aRht);
	CHttpRequest& operator =( CHttpRequest const& aRht);
	~CHttpRequest();

	NSHARE::smart_field_t<bool> const& MShouldKeepAlive() const;
	NSHARE::smart_field_t<CUrl> const& MUrl() const;
	header_t const& MHeader() const;

	const NSHARE::version_t &MHttpVersion() const;
	eHtppMethod MMethod() const;
	eHtppError MError() const;
	NSHARE::CBuffer const& MBody() const;
	// Returns number of parsed bytes
	unsigned MParse(void const*,size_t);

	NSHARE::ICodeConv const* MGetCode();

	static bool sMUintTest();
	std::ostream& MPrint(std::ostream&) const;
private:

	static int sMMessageBegin(http_parser *parser);
	static int sMUrl(http_parser *parser, const char *at, size_t length);
	static int sMHeaderField(http_parser *parser, const char *at,
			size_t length);
	static int sMHeaderValue(http_parser *parser, const char *at,
			size_t length);
	static int sMHeadersComplete(http_parser *parser);
	static int sMBody(http_parser *parser, const char *at, size_t length);
	static int sMMessageComplete(http_parser *parser);
	void MInitParser();
	void MFreeParser();

	NSHARE::smart_field_t<bool> FShouldKeepAlive;
	NSHARE::smart_field_t<CUrl> FURl;
	header_t FHeader;

	NSHARE::version_t FVersion;
	eHtppMethod FMethod;
	eHtppError FError;
	NSHARE::CBuffer FBuffer;

	NSHARE::CText FCurentUrl;
	NSHARE::CText FCurentHeaderField;
	NSHARE::CText FCurentHeaderValue;
	http_parser* FParser;
	http_parser_settings* FParserSettings;
	NSHARE::ICodeConv const* FCode;
};
typedef data_from_id<CHttpRequest> http_request_id_t;
}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::CHttpRequest const& aVal)
{
	aVal.MPrint(aStream);
	return aStream ;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::http_request_id_t const& aVal)
{
	return aStream << aVal.FVal;
}

}
#endif /* CHTTPREQUEST_H_ */
