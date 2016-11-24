/*
 * CUDPMainChannel.cpp
 *
 *  Created on: 25.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <Socket.h>
#include <programm_id.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include <CCustomer.h>
#include <CLocalChannelFactory.h>
#include "receive_from.h"
#include <parser_in_protocol.h>
#include "CUDPMainChannel.h"
#include "CMainUdpRegister.h"

namespace NUDT
{
NSHARE::CText const CUDPMainChannel::NAME = "udp";
NSHARE::CText const CMainUdpRegister::NAME = "udp_main";
using namespace NSHARE;
CUDPMainChannel::CUDPMainChannel() :
		ILocalChannel(NAME), FCustomer(NULL), FMainParser(this)
{
	FIsConnected = false;
	FThreadReceiver += NSHARE::CB_t(CUDPMainChannel::sMReceiver, this);
}

CUDPMainChannel::~CUDPMainChannel()
{
	FUdp.MClose();
	if (FThreadReceiver.MCancel() && FThreadReceiver.MIsRunning())
		FThreadReceiver.MJoin();
}
bool CUDPMainChannel::MOpen(IIOConsumer* aCustomer)
{
	FCustomer = aCustomer;
	FUdp.MOpen();
	FThreadReceiver.MCreate();
	FIsConnected = false;
	return true;
}
bool CUDPMainChannel::MIsConnected() const
{
	return MIsOpened() && FUdp.MGetInitParam().FAddr.MIs() && FIsConnected;
}
bool CUDPMainChannel::MIsOpened() const
{
	return FUdp.MIsOpen();
}
void CUDPMainChannel::MClose()
{
	FUdp.MClose();
	FThreadReceiver.MCancel();
	FIsConnected = false;
}
int CUDPMainChannel::sMReceiver(void* aWho, void* aWhat, void*aData)
{
	reinterpret_cast<CUDPMainChannel*>(aData)->MReceiver();
	return 0;
}
void CUDPMainChannel::MReceiver()
{
	VLOG(2) << "Async receive";
	ISocket::data_t _data;
	for (HANG_INIT; FUdp.MIsOpen(); HANG_CHECK)
	{
		//pthread_testcancel();
		_data.clear();
		net_address _from;
		if (FUdp.MReceiveData(&_from, &_data, 0.0) > 0)
		{
			if (!FUdp.MGetInitParam().FAddr.MIs())
			{
				LOG(ERROR)<< "The default address is not setting";
				continue;
			}

			LOG_IF(ERROR,_from!=FUdp.MGetInitParam().FAddr.MGetConst()) << "Received " << _data.size()
													<< " bytes from unknown host "
													<< _from << ". Kernel ip "
													<< FUdp.MGetInitParam().FAddr.MGetConst();

			if (FUdp.MGetInitParam().FAddr.MGetConst() != _from)
				continue;
			VLOG(2) << "Receiver " << _data.size() << " bytes from "
								<< _from;
			ISocket::data_t const& _fix_data=_data;
			FMainParser.MReceivedData(_fix_data.begin(),_fix_data.end());
		}
	}
	VLOG(1) << "Socket closed";
}

//
//-----
//
template<>
void CUDPMainChannel::MFill<main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(main_channel_param_t));
	main_channel_param_t * _p = new (aTo->ptr()) main_channel_param_t();

	strcpy((char*) _p->FType, E_MAIN_CHANNEL_UDP);
	_p->FUdp.FAddr = INADDR_LOOPBACK;
	_p->FUdp.FPort = FUdp.MGetPort();
	fill_dg_head(_p, sizeof(main_channel_param_t),CCustomer::sMGetInstance().MGetID());
	VLOG(2) << (*_p);
}

template<>
void CUDPMainChannel::MProcess(user_data_dg_t const* aP, void* aParser)
{
	CHECK_EQ(&FMainParser, aParser);
	VLOG(2) << "USER DATA: " << *aP;
	user_data_t _data;
	deserialize(_data,aP,NULL);
	LOG(INFO)<< "Receive packet #" << aP->FUserHeader.FNumber<<" from "<<aP->FFromUUID;
	FCustomer->MReceivedData(_data);
}
//
//----------
//
void CUDPMainChannel::MHandleServiceDG(main_channel_param_t const* aP)
{
	VLOG(2) << "Main channel handling \"Main channel parametrs\":";
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);

	LOG_IF(WARNING,FUdp.MGetInitParam().FAddr.MIs())
															<< "The default address has already been set to "
															<< FUdp.MGetInitParam().FAddr.MGetConst();
	net_address _addr(aP->FUdp.FAddr, aP->FUdp.FPort);
	FUdp.MSetSendAddress(_addr);
}
void CUDPMainChannel::MHandleServiceDG(request_main_channel_param_t const* aP)
{
	VLOG(2) << "Request main channel param " << *aP;
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);

	data_t _buf;
	MFill<main_channel_param_t>(&_buf);

	int _is = FCustomer->MSend(_buf);
	(void) _is;
	LOG_IF(ERROR,_is<0) << "Cannot send main channel params.";
}
void CUDPMainChannel::MHandleServiceDG(close_main_channel_t const* aP)
{
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);

	MClose(); //todo
}
void CUDPMainChannel::MHandleServiceDG(main_channel_error_param_t const* aP)
{
	VLOG(2) << "main channel error " << *aP;
	FIsConnected = aP->FError == 0;
	LOG_IF(ERROR,!FIsConnected) << "Error during setting main channel.";
}
NSHARE::CBuffer CUDPMainChannel::MGetNewBuf(unsigned aSize) const
{
	return NSHARE::CBuffer(aSize);
}
bool CUDPMainChannel::MSend(user_data_t & aVal2)
{
	data_t _to;
	serialize(&_to,aVal2);
	CHECK_NOTNULL(FCustomer);
	VLOG(2) << "Sending user data.";
	if (!FUdp.MIsOpen())
	{
		LOG(ERROR)<<"The Main channel is not opened";
		return false;
	}
	bool _is = FUdp.MSend(_to).MIs();
	LOG_IF(INFO,_is) <<_to.size() << " bytes sent successfully";
	if (!_is)
	{
		LOG(ERROR)<<"Send error";
		net_address _kernel_ip;
		LOG_IF(ERROR,!FUdp.MGetInitParam().FAddr.MIs())
		<< "The kernel address is not setting";
	}
	return _is;
}

//
//--------
//

/* Changelog
 *
 *
 * Версия 0.1
 *	The first Release
 *
 **/

CMainUdpRegister::CMainUdpRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 1))
{

}
void CMainUdpRegister::MUnregisterFactory() const
{
	CLocalChannelFactory::sMGetInstance().MRemoveFactory(CUDPMainChannel::NAME);
}
void CMainUdpRegister::MAdding() const
{
	//CUDPMainChannel* _p =
	CLocalChannelFactory::sMAddFactory<CUDPMainChannel>();
}
bool CMainUdpRegister::MIsAlreadyRegistered() const
{
	if (CLocalChannelFactory::sMGetInstancePtr())
		return CLocalChannelFactory::sMGetInstance().MIsFactoryPresent(
				CUDPMainChannel::NAME);
	return false;

}
}
#if !defined(UDP_MAIN_CHANNEL_STATIC)
static NSHARE::factory_registry_t g_factory;
extern "C" UDP_MAIN_CHANNEL_EXPORT NSHARE::factory_registry_t* get_factory_registry()
{
	if (g_factory.empty())
	{
		g_factory.push_back(new NUDT::CMainUdpRegister());
	}
	return &g_factory;
}
#endif
