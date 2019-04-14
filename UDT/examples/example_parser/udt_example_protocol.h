/*
 * udt_example_protocol.h
 *
 *  Created on:  20.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_EXAMPLE_PROTOCOL_H_
#define UDT_EXAMPLE_PROTOCOL_H_

#define PROTOCOL_NAME "pudt" ///<Unique protocol name.Recommended using a short name
#define CONFIG_PATH "./default_customer_config.xml" ///< Configuration file path. The file format are xml or json.
#define PACKET_SIZE 200000

enum  eMsgType
{
	E_MSG_TEST=1,///< test message
	E_MSG_SWAP_BYTE_TEST=2,
};
enum eParserError ///< uint8_t with error returned by fail_sent_args_t .FUserCode
{
	E_INVALID_HEADER_SIZE=1,
	E_INVALID_MSG_SIZE=2,
	E_INVALID_MSG_TYPE=3,
};

struct msg_head_t
{
	uint32_t  FType           : 8;            ///< Type of msg E_MSG_*
	uint32_t  FSize           : 24;           ///< Size of msg include header

	msg_head_t()
	{
		memset(this,0,sizeof(*this));
	}

	msg_head_t(eMsgType  aType,uint32_t aSize)://
		FType(aType),//
		FSize(aSize)
	{
	}
};
struct msg_data
{
	uint8_t FNumber;
	uint8_t FByte1;
	uint8_t FByte2;
	uint8_t FByte3;
	//4 bytes

	uint16_t FUint16;
	int16_t FInt16;
	//8 bytes

	uint32_t  FUint32;
	int32_t FInt32;
	//16 bytes

	float FFloat;
	//20 bytes
};
struct test_msg_t
{
	msg_head_t FHead;///< Header (E_MSG_SWAP_BYTE_TEST)
	//4 bytes
	msg_data FData;
};

#endif /* UDT_EXAMPLE_PROTOCOL_H_ */
