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

namespace NSHARE
{

class SHARE_EXPORT CMutex: CDenyCopying
{
public:
	// Construction | Destruction
	enum eMutexType
	{
		MUTEX_NORMAL=1,
		MUTEX_RECURSIVE
	};
	explicit CMutex(eMutexType aType = MUTEX_RECURSIVE);
	~CMutex(void);


	bool MLock(void);
	bool MCanLock(void);
	bool MUnlock(void);
	eMutexType MGetMutexType() const
	{
		return FFlags;
	}



	/** \brief mutex unit test
	 *
	 * \note realization in test.cpp
	 */
	static bool sMUnitTest();

	/** \brief thread ID which locked mutex
	 *
	 *	\return thread id if locked otherwise 0
	 */
	unsigned MThread()const;

private:
	struct CImpl;

	/** \brief using in Condvar
	 *
	 */
	void* MGetPtr() const;

	eMutexType FFlags;
	CImpl *FImpl;

	friend class CCondvar;
};
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

/** \brief Используется в шаблонах, для указания отсуствия mutex
 *
 * \note реализация находится в CWin32Mutex.cpp
 */
struct SHARE_EXPORT CMutexEmpty: CDenyCopying
{
	typedef CMutex::eMutexType eMutexType;

	explicit CMutexEmpty(CMutex::eMutexType aType = CMutex::MUTEX_RECURSIVE) :
			FFlags(aType)
	{
		FThreadID = 0;
	}
	bool MLock(void);
	bool MCanLock(void);
	bool MUnlock(void);
	eMutexType MGetMutexType() const
	{
		return FFlags;
	}

	/** \brief mutex unit test
	 *
	 * \todo
	 */
	static bool sMUnitTest();

	/** \brief thread ID which locked mutex
	 *
	 *	\return thread id if locked otherwise 0
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
/** \brief Используется в шаблонах, для указания отсуствия mutex
 * в отличии от CMutexEmpty, этот класс можно копировать
 *
 */
struct  CNoMutex
{
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

	/** \brief mutex unit test
	 *
	 * \todo
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
