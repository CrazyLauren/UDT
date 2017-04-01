/*
 * CTCPMainChannel.cpp
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
#include <revision.h>
#include <share_socket.h>
#include <programm_id.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include <CCustomer.h>
#include <CLocalChannelFactory.h>
#include "receive_from.h"
#include <parser_in_protocol.h>
#include "CTCPMainChannel.h"
#include "CMainTcpRegister.h"
DECLARATION_VERSION_FOR(tcp_main_channel)
namespace NUDT
{
NSHARE::CText const CTCPMainChannel::NAME = E_MAIN_CHANNEL_TCP;
NSHARE::CText const CMainTcpRegister::NAME = "tcp_main";
using namespace NSHARE;
CTCPMainChannel::CTCPMainChannel() :
		ILocalChannel(NAME), FCustomer(NULL), FMainParser(this)
{
	FIsConnected = false;
	FThreadReceiver += NSHARE::CB_t(CTCPMainChannel::sMReceiver, this);
}

CTCPMainChannel::~CTCPMainChannel()
{
	MCloseImpl();
}
bool CTCPMainChannel::MOpen(IIOConsumer* aCustomer)
{
	FCustomer = aCustomer;
	FTcpClient.MOpen();
	return true;
}
bool CTCPMainChannel::MIsConnected() const
{
	return FTcpClient.MIsConnected();
}
bool CTCPMainChannel::MIsOpened() const
{
	return FTcpClient.MIsOpen();
}
void CTCPMainChannel::MClose()
{
	MCloseImpl();
}
void CTCPMainChannel::MCloseImpl()
{
	FTcpClient.MClose();
	FThreadReceiver.MCancel();
	FIsConnected = false;
}
int CTCPMainChannel::sMReceiver(void* aWho, void* aWhat, void*aData)
{
	reinterpret_cast<CTCPMainChannel*>(aData)->MReceiver();
	return 0;
}
void CTCPMainChannel::MReceiver()
{
	VLOG(2) << "Async receive";
	ISocket::data_t _data;
	for (HANG_INIT; FTcpClient.MIsOpen(); HANG_CHECK)
	{
		//pthread_testcancel();
		_data.clear();
		if (FTcpClient.MReceiveData(&_data, 0.0) > 0)
		{

			VLOG(2) << "Receiver " << _data.size() << " bytes";
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
void CTCPMainChannel::MFill<main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create main channel param DG";
	std::pair<size_t, size_t> const _pair = FTcpClient.MBufSize();
	size_t const _tcp_size =
		_pair.first > 0 && _pair.first < _pair.second ?
		_pair.first : _pair.second;
	main_ch_param_t _param;
	_param.FType = E_MAIN_CHANNEL_TCPSERVER;
	_param.FValue.MSet("limit", _tcp_size);
	serialize<main_channel_param_t, main_ch_param_t>(aTo, _param, routing_t(), error_info_t());
}

template<>
void CTCPMainChannel::MFill<user_data_info_t>(data_t* aData,
		user_data_info_t const& aInfo, data_t::size_type aSize)
{
	CHECK_NOTNULL(aData);
	VLOG(2) << "Filling user data DG for " /*<< aInfo.FDestName*/ << " aSize="
						<< aSize<< " Protocol:"
						<< aInfo.FProtocol;
	CHECK(aSize);
	serialize(aData,aInfo,aSize);

}
//fixme убрать
template<>
void CTCPMainChannel::MProcess(user_data_dg_t const* aP, void* aParser)
{
	CHECK_EQ(&FMainParser, aParser);
	VLOG(2) << "USER DATA: " << *aP;
	user_data_t _data;
	deserialize(_data,aP,NULL);
	LOG(INFO)<< "Receive packet #" << aP->MGetUserDataHeader().FNumber<<" from "<<aP->MGetFromUUID();
	FCustomer->MReceivedData(_data);
}
//
//----------
//
void CTCPMainChannel::MHandleServiceDG(main_channel_param_t const* aP)
{
	VLOG(2) << "Main channel handling \"Main channel parametrs\":";
	main_ch_param_t _sparam(deserialize<main_channel_param_t, main_ch_param_t>(aP, (routing_t*)NULL, (error_info_t*)NULL));
	CHECK_EQ(_sparam.FType, E_MAIN_CHANNEL_TCP);

	NSHARE::net_address _addr(_sparam.FValue);
	CHECK(_addr.MIsValid());

	FTcpClient.MClose();
	FTcpClient.MOpen(_addr);
	if (FTcpClient.MConnect())
	{
		FThreadReceiver.MCreate();
	}
}
int CTCPMainChannel::MSendMainChannelError(unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_TCPSERVER);

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(), get_my_id());


	return FCustomer->MSend(_channel);
}
void CTCPMainChannel::MHandleServiceDG(request_main_channel_param_t const* aP)
{
	VLOG(2) << "Request main channel param " << *aP;
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);

	data_t _buf;
	MFill<main_channel_param_t>(&_buf);

	int _is =FTcpClient.MSend(_buf).FError;
	(void) _is;
	LOG_IF(ERROR,_is<0) << "Cannot send main channel params.";
}
void CTCPMainChannel::MHandleServiceDG(close_main_channel_t const* aP)
{
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);

	MClose(); //todo
}
void CTCPMainChannel::MHandleServiceDG(main_channel_error_param_t const* aP)
{
	VLOG(2) << "main channel error " << *aP;
	FIsConnected = aP->FError == 0;
	LOG_IF(ERROR,!FIsConnected) << "Error during setting main channel.";
	MSendMainChannelError(main_channel_error_param_t::E_OK);
}
NSHARE::CBuffer CTCPMainChannel::MGetNewBuf(unsigned aSize) const
{
	return NSHARE::CBuffer(aSize);
}
bool CTCPMainChannel::MSend(user_data_t & aVal)
{
	data_t _head;
	MFill<user_data_info_t>(&_head,aVal.FDataId,aVal.FData.size());

	CHECK_NOTNULL(FCustomer);
	VLOG(2) << "Sending user data.";
	if (!FTcpClient.MIsOpen())
	{
		LOG(ERROR)<<"The Main channel is not opened";
		return false;
	}
	if (FTcpClient.MSend(_head).MIs())
	{
		if (FTcpClient.MSend(aVal.FData).MIs())
		{
			LOG(INFO)<<aVal.FData.size() << " bytes sent successfully";
			return true;
		}
		LOG(DFATAL)<<"Cannot send data to . Disconnecting it.";
		FCustomer->MClose();
		return false;
	}
	LOG(ERROR)<<"Cannot send data.";
	return false;
}

