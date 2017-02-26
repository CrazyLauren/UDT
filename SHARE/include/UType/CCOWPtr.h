/*
 * CCOWPtr.h
 *
 *  Created on: 24.02.2017
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *	
 *	Copyright © 2017 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCOWPTR_H_
#define CCOWPTR_H_

namespace NSHARE
{
//todo allocator
template<typename T>
class CCOWPtr
{
	struct implementation_t: NSHARE::CDenyCopying
	{
		explicit implementation_t(T const& aVal) :
				FVal(aVal)
		{
		}

		counter_t FCounter;
		T FVal;
	};
public:

	typedef T value_type;

	CCOWPtr() :
			FPtr(allocate())
	{
	}

	CCOWPtr(T const& aVal) :
			FPtr(allocate(aVal))
	{
	}

	CCOWPtr(const CCOWPtr& x) :
			FPtr(x.FPtr)
	{
		if (FPtr)
			FPtr->FCounter.MIncrement();
	}

	~CCOWPtr()
	{
		release(FPtr);
	}

	CCOWPtr& operator=(CCOWPtr x)
	{
		swap(*this, x);
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

	bool MIsOne() const
	{
		return !FPtr || FPtr->FCounter.MIsOne();
	}

	bool MIsEqual(const CCOWPtr& x) const
	{
		return FPtr == x.FPtr;
	}

	friend inline void swap(CCOWPtr& x, CCOWPtr& y)
	{
		std::swap(x.FPtr, y.FPtr);
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
private:

	static implementation_t* allocate(const T& aVal = T())
	{
		implementation_t* _tmp = NULL;

		try
		{
			_tmp = ::new implementation_t(aVal);
		} catch (...)
		{
			delete _tmp;
			throw;
		}
		return _tmp;
	}

	static void release(implementation_t* aVal)
	{

		if (aVal == 0 || aVal->FCounter.MDecrement() != 0)
			return;
		delete aVal;
	}

	void reset(implementation_t* to)
	{
		release(FPtr);
		FPtr = to;
	}

	implementation_t* FPtr;
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
