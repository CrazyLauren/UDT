/*
 * ISemaphore.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ISEMAPHORE_H_
#define ISEMAPHORE_H_


namespace NSHARE
{
/** Interface of semaphore
 *
 */
class SHARE_EXPORT ISemaphore
{
public:

	/** @brief The maximal value of semaphore
	 *
	 */
	static unsigned const MAX_VALUE;

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
	 * @param aTime how long to wait
	 * @return true if no error
	 */
	virtual bool MWait(double const aTime)=0;


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

	virtual ~ISemaphore()
	{

	}
};

template<> class SHARE_EXPORT CRAII<ISemaphore> : public CDenyCopying
{
public:
	explicit CRAII(ISemaphore & aSem) :
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
	ISemaphore &FSem;
	volatile bool FIsLock;
};
}


#endif /* ISEMAPHORE_H_ */
