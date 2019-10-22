/*
 * CIPCSemPosixImpl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CIPCSEMPOSIXIMPL_H_
#define CIPCSEMPOSIXIMPL_H_
#include <SHARE/UType/ISemaphore.h>
namespace NSHARE
{
struct CIPCSem::CImpl:ISemaphore
{
	virtual ~CImpl()
	{

	}
	virtual bool MInit(uint8_t* aBuf, size_t aSize, unsigned int value, CIPCSem::eOpenType aHasToBeNew=
			E_UNDEF)=0;

	/** @brief "Deinitialize" the semaphore
	 *
	 * @warning The semaphore is not removed, but becomes not initialized
	 */
	virtual void MFree()=0;

	/** Checks for initialization semaphore
	 *
	 *	@return true if initialized
	 */
	virtual bool MIsInited() const=0;

	/** Wait on a semaphore
	 *
	 * @return true if no error
	 */
	virtual bool MWait(void)=0;

	/** Wait on a semaphore, with a timeout
	 *
	 * @return true if no error
	 */
	virtual bool MWait(double const)=0;

	/** Wait on a semaphore, but don't block
	 *
	 * @return true if no error
	 */
	virtual bool MTryWait(void)=0;

	/** Increment a semaphore
	 *
	 * @return true if no error
	 */
	virtual bool MPost(void)=0;

	/**\brief Get the value of a semaphore
	 *
	 *\return -1 - if error is occured
	 * 			>=0 - the semaphore's value
	 */
	virtual int MValue() const=0;

	/**\brief возвращет уникальный ID семафора, если он существует
	 *
	 *\return - пустую строку в случае ошибки
	 * 			- ID семафора
	 */
	virtual NSHARE::CText const& MName() const=0;

	/**\brief Destroy a semaphore
	 *
	 */
	virtual void MUnlink()=0;

	/**\brief Gets in which mode the semaphore was opened
	 *
	 *\return see #CIPCSem::eOpenType
	 *
	 */
	virtual CIPCSem::eOpenType MGetType() const=0;
};
}



#endif /* CIPCSEMPOSIXIMPL_H_ */
