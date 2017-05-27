/*
 * uuid.cpp
 *
 * Copyright © 2017  https://github.com/CrazyLauren
 *
 *  Created on: 16.01.2017
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>

namespace NSHARE
{
	static int g_counter = 0;
	extern size_t get_unique_name(char const* aPreifix, uint8_t* aBuf, size_t aSize)
	{
		NSHARE::CText _rand;
		_rand.MMakeRandom(10);
		NSHARE::CText _mutex_name;
		if (aPreifix)
			_mutex_name.MPrintf("%s_%d_%s_%d", aPreifix, NSHARE::CThread::sMPid(), _rand.c_str(), ++g_counter);
		else
			_mutex_name.MPrintf("%d_%s_%d", NSHARE::CThread::sMPid(), _rand.c_str(), ++g_counter);

		size_t _name_len = (_mutex_name.length_code());
		_name_len =
			_name_len <= (aSize - 1) ?
			_name_len : (aSize - 1);
		memcpy(aBuf, _mutex_name.c_str(), _name_len);
		aBuf[_name_len] = '\0';
		return _name_len;
	}
}


