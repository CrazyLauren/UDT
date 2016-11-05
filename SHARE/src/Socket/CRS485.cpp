/*
 * CRS485.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 22.03.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <SerialPort.h>
#include <string.h>
#include <console.h>
#include <share_trace.h>
#include <print_socket_error.h>
#define TIME_OF_WAITING_FOR_RECEIVING 0.25 //10 ms
namespace NSHARE
{
namespace NSerial
{
CRS485::CRS485(NSHARE::CConfig const& aConf)
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
			 CRS485::MOpen(_path);
		}
		else
		{
			LOG(DFATAL)<< "path for serial is not valid " << aConf;
		}
	}
}
CRS485::CRS485()
{
	//MPortToWrite(false);
}

CRS485::~CRS485()
{
}

CRS485::sent_state_t CRS485::MSendWithEcho(const void* const aData, std::size_t aSize)
{
//	FSendBuffer.insert(FSendBuffer.end(),
//			reinterpret_cast<const data_t::value_type* const >(aData),
//			reinterpret_cast<const data_t::value_type* const >(aData) + aSize);
	return MSendWithDTR(aData, aSize);
}

bool CRS485::MIsSendingDataCorrectly(const void* const aPData,
		std::size_t aSize)
{
	data_t _received;
	if (MReceiveData(&_received, TIME_OF_WAITING_FOR_RECEIVING) == 0)
	{
		LOG(ERROR)<< "***ERROR***:Port RS485:" << print_socket_error();
		return false;
	}
	data_t _sended(NULL, (const uint8_t*) (aPData),
			(const uint8_t*) (aPData) + aSize);
	if (_received != _sended)
	{
		LOG(ERROR)<< "***ERROR***:Port RS485: sended!=received\n";
		return false;
	}
	return true;
}

CRS485::sent_state_t CRS485::MSend(const void* aData, std::size_t aByte)
{
	if (MIsEcho())
		return MSendWithEcho(aData, aByte);
	else
		return MSendWithDTR(aData, aByte);
}
CRS485::sent_state_t CRS485::MSendWithDTR(const void* const aData, std::size_t aByte)
{
	MPortToWrite(true);
	MFlush(FLUSH_IN);
	sent_state_t _is = CSerialPort::MSend(aData, aByte);
	MFlush(FLUSH_OUT);
	MPortToWrite(false);
	return _is;
}

bool CRS485::MIsEcho()
{
	return true;
}

void CRS485::MEraseEcho(data_t& _received)
{
//	data_t::reverse_iterator _last = _received.rbegin();
//	data_t::reverse_iterator _it = FSendBuffer.rbegin();
//	for (; _it != FSendBuffer.rend() && _last != _received.rend(); ++_it, ++_last)
//	{
//		if (*_last != *_it)
//			break;
//	}
//	_received.erase(_received.rbegin(), _last);
//	if (_received.empty())
//		FSendBuffer.erase(FSendBuffer.rbegin(), _it);
//	else
	FSendBuffer.clear();

//_received.erase(_received.begin(), _received.begin() + FSizeSendData);
}

ssize_t CRS485::MReceiveData(data_t* aContainer, const float aTime)
{
	MPortToWrite(false);
	{
		data_t _received;
		for (; _received.empty();)
		{
			if (CSerialPort::MReceiveData(&_received, aTime) == 0)
				return 0;

//			if (!FSendBuffer.empty())
//			{
//				MEraseEcho(_received);
//				continue;
//			}
//			else
//				break;
		}
		aContainer->insert(aContainer->end(), _received.begin(),
				_received.end());
		return _received.size();
	}
}
//fixme reopen тоже прописать towrite=false
bool CRS485::MOpen(CText const& aNamePort)
{
	bool _val = CSerialPort::MOpen(aNamePort);
	if (_val)
		MPortToWrite(false);
	return _val;
}
void CRS485::MPortToWrite(bool aVal)
{
	for (HANG_INIT; !MSignal(DTR, aVal);HANG_CHECK)
	;
	for (HANG_INIT; !MSignal(RTS, aVal);HANG_CHECK)
	;
}
std::ostream& CRS485::MPrint(std::ostream & aStream) const
{
	if (MIsOpen())
		aStream << NSHARE::NCONSOLE::eFG_GREEN << "Opened.";
	else
		aStream << NSHARE::NCONSOLE::eFG_RED << "Close.";
	aStream << NSHARE::NCONSOLE::eNORMAL;
	aStream << " Type: RS485. ";
	CSerialPort::MPrint(aStream);
	return aStream;
}
} //namespace NSerial
} //namespace NSHARE

