// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CConfigure.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 20.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <fstream>
#include "CConfigure.h"


template<>
NUDT::CConfigure::singleton_pnt_t NUDT::CConfigure::singleton_t::sFSingleton = NULL;
namespace NUDT
{
const NSHARE::CText CConfigure::NAME="conf";
CConfigure::CConfigure() :
		ICore(NAME),FConf(NAME)
{
	;
}
CConfigure::CConfigure(NSHARE::CText const& aPath, eType const& aType) :
		ICore(NAME),FConf(NAME),FPath(aPath)
{
	VLOG(2)<<"Read "<<aPath<<" type ="<<(int)aType;

	std::ifstream _stream;
	_stream.open(aPath.c_str());
	LOG_IF(ERROR,!_stream.is_open())<<"***ERROR***:Configuration file - "<<aPath<<".";
	if(_stream.is_open())
	{
		switch (aType)
		{
		case JSON:
			FConf.MFromJSON(_stream);
			break;
		case XML:
			FConf.MFromXML(_stream);
			break;
		default:
			LOG(DFATAL)<<"Unknown file format "<<aPath;
			break;
		}
		_stream.close();
	}
	VLOG(6)<<FConf.MToJSON(true);
}
bool CConfigure::MStart()
{
	return true;
}

NSHARE::CConfig & CConfigure::MGet()
{
	return FConf;
}
NSHARE::CText const& CConfigure::MGetPath() const
{
	return FPath;
}
NSHARE::CConfig CConfigure::MSerialize() const
{
	return FConf;
}
}//
