// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CStackTrace.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 26.05.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/logging/CStackTrace.h>

#ifdef HAVE_DBGHELP
#	include <dbghelp.h>
#endif

#ifdef HAVE_BACKTRACE_SYMBOLS
#	include <execinfo.h>
#endif /* HAVE_BACKTRACE_SYMBOLS */

#ifdef HAVE_CXXABI_H
#	include <cxxabi.h>
#endif


/*#ifdef _MSC_VER
#	pragma optimize("", off)
#endif 
*/

namespace NSHARE
{

CStackTrace::CStackTrace()
{
	memset(FBuffer,0,sizeof(FBuffer));
#ifdef HAVE_BACKTRACE_SYMBOLS
	FStackSize = backtrace(FBuffer, MAX_NUMBER_OF_FRAMES);
#elif defined(HAVE_CAPTURESTACKBACKTRACE)
	FStackSize = CaptureStackBackTrace(0, MAX_NUMBER_OF_FRAMES, FBuffer, NULL);
#else
	FStackSize = 0;
#endif
}

/*#ifdef _MSC_VER
#	pragma optimize("", on)
#endif 
*/

CText  CStackTrace::MDemangle(CText const& aName) const
{
	CText sym_demangled=aName;
#ifdef HAVE_CXXABI_H
	int status;
	char *realname = abi::__cxa_demangle(aName.c_str(), 0, 0, &status);

	if (realname != NULL)
	{
		sym_demangled = CText(realname);
		free(realname);
	}
#endif
	return sym_demangled;
}
void CStackTrace::MPrint(std::ostream& aStream, int aIgnoreFrames) const
{
	using namespace std;
	aStream << std::endl;

#ifdef HAVE_BACKTRACE_SYMBOLS
	char **messages = backtrace_symbols(FBuffer, FStackSize);

	for (int i = aIgnoreFrames + 1/*skip current frame*/; i < FStackSize && messages != NULL; ++i)
	{
		CText message = messages[i];

		char *sym_begin = strchr(messages[i], '(');
		char const *_nill="nil";

		if (sym_begin && strncmp(sym_begin+1,_nill,min(strlen(sym_begin+1),strlen(_nill)))!=0)
		{
			char *sym_end = strchr(sym_begin, '+');

			if (sym_end != NULL)
			{
				char * _first_sym=sym_begin + 1;

				CText sym = CText(_first_sym, sym_end-_first_sym);

				CText const sym_demangled(MDemangle(sym));

				CText path = CText(messages[i], sym_begin-messages[i]);

				size_t const slashp = path.rfind("/");

				if (slashp != CText::npos)
					path = path.substr(slashp + 1);

				message = path + ": " + sym_demangled + " (" + CText((utf8*)sym_end);
			}
		}

		aStream << "\t(" << i - aIgnoreFrames - 1 << ") " << message << std::endl;
	}

	std::free(messages);

	aStream << std::endl;
#elif defined(HAVE_DBGHELP)

	static LONG once = 0;
    if( !InterlockedExchange(&once, 1) )
        SymInitialize(GetCurrentProcess(), NULL, TRUE);

    const int len = 255; // just some reasonable string buffer size
	union
	{
    	SYMBOL_INFO sym;
    	char pad[sizeof(SYMBOL_INFO)+len];
	};
	sym.MaxNameLen = len;
	sym.SizeOfStruct = sizeof( SYMBOL_INFO );
	DWORD64 offset;
	for(int i = 1 /*skip current frame*/+aIgnoreFrames; i < FStackSize; ++i)
	{
		if(!SymFromAddr( GetCurrentProcess(), DWORD64(FBuffer[i]), &offset, &sym ))
		{
			sym.Address = ULONG64(FBuffer[i]); offset = 0; sym.Name[0] = 0;
		}
		aStream << "\t("<<std::dec << i - aIgnoreFrames - 1 << "): "
					<<"[%d"<<i<<"] 0x"<<std::hex<<sym.Address
					<<"+0x"<<std::hex<<offset
				   <<": "<<sym.Name  << std::endl;
		if(strcmp(sym.Name ,"main")==0)
			break;
	}
#else

	aStream << "(not available)" << std::endl;
#endif /* HAVE_BACKTRACE_SYMBOLS */
}
}
