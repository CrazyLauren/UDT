/*
 * atomic_t.h
 *
 *  Created on: 24.02.2017
 *      Author:  https://github.com/CrazyLauren
 *	
 *	Copyright © 2017  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef ATOMIC_T_H_
#define ATOMIC_T_H_


#if __cplusplus >= 201103L && 0//todo for c++11, copy operation of atomic_t
#	include <atomic>
#elif defined(NOATOMIC)
// do not do anything
#elif defined(__QNX__)
#if	defined(__arm__) ||defined(__mips__)
#		define USING_QNX_ATOMIC
#	endif
#endif
namespace NSHARE
{
	namespace impl//definition in impl as atomic_t defined in macro SHARED_PACKED
	{
#if __cplusplus >= 201103L && 0//todo for c++11, copy operation of atomic_t
		typedef std::atomic<uint32_t> atomic_value_type;
#else
		typedef uint32_t atomic_value_type;
#endif
	}
SHARED_PACKED(struct SHARE_EXPORT atomic_t
		{
		public:
			typedef impl::atomic_value_type value_type;

			explicit atomic_t(value_type const& aVal=value_type(1));

			void MAdd(int const& aVal) ;
			value_type MIncrement() ;
			value_type MDecrement() ;
			bool MIsOne() const;
			value_type MValue() const;
			void MWrite(value_type const& aVal) ;
			inline operator value_type() const {
				return MValue();
			}
			inline void operator-=(const value_type &i);
			inline void operator+=(const value_type &i);
			inline value_type operator++(int);
			inline value_type operator--(int);
			inline value_type operator++();
			inline value_type operator--();
			inline value_type operator=(const value_type&);
		private:
			mutable volatile value_type FCount;
		});
inline atomic_t::value_type atomic_t::operator=(const value_type& aVal)
{
	MWrite(aVal);
	return MValue();
}
inline void atomic_t::operator-=(const value_type &i)
{
	MAdd(-static_cast<int>(i));
}
inline void atomic_t::operator+=(const value_type &i)
{
	MAdd(static_cast<int>(i));
}
inline atomic_t::value_type atomic_t::operator++()
{
	MIncrement();
	return MValue();
}
inline atomic_t::value_type atomic_t::operator--()
{
	MDecrement();
	return MValue();
}
inline atomic_t::value_type atomic_t::operator++(int)
{
	return MIncrement();
}
inline atomic_t::value_type atomic_t::operator--(int)
{
	return MDecrement();
}
}

#endif /* COUNTER_H_ */
