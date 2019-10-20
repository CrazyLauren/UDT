// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSelectSocket.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share/Socket/ISocket.h>
#include <share/Socket/CSocket.h>
#include <share/Socket/CSelectSocket.h>
#ifdef _WIN32
#include <share/Socket/CSeleclSocketPosix.h>
#else
#include <share/Socket/CSeleclSocketPosix.h>
#endif
//#if (defined(unix)|| defined(__QNX__)) && !defined(__MINGW32__)
//#	include <netinet/in.h> // sockaddr_in
//#	include <sys/time.h>
//#else
//#include <winsock2.h>
//#endif//#include <deftype>
#include "print_socket_error.h"
namespace NSHARE
{
CSelectSocket::CSelectSocket() :
		FImpl(new CImpl(*this)), FIsSetUp(false)
{
	VLOG(2) << "Construct CSelectSocket. :" << this;
}

CSelectSocket::~CSelectSocket()
{
	VLOG(2) << "Destruct CSelectSocket: " << this;
	delete FImpl;
}
void CSelectSocket::MAddSocket(CSocket const& aSocket)
{
	VLOG(1) << "Add new socket " << aSocket;

	LOG_IF(INFO,!aSocket.MIsValid()) << "Adding  Invalid socket.";
	CRAII<CMutex> _lock(FMutex);
	FFds.push_back(aSocket);
	FIsSetUp = true;
}
void CSelectSocket::MRemoveAll()
{
	for (HANG_INIT; !FFds.empty();HANG_CHECK)
		MRemoveSocket(FFds.back());
}
void CSelectSocket::MRemoveSocket(CSocket const& aSocket)
{
	VLOG(1) << "Remove socket " << aSocket;
	CRAII<CMutex> _lock(FMutex);
	LOG_IF(WARNING,FFds.empty()) << "Removing socket from empty select";

	if (FFds.empty())
		return;

	typedef std::vector<std::vector<CSocket>::iterator> _its_t;
	_its_t _its;

	
//fixme socket has to be only one

	for (std::vector<CSocket>::iterator _it = FFds.begin(); _it != FFds.end();
			++_it)
	{
		if (*_it == aSocket)
			_its.push_back(_it);
	}
	
	LOG_IF(WARNING,_its.empty()) << "Socket" << aSocket << " is not founded";
	VLOG_IF(0,_its.size()>1) << "There are equal id of socket.";

	if (_its.size() > 1)
	{
		for (_its_t::iterator _it = _its.begin(); _it != _its.end(); ++_it)
		{
			if ((*_it)->FOrderNumber == aSocket.FOrderNumber)
			{
				FFds.erase(*_it);
			}
		}
	}
	else if(_its.size()==1)
	{
		FFds.erase(_its.front());
	}

	if (FFds.empty())
		FIsSetUp = false;
}
int CSelectSocket::MWaitData(socks_t& aTo, float const aTime,
		unsigned aType,socks_t* aToWrite) const
{
	VLOG(2) << "Wait " << aTime << " sec for  reading data available.";
	if (!FIsSetUp)
	{
		LOG(ERROR)<<"Select has not setted up yet.";
		return -1;
	}
	return FImpl->MWaitData(aTo, aTime, aType,aToWrite);
}
} /* namespace NSHARE */
