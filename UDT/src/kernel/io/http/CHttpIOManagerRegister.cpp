/*
 * CHttpIOManagerRegister.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.05.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <udt_share.h>
#include <core/IState.h>
#include <core/kernel_type.h>
#include <core/CDataObject.h>
#include <core/CDescriptors.h>
#include <io/CKernelIo.h>

#include "CHttpIOManagerRegister.h"
#include "CHttpIOManger.h"

namespace NUDT
{
NSHARE::CText const CHttpIOManger::NAME = "http_gui";//Fucking windows!!! I don't know why if CHttpIOManger::NAME
//is definition in the other translation unit, It's value in this translation unit is null. May be export problems.
NSHARE::CText const CHttpIOManagerRegister::NAME =CHttpIOManger::NAME;
CHttpIOManagerRegister::CHttpIOManagerRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 4))
{

}
void CHttpIOManagerRegister::MUnregisterFactory() const
{
	CKernelIo::sMGetInstance().MRemoveFactory(CHttpIOManger::NAME);
}
void CHttpIOManagerRegister::MAdding() const
{
	CKernelIo::sMAddFactory<CHttpIOManger>();
}
bool CHttpIOManagerRegister::MIsAlreadyRegistered() const
{
	if (CKernelIo::sMGetInstancePtr())
		return CKernelIo::sMGetInstance().MIsFactoryPresent(
				CHttpIOManger::NAME);
	return false;


}
} /* namespace NSHARE */
