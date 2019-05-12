// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CProtocolParser.cpp
 *
 *  Created on:  12.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <customer.h>
#include <CParserFactory.h>

#include "import_export_macros.h"
#include "CProtocolParser.h"
#include "protocol_inherited.h"


using namespace NUDT;
using namespace protocol_inherited;
NSHARE::CText const CProtocolParser::NAME = PROTOCOL_NAME;

CProtocolParser::CProtocolParser() :
		IExtParser(NAME)
{

}

CProtocolParser::~CProtocolParser()
{
}
size_t CProtocolParser::MDataOffset(const NUDT::required_header_t& aHeader) const
{
	/*! In the example protocol the data is begun after header.*/
	return sizeof(msg_head_t);
}
CProtocolParser::result_t CProtocolParser::MParserData(
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
			case E_MSG_GRANDCHILD:
				if (_phead->FSize != sizeof(grand_child_msg_t))
				{
					aItBegin+=sizeof(msg_head_t);
					_founded_dg.FErrorCode = E_INVALID_MSG_SIZE;
				}
				else if (_remain < _phead->FSize)
					_buffer_is_small=true;
				else
				{
					aItBegin += _phead->FSize;
					memcpy(_founded_dg.FType.FMessageHeader, _phead, sizeof(*_phead));
				}
				break;
			case E_MSG_CHILD:
				if (_phead->FSize != sizeof(child_msg_t))
				{
					aItBegin+=sizeof(msg_head_t);
					_founded_dg.FErrorCode = E_INVALID_MSG_SIZE;
				}
				else if (_remain < _phead->FSize)
					_buffer_is_small=true;
				else
				{
					aItBegin += _phead->FSize;
					memcpy(_founded_dg.FType.FMessageHeader, _phead, sizeof(*_phead));
				}
				break;

			case E_MSG_PARENT:
				if (_phead->FSize != sizeof(parent_msg_t))
				{
					aItBegin+=sizeof(msg_head_t);
					_founded_dg.FErrorCode = E_INVALID_MSG_SIZE;
				}
				else if (_remain < _phead->FSize)
					_buffer_is_small=true;
				else
				{
					aItBegin += _phead->FSize;
					memcpy(_founded_dg.FType.FMessageHeader, _phead, sizeof(*_phead));
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
std::pair<NUDT::required_header_t,bool> CProtocolParser::MHeader(
		const NSHARE::CConfig& aFrom) const
{
	/*! This is example of deserialization of the message header.*/

	NUDT::required_header_t _header;
	_header.FNumber=aFrom.MValue("num",0u);
	_header.FVersion=NSHARE::version_t(aFrom.MChild("ver"));
	return std::make_pair(_header, aFrom.MIsChild("num"));
}
NSHARE::CConfig CProtocolParser::MToConfig(
		const NUDT::required_header_t& aHeader) const
{
	/*! This is example of serialization  the message header.*/

	NSHARE::CConfig _conf("head");
	_conf.MAdd("num",aHeader.FNumber);
	_conf.MAdd("ver",aHeader.FVersion.MSerialize());
	return _conf;
}
bool CProtocolParser::MSwapEndian(const NUDT::required_header_t& aHeader,
		uint8_t* aItBegin, uint8_t* aItEnd) const
{
	return false;
}
bool CProtocolParser::MSwapEndian(NUDT::required_header_t* aHeader) const
{
	/*! You has to be defined method for swapping byte order
	 * of the header message.*/
	aHeader->FNumber=NSHARE::swap_endain(aHeader->FNumber);
	return true;
}
CProtocolParser::inheritances_info_t CProtocolParser::MGetInheritances() const
{
	inheritances_info_t _info;

	_info.push_back(
			msg_inheritance_t( //
					required_header_t(
							msg_head_t(E_MSG_CHILD, sizeof(child_msg_t))), //
					required_header_t(
							msg_head_t(E_MSG_GRANDCHILD,
									sizeof(grand_child_msg_t))) //
					));

	_info.push_back(
			msg_inheritance_t( //
			required_header_t(msg_head_t(E_MSG_PARENT, sizeof(parent_msg_t))), //
			required_header_t(msg_head_t(E_MSG_CHILD, sizeof(child_msg_t))) //
					));

	return _info;
}


REGISTRE_ONLY_ONE_PROTOCOL_MODULE(CProtocolParser,PROTOCOL_PARSER);

