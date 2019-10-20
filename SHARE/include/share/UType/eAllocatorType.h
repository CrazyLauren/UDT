/*
 * eAllocatorType.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 20.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef EALLOCATERTYPE_H_
#define EALLOCATERTYPE_H_


namespace NSHARE
{
/**\brief Where has to be allocated memory
 *
 */
enum eAllocatorType
{
	ALLOCATE_FROM_COMMON,///< Memory has to be allocated only on common part of memory
	ALLOCATE_FROM_COMMON_AND_RESERV/*!< If possibly Memory has to be allocated on common part
										otherwise in reserved part of memory*/
};
}

#endif /* EALLOCATERTYPE_H_ */
