// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CConnectionHandlerFactory.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 18.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <core/kernel_type.h>
#include "CConnectionHandlerFactory.h"

template<>
NUDT::CConnectionHandlerFactory::singleton_pnt_t
NUDT::CConnectionHandlerFactory::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{

CConnectionHandlerFactory::CConnectionHandlerFactory()
{

}

CConnectionHandlerFactory::~CConnectionHandlerFactory()
{
}

} /* namespace NUDT */
