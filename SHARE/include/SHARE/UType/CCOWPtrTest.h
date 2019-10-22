/*
 * CCOWPtrTest.h
 *
 * Copyright © 2017  https://github.com/CrazyLauren
 *
 *  Created on: 06.06.2017
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef INCLUDE_UTYPE_CCOWPTRTEST_H_
#define INCLUDE_UTYPE_CCOWPTRTEST_H_

namespace NSHARE
{
template<typename T>
inline bool CCOWPtr<T>::sMUnitTest()
{
	typedef CCOWPtr<T> cow_t;
	CCommonAllocater<> _test_alloctor;
	{
		cow_t _cow(&_test_alloctor);
		CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),1);
	}
	CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),0);

	{
		cow_t _1(&_test_alloctor);//allocation
		cow_t _2=_1;//no allocation

		CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),1);

		*_2.get()=cow_t::value_type();//allocation
		CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),2);

		_2=_1;//deallocation
		CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),1);
	}
	CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),0);
	{
		cow_t _1(&_test_alloctor);//allocation
		cow_t _2(&_test_alloctor);//allocation
		CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),2);

		using namespace std;
		swap(_1, _2);

		CHECK_EQ(_test_alloctor.MGetNumberOfAllocations(),2);
	}
	return _test_alloctor.MGetNumberOfAllocations()==0;
}

}

#endif /* INCLUDE_UTYPE_CCOWPTRTEST_H_ */
