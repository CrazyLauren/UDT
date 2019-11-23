/*
 * CTextStream.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTEXTSTREAM_H_
#define CTEXTSTREAM_H_

namespace NSHARE
{

inline CText::CTextBuf::CTextBuf(CText& aText) :
		FText(aText)
{
	FStr[0]='\0';
	FSize=0;
	FBufI=0;
}
inline int CText::CTextBuf::overflow(int c)
{
	if (c != traits_type::eof())
	{
		utf32 ch = static_cast<utf32>(c);
		if(FBufI!=0)
		{
			FStr[FBufI++]=(utf8)ch;
			if(FBufI==FSize)
			{
				FStr[FBufI]='\0';
				FText.append(FStr,1);

				FBufI=0;
				FSize=0;
			}
		}else if((FSize=FCode.MSeqLen(ch))>1)
		{
			FStr[FBufI++]=(utf8)ch;
		}else
			FText.push_back(ch);
		return c;
	}

	return traits_type::eof();
}

inline CText::CTextStream::CTextStream(CText& aText) : //
		std::ostream(&msb), //
		std::ios(0), //
		FText(aText),//
		msb(aText)
{

}
inline CText::CTextStream::~CTextStream()
{
	msb.pubsync();
}

template<typename T>
inline CText& CText::operator<<(const T& aVal)
{
	CTextStream& _stream(*FStream);
	_stream<<aVal;
	return *this;
}
template<>
inline CText& CText::operator<< <NSHARE::CText>(const NSHARE::CText& aVal)
{
	append(aVal);
	return *this;
}
}

#endif /* CTEXTSTREAM_H_ */
