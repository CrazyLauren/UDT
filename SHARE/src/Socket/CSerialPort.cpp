// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSelectSocket.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/console.h>
#include <SHARE/SerialPort.h>
#ifdef _WIN32
#include <SHARE/Socket/CSPWin32Impl.h>
#else
#include <SHARE/Socket/CSPPosixImp.h>
#endif

namespace NSHARE
{
namespace NSerial
{
const CText port_settings_t::BAUND_RATE = "speed";
const CText ISerial::PATH = "path";
const CText port_settings_t::NOTATION = "notation";

using std::invalid_argument;
const NSHARE::CText CSerialPort::NAME="serial";
CSerialPort::CSerialPort(NSHARE::CConfig const& aConf) :
		FImpl(new CImpl(*this))
{
	port_settings_t _param(aConf);
	LOG_IF(DFATAL,!_param.MIsValid())
			<< "Configure for serial is not valid " << aConf;
	if (_param.MIsValid())
	{
		MSetUP(_param);

		CText _path;
		if (aConf.MGetIfSet(PATH, _path))
		{
			MOpen(_path);
		}
		else
		{
			LOG(DFATAL)<< "path for serial is not valid " << aConf;
		}
	}

}
CSerialPort::CSerialPort(CText const & aPort, port_settings_t const& aSetting) :
		FImpl(new CImpl(*this))
{
	MSetUP(aSetting);
	if (!aPort.empty())
		MOpen(aPort);
}
CSerialPort::~CSerialPort()
{
	delete FImpl;
}
void CSerialPort::MSetUP(port_settings_t const& aType)
{
	FSetting = aType;
	if (MIsOpen())
		FImpl->MReconfigure();
}
port_settings_t const& CSerialPort::MGetSetting() const
{
	return FSetting;
}
bool CSerialPort::MOpen(CText const& aNamePort)
{
	FImpl->MSetPort(aNamePort);
	return MReOpen();
}
bool CSerialPort::MReOpen()
{
	if (MIsOpen())
		MClose();
	return FImpl->MOpen();
}
void CSerialPort::MClose()
{
	FImpl->MClose();
}
bool CSerialPort::MIsOpen() const
{
	return FImpl->MIsOpen();
}
size_t CSerialPort::MAvailable() const
{
	return FImpl->MAvailable();
}
int CSerialPort::MWaitData(float const aTime) const
{
	return FImpl->MWaitData(aTime);
}
ssize_t CSerialPort::MReceiveData(data_t * aData, float const aTime)
{
	ssize_t _size = MReceiveData(aData, 0, aTime);
	if (_size > 0)
		FDiagnostic.MRecv(_size);
	return _size;
}

ssize_t CSerialPort::MReceiveData(data_t *aData, std::size_t aCount,
		float const aTime)
{
	ssize_t _size = FImpl->MReceiveData(aData, aCount, aTime);
	if (_size > 0)
		FDiagnostic.MRecv(_size);
	return _size;
}
CSerialPort::sent_state_t CSerialPort::MSend(void const* aData, std::size_t aSize)
{
	sent_state_t const _is = FImpl->MSend(aData, aSize);
	FDiagnostic.MSend(_is);
	return _is;
}

CText CSerialPort::MGetPort() const
{
	return FImpl->MGetPort();
}
const CSocket& CSerialPort::MGetSocket() const
{
	return FImpl->MGetSocket();
}

void CSerialPort::MSetTimeout(timeouts_t &timeout)
{
	FSetting.FTimeout = timeout;
}

void CSerialPort::MSetBaudRate(eBaudRate const& aBaund)
{
	port_settings_t _set = MGetSetting();
	_set.FBaudRate = aBaund;
	MSetUP(_set);

}
eBaudRate CSerialPort::MGetBaudRate() const
{
	return MGetSetting().FBaudRate;
}
void CSerialPort::MFlush(eFlush const& aVal)
{
	FImpl->MFlush(aVal);
}

void CSerialPort::MSendBreak(int aVal)
{
	FImpl->MSendBreak(aVal);
}

bool CSerialPort::MSignal(eLine const& aLine, bool aVal)
{
	return FImpl->MSignal(aLine, aVal);
}

bool CSerialPort::MLineState(eLine const& aLine) const
{
	return FImpl->MLineState(aLine);
}

bool CSerialPort::MWaitForLineChanged()
{
	return FImpl->MWaitForLineChanged();
}

std::ostream & CSerialPort::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "Close.";
	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << " Type: Serial duplex. ";
	aStream << MGetPort() << ". ";
	aStream << FSetting;
	return aStream;
}
NSHARE::CConfig CSerialPort::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd("port",MGetPort());
	_conf.MAdd(FSetting.MSerialize());

	_conf.MAdd("open",MIsOpen());
	_conf.MAdd(FDiagnostic.MSerialize());
	return _conf;
}
}
}
