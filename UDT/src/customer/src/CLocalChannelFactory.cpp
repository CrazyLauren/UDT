/*
 * CLocalChannelFactory.cpp
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <programm_id.h>
#include "CCustomer.h"
#include "CLocalChannelFactory.h"

template<>
NUDT::CLocalChannelFactory::singleton_pnt_t NUDT::CLocalChannelFactory::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
CLocalChannelFactory::CLocalChannelFactory()
{


}

CLocalChannelFactory::~CLocalChannelFactory()
{
}

}//
