// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CFIFOServer.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <console.h>
#include <Pipe.h>

#ifdef _WIN32
#include <Socket/CFIFOServerWin32Impl.h>
#else
#include <Socket/CFIFOServerPosixImpl.h>
#endif

namespace NSHARE
{
const NSHARE::CText CFIFOServer::NAME="pipe_server";
CFIFOServer::CFIFOServer(NSHARE::CConfig const& aConf) :
				FImpl(new CImpl(*this))
{
	VLOG(2) << "Construct CFIFOServer. Setting " << aConf << ":" << this;
	server_setting_t _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid())<<"Configure for fifo is not valid "<<aConf;
	MSetUP (_param);
	if (_param.MIsValid())
		MReOpen();
}
CFIFOServer::CFIFOServer(server_setting_t const& aVal) :
		FImpl(new CImpl(*this))
{
	VLOG(2) << "Construct CFIFOServer. Setting " << aVal << ":" << this;
	MSetUP(aVal);
	if (aVal.MIsValid())
		MReOpen();
}

CFIFOServer::~CFIFOServer()
{
	VLOG(2) << "Destruct CFIFOServer: " << this;
	delete FImpl;
}

void CFIFOServer::MSetUP(server_setting_t const& aPath)
{
	VLOG(2) << "Set up " << aPath << ":" << this;
	FSetting = aPath;
	if (MIsOpen())
	{
		LOG_IF(WARNING,FSetting.FPath!=aPath.FPath)
															<< "To change path, port will to be reopened.";

		FImpl->MReconfigure();
	}
}
CFIFOServer::server_setting_t const& CFIFOServer::MGetSetting() const
{
	return FSetting;
}

bool CFIFOServer::MOpen(path_t const& aClientPath)
{
	VLOG(2) << "Open " << aClientPath << ":" << this;
	LOG_IF(WARNING,MIsClient(aClientPath)) << "Ivalide client_t: "
													<< aClientPath;

	FSetting.FClients.push_back(aClientPath);
	if (MIsOpen())
		FImpl->MReconfigure();
	else
		FImpl->MAddNewClient(aClientPath);

	return true;
}

bool CFIFOServer::MReOpen()
{
	VLOG(2) << "ReOpen :" << this;
	if (MIsOpen())
		MClose();
	return FImpl->MOpen();
}
bool CFIFOServer::MIsOpen() const
{
	return FImpl->MIsOpen();
}

void CFIFOServer::MClose()
{
	FImpl->MClose();
}
void CFIFOServer::MClose(path_t const& aClientPath)
{
	VLOG(2) << "Close :" << aClientPath << ":" << this;
	FImpl->MClose(aClientPath);
	clients_t::iterator _it = FSetting.FClients.begin();
	for (; _it != FSetting.FClients.end(); ++_it)
		if (*_it == aClientPath)
		{
			FSetting.FClients.erase(_it);
			return;
		}
	LOG(WARNING)<< "Fifo: " << aClientPath << " is not exist";
}
size_t CFIFOServer::MAvailable() const
{

	return FImpl->MAvailable();
}
ssize_t CFIFOServer::MReceiveData(data_t * aData, float const aTime,
		recvs_from_t *aFrom)
{
	recvs_from_t _to;
	recvs_from_t* _pto = aFrom ? &_to : NULL;

	ssize_t _size = FImpl->MReceiveData(aData, aTime, _pto);

	if(_size>0)FDiagnostic.MRecv(_size);
	/*	if (_size && _pto)
	 *aFrom = sMGetslave_t(_to);*/
	//TODO not implemented
	return _size;
}
CFIFOServer::sent_state_t CFIFOServer::MSend(void const* aData,
		std::size_t aSize)
{
	LOG(INFO)<<"MSend data To All Clients.:"<<this;
	CHECK_NOTNULL(aData);
	clients_t::const_iterator _it = MGetSetting().FClients.begin();
	int _val=1;
	for (; _it != MGetSetting().FClients.end(); ++_it)
	//if at least one call  return false - result ==false;
	_val*=FImpl->MSend(aData, aSize,*_it).MIs()?1:0;
	return sent_state_t(_val>0?E_SENDED:E_ERROR,aSize);
}
CFIFOServer::sent_state_t CFIFOServer::MSend(const data_t& aBuf, CFifo::path_t const& aTo)
{
	if (aBuf.empty())
		return sent_state_t(E_ERROR,0);
	return MSend(aBuf.ptr_const(), aBuf.size(), aTo);
}
CFIFOServer::sent_state_t CFIFOServer::MSend(void const*  aData, std::size_t aSize,
		CFifo::path_t const& aTo)
{
	VLOG(1) << "MSend data:" << aTo << ":" << this;
	CHECK_NOTNULL(aData);
	sent_state_t _is= FImpl->MSend(aData, aSize, aTo);
	FDiagnostic.MSend(_is.FBytes);
	return _is;
}

