// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CConfigure.cpp
 *
 *  Created on: 20.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "CConfigure.h"

template<>
 NUDT::CConfigure::singleton_pnt_t NUDT::CConfigure::singleton_t::sFSingleton = NULL;
namespace NUDT
{
CConfigure::CConfigure()
{
	;
}
CConfigure::CConfigure(NSHARE::CText const& aPath, eType const& aType) :
		FPath(aPath)
{
	//read config
}


NSHARE::CConfig & CConfigure::MGet()
{
	return FConf;
}
NSHARE::CText const& CConfigure::MGetPath() const
{
	return FPath;
}
}//
