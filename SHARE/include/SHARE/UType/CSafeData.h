/*
 * CSafeData.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.08.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSAFEDATA_H_
#define CSAFEDATA_H_

/**\brief Выбор типа используемой RW блокировки
*/
#if !defined(USE_SHARE_RW) // This one can be defined by users, so it should go first
#	ifdef _WIN32
#		define SHARE_RW_USE_SRWLOCK
#	elif defined(unix) || defined(__QNX__)
#		define SHARE_RW_USE_PTHREAD
#	else
#		define USE_SHARE_RW
#	endif

#endif

namespace NSHARE
{
namespace _impl
{
/**\brief Определение данных для конкретной реализации
*/
#ifdef USE_SHARE_RW
struct safe_data_t
{
	safe_data_t():FReadSignal(),FWriteSignal(),FMutex(CMutex::MUTEX_NORMAL),FWriters(0),FReaders(0),FWritersLock(0)
	{

	}
	CCondvar FReadSignal;//-1 locked for write
	CCondvar FWriteSignal;
	CMutex  FMutex;
	volatile int FWriters;
	volatile int FReaders;
	volatile int FWritersLock;
};
#elif defined(SHARE_RW_USE_SRWLOCK)

extern "C" {

struct SRWLOCK { void* p; };
__declspec(dllimport) void __stdcall InitializeSRWLock(SRWLOCK*);
__declspec(dllimport) void __stdcall ReleaseSRWLockExclusive(SRWLOCK*);
__declspec(dllimport) void __stdcall ReleaseSRWLockShared(SRWLOCK*);
__declspec(dllimport) void __stdcall AcquireSRWLockExclusive(SRWLOCK*);
__declspec(dllimport) void __stdcall AcquireSRWLockShared(SRWLOCK*);

} // extern "C"

struct safe_data_t
{
	safe_data_t():FWritersLock(0)
	{
		 InitializeSRWLock(&FMutex);
	}
	SRWLOCK FMutex;
	volatile int FWritersLock;
};
#elif defined(SHARE_RW_USE_PTHREAD)
struct safe_data_t
{
	safe_data_t():FWritersLock(0)
	{
		pthread_rwlock_init(&FMutex, NULL);
	}
	~safe_data_t()
	{
		 pthread_rwlock_destroy(&FMutex);
	}
	pthread_rwlock_t FMutex;
	volatile int FWritersLock;
};
#endif
}
/**\brief RW lock
*
*   Фактически этот класс это реализация RAII для блокировки чтения записи. Использоватеь его очень легкою
*
*	\code
*	CSafeData<int> _rw_int;
*	{ 
*		CSafeData<int>::WAccess<> _locked_int(_rw_int);\\блокируем на запись
*		*_rw_int=10;
*	}//здесь отработает RAII
*	{
*		CSafeData<int>::RAccess<> _locked_int(_rw_int);\\блокируем на чтение
*		std::cout<<(*_rw_int)<<std::endl;
*	}//здесь отработает RAII
*	\endcode
*
*	Выбор реализации блокировки чтения-записи осуществляется путём установки препроцессоров:
*		USE_SHARE_RW - реализация RW блокировки при помощи двух УП и мютекса, эта реализация
*				не зависит от типа ОС.
*		SHARE_RW_USE_SRWLOCK - реализация RW блокировки SRWLOCK в windows. Для её работы требуется ОС Vista и выше.
*		SHARE_RW_USE_PTHREAD - реализация RW блокировки SRWLOCK в pthread.
*\warning Недопускается рекурсивный вызов RW блокировки. Т.е. если данные были заблокированы
* только для чтения, то для того чтобы захватить их для записи, нужно вначале "освободить" блокировку
* чтения.
*/
template<class T> class  CSafeData
{
public:
	typedef T value_type;
	typedef T* pointer;

	
	template<class Y = T>
	class  WAccess;
	template<class Y = T>
	class  RAccess;


	CSafeData();

	CSafeData(CSafeData const & aA);
	CSafeData & operator=(CSafeData const & aA);

	template<class Y>
	explicit CSafeData(Y  const& aP);

	template<class Y>
	explicit CSafeData(CSafeData<Y> const & aA);
	template<class Y>
	CSafeData & operator=(CSafeData<Y> const & aA);

	WAccess<T> operator*()
	{
		return WAccess<T>(*this);
	}
	WAccess<T> MGetWAccess()
	{
		return WAccess<T>(*this);
	}
	RAccess<T> const operator*() const //only read
	{
		return RAccess<T>(*this);
	}
	RAccess<T> const MGetRAccess() const
	{
		return RAccess<T>(*this);
	}
	WAccess<T> operator->()
	{
		return WAccess<T>(*this);
	}
	RAccess<T> const operator->() const
	{
		return  RAccess<T>(*this);
	}
	CSafeData & operator=(value_type const & aData)
	{
		MGetWAccess().operator=(aData);
		return *this;
	}
	T const& get() const
	{
		return FData;
	}

	inline std::ostream& MPrint(std::ostream & aStream) const;
private:
	T FData;
	mutable _impl::safe_data_t FImpl; // blocker
};
template<class T>
inline CSafeData<T>::CSafeData() :
		FData(), FImpl()
{
}
template<class T>
template<class Y>
inline CSafeData<T>::CSafeData(Y const& aP) :
		FData(aP), FImpl() // Y must be complete, only read - non block
{
}
template<class T>
template<class Y>
inline CSafeData<T>::CSafeData(CSafeData<Y> const& aA) :
		FImpl()
{
	{
		typename CSafeData<Y>::template RAccess<> const _access=aA.MGetRAccess();
		FData = aA.FData;
	}
}
template<class T>
inline CSafeData<T>::CSafeData(CSafeData const & aA):
	FImpl()
{
	{
		CSafeData::RAccess<> const _access=aA.MGetRAccess();
		FData = aA.FData;
	}
}
template<class T>
inline CSafeData<T> & CSafeData<T>::operator=(CSafeData const & aA)
{
	if(&aA==this)
		return *this;
	{
		CSafeData::WAccess<> _waccess = MGetWAccess();
		{
			CSafeData::RAccess<> const _access = aA.MGetRAccess();
			FData = aA.FData;
		}
	}
	return *this;
}
template<class T>
template<class Y>
inline CSafeData<T> & CSafeData<T>::operator=(CSafeData<Y> const & aA)
{
	MASSERT_1((&aA!=this) &&"Wtf!");
	{
		CSafeData::WAccess<> _waccess = MGetWAccess();
		{
		 typename CSafeData<Y>::template RAccess<> const _access = aA.MGetRAccess();
			FData = aA.FData;
		}
	}
	return *this;
}
template<class T>
std::ostream& CSafeData<T>::MPrint(std::ostream & aStream) const
{
	return aStream << MGetRAccess().MGet();
}
} //namespace USHARE
namespace std
{
template<class T>
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CSafeData<T> const& aData)
{
	return aData.MPrint(aStream);
}
} //namespace std

#ifdef USE_SHARE_RW
#	include <SHARE/UType/CSafeData.hpp>
#elif defined(SHARE_RW_USE_SRWLOCK)
#	include <SHARE/UType/CSafeDataWin32.h>
#elif defined(SHARE_RW_USE_PTHREAD)
#	include <SHARE/UType/CSafeDataPosix.h>
#endif


#endif /* CSAFEDATA_H_ */
