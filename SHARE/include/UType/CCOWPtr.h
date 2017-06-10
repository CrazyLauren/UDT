/*
 * CCOWPtr.h
 *
 *  Created on: 24.02.2017
 *      Author:  https://github.com/CrazyLauren
 *	
 *	Copyright © 2017  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCOWPTR_H_
#define CCOWPTR_H_

namespace NSHARE
{
/** \brief template for Copy-On-Write idiom
 *
 */
template<typename T>
class CCOWPtr
{
	struct implementation_t: NSHARE::CDenyCopying
	{
		explicit implementation_t(T const& aVal) :
				FVal(aVal)
		{
		}

		atomic_t FCounter;
		T FVal;
	};
public:
	typedef T value_type;
	typedef IAllocater allocator_type;

	explicit CCOWPtr(allocator_type * aAllocator=get_default_allocator()) :
			FAllocator(aAllocator),//
			FPtr(allocate())
	{
	}

	CCOWPtr(T const& aVal,allocator_type * aAllocator=get_default_allocator()) :
		FAllocator(aAllocator),//
		FPtr(allocate(aVal))
	{
	}

	CCOWPtr(const CCOWPtr& x) :
			FAllocator(x.FAllocator),//
			FPtr(x.FPtr)
	{
		hold(FPtr);
	}

	~CCOWPtr()
	{
		release(FPtr);
		FAllocator=NULL;
	}
	CCOWPtr& operator=(CCOWPtr const& aRht)
	{
		if (FPtr != aRht.FPtr || //
				FAllocator != aRht.FAllocator)
		{
			release(FPtr);
			hold(aRht.FPtr);

			FAllocator=aRht.FAllocator;
			FPtr=aRht.FPtr;
		}
		return *this;
	}

	CCOWPtr& operator=(T const& aVal)
	{
		if (!FPtr)
			FPtr = allocate(aVal);
		else if (FPtr->FCounter.MIsOne())
			FPtr->FVal = aVal;
		else
			reset(allocate(aVal));
		return *this;
	}

	value_type& MWrite()
	{
		using namespace std;
		assert(FPtr && "NULL Ptr");

		if (!FPtr->FCounter.MIsOne())
			reset(allocate(FPtr->FVal));

		return FPtr->FVal;
	}

	const value_type& MRead() const
	{
		using namespace std;
		assert(FPtr && "NULL Ptr");
		return FPtr->FVal;
	}

	operator const value_type&() const
	{
		return MRead();
	}

	const value_type& operator*() const
	{
		return MRead();
	}

	const value_type* operator->() const
	{
		return &MRead();
	}
	const T* get() const
	{
		return &MRead();
	}
	T* get()
	{
		return &MWrite();
	}
	bool MIsOne() const
	{
		return !FPtr || FPtr->FCounter.MIsOne();
	}

	bool MIsEqual(const CCOWPtr& x) const
	{
		return FPtr == x.FPtr;
	}
	bool MIsAllocatorEqual(IAllocater* aAlloc) const
	{
		return FAllocator==aAlloc;
	}
	IAllocater*  MGetAllocator() const
	{
		return FAllocator;
	}
	friend inline void swap(CCOWPtr& x, CCOWPtr& y)
	{
		if (x.FPtr != y.FPtr || //
				x.FAllocator != y.FAllocator)
		{
			std::swap(x.FPtr, y.FPtr);
			std::swap(x.FAllocator, y.FAllocator);
		}
	}

	friend inline bool operator<(const CCOWPtr& aLt, const CCOWPtr& aRht)
	{
		return aRht.FPtr && (!aLt.FPtr || (!aLt.MIsEqual(aRht) && *aLt < *aRht));
	}

	friend inline bool operator>(const CCOWPtr& aLt, const CCOWPtr& aRht)
	{
		return aRht < aLt;
	}

	friend inline bool operator<=(const CCOWPtr& aLt, const CCOWPtr& aRht)
	{
		return !(aRht < aLt);
	}

	friend inline bool operator>=(const CCOWPtr& aLt, const CCOWPtr& aRht)
	{
		return !(aLt < aRht);
	}

	friend inline bool operator==(const CCOWPtr& aLt, const CCOWPtr& aRht)
	{
		return aLt.MIsEqual(aRht) || (aLt.FPtr && aRht.FPtr && *aLt == *aRht);
	}

	friend inline bool operator!=(const CCOWPtr& aLt, const CCOWPtr& aRht)
	{
		return !(aLt == aRht);
	}
	bool MIs() const
	{
		return FPtr!=NULL;
	}
	static bool sMUnitTest();
private:

	implementation_t* allocate(const T& aVal = T())
	{
		using namespace std;
		implementation_t* _tmp = NULL;
		assert(FAllocator && "NULL allocator");
		try
		{
			_tmp =allocate_object<implementation_t>(*FAllocator,aVal);
		} catch (...)
		{
			deallocate_object(*FAllocator,_tmp);
			throw;
		}
		return _tmp;
	}

	void release(implementation_t* aVal) const
	{
		using namespace std;
		if (aVal == 0 || aVal->FCounter.MDecrement() != 1)
			return;
		assert(FAllocator && "NULL allocator");
		deallocate_object(*FAllocator,aVal);
	}
	void hold(implementation_t* aVal)
	{
		if (aVal)
			aVal->FCounter.MIncrement();
	}
	void reset(implementation_t* to)
	{
		release(FPtr);
		FPtr = to;
	}
	allocator_type* FAllocator;
	mutable implementation_t* FPtr;
};
}

namespace std
{
template<typename T>
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::CCOWPtr<T> & aWhat)
{
	return aStream<<aWhat.MRead();
}
}
#endif /* CCOWPTR_H_ */
