/*
 * protocol.h
 *
 *  Created on: 20.01.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef UDT_TESTS_TEST_SELECTION_NEAREST_SUBSCRIBE_PROTOCOL_H_
#define UDT_TESTS_TEST_SELECTION_NEAREST_SUBSCRIBE_PROTOCOL_H_

namespace test_selection
{
#define PROTOCOL_NAME "tudt" ///<Unique protocol name.Recommended using a short name
enum eType
{
	E_MSG_CONTROL=1,
	E_TEST_MSG,
};
struct msg_head_t
{
	uint32_t  FType           : 8;///< Type of msg E_MSG_*
	uint32_t  FReserved       : 24;
};

struct msg_control_t
{
	enum eCMD:uint32_t
	{
		eFINISH=0x1<<0,
		eSEND=0x1<<1,
		eUNSUBSCRIBE=0x1<<2,
	};
	typedef NSHARE::CFlags<eCMD,uint32_t> cmd_t;

	cmd_t FCommand; ///< Commands of test control
	//1 DWORD
};
struct msg_test_t
{
	uint32_t :32;
};
namespace
{
	static_assert(sizeof(msg_control_t)==4,"Invalid size of message");
	static_assert(sizeof(msg_test_t)==4,"Invalid size of message");
}
}
#endif /* UDT_TESTS_TEST_SELECTION_NEAREST_SUBSCRIBE_PROTOCOL_H_ */
