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
	/*
	 *	chanelog
	 *
	 *	Версия 0.2 (29.01.2014)
	 *
	 *		- Исправлен баг с loopback socket.
	 *  		Если кто-то подключался одновременно c "петлёй", то её сокет терялся.
	 *			Теперь в MAddNewClient  присвается FIsLoopConnected true.
	 *
	 *	Версия 0.3 (01.02.2016)
	 *		- Исправлен баг с loopback socket.
	 *			Гонка потоков. Если connect loopback случался рашьше чем accept
	 *			ресурсы утекали.
	 *
	 *	Версия 0.4 (05.02.2016)
	 *		- loopback trick переписон через два udp socketa - теперь всё работает как часы.
	 *
	 *	Версия 0.5 (22.04.2016)
	 *		- CImpl
	 *		- убран костыль smart_recv_t
	 *
	 *	Версия 0.6 (23.04.2016)
	 *		- Теперь можно открять TCP сервер со случайным портом
	 *
	 *	Версия 0.7 (22.05.2016)
	 *		- LoopBack выделен в отдельный класс
	 *
	 *	Версия 1.0 (30.06.2016)
	 *		- Смена API
	 *
	 *	Версия 1.1 (30.08.2016)
	 *		- Добавлен метод MGetBuf
	 *
	 *	Версия 1.2 (14.07.2019)
	 *		- Обновлена реализация класса
	 */
	return version_t(1, 2);
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
bool CTCPServer::MSetAddress(const net_address& aParam)
{
	return FImpl->MSetAddress(aParam);
}
bool CTCPServer::MOpen(const net_address& aAddr, int aFlags)
{
	VLOG(2) << "Open server  " << aAddr << ", aFlags:" << aFlags << this;
	if(MIsOpen() || !MSetAddress(aAddr))
		return false;
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

ssize_t CTCPServer::MReceiveData(data_t* aData, const float aTime)
{
	return MReceiveData(NULL, aData, aTime);
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
bool CTCPServer::MClose(const client_t& aClient)
{
	return FImpl->MCloseClient(aClient.FAddr);
}
void CTCPServer::MCloseAllClients()
{
	return FImpl->MRemoveAllClients();
}
void CTCPServer::MClose()
{
	return FImpl->MClose();
}
void CTCPServer::MForceUnLock()
{
	return FImpl->MForceUnLock();
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
	return FImpl && FImpl->MIsOpen();
}
CTCPServer::settings_t const& CTCPServer::MGetSetting() const
{
	return FImpl->MGetSetting();
}
NSHARE::CConfig CTCPServer::MSettings(void) const
{
	return MGetSetting().MSerialize();
}

bool CTCPServer::MIsClients() const
{
	return FImpl->MIsClients();
}
bool CTCPServer::MIsClient(const net_address& aIP) const
{
	return FImpl->MIsClient(aIP);
}

const CSocket& CTCPServer::MGetSocket(void) const
{
	return FImpl->MGetSocket();
}
diagnostic_io_t const& CTCPServer::MGetDiagnosticState() const
{
	return FImpl->MGetDiagnosticState();
}
NSHARE::CConfig CTCPServer::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);

	_conf.MAdd(MSettings());
	_conf.MAdd(MGetDiagnosticState().MSerialize());

	return _conf;
}
CTCPServer::list_of_clients CTCPServer::MGetClientInfo() const
{
	return FImpl->MGetConnectedClientInfo();
}
CTCPServer::list_of_clients CTCPServer::MGetDisconnectedClientInfo() const
{
	return FImpl->MGetDisconnectedClientInfo();
}
std::ostream& CTCPServer::MPrint(std::ostream& aStream) const
{
	settings_t const& _addr(MGetSetting());
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		return aStream << NSHARE::NCONSOLE::eFG_RED << "Closed."<< NSHARE::NCONSOLE::eNORMAL;

	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << " Type: TCP. " << "Param: " << _addr << ". ";

	list_of_clients const _clients(MGetClientInfo());
	if (!_clients.empty())
	{

		list_of_clients::const_iterator _it = _clients.begin();
		aStream << NSHARE::NCONSOLE::eFG_GREEN;
		for (;;)
		{
			aStream << *_it;
			if (++_it == _clients.end())
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
	return std::pair<size_t, size_t>(FImpl->MGetSendBufSize(MGetSocket()),
			FImpl->MGetRecvBufSize(MGetSocket()));
}

const CText CTCPServer::client_t::NAME="cl";
const CText CTCPServer::client_t::KEY_CONNECTION_TIME="connect_time";
const CText CTCPServer::client_t::KEY_DISCONNECT_TIME="disconnect_time";


CTCPServer::client_t::client_t(net_address const & aAddr ):
		FAddr(aAddr),//
		FTime(aAddr.MIsValid()?NSHARE::get_time():-1),//
		FDiconnectTime(0)
{

}
CTCPServer::client_t::client_t(NSHARE::CConfig const& aConf) :
		FAddr(aConf.MChild(net_address::NAME)),//
		FTime(-1),//
		FDiconnectTime(0),//
		FDiagnostic(aConf.MChild(diagnostic_io_t::NAME))//

{
	aConf.MGetIfSet(KEY_CONNECTION_TIME,FTime);
	aConf.MGetIfSet(KEY_DISCONNECT_TIME,FDiconnectTime);
}
bool CTCPServer::client_t::MIsValid() const
{
	return FAddr.MIsValid()//
			&&FDiagnostic.MIsValid()//
			&&FTime>=0//
			&&FDiconnectTime>=0;
}
CConfig CTCPServer::client_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(FAddr.MSerialize());
	_conf.MAdd(FDiagnostic.MSerialize());
	_conf.MAdd(KEY_CONNECTION_TIME,FTime);
	_conf.MAdd(KEY_DISCONNECT_TIME,FDiconnectTime);
	return _conf;
}
bool CTCPServer::client_t::operator==(client_t const& aRht) const
{
	return aRht.FAddr == FAddr;
}
bool CTCPServer::client_t::operator==(net_address const& aRht) const
{
	return aRht == FAddr;
}
const unsigned CTCPServer::settings_t::DEFAULT_LIST_QUEUE_LEN=2;
const CText CTCPServer::settings_t::NAME=CTCPServer::NAME;
const CText CTCPServer::settings_t::KEY_LIST_QUEUE_LEN="listen_queue_len";

CTCPServer::settings_t::settings_t(network_port_t const& aParam) :
		FServerAddress(aParam),//
		FListenQueue(DEFAULT_LIST_QUEUE_LEN)
{
}

CTCPServer::settings_t::settings_t(net_address const& aParam) :
		FServerAddress(aParam),//
		FListenQueue(DEFAULT_LIST_QUEUE_LEN)
{
}

CTCPServer::settings_t::settings_t(NSHARE::CConfig const& aConf) :
		FSocketSetting(aConf.MChild(socket_setting_t::NAME)),//
		FServerAddress(aConf.MChild(net_address::NAME)),//
		FListenQueue(DEFAULT_LIST_QUEUE_LEN)
{
	aConf.MGetIfSet(KEY_LIST_QUEUE_LEN,FListenQueue);
}
bool CTCPServer::settings_t::MIsValid() const
{
	return FSocketSetting.MIsValid()//
			&&FServerAddress.MIsValid()//
			&&FListenQueue>1;
}

CConfig CTCPServer::settings_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(FServerAddress.MSerialize());
	_conf.MAdd(FSocketSetting.MSerialize());
	_conf.MAdd(KEY_LIST_QUEUE_LEN,FListenQueue);
	return _conf;
}
} /* namespace NSHARE */
