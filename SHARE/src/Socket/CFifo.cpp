// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
 /*
 * CFifo.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <share/console.h>
#include <share/Pipe.h>
#ifdef _WIN32
#include <share/Socket/CFifoWin32Impl.h>
#else
#include <share/Socket/CFifoPosixImpl.h>
#endif
namespace NSHARE
{
const unsigned CFifo::ATOMIC_FIFO_BUUFER = CFifo::CImpl::ATOMIC_FIFO_BUUFER;
const NSHARE::CText CFifo::NAME="pipe";
CFifo::CFifo(NSHARE::CConfig const& aConf) :
		FImpl(new CImpl(*this))
{
	VLOG(2) << "Construct CFifo. Setting " << aConf << ":" << this;
	setting_t  aVal(aConf);
	LOG_IF(DFATAL,!aVal.MIsValid())<<"Configure for fifo is not valid "<<aConf;
	MSetUP (aVal);
	if (aVal.MIsValid())
		MOpen();
}
CFifo::CFifo(setting_t const& aVal) :
		FImpl(new CImpl(*this))
{
	VLOG(2) << "Construct CFifo. Setting " << aVal << ":" << this;
	MSetUP(aVal);
	if (aVal.MIsValid())
		MOpen();
}
CFifo::~CFifo()
{
	VLOG(2) << "Destruct CFifo: " << this;
	delete FImpl;
}

void CFifo::MSetUP(setting_t const& aPath)
{
	VLOG(2) << "Set up " << aPath << ":" << this;
	FSetting = aPath;
	if (MIsOpen())
	{
		LOG_IF(WARNING,FSetting.FPath!=aPath.FPath)
				<< "To change path, port will to be reopened.";

		MReOpen();
	}
}
CFifo::setting_t const& CFifo::MGetSetting() const
{
	return FSetting;
}
bool CFifo::MOpen()
{
	VLOG(2) << "Open :" << this;
	return FImpl->MOpen();
}
bool CFifo::MReOpen()
{
	VLOG(2) << "ReOpen :" << this;
	if (MIsOpen())
		MClose();
	return MOpen();
}
bool CFifo::MIsOpen() const
{
	return FImpl->MIsOpen();
}
void CFifo::MClose()
{
	FImpl->MClose();
}
bool CFifo::MMakefifo(path_t const& aPath)
{
	return FImpl->MMakefifo(aPath);
}
bool CFifo::MIsMade() const
{
	return FImpl->MIsMade();
}
size_t CFifo::MAvailable() const
{
	return FImpl->MAvailable();
}

ssize_t CFifo::MReceiveData(data_t* aContainer, const float aTime)
{
	CHECK_NOTNULL(aContainer);
	ssize_t _size= FImpl->MReceiveData(aContainer,aTime);

	if(_size>0)FDiagnostic.MRecv(_size);
	return _size;
}


ISocket::sent_state_t CFifo::MSend(const void*  aData, std::size_t aSize)
{
	CHECK_NOTNULL(aData);
	sent_state_t const _is= FImpl->MSend(aData, aSize);
	FDiagnostic.MSend(_is);
	return _is;
}
const CSocket& CFifo::MGetSocket(void) const
{
	return  FImpl->MGetSocket();
}
void CFifo::MFlush(eFlush const& aVal)
{
	FImpl->MFlush(aVal);
}
std::ostream & CFifo::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "Close.";
	aStream << NSHARE::NCONSOLE::eNORMAL;
	return aStream << " Type: Pipe. Parameters: Path=" << FSetting << ".";
}
NSHARE::CConfig CFifo::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FSetting.MSerialize());
	_conf.MAdd("atom",ATOMIC_FIFO_BUUFER);
	_conf.MAdd("open",MIsOpen());
	_conf.MAdd(FDiagnostic.MSerialize());
	return _conf;
}
const CText CFifo::setting_t::FIFO_SERVER_PATH="server_path";
const CText CFifo::setting_t::FIFO_PATH="path";
const CText CFifo::setting_t::FIFO_TYPE="type";
const CText CFifo::setting_t::FIFO_R_ONLY="readonly";
const CText CFifo::setting_t::FIFO_W_ONLY="writeonly";
const CText CFifo::setting_t::FIFO_RW="rw";
CFifo::setting_t::setting_t()
{
	FType=READ_WRITE;
}
CFifo::setting_t::setting_t(NSHARE::CConfig const& aConf)
{
	FType=READ_WRITE;

	aConf.MGetIfSet(FIFO_PATH,FPath);
	aConf.MGetIfSet(FIFO_SERVER_PATH,FServerPath);
	if(FPath.empty() && FServerPath.empty())
	{
		LOG(ERROR)<<"Invalid config for fifo";
	}else
	{
#ifdef _WIN32
	if(FPath.empty() && !FServerPath.empty())
		FPath=FServerPath;
#else
	if(FPath.empty())
		FType=WRITE_ONLY;
	else if(FServerPath.empty())
		FType=READ_ONLY;
	else
		FType=READ_WRITE;
#endif
	CText _name;
	aConf.MGetIfSet(FIFO_TYPE,_name);
	if(_name==FIFO_R_ONLY)
		FType = READ_ONLY;
	else if(_name==FIFO_W_ONLY)
		FType = WRITE_ONLY;
	else
		FType=READ_WRITE;
	}
}
CConfig CFifo::setting_t::MSerialize() const
{
	CConfig _conf("param");
	if (MIsValid())
	{
		_conf.MSet(FIFO_PATH,FPath);
		_conf.MSet(FIFO_TYPE,FType);
	}
	return _conf;
}
} //namespace NSHARE
