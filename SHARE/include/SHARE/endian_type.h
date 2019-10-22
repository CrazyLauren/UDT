/*
 * endian_type.h
 *
 * Copyright © 2017  https://github.com/CrazyLauren
 *
 *  Created on: 19.03.2017
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef ENDIAN_TYPE_H_
#define ENDIAN_TYPE_H_

//\see http://www.boost.org/doc/libs/1_47_0/boost/detail/endian.hpp

#if !defined(SHARE_LITTLEENDIAN) && !defined(SHARE_BIGENDIAN)
#	ifdef __BYTE_ORDER__
#		if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  			define SHARE_LITTLEENDIAN
#		elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#			define SHARE_BIGENDIAN
#		else
#			error Unknown machine endianess detected. User needs to define SHARE_ENDIAN.
#		endif // __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN
#		define SHARE_BYTE_ORDER __BYTE_ORDER__
#	elif defined(__GLIBC__)
#		include <endian.h>
#		if (__BYTE_ORDER == __LITTLE_ENDIAN)
#  			define SHARE_LITTLEENDIAN
#		elif (__BYTE_ORDER == __BIG_ENDIAN)
#			define SHARE_BIGENDIAN
#		else
#			error Unknown machine endianess detected. User needs to define SHARE_ENDIAN.
#   	endif // if (__BYTE_ORDER == __LITTLE_ENDIAN)
# 		define SHARE_BYTE_ORDER __BYTE_ORDER
#  	elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN) || \
			defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__) || \
			defined(_STLP_LITTLE_ENDIAN) && !defined(_STLP_BIG_ENDIAN)		
#  		define SHARE_LITTLEENDIAN
#		define SHARE_BYTE_ORDER 1234
#	elif defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN) || \
			defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__) || \
			defined(_STLP_BIG_ENDIAN) && !defined(_STLP_LITTLE_ENDIAN)
#		define SHARE_BIGENDIAN
#  elif defined(__sparc) || defined(__sparc__) \
		|| defined(_POWER) || defined(__powerpc__)\
		|| defined(__ppc__)\
		|| defined(__hpux) || defined(__hppa)\
		|| defined(_MIPSEB) || defined(__MIPSEB__)\
		|| defined(__s390__)
#		define SHARE_BIGENDIAN
#	elif defined(__i386__) || defined(__alpha__)\
		|| defined(__ia64) || defined(__ia64__)\
		|| defined(_M_IX86) || defined(_M_IA64) \
		|| defined(_M_ALPHA) || defined(__amd64) \
		|| defined(__amd64__) || defined(_M_AMD64) \
		|| defined(__x86_64) || defined(__x86_64__) \
		|| defined(_M_X64) || defined(__bfin__)
# 		define SHARE_LITTLEENDIAN
#	elif defined(_MSC_VER) && defined(_M_ARM)
# 		define SHARE_LITTLEENDIAN
#  else
#    error Unknown machine endianess detected. User needs to define _LITTLE_ENDIAN or _BIG_ENDIAN.   
#  endif
#endif // #if !defined(SHARE_LITTLE_ENDIAN) && !defined(SHARE_BIGENDIAN)

#ifndef SHARE_BYTE_ORDER
#	ifdef SHARE_LITTLEENDIAN
#		define SHARE_BYTE_ORDER 1234
#	elif defined(SHARE_BIGENDIAN)
#		define SHARE_BYTE_ORDER 4321
#	else
#    	error Unknown byte order
#	endif
#endif//#ifndef SHARE_BYTE_ORDER

namespace NSHARE
{
/**\brief Platform endian
 */
