// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * api_test.h
 *
 *  Created on: 20.01.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef API_TEST_H_
#define API_TEST_H_

#include <customer.h>
#include <udt/IRtc.h>
#include "protocol.h"

namespace test_rtc
{
#define LOCK_STREAM NSHARE::CRAII<NSHARE::CIPCSem> _block(g_mutex_stream); \
					std::cout<<std::endl\
					<<( g_is_child?"||||||| ":\
								   "_______ ")<<g_name<<" say:"<<std::endl;

extern double const g_start_time;
extern NSHARE::CIPCSem g_mutex_stream;
extern std::string g_name;
extern bool g_is_child;
extern std::vector<std::pair<ptrdiff_t, std::string> > g_child_pid;
extern std::string const g_subscriber_name;
extern std::string const g_publisher_name;
extern const char * g_rtc_name;
extern int g_argc;
extern char const** g_argv;
extern unsigned g_amount_of_rtc_user;
extern  int start_child(char  const* aName);
extern void start_publishers();
extern void stop_publishers();
extern int event_new_receiver(NUDT::CCustomer* WHO, void *aWHAT, void* YOU_DATA);
extern int event_remove_receiver(NUDT::CCustomer* WHO, void *aWHAT, void* YOU_DATA);

extern void send_cmd(msg_control_t::eCMD aCmd, NSHARE::uuid_t const & aTo =
		NSHARE::uuid_t());

extern void send_nextime(NUDT::IRtc::millisecond_t aTime, NSHARE::uuid_t const & aTo =
		NSHARE::uuid_t());

extern void send_nextime(NUDT::IRtc::time_in_second_t aTime, NSHARE::uuid_t const & aTo =
		NSHARE::uuid_t());

extern bool start_rtc_control(int argc, char const*argv[],char const * aName);
extern void start_rtc_user(int argc, char const *argv[], char const * aName);

}



#endif /* API_TEST_H_ */
