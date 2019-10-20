// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSocketFile.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 08.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#include <deftype>
#include <share/Socket/diagnostic_io_t.h>
#include <share/Socket/ISocket.h>
#include <share/Socket/CSocket.h>
#include <share/socket.h>
#include <share/console.h>
#include <share/Socket/CSocketFile.h>

namespace NSHARE
{
const NSHARE::CText CSocketFile::NAME="file";
CSocketFile::CSocketFile(NSHARE::CConfig const& aConf)
{
	FNextTime = 0;
	param_t _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid())<<"Configure for file socket is not valid "<<aConf;
	MOpen(_param);
}
CSocketFile::CSocketFile(const param_t& aParam)
{
	FNextTime = 0;
	if(aParam.MIsValid())
		MOpen(aParam);
}

CSocketFile::~CSocketFile()
{
	VLOG(2)<<"Destruct socket file";
	MClose();
}

size_t CSocketFile::MAvailable() const
{
	switch (FParam.FDirect)
	{
	case eIn:
	{
		std::ifstream::pos_type _pos = FStream.tellg();
		FStream.seekg(0, std::ios::end);
		long long int _avalaible = FStream.tellg() - _pos;
		FStream.seekg(_pos);
		VLOG(2) << _avalaible << " bytes available from file "
							<< FParam.FPath;
		if (_avalaible >= 0)
			return static_cast<size_t>(_avalaible);
		return 0;
	}
		break;
	case eOut:
	{
		LOG(ERROR)<<"Invalid available function for out";
		return 0;
	}
	break;
}
return 0;
}
bool CSocketFile::MOpen()
{
	FNextTime = NSHARE::get_time();
	VLOG(2) << "Open Time  " << FNextTime << " type " << FParam.FDirect
						<< " path " << FParam.FPath;
	switch (FParam.FDirect)
	{
	case eIn:
		FStream.open(FParam.FPath.c_str(),
				std::ios_base::in | std::ios_base::binary);
		break;
	case eOut:
		FStream.open(FParam.FPath.c_str(),
				std::ios_base::out | std::ios_base::trunc
						| std::ios_base::binary);
		break;

	}
	FStream.seekg(0, std::ios::beg);
	return FStream.is_open();
}
bool CSocketFile::MReOpen()
{
	if (MIsOpen())
	{		
		return false;
	}
	MOpen();
	if (!MIsOpen())
		return false;
	return true;
}
bool CSocketFile::MOpen(const param_t& aParam)
{
	VLOG(2) << "Open socket file  " << aParam;
	FParam = aParam;
	return MOpen();
}
void CSocketFile::MClose()
{
	VLOG(2) << "close socket file  " << FParam.FPath;
	if (MIsOpen())
		FStream.close();
}
bool CSocketFile::MIsOpen() const
{
	return FStream.is_open();
}
ssize_t CSocketFile::MReceiveData(data_t* aBuf, const float aTime)
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "***ERROR***:The file "<<FParam.FPath<<" is not exist.";
		return 0; //FIXME throw
	}
	if (FParam.FDirect == eOut)
	{
		LOG(ERROR)<< "The type of socket is out. Cannot read data.";
		std::cerr << "The type of socket is out. Cannot read data" << std::endl;
		return 0;
	}
	for (HANG_INIT;(FNextTime-NSHARE::get_time())>0;HANG_CHECK)
	{
		usleep(100);
	}
	double const _r_time = ((double) FParam.FTime) / 1000.0;
	FNextTime = NSHARE::get_time()+_r_time;

	const size_t _size = std::min(MAvailable(), FParam.FSize);
	const size_t _befor = aBuf->size();
	aBuf->resize(_befor + _size);

	VLOG(2) << "Available " << _size << " bytes";
	VLOG_IF(1,!_size) << "No data file socket ";
	if (!_size) return 0;
	aBuf->resize(_befor + _size);
	FStream.read((char*) &aBuf->front() + (aBuf->size() - _size), _size);
	if (!FStream)
	{
		LOG(ERROR)<< "Only " << FStream.gcount() << " could be read";
		std::cout << "error: only " << FStream.gcount() << " could be read";
		return -1;
	}
	FDiagnostic.MRecv(aBuf->size());
	return aBuf->size();
}
const CSocket& CSocketFile::MGetSocket(void) const
{
	return CSocket::sMNullSocket();
}
CSocketFile::sent_state_t CSocketFile::MSend(const void*  aData, std::size_t aSize)
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "***ERROR***:The file "<<FParam.FPath<<" is not exist.";
		return sent_state_t(sent_state_t::E_NOT_OPENED,0);
	}
	if (FParam.FDirect == eIn)
	{
		LOG(ERROR)<< "The type of socket is in. Cannot write data.";
		std::cerr << "The type of socket is in. Cannot write data" << std::endl;
		return sent_state_t(sent_state_t::E_INVALID_VALUE,0);
	}
	VLOG(1) << "Send " << aData << " size=" << aSize << " to " << FParam.FPath;
	FStream.write((const char* const ) aData, aSize);
	FStream.flush();
	const bool _is= FStream.is_open();

	sent_state_t const _rval(_is?sent_state_t::E_SENDED:sent_state_t::E_ERROR,aSize);
	FDiagnostic.MSend(_rval);
	return _rval;
}
CSocketFile::param_t const & CSocketFile::MGetSetting() const
{
	return FParam;
}
NSHARE::CConfig CSocketFile::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FParam.MSerialize());
	_conf.MAdd("time",FNextTime);
	_conf.MAdd("open",MIsOpen());
	_conf.MAdd(FDiagnostic.MSerialize());
	return _conf;
}
std::ostream & CSocketFile::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "Close.";
	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << " Type: File. Param: " << FParam;
	return aStream;
}
std::ostream& operator<<(std::ostream & aStream,
		CSocketFile::param_t const& aParam)
{
	switch (aParam.FDirect)
	{
	case CSocketFile::eIn:
		aStream << "D=In; Path=" << aParam.FPath << "; BlockSize="
				<< aParam.FSize << "; Time=" << aParam.FTime << ".";

		break;
	case CSocketFile::eOut:
		aStream << "D=Out; Path=" << aParam.FPath << ".";

		break;

	}
	return aStream;
}
const NSHARE::CText CSocketFileDuplex::NAME="filed";
CSocketFileDuplex::CSocketFileDuplex(NSHARE::CSocketFile* aIn,
		NSHARE::CSocketFile* aOut)
{
	FIn = aIn;
	FOut = aOut;
}
CSocketFileDuplex::~CSocketFileDuplex()
{
	delete FIn;
	delete FOut;
}
CSocketFileDuplex::sent_state_t CSocketFileDuplex::MSend(const void*  aData, std::size_t aSize)
{
	return FOut->MSend(aData, aSize);
}
ssize_t CSocketFileDuplex::MReceiveData(data_t* aData, const float aTime)
{
	VLOG(2)<<"Receive data from duplex file.";
	return FIn->MReceiveData(aData, aTime);
}
void CSocketFileDuplex::MClose()
{
	FIn->MClose();
	FOut->MClose();
}
bool CSocketFileDuplex::MIsOpen() const
{
	return FIn->MIsOpen() && FOut->MIsOpen();
}
bool CSocketFileDuplex::MReOpen()
{
	if (FIn->MReOpen() && FOut->MReOpen())
		return true;
	MClose();
	return false;
}
const CSocket& CSocketFileDuplex::MGetSocket(void) const
{
	static CSocket _sock;
	return _sock;
}
NSHARE::CConfig CSocketFileDuplex::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	if(FIn)
		_conf.MAdd(FIn->MSerialize());
	if(FOut)
		_conf.MAdd(FOut->MSerialize());


	return _conf;
}
NSHARE::CConfig CSocketFileDuplex::MSettings(void) const
{
	NSHARE::CConfig _conf(NAME);
	if(FIn)
		_conf.MAdd(FIn->MSettings());
	if(FOut)
		_conf.MAdd(FOut->MSettings());
	return _conf;
}
std::ostream & CSocketFileDuplex::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "The channel is opened.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "The channel is close.";
	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << "Type: File duplex. " << FIn->FParam << ". " << FOut->FParam;
	return aStream;
}
size_t CSocketFileDuplex::MAvailable() const
{
	if(FIn)
		return FIn->MAvailable();	
	return 0;
}

