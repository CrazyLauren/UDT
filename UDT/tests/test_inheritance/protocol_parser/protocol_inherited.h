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


namespace protocol_inherited
{
#define PROTOCOL_NAME "test_udt" ///<A protocol name

/*!\brief A type of example message
 *
 */
enum  eMsgType
{
	E_MSG_GRANDCHILD=1,///< A number message of the lowest in genealogic tree
	E_MSG_CHILD,///< A number message of the middle in genealogic tree
	E_MSG_PARENT,///< A number message of the highest in genealogic tree
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
/*!\brief A  grand child data
 */
struct grand_child_data_t
{
	uint8_t FMsg[16];///< "grand_child" + '/0'
};

/*!\brief A child data
 */
struct child_data_t
{
	uint8_t FMsg[8];///< "child" + '/0'
};

/*!\brief A parent data
 */
struct parent_data_t
{
	uint8_t FMsg[8];///< "parent" + '/0'
};

/*!\brief The Grand child message
 *
 */
struct grand_child_msg_t
{
	msg_head_t FHead;///< Header (type #E_MSG_GRANDCHILD)
	//4 bytes
	grand_child_data_t FData;///< Data
};

/*!\brief The child message
 *
 */
struct child_msg_t
{
	msg_head_t FHead;///< Header (type #E_MSG_CHILD)
	//4 bytes
	child_data_t FData;///< Data
};

/*!\brief The child message
 *
 */
struct parent_msg_t
{
	msg_head_t FHead;///< Header (type #E_MSG_PARENT)
	//4 bytes
	parent_data_t FData;///< Data
};

}

#endif /* UDT_EXAMPLE_PROTOCOL_H_ */
