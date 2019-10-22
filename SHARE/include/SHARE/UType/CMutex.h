/*
 * CMutex.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef MUTEX_H_
#define MUTEX_H_

#include <SHARE/UType/IMutex.h>

namespace NSHARE
{

class SHARE_EXPORT CMutex:public IMutex, CDenyCopying
{
public:

	/** Create the new mutex
	 *
	 */
	explicit CMutex(eMutexType aType = MUTEX_RECURSIVE);
	~CMutex(void);

	/** @copydoc IMutex::MLock
	 *
	 */
	bool MLock(void);

	/** @copydoc IMutex::MCanLock
	 *
	 */
	bool MCanLock(void);

	/** @copydoc IMutex::MUnlock
	 *
	 */
	bool MUnlock(void);

	/** @copydoc IMutex::MGetMutexType
	 *
	 */
	eMutexType MGetMutexType() const;

	/**\brief mutex unit test
	 *
	 *\note realization in test.cpp
	 */
	static bool sMUnitTest();

	/**\brief thread ID which locked mutex
	 *
	 *\return thread id if locked otherwise 0
	 */
	unsigned MThread()const;

private:
	struct CImpl;

	/**\brief using in Condvar
	 *
	 */
	void* MGetPtr() const;

	eMutexType FFlags;
	CImpl *FImpl;

	friend class CCondvar;
};
inline CMutex::eMutexType CMutex::MGetMutexType() const
{
	return FFlags;
}
template<> class  CRAII<CMutex> : public CDenyCopying
{
public:
	explicit CRAII(CMutex & aMutex) :
			FMutex(aMutex)
	{
		MLock();
	}
	~CRAII()
	{
		MUnlock();
	}
	inline void MUnlock()
	{
		FIsLock && FMutex.MUnlock();
		FIsLock = false;
	}
private:
	inline void MLock()
	{
		FIsLock = FMutex.MLock();
	}
	CMutex &FMutex;
	volatile bool FIsLock;
};
template<> class  CRAII<CMutex*>
{
public:
	explicit CRAII(CMutex* aMutex) :
			CRaii(*aMutex)
	{
	}
	~CRAII(void)
	{
	}
	void MUnlock(void)
	{
		return CRaii.MUnlock();
	}
private:
	CRAII<CMutex> CRaii;
};
/**\brief Используется в шаблонах, для указания отсуствия mutex
 * в отличии от CMutexEmpty, этот класс можно копировать
 *
 */
class  SHARE_EXPORT CNoMutex
{
public:
	typedef CMutex::eMutexType eMutexType;
	explicit CNoMutex(CMutex::eMutexType aType = CMutex::MUTEX_RECURSIVE)
	{
	}
	bool MLock(void)
	{
		return true;
	}
	bool MCanLock(void)
	{
		return true;
	}
	bool MUnlock(void){
		return true;
	}

	eMutexType MGetMutexType() const
	{
		return CMutex::MUTEX_RECURSIVE;
	}

	/**\brief mutex unit test
	 *
	 *\todo
	 */
	static bool sMUnitTest();

	unsigned MThread() const
	{
		return 0;
	}

private:
	void* MGetPtr() const
	{
		return NULL;
	}

};
template<> struct SHARE_EXPORT CRAII<CNoMutex> : public CDenyCopying
{
public:
	explicit CRAII(CNoMutex const& aMutex)
	{

	}
	void MUnlock(void)
	{
	}
};
} //namespace USHARE


#endif /*MUTEX_H_*/
