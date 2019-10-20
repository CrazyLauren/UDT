/*
 * CSemaphore.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSEMAPHORE_H_
#define CSEMAPHORE_H_

#include <deftype>
#include <share/UType/ISemaphore.h>

namespace NSHARE
{
/**\brief Crossplatform realization of semaphore
 *
 *
 * There are 3 realization:
 * 1) using CreateSemaphore (in Windows)
 * 2) using sem_init (any posix OS), default for posix OS
 * 3) using mutex and condition variable
 */
class SHARE_EXPORT CSemaphore: public ISemaphore, NSHARE::CDenyCopying
{
public:

	/** Create and initialize semaphore
	 *
	 * @param value initial value of semaphore
	 */
	CSemaphore(unsigned int value);

	/** default constructor
	 *
	 */
	CSemaphore();

	virtual ~CSemaphore();

	/**	@brief Create semaphore
	 *
	 *	@param value Initial value of semaphore (
	 *			if semaphore is exist it's ignored)
	 *
	 *	@return true if no error
	 */
	bool MInit(unsigned int value);

	/** @copydoc ISemaphore::MWait
	 *
	 */
	bool MWait();

	/** @copydoc ISemaphore::MWait(double)
	 *
	 */
	bool MWait(double const aTime);

	/** @copydoc ISemaphore::MTryWait
	 *
	 */
	bool MTryWait();

	/** @copydoc ISemaphore::MPost
	 *
	 */
	bool MPost();

	/** @copydoc ISemaphore::MValue
	 *
	 */
	int MValue() const;

	/** @copydoc ISemaphore::MIsInited
	 *
	 */
	bool MIsInited() const;

	/** @copydoc ISemaphore::MFree
	 *
	 */
	void MFree();

	/** Testing semaphore
	 *
	 * @return true if no error
	 */
	static bool sMUnitTest();
private:
	struct CImpl;
	CImpl* FImpl;

};


template<> class SHARE_EXPORT CRAII<CSemaphore> : public CDenyCopying
{
public:
	explicit CRAII(CSemaphore & aSem) :
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
	CSemaphore &FSem;
	volatile bool FIsLock;
};
}

#endif /* CSEMAPHORE_H_ */
