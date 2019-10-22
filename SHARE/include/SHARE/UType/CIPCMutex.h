/*
 * CIPCMutex.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 06.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CIPCMUTEX_H_
#define CIPCMUTEX_H_

#ifdef _WIN32
#	define USING_MUTEX_ON_SEMAPHORE
#endif

#include <SHARE/UType/IMutex.h>
#ifdef USING_MUTEX_ON_SEMAPHORE
#	include <SHARE/UType/CIPCSem.h>
#endif


namespace NSHARE
{
/**\brief Crossplatform realization of interprocessor mutex
 *
 * There are 3 realization:
 * 1) using Semaphore (in Windows)
 * 2) using Semaphore (in linux)
 * 3) using mutex (linux only)
 *
 */
class SHARE_EXPORT CIPCMutex:public IMutex, CDenyCopying
{
public:
	/**\brief Info about required buffer size
	 *
	 */
	enum
	{
#ifdef USING_MUTEX_ON_SEMAPHORE
		eReguredBufSize=CIPCSem::eReguredBufSize
#else
		eReguredBufSize=SIZEOF_PTHREAD_MUTEX_T+2*sizeof(uint32_t)+__alignof(void*)
#endif
	};

	/** @brief Semaphore creation control flags
	 *
	 */
	enum eOpenType
	{
		E_UNDEF,//!< If the semaphore is exist then opens it, otherwise creates it
		E_HAS_TO_BE_NEW,//!<If the semaphore is exist then return error, otherwise creates it
		E_HAS_EXIST//!<If the semaphore isn't exist then return error, otherwise opens it
	};

	/** Default constructor
	 *
	 */
	CIPCMutex();

	/**	@brief Create or Open interprocessor semaphore
	 *
	 *	@param aBuf pointer to buffer of shared memory,
	 *			where it will be saved or null terminated string
	 *	@param aSize sizeof buffer (has to be greate then
	 *			eReguredBufSize)
	 *	@param aType \see #eOpenType
	 *
	 */
	CIPCMutex(uint8_t* aBuf, size_t aSize,eOpenType const aType=E_UNDEF);

	/** If open then close semaphore but not removed
	 *
	 */
	~CIPCMutex();

	/**	@brief Create or Open interprocessor semaphore
	 *
	 *	@param aBuf pointer to buffer of shared memory,
	 *			where it will be saved or null terminated string
	 *	@param aSize sizeof buffer (has to be greate then
	 *			eReguredBufSize)
	 *	@param aHasToBeNew \see #eOpenType
	 *
	 *	@return true if no error
	 */
	bool MInit(uint8_t* aBuf, size_t aSize,eOpenType aHasToBeNew=E_UNDEF);

	/** @brief "Deinitialize" the semaphore
	 *
	 * @warning The semaphore is not removed, but becomes not initialized
	 */
	void MFree();

	/** Checks for initialization semaphore
	 *
	 *	@return true if initialized
	 */
	bool MIsInited()const;

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

	/**\brief возвращет уникальный ID семафора, если он существует
	 *
	 *\return - пустую строку в случае ошибки
	 * 			- ID семафора
	 */
	NSHARE::CText const& MName()const;

	/**\brief Destroy a semaphore
	 *
	 */
	void MUnlink();

	/**\brief Gets in which mode the semaphore was opened
	 *
	 *\return see #eOpenType
	 *
	 */
	eOpenType MGetType() const;

	/** Gets requirement size of buffer
	 *
	 * @return size of buffer in bytes
	 */
	static size_t sMRequredBufSize();


	/**\brief Unit test
	 *
	 *\note realization in test.cpp
	 */
	static bool sMUnitTest();

	struct CImpl;
private:

	/**\brief using in Condvar
	 *
	 */
	void* MGetPtr() const;

	CImpl *FImpl;
	eOpenType FType;
	friend class CIPCSignalEvent;
};
inline CIPCMutex::eOpenType CIPCMutex::MGetType() const
{
	return FType;
}

template<> class  CRAII<CIPCMutex> : public CDenyCopying
{
public:
	explicit CRAII(CIPCMutex & aMutex) :
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
	CIPCMutex &FMutex;
	volatile bool FIsLock;
};
}

#endif /* CIPCMUTEX_H_ */
