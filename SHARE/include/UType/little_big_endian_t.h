/*
 * little_big_endian_t.h
 *
 * Copyright © 2017  https://github.com/CrazyLauren
 *
 *  Created on: 20.05.2017
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef INCLUDE_UTYPE_LITTLE_BIG_ENDIAN_T_H_
#define INCLUDE_UTYPE_LITTLE_BIG_ENDIAN_T_H_

namespace NSHARE
{
/** \brief Convenient class for working
 * with little endian data
 *
 * \note from habr post 121811
 */
template<typename T>
struct little_endian_t;

/** \brief Convenient class for working
 * with big endian data
 *
 * \note from habr post 121811
 */
template<typename T>
struct big_endian_t;

/** \brief definition endianness types
 *
 *  \{
 */
	typedef uint8_t uint8be_t;
	typedef uint8_t uint8le_t;

	typedef big_endian_t<uint16_t> uint16be_t;
	typedef big_endian_t<uint32_t> uint32be_t;
	typedef big_endian_t<uint64_t> uint64be_t;

	typedef little_endian_t<uint16_t> uint16le_t;
	typedef little_endian_t<uint32_t> uint32le_t;
	typedef little_endian_t<uint64_t> uint64le_t;
//\}

SHARED_PACKED(template<typename T> struct little_endian_t
	{
		union
		{
			unsigned char bytes[sizeof(T)];
			T raw_value;
		};

		little_endian_t(T t = T())
		{
			operator =(t);
		}

		little_endian_t(const little_endian_t<T> & t)
		{
			raw_value = t.raw_value;
		}

		little_endian_t(const big_endian_t<T> & t)
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			bytes[i] = t.bytes[sizeof(T)-1-i];
		}

		operator const T() const
		{
			T t = T();
			for (unsigned i = 0; i < sizeof(T); i++)
			t |= T(bytes[i]) << (i << 3);
			return t;
		}

		const T operator = (const T t)
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			bytes[i] = t >> (i << 3);
			return t;
		}

		// operators

		const T operator += (const T t)
		{
			return (*this = *this + t);
		}

		const T operator -= (const T t)
		{
			return (*this = *this - t);
		}

		const T operator *= (const T t)
		{
			return (*this = *this * t);
		}

		const T operator /= (const T t)
		{
			return (*this = *this / t);
		}

		const T operator %= (const T t)
		{
			return (*this = *this % t);
		}

		little_endian_t<T> operator ++ (int)
		{
			little_endian_t<T> tmp(*this);
			operator ++ ();
			return tmp;
		}

		little_endian_t<T> & operator ++ ()
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			{
				++bytes[i];
				if (bytes[i] != 0)
				break;
			}
			return (*this);
		}

		little_endian_t<T> operator -- (int)
		{
			little_endian_t<T> tmp(*this);
			operator -- ();
			return tmp;
		}

		little_endian_t<T> & operator -- ()
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			{
				--bytes[i];
				if (bytes[i] != (T)(-1))
				break;
			}
			return (*this);
		}
	});

SHARED_PACKED(template<typename T> struct big_endian_t
	{
		union
		{
			unsigned char bytes[sizeof(T)];
			T raw_value;
		};

		big_endian_t(T t = T())
		{
			operator =(t);
		}

		big_endian_t(const big_endian_t<T> & t)
		{
			raw_value = t.raw_value;
		}

		big_endian_t(const little_endian_t<T> & t)
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			bytes[i] = t.bytes[sizeof(T)-1-i];
		}

		operator const T() const
		{
			T t = T();
			for (unsigned i = 0; i < sizeof(T); i++)
			t |= T(bytes[sizeof(T) - 1 - i]) << (i << 3);
			return t;
		}

		const T operator = (const T t)
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			bytes[sizeof(T) - 1 - i] = t >> (i << 3);
			return t;
		}

		// operators

		const T operator += (const T t)
		{
			return (*this = *this + t);
		}

		const T operator -= (const T t)
		{
			return (*this = *this - t);
		}

		const T operator *= (const T t)
		{
			return (*this = *this * t);
		}

		const T operator /= (const T t)
		{
			return (*this = *this / t);
		}

		const T operator %= (const T t)
		{
			return (*this = *this % t);
		}

		big_endian_t<T> operator ++ (int)
		{
			big_endian_t<T> tmp(*this);
			operator ++ ();
			return tmp;
		}

		big_endian_t<T> & operator ++ ()
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			{
				++bytes[sizeof(T) - 1 - i];
				if (bytes[sizeof(T) - 1 - i] != 0)
				break;
			}
			return (*this);
		}

		big_endian_t<T> operator -- (int)
		{
			big_endian_t<T> tmp(*this);
			operator -- ();
			return tmp;
		}

		big_endian_t<T> & operator -- ()
		{
			for (unsigned i = 0; i < sizeof(T); i++)
			{
				--bytes[sizeof(T) - 1 - i];
				if (bytes[sizeof(T) - 1 - i] != (T)(-1))
				break;
			}
			return (*this);
		}
	});
}



#endif /* INCLUDE_UTYPE_LITTLE_BIG_ENDIAN_T_H_ */
