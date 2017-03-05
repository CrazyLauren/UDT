/*
 * CExampleProtocolParser.h
 *
 *  Created on: 22.01.2016
 *      Author: Sergey
 */

#ifndef CEXAMPLEPROTOCOLPARSER_H_
#define CEXAMPLEPROTOCOLPARSER_H_
#include <udt_share_macros.h>
#include <udt_types.h>
#if  defined( _WIN32 )
#   ifdef EXAMPLE_PARSER_EXPORTS
#       define EXAMPLE_PARSER_EXPORT __declspec(dllexport)
#   else
#       define EXAMPLE_PARSER_EXPORT __declspec(dllimport)
#   endif
#else
#       define EXAMPLE_PARSER_EXPORT
#endif

class EXAMPLE_PARSER_EXPORT CExampleProtocolParser:public NUDT::IExtParser
{
public:
	static NSHARE::CText const NAME;
	CExampleProtocolParser();
	virtual ~CExampleProtocolParser();

	virtual result_t MParserData(const uint8_t* aItBegin,
			const uint8_t* aItEnd,NSHARE::uuid_t aFrom=NSHARE::uuid_t(),uint8_t aMask=ORDER_LE);
	virtual std::pair<NUDT::required_header_t,bool> MHeader(const NSHARE::CConfig& aFrom) const;
	virtual NSHARE::CConfig MToConfig(const NUDT::required_header_t&) const;


private:
};
#endif /* CEXAMPLEPROTOCOLPARSER_H_ */