enum eEndian
{
	E_SHARE_LITTLE_ENDIAN = 0,
	E_SHARE_BIG_ENDIAN = 1,
#ifdef SHARE_LITTLEENDIAN
	E_SHARE_ENDIAN = E_SHARE_LITTLE_ENDIAN,
	E_SHARE_OTHER_ENDIAN = E_SHARE_BIG_ENDIAN
#else
	E_SHARE_ENDIAN=E_SHARE_BIG_ENDIAN,
	E_SHARE_OTHER_ENDIAN = E_SHARE_LITTLE_ENDIAN
#endif
};

	/**\brief Convenient method for swapping byte endianness
	 *
	 *\param aVal - swapping value
	 *\note using only for standard type
	 */
	template<class T>
	inline T swap_endain(const T&aVal);
	/**\brief method of quick swapping byte endianness
	 *
	 *\{
	 */
	inline uint8_t swap_endian(uint8_t const& aVal){
		return aVal;
	}
	inline int8_t swap_endian(int8_t const& aVal){
		return aVal;
	}
	inline uint16_t swap_endian(uint16_t const& aVal){
		return (aVal<<8)|(aVal>>8);
	}
	inline int16_t swap_endian(int16_t const& aVal){
		return (aVal<<8)|((aVal>>8)&0xFF);
	}
	inline uint32_t swap_endian(uint32_t  aVal)
	{
		aVal=((aVal<<8)&0xFF00FF00)|((aVal>>8)&0xFF00FF);
		return (aVal<<16)|(aVal>>16);
	}
	inline int32_t swap_endian(int32_t  aVal)
	{
		aVal=((aVal<<8)&0xFF00FF00)|((aVal>>8)&0xFF00FF);
		return (aVal<<16)|((aVal>>16)&0xFFFF);
	}
	inline uint64_t swap_endian(uint64_t  aVal)
	{
		aVal=((aVal<<8)&0xFF00FF00FF00FF00ULL)|((aVal>>8)&0x00FF00FF00FF00FFULL);
		aVal=((aVal<<16)&0xFFFF0000FFFF0000ULL)|((aVal>>16)&0x0000FFFF0000FFFFULL);
		return (aVal<<32)|(aVal>>32);
	}
	inline int64_t swap_endian(int64_t  aVal)
	{
		aVal=((aVal<<8)&0xFF00FF00FF00FF00ULL)|((aVal>>8)&0x00FF00FF00FF00FFULL);
		aVal=((aVal<<16)&0xFFFF0000FFFF0000ULL)|((aVal>>16)&0x0000FFFF0000FFFFULL);
		return (aVal<<32)|((aVal>>32)&0xFFFFFFFFULL);
	}
	inline float swap_endian(float  aVal)
	{
		union
		{
			float f;
			uint32_t i;
		};
		i = swap_endian(*(uint32_t*)&aVal);
		return f;
	}
	inline double swap_endian(double  aVal)
	{
		union
		{
			double f;
			uint64_t i;
		};
		i = swap_endian(*(uint64_t*)&aVal);
		return f;
	}
	//\}

	namespace _impl
	{
		template<unsigned aNumberBytes>
		struct endian_wrapper_t
		{

		};
		template<class T>
		inline T swap_byte_endain(const T& aVal,endian_wrapper_t<1> const&)
		{
			uint8_t const _val=swap_endian(*(uint8_t const*)&aVal);
			return *(T const*)&_val;
		}
		template<class T>
		inline T swap_byte_endain(const T& aVal,endian_wrapper_t<2> const&)
		{
			uint16_t const _val = swap_endian(*(uint16_t const*)&aVal);
			return *(T const*)&_val;
		}
		template<class T>
		inline T swap_byte_endain(const T& aVal,endian_wrapper_t<4> const&)
		{
			uint32_t const _val=swap_endian(*(uint32_t const*)&aVal);
			return *(T const*)&_val;
		}
		template<class T>
		inline T swap_byte_endain(const T& aVal,endian_wrapper_t<8> const&)
		{
			uint64_t const _val=swap_endian(*(uint64_t const*)&aVal);
			return *(T const*)&_val;
		}
	}
	template<class T>
	inline T swap_endain(const T&aVal)
	{
		return _impl::swap_byte_endain<T>(aVal,_impl::endian_wrapper_t<sizeof(T)>());
	}

}
namespace std{
inline std::ostream& operator<<(std::ostream & aStream, NSHARE::eEndian const& aVal)
{
	return aStream<<static_cast<unsigned>(aVal);
}
} //namespace std
#endif /* ENDIAN_TYPE_H_ */
