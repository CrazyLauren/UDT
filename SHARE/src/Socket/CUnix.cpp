/*
 * CUnix.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#if defined (__QNX__) ||defined (unix)||defined (__linux__)
#include <sys/un.h>
#include <deftype>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <Socket/ISocket.h>
#include <Socket/diagnostic_io_t.h>
#include <Socket/CUnix.h>
#include <console.h>


#define BUFFER_SIZE 32768
namespace NSHARE
{
const NSHARE::CText CUnixDGRAM::NAME="unix_dgram";

class CUnixDGRAM::CUnixDGRAM_impl
{
public:
	struct sockaddr_un FAddrFrom;
	struct sockaddr_un FAddrTo;
};
CUnixDGRAM::CUnixDGRAM(NSHARE::CConfig const& aConf) :
		 Impl(new CUnixDGRAM_impl)
{
	param_t _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid())<<"Configure for unix dgram is not valid "<<aConf;
	MOpen(aConf);
}
CUnixDGRAM::CUnixDGRAM(param_t const& aParam) :
		 Impl(new CUnixDGRAM_impl)
{
	if(aParam.MIsValid())
		MOpen(aParam);
}

CUnixDGRAM::~CUnixDGRAM()
{
	MClose();

}

void CUnixDGRAM::MClose()
{
	if (MIsOpenFrom())
	{
		FSockFrom.MClose();
		//close(FSockFrom);
		if (!MGetPathFrom().empty())
			unlink(MGetPathFrom().c_str());
	}
	if (MIsOpenTo())
		FSockTo.MClose();//close(FSockTo);
}
inline void CUnixDGRAM::MSetAddress()
{
	if (MIsOpenFrom())
	{
		memset(&Impl->FAddrFrom, 0, sizeof(Impl->FAddrFrom));
		Impl->FAddrFrom.sun_family = AF_LOCAL;
#ifdef __QNX__
		Impl->FAddrFrom.sun_len = SUN_LEN(&Impl->FAddrFrom);
#endif
		strncpy(Impl->FAddrFrom.sun_path, MGetPathFrom().c_str(),
				MGetPathFrom().size());
	}
	if (MIsOpenTo())
	{
		memset(&Impl->FAddrTo, 0, sizeof(Impl->FAddrTo));
		Impl->FAddrTo.sun_family = AF_LOCAL;
#ifdef __QNX__
		Impl->FAddrTo.sun_len = SUN_LEN(&Impl->FAddrTo);
#endif
		strncpy(Impl->FAddrTo.sun_path, MGetPathTo().c_str(),
				MGetPathTo().size());
	}
}
inline CSocket CUnixDGRAM::MNewSocket()
{
	return socket(AF_LOCAL, SOCK_DGRAM, 0);
}

bool CUnixDGRAM::MOpen(const param_t& aParam)
{
	MSetPath(aParam);
	return MOpen();
}
size_t CUnixDGRAM::MAvailable() const
{
	int count_buf = 0;
	int _val = ioctl(MGetSocket().MGet(), FIONREAD, &count_buf);
	LOG_IF(ERROR,_val<0) << strerror(errno);
	(void) _val;
	CHECK_GE(count_buf, 0);
	VLOG(2) << count_buf << " bytes available for reading from " << MGetSocket();
	return count_buf;
}
bool CUnixDGRAM::MOpen()
{
	if (MIsOpen())
		return false;

	if (!FParam.FFrom.empty())
		FSockFrom = MNewSocket();

	if (!FParam.FTo.empty())
		FSockTo = MNewSocket();

	if (!MIsOpen())
	{
		return false;
	}

	MSetAddress();
	if (MIsOpenFrom())
		MBindSocket(FSockFrom, Impl->FAddrFrom);
	if (!MIsOpen())
	{
		MClose();
		return false;
	}
	return true;
}
int CUnixDGRAM::MBindSocket(CSocket const &aSocket,
		struct sockaddr_un const&aAddr)
{
	unlink(aAddr.sun_path);
	return bind(aSocket.MGet(), (struct sockaddr *) &aAddr, sizeof(aAddr));
}
bool CUnixDGRAM::MReOpen()
{
	MClose();
	return MOpen();
}

bool CUnixDGRAM::MIsPathExist()
{
	return MGetPathFrom() != "";
}

bool CUnixDGRAM::MIsOpen() const
{
	return MIsOpenFrom() || MIsOpenTo();
}

bool CUnixDGRAM::MIsOpenFrom() const
{
	return FSockFrom.MIsValid();
}
bool CUnixDGRAM::MIsOpenTo() const
{
	return FSockTo.MIsValid();
}

void CUnixDGRAM::MSetPath(const param_t& aParam)
{
	FParam= aParam;
}

const CText& CUnixDGRAM::MGetPathFrom() const
{
	return FParam.FFrom;
}
const CText& CUnixDGRAM::MGetPathTo() const
{
	return FParam.FTo;
}

CUnixDGRAM::sent_state_t CUnixDGRAM::MSend(const void* const aData, std::size_t aSize)
{
	if (!MIsOpenTo())
		return sent_state_t(E_NOT_OPENED,0);


	bool _is= sendto(FSockTo, aData, aSize, 0, (struct sockaddr *) &Impl->FAddrTo,
			sizeof(Impl->FAddrTo)) > 0;
	if(_is)
		FDiagnostic.MSend(aSize);
	return _is?sent_state_t(E_SENDED,aSize):sent_state_t(E_ERROR,0);
}
ssize_t CUnixDGRAM::MReceiveData(data_t* aBuf, const float aTime)
{
	if (!MIsOpenFrom())
		return -1;
	uint8_t _buffer[BUFFER_SIZE];
	int _recvd = recvfrom(MGetSocket().MGet(), _buffer, BUFFER_SIZE, 0, NULL, NULL);
	if (_recvd > 0)
	{
		const size_t _befor = aBuf->size();
		aBuf->resize(_befor + _recvd);
		memcpy((data_t::value_type*)aBuf->ptr()+_befor, _buffer, _recvd);
		FDiagnostic.MRecv(_recvd);
	}
	return _recvd;
}

const CSocket& CUnixDGRAM::MGetSocket(void) const
{
	return FSockFrom;
}
NSHARE::CConfig CUnixDGRAM::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FParam.MSerialize());
	_conf.MAdd("open", MIsOpen());
	_conf.MAdd(FDiagnostic.MSerialize());
	return _conf;
}
std::ostream & CUnixDGRAM::MPrint(std::ostream & aStream) const
{
	aStream << "Type: Unix DGRAM. Parameters: From=" << MGetPathFrom();

	if (MIsOpenFrom())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << " opened";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << " closed";
	aStream << NSHARE::NCONSOLE::eNORMAL;

	aStream << "; To=" << MGetPathTo();
	if (MIsOpenTo())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << " opened";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << " closed";
	aStream << NSHARE::NCONSOLE::eNORMAL;

	aStream << ".";
	return aStream;
}
const CText CUnixDGRAM::param_t::UNIX_FROM="from";
const CText CUnixDGRAM::param_t::UNIX_TO="to";

CUnixDGRAM::param_t::param_t()
{
}
CUnixDGRAM::param_t::param_t(NSHARE::CConfig const& aConf)
{

	aConf.MGetIfSet(UNIX_FROM,FFrom);
	aConf.MGetIfSet(UNIX_TO,FTo);
}
bool CUnixDGRAM::param_t::MIsValid() const
{
	return !FFrom.empty() || !FTo.empty();
}
CConfig CUnixDGRAM::param_t::MSerialize() const
{
	CConfig _conf("param");
	if (MIsValid())
	{
		_conf.MSet(UNIX_FROM,FFrom);
		_conf.MSet(UNIX_TO,FTo);
	}
	return _conf;
}
}
#endif//#if defined (__QNX__) ||defined (unix)
