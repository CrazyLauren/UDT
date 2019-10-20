/*
 * CCodeConv.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 20.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#ifndef CCODECONV_H_
#define CCODECONV_H_

#include <exception>
#include <limits>
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
	virtual uint32_t MNext(it_char_t& aBegin, it_char_t aEnd) const =0;

	//return ==aBegin - if buffer is small
	//		 ==next buffer element
	virtual size_t MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const =0;

	virtual bool MIsCodeValid(uint32_t aCode) const =0;
	virtual bool MIsBufValid(it_char_t aBegin, it_char_t aEnd) const =0;

	//return the size of buffer required to encode the given utf32 code point
	virtual size_t MSizeOf(uint32_t const* aBegin, uint32_t const* aEnd) const;
	//return number of utf32 code  required to re-encode buffer
	virtual size_t MLengthOf(it_char_t aBegin, size_t aMax=std::numeric_limits<size_t>::max()) const =0;

	//return the size of buffer required to encode the given utf32 code point
	virtual size_t MCharLen(uint32_t aCode) const =0;
	virtual std::ostream& MPrint(std::ostream& aStream) const;
};
class SHARE_EXPORT CCodeUTF8: public ICodeConv
{
public:
	typedef uint8_t utf8_t;
	typedef utf8_t const* it_utf8_t;//utf8 value > 128 - need use unsigned value
	virtual size_t MSeqLen(uint32_t aCode) const;
	//return number of utf32 code  required to re-encode buffer
	virtual size_t MLengthOf(it_char_t aBegin,
									size_t aMax=std::numeric_limits<size_t>::max()) const;
	size_t MCharLenInline(uint32_t aCode) const;
	virtual size_t MCharLen(uint32_t aCode) const;
	virtual uint32_t MNext(it_char_t& aBegin, it_char_t aEnd) const;
	virtual size_t MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const;
	static utf8_t* sMAppend(uint32_t aCode,utf8_t* _putf8);
	virtual bool MIsCodeValid(uint32_t aCode) const;
	virtual bool MIsBufValid(it_char_t aBegin, it_char_t aEnd) const;
	virtual std::ostream& MPrint(std::ostream& aStream)const;
private:
	template<typename TVal>
	utf8_t MMask8(TVal aVal) const;
};
class SHARE_EXPORT CCodeANSII: public ICodeConv
{
public:
	//return number of utf32 code  required to re-encode buffer
	virtual size_t MLengthOf(it_char_t aBegin,
					size_t aMax=std::numeric_limits<size_t>::max()) const;
	virtual size_t MCharLen(uint32_t aCode) const;
	virtual uint32_t MNext(it_char_t& aBegin, it_char_t aEnd) const;
	virtual size_t MAppend(uint32_t aCode, char_t*& aBegin, char_t* aEnd) const;
	virtual bool MIsCodeValid(uint32_t aCode) const;
	virtual bool MIsBufValid(it_char_t aBegin, it_char_t aEnd) const;
	virtual std::ostream& MPrint(std::ostream& aStream) const;
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
