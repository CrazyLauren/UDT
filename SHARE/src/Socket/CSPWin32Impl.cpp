// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSPWin32Impl.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifdef _WIN32
#include <deftype>
#include <SerialPort.h>
#include <Socket/CSPWin32Impl.h>

namespace NSHARE
{
namespace NSerial
{
using std::wstring;
using std::stringstream;
using std::invalid_argument;

inline wstring prefix_port(const wstring &input)
{
	static wstring _prefix = L"\\\\.\\";
	if (input.compare(_prefix) != 0)
	{
		return _prefix + input;
	}
	return input;
}
CSerialPort::CImpl::CImpl(CSerialPort& aThis) :
		FThis(aThis), FFd(0), FIsOpen(false)
{
	memset(&FOReader, 0, sizeof(FOReader));
	FOReader.hEvent = INVALID_HANDLE_VALUE;
	memset(&FOWriter, 0, sizeof(FOWriter));
	FOWriter.hEvent = INVALID_HANDLE_VALUE;


}

CSerialPort::CImpl::~CImpl()
{
	MClose();
}
inline bool CSerialPort::CImpl::MIsHandleValid()
{
	return FFd != INVALID_HANDLE_VALUE ;
}
void CSerialPort::CImpl::MClose()
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port has already been close.";
		return;
	}
	if (MIsHandleValid())
	{
		if (CloseHandle(FFd))
		LOG(DFATAL) << "Error while closing serial port: "
		<< GetLastError();
		else
		FFd = INVALID_HANDLE_VALUE;
	}
	else
	LOG(WARNING) << "Port is open but Handle is invaid";
	if(FOReader.hEvent!=INVALID_HANDLE_VALUE)CloseHandle(FOReader.hEvent);
	if(FOWriter.hEvent!=INVALID_HANDLE_VALUE)CloseHandle(FOWriter.hEvent);
	VLOG(1) << MGetPort()<<" port is closed.";
	FIsOpen = false;

}
bool CSerialPort::CImpl::MOpen()
{
	if (FPort.empty())
	{
		LOG(DFATAL)<< "Empty port is invalid.";
		return false;
	}
	if (MIsOpen())
	{
		LOG(DFATAL) << "Serial port already open.";
		return false;
	}

	wstring port_with_prefix = prefix_port(FPort);
	LPCWSTR lp_port = port_with_prefix.c_str();
	FFd = CreateFileW(lp_port, GENERIC_READ | GENERIC_WRITE, 0, 0,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
	VLOG(1) << MGetPort()<<" port is opened. Handle:"<<FFd;
	if (FFd == INVALID_HANDLE_VALUE)
	{
		DWORD _err = GetLastError();
		switch (_err)
		{
			case ERROR_FILE_NOT_FOUND:
			LOG(DFATAL) << "Specified port, " << MGetPort()
			<< ", does not exist.";
			break;
			default:
			LOG(DFATAL) << "Unknown error opening the serial port: "
			<< _err;
		}

		return false;
	}
	FOReader.hEvent =CreateEvent(NULL, TRUE, FALSE, NULL);
	if(FOReader.hEvent==INVALID_HANDLE_VALUE)
	{
		DWORD _err = GetLastError();
		switch (_err)
		{
			case ERROR_FILE_NOT_FOUND:
			LOG(DFATAL) << "Specified port, " << MGetPort()
			<< ", does not exist.";
			break;
			default:
			LOG(DFATAL) << "Unknown error opening the serial port: "
			<< _err;
		}
		CloseHandle(FFd);
		return false;
	}
	FOWriter.hEvent =CreateEvent(NULL, TRUE, FALSE, NULL);
	if(FOWriter.hEvent==INVALID_HANDLE_VALUE)
	{
		DWORD _err = GetLastError();
		switch (_err)
		{
			case ERROR_FILE_NOT_FOUND:
			LOG(DFATAL) << "Specified port, " << MGetPort()
			<< ", does not exist.";
			break;
			default:
			LOG(DFATAL) << "Unknown error opening the serial port: "
			<< _err;
		}
		CloseHandle(FFd);
		CloseHandle(FOReader.hEvent);
		return false;
	}
	FIsOpen=true;
	if(!MReconfigure())
	{
		MClose();
	}
	return FIsOpen;
}
bool CSerialPort::CImpl::MReconfigure()
{
	if (FFd == INVALID_HANDLE_VALUE)
	{
		LOG(DFATAL)<< "Invalid file descriptor.";
		return false;
	}

	DCB dcbSerialParams;
	memset(&dcbSerialParams, 0, sizeof(dcbSerialParams));

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(FFd, &dcbSerialParams))
	{
		LOG(DFATAL) << "Error getting the serial port state.";
		return false;
	}
	port_settings_t _setting = FThis.MGetSetting();
	// setup baud rate
	switch (_setting.FBaudRate)
	{
#ifdef CBR_0
		case CBR_0:
#endif
#ifdef CBR_50
		case CBR_50:
#endif
#ifdef CBR_75
		case CBR_75:
#endif
#ifdef CBR_110
		case 110:
#endif
#ifdef CBR_134
		case CBR_134:
#endif
#ifdef CBR_150
		case CBR_150:
#endif
#ifdef CBR_200
		case CBR_200
#endif
#ifdef CBR_300
		case CBR_300:
#endif
#ifdef CBR_600
		case CBR_600:
#endif
#ifdef CBR_1200
		case CBR_1200:
#endif
#ifdef CBR_1800
		case CBR_1800:
#endif
#ifdef CBR_2400
		case CBR_2400:
#endif
#ifdef CBR_4800
		case CBR_4800:
#endif
#ifdef CBR_7200
		case CBR_7200:
#endif
#ifdef CBR_9600
		case CBR_9600:
#endif
#ifdef CBR_14400
		case CBR_14400:
#endif
#ifdef CBR_19200
		case CBR_19200:
#endif
#ifdef CBR_28800
		case CBR_28800:
#endif
#ifdef CBR_57600
		case CBR_57600:
#endif
#ifdef CBR_76800
		case 76800:
#endif
#ifdef CBR_38400
		case CBR_38400:
#endif
#ifdef CBR_115200
		case CBR_115200:
#endif
#ifdef CBR_128000
		case CBR_128000:
#endif
#ifdef CBR_153600
		case CBR_153600:
#endif
#ifdef CBR_230400
		case CBR_230400:
#endif
#ifdef CBR_256000
		case CBR_256000:
#endif
#ifdef CBR_460800
		case CBR_460800:
#endif
#ifdef CBR_921600
		case CBR_921600:
#endif
		dcbSerialParams.BaudRate = _setting.FBaudRate;
		VLOG(1) << MGetPort()<<" port baud "<<_setting.FBaudRate;
		break;
		default:
		LOG(DFATAL) << "Baud rate is invalid";
		dcbSerialParams.BaudRate = _setting.FBaudRate;
		break;
	}
	switch (_setting.FByteSize)
	{
		case DATA_5:
		case DATA_6:
		case DATA_7:
		case DATA_8:
		dcbSerialParams.ByteSize = _setting.FByteSize;
		VLOG(1) << MGetPort()<<" port byte size "<<_setting.FByteSize;
		break;
		default:
		LOG(DFATAL) << "Byte Size is invalid";
		dcbSerialParams.ByteSize = _setting.FByteSize;
		return false;
		break;

	}
	VLOG(1) << MGetPort()<<" port stop bites "<<_setting.FStopBits;
	switch (_setting.FStopBits)
	{
		case STOP_1:
		dcbSerialParams.StopBits = ONESTOPBIT;
		break;
		case STOP_2:
		dcbSerialParams.StopBits = TWOSTOPBITS;
		break;
		case STOP_1_5:
		dcbSerialParams.StopBits = ONE5STOPBITS;
		break;

		default:
		LOG(FATAL) << "Stop bits is invalid";
		return false;
		break;
	}
	VLOG(1) << MGetPort()<<" port Parity "<<_setting.FParity;
	dcbSerialParams.fParity = (_setting.FParity == PAR_NONE) ? false : true;
	switch (_setting.FParity)
	{
		case PAR_NONE:
		dcbSerialParams.Parity = NOPARITY;
		break;

		case PAR_ODD:
		dcbSerialParams.Parity = ODDPARITY;
		break;

		case PAR_EVEN:
		dcbSerialParams.Parity = EVENPARITY;
		break;

		case PAR_MARK:
		dcbSerialParams.Parity = MARKPARITY;
		break;

		case PAR_SPACE:
		dcbSerialParams.Parity = SPACEPARITY;
		break;

		default:
		LOG(FATAL) << "invalid parity";
		return false;
		break;
	}
	VLOG(1) << MGetPort()<<" port Flow cnotrol "<<_setting.FFlowControl;
	switch (_setting.FFlowControl)
	{
		case FLOW_OFF:
		dcbSerialParams.fOutxCtsFlow = false;
		dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
		dcbSerialParams.fOutX = false;
		dcbSerialParams.fInX = false;
		dcbSerialParams.XonLim=1;
		dcbSerialParams.XoffLim =1;
		break;

		case FLOW_HARD:
		dcbSerialParams.fOutxCtsFlow = true;
		dcbSerialParams.fRtsControl = RTS_CONTROL_TOGGLE;
		dcbSerialParams.fOutX = false;
		dcbSerialParams.fInX = false;
		break;

		case FLOW_SOFT:
		dcbSerialParams.fOutxCtsFlow = false;
		dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
		dcbSerialParams.fOutX = true;
		dcbSerialParams.fInX = true;
		break;

		default:
		LOG(FATAL) << "invalid flow control";
		return false;
		break;
	}

	dcbSerialParams.fBinary=1;
	// activate settings
	if (!SetCommState(FFd, &dcbSerialParams))
	{
		LOG(DFATAL) << "Error setting serial port settings.";
		return false;
	}

	// Setup timeouts
	COMMTIMEOUTS timeouts;
	memset(&timeouts, 0, sizeof(timeouts));
	DWORD _per_byte = 8+ 1;
	_per_byte += (_setting.FParity == PAR_NONE) ? 0 : 1;
	_per_byte += (_setting.FStopBits == STOP_1) ? 1 : 2;
	double _ms_byte =(1000.0 * _per_byte + _setting.FBaudRate - 1) / _setting.FBaudRate;
	if (_ms_byte < 1)
	{
		_ms_byte = 1;
	}
	timeouts.ReadIntervalTimeout =(DWORD)(
	_setting.FTimeout.FIntervelTimeout.MIs() ?
	_setting.FTimeout.FIntervelTimeout * 1000.0 :
	_ms_byte);
	timeouts.ReadTotalTimeoutConstant =(DWORD)
	(_setting.FTimeout.FReadTimeOut.MIs() ?
	_setting.FTimeout.FReadTimeOut * 1000.0 :
	1);
	timeouts.ReadTotalTimeoutMultiplier =(DWORD)
	(_setting.FTimeout.FReadByteTimeOut.MIs() ?
	_setting.FTimeout.FReadByteTimeOut * 1000.0 : 1);
	timeouts.WriteTotalTimeoutConstant = 1;
	timeouts.WriteTotalTimeoutMultiplier =(DWORD)(
	_setting.FTimeout.FWriteByteTimeOut.MIs() ?
	_setting.FTimeout.FWriteByteTimeOut * 1000.0 : 1);
//	timeouts.ReadIntervalTimeout = 1;
//	timeouts.ReadTotalTimeoutMultiplier = 1;
//	timeouts.ReadTotalTimeoutConstant = 1;
//	timeouts.WriteTotalTimeoutMultiplier = 1;
//	timeouts.WriteTotalTimeoutConstant = 1;
	if (!SetCommTimeouts(FFd, &timeouts))
	{
		LOG(DFATAL) << "Error setting timeouts.";
		return false;
	}
	PurgeComm(FFd, PURGE_RXCLEAR);
	PurgeComm(FFd, PURGE_TXCLEAR);
	return true;
}
size_t CSerialPort::CImpl::MAvailable()
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return 0;
	}
	COMSTAT cs;
	if (!ClearCommError(FFd, NULL, &cs))
	{
		LOG(ERROR) << "Error while checking status of the serial port: "
		<< GetLastError();
		return 0;
	}
	VLOG(1) << "Available to read from " << MGetPort()<<" "<<cs.cbInQue<<" bytes.";
	return static_cast<size_t>(cs.cbInQue);
}
int CSerialPort::CImpl::MWaitData(float const aTime) const
{
	LOG(DFATAL)<< "MWaitData is not implemented on Windows. ";
	NSHARE::sleep(1);
	return -1;
}
ssize_t CSerialPort::CImpl::MReceiveData(data_t * aBuf, std::size_t aCount,
		float const aTime)
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return 0;
	}
	VLOG(2)<<"Starting receive";
	DWORD const _time=aTime<=0?INFINITE:static_cast<DWORD>(aTime*1000.0);

	CRAII<CMutex> _block(FMutexRead);
	DWORD bytes_read;
	uint8_t buffer[2048];

	LOG_IF(DFATAL,aCount>sizeof(buffer))<<"The buffer is small. Realization of serial port is to rewrited.";

	int long long _count_read=(int long long)((aCount==0 || aCount>sizeof(buffer))?sizeof(buffer):aCount);
	DWORD _rval=0;
	for(HANG_INIT;MIsOpen(); HANG_CHECK)
	{
		VLOG(2)<<"Read data to "<<aBuf<<" aCount "<<aCount;
		FOReader.Offset=FOReader.OffsetHigh=0;
		ResetEvent(FOReader.hEvent);
		if (ReadFile(FFd, buffer, static_cast<DWORD>(_count_read), &bytes_read, &FOReader)==FALSE)
		{
			DWORD dwErr = GetLastError();
			if( (dwErr == ERROR_IO_PENDING))		//
			{
				VLOG(2)<<"The read operation is still pending";
			}
			else if(dwErr ==ERROR_OPERATION_ABORTED)
			{
				LOG(ERROR)<<" operation aborted.";
				MClose();
				return -1;
			}
			else
			{
				LOG(ERROR)<<" An error occurred; disconnect from the client. "<<dwErr;
				return -1;
			}
		}
		if(bytes_read)
		{
			VLOG(1) << "Read from " << MGetPort()<<" "<<bytes_read<<" bytes.";
			aBuf->insert(aBuf->end(),data_t::const_iterator((data_t::value_type*) buffer), data_t::const_iterator( (data_t::value_type*)buffer) + bytes_read);
			_count_read-=bytes_read;
			_rval+=bytes_read;
		}
		else
		{
			VLOG(2)<<"Wait time ="<<_time;
			if (!GetOverlappedResult(FFd, &FOReader, &bytes_read, TRUE))
			{
				DWORD err = GetLastError();
				VLOG(2)<<" An error occurred "<<err;
				return -1;
			}
			else if(bytes_read)
			{
				ResetEvent(FOReader.hEvent);
				VLOG(1) << "Read from " << MGetPort()<<" "<<bytes_read<<" bytes.";
				aBuf->insert(aBuf->end(),data_t::const_iterator((data_t::value_type*) buffer), data_t::const_iterator( (data_t::value_type*)buffer) + bytes_read);
				_count_read-=bytes_read;
				_rval+=bytes_read;
			}
			else
			{
				ResetEvent(FOReader.hEvent);
				VLOG(2)<<"Wtf? Read "<<bytes_read<<" bytes.";
				continue;
			}
		}
		if(!aCount || _count_read<=0)
		break;
	}
	return _rval;
}
CSerialPort::sent_state_t CSerialPort::CImpl::MSend(void const* const aData, size_t aLength)
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return sent_state_t(E_NOT_OPENED,0);
	}
	VLOG(2)<<"Send "<<aLength<<" bytes to "<<FThis.FSetting;
	CRAII<CMutex> _block(FMutexWrite);
	VLOG(2)<<"after mutex.";
	DWORD bytes_written=0;
	long long _length = aLength;
	double _time = NSHARE::get_time();
	uint8_t const* _send_buf=(uint8_t const*)aData;
	for (HANG_INIT; _length > 0 && MIsOpen();_send_buf+=bytes_written, _length -= bytes_written,HANG_CHECK)
	{
		FOWriter.Offset=FOWriter.OffsetHigh=0;
		ResetEvent(FOWriter.hEvent);
		if (WriteFile(FFd, _send_buf, static_cast<DWORD>(_length), &bytes_written,
						&FOWriter)==FALSE)
		{
			DWORD dwErr = GetLastError();
			if( (dwErr == ERROR_IO_PENDING))		//
			{
				VLOG(2)<<"The read operation is still pending";
			}
			else if(dwErr ==ERROR_OPERATION_ABORTED)
			{
				LOG(ERROR)<<" operation aborted.";
				MClose();
				return sent_state_t(E_ERROR,static_cast<size_t>(aLength-_length));
			}
			else
			{
				LOG(ERROR)<<" An error occurred; disconnect from the client. "<<dwErr;
				return sent_state_t(E_ERROR, static_cast<size_t>(aLength-_length));
			}
		}
		VLOG_IF(2,bytes_written)<<"Write to "<<MGetPort()<<" "<<bytes_written<<" bytes.";
		if(!bytes_written)
		{
			if (!GetOverlappedResult(FFd, &FOWriter, &bytes_written, TRUE))
			{
				DWORD err = GetLastError();
				VLOG(2)<<" An error occurred "<<err;
				return sent_state_t(E_ERROR, static_cast<size_t>(aLength-_length));
			}
			LOG_IF(ERROR,bytes_written==0)<<"Send zero bytes. Wtf?";
		}
	}
	VLOG_IF(1,_length<=0) << "Write " << aLength<<" bytes to "<<MGetPort()<<" within "<< (NSHARE::get_time() - _time) << "sec";
	return sent_state_t(E_SENDED, static_cast<size_t>(aLength-_length));
}
void CSerialPort::CImpl::MSetPort(const CText &port)
{
	FPort = wstring(port.begin(), port.end());
}
const CSocket& CSerialPort::CImpl::MGetSocket(void) const
{
	return FFixSock;
}
CText CSerialPort::CImpl::MGetPort() const
{
	CText _text;
	std::wstring::const_iterator _str = FPort.begin(), _str_end(FPort.end());
	for (; _str != _str_end; ++_str)
	{
		_text += *_str;
	}
	return _text;
}
void CSerialPort::CImpl::MFlush(eFlush const& aFlush)
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return;
	}
	if ((aFlush & FLUSH_IN) && (aFlush & FLUSH_OUT))
	(void)0;
	else if (aFlush & FLUSH_IN)
	LOG(ERROR) << "FLUSH_IN is not supported on Windows.";
	else
	LOG(ERROR) << "FLUSH_OUT is not supported on Windows.";

	VLOG(2) << "Flushed data in " << MGetPort();
	FlushFileBuffers(FFd);
}
void CSerialPort::CImpl::MSendBreak(int)
{
	LOG(DFATAL)<<"MSendBreak is not supported on Windows.";
}
bool CSerialPort::CImpl::MSignal(eLine const& aLine, bool aVal)
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return false;
	}
	VLOG(2) << " Set line " << aLine << " = " << aVal << " to "
	<< MGetPort();
	switch (aLine)
	{
		case DTR:
		if (aVal)
		EscapeCommFunction(FFd, SETDTR);
		else
		EscapeCommFunction(FFd, CLRDTR);
		break;

		case RTS:
		if (aVal)
		EscapeCommFunction(FFd, SETRTS);
		else
		EscapeCommFunction(FFd, CLRRTS);
		break;

		case BREAK:
		if (aVal)
		EscapeCommFunction(FFd, SETBREAK);
		else
		EscapeCommFunction(FFd, CLRBREAK);
		break;

		default:
		LOG(WARNING)<<"Invalid line type";
		return false;
		break;
	}
	return true;
}
bool CSerialPort::CImpl::MWaitForLineChanged()
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return false;
	}
	VLOG(2) << "Wait For Line Changed on " << MGetPort();
	DWORD dwCommEvent=0;

	if (!SetCommMask(FFd, EV_CTS | EV_DSR | EV_RING | EV_RLSD))
	{
		// Error setting communications mask
		LOG(WARNING) << "Error setting communications mask " << MGetPort();
		return false;
	}

	if (!WaitCommEvent(FFd, &dwCommEvent, NULL))
	{
		LOG(WARNING) << "An error occurred waiting for the event. " << MGetPort();
		return false;
	}
	else
	{
		VLOG(1) << "Line Changed on " << MGetPort();
		return true;
	}
}
bool CSerialPort::CImpl::MLineState(eLine const& aLine) const
{
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return false;
	}
	DWORD dwModemStatus;
	if (!GetCommModemStatus(FFd, &dwModemStatus))
	{
		LOG(ERROR) << "Error getting the status of the  line.";
		return false;
	}
	switch (aLine)
	{
		case CTS:
		return (MS_CTS_ON & dwModemStatus) != 0;
		break;
		case DSR:
		return (MS_DSR_ON & dwModemStatus) != 0;
		break;

		case RI:
		return (MS_RING_ON & dwModemStatus) != 0;
		break;
		case CD:
		return (MS_RLSD_ON & dwModemStatus) != 0;
		break;
		default:
		LOG(WARNING)<<"Invalid line type";
		return false;
		break;
	}
	return false;
}} //namespace NSerial
} //namespace NSHARE
#endif //#ifdef _WIN32
