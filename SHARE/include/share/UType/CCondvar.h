/*
 * CCondvar.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 04.04.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#ifndef CCONDVAR_H_
#define CCONDVAR_H_

#include <share/UType/IConditionVariable.h>

namespace NSHARE
{
/**\brief condition variable
 *
 */
class SHARE_EXPORT CCondvar:public IConditionVariable,CDenyCopying
{
public:

	/** Create condition variable
	 *
	 */
	CCondvar();
	~CCondvar();

	/** @copydoc IConditionVariable::MSignal
	 *
	 */
	bool MSignal(void);

	/** @copydoc IConditionVariable::MTimedwait(IMutex)
	 *
	 */
	bool MTimedwait(IMutex * aMutex);

	/** @copydoc IConditionVariable::MTimedwait(IMutex,double const)
	 *
	 */
	bool MTimedwait(IMutex * aMutex, double const aTime);

	/** @copydoc IConditionVariable::MTimedwait
	 *
	 */
	bool MBroadcast(void);

	/** Unit test
	 *
	 */
	static bool sMUnitTest();
private:
	struct CImpl;
	CImpl* FPImpl;
};
}


#endif /* CCONDVAR_H_ */
