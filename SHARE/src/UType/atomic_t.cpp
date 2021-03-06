// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * atomic_t.cpp
 *
 *  Created on: 24.02.2017
 *      Author:  https://github.com/CrazyLauren
 *	
 *	Copyright © 2017  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include<deftype>

COMPILE_ASSERT(sizeof(NSHARE::atomic_t) == sizeof(uint32_t),
		InvalidSizeofAtomic);

COMPILE_ASSERT(__alignof(NSHARE::atomic_t) <= 4,
		InvalidAligofAtomic);

#if __cplusplus >= 201103L && 0//todo for c++11, copy operation of atomic_t
namespace NSHARE
{
//	atomic_t::atomic_t(value_type const& aVal):FCount(0)
//	{
//		MWrite(aVal);
//	}
	void atomic_t::MWrite(value_type const& aVal)
	{
		FCount=aVal;
	}
	atomic_t::value_type atomic_t::MIncrement()
	{
		return FCount--;
	}
	atomic_t::value_type atomic_t::MDecrement()
	{
		return FCount--;
	}
	void atomic_t::MAdd(int const& aVal)
	{
		FCount+=aVal;
	}

	bool atomic_t::MIsOne() const
	{
		return MValue() == value_type(1);
	}
	atomic_t::value_type atomic_t::MValue() const
	{
		return FCount;
	}
}
#elif defined(NOATOMIC)
namespace NSHARE
{
	static NSHARE::CMutex g_lock;
//	atomic_t::atomic_t(value_type const& aVal) :
//		FCount(0)
//	{
//		MWrite(aVal);
//	}
	void atomic_t::MWrite(value_type const& aVal)
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_lock);
		FCount = aVal;
	}
	atomic_t::value_type atomic_t::MIncrement()
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_lock);
		return FCount++;
	}
	atomic_t::value_type atomic_t::MDecrement()
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_lock);
		return FCount--;
	}
	void atomic_t::MAdd(int const& aVal)
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_lock);
		FCount += aVal;
	}
	bool atomic_t::MIsOne() const
	{
		return MValue() == value_type(1);
	}
	atomic_t::value_type atomic_t::MValue() const
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_lock);
		return FCount;
	}
}
#elif defined(USING_QNX_ATOMIC)
#	include <pthread.h>
namespace NSHARE
{
//	atomic_t::atomic_t(value_type const& aVal):FCount(0)
//	{
//		MWrite(aVal);
//	}
	void atomic_t::MWrite(value_type const& aVal)
	{
		DCHECK_POINTER_ALIGN(&FCount);
		volatile value_type _val=aVal;//will be change
		_smp_xchg(&FCount,_val);
	}
	atomic_t::value_type atomic_t::MIncrement()
	{
		DCHECK_POINTER_ALIGN(&FCount);
		for (;;)
		{
			volatile const value_type old_value = MValue();
			volatile const value_type new_value = old_value + 1;
			if(_smp_cmpxchg((volatile unsigned *)&FCount,
			                   (unsigned)old_value,
			                   (unsigned)new_value)==old_value)
			{
				return new_value;
			}
		}
	}
	atomic_t::value_type atomic_t::MDecrement()
	{
		DCHECK_POINTER_ALIGN(&FCount);
		for (;;)
		{
			volatile const value_type old_value = MValue();
			volatile const value_type new_value = old_value - 1;
			if(_smp_cmpxchg((volatile unsigned *)&FCount,
			                   (unsigned)old_value,
			                   (unsigned)new_value)==old_value)
			{
				return new_value;
			}
		}
	}
	void atomic_t::MAdd(int const& aVal)
	{
		DCHECK_POINTER_ALIGN(&FCount);
		for (;;)
		{
			volatile const value_type old_value = MValue();
			volatile const value_type new_value = old_value + aVal;
			if(_smp_cmpxchg((volatile unsigned *)&FCount,
			                   (unsigned)old_value,
			                   (unsigned)new_value)==old_value)
			{
				break;
			}
		}
	}
	bool atomic_t::MIsOne() const
	{
		return MValue() == value_type(1);
	}
	atomic_t::value_type atomic_t::MValue() const
	{
		return FCount;
	}
}
#else
#	include <boost/version.hpp>
#	include <boost/interprocess/detail/atomic.hpp>

#	if (BOOST_VERSION / 100000 >=1) &&(BOOST_VERSION / 100 % 1000<=47)
using namespace boost::interprocess::detail;
#	else
using namespace boost::interprocess::ipcdetail;
#	endif

namespace NSHARE
{
inline void atomic_add(volatile boost::uint32_t *mem, int val)
{
	DCHECK_POINTER_ALIGN(mem);

#	ifdef _WIN32
	boost::uint32_t _nval;
	boost::uint32_t _old;
	do
	{
		_old = atomic_read32(mem);
		_nval = _old + val;
		//atomic_write32(mem,_nval);
	} while (atomic_cas32(mem, _nval, *mem) != _old);
#	else
	atomic_add32(mem,val);
#	endif
}
//atomic_t::atomic_t(value_type const& aVal) :
//		FCount(0)
//{
//	MWrite(aVal);
//}
atomic_t::value_type atomic_t::MWriteIfEqual(value_type const& aVal,value_type const& aEqualOf)
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_cas32(&FCount, aVal, aEqualOf);
}
atomic_t::value_type atomic_t::MWriteIfEqual(value_type volatile const& aVal,value_type volatile const& aEqualOf) volatile
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_cas32(&FCount, aVal, aEqualOf);
}
void atomic_t::MWrite(value_type const& aVal)
{
	DCHECK_POINTER_ALIGN(&FCount);
	atomic_write32(&FCount, aVal);
}
void atomic_t::MWrite(value_type const& aVal) volatile
{
	DCHECK_POINTER_ALIGN(&FCount);
	atomic_write32(&FCount, aVal);
}
atomic_t::value_type atomic_t::MIncrement()
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_inc32(&FCount);
}
atomic_t::value_type atomic_t::MIncrement() volatile
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_inc32(&FCount);
}
atomic_t::value_type atomic_t::MDecrement()
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_dec32(&FCount);
}
atomic_t::value_type atomic_t::MDecrement() volatile
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_dec32(&FCount);
}
void atomic_t::MAdd(int const& aVal)
{
	DCHECK_POINTER_ALIGN(&FCount);
	atomic_add(&FCount, aVal);
}
void atomic_t::MAdd(int const& aVal) volatile
{
	DCHECK_POINTER_ALIGN(&FCount);
	atomic_add(&FCount, aVal);
}
bool atomic_t::MIsOne() const
{
	DCHECK_POINTER_ALIGN(&FCount);
	return MValue() == value_type(1);
}
bool atomic_t::MIsOne() volatile const
{
	DCHECK_POINTER_ALIGN(&FCount);
	return MValue() == value_type(1);
}
atomic_t::value_type atomic_t::MValue() const
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_read32(&FCount);
}
atomic_t::value_type atomic_t::MValue() volatile const
{
	DCHECK_POINTER_ALIGN(&FCount);
	return atomic_read32(&FCount);
}
}
#endif

