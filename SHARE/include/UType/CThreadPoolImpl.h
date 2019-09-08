/*
 * CThreadPoolImpl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 09.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTHREADPOOLIMPL_H_
#define CTHREADPOOLIMPL_H_

namespace NSHARE
{
class CPoolThread;

class SHARE_EXPORT COperationQueue
{
public:

	COperationQueue();
	~COperationQueue();

	bool MWaitFor(double aTime) const;
	void MFinishOperation();
	operation_t MNextOperation(bool blockIfEmpty = false);

	bool MIsEmpty();

	unsigned int MSize();

	void MAdd(operation_t const& operation);

	bool MEraseOp(operation_t const& operation);

	void MEraseAll();

	void MForceUnlock();

	typedef std::set<CPoolThread*> threads_t;
	const threads_t& MGetThreads() const
	{
		return FThreads;
	}

protected:

	friend class CPoolThread;

	void MAddThread(CPoolThread* thread);
	void MRemoveThread(CPoolThread* thread);

	typedef std::list<operation_t> Operations;
	mutable NSHARE::CMutex FMutex;
	mutable NSHARE::CCondvar FCond;
	unsigned FNumberOfInvoked;///< The number of working operation
	mutable NSHARE::CCondvar FWaitForCond;///<Wait for all operation are finished

	Operations FOperations;
	Operations::iterator FCurrentOperationIterator;
	threads_t FThreads;
};

class SHARE_EXPORT CPoolThread: public NSHARE::CThread
{
public:
	CPoolThread();
	virtual ~CPoolThread();

	void MSetQueue(SHARED_PTR<COperationQueue> opq);

	COperationQueue* MGetQueue()
	{
		return FQueue.get();
	}

	const COperationQueue* MGetQueue() const
	{
		return FQueue.get();
	}

	void MDone(bool done);

	bool MIsDone() const
	{
		return FDone != 0;
	}
	bool MCancel();
	bool MIsFree() const;
protected:

	/** Run does the opertion thread run loop.*/
	virtual void MRun();

	atomic_t FDone;
	volatile bool FInOperation;
	atomic_t FIsWaitNextOperation;///< 1 if thread blocked as no operation

	mutable NSHARE::CMutex FMutex;
	SHARED_PTR<COperationQueue> FQueue;
	SHARED_PTR<COperationQueue> operationQueue;
};
struct CThreadPool::CImpl
{
	typedef std::vector< SHARED_PTR<CPoolThread> > threads_t;
	CImpl() :
			FTask(new COperationQueue), FIOTask(new COperationQueue)
	{

	}
	bool MAddIOThreadIfNeed(unsigned aMaxNumber);
	bool MAddIOThread();
	bool MRemoveIOThread(unsigned aId);
	SHARED_PTR<COperationQueue> FTask;
	threads_t FThreads;

	SHARED_PTR<COperationQueue> FIOTask;
	threads_t  FIOThreads;

	mutable CMutex FMutex;
};
}

#endif /* CTHREADPOOLIMPL_H_ */
