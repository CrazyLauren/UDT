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
#include "CDiagnostic.h"

namespace NUDT
{
IState::IState(NSHARE::CText const& aName)
{
	CDiagnostic::sMGetInstance().MAddState(this,aName);
}
IState::~IState()
{
	CDiagnostic::sMGetInstance().MRemoveState(this);
}
NSHARE::CConfig IState::MSerializeRef() const
{
	NSHARE::CConfig _ref(CDiagnostic::REFFRENCE_TO,this);
	_ref.MValue().insert(0,CDiagnostic::REFFRENCE_PREFIX);
	return _ref;
}
}

