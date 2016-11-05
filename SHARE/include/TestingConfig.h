/*
 *
 * TestingConfig.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.09.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef TESTINGCONFIG_H_
#define TESTINGCONFIG_H_

//#define TESTING_ON_THE_STRATUM

#ifdef TESTING_ON_THE_STRATUM
#	define UNIT_TEST_FUNC_ATTR	 extern "C" __attribute__((constructor))
#else
#	define UNIT_TEST_FUNC_ATTR	extern "C"
#endif//#ifdef TESTING_ON_THE_STRATUM


#endif /* TESTINGCONFIG_H_ */
