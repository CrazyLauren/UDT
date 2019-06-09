// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTCPServer.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <Net.h>
#include <Socket/CNetBase.h>
#include <console.h>
#include <Socket/CLoopBack.h>
#include <UType/CDenyCopying.h>
#include <UType/CThread.h>
#include <Socket/CTCPServerImpl.h>
#if !defined(_WIN32)
#	include <arpa/inet.h>                      // htons, htonl
#else
#	include <ws2tcpip.h>
#endif
namespace NSHARE
{
const NSHARE::CText CTCPServer::NAME="tcpser";
version_t CTCPServer::sMGetVersion()
{
	return version_t(1, 0);
}
CTCPServer::events_t::key_t const CTCPServer::EVENT_CONNECTED("Connected");
CTCPServer::events_t::key_t const CTCPServer::EVENT_DISCONNECTED(
		"Disconnected");

CTCPServer::CTCPServer(NSHARE::CConfig const& aConf) :
		events_t(this), FImpl(new CImpl(this))
{
	VLOG(2) << "Construct CTCPServer. Setting " << aConf << ":" << this;

	net_address _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid())<<"Configure for tcp server is not valid "<<aConf;
	if (_param.FPort)
		MOpen (_param);
}
CTCPServer::CTCPServer(const net_address& aParam) :
		events_t(this), FImpl(new CImpl(this))
{
	VLOG(2) << "Construct CTCPServer. Setting " << aParam << ":" << this;
	if (aParam.MIsValid())
		MOpen(aParam);
}
CTCPServer::~CTCPServer()
{
	VLOG(2) << "Destruct CTCPServer: " << this;
	MClose();
}
bool CTCPServer::MOpen(const net_address& aAddr, int aFlags)
{
	VLOG(2) << "Open server  " << aAddr << ", aFlags:" << aFlags << this;
	FImpl->FHostAddr=aAddr;
	return FImpl->MOpen();
}
bool CTCPServer::MReOpen()
{
	MClose();
	return FImpl->MOpen();
}
size_t CTCPServer::MAvailable() const
{
	return FImpl->MAvailable();
}
ssize_t CTCPServer::MReceiveData(recvs_t*aFrom, data_t*aBuf, const float aTime)
{
	return FImpl->MReceiveData(aFrom, aBuf, aTime);
}

ssize_t CTCPServer::MReceiveData(data_t* aBuf, const float aTime,
		recvs_from_t* aFrom)
{
	if (!aFrom)
		return MReceiveData(aBuf, aTime);
	recvs_t _from;
	ssize_t _val = MReceiveData(&_from, aBuf, aTime);
	recvs_t::const_iterator _it = _from.begin();
	for (; _it != _from.end(); ++_it)
	{
		receive_from_t _tmp;
		_tmp.FAddr = _it->FClient.FAddr.MSerialize();
		_tmp.FAddr.MAdd("time", _it->FClient.FTime);
		_tmp.FBufBegin = _it->FBufBegin;
		_tmp.FSize = _it->FSize;
		aFrom->push_back(_tmp);
	}
	return _val;
}

bool CTCPServer::MCloseClient(const net_address& aTo)
{
	return FImpl->MCloseClient(aTo);
}
void CTCPServer::MCloseAllClients()
{
	return FImpl->MCloseAllClients();
}
void CTCPServer::MClose()
{
	return FImpl->MClose();
}
CTCPServer::sent_state_t CTCPServer::MSendLoopBack(data_t const& aData)
{
	return FImpl->FLoopBack->FLoop.MSend(aData);
}

CTCPServer::sent_state_t CTCPServer::MSend(const void* pData, size_t nSize)
{
	return FImpl->MSend(pData, nSize);
}
CTCPServer::sent_state_t CTCPServer::MSend(const void* pData, size_t nSize, const net_address&aTo)
{
	return FImpl->MSend(pData, nSize, aTo);
}
CTCPServer::sent_state_t CTCPServer::MSend(const void* pData, size_t nSize,
		NSHARE::CConfig const& aTo)
{
	return FImpl->MSend(pData, nSize, aTo);
}
bool CTCPServer::MIsOpen() const
{
	return FImpl->FHostSock.MIsValid();
}
net_address CTCPServer::MGetSetting() const
{
	net_address _addr;
	MGetInitParam(&_addr);
	return _addr;
}
bool CTCPServer::MGetInitParam(net_address* aParam) const
{
	*aParam=FImpl->FHostAddr;
	return true;
}

bool CTCPServer::CImpl::MIsClient() const
{
	return FHostAddr.FIp.MIs();
}
bool CTCPServer::MCanReceive() const
{
	return FImpl->MCanReceive();
}
bool CTCPServer::CImpl::MCanReceive() const
{
	return FSelectSock.MIsSetUp();
}
bool CTCPServer::MIsClients() const
{
	return !FImpl->FClients.MGetRAccess().MGet().empty();
}

const CSocket& CTCPServer::MGetSocket(void) const
{
	return FImpl->FHostSock;
}
NSHARE::CConfig CTCPServer::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	{
		net_address _addr;
		MGetInitParam(&_addr);
		_conf.MAdd(_addr.MSerialize());
	}
	_conf.MAdd(FImpl->FDiagnostic.MSerialize());
	{
		CImpl::CRAccsess _r = FImpl->FClients.MGetRAccess();

		VLOG(2) << _r->size() << " clients and "
							<< FImpl->FSelectSock.MGetSockets().size()
							<< " sock now.";

		CImpl::clients_fd_t::const_iterator _it = _r->begin(),_it_end(_r->end());
		for (;_it!=_it_end;++_it)
		{
			NSHARE::CConfig _cl("cl");
			_cl.MAdd(_it->second.FAddr.MSerialize());
			_cl.MAdd("eagain",_it->second.FAgainError);
			_cl.MAdd(_it->second.FDiagnostic.MSerialize());
			_cl.MAdd("time",_it->second.FTime);
			_conf.MAdd(_cl);
		}
	}
	return _conf;
}
std::ostream& CTCPServer::MPrint(std::ostream& aStream) const
{
	net_address _addr;
	MGetInitParam(&_addr);
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		return aStream << NSHARE::NCONSOLE::eFG_RED << "Closed."<< NSHARE::NCONSOLE::eNORMAL;

	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << " Type: TCP. " << "Param: " << _addr << ". ";

	if (MIsClients())
	{
		CImpl::CRAccsess const _r = FImpl->FClients.MGetRAccess();

		VLOG(2) << _r->size() << " clients and "
							<< FImpl->FSelectSock.MGetSockets().size()
							<< " sock now.";

		CImpl::clients_fd_t::const_iterator _it = _r->begin();
		aStream << NSHARE::NCONSOLE::eFG_GREEN;
		for (;;)
		{
			aStream << _it->second;
			if (++_it == _r->end())
				break;
			else
				aStream << ';';
		}
		aStream << '.';
	}
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "No clients connected.";

	aStream << NSHARE::NCONSOLE::eNORMAL;
	return aStream;
}
std::pair<size_t, size_t> CTCPServer::MBufSize() const
{
	return std::pair<size_t, size_t>(FImpl->MGetSendBufSize(FImpl->FHostSock),
			FImpl->MGetRecvBufSize(FImpl->FHostSock));
}
bool CTCPServer::client_t::operator==(client_t const& aRht) const
{
	return aRht.FAddr == FAddr;
}
bool CTCPServer::client_t::operator==(net_address const& aRht) const
{
	return aRht == FAddr;
}

} /* namespace NSHARE */
