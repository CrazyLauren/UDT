// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSeleclSocketPosix.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <SHARE/Socket/ISocket.h>
#include <SHARE/Socket/CSocket.h>
#include <SHARE/Socket/CSelectSocket.h>
#include <SHARE/Socket/CSeleclSocketPosix.h>
#include "print_socket_error.h"
#if (defined(unix)|| defined(__QNX__)) && !defined(__MINGW32__)
#	include <netinet/in.h> // sockaddr_in
#	include <sys/time.h>
#endif
namespace NSHARE
{
CSelectSocket::CImpl::CImpl(CSelectSocket& aThis) :
		FThis(aThis)
{
	memset(&readset, 0, sizeof(readset));
	memset(&writeset, 0, sizeof(writeset));
	FD_ZERO(&readset);
	FD_ZERO(&writeset);
}
int CSelectSocket::CImpl::MWaitData(socks_t& aTo, float const aTime,
		unsigned aType,socks_t* aToWrite) const
{
	struct timeval _sleep;
	struct timeval* _psleep = &_sleep;

	if (aTime > 0.0)
		NSHARE::convert(&_sleep, aTime);
	else
		_psleep = NULL;

	FD_ZERO(&readset);
	FD_ZERO(&writeset);

	CSocket::socket_t _max = 0;
	CFlags<eSocketType, unsigned> _flags(aType);

	DCHECK(
			(_flags.MGetFlag(E_READ_ONLY)&& _flags.MGetFlag(E_WRITE_ONLY) && aToWrite !=NULL)//
			|| (!_flags.MGetFlag(E_READ_ONLY) && _flags.MGetFlag(E_WRITE_ONLY) && aToWrite==NULL)//
			|| (_flags.MGetFlag(E_READ_ONLY) && !_flags.MGetFlag(E_WRITE_ONLY) && aToWrite==NULL)//
		);//
	bool _is = false;
	std::vector<CSocket> _sockets;
	{
		CRAII<CMutex> _lock(FThis.FMutex);
		_sockets = FThis.FFds;
	}
	for (socks_t::iterator _it = _sockets.begin(); _it != _sockets.end(); ++_it)
	{
		VLOG(2) << "Add socket " << *_it;
		if (_flags.MGetFlag(NSHARE::E_READ_ONLY) //
		&& _it->MGetFlag(NSHARE::E_READ_ONLY) //
				)
		{
			_is = true;
			FD_SET(_it->MGet(), &readset);
			_max = _max < _it->MGet() ? _it->MGet() : _max;
			VLOG(2) << "Add socket " << *_it << " to  read select";
		}

		if (_flags.MGetFlag(NSHARE::E_WRITE_ONLY) //
		&& _it->MGetFlag(NSHARE::E_WRITE_ONLY) //
				)
		{
			_is = true;
			FD_SET(_it->MGet(), &writeset);
			_max = _max < _it->MGet() ? _it->MGet() : _max;
			VLOG(2) << "Add socket " << *_it << " to write select";
		}
	}
	VLOG(2) << "Max socket =" << _max;
	if (!_is)
	{
		LOG(ERROR)<<"Socket list is empty.";
		return -1;
	}
	int n = 0;

	if (_flags.MGetFlag(NSHARE::E_READ_ONLY)
			&& _flags.MGetFlag(NSHARE::E_WRITE_ONLY))
	{
		VLOG(2) << "RW select";
		n = select(_max + 1, &readset, &writeset, NULL, _psleep);
	}
	else if (_flags.MGetFlag(NSHARE::E_READ_ONLY))
	{
		VLOG(2) << "RO select";
		n = select(_max + 1, &readset, NULL, NULL, _psleep);
	}
	else if (_flags.MGetFlag(NSHARE::E_WRITE_ONLY))
	{
		VLOG(2) << "WO select";
		n = select(_max + 1, NULL, &writeset, NULL, _psleep);
	}
	else
		MASSERT_1(false);

	VLOG(2) << "select status" << n;
	if (n < 0)
	{
		LOG(ERROR)<<"Select error"<<print_socket_error();
		return n;
	}
	else if (n == 0)
	{
		VLOG(2) << "select timeout";
		return 0;
	}
	for (socks_t::iterator _it = _sockets.begin(); _it != _sockets.end(); ++_it)
	{
		bool const _is_read=FD_ISSET(_it->MGet(),&readset)!=0;
		bool const _is_write=FD_ISSET(_it->MGet(),&writeset)!=0;

		if (_is_read)
			aTo.push_back(*_it);

		if (_is_write)
		{
			if (_flags.MGetFlag(E_READ_ONLY))
				aToWrite->push_back(*_it);
			else
				aTo.push_back(*_it);
		}
	}
	return n;
}

}

