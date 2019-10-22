// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CFifoWin32Impl.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifdef _WIN32
#include <deftype>
#include <winerror.h>
#include <SHARE/Pipe.h>
#include <SHARE/Socket/CFifoWin32Impl.h>

namespace NSHARE
{

const unsigned CFifo::CImpl::ATOMIC_FIFO_BUUFER = 8192;
CFifo::CImpl::CImpl(CFifo& aThis) :
		FThis(aThis), FHandler(INVALID_HANDLE_VALUE), FIsOpen(false)
{
	memset(&FOReader, 0, sizeof(FOReader));
	FOReader.hEvent = INVALID_HANDLE_VALUE;
	memset(&FOWriter, 0, sizeof(FOWriter));
	FOWriter.hEvent = INVALID_HANDLE_VALUE;

	memset(FBufferPipe, 0, sizeof(FBufferPipe));
	FBufSize = 0;
	VLOG(2) << "Construct CFifo::CImpl: " << this;
}
CFifo::CImpl::~CImpl()
{
	VLOG(2) << "Destruct CFifo::CImpl: " << this;
	MClose();
}
bool CFifo::CImpl::MMakefifo(CText const& aPath)
{
	CHECK(!aPath.empty()) << "The Path is empty";
	CText _path(MGetValidPath(aPath));

	DWORD dwOpenMode = PIPE_ACCESS_DUPLEX;
	switch (FThis.MGetSetting().FType)
	{
	case READ_ONLY:
		dwOpenMode = PIPE_ACCESS_INBOUND;
		break;
	case WRITE_ONLY:
		dwOpenMode = PIPE_ACCESS_OUTBOUND;
		break;
	case READ_WRITE:
		dwOpenMode = PIPE_ACCESS_DUPLEX;
		break;
	default:
		CHECK(false) << " Bad enum";
		break;
	}
	dwOpenMode |= FILE_FLAG_OVERLAPPED;

	DWORD dwPipeMode = PIPE_TYPE_BYTE | PIPE_WAIT;
	DWORD const nMaxInstances = PIPE_UNLIMITED_INSTANCES; //todo to config
	HANDLE _handler = CreateNamedPipe((LPSTR) _path.c_str(), dwOpenMode,
			dwPipeMode, nMaxInstances, ATOMIC_FIFO_BUUFER, ATOMIC_FIFO_BUUFER,
			0, NULL);
	if (_handler == INVALID_HANDLE_VALUE && GetLastError() != ERROR_PIPE_BUSY)
	{
		LOG(ERROR)<<"Cannot open pipe "<<_path<<" "<<GetLastError();
		return false;
	}
	return true;
}
CText CFifo::CImpl::MGetValidPath(CText const& _path)
{
	const CText _prefix("\\\\");
	CHECK_EQ(_prefix.length(), 2);
	if (_path.compare(0, _prefix.length(), _prefix) != 0)
	{
		LOG(WARNING)<<"There is not prefix "<<_prefix<<" in the fifo path:"<<_path;
		CText _new_path("\\\\.\\pipe\\");
		CText::size_type _pos=_path.find_last_of("\\");
		if(_pos==CText::npos)
		{
			_pos=_path.find_last_of("/");
		}
		if(_pos==CText::npos)
		_new_path+=_path;
		else
		_new_path+=_path.substr(_pos+1,CText::npos);

		return _new_path;
	}
	return _path;
}
bool CFifo::CImpl::MOpen()
{
	VLOG(2) << "Open Win32Fifo :" << this;
	MOpen(.2);
	return true;
}
bool CFifo::CImpl::MOpen(double aTime)
{
	VLOG(2) << "Open Win32Fifo :" << this;
	CRAII<CMutex> _mutex(FMutexConnect);
	VLOG_IF(2,MIsOpen()) << "FIFO has already been opened.";
	if (MIsOpen())
		return false;
	if (FOReader.hEvent == INVALID_HANDLE_VALUE)
		FOReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (FOWriter.hEvent == INVALID_HANDLE_VALUE)
		FOWriter.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	DWORD const _time =
			definitelyLessOrEqualThan(aTime, 0.) ?
					NMPWAIT_WAIT_FOREVER : static_cast<DWORD>(aTime * 1000.0);
//	if(!MMakefifo( FThis.MGetSetting().FPath))
//		return false;
	CText _path(MGetValidPath(FThis.MGetSetting().FPath));
	CHECK(!_path.empty()) << "The Path is empty";

	double _max_time = (double) _time / 1000.0;
	CHECK_GT(_max_time, 0);
	double _cur_time = NSHARE::get_time();
	for (HANG_INIT;_max_time>0;HANG_CHECK)
	{
		BOOL _is=WaitNamedPipe((LPSTR) _path.data(), _time);
		_max_time-=(NSHARE::get_time()-_cur_time);
		_cur_time=NSHARE::get_time();
		if (_is==FALSE)
		{
			DWORD _error = GetLastError();
			LOG_IF(DFATAL,_error!=ERROR_SEM_TIMEOUT && _error!=ERROR_FILE_NOT_FOUND ) << "Unknown error "
			<< _error;
			VLOG_IF(1,_error==ERROR_SEM_TIMEOUT) << "Timeout ";

			if(_error==ERROR_FILE_NOT_FOUND)
			{
				VLOG(5)<<"Try conncet fifo again." <<_max_time;
				NSHARE::usleep(10000);
				continue;
			}
			else
			return false;
		}
		FHandler = CreateFile(_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

		if (FHandler != INVALID_HANDLE_VALUE
		|| GetLastError() != ERROR_PIPE_BUSY)
		break;
		VLOG(2) << "Try again.";
	}
	if (FHandler == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwMode = PIPE_TYPE_BYTE;
	BOOL _is = SetNamedPipeHandleState(FHandler, &dwMode, NULL, NULL);
	if (_is == FALSE)
	{
		return false;
	}
	FIsOpen = true;
	VLOG(2) << "Open " << _path << " fifo opened successfully  :" << this;
	return true;
}
bool CFifo::CImpl::MIsOpen() const
{
	return FIsOpen;
}

bool CFifo::CImpl::MIsMade() const
{
	//todo
	return true;
}
void CFifo::CImpl::MClose()
{
	VLOG(2) << "Close  fifo: " << this;
	if (!MIsOpen())
		return;
	if (MIsMade())
	{
		//DisconnectNamedPipe(FHandler);
	}
	if (FOReader.hEvent != INVALID_HANDLE_VALUE)
		CloseHandle(FOReader.hEvent);
	if (FOWriter.hEvent != INVALID_HANDLE_VALUE)
		CloseHandle(FOWriter.hEvent);
	FOReader.hEvent = INVALID_HANDLE_VALUE ;
	FOWriter.hEvent = INVALID_HANDLE_VALUE ;
	CloseHandle(FHandler);
	FHandler = INVALID_HANDLE_VALUE ;
	FIsOpen = false;
	VLOG(1) << "Fifo " << FThis.MGetSetting().FPath << "  is closed.";

}

size_t CFifo::CImpl::MAvailable() const
{
	LOG_IF(FATAL,FHandler!=INVALID_HANDLE_VALUE) << "Fifo is not opened ";
	DWORD avail = 0;
	if (PeekNamedPipe(FHandler, NULL, 0, NULL, &avail, NULL) == FALSE)
	{
		LOG(ERROR)<< "Available error:" << GetLastError();

	}
	VLOG(2) << avail << " bytes available for reading from ";
	return static_cast<size_t>(avail);
}

CFIFOServer::sent_state_t CFifo::CImpl::MSend(const void* const aData, std::size_t aSize)
{
	if (!aData)
		return sent_state_t(sent_state_t::E_ERROR,0);
	double _time = NSHARE::get_time();
	VLOG(2) << "Send data " << aData << " (size=" << aSize << "):" << this;

	LOG_IF(WARNING,ATOMIC_FIFO_BUUFER<aSize) << "The Size " << aSize
														<< " byte is more then the atomic buffer length = "
														<< ATOMIC_FIFO_BUUFER;
	{
		NSHARE::CRAII<CMutex> _block(FMutexWrite);
		VLOG(2) << "Our turn Send :" << CThread::sMThreadId()
							<< " delta time " << (NSHARE::get_time() - _time)
							<< "sec :" << this;
		if (!MIsOpen())
		{
			VLOG(2) << "FIFO  not opened try opened for writing.";
			if (!MOpen(2.0))
				return sent_state_t(sent_state_t::E_NOT_OPENED,0);
		}
		uint8_t const* _send_buf=(uint8_t const*)aData;
		DWORD _write=0;
		std::size_t const _full_size= aSize;
		for (HANG_INIT; aSize && MIsOpen();aSize-=static_cast<size_t>(_write),_send_buf+=_write,HANG_CHECK)
		{
			VLOG(1) << "Write, len " << aSize;
			FOWriter.Offset=FOWriter.OffsetHigh=0;
			ResetEvent(FOWriter.hEvent);
			BOOL _is = FALSE;
			CHECK_LE(aSize, std::numeric_limits<DWORD>::max());
			_is = WriteFile(FHandler, _send_buf, static_cast<DWORD>(aSize), &_write, &FOWriter);
			if(_is==FALSE)
			{
				DWORD _error=GetLastError();
				if(_error== ERROR_IO_PENDING)
				{
					LOG(ERROR)<< "ERROR_IO_PENDING ";
				}
				else
				{
					LOG(ERROR)<< "Write error " << _error;
					MClose();
					return sent_state_t(sent_state_t::E_ERROR,_full_size-aSize);
				}
			}
			if(!_write)
			{
				if (!GetOverlappedResult(FHandler, &FOWriter, &_write, TRUE))
				{
					DWORD err = GetLastError();
					VLOG(2)<<" An error occurred "<<err;
					MClose();
					return sent_state_t(sent_state_t::E_ERROR,_full_size-aSize);
				}
				CHECK_NE(_write,0);
			}
			CHECK_LE(_write,aSize);
			;
		}
	}
	VLOG_IF(1,!aSize) << "Write all bytes to "
								<< FThis.MGetSetting().FPath << " within "
								<< (NSHARE::get_time() - _time) << "sec";
	return sent_state_t(sent_state_t::E_SENDED,aSize);
}
ssize_t CFifo::CImpl::MReceiveData(data_t* aBuf, const float aTime)
{
	double _time = NSHARE::get_time();
	VLOG(2) << "Receive data to " << aBuf << " from socket " << FHandler
						<< "; aTime " << aTime << ":" << this;
	if (!MIsOpen())
	{
		VLOG(2) << "Port is closed.";
		if (!MOpen(aTime))
			return -1;
	}
	DWORD cbBytesRead = 0;
	NSHARE::CRAII<CMutex> _block(FMutexRead);
	for (HANG_INIT;MIsOpen() && !cbBytesRead; HANG_CHECK)
	{

		VLOG(2) << "Our turn reading :" << CThread::sMThreadId() << ":"
		<< this;
		FOReader.Offset=FOReader.OffsetHigh=0;
		ResetEvent(FOReader.hEvent);
		BOOL _is = FALSE;
		_is = ReadFile(FHandler, FBufferPipe, sizeof(FBufferPipe), &cbBytesRead, &FOReader);
		if (_is == FALSE)
		{
			DWORD _error = GetLastError();
			if( (_error == ERROR_IO_PENDING))		//
			{
				VLOG(2)<<"The read operation is still pending";
			}
			else if (_error == ERROR_BROKEN_PIPE)
			{
				LOG(WARNING)<<"Pipe is disconnected.";
				MClose();
				return -1;
			}
			else
			{
				LOG(ERROR)<<"Unknown error "<<_error;
				return -1;
			}

		}

		if(!cbBytesRead)
		{
			if (GetOverlappedResult(FHandler, &FOReader, &cbBytesRead, TRUE)==FALSE)
			{
				DWORD err = GetLastError();
				VLOG(2)<<" An error occurred "<<err;
				return -1;
			}
			VLOG_IF(2,!cbBytesRead)<<"Wtf? Read "<<cbBytesRead<<" bytes.";
		}
	}
	VLOG(2) << "Read " << cbBytesRead << " bytes" << " delta time "
						<< (NSHARE::get_time() - _time) << " sec";

	data_t::const_iterator _it_beg((data_t::value_type*) FBufferPipe);
	aBuf->insert(aBuf->end(), _it_beg, _it_beg + cbBytesRead);
	return cbBytesRead;
}

const CSocket& CFifo::CImpl::MGetSocket(void) const
{
	return CSocket::sMNullSocket();
}
void CFifo::CImpl::MFlush(eFlush const&)
{
	FlushFileBuffers(FHandler);
}

} /* namespace NSHARE */
#endif//#ifdef _WIN32
