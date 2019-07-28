// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTCPSelectReceiver.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <Net.h>
#include <Socket/CNetBase.h>
#include <Socket/CLoopBack.h>
#include <string.h>
#include <console.h>
#include <Socket/CTCPSelectReceiver.h>
#include <Socket/print_socket_error.h>

namespace NSHARE
{
static inline char const* print_error()
{
	return print_socket_error().c_str();
}
CTCPSelectReceiver::CTCPSelectReceiver():
		FReceiveMutex(CMutex::MUTEX_NORMAL), //
		FTestMsg(4u, 0)
{
	FIsDoing=0;
}
void CTCPSelectReceiver::MRemoveAllClients()
{
	info_fd_t _fd;
	{
		CRAccsess _r = FSockets.MGetRAccess();
		_fd=_r.MGet();
	}
	info_fd_t::iterator _it = _fd.begin();
	for (; _it != _fd.end(); ++_it)
	{
		CSocket _tmp = *_it;
		MRemoveClient(_tmp);
	}
}
void CTCPSelectReceiver::MClose()
{
	VLOG(1) << "Close select receiver ";
	FIsDoing=0;
	MRemoveAllClients();

	if (FLoopBack.MIsOpen())
		MForceUnLock();

	VLOG(2)<<"Wait for receiver is stopped";
	CRAII<CMutex> _mutex(FReceiveMutex);

	if (FLoopBack.MGetSocket().MIsValid())
	{
		FSelectSock.MRemoveSocket(FLoopBack.MGetSocket());
		FLoopBack.MClose();
	}

	DCHECK(!FSelectSock.MIsSetUp());
	DCHECK(FSelectSock.MGetSockets().empty());
}
void CTCPSelectReceiver::MForceUnLock()
{
	sent_state_t const _val = FLoopBack.MSend(FTestMsg);
	VLOG_IF(1,_val.MIs()) << "Select unlocked successfully. ";
	LOG_IF(WARNING,!_val.MIs()) << "Cannot unlock select. ";
}
bool CTCPSelectReceiver::MIsOpen() const
{
	return FSelectSock.MIsSetUp();
}
bool CTCPSelectReceiver::MOpenSelectReceiver()
{
	if(MIsOpen())
		return false;
	CRAII<CMutex> _mutex(FReceiveMutex);

    if(!MOpenLoopSocket())
    	return false;

    FIsDoing = 1;

    VLOG(1) << "The server has been opened successfully.";
    return true;
}

/** Removes loopback socket and closed socket
 *
 * @param aTo A sockets list
 * @return first - is loopback exist
 * 		   second - the number of removed sockets
 */
std::pair<bool,unsigned> CTCPSelectReceiver::MFilteringSockets(CSelectSocket::socks_t& aTo)
{
	bool _is_loopback = false;
	unsigned _num_removed_sockets = 0;

	///fixme to function (and may be with code it is not needed)
	//the client can be closed
	CRAccsess _r = FSockets.MGetRAccess();
	const info_fd_t::const_iterator _end = _r->end();
	CSelectSocket::socks_t::iterator _it = aTo.begin();
	for (; _it != aTo.end(); ++_it)
	{
		if (!_is_loopback && (FLoopBack.MGetSocket() == *_it)) //looking for loopback socket
		{
			VLOG(1)  << "It's internal msg";
			_is_loopback = true;
			FLoopBack.MReadAll();
			*_it = CSocket();
			++_num_removed_sockets;
		}
		else
		if (_r->find(*_it) == _end)
		{
			VLOG(1)<< *_it << " has been closed already.";
			*_it = CSocket();
			++_num_removed_sockets;
		}
	}

	return std::make_pair(_is_loopback, _num_removed_sockets);
}

ssize_t CTCPSelectReceiver::MReceiveData(read_data_from_t*aFrom, data_t*aBuf, const float aTime)
{
	if (!FIsDoing.MIsOne())
	{
		LOG(ERROR)<< "The is not opened";
		return 0;
	}
	VLOG(2) << "Receive data to " << aBuf << ", max time " << aTime;
	int _recvd = 0;
	for (HANG_INIT; FIsDoing.MIsOne() && _recvd == 0; HANG_CHECK)
	{
		CRAII<CMutex> _mutex(FReceiveMutex);

		DCHECK(FSelectSock.MIsSetUp());

		VLOG(2) << "Wait for the data.";
		FTo.clear();
		int _val = FSelectSock.MWaitData(FTo, aTime);

		if(FIsDoing.MIsOne())
		{
			VLOG(2) << "Wait status " << _val<<" select sockets:"<<FTo.size();

			if (_val == 0) //timeout
			{
				VLOG(2) << "Timeout";
				return 0;
			}
			else if (_val < 0)
			{
				LOG(ERROR) << "Unknown error:" << print_error();
				continue;
			}
			else //if (_val > 0)
			{
				std::pair<bool,unsigned> const _rval=MFilteringSockets(FTo);
				bool const _is_loopback=_rval.first;
				unsigned const _num_removed_sockets=_rval.second;
				bool const _is_empty=_num_removed_sockets==FTo.size();

				VLOG(3)<<"Is empty "<<_is_empty<<" loopback:"<<_is_loopback;
				if( _is_empty && _is_loopback)
				{
					VLOG(2)<<"Only internal MSG has been received.";
					continue;
				}
				if(!_is_empty)
				{
					MReserveMemory(aBuf,FTo);
					_recvd=MReceiveFromAllSocket(*aFrom,*aBuf,FTo);
				}
				else
					_recvd=-1;
				VLOG(1) << "Resultant:Reads " << _recvd << " bytes";
			}
		}else
		{
			VLOG(2)<<"Port is closed";
		}
	}
	VLOG(2) << "End Receive";
	return _recvd;
}
bool CTCPSelectReceiver::MRemoveClient(CSocket& aSocket)
{
	VLOG(1) << "Close socket " << aSocket;
	if (!aSocket.MIsValid())
	{
		DLOG_IF(WARNING, !aSocket.MIsValid()) << "Socket is invalid";
		return false;
	}
	DCHECK_NE(FLoopBack.MGetSocket(), aSocket);

	FSelectSock.MRemoveSocket(aSocket);

	DLOG_IF(FATAL, !FLoopBack.MGetSocket().MIsValid())
																<< "Very interesting!, The Inside loop socket is closed, but  it is impossible";

	if (MRemoveClientImpl(aSocket))
	{
		MClientIsRemoved(aSocket);
		return true;
	}
	else
	{
		DLOG(FATAL) << "The socket is not removed";
		return false;
	}
}

bool CTCPSelectReceiver::MAddClient(CSocket& aSocket)
{
	VLOG(1) << "Add socket " << aSocket;
	if (!aSocket.MIsValid())
	{
		DLOG_IF(WARNING, !aSocket.MIsValid()) << "Socket is invalid";
		return false;
	}
	DCHECK_NE(FLoopBack.MGetSocket(), aSocket);

	FSelectSock.MAddSocket(aSocket);

	DLOG_IF(FATAL, !FLoopBack.MGetSocket().MIsValid())
	<< "Very interesting!, The Inside loop socket is closed, but  it is impossible";

	if (MAddClientImpl(aSocket))
	{
		MClientIsAdded(aSocket);
		MForceUnLock();
		return true;
	}
	else
	{
		DLOG(FATAL)<<"The socket is not added";
		return false;
	}
}
bool CTCPSelectReceiver::MRemoveClientImpl(
		CSocket& aSocket)
{
	CWAccsess _r = FSockets.MGetWAccess();
	return _r->erase(aSocket)>0;
}
bool CTCPSelectReceiver::MAddClientImpl(
		CSocket& aSocket)
{
	CWAccsess _r = FSockets.MGetWAccess();
	return _r->insert(aSocket).second;
}
int CTCPSelectReceiver::MReceiveFromAllSocket(read_data_from_t& aFrom,data_t& aBuf,
		CSelectSocket::socks_t & _to)
{
	CSelectSocket::socks_t::iterator _it = _to.begin();
	int _recvd = 0;
	for (; _it != _to.end(); ++_it)
	{
		if(_it->MIsValid())
		{
			DCHECK_NE(*_it, FLoopBack.MGetSocket());
			socket_info_t _result;
			_result.FSocket=*_it;

			int _size = MReadData(&aBuf, *_it,_result.FDiagnostic);
			if(_size<=0)
				MRemoveClient(*_it);
			else
			{
				_result.FSize=_size;
				_recvd += _size;
				aFrom.push_back(_result);
			}
		}
	}
	return _recvd > 0 ? _recvd : -1;
}
/** Reserves space in the buffer which is enough to receive all data
 * without memory reallocation
 *
 * @param aBuf A buffer
 * @param aSockets the number sockets from each the data will be received
 */
void CTCPSelectReceiver::MReserveMemory(data_t* aBuf,
		CSelectSocket::socks_t const& aSockets)
{
	size_t _available = 0;
	CSelectSocket::socks_t::const_iterator _it = aSockets.begin();
	for (; _it != aSockets.end(); ++_it)
	{
		_available +=_it->MIsValid()?CNetBase::MAvailable(*_it):0;
	}

	VLOG(2) << "Available for reading " << _available << ", The capacity is "
	<< aBuf->capacity();

	_available += aBuf->size();

	if (_available > aBuf->capacity())
		aBuf->reserve(_available);
}
size_t  CTCPSelectReceiver::MAvailable() const
{
	size_t _rval = 0;
	CRAccsess _r = FSockets.MGetRAccess();
	for (info_fd_t::const_iterator _it = _r->begin(); _it != _r->end();
			++_it)
		_rval += _it->MIsValid()?CNetBase::MAvailable(*_it):0;

	VLOG(2) << _rval << " bytes available for reading from all clients";
	return _rval;
}
/** Open loop socket
 *
 * @return
 */
bool CTCPSelectReceiver::MOpenLoopSocket()
{
	bool const _rval = FLoopBack.MOpen();
	LOG_IF(DFATAL,!_rval)<<"Cannot open loop back socket ";

	if(_rval)
	{
		FSelectSock.MAddSocket(FLoopBack.MGetSocket());
	}
	return _rval;
}

CTCPSelectReceiver::socket_info_t::socket_info_t()://
		FSize(0)
{
	;
}
}



