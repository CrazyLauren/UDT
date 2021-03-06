// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * trace_cout.cpp
 *
 *  Created on: 24.11.2016
 *      Author:  https://github.com/CrazyLauren
 *	
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#if defined(COUT_LOG)
#include <deftype>
namespace NSHARE
{
	namespace logging_impl
	{
		static NSHARE::CMutex g_cout_mutex;//fixme resource leak

		SHARE_EXPORT void lock_cout_mutex() {
			g_cout_mutex.MLock();
		}
		SHARE_EXPORT void unlock_cout_mutex() {
			g_cout_mutex.MUnlock();
		}
		static NSHARE::CMutex g_err_mutex;
		SHARE_EXPORT void lock_cerr_mutex() {
			g_err_mutex.MLock();
		}
		SHARE_EXPORT void unlock_cerr_mutex() {
			g_err_mutex.MUnlock();
		}
	}
}
#endif
