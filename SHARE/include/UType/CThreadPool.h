/*
 * CThreadPool.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 08.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTHREADPOOL_H_
#define CTHREADPOOL_H_
namespace NSHARE
{
class CThreadPool;
struct operation_t;
typedef int (*op_signal_t)(CThread const* WHO, operation_t * WHAT,
		void* YOU_DATA);
struct SHARE_EXPORT operation_t: NSHARE::Callback_t<op_signal_t>
{
	typedef NSHARE::Callback_t<op_signal_t> cb_t;
	enum eType
	{
		IMMEDIATE, // Should run immediately
		AS_LOWER, //when IMMEDIATE tasks are not available
		//it runs  as lower priority
		IO //these tasks are blocking thread but aren't cpu intensive.
	};

	operation_t(eType const& aType = AS_LOWER);
	operation_t(pM const& aSignal, void * const aData, eType const& aType =
			AS_LOWER);
	operation_t(operation_t const& aCB);
	operation_t& operator=(operation_t const& aCB);
	/**
	 * should be kept once its been applied
	 */
	void MKeep(bool aKeep);

	bool MIsKeep() const;
	eType MType() const;
	bool operator ==(operation_t const& rihgt) const;
private:
	eType FType;
	SHARED_PTR<bool> FKeep;
};

class SHARE_EXPORT CThreadPool: NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;
	/**
	 * param see MCreate
	 */
	CThreadPool(int);//todo from config
	CThreadPool();
	~CThreadPool();

	bool MIsRunning() const;

	/**
	 * Initializing the thread pool and starts it.
	 * @param numThreads the number of threads to create,
	 * if value <=0 it will create a thread per core.
	 */
	bool MCreate(int aNum = -1, CThread::param_t* aParam = NULL);
	bool MCancel();

	bool MAdd(operation_t const& task);
	bool operator+=(operation_t const& task)
	{
		return MAdd(task);
	}
	//void MRemove(operation_t const& task);
	bool MAddThread(CThread::param_t* aParam);
	//0 - current
	bool MRemoveThread(unsigned aId = 0);

	/**
	 * if current operation  is IMMEDIATE, you can call this method, it will run the set of tasks
	 * on both this thread and the additional  threads until they all complete.
	 */
	void MExecuteOtherTasks(); //TODO

	/**
	 * the number of worker thread
	 */
	unsigned MThreadNum() const;

	void MEraseAll();
	std::ostream& MPrint(std::ostream& aVal) const;
	NSHARE::CConfig MSerialize() const;
private:

	struct CImpl;
	CImpl* FImpl;

	bool FIs;
};
}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::operation_t const& aCb)
{
	return aStream << static_cast<NSHARE::operation_t::cb_t const&>(aCb)
			<< " Type=" << (int) aCb.MType() << " Keep=" << aCb.MIsKeep();
}
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::CThreadPool const& aCb)
{
	aCb.MPrint(aStream);
	return aStream;
}
}

#endif /* CTHREADPOOL_H_ */
