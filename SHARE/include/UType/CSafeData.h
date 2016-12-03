/*
 * CSafeData.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.08.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSAFEDATA_H_
#define CSAFEDATA_H_

namespace NSHARE
{
template<class T> class  CSafeData
{
public:
	typedef T value_type;
	typedef T* pointer;

	//pthread asserts
	template<class Y = T>
	class  WAccess;
	template<class Y = T>
	class  RAccess;


	CSafeData();
	template<class Y>
	explicit CSafeData(Y  const& aP);
	CSafeData(CSafeData const & aA);
	CSafeData & operator=(CSafeData const & aA);

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
//	inline operator value_type() const
//	{
//		value_type _copy = MGetRAccess();
//		return _copy;
//	}
	T const& get() const//FIXME error, using MGetWAccess or MGetRAccess
	{
		return FData;
	}

	typedef RAccess<T> const RAtomic;//FIXME depreceted
	typedef WAccess<T> WAtomic;//FIXME depreceted
/*
	RAtomic MGetAtomic() const
	{
		return *this;
	}
	WAtomic MGetAtomic()
	{
		return *this;
	}
*/
	inline std::ostream& MPrint(std::ostream & aStream) const;
private:
	T FData; // contained pointer
	struct data_t
	{
		data_t():FReadSignal(),FWriteSignal(),FMutex(CMutex::MUTEX_NORMAL),FWriters(0),FReaders(0)
		{

		}
		CCondvar FReadSignal;//-1 locked for write
		CCondvar FWriteSignal;
		CMutex  FMutex;
		volatile int FWriters;
		volatile int FReaders;
	};
	mutable SHARED_PTR<data_t> pFImplData; // blocker
};
} //namespace USHARE
#include <UType/CSafeData.hpp>
#endif /* CSAFEDATA_H_ */
