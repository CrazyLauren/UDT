/*
 * CParserFactory.cpp
 *
 *  Created on: 16.12.2018
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016, 2017, 2018  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CRAWPROTOCOL_H_
#define CRAWPROTOCOL_H_


namespace NUDT
{
extern UDT_SHARE_EXPORT const NSHARE::CText RAW_PROTOCOL_NAME;

class CRawProtocolParser:public IExtParser
{
public:
	CRawProtocolParser() :
			IExtParser(RAW_PROTOCOL_NAME)
	{
	}

	~CRawProtocolParser()
	{
	}

	result_t MParserData(const uint8_t* aItBegin,
			const uint8_t* aItEnd,NSHARE::uuid_t aFrom=NSHARE::uuid_t(),uint8_t aMask=NSHARE::E_SHARE_ENDIAN)
	{
		//Parsing buffer aItBegin:aItEnd
		static result_t _result(1);
		LOG(DFATAL)<<"In "<<RAW_PROTOCOL_NAME<<" calling  method 'parser data' is no sense. ";
		return _result;

	}
	NSHARE::CText MToString(
			const required_header_t& aHeader) const
	{
		NSHARE::CText _text;
        _text<<"# "<<aHeader.FNumber;
		return _text;
	}
	std::pair<NUDT::required_header_t, bool> MHeader(
			const NSHARE::CConfig& aFrom) const
	{
		NUDT::required_header_t _header;
		_header.FNumber = aFrom.MValue("num", 0u);
		_header.FVersion = NSHARE::version_t(aFrom.MChild("ver"));
		return std::make_pair(_header, aFrom.MIsChild("num"));
	}
	NSHARE::CConfig MToConfig(
			const NUDT::required_header_t& aHeader) const
	{
		NSHARE::CConfig _conf("head");
		_conf.MAdd("num", aHeader.FNumber);
		_conf.MAdd( aHeader.FVersion.MSerialize());
		return _conf;
	}
	NSHARE::CConfig MToConfig(const required_header_t& aHead, const uint8_t* aItBegin,
			const uint8_t* aItEnd) const
	{
		NSHARE::CConfig  _conf("data");
		_conf.MAdd("DataView",aItBegin,aItEnd-aItBegin);
		return _conf;
	}
	NSHARE::CBuffer MFromConfig(const NSHARE::CConfig& aFrom) const
	{
		NSHARE::CBuffer _rval;
		if(aFrom.MIsChild("DataView"))
		{
			aFrom.MChild("DataView").MValue(_rval);
		}
		return _rval;
	}
	char const* MDescription() const
	{
		return "Raw protocol";
	}
	size_t MDataOffset(const required_header_t& aHeader) const
	{
		return 0u;
	}
	bool MSwapEndian(required_header_t* aHeader) const
	{
		aHeader->FNumber=NSHARE::swap_endian(aHeader->FNumber);
		return true;
	}
};
}


#endif /* CRAWPROTOCOL_H_ */
