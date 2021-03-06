// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CKernelChannel.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 28.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/share_socket.h>
#include <string.h>
#include <udt/programm_id.h>
#include <internel_protocol.h>
#include <core/kernel_type.h>
#include <core/CConfigure.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>

#include <io/CKernelIo.h>
#include <io/ILink.h>
#include "ILinkBridge.h"
#include "CConnectionHandlerFactory.h"

#include <parser_in_protocol.h>
#include "CKernelIOByTCPClient.h"
#include "CKernelChannel.h"

namespace NUDT
{
#define IMPL CKernelIOByTCPClient::CKernelChannel

NSHARE::CText const IMPL::NAME = "cl";

CKernelIOByTCPClient::CKernelChannel::CKernelChannel(
		network_channel_t const& aWhat,
		CKernelIOByTCPClient& aThis) :
		FState(E_CLOSED),	//
		FThis(aThis),	//
		FSetting(aWhat),	//
		Fd(-1)

{
	VLOG(3) << "Create object " << FSetting;

	FCBServiceConnect = NSHARE::CB_t(sMConnect, this);
	FCBServiceDisconncet = NSHARE::CB_t(sMDisconnect, this);

	FTcp += CTCPServer::value_t(CTCP::EVENT_CONNECTED, FCBServiceConnect);
	FTcp += CTCPServer::value_t(CTCP::EVENT_DISCONNECTED, FCBServiceDisconncet);
}
IMPL ::~CKernelChannel()
{
	MClose();

	VLOG(1)<<"Wait for thread stopped";
	NSHARE::CRAII<NSHARE::CMutex> _lock(FLockToRemove);
}
void IMPL::MClose()
{
	VLOG(2) << "Close client.";
	FTcp.MClose();
}
void IMPL::MOpen()
{
	FTcp.MOpen(FSetting.FAddress);
	NSHARE::operation_t _op(CKernelChannel::sMReceiver, this, NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);
}
NSHARE::eCBRval IMPL::sMConnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);

	net_address *_client = reinterpret_cast<net_address*>(aWhat);
	reinterpret_cast<CKernelChannel*>(aThis)->MConnect(_client);
	return E_CB_SAFE_IT;
}
NSHARE::eCBRval IMPL::sMDisconnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);

	net_address *_client = reinterpret_cast<net_address*>(aWhat);

	reinterpret_cast<CKernelChannel*>(aThis)->MDisconnect(_client);
	return E_CB_SAFE_IT;
}

void IMPL::MConnect(NSHARE::net_address* aVal)
{
	LOG_IF(ERROR,FState!=E_CLOSED) << "Invalid state" << (unsigned) FState;
	FState = E_SERVICE_CONNECTED;

	CHECK(!FLink);
	CHECK(!FConnectionHandler);
	CHECK(!CDescriptors::sMIsValid(Fd));

	Fd = CDescriptors::sMGetInstance().MCreate();
	VLOG(2) << "Connected to " << *aVal << ", " << NSHARE::get_unix_time();

	IConnectionHandlerFactory* _factory = CConnectionHandlerFactory::sMGetInstance().MGetFactory(FSetting.FProtocolType);

	CHECK_NOTNULL(_factory);
	IConnectionHandler* _p = _factory->MCreateHandler(Fd, NSHARE::get_unix_time(), this);
	_p->MConnect();
	if (_p->MState() != IConnectionHandler::E_ERROR)
	{
		LOG(INFO)<<*aVal<<" protocol :"<<_factory->MGetType();
		FConnectionHandler = handler_t(_p);
	}
	else
	{
		LOG(ERROR)<<"Connection error";
		delete _p;
	}
}

void IMPL::MDisconnect(NSHARE::net_address* aVal)
{
	VLOG(2) << "Disconnected from " << *aVal << ", " << NSHARE::get_unix_time();
	FState = E_CLOSED;
	if(FLink)
	{
		VLOG(2)<<"Closing link...";
		FThis.MRemoveChannel(this);
		FLink->MClose();
		FLink.MRelease();
	}
	if(FConnectionHandler)
	{
		VLOG(2)<<"Closing connection handler...";
		FConnectionHandler.MRelease();
	}
	if (CDescriptors::sMIsValid (Fd))
	{
		VLOG(1) << Fd<< " is removing.";
		CDescriptors::sMGetInstance().MClose(Fd);
	}
	Fd=-1;

	if(FSetting.FIsAutoRemoved)
	{
		FThis.MRemoveClient(FSetting);
	}
}

NSHARE::eCBRval IMPL::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CKernelChannel*>(aData)->MServiceReceiver();
	return E_CB_REMOVE;
}

void IMPL::MServiceReceiver()
{
	VLOG(2) << "Async receive";
	NSHARE::CRAII<NSHARE::CMutex> _lock(FLockToRemove);

	ISocket::data_t _data;
	LOG_IF(FATAL, !FTcp.MIsOpen()) << "Port is closed";
	for (/*HANG_INIT*/; FTcp.MIsOpen(); /*HANG_CHECK*/)
	{
		_data.clear();
		if (FTcp.MReceiveData( &_data, 0.0) > 0)
			MReceivedData(_data.cbegin(),_data.cend());
	}
	VLOG(1) << "Socket closed";
}

