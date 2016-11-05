/*
 * CThreadPoolImpl.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 09.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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
	virtual ~COperationQueue();

	operation_t MNextOperation(bool blockIfEmpty = false);

	bool MIsEmpty();

	unsigned int MSize();

	void MAdd(operation_t const& operation);

	void MErase(operation_t const& operation);

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
	NSHARE::CMutex FMutex;
	NSHARE::CCondvar FCond;
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

	bool FDone;
	bool FInOperation;

	mutable NSHARE::CMutex FMutex;//fixme
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
	bool MAddIOThreadIfNeed();
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
