/*
 * CThreadPool.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 08.02.2016
 *      Author:  https://github.com/CrazyLauren
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
/** \brief сигнатура функции используемая в CThreadPool
 *
 *
 *	\note если ф-ция возвращает E_CB_REMOVE то функция будет удалена их пула
 *		если ф-я возвращает иное значение, то останется
 */
typedef eCBRval (*op_signal_t)(CThread const* WHO, operation_t * WHAT,
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
//	/**
//	 * should be kept once its been applied
//	 */
//	void MKeep(bool aKeep);
//
//	bool MIsKeep() const;
	eType MType() const;
	bool operator ==(operation_t const& rihgt) const;
private:
	eType FType;
//	SHARED_PTR<bool> FKeep;
};

/** \brief Пул потоков
 */
class SHARE_EXPORT CThreadPool: NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText NUMBER_OF_THREAD;
	static const NSHARE::CText MAX_NUMBER_OF_IO_THREAD;

	/** \brief construct and start pool
	 * \param aNum - the number of threads to create, if value <=0 it will create a thread per core.
	 * \param aMaxNumberOfIOThread - the maximal number of threads using for IO operation.
	 */
	CThreadPool(int aNum,unsigned aMaxNumberOfIOThread = std::numeric_limits<unsigned>::max());

	/** \brief construct and start pool
	 *
	 */
	CThreadPool(NSHARE::CConfig const& aConfig);
	CThreadPool(int aNum, CThread::param_t const& aParam,unsigned aMaxNumberOfIOThread = std::numeric_limits<unsigned>::max());

	/** \brief constructor
	 *
	 */
	CThreadPool();
	~CThreadPool();

	bool MIsRunning() const;

	/**\brief Initializing the thread pool and starts it.
	 * \param aNum - the number of threads to create, if value <=0 it will create a thread per core.
	 */
	bool MCreate(int aNum = -1, CThread::param_t const* aParam = NULL);
	bool MCancel();

	bool MAdd(operation_t const& task);
	bool operator+=(operation_t const& task)
	{
		return MAdd(task);
	}

	/** \brief добавляет новый поток в пул потоков
	 *
	 * \param aParam - параметры нового потока
	 * \return true - в случае успеха
	 */
	bool MAddThread(CThread::param_t const* aParam);

	/** \brief удалить поток из пула потоков
	 *
	 * \param aId - id потока, если id=0, то удаляется текущий поток
	 * \return true - в случае успеха
	 */
	bool MRemoveThread(unsigned aId = 0);

	/**
	 * if current operation  is IMMEDIATE, you can call this method, it will run the set of tasks
	 * on both this thread and the additional  threads until they all complete.
	 */
	void MExecuteOtherTasks(); //TODO

	/** \brief the number of worker thread
	 */
	unsigned MThreadNum() const;
	unsigned MGetMaxNumberOfIOThread() const;

	void MEraseAll();
	std::ostream& MPrint(std::ostream& aVal) const;
	NSHARE::CConfig MSerialize() const;
private:

	struct CImpl;
	CImpl* FImpl;

	bool FIs;
	const unsigned FMaxNumberOfIOThread;
};
inline unsigned CThreadPool::MGetMaxNumberOfIOThread() const
{
	return FMaxNumberOfIOThread;
}
}
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::operation_t const& aCb)
{
	return aStream << static_cast<NSHARE::operation_t::cb_t const&>(aCb)
			<< " Type=" << (int) aCb.MType()/* << " Keep=" << aCb.MIsKeep()*/;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::CThreadPool const& aCb)
{
	aCb.MPrint(aStream);
	return aStream;
}
}

#endif /* CTHREADPOOL_H_ */
