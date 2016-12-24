/*
 * CHttpResponse.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 16.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CHTTPRESPONSE_H_
#define CHTTPRESPONSE_H_

namespace NUDT
{

class CHttpResponse
{
public:
	CHttpResponse();
	~CHttpResponse();

	void MSetVersion(
			NSHARE::version_t const & version = NSHARE::version_t(1, 1));
	bool MAppendHeader(NSHARE::CText const & key, NSHARE::CText const & val);
	void MRemoveHeader(NSHARE::CText const & key);

	void MSetBody(NSHARE::CText const & value);
	void MSetBody(NSHARE::CBuffer const & aBody);
	bool MWriteFile(NSHARE::CText const&);

	void MSetStatus(eStatusCode code, NSHARE::CText const & msg =
			NSHARE::CText());
	//fill response
	NSHARE::CBuffer MRaw(NSHARE::CBuffer =
			NSHARE::CBuffer()/*,NSHARE::ICodeConv*/);
	void MRawHeaders(NSHARE::CText&) const;

	static NSHARE::CText sMGetMimetype (NSHARE::CText const& filename);
private:

	typedef std::map<NSHARE::CText, NSHARE::CText> header_array_t;

	NSHARE::version_t FVersion;
	header_array_t FHeaders;
	NSHARE::CBuffer FBody;
	eStatusCode FCode;
	NSHARE::CText FCodeMsg;
	//NSHARE::CBuffer FResponse;
};

} /* namespace NUDT */
#endif /* CHTTPRESPONSE_H_ */
