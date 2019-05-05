// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * IState.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "IState.h"
#include "CCore.h"

namespace NUDT
{
IState::IState(NSHARE::CText const& aName)
{
	CCore::sMGetInstance().MAddState(this,aName);
}
IState::~IState()
{
	CCore::sMGetInstance().MRemoveState(this);
}
NSHARE::CConfig IState::MSerializeRef() const
{
	NSHARE::CConfig _ref(CCore::REFFRENCE_TO,this);
	_ref.MValue().insert(0,CCore::REFFRENCE_PREFIX);
	return _ref;
}
}

