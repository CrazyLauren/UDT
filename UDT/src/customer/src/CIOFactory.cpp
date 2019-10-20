// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIOFactory.cpp
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <udt/programm_id.h>
#include "udt/CCustomer.h"
#include "CIOFactory.h"

template<>
NUDT::CIOFactory::singleton_pnt_t NUDT::CIOFactory::singleton_t::sFSingleton = NULL;
namespace NUDT
{
CIOFactory::CIOFactory()
{

}

CIOFactory::~CIOFactory()
{
}

}//