//
//--------
//

CMainTcpRegister::CMainTcpRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(MAJOR_VERSION_OF(tcp_main_channel), MINOR_VERSION_OF(tcp_main_channel), REVISION_OF(tcp_main_channel)))
{

}
void CMainTcpRegister::MUnregisterFactory() const
{
	CLocalChannelFactory::sMGetInstance().MRemoveFactory(CTCPMainChannel::NAME);
}
void CMainTcpRegister::MAdding() const
{
	//CTCPMainChannel* _p =
	CLocalChannelFactory::sMAddFactory<CTCPMainChannel>();
}
bool CMainTcpRegister::MIsAlreadyRegistered() const
{
	if (CLocalChannelFactory::sMGetInstancePtr())
		return CLocalChannelFactory::sMGetInstance().MIsFactoryPresent(
				CTCPMainChannel::NAME);
	return false;

}
}
#if !defined(TCP_MAIN_CHANNEL_STATIC)
static NSHARE::factory_registry_t g_factory;
extern "C" TCP_MAIN_CHANNEL_EXPORT NSHARE::factory_registry_t* get_factory_registry()
{
	if (g_factory.empty())
	{
		g_factory.push_back(new NUDT::CMainTcpRegister());
	}
	return &g_factory;
}
#else
#	include <load_static_module.h>
namespace
{
	static NUDT::CStaticRegister<NUDT::CMainTcpRegister> _reg;
}
#endif
