/*
 * CHttpNewConncetion.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <udt_share.h>
#include "../../core/IState.h"
#include "../../core/kernel_type.h"
#include "../CConnectionHandlerFactory.h"
#include "../ILinkBridge.h"
#include "../CLinkDiagnostic.h"
#include "../ILink.h"
#include "CHttpNewConncetion.h"
#include "CHttpRequest.h"
#include "CHttpLink.h"
namespace NUDT
{
namespace
{
static const NSHARE::CText NEW_NAME = "http";

class CNewHttpHandler: public IConnectionHandler
{
	bool FIsHTTPHeader;
	CHttpRequest FRequest;
	descriptor_t FFd;
	uint64_t FTime;
	smart_bridge_t FKernel;
public:
	CNewHttpHandler(descriptor_t aFD, uint64_t aTime, ILinkBridge* aKer) :
			FFd(aFD), FTime(aTime), FKernel(aKer)
	{
		FIsHTTPHeader = false;
		FState = E_CLOSE;
	}
	~CNewHttpHandler()
	{
	}
	virtual bool MConnect()
	{
		return false;
	}
	virtual bool MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd)
	{
		unsigned _count = FRequest.MParse(&(*aBegin), aEnd - aBegin);
		VLOG(2) << "Parse " << _count << " bytes by http. Error="
							<< FRequest.MError();
		switch (FRequest.MError())
		{
		case E_HTTP_OK:
		{
			FState = E_OK;
			break;
		}

		default:
			FState = E_ERROR;
			break;
		}
		return true;
	}
	virtual ILink* MCreateLink()
	{
		CHttpLink* _p = new CHttpLink(FFd,  FTime, FKernel.MGet());
		_p->MReceivedData(FRequest);
		return _p;
	}
};
class CNewConnection: public IConnectionHandlerFactory
{
public:
	CNewConnection() :
			IConnectionHandlerFactory(NEW_NAME)
	{

	}

	IConnectionHandler* MCreateHandler(descriptor_t aFD, uint64_t aTime,
			ILinkBridge* aKer)
	{
		return new CNewHttpHandler(aFD, aTime, aKer);
	}

};
}
NSHARE::CText const CIOHttplLinkRegister::NAME = "http_link_register";
CIOHttplLinkRegister::CIOHttplLinkRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 3))
{

}
void CIOHttplLinkRegister::MUnregisterFactory() const
{
	CConnectionHandlerFactory::sMGetInstance().MRemoveFactory(NEW_NAME);
}
void CIOHttplLinkRegister::MAdding() const
{
	CConnectionHandlerFactory::sMAddFactory<CNewConnection>();
}
bool CIOHttplLinkRegister::MIsAlreadyRegistered() const
{
	if (CConnectionHandlerFactory::sMGetInstancePtr())
		return CConnectionHandlerFactory::sMGetInstance().MIsFactoryPresent(
				NEW_NAME);
	return false;

}

} /* namespace NSHARE */
