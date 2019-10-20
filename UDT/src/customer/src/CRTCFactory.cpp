// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CRTCFactory.cpp
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <udt/programm_id.h>
#include "CRTCFactory.h"

template<>
NUDT::CRTCFactory::singleton_pnt_t NUDT::CRTCFactory::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
CRTCFactory::CRTCFactory()
{


}

CRTCFactory::~CRTCFactory()
{
}

}//
