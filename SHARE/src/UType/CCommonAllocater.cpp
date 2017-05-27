/*
 * CCommonAllocater.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 14.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <UType/CCommonAllocater.h>
namespace NSHARE
{
const IAllocater::offset_pointer_t IAllocater::NULL_OFFSET =
		std::numeric_limits<IAllocater::offset_pointer_t>::max(); //held here by historical reason

extern IAllocater* get_default_allocator()
{
	return get_default_allocator<CCommonAllocater<> >();
}

namespace detail
{
extern CMutex&  get_common_allocator_mutex()
{
	static CMutex _mutex;
	return _mutex;
}
extern IAllocater* get_default_allocator_common_allocate()
{
	static CCommonAllocater<> g_common_allocater;
	return &g_common_allocater;
}
}
}

