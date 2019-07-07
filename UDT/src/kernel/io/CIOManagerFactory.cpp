/*
 * CIOManagerFactory.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 07.07.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <io/CIOManagerFactory.h>

template<>
NUDT::CIOManagerFactory::singleton_pnt_t NUDT::CIOManagerFactory::singleton_t::sFSingleton =
NULL;
namespace NUDT
{
using namespace NSHARE;
CIOManagerFactory::CIOManagerFactory()
{
	;
}
CIOManagerFactory::~CIOManagerFactory()
{
	;
}
}
