/*
 * CSharedCounter.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 23.03.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSHAREDCOUNTER_H_
#define CSHAREDCOUNTER_H_

namespace NSHARE
{

template<class Y = CMutexEmpty>
class  shared_count
{
	//fixme replace to atomic
	template<class TMutex>
	class  sp_counted_base
	{
	private:

		sp_counted_base(sp_counted_base const &);
		sp_counted_base & operator=(sp_counted_base const &);

		int FCount;        // #shared
		TMutex FMutex;
	public:

		sp_counted_base() :
				FCount(1)
		{
		}

		void add_ref_copy()
		{
			typename NSHARE::CRAII<TMutex> _lock(FMutex);
			++FCount;
		}

		void release() // nothrow
		{
			typename NSHARE::CRAII<TMutex> _lock(FMutex);
			--FCount;
		}

		long use_count() const // nothrow
		{
			return static_cast<int const volatile &>(FCount);
		}
	};

	sp_counted_base<Y> * FCount;

public:

	shared_count() :
			FCount(0) // nothrow

	{
	}

	~shared_count() // nothrow
	{
		if (FCount != 0)
			FCount->release();
	}

	shared_count(shared_count const & r) :
			FCount(r.FCount) // nothrow
	{
		if (FCount != 0)
			FCount->add_ref_copy();
	}
	shared_count & operator=(shared_count const & r) // nothrow
	{
		sp_counted_base<Y> * tmp = r.FCount;

		if (tmp != FCount)
		{
			if (tmp != 0)
				tmp->add_ref_copy();
			if (FCount != 0)
				FCount->release();
			FCount = tmp;
		}

		return *this;
	}

	void swap(shared_count & r) // nothrow
	{
		sp_counted_base<Y> * tmp = r.FCount;
		r.FCount = FCount;
		FCount = tmp;
	}

	long use_count() const // nothrow
	{
		return FCount != 0 ? FCount->use_count() : 0;
	}

	bool unique() const // nothrow
	{
		return use_count() == 1;
	}

	bool empty() const // nothrow
	{
		return FCount == 0;
	}

	friend inline bool operator==(shared_count const & a,
			shared_count const & b)
	{
		return a.FCount == b.FCount;
	}

	friend inline bool operator<(shared_count const & a, shared_count const & b)
	{
		return std::less<sp_counted_base<Y> *>()(a.FCount, b.FCount);
	}
};
};
#endif /* CSHAREDCOUNTER_H_ */
