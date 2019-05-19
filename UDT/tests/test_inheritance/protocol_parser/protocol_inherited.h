/*
 * protocol_inherited.h
 *
 *  Created on:  12.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_EXAMPLE_PROTOCOL_H_
#define UDT_EXAMPLE_PROTOCOL_H_


namespace protocol_inherited
{
static const char* PROTOCOL_NAME="test_udt"; ///<A protocol name

/*!\brief A type of example message
 *
 */
enum class eMsgType:uint8_t
{
	E_MSG_SUB_MESSAGE=0,///< A number message of the middle in genealogic tree
	E_MSG_SUB_SUB_MESSAGE,///< A number message of the lowest in genealogic tree
	E_MSG_PARENT,///< A number message of the highest in genealogic tree
	E_MSG_LAST_NUMBER
};
template<eMsgType> constexpr const char g_enum_name[]="Unknown";
template<> constexpr const char g_enum_name<eMsgType::E_MSG_SUB_SUB_MESSAGE>[]="sub_sub_msg";
template<> constexpr const char g_enum_name<eMsgType::E_MSG_SUB_MESSAGE>[]="sub_msg";
template<> constexpr const char g_enum_name<eMsgType::E_MSG_PARENT>[]="parent";

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
	eMsgType    FType;            ///< A type of message eParserError
	uint8_t 	FReserv;
	uint16_t  	FSize;           ///< A size of message include header

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
		FReserv(0),//
		FSize(aSize)
	{
	}
};
static_assert(sizeof(msg_head_t) == sizeof(uint32_t), "invalid size of message header");
/*! \brief Calculate requirement size of array
 * for saving string
 *
 */
template<unsigned N>
constexpr unsigned array_size_for(char const (&) [N])
{
	return ((N-1)%sizeof(uint64_t))*sizeof(uint64_t);
}

/*!\brief A  grand child data
 */
struct sub_sub_msg_data_t
{
	char FMsg[array_size_for(g_enum_name<eMsgType::E_MSG_SUB_SUB_MESSAGE>)];///< "grand_child" + '/0'

	sub_sub_msg_data_t()
	{
		strcpy( FMsg, g_enum_name<eMsgType::E_MSG_SUB_SUB_MESSAGE>);
	}
};

/*!\brief A child data
 */
struct sub_msg_data_t:sub_sub_msg_data_t
{
	char FMsg[array_size_for(g_enum_name<eMsgType::E_MSG_SUB_MESSAGE>)];///< "child" + '/0'

	sub_msg_data_t()
	{
		strcpy( FMsg, g_enum_name<eMsgType::E_MSG_SUB_MESSAGE>);
	}
};

/*!\brief A parent data
 */
struct parent_data_t:sub_msg_data_t
{
	char FMsg[array_size_for(g_enum_name<eMsgType::E_MSG_PARENT>)];///< "parent" + '/0'

	parent_data_t()
	{
		strcpy( FMsg, g_enum_name<eMsgType::E_MSG_PARENT>);
	}
};

/*!\brief The Grand child message
 *
 */
struct sub_sub_msg_t
{
	static constexpr eMsgType type(){return eMsgType::E_MSG_SUB_SUB_MESSAGE;};///< Number of message

	msg_head_t FHead;///< Header (type #E_MSG_GRANDCHILD)
	//4 bytes
	sub_sub_msg_data_t FData;///< Data
};

/*!\brief The child message
 *
 */
struct sub_msg_t
{
	static constexpr eMsgType type(){return eMsgType::E_MSG_SUB_MESSAGE;};

	msg_head_t FHead;///< Header (type #E_MSG_CHILD)
	//4 bytes
	sub_msg_data_t FData;///< Data
};

/*!\brief The parent message
 *
 */
struct parent_msg_t
{
	static constexpr eMsgType type(){return eMsgType::E_MSG_PARENT;};///< Number of message

	msg_head_t FHead;///< Header (type #E_MSG_PARENT)
	//4 bytes
	parent_data_t FData;///< Data
};

}

#endif /* UDT_EXAMPLE_PROTOCOL_H_ */
