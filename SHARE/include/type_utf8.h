/*
 *
 * type_utf8.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef NTYPE_UTF8_H_
#define NTYPE_UTF8_H_

/*
#ifndef CP_UTF8
#define CP_UTF8 65001
int WideCharToMultiByte(UINT,DWORD,LPCWSTR,int,LPSTR,int,LPCSTR,LPBOOL);
#endif
*/
//#include <windows.h>
namespace NSHARE //TODO
{
#if defined(_UNICODE)
#ifndef _T
#	define _T(x) L ##x
#endif
typedef std::wstring tstring;
typedef std::wostringstream tostringstream;
typedef std::wstringstream tstringstream;
typedef std::wistringstream tistringstream;

#else
#ifndef _T
#	define _T(x) x
#endif
typedef std::string tstring;
typedef std::ostringstream tostringstream;
typedef std::stringstream tstringstream;
typedef std::istringstream tistringstream;
#endif

#ifdef _WIN32
inline std::string utf8_encode(const std::wstring &wstr)
{

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0],
			(int) wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int) wstr.size(), &strTo[0],
			size_needed, NULL, NULL);
	return strTo;
}
#else
inline std::string utf8_encode(const std::wstring &wstr)
{
	char buffer[128];
	std::string _str;
	std::wstring::const_iterator _it = wstr.begin();
	for( int _length;_it!=wstr.end();++_it)
	{
		_length = wctomb(buffer,*_it);
		for(int i=0;i<_length;++i)
		_str.push_back(buffer[_length]);
	}
	return _str;
}

#endif
}//NSHARE
#endif /* NTYPE_UTF8_H_ */
