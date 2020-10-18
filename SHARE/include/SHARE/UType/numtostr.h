/*
 * numtostr.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 18.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef NUMTOSTR_H_
#define NUMTOSTR_H_
#define NUM_TO_STRING_EXIST

/**\brief преобразование стандартных типов в строку
 *
 * стандартный ostream определяет преобразование типов
 * с учётом локалей, чтобы этого избежать есть два варианта
 * либо установить "глобальный" локаль, либо написать свой
 * костыль.
 *
 *\todo заменить на boost lexical_cast
 */
namespace NSHARE
{
template<class TStr>
inline bool bool_to_str(bool const& aVal, TStr &aTo)
{
	if (aVal)
		aTo = "true";
	else
		aTo = "false";
	return true;
}

template<typename T, class TStr>
inline bool num_to_str(T const& aVal, TStr &aTo, unsigned const aBase = 10)
{
	assert(std::numeric_limits<T>::is_integer);
	size_t _count = (aVal <= 0 ? 1 : 0);
	T const _base = static_cast<T const>(aBase);
	for (T _sum = aVal; _sum; ++_count)
		_sum /= _base;
	if (_base == 16 || _base == 2)
		_count += 2;
	else if (_base != 10)
		return false;
	const size_t _size = aTo.length();
	aTo.resize(_count + _size);
	typename TStr::iterator _type_it = aTo.begin() + _size;
	//fucking warning: warning: comparison is always false due to limited range of data type
	//for checking whether a number positive or negative using bitwise operation
	//if (std::numeric_limits<T>::is_signed && aVal<0)
	if (std::numeric_limits<T>::is_signed && (aVal >> (sizeof(aVal) * 8 - 1)))
			*_type_it++ = '-';

	if (_base == 16)
	{
		*_type_it++ = '0';
		*_type_it++ = 'x';
	}
	else if (_base == 2)
	{
		*_type_it++ = '0';
		*_type_it++ = 'b';
	}

	typename TStr::reverse_iterator _it = aTo.rbegin();
	int _digit;
	T _sum = aVal;
	if (_sum > 0 ||_sum==0)
		do
		{
			_digit =(int) (_sum % _base);
			if (_digit < 0xA)
				*_it = (char)((int)'0' + _digit);
			else
				*_it = (char)((int)'A' + _digit - 0xA);

			_sum /= _base;
			++_it;
		} while (_sum);
	else
		for (; _sum; ++_it)
		{
			_digit =(int) (_sum % _base);
			if (_digit < 0xA)
				*_it = (char)((int)'0' - _digit);
			else
				*_it = (char)((int)'A' - _digit - 0xA);

			_sum /= _base;
		}
	return true;
}
template<typename T, class TStr>
inline bool float_to_str(T f, TStr &aTo, int const aPrecision = static_cast<int>((std::numeric_limits<T>::digits*643./2136.+2)+1))
{
	assert(!std::numeric_limits<T>::is_integer);
	int i;
	int z;
	int exp = 0;

	if (f < 0)
	{
		aTo.push_back('-');
		f = -f;
	}

	if (f)
	{
		while (f < 1)
		{
			f *= 10;
			exp--;
		}

		while (f >= 10)
		{
			f /= 10;
			exp++;
		}
	}
	while ((exp > 0) && (exp <= std::numeric_limits<T>::digits10))
	{
		aTo.push_back('0' + static_cast<int>(f));
		z = static_cast<int>(f);
		f -= z;
		f *= 10;
		exp--;
	}

	aTo.push_back('0' + static_cast<int>(f));
	z = static_cast<int>(f);
	f -= z;
	f *= 10;

	aTo.push_back('.');

	int _odd = 0;
	for (i = 0; i < aPrecision; i++)
	{
		aTo.push_back('0' + static_cast<int>(f));
		if (static_cast<int>(f) == 0)
			++_odd;
		else
			_odd = 0;
		z = static_cast<int>(f);
		f -= z;
		f *= 10;
	}
	if (_odd > 0)
	{
		if (_odd == aPrecision)
			aTo.erase(aTo.end() - _odd - 1, aTo.end()); //1 - point
		else
			aTo.erase(aTo.end() - _odd, aTo.end());
	}

	if (exp != 0)
	{

		aTo.push_back('e');
		if (exp < 0)
		{
			aTo.push_back('-');
			exp = -exp;
		}
		else
		{
			aTo.push_back('+');
		}
		char _buf[8]="";//max 2^64
		int i=0;
		do
		{
			int const _digit = (char) (exp % 10);
			_buf[i++]='0' + _digit;
			exp /= 10;
		} while (exp);
		for(;--i>=0;)
		{
			aTo.push_back(_buf[i]);
		}
//		int const _digit=exp/10;//fixme
//		aTo.push_back('0' + _digit);
//		exp -= _digit * 10;
//		aTo.push_back('0' + exp);

	}
	return true;
}
class CText;
template<class T>
inline CText fund_to_str(T const & aNum)
{
	std::stringstream out;
	//out << std::setprecision(20) << aNum;
	out << aNum;
	return CText(out.str());
}
template<>
inline CText fund_to_str<bool>(bool const & aVal)
{
	CText _str;
	NSHARE::bool_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<unsigned char>(unsigned char const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<signed char>(signed char const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<unsigned short int>(
		unsigned short int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<short int>(short int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<unsigned int>(unsigned int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<int>(int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<unsigned long int>(
		unsigned long int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<long int>(long int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
#ifdef SIZE_OF_LONG_LONG_INT
template<>
inline CText fund_to_str<long long int>(long long int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<unsigned long long int>(unsigned long long int const & aVal)
{
	CText _str;
	NSHARE::num_to_str(aVal, _str);
	return _str;
}
#endif
template<>
inline CText fund_to_str<double>(double const & aVal)
{
	CText _str;
	NSHARE::float_to_str(aVal, _str);
	return _str;
}
template<>
inline CText fund_to_str<float>(float const & aVal)
{
	CText _str;
	NSHARE::float_to_str(aVal, _str);
	return _str;
}
#ifdef SIZE_OF_LONG_DOUBLE
template<>
inline CText fund_to_str<long double>(long double const & aVal)
{
	CText _str;
	NSHARE::float_to_str(aVal, _str);
	return _str;
}
#endif
template<class T>
inline CText to_string(T const & aVal)
{
	return CText(fund_to_str<T>(aVal));
}
template<>
inline CText to_string<>(char const & aVal)
{
	return CText(fund_to_str<int>(aVal));
}
}

#endif /* NUMTOSTR_H_ */
