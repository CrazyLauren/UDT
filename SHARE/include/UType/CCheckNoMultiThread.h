/*
 * CCheckNoMultiThread.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 14.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#ifndef CCHECKNOMULTITHREAD_H_
#define CCHECKNOMULTITHREAD_H_

namespace NSHARE
{
#ifndef NDEBUG
#define DCHECK_NO_MULTI_THREAD \
	static std::map<void*,unsigned> _s_check_no_multi_thread;\
	NSHARE::CCheckNoMultiThread __check_no_multi_thread(_s_check_no_multi_thread[this],__FILE__,__LINE__);
#else
#define DCHECK_NO_MULTI_THREAD
#endif

class CCheckNoMultiThread
{
public:
	CCheckNoMultiThread(unsigned& aThread, char const* aFile,
			const unsigned & aLine);
	~CCheckNoMultiThread();
private:
	unsigned& FThread;
	char const * FFile;
	unsigned const FLine;
};

} /* namespace NSHARE */
#endif /* CCHECKNOMULTITHREAD_H_ */
