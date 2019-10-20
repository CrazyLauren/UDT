// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CNewConnection.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share/share_socket.h>
#include <string.h>

#include <core/kernel_type.h>
#include <core/CDescriptors.h>
#include "CKernelIOByTCP.h"
#include "CServerBridge.h"
#include <io/ILink.h>
#include "CConnectionHandlerFactory.h"

#include <internel_protocol.h>
#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_PROTOCOL_MSG,protocol_type_dg_t)/*Протокол КД*/\
	/*END*/
#include <parser_in_protocol.h>
#include "CNewConnection.h"

namespace NUDT
{

CKernelIOByTCP::CConnectionHandler::CConnectionHandler(uint64_t aTime,
		CServerBridge* aBridge,handlers_name_t const& aHandlers ) :
		FTime(aTime), //
		FBridge(aBridge), //
		Fd(CDescriptors::INVALID),//
		FParser(this),//
		FHandlerTypes(aHandlers)
{

	Fd = CDescriptors::sMGetInstance().MCreate();
}
CKernelIOByTCP::CConnectionHandler::~CConnectionHandler()
{
	if (!FLinks.MIs() && CDescriptors::sMIsValid(Fd))
		CDescriptors::sMGetInstance().MClose(Fd);
}
template<class T>
void CKernelIOByTCP::CConnectionHandler::MProcess(T const* aP, parser_t* aThis)
{

}
template<>
void CKernelIOByTCP::CConnectionHandler::MProcess(protocol_type_dg_t const* aP,
		parser_t* aThis)
{
	handlers_name_t::const_iterator _it=FHandlerTypes.begin(),_it_end=FHandlerTypes.end();


	for (; _it !=_it_end; ++_it)
	{
		IConnectionHandlerFactory* _factory=CConnectionHandlerFactory::sMGetInstance().MGetFactory(*_it);
		DCHECK_NOTNULL(_factory);
		if (_factory && aP->MGetProtocol() == _factory->MGetProtocolNumber())
		{
			IConnectionHandler* _p = _factory->MCreateHandler(Fd,
					FTime, FBridge.MGet());
			CHECK_NOTNULL(_p);
			LOG(INFO)<<FBridge->FAddr<<" protocol :"<<(*_it);
			FHandler = handler_t(_p);
			FHandler->MConnect();
		}
	}
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
	if (!FHandler.MIs())
	{
		if (CInParser<void>::sMIsValidProtocol(aBegin, aEnd))
			FParser.MReceivedData(aBegin, aEnd);
	}
	if (FHandler.MIs())
	{
		CHECK_NOTNULL(FHandler.MGet());
		FHandler->MReceivedData(aBegin, aEnd);
		_state = FHandler->MState();
		VLOG(2) << "state=" << _state;
		switch (_state)
		{
		case IConnectionHandler::E_OK:
		{
			VLOG(2) << "Creating new link";
			FLinks = smart_link_t(FHandler->MCreateLink());
			break;
		}
		case IConnectionHandler::E_ERROR:
		case IConnectionHandler::E_CONTINUE:
		case IConnectionHandler::E_CLOSE:
			break;
		}
	}
	return _state;
}

} /* namespace NUDT */
