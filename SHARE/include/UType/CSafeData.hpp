/*
 * CSafeData.hpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.09.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSAFEDATA_HPP_
#define CSAFEDATA_HPP_

namespace NSHARE
{
template<class _T>
template<class _Y>
class CSafeData<_T>::WAccess
{
public:
	typedef _Y value_type;
	typedef _Y* pvalue_type;
	WAccess(WAccess<_Y> const& aRhs);
	~WAccess();

	value_type& operator*()
	{
		return FSafe.FData;
	}

	pvalue_type operator&() // never throws
	{
		return MGetPtr();
	}

	inline pvalue_type operator->()
	{
		return MGetPtr();
	}
	WAccess<value_type> & operator=(value_type const & aData)
	{
		FSafe.FData = aData;
		return *this;
	}
	operator value_type &()
	{
		return MGet();
	}
	inline value_type& MGet()
	{
		return FSafe.FData;
	}
	inline pvalue_type MGetPtr()
	{
		return &FSafe.FData;
	}
private:
	WAccess(CSafeData<_T>& aA) :
			FSafe(aA), pFAntiRecursiveBlock(NULL)
	{
		MLock();
#ifndef NDEBUG
		FPThread = new unsigned;
		*FPThread = CThread::sMThreadId();
#endif //NDEBUG
	}
	WAccess<_Y>& operator=(WAccess<_Y> const& aRhs)
	{
		return *this;
	}
	void MLock()
	{
		CHECK_NOTNULL(FSafe.pFImplData.get());
		LOG_IF(FATAL,
				FSafe.pFImplData->FMutex.MThread()
						== NSHARE::CThread::sMThreadId())
				<< "Dead lock for rw writer " << FSafe.pFImplData->FMutex.MThread()
				<< " == " << NSHARE::CThread::sMThreadId();
		FIsLock=FSafe.pFImplData->FMutex.MLock();
		++FSafe.pFImplData->FWriters;
		for (; FSafe.pFImplData->FReaders;)
		{
			bool _rval = FSafe.pFImplData->FWriteSignal.MTimedwait(
					&FSafe.pFImplData->FMutex);
			MASSERT_1(_rval);
		}
	}

	void MUnlock()
	{
		MASSERT_1(FIsLock);
		CHECK_NOTNULL(FSafe.pFImplData.get());
		MASSERT_1(FSafe.pFImplData->FWriters > 0);
		if (--FSafe.pFImplData->FWriters)
			FSafe.pFImplData->FWriteSignal.MSignal();
		else
			FSafe.pFImplData->FReadSignal.MBroadcast();
		if (FIsLock)
		{
			FIsLock = false;
			FSafe.pFImplData->FMutex.MUnlock();
		}
	}

	CSafeData<_T>& FSafe;
	volatile bool FIsLock;
	volatile mutable int* pFAntiRecursiveBlock;
#ifndef NDEBUG
	volatile unsigned *FPThread;
#endif //NDEBUG
	friend class CSafeData<_T> ;
};
template<class _T>
template<class _Y>
CSafeData<_T>::WAccess<_Y>::WAccess(WAccess<_Y> const& aRhs) :
		FSafe(aRhs.FSafe),FIsLock(aRhs.FIsLock)
{
#ifndef NDEBUG
	MASSERT_1(CThread::sMThreadId() == *FPThread);
#endif //NDEBUG
	if (!aRhs.pFAntiRecursiveBlock)
	{
		aRhs.pFAntiRecursiveBlock = new int;
		*aRhs.pFAntiRecursiveBlock=1;
	}
	pFAntiRecursiveBlock = aRhs.pFAntiRecursiveBlock;
	if (pFAntiRecursiveBlock)
		++(*pFAntiRecursiveBlock);
}
template<class _T>
template<class _Y>
CSafeData<_T>::WAccess<_Y>::~WAccess()
{
	if (!pFAntiRecursiveBlock)
	{
		MUnlock();
#ifndef NDEBUG
		delete FPThread;
#endif //NDEBUG
	}
	else if (1 == *pFAntiRecursiveBlock)
	{

		delete pFAntiRecursiveBlock;
		MUnlock();
#ifndef NDEBUG
		delete FPThread;
#endif //NDEBUG
	}
	else
		--(*pFAntiRecursiveBlock);
}

template<class _T>
template<class _Y>
class CSafeData<_T>::RAccess
{
public:
	typedef _Y const value_type;
	typedef _Y const* pvalue_type;

	RAccess(RAccess<_Y> const& aRhs) :
			FSafe(aRhs.FSafe)
	{
		MASSERT_1(FSafe.pFImplData->FReaders);
		MLock();
	}
	~RAccess()
	{
		MUnlock();
	}
	inline value_type& operator*() const
	{
		return MGet();
	}

	inline pvalue_type operator->() const
	{
		return MGetPtr();
	}
	inline operator value_type&() const
	{
		return MGet();
	}
	inline value_type& MGet() const
	{
		return FSafe.FData;
	}
	inline pvalue_type MGetPtr() const
	{
		return &FSafe.FData;
	}
private:
	RAccess(CSafeData<_T> const& aA) :
			FSafe(aA)
	{
		MLock();
	}
	RAccess<_Y>& operator=(RAccess<_Y> const& aRhs) const
	{
		return *this;
	}
	RAccess<_Y> & operator=(value_type & aData) const
	{
		return *this;
	}
	inline void MLock()
	{
		LOG_IF(FATAL,
				FSafe.pFImplData->FMutex.MThread()
						== NSHARE::CThread::sMThreadId())
				<< "Dead lock for rw reader " << FSafe.pFImplData->FMutex.MThread()
				<< " == " << NSHARE::CThread::sMThreadId();
		NSHARE::CRAII<CMutex> _block(FSafe.pFImplData->FMutex);
		for (; FSafe.pFImplData->FWriters;)
		{
			bool _rval = FSafe.pFImplData->FReadSignal.MTimedwait(
					&FSafe.pFImplData->FMutex);
			MASSERT_1(_rval);
		}
		++FSafe.pFImplData->FReaders;
	}
	inline void MUnlock()
	{
		NSHARE::CRAII<CMutex> _block(FSafe.pFImplData->FMutex);
		MASSERT_1(FSafe.pFImplData->FReaders > 0);
		if (--FSafe.pFImplData->FReaders == 0 && FSafe.pFImplData->FWriters)
			FSafe.pFImplData->FWriteSignal.MSignal();
	}

	CSafeData<_T> const& FSafe;
	friend class CSafeData<_T> ;
};
template<class T>
inline CSafeData<T>::CSafeData() :
		FData(T()), pFImplData(new data_t) //fixme ����� �����������
{
	pFImplData->FReaders = 0;
	pFImplData->FWriters = 0;
}
template<class T>
template<class Y>
CSafeData<T>::CSafeData(Y const& aP) :
		FData(aP), pFImplData(new data_t) // Y must be complete, only read - non block
{
	pFImplData->FReaders = 0;
	pFImplData->FWriters = 0;
}
template<class T>
CSafeData<T>::CSafeData(CSafeData const & aA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(aA.pFImplData->FMutex);
	MASSERT_1(!aA.pFImplData->FReaders);
	MASSERT_1(!aA.pFImplData->FWriters);
	FData = aA.FData;
	pFImplData = aA.pFImplData;
}
template<class T>
CSafeData<T> & CSafeData<T>::operator=(CSafeData const & aA)
{
	MASSERT_1(false); //TODO
	MASSERT_1(pFImplData != aA.pFImplData);
	NSHARE::CRAII<NSHARE::CMutex> _block(pFImplData->FMutex);
	NSHARE::CRAII<NSHARE::CMutex> _block2(aA.pFImplData->FMutex);

	MASSERT_1(!pFImplData->FReaders);
	MASSERT_1(!aA.pFImplData->FReaders);
	FData = aA.FData;
///		pFImplData = aA.pFImplData; //TODO
	return *this;
}
template<class T>
std::ostream& CSafeData<T>::MPrint(std::ostream & aStream) const
{
	return aStream << MGetRAccess().MGet();
}
} //namespace NSHARE
namespace std
{
template<class T>
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CSafeData<T> const& aData)
{
	return aData.MPrint(aStream);
}
} //namespace std
#endif /* CSAFEDATA_HPP_ */
