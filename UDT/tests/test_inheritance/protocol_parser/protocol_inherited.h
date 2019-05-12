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
enum  eMsgType
{
	E_MSG_GRANDCHILD=0,///< A number message of the lowest in genealogic tree
	E_MSG_CHILD,///< A number message of the middle in genealogic tree
	E_MSG_PARENT,///< A number message of the highest in genealogic tree
	E_MSG_LAST_NUMBER
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
/*! \brief Calculate requirement size of array
 * for saving string
 *
 */
template<unsigned N>
constexpr unsigned array_size_for(char const (&) [N])
{
	return ((N-1)%sizeof(uint64_t))*sizeof(uint64_t);
}

static const char string_grand []="grand_child";
/*!\brief A  grand child data
 */
struct grand_child_data_t
{
	constexpr auto const MString() const{return string_grand; }

	char FMsg[array_size_for(string_grand)];///< "grand_child" + '/0'
};

static const char string_child[]="grand_child";
/*!\brief A child data
 */
struct child_data_t
{
	constexpr auto const MString() const {return string_child; }

	char FMsg[array_size_for(string_child)];///< "child" + '/0'
};

static const char string_parent[]="parent";
/*!\brief A parent data
 */
struct parent_data_t
{
	constexpr auto const MString()const{return string_parent; }

	char FMsg[array_size_for(string_parent)];///< "parent" + '/0'
};

/*!\brief The Grand child message
 *
 */
struct grand_child_msg_t
{
	enum
	{
		eType=E_MSG_GRANDCHILD,///< Number of message
	};

	msg_head_t FHead;///< Header (type #E_MSG_GRANDCHILD)
	//4 bytes
	grand_child_data_t FData;///< Data
};

/*!\brief The child message
 *
 */
struct child_msg_t
{
	enum
	{
		eType = E_MSG_CHILD,	///< Number of message
	};

	msg_head_t FHead;///< Header (type #E_MSG_CHILD)
	//4 bytes
	child_data_t FData;///< Data
};

/*!\brief The child message
 *
 */
struct parent_msg_t
{
	enum
	{
		eType = E_MSG_PARENT,	///< Number of message
	};

	msg_head_t FHead;///< Header (type #E_MSG_PARENT)
	//4 bytes
	parent_data_t FData;///< Data
};

}

#endif /* UDT_EXAMPLE_PROTOCOL_H_ */
