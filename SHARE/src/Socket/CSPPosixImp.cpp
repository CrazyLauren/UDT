/*
 * CSPPosixImp.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#if defined(unix)|| defined(__QNX__) || !defined(_WIN32)//for all
#include <deftype>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <paths.h>
#include <termios.h>
#include <sys/param.h>
#include <pthread.h>

#if defined(__linux__)
# include <linux/serial.h>
#include <sysexits.h>
#include <sys/signal.h>
#endif
#include <SerialPort.h>
#include <Socket/CSPPosixImp.h>
namespace NSHARE
{
namespace NSerial
{
using std::string;
using std::stringstream;
using std::invalid_argument;

CSerialPort::CImpl::CImpl(CSerialPort& aThis)
		: FThis(aThis)
{
	FIsOpen = 0;
	FPort = "";
	FByteTime=0;
}
CSerialPort::CImpl::~CImpl()
{
	MClose();
}
inline bool CSerialPort::CImpl::MIsFdValid()
{
	return FFd.MIsValid();
}
void CSerialPort::CImpl::MClose()
{
	if (!MIsOpen())
	{
		//LOG(ERROR) << "Port has already been close.";
		return;
	}
	if (!MIsFdValid())
		LOG(WARNING)<< "Port is open but fd is invaid";
		else
		{
			FFd.MClose();
		}
	FIsOpen = false;
	VLOG(1) << MGetPort() << " port is closed.";
}
bool CSerialPort::CImpl::MOpen()
{
	if (FPort.empty())
	{
		LOG(DFATAL) << "Empty port is invalid.";
		return false;
	}
	if (MIsOpen())
	{
		LOG(DFATAL) << "Serial port already open.";
		return false;
	}
	if (MIsFdValid())
	LOG(WARNING) << "Port is valid";


	for (HANG_INIT; !FFd.MIsValid();HANG_CHECK)
	{
		FFd = ::open(FPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
		if (!FFd.MIsValid())
		{
			LOG(WARNING) << "Error during the opening of the port.";
			switch (errno)
			{
				case EINTR:
				// Recurse because this is a recoverable error.
				break;

				case ENFILE:
				case EMFILE:
				LOG(DFATAL)<<"Too many file handles open.";
				return false;
				break;
				default:
				LOG(DFATAL)<<"Unknown error opening the serial port."<<strerror(errno);
				return false;
				break;

			}
		}
	}
	FIsOpen = true;
	if (!MReconfigure()) MClose();
	return FIsOpen;
}
bool CSerialPort::CImpl::MReconfigure()
{
	if (!MIsFdValid())
	{
		LOG(DFATAL) << "Invalid file descriptor.";
		return false;
	}
	struct termios options;
	if (tcgetattr(FFd, &options) == -1)
	{
		LOG(DFATAL)
				<< "Error getting the serial port state." << strerror(errno);
				return false;
			}
	::cfmakeraw(&options);
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &=
			(~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN)); //|ECHOPRT

	options.c_oflag &= (~(OPOST));
	options.c_iflag &= (~(INLCR | IGNCR | ICRNL | IGNBRK));

#ifdef IUCLC
	options.c_iflag &= (~IUCLC);
#endif
#ifdef PARMRK
	options.c_iflag &= (~PARMRK);
#endif
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 0;

	port_settings_t _setting = FThis.MGetSetting();
	speed_t _baud;
	switch (_setting.FBaudRate)
	{
#ifdef B0
		case 0:
#endif
#ifdef B50
		case 50:
#endif
#ifdef B75
		case 75:
#endif
#ifdef B110
		case 110:
#endif
#ifdef B134
		case 134:
#endif
#ifdef B150
		case 150:
#endif
#ifdef B200
		case 200:
#endif
#ifdef B300
		case 300:
#endif
#ifdef B600
		case 600:
#endif
#ifdef B1200
		case 1200:
#endif
#ifdef B1800
		case 1800:
#endif
#ifdef B2400
		case 2400:
#endif
#ifdef B4800
		case 4800:
#endif
#ifdef B7200
			case 7200:
#endif
#ifdef B9600
		case 9600:
#endif
#ifdef B14400
			case 14400:
#endif
#ifdef B19200
		case 19200:
#endif
#ifdef B28800
			case 28800:
#endif
#ifdef B57600
		case 57600:
#endif
#ifdef B76800
		case 76800:
#endif
#ifdef B38400
		case 38400:
#endif
#ifdef B115200
		case 115200:
#endif
#ifdef B128000
			case 128000:
#endif
#ifdef B153600
			case 153600:
#endif
#ifdef B230400
			case 230400:
#endif
#ifdef B256000
			case 256000:
#endif
#ifdef B460800
			case 460800:
#endif
#ifdef B576000
			case 576000:
#endif
#ifdef B921600
			case 921600:
#endif
#ifdef B1000000
			case 1000000:
#endif
#ifdef B1152000
			case 1152000:
#endif
#ifdef B1500000
			case 1500000:
#endif
#ifdef B2000000
			case 2000000:
#endif
#ifdef B2500000
			case 2500000:
#endif
#ifdef B3000000
			case 3000000:
#endif
#ifdef B3500000
			case 3500000:
#endif
#ifdef B4000000
			case 4000000:
#endif
			_baud = _setting.FBaudRate;
#if  defined(__FreeBSD__)
			::cfsetspeed(&options, _baud);
#else
			::cfsetispeed(&options, _baud);
			::cfsetospeed(&options, _baud);
#endif
			break;
		default:
			// OS X support
#if defined (TIOCSSERIAL)
			struct serial_struct ser;

			if (-1 == ioctl (FFd, TIOCGSERIAL, &ser))
			{
				LOG(DFATAL)<<"Failed setting custom baund.";
				return false;
			}
			ser.custom_divisor = ser.baud_base / static_cast<int> (_setting.FBaudRate);
			// update flags
			ser.flags &= ~ASYNC_SPD_MASK;
			ser.flags |= ASYNC_SPD_CUST;

			if (-1 == ioctl (FFd, TIOCSSERIAL, &ser))
			{
				LOG(DFATAL)<<"Failed setting custom baund.";
				return false;
			}
#else
			LOG(DFATAL) << "Baud rate is invalid.";
			return false;
#endif
	}
	options.c_cflag &= (tcflag_t) ~CSIZE;
	switch (_setting.FByteSize)
	{
		case DATA_5:
			options.c_cflag |= CS5;
			break;
		case DATA_6:
			options.c_cflag |= CS6;
			break;

		case DATA_7:
			options.c_cflag |= CS7;
			break;

		case DATA_8:
			options.c_cflag |= CS8;
			break;

		default:
			LOG(DFATAL) << "Byte Size is invalid";
			return false;
			break;

	}
	VLOG(1) << MGetPort() << " port byte size " << _setting.FByteSize;

	switch (_setting.FStopBits)
	{
		case STOP_1:
			options.c_cflag &= (~(CSTOPB));
			break;
		case STOP_2:
			options.c_cflag |= (CSTOPB);
			break;

		default:
			LOG(FATAL) << "Stop bits is invalid";
			return false;
			break;
	}

	VLOG(1) << MGetPort() << " port stop bites " << _setting.FStopBits;

	options.c_iflag &= (~(INPCK | ISTRIP));
	switch (_setting.FParity)
	{
		case PAR_NONE:
			options.c_cflag &= (tcflag_t) ~(PARENB | PARODD);
			break;

		case PAR_ODD:
			options.c_cflag |= (PARENB | PARODD);
			break;

		case PAR_EVEN:
			options.c_cflag &= (tcflag_t) ~(PARODD);
			options.c_cflag |= (PARENB);
			break;

#ifdef CMSPAR //QNX doesn't support.))
			case PAR_SPACE:
			options.c_cflag |= (PARENB | CMSPAR);
			options.c_cflag &= (tcflag_t) ~(PARODD);
			break;
#elif defined(__QNX__)
		case PAR_SPACE:
			LOG(FATAL) << "QNX does not support space parity";
			return false;
			break;
#else
#error	Unknown target
#endif
		default:
			LOG(FATAL) << "invalid parity";
			return false;
			break;
	}

	VLOG(1) << MGetPort() << " port Parity " << _setting.FParity;
	bool _xonxoff=false;
	bool _rtscts;
	switch (_setting.FFlowControl)
	{
		case FLOW_OFF:
			_xonxoff = false;
			_rtscts = false;
			break;

#ifdef CRTSCTS
			case FLOW_HARD:
			_xonxoff = false;
			_rtscts = true;
			break;
#elif defined(__QNX__)
		case FLOW_HARD:
			LOG(FATAL) << "QNX does not support  hardware flow control";
			break;
#else
#	error Unknown target
#endif
		case FLOW_SOFT:
			_xonxoff = true;
			_rtscts = false;
			break;

		default:
			LOG(FATAL) << "invalid flow control";
			return false;
			break;
	}
	if (_xonxoff)
		options.c_iflag |= (IXON | IXOFF); //|IXANY)
	else
		options.c_iflag &= (tcflag_t) ~(IXON | IXOFF | IXANY);

#ifdef CRTSCTS  //QNX doesn't support.))
	if (_rtscts)
	options.c_cflag |= (CRTSCTS);
	else
	options.c_cflag &= (unsigned long) ~(CRTSCTS);
#endif
	if (::tcsetattr(FFd, TCSANOW, &options))
	{
		LOG(DFATAL) << "Error setting serial port settings.";
		return false;
	}
	uint32_t bit_time_ns =static_cast<uint32_t>( 1e9 / _setting.FBaudRate);

	FByteTime = bit_time_ns
			* (1 + _setting.FByteSize + _setting.FParity + _setting.FStopBits);
	VLOG(1) << " Byte time " << FByteTime;
	return true;
}
size_t CSerialPort::CImpl::MAvailable()
{
	int bytes = 0;
	if (ioctl(FFd, FIONREAD, &bytes) != 0)
	{
		LOG(ERROR) << "Error";
		return 0;
	}
	return static_cast<size_t>(bytes);
}
void CSerialPort::CImpl::MWaitBytes(size_t count)
{
	NSHARE::usleep(static_cast<unsigned long>((FByteTime / 1000.0) * count));
}
ssize_t CSerialPort::CImpl::MReceiveData(data_t * aContainer,
		std::size_t aCount, float const aTime)
{
	VLOG(1)
			<< "Try receive " << aCount << " bytes from " << MGetPort()
					<< ", Time:" << aTime;

	NSHARE::CRAII<CMutex> _block(FMutexRead);
	if (!MIsOpen())
	{
		LOG(ERROR) << "Port is closed.";
		return 0;
	}
	int bytes_read = 0;
	if ((bytes_read = MRead(aContainer, 0))>0)
	{
		return bytes_read;
	}
	int _rval = MWaitData(aTime);
	VLOG(2) << "Wait status" << _rval;

	if (_rval == 0)
		return 0;
	else if (_rval < 0)
	{
		MClose();
		return 0;
	}
	int _count=aCount;
	for (;;)
	{
		VLOG(2) << "Try read " << _count << " bytes.";
		ssize_t bytes_read_now = MRead(aContainer, _count);

		VLOG(2) << "Read " << bytes_read_now << " bytes.";
		if (bytes_read_now > 0)
			_count -= bytes_read_now;
		else
			break;
		if (_count <= 0) break;
		VLOG(2) << "Try again read from port.";
		MWaitBytes(_count);
	}
	return aContainer->size();
}
inline int CSerialPort::CImpl::MRead(data_t * aContainer,
		std::size_t aCount) const
{
	char buffer[256];
	int _val = read(FFd.MGet(), buffer, 256);
	VLOG(1) << "read " << _val << " bytes.";
	VLOG_IF(1,_val<0 &&errno!=EAGAIN )<<"Socket "<<FFd.MGet()<<" Read error "<< strerror(errno)<<"("<<errno<<").";
	if (_val > 0)
	{
		for (int i = 0; i < _val; ++i)
			aContainer->push_back(buffer[i]);
	}
	return _val;
}
CSerialPort::sent_state_t CSerialPort::CImpl::MSend(void const* const aData, std::size_t aByte)
{
	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(FFd, &writefds);

	struct timeval _sleep;
	struct timeval *_psleep = &_sleep;
	NSHARE::convert(_psleep, 1.0); //1 sec max
	NSHARE::CRAII<CMutex> _block(FMutexWrite);
	std::size_t const _full_size=aByte;
	for (ssize_t _count = 0; aByte;)
	{
		VLOG(1) << "Send " << aByte << " bytes.";
		if (select(FFd.MGet() + 1, NULL, &writefds, NULL, _psleep) <= 0)
		{
			LOG(ERROR)
					<< "Timeout during sending " << aByte << " to "
							<< MGetPort();
			return sent_state_t(E_ERROR,_full_size-aByte);
		}
		aByte -= _count = write(FFd,
				reinterpret_cast<char const* const >(aData) + _count, aByte);
		if (_count <= 0)
		{
			VLOG(1)
					<< "Send error " << aByte << " to " << MGetPort() << "."
							<< strerror(errno);
			return sent_state_t(E_ERROR,_full_size-aByte);
						}
		MWaitBytes(_count);
		MFlush(FLUSH_OUT);
	}
	return sent_state_t(E_SENDED,_full_size-aByte);
}
void CSerialPort::CImpl::MFlush(eFlush const& aVal)
{
	if ((aVal & FLUSH_IN) && (aVal & FLUSH_OUT))
	{
		tcflush(FFd, TCIOFLUSH);
		tcdrain(FFd);
	}
	else if (aVal & FLUSH_IN)
		tcflush(FFd, TCIFLUSH);
	else
	{
		tcflush(FFd, TCOFLUSH);
		tcdrain(FFd);
	}
}
void CSerialPort::CImpl::MSendBreak(int aVal)
{
	tcsendbreak(FFd, static_cast<int>(aVal / 4));
}
bool CSerialPort::CImpl::MSignal(eLine const& aLine, bool aVal)
{
	if (!MIsOpen())
	{
		LOG(ERROR) << "Port is closed.";
		return false;
	}
	VLOG(2)
			<< " Set line " << aLine << " = " << aVal << " to " << MGetPort();

	int command = -1;
	switch (aLine)
	{
		case DTR:
			command = TIOCM_DTR;
			break;

		case RTS:
			command = TIOCM_RTS;
			break;

		case CTS:
			command = TIOCM_CTS;
			break;

		case DSR:
			command = TIOCM_DSR;
			break;

		case RI:
			command = TIOCM_RI;
			break;

		case CD:
			command = TIOCM_CD;
			break;

		case BREAK:
			if (aVal)
				ioctl(FFd, TIOCSBRK);
			else
				ioctl(FFd, TIOCCBRK);
			break;

		default:
			LOG(WARNING) << "Invalid line type";
			return false;
			break;
	}
	if (command >= 0)
	{
		if (aVal)
		{
			if (-1 == ioctl(FFd, TIOCMBIS, &command))
			{
				LOG(ERROR)
						<< "setDTR failed on a call to ioctl(TIOCMBIS): "
								<< errno<< " " << strerror(errno);
				return false;
			}
		}
		else
		{
			if (-1 == ioctl(FFd, TIOCMBIC, &command))
				{
								LOG(ERROR)<< "setDTR failed on a call to ioctl(TIOCMBIC): " << errno<< " " << strerror(errno);
								return false;
				}
		}
	}
	return true;
}
bool CSerialPort::CImpl::MLineState(eLine const& aLine) const
{
	if (!MIsOpen())
	{
		LOG(ERROR) << "Port is closed.";
		return false;
	}
	int status;
	if (-1 == ioctl (FFd.MGet(), TIOCMGET, &status))
	{
		LOG(ERROR) << "Error getting the status of the  line."<< strerror(errno);
		return false;
	}
	switch (aLine)
	{
	case CTS:
		return 0 != (status & TIOCM_CTS);
		break;
	case DSR:
		return 0 != (status & TIOCM_DSR);
		break;

	case RI:
		return  0 != (status & TIOCM_RI);
		break;
	case CD:
		return  0 != (status & TIOCM_CD);
		break;
	default:
		LOG(WARNING)<<"Invalid line type";
		return false;
		break;
	}
	return false;
}

bool CSerialPort::CImpl::MWaitForLineChanged()
{
	if (!MIsOpen())
	{
		LOG(ERROR) << "Port is closed.";
		return false;
	}
	VLOG(2) << "Wait For Line Changed on " << MGetPort();

	for (; MIsOpen();)
	{

		int status;

		if (-1 == ioctl(FFd.MGet(), TIOCMGET, &status))
		{
			LOG(ERROR) << "waitForChange failed on a call to ioctl(TIOCMGET): " << errno<<" " << strerror(errno);
			return false;
		}
		else
		{
			if (0 != (status & TIOCM_CTS) || 0 != (status & TIOCM_DSR)
					|| 0 != (status & TIOCM_RI) || 0 != (status & TIOCM_CD))
			{
				return true;
			}
		}

		NSHARE::usleep(1000);
	}

	return false;
}
int CSerialPort::CImpl::MWaitData(float const aTime) const
{
	struct timeval _sleep;
	struct timeval *_psleep = &_sleep;
	if (aTime > 0.0)
		NSHARE::convert(_psleep, aTime);
	else
		_psleep = NULL;

	fd_set _rfds;
	FD_ZERO(&_rfds);
	FD_SET(FFd.MGet(), &_rfds);

	int _rval = select(FFd .MGet()+1, &_rfds, NULL, &_rfds, _psleep);
	if (_rval < 0)
	{
		// Select was interrupted
		if (errno== EINTR)
		{
			return -1;
		}
		LOG(DFATAL) << "There was some error in select."<<strerror(errno);
	}
	if (_rval == 0)
	{
		return 0;
	}
	//This shouldn't happen!
	if (!FD_ISSET (FFd.MGet(), &_rfds))
	{
		LOG(DFATAL) << "select reports ready to read, but our fd isn't"
				" in the list.";
		return -1;
	}
	return _rval;
}
}
}
#endif//#if defined(unix)|| defined(__QNX__)
