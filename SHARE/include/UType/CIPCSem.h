/*
 * CIPCSem.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 06.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CIPCSEM_H_
#define CIPCSEM_H_

#include <UType/ISemaphore.h>

#ifndef _WIN32
#	if defined(__linux__) && defined(USING_FUTEX)
#		define SEM_USING_FUTEX //!< using futex
#	else
#		define SM_USING_SEM_INIT
#	endif
#endif
namespace NSHARE
{
/**\brief Crossplatform realization of interprocessor semaphore
 *
 * There are 4 realization:
 * 1) using CreateSemaphore (in Windows)
 * 2) using futex (linux only if defined USING_FUTEX preprocessor)
 * 3) using sem_init (any posix OS), default for posix OS
 * 4) using sem_open (any posix), only if defined SM_USING_SEM_OPEN preprocessor
 *
 * For working of semaphore need to allocate shared memory.
 * If you doesn't want to allocate memory, you can pass, instead of
 * pointer to buffer, an unique name of semaphore (length of name
 * has to be less eReguredBufSize-1).
 *
 */
class SHARE_EXPORT CIPCSem:public ISemaphore, CDenyCopying
{
public:
	/**\brief Info about required buffer size
	 *
	 */
	enum
	{
# ifdef _WIN32
		eReguredBufSize=16
#elif defined(SEM_USING_FUTEX)
		eReguredBufSize=sizeof(int32_t)+2*4+4
#elif defined(SM_USING_SEM_INIT)
		eReguredBufSize=SIZEOF_SEM_T+2*sizeof(uint32_t)+__alignof(void*)
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
	CIPCSem();

	/**	@brief Create or Open interprocessor semaphore
	 *
	 *	@param aBuf pointer to buffer of shared memory,
	 *			where it will be saved or null terminated string
	 *	@param aSize sizeof buffer (has to be greate then
	 *			eReguredBufSize)
	 *	@param value Initial value of semaphore (
	 *			if semaphore is exist it's ignored)
	 *	@param aType \see #eOpenType
	 *
	 */
	CIPCSem(uint8_t* aBuf, size_t aSize,unsigned int value,
			eOpenType const aType=E_UNDEF);

	/** If open then close semaphore but not removed
	 *
	 */
	~CIPCSem();

	/**	@brief Create or Open interprocessor semaphore
	 *
	 *	@param aBuf pointer to buffer of shared memory,
	 *			where it will be saved or null terminated string
	 *	@param aSize sizeof buffer (has to be greate then
	 *			eReguredBufSize)
	 *	@param value Initial value of semaphore (
	 *			if semaphore is exist it's ignored)
	 *	@param aHasToBeNew \see #eOpenType
	 *
	 *	@return true if no error
	 */
	bool MInit(uint8_t* aBuf, size_t aSize,unsigned int value,eOpenType aHasToBeNew=E_UNDEF);

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

	/** Wait on a semaphore
	 *
	 * @return true if no error
	 */
	bool MWait(void);

	/** Wait on a semaphore, with a timeout
	 *
	 * @return true if no error
	 */
	bool MWait(double const);

	/** Wait on a semaphore, but don't block
	 *
	 * @return true if no error
	 */
	bool MTryWait(void);

	/** Increment a semaphore
	 *
	 * @return true if no error
	 */
	bool MPost(void);

	/**\brief Get the value of a semaphore
	 *
	 *\return -1 - if error is occured
	 * 			>=0 - the semaphore's value
	 */
	int MValue() const;

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

	struct CImpl;
private:

	CImpl *FImpl;
	eOpenType FType;
};
inline CIPCSem::eOpenType CIPCSem::MGetType() const
{
	return FType;
}
template<> class SHARE_EXPORT CRAII<CIPCSem> : public CDenyCopying
{
public:
	explicit CRAII(CIPCSem & aSem) :
	FSem(aSem)
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
		FSem.MPost();
		FIsLock = false;
	}
private:
	inline void MLock()
	{
		FSem.MWait();
		FIsLock =true;
	}
	CIPCSem &FSem;
	volatile bool FIsLock;
};
}
/* namespace NSHARE */
#endif /* CIPCSEM_H_ */
