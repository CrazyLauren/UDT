/*
 * CThreadPool.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 08.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <UType/CThreadPool.h>
#include <UType/CThreadPoolImpl.h>

namespace NSHARE
{
const CText CThreadPool::NAME="tpool";
const CText CThreadPool::NUMBER_OF_THREAD="thnum";
const CText CThreadPool::MAX_NUMBER_OF_IO_THREAD="th_io_max";

COperationQueue::COperationQueue() :
		FMutex(CMutex::MUTEX_NORMAL),FNumberOfInvoked(0)
{
	FCurrentOperationIterator = FOperations.begin();
}
COperationQueue::~COperationQueue()
{
	MEraseAll();
	MForceUnlock();
}

bool COperationQueue::MIsEmpty()
{

	CRAII<CMutex> lock(FMutex);
	return FOperations.empty();
}

unsigned int COperationQueue::MSize()
{
	CRAII<CMutex> lock(FMutex);
	return static_cast<unsigned int>(FOperations.size());
}

bool COperationQueue::MWaitFor(double aTime) const
{
	CRAII<CMutex> lock(FMutex);
	if(FNumberOfInvoked == 0 && FOperations.empty())
		return true;

	if(aTime<=0.0)
		return FWaitForCond.MTimedwait(&FMutex);
	else
		return FWaitForCond.MTimedwait(&FMutex,aTime);
}
void COperationQueue::MFinishOperation()
{
	CRAII<CMutex> lock(FMutex);
	--FNumberOfInvoked;

	if (FNumberOfInvoked == 0 && FOperations.empty())
		FWaitForCond.MBroadcast();
}
operation_t COperationQueue::MNextOperation(bool aIfBlocking)
{
	CRAII<CMutex> lock(FMutex);
	if (aIfBlocking && FOperations.empty())
	{
		VLOG(2) << "Wait for";
		FCond.MTimedwait(&FMutex);
	}
	VLOG(2) << "Our turn " << !FOperations.empty();

	if (FOperations.empty())
		return operation_t(); //if broadcast signal for unlock thread

	VLOG(2) << "There is data ";
	if (FCurrentOperationIterator == FOperations.end())
		FCurrentOperationIterator = FOperations.begin();

	operation_t currentOperation(*FCurrentOperationIterator);
	++FNumberOfInvoked;
//	if (!currentOperation.MIsKeep())
//	{
//		VLOG(2) << "Removing  operation " << currentOperation;

		FCurrentOperationIterator = FOperations.erase(
				FCurrentOperationIterator);

//		VLOG(2) << "Now " << FOperations.size();
//		VLOG_IF(2,FOperations.empty()) << "Data empty now";
//	}
//	else
//	{
//		VLOG(2) << "Move on to the next operation...";
//		++FCurrentOperationIterator;
//	}

	return currentOperation;
}

void COperationQueue::MAdd(operation_t const& operation)
{
	VLOG(2) << "Doing add " << operation;

	CRAII<CMutex> lock(FMutex);

	VLOG(2) << "add the operation to the end of the list";

	if (operation.MType() == operation_t::IMMEDIATE)
	{
		Operations::iterator _it = FOperations.begin(),_it_end = FOperations.end();
		for (; _it != _it_end && _it->MType() == operation_t::IMMEDIATE; ++_it)
		{
			VLOG(2)<<"O_O. Already There is some immediately operation.";
		}
		FOperations.insert(_it,operation);
		FCurrentOperationIterator = FOperations.begin();
	}
	else
		FOperations.push_back(operation);
	FCond.MSignal();
}

void COperationQueue::MErase(operation_t const& operation)
{
	VLOG(2) << "Doing remove operation";

	CRAII<CMutex> lock(FMutex);

	for (Operations::iterator itr = FOperations.begin();
			itr != FOperations.end();)
	{
		if ((*itr) == operation)
		{
			bool needToResetCurrentIterator = (FCurrentOperationIterator == itr);

			itr = FOperations.erase(itr);

			if (needToResetCurrentIterator)
				FCurrentOperationIterator = itr;

		}
		else
			++itr;
	}
}

void COperationQueue::MEraseAll()
{
	VLOG(2) << "Doing remove all operations";

	CRAII<CMutex> lock(FMutex);

	FOperations.clear();

	// reset current operator.
	FCurrentOperationIterator = FOperations.begin();
}

void COperationQueue::MForceUnlock()
{
	FCond.MBroadcast();
	FWaitForCond.MBroadcast();
}

void COperationQueue::MAddThread(CPoolThread* thread)
{
	FThreads.insert(thread);
}

void COperationQueue::MRemoveThread(CPoolThread* thread)
{
	FThreads.erase(thread);
}

CPoolThread::CPoolThread() :
		FDone(0), FInOperation(0)
{
	MSetQueue(SHARED_PTR<COperationQueue>(new COperationQueue));
}

CPoolThread::~CPoolThread()
{
	if (MIsRunning())
		MCancel();
	if (FQueue.get())
		FQueue->MRemoveThread(this);
}

void CPoolThread::MSetQueue(SHARED_PTR<COperationQueue> aOp)
{
	CRAII < CMutex > lock(FMutex);
	VLOG(2)<<"Add operation queue.";

	if (FQueue.get() == aOp.get())
	return;

	if (FQueue.get())
	FQueue->MRemoveThread(this);

	FQueue = aOp;

	if (FQueue.get())
	FQueue->MAddThread(this);
}

void CPoolThread::MDone(bool done)
{
	if (FDone == done)
		return;

	FDone = done;

	if (done)
	{
		if (FQueue.get()) //FIXME unblocking thread
			FQueue->MForceUnlock();
	}
}

bool CPoolThread::MCancel()
{
	VLOG(2) << "Cancelling OperationThread" << MIsRunning();

	FInOperation = false;
	int result = 0;
	if (MIsRunning())
	{

		FDone = true;

		VLOG(2) << "   Doing cancel " << this;

		{
			CRAII<CMutex> lock(FMutex);

			if (operationQueue.get())
				operationQueue->MForceUnlock();
		}
		
		NSHARE::CThread::sMYield();
		VLOG(2) << " wait for the thread to stop running.";
		while (MIsRunning() && !CThread::MCancel())
		{

			{
				CRAII<CMutex> lock(FMutex);

				if (operationQueue.get())
				{
					operationQueue->MForceUnlock();
					// _operationQueue->releaseAllOperations();
				}
			}
			VLOG(2) << "   Waiting for OperationThread to cancel " << this;
			NSHARE::CThread::sMYield();
		}

		VLOG(2) << "Join ";
		MJoin();
	}

	VLOG(2) << " thread cancelled ";

	return result!=0;
}
bool CPoolThread::MIsFree() const
{
	CRAII<CMutex> lock(FMutex);
	return !FInOperation;
}
void CPoolThread::MRun()
{
	VLOG(2) << "Doing run  " << MIsRunning();

	bool firstTime = true;

	do
	{
		VLOG(2) << "In thread loop ";
		{
			CRAII<CMutex> lock(FMutex);
			operationQueue = FQueue;
		}
		operation_t operation(operationQueue->MNextOperation(true));

		{
			CRAII<CMutex> lock(FMutex);
			FInOperation = true;
		}
		if (FDone)
		{
			if(operation.MIs())
				operationQueue->MFinishOperation();
			break;
		}
		LOG_IF(WARNING,!operation.MIs()) << "Empty operation.";

		if (operation.MIs())
		{
			VLOG(4)<<"operation";
			eCBRval const _rval=(operation)(this, &operation);

			switch (_rval)
			{
			case E_CB_REMOVE:
				//
				break;
			case E_CB_SAFE_IT:
			case E_CB_BLOCING_OTHER:
				VLOG(2) << " Keeping operartion";
				operationQueue->MAdd(operation);
				break;
			};
			operationQueue->MFinishOperation();
		}

		{
			CRAII<CMutex> lock(FMutex);
			FInOperation = false;
		}
		if (firstTime)
		{
			NSHARE::CThread::sMYield();
			firstTime = false;
		}

	} while (!FDone);
	FInOperation = false;
	VLOG(2) << "exit loop ";
}
//
//--------------------
//
CThreadPool::CThreadPool(int aVal,unsigned aMaxNumberOfIOThread) :
		FImpl(new CImpl),//
		FMaxNumberOfIOThread(aMaxNumberOfIOThread)
{
	FIs = MCreate(aVal);
}
CThreadPool::CThreadPool(NSHARE::CConfig const& aConfig) :
		FImpl(new CImpl),//
		FIs(false),//
		FMaxNumberOfIOThread(
				aConfig.MValue(MAX_NUMBER_OF_IO_THREAD,
						std::numeric_limits<unsigned>::max()))
{
	int _number = -1;
	if (aConfig.MGetIfSet(NUMBER_OF_THREAD, _number))
	{
		if (aConfig.MIsChild(CThread::param_t::NAME))
		{
			CThread::param_t const _param(
					aConfig.MChild(CThread::param_t::NAME));
			FIs = _param.MIsValid() && MCreate(_number, &_param);
		}
		else
			FIs = MCreate(_number);
	}
}
CThreadPool::CThreadPool(int aNum, CThread::param_t const& aParam,unsigned aMaxNumberOfIOThread):
				FImpl(new CImpl),//
				FMaxNumberOfIOThread(aMaxNumberOfIOThread)
{
	FIs = MCreate(aNum,&aParam);
}
CThreadPool::CThreadPool() :
		FImpl(new CImpl),//
		FMaxNumberOfIOThread(std::numeric_limits<unsigned>::max())
{
	FIs = false;
}
CThreadPool::~CThreadPool()
{
	if (MIsRunning())
		MCancel();
	delete FImpl;
}
bool CThreadPool::MIsRunning() const
{
	return FIs;
}

bool CThreadPool::MCreate(int aNum, CThread::param_t const* aParam)
{
	VLOG(2) << "Create thread pool Number=" << aNum;
	CRAII<CMutex> _blocked(FImpl->FMutex);
	LOG_IF(WARNING,MIsRunning()) << "The pool has been created already.";
	if (MIsRunning())
		return false;

	if (aNum <= 0)
	{
		aNum = CThread::sMNumberOfProcessor();
		VLOG(2) << "Number of threads is " << aNum;
	}
	if (!FImpl->FTask.get())
		FImpl->FTask = SHARED_PTR<COperationQueue>(new COperationQueue);
	VLOG(2) << "Initialize task pool";
	for (int i = 0; i < aNum; ++i)
	{
		MAddThread(aParam);
	}
	if (!FImpl->FIOTask.get())
		FImpl->FIOTask = SHARED_PTR<COperationQueue>(new COperationQueue);
	VLOG(2) << "Initialize io pool";
	unsigned i = 0;
	do
	{
		SHARED_PTR<CPoolThread> newThread(new CPoolThread);
		FImpl->FIOThreads.push_back(newThread);
		newThread->MSetQueue(FImpl->FIOTask);
		newThread->MCreate(aParam);

	}while(++i<FImpl->FIOTask->MSize());
	FIs = true;
	return FIs;
}
/////////////////////////////////////////////////
bool CThreadPool::MAddThread(CThread::param_t const* aParam)
{
	CRAII<CMutex> _blocked(FImpl->FMutex);
	if (!FImpl->FTask.get())
		return false;
	SHARED_PTR<CPoolThread> newThread(new CPoolThread);
	FImpl->FThreads.push_back(newThread);
	CHECK_NOTNULL(FImpl->FTask.get());
	newThread->MSetQueue(FImpl->FTask);
	newThread->MCreate(aParam);
	VLOG(2) << "Add new thread :" << newThread->MThreadId();
	return true;

}
bool CThreadPool::MRemoveThread(unsigned aId)
{
	CRAII<CMutex> _blocked(FImpl->FMutex);
	VLOG(2) << "Removing thread " << aId;
	VLOG(2) << "Current ID " << CThread::sMThreadId();
	if (!aId)
		aId = CThread::sMThreadId();

	for (CImpl::threads_t::iterator _it = FImpl->FThreads.begin();
			_it != FImpl->FThreads.end(); ++_it)
	{
		if ((*_it)->MThreadId() == aId)
		{
			VLOG(1) << "The thread " << aId << " removed from pool.";
			FImpl->FThreads.erase(_it);
			return true;
		}
	}
	LOG(ERROR)<<"Cannot remove "<<aId<<" thread from pool. Current ID "<<CThread::sMThreadId();
	return false;
}
bool CThreadPool::CImpl::MAddIOThreadIfNeed(unsigned aMaxNumber)
{
	CRAII<CMutex> _blocked(FMutex);
	unsigned _size = FIOTask->MSize();
	for (CImpl::threads_t::iterator _it = FIOThreads.begin();
			_it != FIOThreads.end(); ++_it)
	{
		if ((*_it)->MIsFree())
		{
			if (!_size)
			{
				VLOG(2) << "New thread is not necessary.";
				return true;
			}
			else
			{
				--_size;
				VLOG(2) << "The thread may be locked later.";
			}
		}
	}
	LOG_IF(DFATAL,FIOThreads.size() >= aMaxNumber)<< "Maximum amount of IO threads.";
	if (FIOThreads.size() <  aMaxNumber)
	{
		MAddIOThread();
		return true;
	}
	else
		return false;
}
bool CThreadPool::CImpl::MAddIOThread()
{
	CRAII<CMutex> _blocked(FMutex);
	if (!FIOTask.get())
		return false;
	SHARED_PTR<CPoolThread> newThread(new CPoolThread);
	FIOThreads.push_back(newThread);
	CHECK_NOTNULL(FIOTask.get());
	newThread->MSetQueue(FIOTask);
	newThread->MCreate();
	VLOG(2) << "Add new thread :" << newThread->MThreadId();
	return true;

}
bool CThreadPool::CImpl::MRemoveIOThread(unsigned aId)
{
	CRAII<CMutex> _blocked(FMutex);
	VLOG(2) << "Removing thread " << aId;
	VLOG(2) << "Current ID " << CThread::sMThreadId();
	if (!aId)
		aId = CThread::sMThreadId();

	for (CImpl::threads_t::iterator _it = FIOThreads.begin();
			_it != FIOThreads.end(); ++_it)
	{
		if ((*_it)->MThreadId() == aId)
		{
			VLOG(1) << "The thread " << aId << " removed from pool.";
			FIOThreads.erase(_it);
			return true;
		}
	}
	LOG(ERROR)<<"Cannot remove "<<aId<<" thread from pool. Current ID "<<CThread::sMThreadId();
	return false;
}

//////////////////////////////////////////////////
bool CThreadPool::MCancel()
{
	CRAII<CMutex> _blocked(FImpl->FMutex);

	FImpl->FTask->MEraseAll();
	FImpl->FTask.reset();

	FImpl->FIOTask->MEraseAll();
	FImpl->FIOTask.reset();

	FImpl->FThreads.clear();
	FImpl->FIOThreads.clear();

	FIs = false;
	return true;
}
bool  CThreadPool::MWaitForTask(double aTime)const
{
	return FImpl->FTask->MWaitFor(aTime);
}
bool  CThreadPool::MWaitForIO(double aTime) const
{
	return FImpl->FIOTask->MWaitFor(aTime);
}
void CThreadPool::MEraseAll()
{
	CRAII<CMutex> _blocked(FImpl->FMutex);
	FImpl->FTask->MEraseAll();
	FImpl->FIOTask->MEraseAll();
}
NSHARE::CConfig CThreadPool::MSerialize() const
{
	NSHARE::CConfig aTo(NAME);
	CRAII<CMutex> _blocked(FImpl->FMutex);

	{
		NSHARE::CConfig _tasks("tasks", FImpl->FTask->MSize());
		aTo.MAdd(_tasks);
		_tasks.MAdd(NUMBER_OF_THREAD, FImpl->FThreads.size());

		for (CImpl::threads_t::const_iterator _it = FImpl->FThreads.begin();
				_it != FImpl->FThreads.end(); ++_it)
		{
			CHECK_NOTNULL(_it->get());
			_tasks.MAdd(NSHARE::CThread::NAME, (*_it)->MSerialize());
		}
	}
	{
		NSHARE::CConfig _tasks("io", FImpl->FIOTask->MSize());
		aTo.MAdd(_tasks);
		_tasks.MAdd(NUMBER_OF_THREAD, FImpl->FIOThreads.size());
		_tasks.MAdd(MAX_NUMBER_OF_IO_THREAD, MGetMaxNumberOfIOThread() );


		for (CImpl::threads_t::const_iterator _it = FImpl->FIOThreads.begin();
				_it != FImpl->FIOThreads.end(); ++_it)
		{
			CHECK_NOTNULL(_it->get());
			_tasks.MAdd(NSHARE::CThread::NAME, (*_it)->MSerialize());
		}
	}
	return aTo;
}
std::ostream& CThreadPool::MPrint(std::ostream& aVal) const
{
	CRAII<CMutex> _blocked(FImpl->FMutex);
	aVal << "Tasks " << FImpl->FTask->MSize() << "; ";
	aVal << "Threads for Tasks " << FImpl->FThreads.size();
	for (CImpl::threads_t::const_iterator _it = FImpl->FThreads.begin();
			_it != FImpl->FThreads.end(); ++_it)
	{
		CHECK_NOTNULL(_it->get());
		aVal << std::endl << *_it->get();
	}
	aVal << std::endl << "IO Tasks " << FImpl->FIOTask->MSize() << "; ";
	aVal << "Threads for IO Tasks " << FImpl->FIOThreads.size();

	for (CImpl::threads_t::const_iterator _it = FImpl->FIOThreads.begin();
			_it != FImpl->FIOThreads.end(); ++_it)
	{
		CHECK_NOTNULL(_it->get());
		aVal << std::endl << *_it->get();
	}
	return aVal;
}
//////////////////////////////////////////////////
bool CThreadPool::MAdd(operation_t const& task)
{
	CRAII<CMutex> _blocked(FImpl->FMutex);
	VLOG(1) << "Doing add of " << task;
	switch (task.MType())
	{
	case operation_t::IMMEDIATE:
	case operation_t::AS_LOWER:
		CHECK_NOTNULL(FImpl->FTask.get());
		FImpl->FTask->MAdd(task);
		break;

	case operation_t::IO:
	{
		VLOG(2) << "Adding  IO operation!!!";
		CHECK_NOTNULL(FImpl->FIOTask.get());
		bool _val=FImpl->MAddIOThreadIfNeed(MGetMaxNumberOfIOThread());
		if(_val)
			FImpl->FIOTask->MAdd(task);
		else
			return false;
	}
		break;
	}
	return true;
}

//////////////////////////////////////////////////
void CThreadPool::MExecuteOtherTasks()
{
	LOG(FATAL)<<"It's not implemented.";
}

//////////////////////////////////////////////////
unsigned CThreadPool::MThreadNum() const
{
	return (unsigned)FImpl->FThreads.size();
}
operation_t::operation_t(eType const& aType) :
		cb_t(), FType(aType)//, FKeep(new bool(false))
{
}
operation_t::operation_t(pM const& aSignal, void * const aData,
		eType const& aType) :
		cb_t(aSignal, aData), FType(aType)//, FKeep(new bool(false))
{
}
operation_t::operation_t(operation_t const& aCB) :
		cb_t(aCB), FType(aCB.FType)//, FKeep(aCB.FKeep)
{
	;
}
operation_t& operation_t::operator=(operation_t const& aCB)
{
	FType=aCB.FType;
	//FKeep=aCB.FKeep;
	return *this;
}
//void operation_t::MKeep(bool aKeep)
//{
//	CHECK_NOTNULL(FKeep.get());
//	*FKeep.get() = aKeep;
//}
//
//bool operation_t::MIsKeep() const
//{
//	CHECK_NOTNULL(FKeep.get());
//	return *FKeep.get();
//}
operation_t::eType operation_t::MType() const
{
	return FType;
}
bool operation_t::operator ==(operation_t const& rihgt) const
{
	return FSignal == rihgt.FSignal && FYouData == rihgt.FYouData;
}

}
