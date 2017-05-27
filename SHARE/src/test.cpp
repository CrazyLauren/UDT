/*
 * test.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <deftype>
const NSHARE::CText NSHARE::version_t::NAME="ver";
#ifndef NO_SHARE_TEST
#include <iostream>
#include <TestingConfig.h>

using namespace NSHARE;

#ifdef DEFINE_MAIN
int main()
{
	return 0;
}
#endif
static void _volatile_test()
{
	const int local = 10;
	int *ptr = (int*) &local;
	*ptr = 100;

	if (local == 10)
	{

		std::cout << "Warning - need using volatiles variable" << std::endl;
	}

}
UNIT_TEST_FUNC_ATTR void __testing__()
{
	std::cout << "Share testing before execution program." << std::endl;
	_volatile_test();
	if (!NSHARE::CMutex::sMUnitTest())
		std::cerr << "CMutex::sMUnitTest() - " << "***Failed***" << std::endl;
	if (!NSHARE::CCondvar::sMUnitTest())
		std::cerr << "CCondvar::sMUnitTest() - " << "***Failed***" << std::endl;
//	if (!CProperties<>::sMUnitTest())
//		std::cerr << "CPTree::sMUnitTest() - " << "***Failed***" << std::endl;
}
#endif//#ifndef NO_SHARE_TEST
namespace mutex_test_impl
{
using namespace NSHARE;
std::string g_buffer;
CMutex g_mutex;
eCBRval thread1_run(void*, void*, void*)
{
	for (int i = 0; i < 10; ++i)
	{
		{
			CRAII<CMutex> _block(g_mutex);

			g_buffer += "**************11111**************";
		}
		NSHARE::usleep(rand() % 500);
	}
	return E_CB_SAFE_IT;
}
eCBRval thread2_run(void*, void*, void*)
{
	for (int i = 0; i < 10;)
	{
		if (g_mutex.MCanLock())
		{
			g_buffer += "##############2222###############";
			g_mutex.MUnlock();
			++i;
		}
		NSHARE::usleep(rand() % 500);
	}
	return E_CB_SAFE_IT;
}

eCBRval thread3_run(void*, void*, void*)
{
	for (int i = 0; i < 10;)
	{
		if (g_mutex.MCanLock())
		{
			g_buffer += "##############33333###############";
			g_mutex.MUnlock();
			++i;
		}
		NSHARE::usleep(rand() % 500);
	}
	return E_CB_SAFE_IT;
}
}
bool NSHARE::CMutex::sMUnitTest()
{
	CThread t1;
	t1 += CB_t(mutex_test_impl::thread1_run, NULL);
	CThread t2;
	t2 += CB_t(mutex_test_impl::thread2_run, NULL);

	CThread t3;
	t3 += CB_t(mutex_test_impl::thread3_run, NULL);

	t1.MCreate();
	t2.MCreate();
	t3.MCreate();

	t1.MJoin();
	t2.MJoin();
	t3.MJoin();
	return true;
}
namespace condvar_test_impl
{
using namespace NSHARE;
char buffer[50] = "";
CMutex bufferLock(CMutex::MUTEX_NORMAL);
CCondvar cond;
bool ok = true;
eCBRval thread1_run(void*, void*, void*)
{
	NSHARE::usleep(1000);
	for (int i = 0; i < 10; ++i)
	{

		bufferLock.MLock();

		for (HANG_INIT;buffer[0] != '\0' ; HANG_CHECK)
		{
			cond.MTimedwait(&bufferLock);
		}

		sprintf(buffer, "%d", i);
		bufferLock.MUnlock();

		NSHARE::usleep(rand() % 500);
	}
	return E_CB_SAFE_IT;
}

eCBRval thread2_run(void*, void*, void*)
{
	for (int i = 0; i < 10;)
	{

		bufferLock.MLock();
		for (; strlen(buffer); NSHARE::usleep(10000))
		{
			char _str[5];
			sprintf(_str, "%d", i);
			ok = (strcmp(buffer, _str) == 0) && ok;
			buffer[0] = '\0';

			cond.MSignal();
			++i;
		}
		bufferLock.MUnlock();

		NSHARE::usleep(rand() % 500);
	}
return E_CB_SAFE_IT;
}
}
bool NSHARE::CCondvar::sMUnitTest()
{
	CThread t1;
	t1 += CB_t(condvar_test_impl::thread1_run, NULL);
	CThread t2;
	t2 += CB_t(condvar_test_impl::thread2_run, NULL);

	t1.MCreate();
	t2.MCreate();

	t1.MJoin();
	t2.MJoin();
	return condvar_test_impl::ok;
}

