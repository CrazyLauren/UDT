/*
 * CServerBridge.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSERVERBRIDGE_H_
#define CSERVERBRIDGE_H_
#include "ILinkBridge.h"
namespace NUDT
{
class CKernelIOByTCP::CServerBridge: public ILinkBridge
{
public:
	CServerBridge(CKernelIOByTCP& aThis,net_address const& aAddr) :
			FThis(aThis),//
			FAddr(aAddr)
	{
		;
	}

	virtual bool MSend(const data_t& aVal)
	{
		return FThis.MSendService(aVal,FAddr);
	}
	virtual bool MCloseRequest(descriptor_t  aFor)
	{
		FThis.MCloseRequest(aFor);
		return true;
	}
	virtual bool MInfo(NSHARE::CConfig & aTo)
	{
		aTo=FAddr.MSerialize();
		return true;
	}
	virtual bool MConfig(NSHARE::CConfig & aTo);
	NSHARE::net_address MGetAddr() const
	{
		return FAddr;
	}
	CKernelIOByTCP& FThis;
	NSHARE::net_address const FAddr;
};
}

#endif /* CSERVERBRIDGE_H_ */
