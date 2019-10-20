/*
 *
 * type_manip.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.09.2011
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef TYPEMANIP_H_
#define TYPEMANIP_H_

namespace NSHARE
{

template<int v>
struct int2type_t
{
	enum
	{
		value = v
	};
};

template<typename T>
struct type2type_t
{
	typedef T OriginalType;
};

template<bool flag, typename T, typename U>
struct selector_t
{
	typedef T result_t;
};
template<typename T, typename U>
struct selector_t<false, T, U>
{
	typedef U result_t;
};

template<typename T, typename U>
struct is_equal
{
	enum
	{
		result = false
	};
};

template<typename T>
struct is_equal<T, T>
{
	enum
	{
		result = true
	};
};
template<unsigned T, unsigned U>
struct is_val_equal
{
	enum
	{
		result = false
	};
};

template<unsigned T>
struct is_val_equal<T, T>
{
	enum
	{
		result = true
	};
};
//From T to U
template<class T, class U>
struct conversion_t
{
	typedef char False;
	struct True
	{
		char dummy[2];
	};
	static False Test(...);
	static True Test(U const&);
	static T MakeT();

	enum
	{
		result = sizeof(True) == sizeof((Test(MakeT())))
	};
	enum
	{
		equal = is_equal<T, U>::result
	};
};

template<class T>
struct conversion_t<void, T>
{
	enum
	{
		result = 0, equal = 0
	};
};

template<class T>
struct conversion_t<T, void>
{
	enum
	{
		result = 0, equal = 0
	};
};

template<>
struct conversion_t<void, void>
{
public:
	enum
	{
		result = 1, equal = 1
	};
};

//T -parent,  U -child
template <class T, class U>
struct is_parent
{
	enum
	{
		result = (conversion_t<const volatile U*, const volatile T*>::result
				&& !conversion_t<const volatile T*, const volatile void*>::equal)
	};
};
template <>
struct is_parent<void, void>
{
	 enum { result = false };
};
}   // namespace NSHARE

//T -parent,  U -child
#define IS_PARENTCLASS(T, U) \
    NSHARE::is_parent<T,U>::result

#endif //#ifndef TYPEMANIP_H_
