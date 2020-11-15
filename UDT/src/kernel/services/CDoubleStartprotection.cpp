/*
 * CDoubleStartprotection.cpp
 *
 * Copyright © 2020  https://github.com/CrazyLauren
 *
 *  Created on: 04.11.2020
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <core/CConfigure.h>

#include "CDoubleStartprotection.h"

template<>
NUDT::CDoubleStartprotection::singleton_pnt_t NUDT::CDoubleStartprotection::singleton_t::sFSingleton =
NULL;

namespace NUDT
{
using namespace NSHARE;
const NSHARE::CText CDoubleStartprotection::NAME = "start_protection";
NSHARE::CText const CDoubleStartprotection::PORT = "port";
CDoubleStartprotection::CDoubleStartprotection():
		ICore(NAME)//
{
}

CDoubleStartprotection::~CDoubleStartprotection()
{

}
bool CDoubleStartprotection::MStart()
{
	CConfig const* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);
	if (_p && _p->MIsChild(PORT))
	{
		unsigned _port = 18011;
		_p->MGetIfSet(PORT, _port);
		FProcess = NSHARE::intrusive_ptr<NSHARE::CSingletonProcess>(
				new NSHARE::CSingletonProcess(_port));
		if (!FProcess->MIs())
		{
			FProcess = NSHARE::intrusive_ptr<NSHARE::CSingletonProcess>();
			std::cout << "Port is busy!!!" << std::endl;
			LOG(ERROR) << "Double start protection!!!";
			return false;
		}
	}
	return true;
}
void CDoubleStartprotection::MStop()
{
	FProcess = NSHARE::intrusive_ptr<NSHARE::CSingletonProcess>();
}
NSHARE::CConfig CDoubleStartprotection::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	if(FProcess.MIs())
		_config.MAdd(PORT, FProcess->FPort);
	return _config;
}
} /* namespace NUDT */
