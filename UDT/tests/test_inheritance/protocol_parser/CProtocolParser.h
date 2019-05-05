/*
 * CProtocolParser.h
 *
 *  Created on:  22.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CROTOCOLPARSER_H_
#define CROTOCOLPARSER_H_

#include <udt_share_macros.h>
#include <udt_types.h>

/*!\brief An example of easiest protocol parser
 *
 */
class PROTOCOL_PARSER_EXPORT CProtocolParser:public NUDT::IExtParser
{
public:
	static NSHARE::CText const NAME;///< The unique name of protocol

	/*!\brief the default constructor
	 * in which you has to pass to IExtParser
	 * the unique protocol name
	 *
	 */
	CProtocolParser();

	/*!\brief default destructor
	 *
	 */
	virtual ~CProtocolParser();

	virtual result_t MParserData(const uint8_t* aItBegin,
			const uint8_t* aItEnd,NSHARE::uuid_t aFrom=NSHARE::uuid_t(),uint8_t aMask=NSHARE::E_SHARE_ENDIAN);
	virtual std::pair<NUDT::required_header_t,bool> MHeader(const NSHARE::CConfig& aFrom) const;
	virtual NSHARE::CConfig MToConfig(const NUDT::required_header_t&) const;

	virtual size_t MDataOffset(const NUDT::required_header_t& aHeader) const;
	virtual bool MSwapEndian(const NUDT::required_header_t& aHeader,
			uint8_t* aItBegin, uint8_t* aItEnd) const;
	bool MSwapEndian(NUDT::required_header_t* aHeader) const;
	inheritances_info_t MGetInheritances() const;
private:
};
#endif /* CROTOCOLPARSER_H_ */