const CText CSocketFile::param_t::FILE_PATH = "path";
const CText CSocketFile::param_t::FILE_SIZE = "size";
const CText CSocketFile::param_t::FILE_TIME = "period";
const CText CSocketFile::param_t::FILE_DIRECT = "direction";
const CText CSocketFile::param_t::FILE_DIRECT_IN = "in";
const CText CSocketFile::param_t::FILE_DIRECT_OUT = "out";

CSocketFile::param_t::param_t()
{
	FTime=FSize = 0;
	FDirect=eOut;
}
CSocketFile::param_t::param_t(NSHARE::CConfig const& aConf)
{
	FTime=FSize = 0;
	FDirect=eOut;
	if (aConf.MGetIfSet(FILE_PATH, FPath))
	{
		CText _dir;
		aConf.MGetIfSet(FILE_DIRECT, _dir);
		LOG_IF(DFATAL,_dir.empty())<<"Invalid param "<<FILE_DIRECT<<" Conf:"<<aConf;
		if (!_dir.empty())
		{
			if (_dir == FILE_DIRECT_IN)
			{
				FDirect = eIn;
				aConf.MGetIfSet(FILE_SIZE, FSize);
				aConf.MGetIfSet(FILE_TIME, FTime);
			}
			else if (_dir == FILE_DIRECT_OUT)
			{
				FDirect = eOut;
			}
			else
				LOG(DFATAL)<<"Invalid param "<<_dir;

		}
	}
}
bool CSocketFile::param_t::MIsValid() const
{
	return !FPath.empty();
}
CConfig CSocketFile::param_t::MSerialize() const
{
	CConfig _conf("param");
	if (MIsValid())
	{
		_conf.MSet(FILE_PATH, FPath);
		_conf.MSet(FILE_DIRECT, FDirect);
		if(FDirect==eIn)
		{
			_conf.MSet(FILE_SIZE, FSize);
			_conf.MSet(FILE_TIME, FTime);
		}
	}
	return _conf;
}
}
