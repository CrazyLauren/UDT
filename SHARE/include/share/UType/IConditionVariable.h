/*
 * IConditionVariable.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ICONDITIONVARIABLE_H_
#define ICONDITIONVARIABLE_H_

namespace NSHARE
{

/** Interface of condition variable
 *
 * @warning Don't use a recursive mutex with condition variables.
 * @warning Make sure that the thread's cleanup handlers unlock the mutex.
 */
class SHARE_EXPORT IConditionVariable
{
public:

	/** @brief Unblock a thread that's waiting on a condition variable
	 *
	 * @return true if no error
	 */
	virtual bool MSignal(void)=0;

	/** @brief Wait on condition variable
	 *
	 * @param aMutex - mutex for lock
	 * @return true if no error
	 */
	virtual bool MTimedwait(IMutex * aMutex)=0;

	/** @brief Wait on condition variable
	 *
	 * @param aMutex - mutex for lock
	 * @param aTime - timeout
	 * @return true if no error
	 */
	virtual bool MTimedwait(IMutex * aMutex, double const aTime)=0;

	/** @brief Unblock threads waiting on condition
	 *
	 * @return true if no error
	 */
	virtual bool MBroadcast(void)=0;


	virtual ~IConditionVariable()
	{

	}
};
}


#endif /* ICONDITIONVARIABLE_H_ */
