/*
 * CCodeConv.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 20.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#ifndef CCODECONV_H_
#define CCODECONV_H_

#include <exception>
namespace NSHARE
{
class SHARE_EXPORT ICodeConv
{
public:
	typedef char char_t;
	typedef char_t const* it_char_t;
	class SHARE_EXPORT CEInvalidChar//: public std::exception
	{
		char_t FVal;
	public:
		CEInvalidChar(char_t u) :
				FVal(u)
		{
		}
		 virtual const char* what() const throw()
		{
			/*std::stringstream _buf;
			_buf << "Invalid Char code=" << static_cast<int>(FVal) << " , Char:"
					<< static_cast<char>(FVal);
			return _buf.str().c_str();*/
			return "Invalid Char code";
		}
	};
	virtual ~ICodeConv()
	{
		;
	}

	//return next encoded to utf32 char, aBegin will been changed
	virtual uint32_t MNext(it_char_t& aBegin, it_char_t aEnd) const
			throw (CEInvalidChar) =0;

	//return ==aBegin - if buffer is small
	//		 ==next buffer element
	virtual size_t MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const
			throw (CEInvalidChar) =0;

	virtual bool MIsCodeValid(uint32_t aCode) const =0;
	virtual bool MIsBufValid(it_char_t aBegin, it_char_t aEnd) const =0;

	//return the size of buffer required to encode the given utf32 code point
	virtual size_t MSizeOf(uint32_t const* aBegin, uint32_t const* aEnd) const
			throw (CEInvalidChar)
	{
		size_t _size = MCharLen(*aBegin);
		for (; ++aBegin != aEnd; _size += MCharLen(*aBegin))
			;
		return _size;
	}
	//return number of utf32 code  required to re-encode buffer
	virtual size_t MLengthOf(it_char_t aBegin) const
			throw (CEInvalidChar) =0;

	//return the size of buffer required to encode the given utf32 code point
	virtual size_t MCharLen(uint32_t aCode) const =0;
	virtual std::ostream& MPrint(std::ostream& aStream) const
	{
		return aStream << "Unknown type";
	}
};
class SHARE_EXPORT CCodeUTF8: public ICodeConv
{
public:
	typedef uint8_t utf8_t;
	typedef utf8_t const* it_utf8_t;//utf8 value > 128 - need use unsigned value
	virtual size_t MSeqLen(uint32_t aCode) const
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
		
		return _size;
	}
	//return number of utf32 code  required to re-encode buffer
	virtual size_t MLengthOf(it_char_t aBegin) const
			throw (CEInvalidChar)
	{
		size_t _length = 0;
		for (; *aBegin ;)
		{
			size_t _len = MSeqLen(*aBegin);
			if (!_len)
				throw CEInvalidChar(*aBegin);

			aBegin += _len;
			++_length;
		}
		return _length;
	}
	size_t MCharLenInline(uint32_t aCode) const
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
	virtual size_t MCharLen(uint32_t aCode) const
	{
		return MCharLenInline(aCode);
	}
	virtual uint32_t MNext(it_char_t& aBegin, it_char_t aEnd) const
			throw (CEInvalidChar)
	{
		it_utf8_t _putf8=reinterpret_cast<it_utf8_t>(aBegin);
		uint32_t _code = 0;
		size_t  _len = MSeqLen(*_putf8);
		if (!_len)
			throw CEInvalidChar(*_putf8);
		size_t const _remain = aEnd-aBegin;
		if(_len>_remain)
		{
			//LOG(DFATAL)<<"Invaild charaster code.";
			_len=_remain;
		}

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
		}
		aBegin=reinterpret_cast<it_char_t>(_putf8);
		return _code;
	}
	virtual size_t MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const
			throw (CEInvalidChar)
	{
		if (MCharLenInline(aCode) > static_cast<size_t>(aEnd-aBegin))
			return 0;
		char_t* _befor=aBegin;
		aBegin=reinterpret_cast<char_t*>(sMAppend(aCode,reinterpret_cast<utf8_t*>(aBegin)));
		return aBegin-_befor;
	}
	static utf8_t* sMAppend(uint32_t aCode,utf8_t* _putf8)
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
	virtual bool MIsCodeValid(uint32_t aCode) const
	{
		return (aCode <= 0x0010ffffu
				&& !(aCode >= 0xd800u && aCode <= 0xdfffu));
	}
	virtual bool MIsBufValid(it_char_t aBegin, it_char_t aEnd) const
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
	virtual std::ostream& MPrint(std::ostream& aStream)const
	{
		return aStream << "UTF8";
	}
private:
	template<typename TVal>
	utf8_t MMask8(TVal aVal) const
	{
		return static_cast<utf8_t>(0xff & aVal);
	}
};
class SHARE_EXPORT CCodeANSII: public ICodeConv
{
public:
	//return number of utf32 code  required to re-encode buffer
	virtual size_t MLengthOf(it_char_t aBegin) const
			throw (CEInvalidChar)
	{
		return strlen(aBegin);
	}
	virtual size_t MCharLen(uint32_t aCode) const
	{
		return 1;
	}
	virtual uint32_t MNext(it_char_t& aBegin, it_char_t aEnd) const
			throw (CEInvalidChar)
	{
		if (!::iscntrl(*aBegin)||!::isprint(*aBegin))
			throw CEInvalidChar(*aBegin);

		return *aBegin++;
	}
	virtual size_t MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const
			throw (CEInvalidChar)
	{
		if (aCode>=0x80)
			throw CEInvalidChar(aCode);

		*(aBegin++) = static_cast<char_t>(aCode);
		return 1;
	}
	virtual bool MIsCodeValid(uint32_t aCode) const
	{
		return (aCode <= 0x0010ffffu
				&& !(aCode >= 0xd800u && aCode <= 0xdfffu));
	}
	virtual bool MIsBufValid(it_char_t aBegin, it_char_t aEnd) const
	{
		for (; aBegin != aEnd;aBegin++)
			if (static_cast<unsigned>(*aBegin)>=0x80)
				return false;
		return true;
	}
	virtual std::ostream& MPrint(std::ostream& aStream) const
	{
		return aStream << "ANSII only";
	}
};

} /* namespace NSHARE */
namespace std
{
inline std::ostream& operator<<(std::ostream& aStream,NSHARE::ICodeConv const& aVal)
{
	aVal.MPrint(aStream);
	return aStream;
};
};
#endif /* CCODECONV_H_ */
