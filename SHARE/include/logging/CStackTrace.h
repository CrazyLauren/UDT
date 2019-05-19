/*
 *
 * CStackTrace.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */


#ifndef CSTACK_TRACE_H
#define CSTACK_TRACE_H

#include <macro_attributes.h>

#if _WIN32_WINNT > 0x0501 && _MSC_VER
#	define HAVE_CAPTURESTACKBACKTRACE
#endif

namespace NSHARE
{

/** @brief A trace stack of function
 *
 */
class SHARE_EXPORT CStackTrace
{
public:

	/*!@brief Reads current stack
	 *
	 */
	CStackTrace();

	/** @brief Prints a stack trace to the stream.
	 *
	 * @param aStream The stream.
	 * @param aIgnoreFrames The number of stack frames to ignore
	 *
	 */
	void MPrint(std::ostream& aStream, int aIgnoreFrames = 0) const;

	/*! @brief Transformation  C++ ABI identifier into original C++ source  identifier
	 *
	 * @param aName A Abi function name
	 * @return A human-clear function name
	 */
	CText MDemangle(CText const& aName) const;

private:
	enum
	{

#ifdef BACKTRACE_FUNCTION_AVAILABLE
		MAX_NUMBER_OF_FRAMES=100,///< A max number of frames to capture
#elif defined (HAVE_CAPTURESTACKBACKTRACE)
		MAX_NUMBER_OF_FRAMES=62,///< A max number of frames to capture (For XP Max 62)

#else
		MAX_NUMBER_OF_FRAMES=64
#endif
	};

	void *FBuffer[MAX_NUMBER_OF_FRAMES];///< Size of frame buffer
	int FStackSize;///< A number of called function
};

}
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream, const NSHARE::CStackTrace& aBase)
{
	aBase.MPrint(aStream,1 /*this call*/);
	return aStream;
}
}
#endif /* CSTACK_TRACE_H */
