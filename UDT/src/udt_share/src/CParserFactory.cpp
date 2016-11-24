/*
 * CParserFactory.cpp
 *
 *  Created on: 21.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <programm_id.h>
#include <time.h>
#include <internel_protocol.h>
#include <shared_types.h>
#include <udt_share.h>
#include <CParserFactory.h>

template<>
NSHARE::CFactoryManager<NUDT::IExtParser>::singleton_pnt_t NSHARE::CSingleton<
		NSHARE::CFactoryManager<NUDT::IExtParser> >::sFSingleton = NULL;

