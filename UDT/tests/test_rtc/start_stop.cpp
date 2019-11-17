// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * start_stop.cpp
 *
 *  Created on: 20.01.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#ifdef __linux__
#	include <spawn.h>
#	include <signal.h>
#else
#	include <process.h>
#endif
#include <customer.h>
#include <SHARE/tasks.h>
#include "protocol.h"
#include "api_test.h"

using namespace NSHARE;
using namespace NUDT;
namespace test_rtc
{

static NSHARE::CMutex g_mutex(NSHARE::CMutex::MUTEX_NORMAL);
static NSHARE::CCondvar g_convar;
unsigned g_amount_of_rtc_user=0;

int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	subcribe_receiver_args_t* _recv_arg=(subcribe_receiver_args_t*)aWHAT;

	for(auto const& _it:_recv_arg->FReceivers)
	{
		{
			CRAII<CMutex> _block(g_mutex);
			++g_amount_of_rtc_user;
			g_convar.MBroadcast();
		}
	}

	return 0;
}
int event_remove_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	subcribe_receiver_args_t* _recv_arg=(subcribe_receiver_args_t*)aWHAT;
	{
		CRAII<CMutex> _block(g_mutex);
		--g_amount_of_rtc_user;
		g_convar.MBroadcast();
	}

	for(auto& _it:_recv_arg->FReceivers)
	{
		std::cout <<"-------------------------------------"<< std::endl;

	}
	return 0;
}
void start_publishers()
{
	for (auto& _v : g_child_pid)
		_v.first = start_child(_v.second.c_str());
	{
		LOCK_STREAM
		std::cout << g_name << std::endl;
		std::cout << "Child " << std::endl;
		std::cout << "  PID  " << "\t Name" << std::endl;
		for (auto& _v : g_child_pid)
			std::cout << _v.first << "\t" << _v.second << std::endl;
	}
	{
		CRAII<CMutex> _block(g_mutex);
		for (; g_amount_of_rtc_user != g_child_pid.size();)
			g_convar.MTimedwait(&g_mutex);
	}
}
void stop_publishers()
{
	send_cmd(msg_control_t::eFINISH);
	{
		CRAII<CMutex> _block(g_mutex);
		for (; g_amount_of_rtc_user;)
			g_convar.MTimedwait(&g_mutex);
	}
}

extern int start_child(char const* aName)
{
	std::string _str_t("-t");
	const int _index_t = g_argc;

	std::string _str_name("-n ");
	_str_name += aName;
	const int _index_n = g_argc + 1;

	const int _amount_of_additional_param = 2;
	const int _index_of_null = g_argc + _amount_of_additional_param;
	const unsigned _sizeof_array = g_argc + _amount_of_additional_param
			+ 1/*NULL*/;

	char const * * const _p = new char const *[_sizeof_array];

	for (int i = 0; i < g_argc; ++i)
		_p[i] = g_argv[i];

	_p[_index_t] = _str_t.c_str();
	_p[_index_n] = _str_name.c_str();
	_p[_index_of_null] = NULL;

	NSHARE::CThread::process_id_t _id = 0;
#ifdef __linux__
	int const exit_val=posix_spawn((pid_t*)&_id, g_argv[0],NULL,NULL,(char *const*)_p,(char* const*)environ);
	signal(SIGCHLD,SIG_IGN);
#else
	int const exit_val = spawnv(P_NOWAIT, g_argv[0],(char *const*) _p);
	_id = exit_val;
#endif
	if (exit_val < 0)
	{
		std::cerr << "Cannot start process " << std::endl;
		exit(EXIT_FAILURE);
	}

	delete[] _p;

	return _id;
}
}
