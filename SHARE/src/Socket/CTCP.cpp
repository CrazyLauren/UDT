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
#include <Net.h>
#include <Socket/CNetBase.h>
#include <UType/CDenyCopying.h>
#include <UType/CThread.h>
#include <Socket/CTcpClientImpl.h>
#include <console.h>
#if !defined(_WIN32)
#include <arpa/inet.h>                      // htons, htonl
#else
#include <ws2tcpip.h>
#endif
namespace NSHARE
{
//Early there were two implementation. One of was client,the other was server.
//Now In "CTCP" class there is only client  implementation.
//"CTCP::CClientImpl" class  is remained by historical reason.

const NSHARE::CText CTCP::NAME="tcp";
CTCP::events_t::key_t const CTCP::EVENT_CONNECTED="Connect";
CTCP::events_t::key_t const CTCP::EVENT_DISCONNECTED="Disconnect";

CTCP::CTCP(NSHARE::CConfig const& aConf) :
				FClientImpl(new CClientImpl(*this))
{

	FIsWorking = false;
	FIsConnected = false;

	net_address _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid())<<"Configure for tcp is not valid "<<aConf;
	if (_param.MIsValid())
		MOpen(_param);
}
CTCP::CTCP(net_address const& aParam) :
		FClientImpl(new CClientImpl(*this))
{


	FIsWorking = false;
	FIsConnected = false;

	if (aParam.MIsValid())
		MOpen(aParam);
}
CTCP::~CTCP()
{
	MClose();
	delete FClientImpl;
}

bool CTCP::MOpen(net_address const& aAddr, int aFlags)
{

	CNetBase::MSetAddress(aAddr, &FClientImpl->FAddr); //FIXME asserts
	if (aAddr.FIp.MGetConst().empty())
		FClientImpl->FAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK );
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
	if (MIsOpen())
	{
		LOG(WARNING)<< "The Port has been opened already.";
		return false;
	}

	FIsWorking = true;
	CHECK_NOTNULL(FClientImpl);
	return FIsWorking;
}
bool CTCP::MReOpen()
{
	MClose();
	return MOpen();
}
ssize_t CTCP::MReceiveData(data_t * aBuf, float const aTime)
{
	return FClientImpl->MReceiveData(aBuf, aTime);
}
void CTCP::MClose()
{
	FIsWorking = false;
	FClientImpl->MClose();

	usleep(1); //waitfor receive end;
	for (int i = 0; FClientImpl->FIsReceive && (++i < 10);
			NSHARE::usleep(100000))
		VLOG(2) << "Waitfor Close Receive";

}
CTCP::sent_state_t CTCP::MSend(void const* pData, size_t nSize)
{
	return FClientImpl->MSend(pData, nSize);
}
bool CTCP::MIsOpen() const
{
	return FIsWorking;
}
net_address CTCP::MGetSetting() const
{
	return FClientImpl->MGetInitParam();
}
bool CTCP::MGetInitParam(net_address* aParam) const
{
	*aParam = FClientImpl->MGetInitParam();
	return true;
}
const CSocket& CTCP::MGetSocket(void) const
{
	return FClientImpl->FSock;
}
bool CTCP::MIsConnected() const
{
	return FIsConnected;
}
size_t CTCP::MAvailable() const
{
	return FClientImpl->MAvailable(FClientImpl->FSock);
}
std::ostream& CTCP::MPrint(std::ostream & aStream) const
{
	net_address _addr;
	MGetInitParam(&_addr);
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
	{
		net_address _addr;
		MGetInitParam(&_addr);
		_conf.MAdd(_addr.MSerialize());
	}
	_conf.MAdd("con", MIsConnected());
	_conf.MAdd(FClientImpl->FDiagnostic.MSerialize());
	_conf.MAdd("connum", FClientImpl->FConnectionCount);
	_conf.MAdd("eagain", FClientImpl->FAgainError);
	return _conf;
}
std::pair<size_t, size_t> CTCP::MBufSize() const
{
	return std::pair<size_t, size_t>(FClientImpl->MGetSendBufSize(FClientImpl->FSock),FClientImpl->MGetRecvBufSize(FClientImpl->FSock));
}
} //namespace NSHARE

