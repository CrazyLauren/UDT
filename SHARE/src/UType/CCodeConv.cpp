// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CCodeConv.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <deftype>
#include <SHARE/UType/CText.h>

namespace NSHARE
{
size_t ICodeConv::MSizeOf(uint32_t const* aBegin, uint32_t const* aEnd) const
{
	size_t _size = MCharLen(*aBegin);
	for (; ++aBegin != aEnd; _size += MCharLen(*aBegin))
		;
		return _size;
}
std::ostream& ICodeConv::MPrint(std::ostream& aStream) const
{
	return aStream << "Unknown type";
}

size_t CCodeUTF8::MSeqLen(uint32_t aCode) const
	{
		utf8_t lead = MMask8(aCode);
		size_t _size=0;
		if (lead < 0x80)
			_size= 1;
		else if ((lead >> 5) == 0x6)
			_size= 2;
		else if ((lead >> 4) == 0xe)
			_size= 3;
		else if ((lead >> 3) == 0x1e)
			_size= 4;
		else
		{
			LOG(ERROR)<<"Invalid utf8 char code:"<<aCode;
			_size=1;
		}	
		
		return _size;
	}
	//return number of utf32 code  required to re-encode buffer
	 size_t CCodeUTF8::MLengthOf(it_char_t aBegin,size_t aMax) const
	{
		size_t _length = 0;
		for (; *aBegin //
				&& aMax!=0;--aMax)
		{
				
			uint32_t const _code=/*static_cast<uint32_t>(
					 static_cast<uint8_t>(_str_size>=0?*(aBegin+0):0)<<24//
					|static_cast<uint8_t>(_str_size>=1?*(aBegin+1):0)<<16//
					|static_cast<uint8_t>(_str_size>=2?*(aBegin+2):0)<<8//
					|static_cast<uint8_t>(_str_size>=3?*(aBegin+3):0)<<0//
			);*/*aBegin;			
			size_t _len = MSeqLen(_code);
			aBegin += _len;
			++_length;
		}
		return _length;
	}
	size_t CCodeUTF8::MCharLenInline(uint32_t aCode) const
	{
		size_t _size=4;
		if (aCode < 0x80)
			_size=1;
		else if (aCode < 0x0800)
			_size=2;
		else if (aCode < 0x10000)
			_size=3;
		else
			_size=4;

		return _size;
	}
	 size_t CCodeUTF8::MCharLen(uint32_t aCode) const
	{
		return MCharLenInline(aCode);
	}
	 uint32_t CCodeUTF8::MNext(it_char_t& aBegin, it_char_t aEnd) const
	{
		it_utf8_t _putf8=reinterpret_cast<it_utf8_t>(aBegin);
		uint32_t _code = 0;
		size_t  _len = MSeqLen(*_putf8);
		if (!_len)			
			{
				
				LOG(DFATAL)<<"Invalid utf8 char:"<<*_putf8;

#ifdef USE_EXCEPTION
				throw CEInvalidChar(*_putf8);
#endif										
			}
		size_t const _remain = aEnd-aBegin;
		if(_len>_remain)
		{
			//LOG(DFATAL)<<"Invaild charaster code.";
			_len=_remain;
		}

		switch (_len)
		{
		case 0:
		case 1:
			_code = *_putf8++;
			break;
		case 2:
			_code = (*_putf8++ << 6) & 0x7ff;
			_code += (*_putf8++) & 0x3f;
			break;
		case 3:
			_code = (*_putf8++ << 12) & 0xffff;
			_code += (*_putf8++ << 6) & 0xfff;
			_code += (*_putf8++) & 0x3f;
			break;
		case 4:
			_code = (*_putf8++ << 18) & 0x1fffff;
			_code += (*_putf8++ << 12) & 0x3ffff;
			_code += (*_putf8++ << 6) & 0xfff;
			_code += (*_putf8++) & 0x3f;
			break;
		}
		aBegin=reinterpret_cast<it_char_t>(_putf8);
		return _code;
	}
	 size_t CCodeUTF8::MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const
	{
		if (MCharLenInline(aCode) > static_cast<size_t>(aEnd-aBegin))
			return 0;
		char_t* _befor=aBegin;
		aBegin=reinterpret_cast<char_t*>(sMAppend(aCode,reinterpret_cast<utf8_t*>(aBegin)));
		return aBegin-_befor;
	}
	 CCodeUTF8::utf8_t* CCodeUTF8::sMAppend(uint32_t aCode,utf8_t* _putf8)
	{
		if (aCode < 0x80)
		{
			*(_putf8++) = static_cast<utf8_t>(aCode);
		}
		else if (aCode < 0x0800)
		{
			*(_putf8++) = static_cast<utf8_t>((aCode >> 6) | 0xc0);
			*(_putf8++) = static_cast<utf8_t>((aCode & 0x3f) | 0x80);
		}
		else if (aCode < 0x10000)
		{
			*(_putf8++) = static_cast<utf8_t>((aCode >> 12) | 0xe0);
			*(_putf8++) = static_cast<utf8_t>(((aCode >> 6) & 0x3f) | 0x80);
			*(_putf8++) = static_cast<utf8_t>((aCode & 0x3f) | 0x80);
		}
		else
		{
			*(_putf8++) = static_cast<utf8_t>((aCode >> 18) | 0xf0);
			*(_putf8++) = static_cast<utf8_t>(((aCode >> 12) & 0x3f) | 0x80);
			*(_putf8++) = static_cast<utf8_t>(((aCode >> 6) & 0x3f) | 0x80);
			*(_putf8++) = static_cast<utf8_t>((aCode & 0x3f) | 0x80);
		}
		return _putf8;
	}
	 bool CCodeUTF8::MIsCodeValid(uint32_t aCode) const
	{
		return (aCode <= 0x0010ffffu
				&& !(aCode >= 0xd800u && aCode <= 0xdfffu));
	}
	 bool CCodeUTF8::MIsBufValid(it_char_t aBegin, it_char_t aEnd) const
	{
		it_utf8_t _putf8=reinterpret_cast<it_utf8_t>(aBegin);
		it_utf8_t _putf8_end=reinterpret_cast<it_utf8_t>(aEnd);
		for (; _putf8 != _putf8_end;)
		{
			size_t _len = MSeqLen(*_putf8);
			uint32_t _code = 0;
			switch (_len)
			{
			case 1:
				_code = *_putf8++;
				break;
			case 2:
				_code = (*_putf8++ << 6) & 0x7ff;
				_code += (*_putf8++) & 0x3f;
				break;
			case 3:
				_code = (*_putf8++ << 12) & 0xffff;
				_code += (*_putf8++ << 6) & 0xfff;
				_code += (*_putf8++) & 0x3f;
				break;
			case 4:
				_code = (*_putf8++ << 18) & 0x1fffff;
				_code += (*_putf8++ << 12) & 0x3ffff;
				_code += (*_putf8++ << 6) & 0xfff;
				_code += (*_putf8++) & 0x3f;
				break;
			default:
				return false;
				break;
			}
			if(!MIsCodeValid(_code))
				return false;
		}

		return true;
	}
	 std::ostream& CCodeUTF8::MPrint(std::ostream& aStream)const
	{
		return aStream << "UTF8";
	}
	template<typename TVal>
	CCodeUTF8::utf8_t CCodeUTF8::MMask8(TVal aVal) const
	{
		return static_cast<utf8_t>(0xff & aVal);
	}
	
	
	//return number of utf32 code  required to re-encode buffer
	 size_t CCodeANSII::MLengthOf(it_char_t aBegin,size_t aMax) const
	{
		//return strnlen(aBegin,aMax);
		return strlen(aBegin);
	}
	 size_t CCodeANSII::MCharLen(uint32_t aCode) const
	{
		return 1;
	}
	 uint32_t CCodeANSII::MNext(it_char_t& aBegin, it_char_t aEnd) const
	{
		if (!::iscntrl(*aBegin)||!::isprint(*aBegin))
		{
			
			LOG(DFATAL)<<"Invalid utf8 char:"<<*aBegin;
#ifdef USE_EXCEPTION
				throw CEInvalidChar(*aBegin);
#endif		
		}	

		return *aBegin++;
	}
	 size_t CCodeANSII::MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const
	{
		if (aCode>=0x80)
		{				
			LOG(DFATAL)<<"Invalid utf8 char:"<<aCode;
#ifdef USE_EXCEPTION
			throw CEInvalidChar(aCode);
#endif	
		}
		*(aBegin++) = static_cast<char_t>(aCode);
		return 1;
	}
	 bool CCodeANSII::MIsCodeValid(uint32_t aCode) const
	{
		return (aCode <= 0x0010ffffu
				&& !(aCode >= 0xd800u && aCode <= 0xdfffu));
	}
	 bool CCodeANSII::MIsBufValid(it_char_t aBegin, it_char_t aEnd) const
	{
		for (; aBegin != aEnd;aBegin++)
			if (static_cast<unsigned>(*aBegin)>=0x80)
				return false;
		return true;
	}
	 std::ostream& CCodeANSII::MPrint(std::ostream& aStream) const
	{
		return aStream << "ANSII only";
	}
} // NSHARE

