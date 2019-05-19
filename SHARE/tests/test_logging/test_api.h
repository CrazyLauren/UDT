/*
 * no_route_test_api.h
 *
 *  Created on: 29.04.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef TEST_API_H_
#define TEST_API_H_

namespace test_sigmentation_failed
{

/*!\brief Doing tests
 *
 */
extern void doing_tests();

/*!\brief Main function of test
 *
 */
int main(int argc, char const*argv[]);

/*! \brief The function for imitation sigmentation failed
* error.
*/
int call_function_with_segmentation(void);

/*! \brief The function number 1
*/
int call_function_1(void);

/*! \brief The function number 2
*/
int call_function_2(void);


}

#endif /* TEST_API_H_ */
