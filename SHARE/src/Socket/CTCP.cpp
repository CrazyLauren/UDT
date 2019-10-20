// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTCP.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 04.03.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#include <share/Net.h>
#include <share/Socket/CNetBase.h>
#include <share/UType/CDenyCopying.h>
#include <share/UType/CThread.h>
#include <share/Socket/CTcpClientImpl.h>
#include <share/console.h>

namespace NSHARE
{
//Early there were two implementation. One of was client,the other was server.
//Now In "CTCP" class there is only client  implementation.
//"CTCP::CClientImpl" class  is remained by historical reason.

const NSHARE::CText CTCP::NAME="tcp";
CTCP::events_t::key_t const CTCP::EVENT_CONNECTED="Connect";
CTCP::events_t::key_t const CTCP::EVENT_DISCONNECTED="Disconnect";

CTCP::CTCP(NSHARE::CConfig const& aConf) :
				FClientImpl(new CClientImpl(*this,settings_t(aConf)))
{
	LOG_IF(DFATAL,!MGetSetting().MIsValid())<<"Configure for tcp is not valid "<<aConf;
	if (MGetSetting().MIsValid())
		MOpen();
}
CTCP::CTCP(net_address const& aParam) :
		FClientImpl(new CClientImpl(*this,settings_t(aParam)))
{
	if (MGetSetting().MIsValid())
		MOpen();
}
CTCP::CTCP(settings_t const& aParam) :
		FClientImpl(new CClientImpl(*this,aParam))
{
	if (MGetSetting().MIsValid())
		MOpen();
}
CTCP::~CTCP()
{
	MClose();
}

bool CTCP::MOpen(net_address const& aAddr)
{
	if(MIsOpen())
		return false;
	FClientImpl->MSetAddress(aAddr);
	return MOpen();
}
bool CTCP::MConnect()
{
	if(!MIsOpen())
		return false;
	return FClientImpl->MClientConnect();
}
bool CTCP::MOpen()
{
	return FClientImpl->MOpen();
}
bool CTCP::MReOpen()
{
	MClose();
	return MOpen();
}
NSHARE::CConfig CTCP::MSettings(void) const
{
	return MGetSetting().MSerialize();
}
ssize_t CTCP::MReceiveData(data_t * aBuf, float const aTime)
{
	return FClientImpl->MReceiveData(aBuf, aTime);
}
void CTCP::MClose()
{
	FClientImpl->MClose();
}
CTCP::sent_state_t CTCP::MSend(void const* pData, size_t nSize)
{
	return FClientImpl->MSend(pData, nSize);
}
bool CTCP::MIsOpen() const
{
	return FClientImpl->MIsOpen();
}
CTCP::settings_t const& CTCP::MGetSetting() const
{
	return FClientImpl->MGetInitParam();
}
const CSocket& CTCP::MGetSocket(void) const
{
	return FClientImpl->FSock;
}
bool CTCP::MIsConnected() const
{
	return FClientImpl->FIsConnected;
}
size_t CTCP::MAvailable() const
{
	return FClientImpl->CTcpImplBase::MAvailable(FClientImpl->FSock);
}
std::ostream& CTCP::MPrint(std::ostream & aStream) const
{
	net_address const& _addr(MGetSetting().FServerAddress);
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		return aStream << NSHARE::NCONSOLE::eFG_RED << "Close.";

	aStream << NSHARE::NCONSOLE::eNORMAL;

	aStream << " Type: TCP. " << "Param: " << _addr << ". ";
	if (MIsConnected())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Connected.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "Disconnected.";
	aStream << NSHARE::NCONSOLE::eNORMAL;
	return aStream;
}
NSHARE::CConfig CTCP::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(MGetSetting().MSerialize());

	_conf.MAdd("con", MIsConnected());
	_conf.MAdd(FClientImpl->FDiagnostic.MSerialize());
	return _conf;
}
std::pair<size_t, size_t> CTCP::MBufSize() const
{
	return std::pair<size_t, size_t>(FClientImpl->MGetSendBufSize(FClientImpl->FSock),FClientImpl->MGetRecvBufSize(FClientImpl->FSock));
}
diagnostic_io_t const& CTCP::MGetDiagnosticState() const
{
	return FClientImpl->FDiagnostic;
}
const CText CTCP::settings_t::NAME=CTCP::NAME;

CTCP::settings_t::settings_t(net_address const& aParam) :
		FServerAddress(aParam)
{
}

CTCP::settings_t::settings_t(NSHARE::CConfig const& aConf) :
		FSocketSetting(aConf.MChild(socket_setting_t::NAME)),//
		FServerAddress(aConf.MChild(net_address::NAME))
{
}
bool CTCP::settings_t::MIsValid() const
{
	return FSocketSetting.MIsValid()//
			&&FServerAddress.MIsValid()//
			;
}

CConfig CTCP::settings_t::MSerialize() const
{
	CConfig _conf(NAME);
	_conf.MAdd(FServerAddress.MSerialize());
	_conf.MAdd(FSocketSetting.MSerialize());
	return _conf;
}
} //namespace NSHARE

