/*
 * CAsynchronousSocket.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 14.03.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <socket.h>

namespace NSHARE
{
CAsyncSocket::CAsyncSocket(ISocket*& aSock) :
		FSock(aSock)
{
	FThreadReceiver += CB_t(CAsyncSocket::sMReceiver, this);
	FIsWorking=false;
}
static ISocket* g_null_soket = NULL;
CAsyncSocket::CAsyncSocket() :
		FSock(g_null_soket)
{
	FThreadReceiver += CB_t(CAsyncSocket::sMReceiver, this);
	FIsWorking=false;
}
CAsyncSocket::~CAsyncSocket()
{
	MStop();
}
bool CAsyncSocket::MStart(const CThread::param_t* aP)
{
	return FThreadReceiver.MCreate(aP);
}

bool CAsyncSocket::MCancel()
{
	FIsWorking=false;
	return FThreadReceiver.MCancel();
}
void CAsyncSocket::MStop()
{
	if (MCancel())
		MJoin();
	FIsWorking=false;
}
bool CAsyncSocket::MJoin()
{
	if (FThreadReceiver.MIsRunning())
		return FThreadReceiver.MJoin();
	return false;
}
int CAsyncSocket::sMReceiver(void* aWho, void* aWhat, void*aData)
{
	reinterpret_cast<CAsyncSocket*>(aData)->MReceiver();
	return 0;
}
void CAsyncSocket::MReceivedData(const ISocket::data_t& aData)
{
	//MCall(const_cast<cb_arg_t*>(&aData));
}
void CAsyncSocket::MReceivedData(const ISocket::data_t& aData,
		const ISocket::recvs_from_t&aFrom)
{
	MReceivedData(aData);
}
ISocket* CAsyncSocket::MGetSocket()
{
	return FSock;
}
ISocket const* CAsyncSocket::MGetSocket() const
{
	return FSock;
}
void CAsyncSocket::MReceiver()
{
	LOG_IF(ERROR,FIsWorking)<<"Async socket has worked already.";
	if(FIsWorking)
		return;
	FIsWorking=true;

	VLOG(2) << "Async receive";
	for (; !MGetSocket(); usleep(1000))
		VLOG(1) << "Socket is null";
	ISocket::data_t _data;
	int _count = 0;
	for (;FIsWorking&& MGetSocket() && MGetSocket()->MIsOpen();)
	{
		//pthread_testcancel();
		_data.clear();
		ISocket::recvs_from_t _from;
		if (MGetSocket()->MReceiveData(&_data, 0.0, &_from) > 0 && FIsWorking)
		{
			if(_data.empty())
				continue;
			_count = 0;
			//pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			MReceivedData(_data, _from);
			//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		}
		else
		{
			++_count;
			sleep(1);
			if (_count > 5)
				break;
		}
	}
	VLOG(1) << "Socket closed";
}
}