CFIFOServer::sent_state_t CFIFOServer::MSend(void const*  aData, std::size_t aSize,
		NSHARE::CConfig const& aTo)
{
	CHECK_NOTNULL(aData);
	sent_state_t _is= FImpl->MSend(aData, aSize, aTo);
	FDiagnostic.MSend(_is.FBytes);
	return _is;
}
const CSocket& CFIFOServer::MGetSocket() const
{
	return FImpl->MGetSocket();
}
void CFIFOServer::MFlush(eFlush const&aVal)
{
	FImpl->MFlush(aVal);
}

bool CFIFOServer::MIsClient(path_t const& aClientPath) const
{
	clients_t::const_iterator _it = MGetSetting().FClients.begin();
	for (; _it != MGetSetting().FClients.end(); ++_it)
		if (*_it == aClientPath)
			return true;
	return false;
}
std::ostream & CFIFOServer::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "Close.";
	return aStream << " Type: Pipe. Parameters: " <<FSetting << ".";
}
NSHARE::CConfig CFIFOServer::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(MGetSetting().MSerialize());
	FImpl->MSerialize(_conf);
	_conf.MAdd("atom",ATOMIC_FIFO_BUUFER);
	_conf.MAdd(FDiagnostic.MSerialize());
	return _conf;
}
const CText CFIFOServer::client_t::PATH = "path";
const CText CFIFOServer::client_t::TIME = "time";
CFIFOServer::client_t::client_t() :
		FTime(0)
{
	;
}
CFIFOServer::client_t::client_t(NSHARE::CConfig const& aConf)
{
	aConf.MGetIfSet(PATH, FClient);
	aConf.MGetIfSet(TIME, FTime);
}

NSHARE::CConfig CFIFOServer::client_t::MSerialize() const
{
	CConfig _conf;
	if (MIsValid())
	{
		_conf.MSet(PATH, FClient);
		_conf.MSet("time", FTime);
	}
	return _conf;

}
const CText CFIFOServer::server_setting_t::PATH = "path";
const CText CFIFOServer::server_setting_t::CLIENT = "client";
CFIFOServer::server_setting_t::server_setting_t()
{
	;
}
CFIFOServer::server_setting_t::server_setting_t(NSHARE::CConfig const& aConf)
{
	if (aConf.MGetIfSet(PATH, FPath))
	{
		ConfigSet _set=aConf.MChildren(CLIENT);
		ConfigSet::const_iterator _it=_set.begin();
		for(;_it!=_set.end();++_it)
		{
			VLOG(2)<<"Push client "<<_it->MValue();
			FClients.push_back(_it->MValue());
		}
	}
}
NSHARE::CConfig CFIFOServer::server_setting_t::MSerialize() const
{
	CConfig _conf("param");
	if (MIsValid())
	{
		_conf.MSet(PATH, FPath);
		ConfigSet _clients;
		clients_t::const_iterator _it=FClients.begin();
		for(;_it!=FClients.end();++_it)
		{
			CConfig _client(CLIENT,*_it);
			_clients.push_back(_client);
		}
		if(!_clients.empty())
			_conf.MAdd(_clients);
	}
	return _conf;

}
} //namespace
