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

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

namespace test_rtc
{
#define PROTOCOL_NAME "tudt" ///<Unique protocol name.Recommended using a short name
enum eType
{
	E_MSG_RTC_NEXT_TIME=3,
	E_MSG_RTC_CONTROL=2,
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
		eJOIN=0x1<<1,
		eUNJOIN=0x1<<2,
		eGO_TIMER_TEST_1=0x1<<3,
		eSTART_TIMER_TEST=0x1<<4,
		eSTOP_TIMER_TEST=0x1<<5,
		eGO_TIMER_TEST_2=0x1<<6,
	};
	typedef NSHARE::CFlags<eCMD,uint32_t> cmd_t;

	cmd_t FCommand; //!< Commands of test control
	uint32_t :32;
	//2 DWORD
	char FName[8];//!< Name of RTC
	//4 DWORD
};
struct msg_next_time_t
{
	uint64_t FNextTime;//!< Next Time of calling
};
namespace
{
	static_assert(sizeof(msg_control_t)==16,"Invalid size of message");
	static_assert(sizeof(msg_next_time_t)==sizeof(uint64_t),"Invalid size of message");
}
}
#endif /* PROTOCOL_H_ */
