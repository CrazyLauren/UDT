/*
 * CControlByTCP.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.12.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>
#include <string.h>
#include <programm_id.h>
#include <internel_protocol.h>
#include "../core/kernel_type.h"
#include "../core/CConfigure.h"
#include "../core/CDescriptors.h"
#include "../core/CDataObject.h"

#include "receive_from.h"
#include "parser_in_protocol.h"

#include "ILinkBridge.h"
#include "CKernelIOByTCP.h"
#include "CNewConnection.h"
#include "CServerBridge.h"
#include "CKernelIo.h"

using namespace NSHARE;
namespace NUDT
{
NSHARE::CText const CKernelIOByTCPRegister::NAME = "tcp_io_manager";
NSHARE::CText const CKernelIOByTCP::NAME = "tcp_io_manager";
NSHARE::CText const CKernelIOByTCP::TIMEOUT = "timeout";
NSHARE::CText const CKernelIOByTCP::PORT = "port";
NSHARE::CText const CKernelIOByTCP::LINKS = "links";

bool CKernelIOByTCP::CServerBridge::MConfig(NSHARE::CConfig & aTo)
{
	NSHARE::CConfig const* _p = CConfigure::sMGetInstance().MGet().MFind(
			CKernelIOByTCP::NAME);
	if (_p)
		aTo = *_p;

	return _p;
}
CKernelIOByTCP::CKernelIOByTCP() :
		ITcpIOManager(NAME), //
		FTimeout(2000)
{

	FIo = NULL;
}

void CKernelIOByTCP::MInitTcp()
{
	CHECK(!FTcpServiceSocket.MIsOpen());
	CConfig* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);

	unsigned _port = 0xDEAD;
	if (_p)
	{

		FConfig = *_p;
		bool _val = _p->MGetIfSet(PORT, _port);
		(void) _val;
		LOG_IF(WARNING,!_val)
										<< "The port number is not present in the config file"
										<< CConfigure::sMGetInstance().MGetPath()
										<< ".Using standard port " << _port;

		_p->MGetIfSet(TIMEOUT, FTimeout);
	}
	VLOG(2) << "Construct IOContol Port=" << _port << ":" << this;
	FTcpServiceSocket.MOpen(net_address(_port));
}

CKernelIOByTCP::~CKernelIOByTCP()
{
	MClose();
}
void CKernelIOByTCP::MInit(CKernelIo * aVal)
{
	CHECK_NOTNULL(aVal);
	CHECK(FIo==NULL);
	FIo = aVal;

	//CHECK(FTcpServiceSocket.MIsOpen());
	FCBServiceConnect = NSHARE::CB_t(sMConnect, this);
	FCBServiceDisconncet = NSHARE::CB_t(sMDisconnect, this);

	FTcpServiceSocket += CTCPServer::value_t(CTCPServer::EVENT_CONNECTED,
			FCBServiceConnect);
	FTcpServiceSocket += CTCPServer::value_t(CTCPServer::EVENT_DISCONNECTED,
			FCBServiceDisconncet);


}
void CKernelIOByTCP::MCloseRequest(const descriptor_t& aFrom)
{
	FIo->MClose(aFrom);
}
void CKernelIOByTCP::MClose(const descriptor_t& aFrom)
{
	net_address _addr;
	{
		CHECK_GE(aFrom, 0);
		CRAII<CMutex> _blocked(FMutex);
		channels_t::iterator _it=FServiceChannels.find(aFrom);

		if (_it==FServiceChannels.end())
		{
			LOG(ERROR)<<"Ivalid descriptor "<<aFrom;
			return;
		}
		_addr = _it->second.FBridge->FAddr;
	}
	if (_addr.MIsValid())
		MRefuseClient(_addr);
}
void CKernelIOByTCP::MClose()
{
	FTcpServiceSocket.MClose();

	std::vector<net_address> _addr;
	for (channels_t::iterator i = FServiceChannels.begin(); i != FServiceChannels.end(); ++i)
	{
		_addr.push_back(i->second.FBridge->FAddr);
	}
	std::vector<net_address>::const_iterator _it=_addr.begin(),_it_end(_addr.end());
	for(;_it!=_it_end;++_it)
	{
		MRefuseClient(*_it);
	}
}

bool CKernelIOByTCP::MOpen(const void* aP)
{
	VLOG(2) << "Open KernelIOByTcp";
	MInitTcp();

	NSHARE::operation_t _op(CKernelIOByTCP::sMReceiver, this, NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);

	return true;
}

int CKernelIOByTCP::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CKernelIOByTCP*>(aData)->MServiceReceiver();
	return 0;
}

void CKernelIOByTCP::MServiceReceiver()
{
	VLOG(2) << "Async receive";
	ISocket::data_t _data;
	LOG_IF(FATAL, !FTcpServiceSocket.MIsOpen()) << "Port is closed";
	for (HANG_INIT; FTcpServiceSocket.MIsOpen(); HANG_CHECK)
	{
		_data.clear();
		CTCPServer::recvs_t _from;

		if (FTcpServiceSocket.MReceiveData(&_from, &_data, 0.0) > 0) MReceivedServiceDataImpl(_data, _from);
	}
	VLOG(1) << "Socket closed";
}

void CKernelIOByTCP::MReceivedServiceDataImpl(
		const NSHARE::ISocket::data_t& aData, const CTCPServer::recvs_t& aFrom)
{
	VLOG(2) << "Receive :" << aData.size();;
	{
		for (CTCPServer::recvs_t::const_iterator _it = aFrom.begin();
				_it != aFrom.end(); ++_it)
		{
			ISocket::data_t::const_iterator _data_it(
					(ISocket::data_t::const_pointer) _it->FBufBegin.base());
			VLOG(2) << "From " << _it->FClient << " " << _it->FSize
								<< " bytes.";
			smart_link_t _link;
			{ //locking for Link
				CRAII<CMutex> _blocked(FMutex);
				ips_t::const_iterator _jt = FActiveLinks.find(
						_it->FClient.FAddr);
				if (_jt != FActiveLinks.end()) //may be new
				{
					channels_t::const_iterator _it = FServiceChannels.find(_jt->second);
					if (_it != FServiceChannels.end())
						_link = _it->second.FLink;
				}
			}
			if (!_link) //may be new
			{
				VLOG(2) << "DG from new abonent " << _it->FClient;
				if (!MHandleNewLinkage(_it->FClient, _data_it,
						_data_it + _it->FSize))
				{
					MRefuseClient(_it->FClient.FAddr);
				}
			}
			else
			{
				VLOG(2) << _it->FClient << " is our client";
				MReceiveImpl(_link, _data_it, _data_it + _it->FSize);
			}
		}
	}
	LOG_IF(INFO, aData.empty()) << "Empty Data.";
	MCleanUpNewLinkages();
}

void CKernelIOByTCP::MReceiveImpl(const smart_link_t& _link,
		const data_t::const_iterator& aBegin,
		const data_t::const_iterator& aEnd)
{
	LOG_IF(ERROR, !_link->MIsOpened()) << "The Channel " << _link->MGetID()
												<< "is not inited";
	if (_link->MIsOpened())
		_link->MReceivedData(aBegin, aEnd);
}

void CKernelIOByTCP::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd, const descriptor_t& aFrom)
{
	CHECK_GE(aFrom, 0);

	smart_link_t _link;
	{
		CRAII<CMutex> _blocked(FMutex);
		channels_t::const_iterator _it = FServiceChannels.find(aFrom);
		if (_it != FServiceChannels.end())
			_link = _it->second.FLink;
		LOG_IF(DFATAL, !_link) << "No service channel for " << aFrom;
	}
	if (_link)
		MReceiveImpl(_link, aBegin, aEnd);
}

bool CKernelIOByTCP::MSend(const data_t& aVal, descriptor_t const& aTo)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_link_t _link;
	{
		CRAII<CMutex> _blocked(FMutex);
		channels_t::const_iterator _it = FServiceChannels.find(aTo);
		if (_it != FServiceChannels.end())
			_link = _it->second.FLink;
	}
	if (!_link)
	{
		LOG(ERROR)<< "No  channel for " << aTo;
		return false;
	}

	return _link->MSend(aVal);
}
template<class T>
inline bool CKernelIOByTCP::MSendImpl(const T& _id, descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_link_t _link;
	{
		CRAII<CMutex> _blocked(FMutex);
		channels_t::const_iterator _it = FServiceChannels.find(aTo);
		if (_it != FServiceChannels.end())
			_link = _it->second.FLink;
	}
	if (!_link)
	{
		LOG(ERROR)<< "No  channel for " << aTo;
		return false;
	}
	return _link->MSend(_id,aRoute,aError);
}
inline bool CKernelIOByTCP::MSendImpl(const user_data_t& _id, descriptor_t const& aTo)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_link_t _link;
	{
		CRAII<CMutex> _blocked(FMutex);
		channels_t::const_iterator _it = FServiceChannels.find(aTo);
		if (_it != FServiceChannels.end())
			_link = _it->second.FLink;
	}
	if (!_link)
	{
		LOG(ERROR)<< "No  channel for " << aTo;
		return false;
	}
	return _link->MSend(_id);
}
bool CKernelIOByTCP::MSend(const programm_id_t& _id, descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}

bool CKernelIOByTCP::MSend(const fail_send_t& _id, descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}
bool CKernelIOByTCP::MSend(const kernel_infos_array_t& _id, descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}
bool CKernelIOByTCP::MSend(const user_data_t& aVal, descriptor_t const& aTo)
{
	return MSendImpl(aVal, aTo);
}
bool CKernelIOByTCP::MSend(const demand_dgs_for_t& _id, descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}

bool CKernelIOByTCP::MSendService(const data_t& aVal,
		const NSHARE::net_address& aAddr)
{
	VLOG(2) << "Sending service msg to " << aAddr;
	if (FTcpServiceSocket.MSend(&aVal.front(), aVal.size(), aAddr).MIs())
		return true;

	LOG(ERROR)<< "Can't service send MSG to " << aAddr;
	return false;
}
void CKernelIOByTCP::MRefuseClient(const NSHARE::net_address& _addr)
{
	FTcpServiceSocket.MCloseClient(_addr);
}
void CKernelIOByTCP::MCleanUpNewLinkages()
{
	std::vector<NSHARE::net_address> _reset;
	{
		CRAII<CMutex> _blocked(FMutex);
		new_channels_t::iterator _it = FNewChannels.begin();
		VLOG(2) << "CleanUp";
		for (; _it != FNewChannels.end(); ++_it)
		{
			unsigned _delta = NSHARE::get_unix_time() - _it->second->FTime;
			VLOG(2) << NSHARE::get_unix_time() << " - "
								<< _it->second->FTime;
			if (_delta > FTimeout)
			{
				LOG(WARNING)<< _it->second->MGetBridge()->FAddr << " is enemy client as timeout";

				_reset.push_back(_it->second->MGetBridge()->FAddr);
			}
		}
	}
	for (; !_reset.empty(); _reset.pop_back())
	{
		MRefuseClient(_reset.back());
	}

}


NSHARE::CConfig const& CKernelIOByTCP::MBufSettingFor(const descriptor_t& aFor,
		NSHARE::CConfig const& aFrom) const
{
	smart_link_t _link;
	{
		CRAII<CMutex> _blocked(FMutex);
		channels_t::const_iterator _it = FServiceChannels.find(aFor);
		if (_it != FServiceChannels.end())
			_link = _it->second.FLink;
	}
	if (_link.MIs())
	{
		NSHARE::CConfig const& _io = aFrom.MChild(_link->MGetType());
		if (!_io.MIsEmpty())
		{
			NSHARE::CConfig const& _for=_link->MBufSettingFor(_io);
			if (!_for.MIsEmpty())
			{
				return _for;
			}
		}
		NSHARE::CConfig const& _def = aFrom.MChild(CKernelIo::DEFAULT);
		if (!_def.MIsEmpty())
		{
			return _def;
		}
	}
	return NSHARE::CConfig::sMGetEmpty();
}

void CKernelIOByTCP::MAddNewActiveLink(channel_t const& aVal)
{

	descriptor_t const _desc = aVal.FLink->MGetID();
	CHECK_GT(_desc, 0);

	FServiceChannels[_desc] = aVal;
	FActiveLinks[aVal.FBridge->FAddr] = _desc;
	CHECK_NOTNULL(FIo);

	FIo->MAddChannelFor(_desc, this, aVal.FLink->MLimits());
}

bool CKernelIOByTCP::MAcceptLink(const smart_new_client_t& aNewLink)
{
	CHECK(aNewLink);
	channel_t _channel;
	_channel.FLink = aNewLink->MGetLink();
	_channel.FBridge =
			dynamic_cast<CServerBridge*>(aNewLink->MGetBridge().MGet());
	CHECK(_channel.FLink);
	CHECK(_channel.FBridge);
	if (!_channel.FLink->MIsOpened())
	{
		LOG(ERROR)<< "The Abonent " << _channel.FBridge->FAddr
		<< " has not been initialized. Ignoring...";

		return false;
	}

	//CRAII<CMutex> _blocked(FMutex);
	VLOG(1) << _channel.FBridge->FAddr << " is a valid new client";
	CHECK_GE(_channel.FLink->MGetID(), 0);

	MAddNewActiveLink(_channel);

	return _channel.FLink->MAccept();
}

bool CKernelIOByTCP::MHandleNewLinkage(CTCPServer::client_t const& aAb,
		ISocket::data_t::const_iterator aBegin,
		ISocket::data_t::const_iterator aEnd)
{
	VLOG(1) << "Handle new abonent " << aAb;
	smart_new_client_t _connect;
	{
		CRAII<CMutex> _blocked(FMutex);
		new_channels_t::iterator _it = FNewChannels.find(aAb.FAddr);
		LOG_IF(ERROR,_it == FNewChannels.end()) << "Invalid new abonent "
														<< aAb;
		if (_it != FNewChannels.end())
		{
			_connect = _it->second;
		}
	}
	DCHECK(_connect);
	if (!_connect)
		return false;

	switch (_connect->MReceivedData(aBegin, aEnd))
	{
	case IConnectionHandler::E_ERROR:
	{
		LOG(INFO)<<"The new client "<<aAb.FAddr<<" has to be closed.";
		FSampling[aAb.FAddr]=_connect->MCurrentHandler();
		return false;
		break;
	}
	case IConnectionHandler::E_OK:
	{
		CRAII<CMutex> _blocked(FMutex);

		FSampling.erase(aAb.FAddr);
		bool _is = MAcceptLink(_connect);
		FNewChannels.erase(aAb.FAddr);
		LOG_IF(INFO,_is)<<"The client "<<aAb.FAddr<<" is accepted.";
		if (!_is)
		{
			LOG(ERROR)<<"Cannot accept "<<aAb.FAddr;
			return false;//will be remove automate when disconnect function will be called
		}
		break;
	}
	case IConnectionHandler::E_CONTINUE:
	{
		VLOG(2)<<"The handler ask for continue.";
		break;
	}
	case IConnectionHandler::E_CLOSE:
	{
		VLOG(2)<<"The handler command to close client";
		return false;
		break;
	}
	}

	return true;
}

int CKernelIOByTCP::sMConnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);
	CTCPServer::client_t* _client =
			reinterpret_cast<CTCPServer::client_t*>(aWhat);
	reinterpret_cast<CKernelIOByTCP*>(aThis)->MConnect(_client);
	return 0;
}

void CKernelIOByTCP::MConnect(CTCPServer::client_t* aVal)
{
	VLOG(2) << "New abonent " << *aVal << ", " << NSHARE::get_unix_time();
	CRAII<CMutex> _blocked(FMutex);

	const  NSHARE::net_address& _addr = aVal->FAddr;
	smart_new_client_t _link;
	CConnectionHandler::handlers_name_t _handlers;

	last_sampling_link_type_t::const_iterator _kt = FSampling.find(_addr);

	ConfigSet _conf = FConfig.MChildren(CKernelIOByTCP::LINKS);
	ConfigSet::const_iterator _it = _conf.begin(), _it_end(_conf.end());

	bool _need_filter = _kt != FSampling.end()
			&& _kt->second != _conf.back().MValue();
	for (; _it != _it_end; ++_it)
	{
		VLOG(2) << "Handler = " << _it->MValue();
		if (!_need_filter)
			_handlers.push_back(_it->MValue());
		else
		{
			VLOG(2) << "Filtering ...";
			if (_it->MValue() == _kt->second)
				_need_filter = false;
			else
				VLOG(2) << "Filtering " << _it->MValue();
		}
	}
	CHECK(!_need_filter);
	{
		//CRAII<CMutex> _blocked(FMutex);
		MDisconnectImpl(_addr); //make sure it's not connected.

		NSHARE::intrusive_ptr<CServerBridge> _bridge(
				new CServerBridge(*this, _addr));
		_link = smart_new_client_t(
				new CConnectionHandler(NSHARE::get_unix_time(), _bridge.MGet(),
						_handlers));
	}
	if (_link->MConnect())
	{
		//CRAII<CMutex> _blocked(FMutex);
		new_channels_t::value_type _val(_addr, _link);
		FNewChannels.insert(_val);
	}
	else
	{
		LOG(ERROR)<<"Connecting error for "<<_addr;
		MRefuseClient(_addr);
	}
}
int CKernelIOByTCP::sMDisconnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);
	CTCPServer::client_t* _client =
			reinterpret_cast<CTCPServer::client_t*>(aWhat);
	CRAII<CMutex> _blocked(reinterpret_cast<CKernelIOByTCP*>(aThis)->FMutex);
	reinterpret_cast<CKernelIOByTCP*>(aThis)->MDisconnectImpl(_client->FAddr);
	return 0;
}

void CKernelIOByTCP::MDisconnectImpl(const NSHARE::net_address& _addr)
{

	ips_t::iterator _it_client = FActiveLinks.find(_addr);
	if (_it_client != FActiveLinks.end())
	{
		descriptor_t i = _it_client->second;
		CHECK_GE(i, 0);
		VLOG(2) << "The client " << i << " will be disconnected.";

		channels_t::iterator _it = FServiceChannels.find(i);
		if (_it == FServiceChannels.end())
		{
			LOG(ERROR)<<"Ivalid descriptor "<<i;
			return;
		}

		CHECK(FServiceChannels[i].FLink);

		CHECK_NOTNULL(FIo);
		FIo->MRemoveChannelFor(i, this);

		_it->second.FLink->MClose();

		FActiveLinks.erase(_it_client);

		smart_link_t _null;
		_it->second.FLink = _null;

		FServiceChannels.erase(_it);
		if (CDescriptors::sMIsValid(i))
		{
			VLOG(1) << i << " is removing.";
			CDescriptors::sMGetInstance().MClose(i);
		}
	}
	else
	{
		new_channels_t::iterator _it = FNewChannels.find(_addr);
		LOG_IF(WARNING,_it == FNewChannels.end())
															<< "Anonymous client disconnected.";

		if (_it != FNewChannels.end())
		{
			VLOG(2) << "New channel " << _addr << " is removed.";
			FNewChannels.erase(_it);
		}
	}
}

bool CKernelIOByTCP::MIsOpen() const
{
	return FTcpServiceSocket.MIsOpen();
}
CKernelIOByTCP::descriptors_t CKernelIOByTCP::MGetDescriptors() const
{
	CRAII<CMutex> _blocked(FMutex);
	ips_t::const_iterator _it = FActiveLinks.begin(), _it_end(
			FActiveLinks.end());

	descriptors_t _buf;
	for (; _it != _it_end; ++_it)
	{
		_buf.push_back(_it->second);
	}
	return _buf;
}
bool CKernelIOByTCP::MIs(descriptor_t aVal) const
{
	if (!CDescriptors::sMIsValid(aVal))
		return false;
	CRAII<CMutex> _blocked(FMutex);
	return FServiceChannels.find(aVal)!=FServiceChannels.end();
}
NSHARE::CConfig CKernelIOByTCP::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FTcpServiceSocket.MSerialize());
	_conf.MAdd("timeout",FTimeout);
	{
		CRAII<CMutex> _blocked(FMutex);
		ips_t::const_iterator _it = FActiveLinks.begin(), _it_end(
				FActiveLinks.end());
		for (; _it != _it_end; ++_it)
		{
			channels_t::const_iterator jt = FServiceChannels.find(_it->second);
			DCHECK(jt != FServiceChannels.end());
			if (jt != FServiceChannels.end())
			{
				CHECK(jt->second.FLink.MIs());
				_conf.MAdd(jt->second.FLink->MSerializeRef());
			}
		}
	}
	return _conf;
}
CKernelIOByTCPRegister::CKernelIOByTCPRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 3))
{

}
void CKernelIOByTCPRegister::MUnregisterFactory() const
{
	CKernelIo::sMGetInstance().MRemoveFactory(CKernelIOByTCP::NAME);
}
void CKernelIOByTCPRegister::MAdding() const
{
//CKernelIOByTCP* _p =
	CKernelIo::sMAddFactory<CKernelIOByTCP>();
}
bool CKernelIOByTCPRegister::MIsAlreadyRegistered() const
{
	if (CKernelIo::sMGetInstancePtr())
		return CKernelIo::sMGetInstance().MIsFactoryPresent(
				CKernelIOByTCP::NAME);
	return false;

}
}
