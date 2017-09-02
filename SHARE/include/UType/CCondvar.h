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

namespace NSHARE
{
#ifdef _MSC_VER
	struct timespec;
#endif
/** \brief condition variable
 *
 */
class SHARE_EXPORT CCondvar:CDenyCopying
{
public:
// Construction | Destruction
	CCondvar();
	~CCondvar(void);

	bool MSignal(void);
	bool MTimedwait(CMutex *, const struct timespec* = NULL);
	bool MTimedwait(CMutex *, double const);
	bool MBroadcast(void);

	static bool sMUnitTest();
private:
	struct CImpl;
	CImpl* FPImpl;
};
}


#endif /* CCONDVAR_H_ */
