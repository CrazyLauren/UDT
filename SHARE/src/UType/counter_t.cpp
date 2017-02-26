/*
 * counter_t.cpp
 *
 *  Created on: 24.02.2017
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *	
 *	Copyright © 2017 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include<deftype>
#include <boost/version.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#if (BOOST_VERSION / 100000 >=1) &&(BOOST_VERSION / 100 % 1000<=47)
using namespace boost::interprocess::detail;
#else
using namespace boost::interprocess::ipcdetail;
#endif
namespace NSHARE
{
inline void atomic_add(volatile boost::uint32_t *mem, int val)
{
#ifdef _WIN32
	boost::uint32_t   _nval;
	boost::uint32_t   _old;
	do
	{
		_old=atomic_read32(mem);
		_nval=_old+val;
		//atomic_write32(mem,_nval);
	}while(atomic_cas32(mem,_nval,*mem)!=_old);
#else
	atomic_add32(mem,val);
#endif
}
counter_t::counter_t(value_type const& aVal)
{
	MWrite(aVal);
}
void counter_t::MWrite(value_type const& aVal) const
{
	atomic_write32(&FCount,aVal);
}
void counter_t::MIncrement() const
{
	atomic_inc32(&FCount);
}
void counter_t::MAdd(int const&  aVal) const
{
	atomic_add(&FCount, aVal);
}
counter_t::value_type counter_t::MDecrement() const
{
	return atomic_dec32(&FCount);
}

bool counter_t::MIsOne() const
{
	return MValue() == value_type(1);
}
counter_t::value_type counter_t::MValue() const
{
	return atomic_read32(&FCount);
}
}
