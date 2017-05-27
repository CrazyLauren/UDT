/*
 * console.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 14.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
 
#ifndef CONSOLE_H_
#define CONSOLE_H_
#include <iomanip>

namespace NSHARE
{
namespace NCONSOLE

{
SHARE_EXPORT void clear();
enum eCode
{
	eNORMAL = 0,
	eBOLD = 1,
	eUNDERLINE = 4,
	eFLASHING = 5,
	eINVERTED = 7,
	eINVISIBLE = 8,

	eFG_RED = 31,
	eFG_GREEN = 32,
	eFG_YELLOW = 33,
	eFG_BLUE = 34,
	eFG_DEFAULT = 39,

	eBG_RED = 41,
	eBG_GREEN = 42,
	eBG_BLUE = 44,
	eBG_DEFAULT = 49
};
class CModifier
{
	eCode const& FCode;
public:
	explicit CModifier(eCode const& aCode) :
			FCode(aCode)
	{
	}
	friend inline std::ostream&
	operator<<(std::ostream& aStream, const CModifier& aMod)
	{
		return aStream << "\x1b[" << (int)aMod.FCode << "m";
	}
};
inline  NSHARE::CText center(size_t const aWidth, const NSHARE::CText& aStr)
{
	if (aWidth < aStr.length())
		return aStr;

	size_t const _diff = aWidth - aStr.length();
	return NSHARE::CText(_diff / 2, ' ') + aStr;
}
template<size_t aWidth, class T>
class table_column
{
public:
	typedef table_column<aWidth, T> Column;
	explicit table_column(const T& aStr, bool aCenter = true) :
			FStr(aStr), FCenter(aCenter)
	{
		;
	}
	const T& FStr;
	bool const FCenter;
};
SHARE_EXPORT std::ostream&print_color(std::ostream& aStream,
		const NSHARE::NCONSOLE::eCode& aCode);
} //namespace NConsole
}
namespace std
{
inline std::ostream&operator<<(std::ostream& aStream,
		const NSHARE::NCONSOLE::eCode& aCode)
{
	return NSHARE::NCONSOLE::print_color(aStream, aCode);
}
template<size_t aWidth, class T> inline
std::ostream& operator<<(std::ostream& aStream,
		const NSHARE::NCONSOLE::table_column<aWidth, T>& aColumn)
{
	std::streamsize _width = aStream.width();
	aStream << std::setw(aWidth) << std::left;
	if (aColumn.FCenter)
		aStream << NSHARE::NCONSOLE::center(aWidth, aColumn.FStr);
	else
		aStream << aColumn.FStr;
	return aStream << std::setw(_width);
}

}

#endif /* CONSOLE_H_ */
