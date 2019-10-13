// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * IModule.cpp
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
#include <IModule.h>
#include <CCustomerImpl.h>

namespace NUDT
{
/** Store info about new module
 *
 * @param type A name of module
 */
IModule::IModule(const NSHARE::CText& type) :
		NSHARE::IFactory(type)
{
	CCustomer::sMGetInstance().MGetImpl().MPutModule(this);
}

/** Remove info about the module
 *
 */
IModule::~IModule()
{
	CCustomer::sMGetInstance().MGetImpl().MPopModule(this);
}
}
