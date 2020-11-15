/*
 * CSingletonProcess.cpp
 *
 * Copyright © 2020  https://github.com/CrazyLauren
 *
 *  Created on: 01.11.2020
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <SHARE/socket.h>
#include <SHARE/Socket/CNetBase.h>

namespace NSHARE
{
CSingletonProcess::CSingletonProcess(uint16_t aPort) :
		FPort(aPort), //
		FRc(-1) //
{
	MCheck();
}
CSingletonProcess::~CSingletonProcess()
{
	FSockId.MClose();
}
bool CSingletonProcess::MIs() const
{
	return FSockId != -1 && FRc == 0;
}
void CSingletonProcess::MCheck()
{
	if (!FSockId.MIsValid())
	{
		FSockId = CNetBase::MNewSocket(SOCK_DGRAM, 0);
		CNetBase::MSetCloseOnExist(FSockId);
	}
	if (FSockId.MIsValid())
	{
		FRc = CNetBase::MSetLocalAddrAndPort(FSockId, net_address(FPort));
	}
}
}
