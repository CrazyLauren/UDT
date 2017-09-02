/*
 * unit_tests.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef INCLUDE_UNIT_TESTS_H_
#define INCLUDE_UNIT_TESTS_H_

#ifdef TESTING_ON_THE_STRATUM
#	define UNIT_TEST_FUNC_ATTR	 extern "C" __attribute__((constructor))
#else
#	define UNIT_TEST_FUNC_ATTR	extern "C"
#endif//#ifdef TESTING_ON_THE_STRATUM

//START_UNIT_TESTING

namespace NSHARE
{
	UNIT_TEST_FUNC_ATTR SHARE_EXPORT bool unit_testing();
}


#endif /* INCLUDE_UNIT_TESTS_H_ */
