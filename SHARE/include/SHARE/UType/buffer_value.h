/*
 * buffer_value.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 29.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef BUFFER_VALUE_H_
#define BUFFER_VALUE_H_

namespace NSHARE
{
/*!\brief convenient template for 
 * wrapping standard types
 * 
 * It's used to streams (and another templates)
 * work correctly.
 * 
 */
template<class T>
struct buf_val_t
{
	buf_val_t()
	{
	}
	;
	buf_val_t(T const & aVal) :
			FVal(aVal)
	{
	}
	;
	template<typename U>
	explicit buf_val_t(U const & aVal) :
			FVal(aVal)
	{
	}
	;

	operator T()
	{
		return FVal;
	}
	operator T const() const
	{
		return FVal;
	}
	inline void operator-=(const buf_val_t &i)
	{
		FVal -= i.FVal;
	}
	inline void operator+=(const buf_val_t &i)
	{
		FVal += i.FVal;
	}

	inline buf_val_t operator++(int)
	{
		return FVal++;
	}
	inline buf_val_t operator--(int)
	{
		return FVal--;
	}
	inline buf_val_t& operator++()

	{
		++FVal;
		return *this;
	}
	inline buf_val_t& operator--()
	{
		--FVal;
		return *this;
	}
	inline buf_val_t& operator=(const buf_val_t& aVal)
	{
		FVal = aVal.FVal;
		return *this;
	}
	inline void operator-=(const buf_val_t &i) volatile
	{
		FVal -= i.FVal;
	}
	inline void operator+=(const buf_val_t &i) volatile
	{
		FVal += i.FVal;
	}

	inline buf_val_t operator++(int) volatile
	{
		return FVal++;
	}
	inline buf_val_t operator--(int) volatile
	{
		return FVal--;
	}
	inline buf_val_t& operator++() volatile

	{
		++FVal;
		return *this;
	}
	inline buf_val_t& operator--() volatile
	{
		--FVal;
		return *this;
	}


	T FVal;
};
#ifdef COMPILE_ASSERT
  COMPILE_ASSERT(sizeof(buf_val_t<uint8_t>)==sizeof(uint8_t),InvalidSizeofBufVal8);
  COMPILE_ASSERT(sizeof(buf_val_t<uint16_t>)==sizeof(uint16_t),InvalidSizeofBufVal16);
  COMPILE_ASSERT(sizeof(buf_val_t<uint32_t>)==sizeof(uint32_t),InvalidSizeofBufVal32);
#endif

namespace detail
{
template<class T>
inline std::ostream& print_b_value(std::ostream& aStream, T const& aVal)
{
	aStream.setf(std::ios::hex, std::ios::basefield);
	typedef unsigned char const _array_t[sizeof(T)];
	_array_t const&  _array=reinterpret_cast<_array_t const&>(aVal);

	aStream.setf(std::ios::hex, std::ios::basefield);
	aStream << "0x";
	for(unsigned i=0;i<sizeof(_array_t);++i)
		aStream<< static_cast<unsigned const>(_array[i]);
	aStream.unsetf(std::ios::hex);
	return aStream;
}
};

/*!\brief Printing data buffer
 * 
 * The data buffer is printed 
 * in \a NCollums collums in hex format.
 * 
 *\tparam T iterator or pointer
 *\tparam NCollums Number of collums (default 8).
 */
template<class T, unsigned NCollums>
inline std::ostream& print_buffer(std::ostream& aStream, T aBegin, T aEnd)
{
	unsigned _i = 0;
	for (; aBegin != aEnd; ++aBegin)
	{
		detail::print_b_value(aStream,*aBegin);
		aStream << " ";
		if (!(++_i % NCollums))
			aStream << "\n";
	}
	return aStream;
}
template<class T>
inline std::ostream& print_buffer(std::ostream& aStream, T aBegin, T aEnd)
{
  return print_buffer<T,8u>(aStream,aBegin,aEnd);
}

/*!\brief class is used for printing
 * data buffer to stream
 * 
 * For example
 *\code
 * std::cout<< print_buffer_t<char*>(_data_begin, _data_end) <<std:;endl;
 *\endcode
 * 
 *\tparam T iterator or pointer
 */ 
template<class T>
struct print_buffer_t
{
	const T& FBegin;
	const T& FEnd;
	
	/*!\brief printing data from aBegin to aEnd
	 * 
	 *\param aBegin buffer begin
	 *\param aEnd buffer end
	 */ 
	print_buffer_t(const T& aBegin, const T& aEnd) :
			FBegin(aBegin), FEnd(aEnd)
	{
	}
};
}

//specialize template from std namespace

namespace std
{
template<typename T>
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::print_buffer_t<T> const& aVal)
{
	NSHARE::print_buffer(aStream,aVal.FBegin,aVal.FEnd);
	return aStream;
}
template<typename T>
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::buf_val_t<T> const& aVal)
{
	NSHARE::detail::print_b_value<NSHARE::buf_val_t<T> >(aStream,aVal);
	return aStream;
}
template<class T>
struct numeric_limits<NSHARE::buf_val_t<T> >:numeric_limits<T>
{

};
}
#endif /* BUFFER_VALUE_H_ */