void IMPL::MReceivingForNewLink(
		const data_t::const_iterator& aBegin,
		const data_t::const_iterator& aEnd)
{
	VLOG(2) << "DG from new client ";
	CHECK(FConnectionHandler);
	FConnectionHandler->MReceivedData(aBegin, aEnd);
	IConnectionHandler::eState _state = FConnectionHandler->MState();
	switch (_state)
	{
	case IConnectionHandler::E_ERROR:
	{

		NSHARE::net_address _addr(FTcp.MGetSetting().FServerAddress);
		LOG(ERROR)<<"The  client "<<_addr<<" cannot connect as  the protocol "<<FSetting.FProtocolType<<" is invalid.";
		FTcp.MReOpen();
		break;
	}
	case IConnectionHandler::E_OK:
	{
		if (!MAddNewLink())
		{
			LOG(ERROR)<<"Cannot accept ";
			FTcp.MReOpen();

		}
		break;
	}
	case IConnectionHandler::E_CONTINUE:
	break;

	case IConnectionHandler::E_CLOSE:
	{
		FTcp.MReOpen();	//todo may be close
		break;
	}
}
}

void IMPL::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd)
{
	VLOG(2) << "Receive :" << aEnd-aBegin;
	LOG_IF(INFO, aEnd==aBegin) << "Empty Data.";
	if(aEnd!=aBegin)
	{
		if(FLink)
		{
			VLOG(2) << "It is our client";
			LOG_IF(ERROR, !FLink->MIsOpened()) << "The Channel " << FLink->MGetID() << "is not initialized";
			if (FLink->MIsOpened())
			if(!FLink->MReceivedData(aBegin, aEnd))
			{
				FTcp.MReOpen();
			}
		}
		else
		{
			MReceivingForNewLink(aBegin, aEnd);
		}
	}
}
bool IMPL::MAddNewLink()
{
	NSHARE::net_address _addr(FTcp.MGetSetting().FServerAddress);
	LOG(INFO)<<"The  client is connected to "<<_addr;
	FLink = smart_link_t(FConnectionHandler->MCreateLink());
	FConnectionHandler.MRelease();
	if(!FLink->MAccept())
	{
		LOG(ERROR)<<"Cannot accept "<<_addr;
		return false;
	}
	return FThis.MAddChannel(this,FLink->MGetID(),FLink->MLimits());

}
NSHARE::CConfig const& IMPL::MBufSettingFor(
		NSHARE::CConfig const& aFrom) const
{
	NSHARE::CConfig const& _io = aFrom.MChild(FSetting.FProtocolType);
	if (!_io.MIsEmpty())
	{
		if(FLink.MIs())
		{
			NSHARE::CConfig const& _for=FLink->MBufSettingFor(_io);
			if (!_for.MIsEmpty())
			{
				return _for;
			}
		}
	}
	NSHARE::CConfig const& _def = aFrom.MChild(CKernelIo::DEFAULT);
	if (!_def.MIsEmpty())
	{
		return _def;
	}
	return NSHARE::CConfig::sMGetEmpty();
}

bool IMPL::MCloseRequest(descriptor_t aId)
{
	FThis.MCloseRequest(aId);
	return true;
}
NSHARE::net_address  IMPL::MGetAddr() const
{
	NSHARE::net_address _addr(FTcp.MGetSetting().FServerAddress);
	return _addr;
}
bool IMPL::MInfo(NSHARE::CConfig & aTo)
{
	NSHARE::net_address _addr(FTcp.MGetSetting().FServerAddress);
	if(!_addr.MIsValid()) return false;
	aTo=_addr.MSerialize();
	return true;
}
bool IMPL::MConfig(NSHARE::CConfig & aTo)
{
	aTo=FSetting.MSerialize().MChild(network_channel_t::NAME);
	return true;
}

bool IMPL::MSend(const data_t& aVal)
{
	LOG_IF(ERROR,FState==E_CLOSED ) << "Invalid state"
	<< (unsigned) FState;

	if (!FTcp.MIsConnected())
	{
		LOG(ERROR)<<"Not connected:" <<FTcp;
		return false;
	}
	return FTcp.MSend(aVal).MIs();
}
NSHARE::CConfig IMPL::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	if(FLink.MIs())
		_conf.MAdd(FLink->MSerializeRef());
	_conf.MAdd(FTcp.MSerialize());
	_conf.MAdd(FSetting.MSerialize());
	_conf.MAdd("id",Fd);
	return _conf;
}
bool IMPL::MIs(network_channel_t const& aSetting) const
{
	return FSetting.FAddress==aSetting.FAddress;
}
} /* namespace NUDT */
