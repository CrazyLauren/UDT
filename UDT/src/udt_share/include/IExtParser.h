/*
 * IExtParser.h
 *
 *  Created on: 21.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef IEXTPARSER_H_
#define IEXTPARSER_H_
#include <udt_share_macros.h>
#include <udt_types.h>


namespace NUDT
{
class UDT_SHARE_EXPORT IExtParser:public NSHARE::IFactory
{
public:
	struct obtained_dg_t
	{
		obtained_dg_t():
			FErrorCode(0),//
			FBegin(NULL),
			FEnd(NULL)
		{

		}
		required_header_t FType;
		uint8_t FErrorCode;
		const uint8_t* FBegin;
		const uint8_t* FEnd;//equal vector::end()
	};
	typedef std::vector<obtained_dg_t> result_t;

	virtual result_t MParserData(const uint8_t* aItBegin,
			const uint8_t* aItEnd,NSHARE::uuid_t aFrom=NSHARE::uuid_t(),uint8_t aMask=NSHARE::E_SHARE_ENDIAN)=0;

	virtual std::pair<required_header_t,bool> MHeader(const NSHARE::CConfig& aFrom) const=0;
	virtual NSHARE::CConfig MToConfig(const required_header_t&) const=0;

	virtual NSHARE::CConfig MToConfig(const required_header_t& aHeader,//for parsing a raw protocol.
			const uint8_t* aItBegin, const uint8_t* aItEnd) const
	{
		return NSHARE::CConfig();
	}
	virtual NSHARE::CBuffer MFromConfig(const NSHARE::CConfig& aFrom) const
	{
		return NSHARE::CBuffer();
	}
	virtual char const* MDescription() const
	{
		return "no description";
	}
	virtual bool MSwapEndian(const required_header_t& aHeader,//for optimization
			uint8_t* aItBegin, uint8_t* aItEnd){
		return false;
	}
protected:
	IExtParser(const NSHARE::CText& type) :
		NSHARE::IFactory(type)
	{
	}
};
}//
#endif /* IEXTPARSER_H_ */
