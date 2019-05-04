/*
 * sm_shared.h
 *
 *  Created on: 07.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SM_SHARED_H_
#define SM_SHARED_H_
namespace NUDT
{
enum eDataType
{
	E_SM_INFO=1,
	E_SM_DATA,
	E_SM_DATA_INFO,
};
struct flag_mask_t
{
	union
	{
		struct
		{
			uint32_t FCounter :30;
			uint32_t FType :2; //eDataType
		} FData;
		uint32_t FMask;
	};
};

}
#endif /* SM_SHARED_H_ */
