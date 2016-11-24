/*
 * CNewConnection.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>
#include <string.h>
#include <internel_protocol.h>
#include "receive_from.h"
#include "parser_in_protocol.h"
#include "../core/kernel_type.h"
#include "../core/CDescriptors.h"
#include "CKernelIOByTCP.h"
#include "CServerBridge.h"
#include "ILink.h"
#include "CConnectionHandlerFactory.h"
#include "CNewConnection.h"

namespace NUDT
{

CKernelIOByTCP::CConnectionHandler::CConnectionHandler(uint64_t aTime,
		CServerBridge* aBridge, handlers_name_t const& aHandlers) :
		FTime(aTime), //
		FBridge(aBridge), //
		Fd(CDescriptors::INVALID), //
		FHandlerTypes(
				aHandlers/*FKernel->FConfig.MChildren(CKernelIOByTCP::LINKS)*/)
{
	CHECK(!FHandlerTypes.empty());

	FIteration = 0;

	Fd = CDescriptors::sMGetInstance().MCreate();
	MCreateConnectionHandler();
}
bool CKernelIOByTCP::CConnectionHandler::MCreateConnectionHandler()
{
	if (FIteration)
	{
		LOG(ERROR)<<"Cannot change handler as some data has been sent. Iteration="<<FIteration;
		FHandler.MRelease();
		return false;
	}
	else if (FHandlerTypes.empty())
	{
		VLOG(2) << "There are no more handlers.";
		FHandler.MRelease();
		return false;
	}
	else
	{
		FIteration=0;
		handlers_name_t::iterator _it = FHandlerTypes.begin();
		for (; _it != FHandlerTypes.end();)
		{
			IConnectionHandlerFactory* _factory =
					CConnectionHandlerFactory::sMGetInstance().MGetFactory(
							*_it);
			FCurrent = *_it;
			_it = FHandlerTypes.erase(_it);

			CHECK_NOTNULL(_factory);
			IConnectionHandler* _p = _factory->MCreateHandler(Fd, FTime,
					FBridge.MGet());
			CHECK_NOTNULL(_p);


			if (_p->MState() != IConnectionHandler::E_ERROR)
			{
				LOG(INFO)<<FBridge->FAddr<<" protocol :"<<_factory->MGetType();
				FHandler = handler_t(_p);
				break;
			}
			else
			{
				delete _p;
			}
		}
		return FHandler.MIs();
	}
}
CKernelIOByTCP::CConnectionHandler::~CConnectionHandler()
{
	if (!FLinks.MIs() && CDescriptors::sMIsValid(Fd))
		CDescriptors::sMGetInstance().MClose(Fd);
}
bool CKernelIOByTCP::CConnectionHandler::MConnect()
{
	VLOG(2) << "New Connect.";
	CHECK_NOTNULL(FHandler.MGet());
	IConnectionHandler::eState _state = IConnectionHandler::E_ERROR;

	for (; _state == IConnectionHandler::E_ERROR && FHandler;)
	{
		if (FHandler->MConnect())
			++FIteration;
		_state = FHandler->MState();
		if (_state == IConnectionHandler::E_ERROR)
		{
			MCreateConnectionHandler();
		}
	}

	return FHandler;
}
NSHARE::CText const& CKernelIOByTCP::CConnectionHandler::MCurrentHandler() const
{
	return FCurrent;
}
smart_link_t const& CKernelIOByTCP::CConnectionHandler::MGetLink() const
{
	return FLinks;
}
NSHARE::intrusive_ptr<CKernelIOByTCP::CServerBridge> const& CKernelIOByTCP::CConnectionHandler::MGetBridge() const
{
	return FBridge;
}
IConnectionHandler::eState CKernelIOByTCP::CConnectionHandler::MReceivedData(
		data_t::const_iterator aBegin, data_t::const_iterator aEnd)
{
	VLOG(2) << "Receive data";

	IConnectionHandler::eState _state = IConnectionHandler::E_ERROR;

	for (; _state == IConnectionHandler::E_ERROR && FHandler;)
	{
		CHECK_NOTNULL(FHandler.MGet());
		VLOG(2)<<"handling by "<<FCurrent;
		if (FHandler->MReceivedData(aBegin, aEnd))
			++FIteration;

		_state = FHandler->MState();
		VLOG(2)<<"state="<<_state;
		switch (_state)
		{
		case IConnectionHandler::E_ERROR:
		{
			VLOG(2) << "The new client " << FBridge->FAddr
								<< " will be closed as  the protocol is invalid.";
			if (MCreateConnectionHandler())
				MConnect();
			break;
		}
		case IConnectionHandler::E_OK:
		{
			VLOG(2)<<"Creating new link";
			FLinks = smart_link_t(FHandler->MCreateLink());
			break;
		}
		case IConnectionHandler::E_CONTINUE:
		case IConnectionHandler::E_CLOSE:
			break;
		}
	}
	return _state;
}

} /* namespace NUDT */
