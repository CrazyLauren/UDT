/*
 * CIOFactory.cpp
 *
 *  Created on: 19.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <programm_id.h>
#include "CCustomer.h"
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