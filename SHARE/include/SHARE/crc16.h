/*
 * crc16.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.09.2012
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRC16_H_
#define CRC16_H_
#ifndef SHARE_CRC16_DEFINED
#	define SHARE_CRC16_DEFINED
#endif
#include <stdint.h>
#include <iostream>
#include <assert.h>
namespace NSHARE
{
#ifndef CHECK
#	define CHECK assert
#	define CUSTOM_CHECK
#endif
///@brief structure that helps to work with crc16
/// @tparam FPolynom crc8 polynomial
/// @tparam TOffset Initial value
/// @tparam TLsb Byte order
template<unsigned FPolynom, uint16_t TOffset = 0x0, bool FTLSB = false>
struct crc16_t
{
	typedef uint16_t type_t;
	enum
	{
		Polynom = FPolynom
	};
	enum
	{
		Offset = TOffset
	};
	enum
	{
		SizeTable = 256
	};
	enum
	{
		isLSB = FTLSB
	};
	struct table_t
	{
		table_t();
		type_t FCrc[SizeTable];
	};
	///@brief CRC Table
	///@return  CRC Table
	static type_t const* sMCRCTable();

	///@brief Print CRC Table
	inline static void sMPrintTable(std::ostream & aStream);

	///@brief Calculate buffer CRC
	///@tparam  T type of buffer pointer
	///@param  pBegin Buffer begin
	///@param  pEnd Buffer end
	///@param  aOffset Initial value
	template<class T>
	inline static type_t sMCalcCRCofBuf(/*register*/T const* pBegin,
			T const*  pEnd,
			/*register*/type_t aOffset = Offset,
			 unsigned const aOldCrc=std::numeric_limits<unsigned>::max()) //register has been deprecated since c++11
	{
		static table_t const _table;
		using namespace std;

		/*register*/size_t const _size = (pEnd - pBegin) * sizeof(T)
				/*/ sizeof(uint8_t)*/;

		uint8_t const* __restrict _begin = (uint8_t const*) (pBegin);
		for (size_t i = 0; i < _size; ++i)
		{
			if(aOldCrc != std::numeric_limits<unsigned>::max() && //
					(aOldCrc <= i || i < aOldCrc + sizeof(type_t))
					)
				continue;
			aOffset = (aOffset << 8)
					^ _table.FCrc[((aOffset >> 8) ^ _begin[i]) & 0xff]; //sMCalcNextCRC is not used for for optimization
		}
		return aOffset;
	}
	///@brief Adds the following byte to the CRC
	///@param aCRC8 Current  CRC
	///@param  aNextVal  The following byte
	///@return  new CRC
	inline static type_t sMCalcNextCRC(type_t const& aCRC16,
			uint8_t const& aNextVal)
	{
		return (aCRC16 << 8) ^ sMCRCTable()[((aCRC16 >> 8) ^ aNextVal) & 0xff];
	}
private:

	inline static void sMPrintHex(std::ostream & aStream, unsigned aVal)
	{

		aStream.setf(std::ios::hex, std::ios::basefield);
		aStream << "0x" << static_cast<unsigned const&>(aVal);
		aStream.unsetf(std::ios::hex);
	}
	template<typename TPolyType, bool _TLsb = true>
	struct make_t;
}
;
template<unsigned FPolynom, uint16_t TOffset, bool FTLSb>
template<typename TPolyType, bool _TLsb>
struct crc16_t<FPolynom, TOffset, FTLSb>::make_t
{
	///@brief Calculate CRCR table
	/// @param _crctab CRC Table
	inline static void sMakeCRCTable(type_t _crctab[SizeTable])
	{
		TPolyType _remaind = 1;

		_crctab[0] = 0;

		for (int i = (SizeTable >> 1); i; i >>= 1)
		{
			_remaind = (_remaind >> 1) ^ (_remaind & 1 ? Polynom : 0);
			for (int j = 0; j < SizeTable; j += 2 * i)
				_crctab[i + j] = _crctab[j] ^ _remaind;
		}

	}
};
template<unsigned FPolynom, uint16_t TOffset, bool FTLSb>
template<typename TPolyType>
struct crc16_t<FPolynom, TOffset, FTLSb>::make_t<TPolyType, false>
{
	typedef NSHARE::crc16_t<FPolynom, TOffset, FTLSb> base_t;
	inline static void sMakeCRCTable(
			typename base_t::type_t _crctab[base_t::SizeTable])
	{
		const TPolyType msbit = 0x8000;
		TPolyType _remaind = msbit;

		_crctab[0] = 0;

		for (int i = 1; i < base_t::SizeTable; i *= 2)
		{
			_remaind = (_remaind << 1)
					^ (_remaind & msbit ? base_t::Polynom : 0);
			for (int j = 0; j < i; j++)
				_crctab[i + j] = _crctab[j] ^ _remaind;
		}

	}
};
template<unsigned FPolynom, uint16_t TOffset, bool FTLSb>
inline crc16_t<FPolynom, TOffset, FTLSb>::table_t::table_t()
{
	make_t<unsigned, isLSB>::sMakeCRCTable(FCrc);
}
;
template<unsigned FPolynom, uint16_t TOffset, bool FTLSb>
inline typename crc16_t<FPolynom, TOffset, FTLSb>::type_t const* crc16_t<
		FPolynom, TOffset, FTLSb>::sMCRCTable()
{
	static table_t const _crc;
	return _crc.FCrc;

}
template<unsigned FPolynom, uint16_t TOffset, bool FTLSb>
inline void crc16_t<FPolynom, TOffset, FTLSb>::sMPrintTable(
		std::ostream & aStream)
{
	aStream << "CRC16 Polynomial ";
	sMPrintHex(aStream, FPolynom);
	aStream << std::endl;

	for (size_t i = 0; i < SizeTable; i++)
	{
		sMPrintHex(aStream, sMCRCTable()[i]);
		if (!((i + 1) % 4))
			aStream << std::endl;
		else
			aStream << "\t";
	}
	aStream << std::endl;
}
}
namespace std
{
template<unsigned FPolynom, uint16_t TOffset, bool FTLSb>
inline std::ostream& operator<<(std::ostream & aStream,
		struct NSHARE::crc16_t<FPolynom, TOffset, FTLSb> const& aCrc)
{
	aCrc.sMPrintTable(aStream);
	return aStream;
}
}
#ifdef CUSTOM_CHECK
#	undef  CUSTOM_CHECK
#	undef  CHECK
#endif
#endif /* CRC16_H_ */
