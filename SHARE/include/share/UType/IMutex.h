/*
 * IMutex.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IMUTEX_H_
#define IMUTEX_H_


namespace NSHARE
{
/** Interface of semaphore
 *
 */
class SHARE_EXPORT IMutex
{
public:
	/** Type of mutex
	 *
	 */
	enum eMutexType
	{
		MUTEX_NORMAL=1,//!< Mutex cannot be locked twice in thread
		MUTEX_RECURSIVE//!< Mutex can be locked repeated in the thread
	};

	/** @brief Lock mutex
	 *
	 * @return true if no error
	 */
	virtual bool MLock(void) =0;

	/** @brief Attempt to lock a mutex
	 *
	 * @return true if no error
	 */
	virtual bool MCanLock(void)  =0;

	/** @brief Unlock mutex
	 *
	 * @return true if no error
	 */
	virtual bool MUnlock(void)  =0;


	/** Gets mutex type
	 *
	 */
	virtual eMutexType MGetMutexType() const  =0;

	virtual ~IMutex()
	{

	}
};

template<> class SHARE_EXPORT CRAII<IMutex> : public CDenyCopying
{
public:
	explicit CRAII(IMutex & aSem) :
			FMutex(aSem)
	{
		MLock();
	}
	~CRAII()
	{
		MUnlock();
	}
	inline void MUnlock()
	{
		if (FIsLock)
			FMutex.MUnlock();
		FIsLock = false;
	}
private:
	inline void MLock()
	{
		FMutex.MLock();
		FIsLock =true;
	}
	IMutex &FMutex;
	volatile bool FIsLock;
};
}


#endif /* IMUTEX_H_ */
