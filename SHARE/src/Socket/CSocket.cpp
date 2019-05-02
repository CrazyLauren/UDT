// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSocket.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifdef _WIN32
#include <winsock2.h>
#endif
#include <deftype>
#include <Socket/CSocket.h>
namespace NSHARE
{
unsigned CSocket::g_counter = 0;
CSocket::CSocket(socket_t aFd,unsigned aFlags) ://fixme FIsCloseOnDestructor - for smart ptr
		CSocketFlags(aFlags),FIsCloseOnDestructor(false), FOrderNumber(++g_counter), FFd(aFd)
{
	VLOG_IF(2,aFd>=0) << "The Socket equal " << aFd;
}
CSocket::CSocket(CSocket const& aRht) :
		CSocketFlags(aRht),FOrderNumber(aRht.FOrderNumber)
{
	operator=(aRht);
}
CSocket& CSocket::operator=(socket_t aFd)
{
	FFd=aFd;
	return *this;
}
CSocket& CSocket::operator=(CSocket const&aRht)
{
	FFd = aRht.FFd;
	FIsCloseOnDestructor = aRht.FIsCloseOnDestructor;
	*const_cast<unsigned*>(&FOrderNumber) = aRht.FOrderNumber;
	return *this;
}

CSocket::~CSocket()
{
	if (MIsValid() && FIsCloseOnDestructor)
		MCloseImpl();
}

bool CSocket::sMIsValid(socket_t aFd)
{
	return aFd >= 0;
}
bool CSocket::MIsValid() const
{
	return sMIsValid(FFd);
}

CSocket::socket_t CSocket::MGet(void) const
{
	return FFd;
}

void CSocket::MSet(socket_t aFd, eSocketType aType)
{
	VLOG_IF(2, aFd >= 0) << "The Socket equal " << aFd;
	FFd = aFd;
	MReset();
	MSetFlag(aType, true);
}
void CSocket::MClose()
{
	MCloseImpl();
}
void CSocket::MCloseImpl()
{
	VLOG(2) << "Close socket " << FFd;
	if (MIsValid())
	{
		socket_t _tmp=FFd;//Than closing  multithread programm
		//The CSocket can be valid, but the socket has been closed.
		//Therefore the first is making invalid CSocket, than closing the socket.
		MSet(-1, E_READ_ONLY);
#ifndef _WIN32
		::close(_tmp);
#else
		::closesocket(_tmp);
#endif
	}
}
CSocket const& CSocket::sMNullSocket()
{
	static CSocket _fix;
	return _fix;
}
}
