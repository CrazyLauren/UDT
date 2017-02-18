/*
 * udt_example_protocol.h
 *
 *  Created on: 20.07.2016
 *      Author: Sergey
 */

#ifndef UDT_EXAMPLE_PROTOCOL_H_
#define UDT_EXAMPLE_PROTOCOL_H_

#define PROTOCOL_NAME "pudt" //!<Unique protocol name.Recommended using a short name
#define CONFIG_PATH "./example_customer2.xml" //!< Configuration file path. The file format are xml or json.

enum  eMsgType
{
	E_MSG_TEST=1,//!< test message

};
enum eParserError //!< uint8_t with error returned by fail_sent_args_t .FUserCode
{
	E_INVALID_HEADER_SIZE=1,
	E_INVALID_MSG_SIZE=2,
};

struct msg_head_t
{
	uint32_t  FType           : 8;            //!< Type of msg E_MSG_*
	uint32_t  FSize           : 24;           //!< Size of msg include header
};
struct test_msg_t
{
	msg_head_t FHead;//!< Header (E_MSG_TEST)
};

#endif /* UDT_EXAMPLE_PROTOCOL_H_ */
