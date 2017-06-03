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
#include <unit_tests.h>

const NSHARE::CText NSHARE::version_t::NAME = "ver";//by historical reason

#ifdef START_UNIT_TESTING
int main(int argc, char *argv[])
{
	init_trace(argc, argv);
	return NSHARE::unit_testing()?0:-1;
}
#endif

namespace NSHARE
{
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
UNIT_TEST_FUNC_ATTR bool unit_testing()
{
	std::cout << "Share testing ..." << std::endl;
	_volatile_test();
	if (!NSHARE::CMutex::sMUnitTest())
	{
		std::cerr << "CMutex::sMUnitTest() - " << "***Failed***" << std::endl;
		return false;
	}
	if (!NSHARE::CCondvar::sMUnitTest())
	{
		std::cerr << "CCondvar::sMUnitTest() - " << "***Failed***" << std::endl;
		return false;
	}

	if (!NSHARE::CText::sMUnitTest())
	{
		std::cerr << "CText::sMUnitTest() - " << "***Failed***" << std::endl;
		return false;
	}

	if (!NSHARE::CBuffer::sMUnitTest(NULL))//todo for several allocators
	{
		std::cerr << "CBuffer::sMUnitTest() - " << "***Failed***" << std::endl;
		return false;
	}
	if (!NSHARE::CRegistration::sMUnitTest())
	{
		std::cerr << "CAddress::sMUnitTest() - " << "***Failed***" << std::endl;
		return false;
	}
	return true;
}
/****************************************************
 *		      		Mutex test		     			*
 ****************************************************/
template<typename TMutex>
struct CMutexTestImpl
{
	TMutex mutex;
	atomic_t thread_number;
	volatile bool is;
	std::vector<SHARED_PTR<CThread> > threads;

	bool MIs()
	{
		for(unsigned i=0;i<20;++i)
		{
			threads[i]->MCreate();
		}
		for(unsigned i=0;i<20;++i)
		{
			threads[i]->MJoin();
		}
		return is;
	}
	CMutexTestImpl()
	{
		thread_number=0;
		is=true;
		for(unsigned i=0;i<20;++i)
		{
			threads.push_back(SHARED_PTR<CThread>(new CThread));
			*threads.back()+=CB_t(thread_run, this);
		}
	}

	static eCBRval thread_run(void*, void*, void* aData)
	{
		CMutexTestImpl<TMutex>* _p=reinterpret_cast<CMutexTestImpl<TMutex>*>(aData);
		if(_p->is)
		{
			CRAII < TMutex > _block(_p->mutex);
			for (int i = 0; i < 100; ++i)
			{
				++_p->thread_number;
				CThread::sMYield();
			}
			if(_p->thread_number!=100)
			_p->is=false;

			_p->thread_number=0;
		}
		return E_CB_SAFE_IT;
	}

};
template<typename Mutex>
bool test_mutex()
{
	{
		Mutex _mutex1(Mutex::MUTEX_NORMAL);
		Mutex _mutex2(Mutex::MUTEX_NORMAL);
		Mutex _mutex3(Mutex::MUTEX_NORMAL);
		{
			_mutex1.MLock();
			_mutex2.MLock();
			_mutex3.MLock();

			_mutex1.MUnlock();
			_mutex2.MUnlock();
			_mutex3.MUnlock();
		}
		{
			_mutex1.MLock();
			_mutex2.MLock();
			_mutex3.MLock();

			_mutex3.MUnlock();
			_mutex2.MUnlock();
			_mutex1.MUnlock();
		}
	}
	{
		bool _is = true;
		Mutex _mutex1(Mutex::MUTEX_RECURSIVE);
		Mutex _mutex2(Mutex::MUTEX_RECURSIVE);

		_mutex1.MLock();
		_mutex2.MLock();
		_is = _mutex1.MCanLock() && _is;
		_is = _mutex2.MCanLock() && _is;

		_mutex1.MUnlock();
		_mutex2.MUnlock();

		_mutex1.MUnlock();
		_mutex2.MUnlock();
		if (!_is)
			return false;
	}
	CMutexTestImpl<Mutex> _test;
	return _test.MIs();
}

bool CMutex::sMUnitTest()
{
	return test_mutex<CMutex>();
}
/****************************************************
 *		      		Condvar test		     		*
 ****************************************************/
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

		for (HANG_INIT;buffer[0] != '\0'; HANG_CHECK)
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
bool CCondvar::sMUnitTest()
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
}
