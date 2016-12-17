/*
 * IExtParser.h
 *
 *  Created on: 21.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
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
		required_header_t FType;
		const uint8_t* FBegin;
		const uint8_t* FEnd;//equal vector::end()
	};
	enum eMask
	{
		ORDER_LE=0x1u<<0,
		ORDER_BE=0x1u<<1,
	};
	typedef std::vector<obtained_dg_t> result_t;

	virtual result_t MParserData(const uint8_t* aItBegin,
			const uint8_t* aItEnd,uint8_t aMask=ORDER_LE, required_header_t const* aLast =NULL)=0;

	virtual required_header_t MHeader(const NSHARE::CConfig& aFrom) const=0;
	virtual NSHARE::CConfig MToConfig(const required_header_t&) const=0;

	virtual NSHARE::CConfig MToConfig(const required_header_t&,
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
protected:
	IExtParser(const NSHARE::CText& type) :
		NSHARE::IFactory(type)
	{
	}
};
}//
#endif /* IEXTPARSER_H_ */
