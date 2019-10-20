/*
 * CMutexEmpty.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CMUTEXEMPTY_H_
#define CMUTEXEMPTY_H_

#include <share/UType/IMutex.h>
namespace NSHARE
{

/**\brief Using for removing mutex
 *
 */
struct SHARE_EXPORT CMutexEmpty:public IMutex, CDenyCopying
{
	explicit CMutexEmpty(eMutexType aType = MUTEX_RECURSIVE);

	bool MLock(void);
	bool MCanLock(void);
	bool MUnlock(void);
	eMutexType MGetMutexType() const
	{
		return FFlags;
	}

	/**\brief mutex unit test
	 *
	 *\todo
	 */
	static bool sMUnitTest();

	/**\brief thread ID which locked mutex
	 *
	 *\return thread id if locked otherwise 0
	 */
	unsigned MThread()const;

private:
	void* MGetPtr() const
	{
		return NULL;
	}

	eMutexType FFlags;
	atomic_t FThreadID;
};

template<> struct SHARE_EXPORT CRAII<CMutexEmpty> : public CDenyCopying
{
public:
	explicit CRAII(CMutexEmpty const& aMutex)
	{

	}
	void MUnlock(void)
	{
	}
};
}



#endif /* CMUTEXEMPTY_H_ */
