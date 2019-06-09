// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CFIFOServerPosixImpl.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef _WIN32
#include <deftype>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
//#include <ev.h>
#include <fdir.h>
#include <Pipe.h>
#include <Socket/CFIFOServerPosixImpl.h>

#define PERM 0666
using namespace std;
namespace NSHARE
{
const unsigned CFIFOServer::ATOMIC_FIFO_BUUFER = PIPE_BUF;
inline char *print_error()
{
	return strerror(errno);
}
CFIFOServer::CImpl::CImpl(CFIFOServer& aThis) :
		FThis(aThis)
{
	VLOG(2) << "Construct CFIFOServer::CImpl: " << this;
	FIsOpen = 0;
}

CFIFOServer::CImpl::~CImpl()
{
	VLOG(2) << "Destruct CFIFOServer::CImpl: " << this;
	MClose();
}
bool CFIFOServer::CImpl::MIsMade() const
{
	return NSHARE::is_the_file_exist(FThis.MGetSetting().FPath.MToStdString());
}
void CFIFOServer::CImpl::MCloseAllWriteCannnel()
{
	VLOG(2) << "Close  all write fifo: " << this;
	clients_t::const_iterator _it = FThis.MGetSetting().FClients.begin();
	for (; _it != FThis.MGetSetting().FClients.end(); ++_it)
		MClose(*_it);
	LOG_IF(DFATAL,!FFDWrites.empty()) << "Not all fifo has been closed.";
}
void CFIFOServer::CImpl::MCloseReadCannel()
{
	VLOG(2) << "Close  read fifo: " << this;
	FSelect.MRemoveSocket(FFdRead);
	if (MClose(FFdRead))
	{
		LOG(ERROR)<< "Error while closing fifo:" << FFdRead << " Error:"
		<< print_error();
		return;
	}
	MClose(FFdFixWrite);
	LOG(ERROR)<< "Error while closing fifo:" << FFdRead << " Error:"
	<< print_error();
	MUnlink(FThis.MGetSetting().FPath);
}

void CFIFOServer::CImpl::MClose()
{
	VLOG(2) << "Close fifo socket: " << this;
	if (!MIsOpen())
	{
		LOG(ERROR)<< "FIFO has already been close.";
		return;
	}
	MCloseAllWriteCannnel();
	MCloseReadCannel();

	FIsOpen = false;
	VLOG(1) << "Fifo " << FThis.MGetSetting().FPath << "  is closed.";
}
void CFIFOServer::CImpl::MClose(CFIFOServer::path_t const& aClientPath)
{
	VLOG(2) << "Client is closing " << aClientPath;
	write_fd_t::iterator _it=FFDWrites.find(aClientPath);
	if(_it==FFDWrites.end())
	{
		VLOG(2)<<aClientPath<<" is not exist";
		return;
	}
	if (!_it->second.FWSocket.MIsValid())
	{
		LOG(WARNING)<< "Port is open but fd is invaid";
		return;
	}
	else if (MClose(_it->second.FWSocket))
	{
		LOG(ERROR)
		<< "Error while closing fifo:" << aClientPath << " Error:"
		<< print_error();

	}
	FFDWrites.erase(aClientPath);

	//unlink //TODO
}
inline void CFIFOServer::CImpl::MUnlink(CText const& aClientPath)
{
	VLOG(2) << "Unlink " << aClientPath << ":" << this;
	if (unlink(aClientPath.c_str()) < 0)
		LOG(ERROR)<< "Can't unlink " << aClientPath.c_str() << ". "
		<< print_error() << "(" << errno<<").";
	}
inline int CFIFOServer::CImpl::MClose(CSocket& _s)
{
	VLOG(2) << "Close " << _s << ":" << this;
	_s.MClose();
	return 0;
}
inline bool CFIFOServer::CImpl::MOpenReadCanncel()
{
	VLOG(2)
			<< "Open read fifo \"" << FThis.MGetSetting().FPath << "\" :"
					<< this;
	NSHARE::CText _path = FThis.MGetSetting().FPath;
	CHECK(!_path.empty()) << "The path is empty.";
	LOG_IF(DFATAL,MIsOpen()) << "FIFO has already been opened.";
	VLOG_IF(1, MIsMade()) << "File " << _path << " is exist.";
	//unlink(_path.c_str()); //just in case

	if (mkfifo(_path.c_str(), PERM) < 0)
	{
		if (errno!=EEXIST)
		{
			LOG(DFATAL)
			<< "Can't open fifo:" << _path << " Error:" << print_error();
			MUnlink(_path);
			return false;
		}
		else
		LOG(WARNING)<< "File " << _path << " is exist.";
	}
	FFdRead = open(_path.c_str(), O_RDWR | O_NONBLOCK, 0);
	if (!FFdRead.MIsValid())
	{
		LOG(DFATAL)<< "Can't open fifo:" << _path << " Error:" << print_error();
		return false;
	}
	else
	{
		FFdFixWrite=open(_path.c_str(), O_WRONLY | O_NONBLOCK, 0);
		FSelect.MAddSocket(FFdRead);
	}
	FIsOpen = true;

	return true;
}
bool CFIFOServer::CImpl::MOpen()
{
	VLOG(2) << "Open socket :" << this;
	if (!MOpenReadCanncel())
		return false;
	if (!MReconfigure())
		MClose();
	return FIsOpen;
}
ssize_t CFIFOServer::CImpl::MReceiveData(data_t * aBuf, float const aTime,
		CFIFOServer::recvs_from_t *aFrom)
{
	VLOG(2)
			<< "Receive data to " << aBuf << " from socket " << FFdRead
					<< "; aTime " << aTime << ":" << this;

	if (!MIsOpen())
	{
		LOG(ERROR)<< "Port is closed.";
		return 0;
	}
	NSHARE::CRAII<CMutex> _block(FMutexRead);
	int _len = 0;
	for (HANG_INIT;; HANG_CHECK)
	{
		//NSHARE::CRAII<CMutex> _block(FMutexRead);
		double _time = NSHARE::get_time();
		VLOG(2)
				<< "Our turn reading :" << CThread::sMThreadId() << ":" << this;
		CSelectSocket::socks_t _to;
		int _rval = FSelect.MWaitData(_to, aTime);
		VLOG(2)
				<< "Wait status " << _rval << " delta time "
						<< (NSHARE::get_time() - _time) << " sec";

		if (_rval == 0)
			return 0;
		else if (_rval < 0)
		{
			MCloseReadCannel();
			return 0;
		}

		const size_t _befor = aBuf->size();
		VLOG(2) << "Befor " << _befor << " bytes";
		const size_t _avalable = MAvailable();
		VLOG_IF(1,(!_avalable))
				<< "No data on socket " << _to.front()
						<< ", may be it has been closed already."
								"Assuming the size equal 1 byte.";
		const size_t _size = (_avalable ? _avalable : 1) + _befor;
		aBuf->resize(_size);
		data_t::value_type* _pbegin = (data_t::value_type*) aBuf->ptr()
				+ _befor;
		errno=0;
		_len = read(FFdRead, _pbegin, _avalable);
		VLOG(2)
				<< "Read " << _len << " bytes time :"
						<< (NSHARE::get_time() - _time) << " sec";
		//Set real size
		if (_len > 0)
			aBuf->resize(_befor + _len);
		else
			aBuf->resize(_befor);

		if (_len == 0)
		{
			VLOG(1) << "Fifo not open for writing ";
			return 0;
		}
		else if (_len < 0)
		{
			LOG_IF(ERROR, _len<0) << "Read error " << print_error();
			if (errno==EAGAIN )
			continue; //HZ what is it

			MCloseReadCannel();
			return 0;
		}
		else
			break;
	}
	LOG_IF(WARNING,!aFrom) << "Receive from is not support for fifo socket";
	return _len;
}
bool CFIFOServer::CImpl::MReconfigure()
{
	VLOG(2) << "Reconfigure socket :" << this;
	clients_t _clients = FThis.MGetSetting().FClients;
	VLOG_IF(1,_clients.empty()) << " There are not clients.";
	if (_clients.empty())
		return true; //open канал;

	//Looking for opened clients what there is in clients_t.
	it_write_fd_t _it = FFDWrites.begin();

	for (; _it != FFDWrites.end(); )
	{
		LOG_IF(WARNING,!_it->second.FWSocket.MIsValid())
				<< "The Socket for " << _it->first << " isn't valid";

		clients_t::iterator _jt = _clients.begin();
		for (; _jt != _clients.end(); ++_jt)
			if (_it->second.FClient== *_jt)
				break;

		if (_jt != _clients.end())
		{
			//erase from _clients
			_clients.erase(_jt);
		}
		else
		{
			//close client
			if (_it != FFDWrites.begin())
			{
				path_t _client = _it->first;
				--_it;
				MClose(_client);
			}
			else
			{
				MClose(_it->first);
				_it = FFDWrites.begin();
				continue;
			}
		}
		++_it;
	}
	//add new client

	clients_t::iterator _jt = _clients.begin();
	for (; _jt != _clients.end(); ++_jt)
		MAddNewClient(*_jt);

	return true;

}
void CFIFOServer::CImpl::MAddNewClient(CFIFOServer::path_t const& aTo)
{
	VLOG(1) << " Add new client " << aTo;
	NSHARE::CText _path = aTo;
	CHECK(!_path.empty()) << "The path is empty.";

	LOG_IF(DFATAL,MGetWriteFd(aTo).MIsValid())
			<< "FIFO " << aTo << " has already been opened.";

	LOG_IF(ERROR, !NSHARE::is_the_file_exist(_path.MToStdString()))<< "File " << _path << " isn't exist.";

	if (mkfifo(_path.c_str(), PERM) < 0)
	{
		if (errno!=EEXIST)
		{
			LOG(DFATAL)
			<< "Can't open fifo:" << _path << " Error:" << print_error();
			MUnlink(_path);
			return;
		}
		else
		VLOG(2)<< "File " << _path << " is exist.";
	}
	_write_t _n;
	_n.FClient=aTo;
	_n.FTime = ::time(NULL);
	_n.FWSocket=open(_path.c_str(), O_WRONLY | O_NONBLOCK, 0);
	FFDWrites[aTo] = _n;
//	CSocket _s = open(_path.c_str(), O_WRONLY | O_NONBLOCK, 0);
//	if (!_s.MIsValid())
//	{
//		LOG(DFATAL)<< "Can't open fifo:" << _path << " Error:" << print_error();
//		return;
//	}
//	SHARED_PTR<client_t> _n(new client_t);
//	_n->FClient = aTo;
//	_n->FTime = ::time(NULL);
//	FFDWrites[_s] = _n;
}

bool CFIFOServer::CImpl::MIsOpen() const
{
	return FIsOpen;
}

size_t CFIFOServer::CImpl::MAvailable() const
{

	int bytes = 0;
	errno=0;
	if (ioctl(FFdRead.MGet(), FIONREAD, &bytes) != 0)
	{
		LOG(ERROR)<< "Ioctl error" << print_error();
		return 0;
	}
	VLOG(2) << bytes << " bytes available for reading from " << FFdRead;
	return static_cast<size_t>(bytes);
}
ISocket::sent_state_t CFIFOServer::CImpl::MSend(void const* const aData, std::size_t aSize,
		NSHARE::CConfig const& aTo)
{
	client_t _rht(aTo);
	LOG_IF(DFATAL,!_rht.MIsValid()) << "Invalide type of smart_addr";
	if (!_rht.MIsValid())
		return sent_state_t(sent_state_t::E_ERROR,0);
	return MSend(aData, aSize, _rht.FClient);
}
ISocket::sent_state_t CFIFOServer::CImpl::MSend(void const* const aData, std::size_t aSize,
		CFIFOServer::path_t const& aTo)
{
	double _time = NSHARE::get_time();
	VLOG(2)
			<< "Send data " << aData << " (size=" << aSize << ") to " << aTo
					<< ":" << this;

	LOG_IF(WARNING,ATOMIC_FIFO_BUUFER<aSize)
			<< "The Size " << aSize
					<< " byte is more then the atomic buffer length = "
					<< ATOMIC_FIFO_BUUFER;
	std::size_t const _full_size=aSize;
	{
		NSHARE::CRAII<CMutex> _block(FMutexWrite);
		VLOG(2)
				<< "Our turn Send :" << CThread::sMThreadId() << " delta time "
						<< (NSHARE::get_time() - _time) << "sec :" << this;
		CSocket _socket = MGetWriteFd(aTo);
		DCHECK(_socket.MIsValid()) << "Unknown client " << aTo;

		if (!_socket.MIsValid())
			return sent_state_t(sent_state_t::E_ERROR,0);

		int _err_count = 0;
		for (HANG_INIT; aSize; HANG_CHECK)
		{
			errno=0;
			int _len = write(_socket, aData, aSize);
			VLOG(2)<<"Write "<<_len<<" within "<< (NSHARE::get_time() - _time) << "sec";
			if (_len < 0)
			{
				LOG(ERROR)<< "Write error " << print_error();

				switch (errno)
				{
					case EAGAIN:
					LOG(ERROR) <<"FIFO is Full.";
					continue;
					break;

					case EPIPE:
					LOG(ERROR) <<"FIFO isn't open for reading.";
					break;

					default:
					break;
				}
				++_err_count;
				if(_err_count>5)
					return sent_state_t(sent_state_t::E_ERROR,_full_size-aSize);
			}
			else
			{
				CHECK_GE(aSize,_len)<<"O_O I don't known, why it happened.";
				aSize-=_len;
			}
			CHECK(_len)<<"Len can't be equal 0";

			VLOG_IF(1,aSize) << "Repeat write, len " << _len;
			if(!aSize)
			break;
		}
	}
	return sent_state_t(sent_state_t::E_SENDED,_full_size-aSize);
}

const CSocket& CFIFOServer::CImpl::MGetSocket() const
{
	return FFdRead;
}
void CFIFOServer::CImpl::MFlush(const eFlush&)
{
	LOG(WARNING)<< "FIFO flush method is not implemented.";
}
CSocket CFIFOServer::CImpl::MGetWriteFd(CFIFOServer::path_t const& aVal) const
{
	cit_write_fd_t _it = FFDWrites.find(aVal);
	if (_it == FFDWrites.end())
	{
		VLOG(2) << aVal << " is not exist";
		return -1;
	}

	if (!_it->second.FWSocket.MIsValid())
	{
		VLOG(2) << "Try open client " << aVal;
		_it->second.FWSocket = open(aVal.c_str(), O_WRONLY | O_NONBLOCK, 0);
		LOG_IF(ERROR,!_it->second.FWSocket.MIsValid())<<"Cannot open client "<<aVal;
	}
	return _it->second.FWSocket;
}
void CFIFOServer::CImpl::MSerialize(NSHARE::CConfig & aConf) const
{
	write_fd_t::const_iterator _it(FFDWrites.begin()), _it_end(FFDWrites.end());
	for(;_it!=_it_end;++_it)
	{
		NSHARE::CConfig  _conf("cl");
		_conf.MAdd("path",_it->first);
		_conf.MAdd("time",_it->second.FTime);
		aConf.MAdd(_conf);
	}
}
} // namespace NSHARE
#endif //#ifndef _WIN32
