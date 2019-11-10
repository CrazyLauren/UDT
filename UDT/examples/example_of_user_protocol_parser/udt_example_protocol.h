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

namespace example_of_user_protocol
{
#define PROTOCOL_NAME "pudt" ///<Unique protocol name.Recommended using a short name
#define CONFIG_PATH "./default_customer_config.json" ///< Configuration file path. The file format are xml or json.
#define MSG_TEST_SIZE 200000 ///< The size of data of message #example_of_user_protocol::E_MSG_TEST

/*!\brief A type of example message
 *
 */
enum  eMsgType
{
	E_MSG_TEST=1,///< A type of example message
	E_MSG_SWAP_BYTE_TEST=2,///< A type of message is used in the #example_customer_swap_endian.
};

/*!\brief A user error number which is passed
 * to field NUDT::fail_sent_args_t::FUserCode
 *
 *\see CExampleProtocolParser
 *\see example_parser_error
 *\see example_parser_error_filter
 */
enum eParserError
{
	E_INVALID_HEADER_SIZE=1,///< The message of header is invalid
	E_INVALID_MSG_SIZE=2,///<The message size is invalid
	E_INVALID_MSG_TYPE=3,///<The message type is invalid
};

/*!\brief A message header type
 *
 */
struct msg_head_t
{
	uint32_t  FType           : 8;            ///< A type of message eParserError
	uint32_t  FSize           : 24;           ///< A size of message include header

	/*!\brief default constructor
	 *
	 */
	msg_head_t()
	{
		memset(this,0,sizeof(*this));
	}

	/*!\brief Two objects may be passed to a object
	 * constructor to be copied
	 */
	msg_head_t(eMsgType  aType,uint32_t aSize)://
		FType(aType),//
		FSize(aSize)
	{
	}
};
/*!\brief The structure for demonstration
 * swapping byte order of message
 *
 */
struct swap_byte_order_data
{
	uint8_t FNumber;///< A test value which is used by the other fields
	uint8_t FByte1;///< A reserve
	uint8_t FByte2;///< A reserve
	uint8_t FByte3;///< A reserve
	//4 bytes

	uint16_t FUint16;///< FNumber^2
	int16_t FInt16;///< -FNumber*FNumber
	//8 bytes

	uint32_t  FUint32;///< FUint16^2
	int32_t FInt32;///< -FUint16*FUint16
	//16 bytes

	float FFloat;///< FNumber*M_PI
	//20 bytes
};

/*!\brief The example message is used in
 * swap endian example #example_customer_swap_endian
 *
 */
struct swap_byte_order_msg_t
{
	msg_head_t FHead;///< Header (type #E_MSG_SWAP_BYTE_TEST)
	//4 bytes
	swap_byte_order_data FData;///< Data
};
}

#endif /* UDT_EXAMPLE_PROTOCOL_H_ */
