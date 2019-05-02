// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CFifoPosixImpl.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 29.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#if defined (__QNX__) ||defined (unix)

#include <deftype>
#include <dirent.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <console.h>
#include <fdir.h>
#include <Pipe.h>
#include <Socket/CFifoPosixImpl.h>

#define PERM 0666
inline char *print_error()
{
	return strerror(errno);
}
namespace NSHARE
{
const unsigned CFifo::CImpl::ATOMIC_FIFO_BUUFER = PIPE_BUF;
CFifo::CImpl::CImpl(CFifo& aThis) :
		FThis(aThis), FFdRead(-1),FFdWrite(-1), FIsOpen(false)
{
	VLOG(2) << "Construct CFifo::CImpl: " << this;
}
CFifo::CImpl::~CImpl()
{
	VLOG(2) << "Destruct CFifo::CImpl: " << this;
	MClose();
}

bool CFifo::CImpl::MOpen()
{
	switch (FThis.MGetSetting().FType)
	{
		case READ_ONLY:
			return FIsOpen=MOpenRead();
			break;
		case WRITE_ONLY:
			return FIsOpen=MOpenWrite();
			break;
		case READ_WRITE:
			MOpenWrite();
			return FIsOpen=MOpenRead();
			break;
		default:
			CHECK(false) << " Bad enum";
		return false;
	}
}
bool CFifo::CImpl::MOpenWrite()
{
	VLOG(2) << "Open PosixFifo :" << this;
	CText _path = FThis.MGetSetting().FServerPath;
	CHECK(!_path.empty()) << "The Path is empty";
	VLOG_IF(2, MIsMade()) << "File " << _path << " is exist.";

	FFdWrite = open(_path.c_str(), O_RDWR | O_NONBLOCK, 0);
	if (!FFdWrite.MIsValid())
	{
		FFdWrite=-1;
		VLOG(1)<< "Can't open fifo:" << _path << " Error:" << print_error();
		return false;
	}

	VLOG(2) << "Open " << _path << " fifo opened successfully  :" << this;

	return true;

}

bool CFifo::CImpl::MOpenRead()
{
	VLOG(2) << "Open PosixFifo :" << this;
	LOG_IF(DFATAL,MIsOpen()) << "FIFO has already been opened.";
	CText _path = FThis.MGetSetting().FPath;
	CHECK(!_path.empty()) << "The Path is empty";
	VLOG_IF(2, MIsMade()) << "File " << _path << " is exist.";

	if (!MMakefifo(_path))
		return false;

	FFdRead = open(_path.c_str(), O_RDWR | O_NONBLOCK, 0);
	if (!FFdRead.MIsValid())
	{
		FFdRead=-1;
		if (FThis.MGetSetting().FType == WRITE_ONLY && errno==ENXIO)
		{
			VLOG(1)<<"FIFO "<<_path.c_str()<<" not open for reading.";
			return true; //when will send try to open again;
		}
		LOG(DFATAL)<< "Can't open fifo:" << _path << " Error:" << print_error();
		return false;
	}
		else
		FSelect.MAddSocket(FFdRead);

	VLOG(2) << "Open " << _path << " fifo opened successfully  :" << this;

	return true;
}
bool CFifo::CImpl::MIsOpen() const
{
	return FIsOpen;
}

bool CFifo::CImpl::MIsMade() const
{
	return NSHARE::is_the_file_exist(FThis.MGetSetting().FPath.MToStdString());
}
void CFifo::CImpl::MClose()
{
	VLOG(2) << "Close  fifo: " << this;
	if (MIsOpen())
	{
		FSelect.MRemoveSocket(FFdRead);
		FFdRead.MClose();
		FFdWrite.MClose();
	}
	if (MIsMade())
	{
		MUnlink(FThis.MGetSetting().FPath);
	}
	FIsOpen = false;
	VLOG(1) << "Fifo " << FThis.MGetSetting().FPath << "  is closed.";

	CHECK(!FFdRead.MIsValid());
	CHECK(!FSelect.MIsSetUp());
	CHECK(FSelect.MGetSockets().empty());
}
bool CFifo::CImpl::MMakefifo(CText _path)
{
	VLOG(2) << "Make fifo file " << _path;

	CHECK(!_path.empty()) << "The path is empty.";
	LOG_IF(DFATAL,MIsOpen()) << "FIFO has already been opened.";
	errno=0;
	if (mkfifo(_path.c_str(), PERM) < 0)
	{
		VLOG(2) << "Error during making fifo: " << print_error();
		if (errno!=EEXIST)
		{
			LOG(DFATAL)
			<< "Can't open fifo:" << _path << " Error:" << print_error();
			MUnlink(_path);
			return false;
		}
		else
		VLOG(2)<< "File " << _path << " is exist.";
	}
	return true;
}

inline void CFifo::CImpl::MUnlink(const CText& aClientPath)
{
	VLOG(2) << "Unlink " << aClientPath << ":" << this;
	if (unlink(aClientPath.c_str()) < 0)
		LOG(ERROR)<< "Can't unlink " << aClientPath.c_str() << ". "
		<< print_error() << "(" << errno<<").";
	}
size_t CFifo::CImpl::MAvailable() const
{
	CHECK(FFdRead.MIsValid()) << "Fifo is not opened ";
	int bytes = 0;
	errno=0;
	if (ioctl(FFdRead.MGet(), FIONREAD, &bytes) != 0)
	{
		LOG(ERROR)<< "Ioctl error" << print_error();
		return 0;
	}
	VLOG(2) << bytes << " bytes available for reading from ";
	return static_cast<size_t>(bytes);
}
inline void CFifo::CImpl::MSetBlock()
{
	int flags=fcntl(FFdRead.MGet(),F_GETFL,0);
	flags&=(~O_NONBLOCK);
	fcntl(FFdRead.MGet(),F_SETFL,flags);
}
inline void CFifo::CImpl::MSetUnBlock()
{
	int flags=fcntl(FFdRead.MGet(),F_GETFL,0);
	flags|=O_NONBLOCK;
	fcntl(FFdRead.MGet(),F_SETFL,flags);
}

ISocket::sent_state_t CFifo::CImpl::MSend(const void* const aData, std::size_t aSize)
{
	double _time=NSHARE::get_time();
	std::size_t const _buf_size=aSize;
	VLOG(2) << "Send data " << aData << " (size=" << aSize << "):" << this;

	LOG_IF(WARNING,ATOMIC_FIFO_BUUFER<aSize)
			<< "The Size " << aSize
					<< " byte is more then the atomic buffer length = "
					<< ATOMIC_FIFO_BUUFER;
	{
		NSHARE::CRAII<CMutex> _block(FMutexWrite);
		VLOG(2)
				<< "Our turn Send :" << CThread::sMThreadId()
						<<" delta time "<< (NSHARE::get_time() - _time) << "sec :" << this;

		if (!FFdWrite.MIsValid())
		{
			if (!FThis.MGetSetting().FServerPath.empty())
			{
				VLOG(2) << "FIFO  not opened try opened for writing.";
				bool _rval = MOpenWrite();
				if (!_rval)
					return ISocket::sent_state_t(E_ERROR,0);
			}
			else
				return ISocket::sent_state_t(E_ERROR,0);
		}

		int _err_count = 0;
		bool _is_blocking=false;
		for (HANG_INIT; aSize; HANG_CHECK)
		{
			errno=0;
			int _len = write(FFdWrite, aData, aSize);

			if(_is_blocking)//By default
				MSetUnBlock();

			VLOG(1) << "Write " << _len<<" bytes to "<<FThis.MGetSetting().FPath
			<<" within "<< (NSHARE::get_time() - _time) << "sec";
			if (_len < 0)
			{

				switch (errno)
				{
					case EAGAIN:
					VLOG(3) <<"FIFO is Full.";
					MSetBlock();//wait until space is available
					_is_blocking=true;
					continue;
					break;

					case EPIPE:
						VLOG(0) <<"FIFO isn't open for reading.";
						return ISocket::sent_state_t(E_ERROR,_buf_size-aSize);
					break;

					default:
					break;
				}
				LOG(ERROR)<< "Write error " << print_error() <<"("<<errno<<").";
				++_err_count;
				if(_err_count>5)
					return ISocket::sent_state_t(E_ERROR,_buf_size-aSize);
			}
			else
			{
				CHECK_GE(aSize,_len)<<"O_O I don't known, why it happened.";
				aSize-=_len;
				VLOG_IF(1,aSize) << "Repeat write, len " << _len;
			}
			CHECK(_len)<<"Len can't be equal 0";
			if(!aSize)
				break;
		}
	}
	return ISocket::sent_state_t(E_SENDED,_buf_size-aSize);
}
ssize_t CFifo::CImpl::MReceiveData(data_t* aBuf, const float aTime)
{
	double _time=NSHARE::get_time();
	VLOG(2)
			<< "Receive data to " << aBuf << " from socket " << FFdRead
					<< "; aTime " << aTime << ":" << this;
	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return 0;
	}
	int _len = 0;
	for (HANG_INIT;; HANG_CHECK)
	{
		NSHARE::CRAII<CMutex> _block(FMutexRead);

		VLOG(2)
				<< "Our turn reading :" << CThread::sMThreadId() << ":" << this;
		CSelectSocket::socks_t _to;
		int _rval = FSelect.MWaitData(_to, aTime);
		VLOG(2) << "Wait status" << _rval<<" delta time "<<(NSHARE::get_time()-_time)<<" sec";

		if (_rval == 0)
			return 0;
		else if (_rval < 0)
		{
			MClose();
			return 0;
		}



		const size_t _befor = aBuf->size();
		VLOG(2) << "Befor " << _befor << " bytes";
		const size_t _avalable = MAvailable();
		VLOG_IF(1,(!_avalable)) << "No data on socket " << _to.front()
											<< ", may be it has been closed already."
													"Assuming the size equal 1 byte.";
		const size_t _size = (_avalable ? _avalable : 1) + _befor;
		aBuf->resize(_size);
		data_t::value_type* _pbegin = (data_t::value_type*)aBuf->ptr() + _befor;
		errno=0;
		_len = read(FFdRead, _pbegin, _avalable);
		VLOG(2) << "Read " << _len << " bytes"<<" delta time "<<(NSHARE::get_time()-_time)<<" sec";
		//Set real size
		if(_len>0)
			aBuf->resize(_befor + _len);
		else
			aBuf->resize(_befor);

		if (_len == 0)
		{

			VLOG(1)<< "Fifo "<< FThis.MGetSetting()<<" not open for reading ";
			return 0;
		}
		else if (_len < 0)
		{
			LOG_IF(ERROR, _len<0) << "Read error " << print_error();
			if(errno==EAGAIN )
			continue; //HZ what is it

			MClose();
			return 0;
		}
		else break;
	}
	return _len;
}

const CSocket& CFifo::CImpl::MGetSocket(void) const
{
	return FFdRead;
}
void CFifo::CImpl::MFlush(eFlush const&)
{
	LOG(WARNING)<< "FIFO flush method is not implemented.";
}

} //namespace NSHARE

#endif
