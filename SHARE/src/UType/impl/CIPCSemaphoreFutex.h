/*
 * CIPCSemaphoreFutex.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CIPCSEMAPHOREFUTEX_H_
#define CIPCSEMAPHOREFUTEX_H_

#include "CIPCSemPosixImpl.h"
namespace NSHARE
{
namespace impl
{
/** Realization semaphore based Futex
 *
 */
class CIPCSemaphoreFutex:public CIPCSem::CImpl
{
public:
	/** it's stored in shared memory
	 *
	 */
	/*SHARED_PACKED(*/struct _sem_t
	{
		inline _sem_t();
		volatile int32_t FFutex;
		uint16_t FCount;
		uint16_t FIsUnlinked:1;
		uint16_t :15;
		uint32_t FId;

		inline bool MIsValid() const;
		inline void MInc();
		inline bool MDec();
		inline void MCreatedNew(int aVal);
	}/*)*/;
	/** Default constructor
	 *
	 */
	CIPCSemaphoreFutex();

	/** If open then close semaphore but not removed
	 *
	 */
	virtual ~CIPCSemaphoreFutex();

	/**	@brief Create or Open interprocessor semaphore
	 *
	 *	@param aBuf pointer to buffer of shared memory,
	 *			where it will be saved or null terminated string
	 *	@param aSize sizeof buffer (has to be greate then
	 *			eReguredBufSize)
	 *	@param value Initial value of semaphore (
	 *			if semaphore is exist it's ignored)
	 *	@param aType \see #CIPCSem::eOpenType
	 *
	 *	@return true if no error
	 */
	bool MInit(uint8_t* aBuf, size_t aSize,unsigned int value,CIPCSem::eOpenType =CIPCSem::E_UNDEF);

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
	 *\return see #CIPCSem::eOpenType
	 *
	 */
	CIPCSem::eOpenType MGetType() const;
private:
	NSHARE::CText FName;
	mutable _sem_t* FSem;//!< Pointer to stored data
	CIPCSem::eOpenType FType;
};
inline CIPCSem::eOpenType CIPCSemaphoreFutex::MGetType() const
{
	return FType;
}
}
}

#endif /* CIPCSEMAPHOREFUTEX_H_ */
