// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CExampleProtocolParser.cpp
 *
 *  Created on:  22.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <customer.h>
#include <CParserFactory.h>
#include <udt_example_protocol.h>
#include "import_export_macros.h"
#include "CExampleProtocolParser.h"


using namespace NUDT;
using namespace example_of_user_protocol;
NSHARE::CText const CExampleProtocolParser::NAME = PROTOCOL_NAME;

CExampleProtocolParser::CExampleProtocolParser() :
		IExtParser(NAME)
{

}

CExampleProtocolParser::~CExampleProtocolParser()
{
}
size_t CExampleProtocolParser::MDataOffset(const NUDT::required_header_t& aHeader) const
{
	/*! In the example protocol the data is begun after header.*/
	return sizeof(msg_head_t);
}
CExampleProtocolParser::result_t CExampleProtocolParser::MParserData(
		const uint8_t* aItBegin, const uint8_t* aItEnd,NSHARE::uuid_t aFrom,uint8_t aMask)
{
	/*! Algorithm of Parsing buffer from aItBegin to aItEnd:*/
	result_t _result;

	for (; aItBegin != aItEnd;)
	{
		bool  _buffer_is_small=false;
		const size_t _remain=aItEnd-aItBegin;
		obtained_dg_t _founded_dg;
		_founded_dg.FBegin = aItBegin;

		/*! If remain of buffer is less than the message header size
		 * wait for a new data.*/
		if (_remain < sizeof(msg_head_t))
			_buffer_is_small=true;
		else
		{
			/*! Compare the message type and size with predefined value of #eMsgType.
			 *  If are equals then the message is founded.
			 *  In the other case the error #E_INVALID_MSG_TYPE or #E_INVALID_MSG_SIZE
			 *  is occured.
			 * */

			msg_head_t const *_phead = (msg_head_t const*) aItBegin;
			switch (_phead->FType)
			{
			case E_MSG_TEST:
				if (_phead->FSize != MSG_TEST_SIZE)
				{
					aItBegin+=sizeof(msg_head_t);
					_founded_dg.FErrorCode = E_INVALID_MSG_SIZE;
				}
				else if (_remain < _phead->FSize)
					_buffer_is_small=true;
				else
				{
					aItBegin += _phead->FSize;
					memcpy(_founded_dg.FType.FMessageHeader, _phead, sizeof(msg_head_t));
				}
				break;
			case E_MSG_SWAP_BYTE_TEST:
				if (_phead->FSize != sizeof(swap_byte_order_msg_t))
				{
					aItBegin+=sizeof(msg_head_t);
					_founded_dg.FErrorCode = E_INVALID_MSG_SIZE;
				}
				else if (_remain < _phead->FSize)
					_buffer_is_small=true;
				else
				{
					aItBegin += _phead->FSize;
					_founded_dg.FType.FNumber = _phead->FType;
				}
				break;

			default:
				aItBegin = aItEnd;
				_founded_dg.FErrorCode = E_INVALID_MSG_TYPE;
				break;
			}
		}

		if (_buffer_is_small)
			break;
		else
		{
			_founded_dg.FEnd = aItBegin;

			_result.push_back(_founded_dg);
		}
	}

	return _result;

}
std::pair<NUDT::required_header_t,bool> CExampleProtocolParser::MHeader(
		const NSHARE::CConfig& aFrom) const
{
	/*! This is example of deserialization of the message header.*/

	NUDT::required_header_t _header;
	_header.FNumber=aFrom.MValue("num",0u);
	_header.FVersion=NSHARE::version_t(aFrom.MChild("ver"));
	return std::make_pair(_header, aFrom.MIsChild("num"));
}
NSHARE::CConfig CExampleProtocolParser::MToConfig(
		const NUDT::required_header_t& aHeader) const
{
	/*! This is example of serialization  the message header.*/

	NSHARE::CConfig _conf("head");
	_conf.MAdd("num",aHeader.FNumber);
	_conf.MAdd("ver",aHeader.FVersion.MSerialize());
	return _conf;
}
bool CExampleProtocolParser::MSwapEndian(const NUDT::required_header_t& aHeader,
		uint8_t* aItBegin, uint8_t* aItEnd) const
{
	/*! You has to be defined method for swapping byte order
	 * of message.*/

	if(aHeader.FNumber==E_MSG_SWAP_BYTE_TEST)
	{
		swap_byte_order_msg_t* const _msg=(swap_byte_order_msg_t*)aItBegin;
		_msg->FData.FNumber=NSHARE::swap_endain(_msg->FData.FNumber);
		_msg->FData.FByte1=NSHARE::swap_endain(_msg->FData.FByte1);
		_msg->FData.FByte2=NSHARE::swap_endain(_msg->FData.FByte2);
		_msg->FData.FByte3=NSHARE::swap_endain(_msg->FData.FByte3);

		_msg->FData.FUint16=NSHARE::swap_endain(_msg->FData.FUint16);
		_msg->FData.FInt16=NSHARE::swap_endain(_msg->FData.FInt16);

		_msg->FData.FUint32=NSHARE::swap_endain(_msg->FData.FUint32);
		_msg->FData.FInt32=NSHARE::swap_endain(_msg->FData.FInt32);

		_msg->FData.FFloat=NSHARE::swap_endain(_msg->FData.FFloat);
		return true;
	}else
		return false;
}
bool CExampleProtocolParser::MSwapEndian(NUDT::required_header_t* aHeader) const
{
	/*! You has to be defined method for swapping byte order
	 * of the header message.*/
	aHeader->FNumber=NSHARE::swap_endain(aHeader->FNumber);
	return true;
}

