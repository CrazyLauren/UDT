/*
 * CKernelIOByTCPClientClient.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 27.07.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>
#include <string.h>
#include <programm_id.h>
#include <internel_protocol.h>
#include <core/kernel_type.h>
#include <core/CConfigure.h>
#include <core/CDescriptors.h>

#include <io/CKernelIo.h>
#include <io/ILink.h>
#include "ILinkBridge.h"
#include "CConnectionHandlerFactory.h"
//#include "receive_from.h"
//#include <parser_in_protocol.h>

#include "CKernelIOByTCPClient.h"
#include "CKernelChannel.h"

using namespace NSHARE;
namespace NUDT
{
NSHARE::CText const CKernelIOByTCPClient::NAME = "tcp_client_io_manager";



CKernelIOByTCPClient::CKernelIOByTCPClient() :
		ITcpIOManager(NAME)
{

	FIo = NULL;
}


CKernelIOByTCPClient::~CKernelIOByTCPClient()
{
	MClose();
}
void CKernelIOByTCPClient::MInit(CKernelIo * aVal)
{
	VLOG(2)<<"Init";
	CHECK_NOTNULL(aVal);
	CHECK(FIo==NULL);
	FIo=aVal;
}
void CKernelIOByTCPClient::MCloseRequest(const descriptor_t& aFrom)
{
	CHECK_NOTNULL(FIo);
	FIo->MClose(aFrom);
}
void CKernelIOByTCPClient::MClose(const descriptor_t& aFrom)
{
	smart_channel_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		active_channels_t::iterator _it=FActiveChannels.find(aFrom);
		if(_it!=FActiveChannels.end())
		{
			_channel=(_it->second);
		}
	}
	if(_channel.MIs())
	{
		_channel->MClose();
	}
}
void CKernelIOByTCPClient::MClose()
{
	for (unsigned i = 0; i < FChannels.size(); ++i)
	{
		FChannels[i]->MClose();
	}
}

bool CKernelIOByTCPClient::MOpen(const void* aP)
{
	VLOG(2) << "Open KernelIOByTcp";
	CHECK_NE(CConfigure::sMGetInstance().MGet().MKey(),NAME);
	CConfig const* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);
	LOG_IF(ERROR,!_p)<<CKernelIOByTCPClient::NAME<<" is not exist";
	if (_p)
	{
		ConfigSet const _childs = _p->MChildren(CKernelChannel::NAME);
		LOG_IF(WARNING,_childs.empty()) << "There are not clients.";

		ConfigSet::const_iterator _it = _childs.begin(), _it_end(_childs.end());
		for (; _it != _it_end; ++_it)
		{
			VLOG(2) << "Adding new client " << *_it;
			smart_channel_t _channel(new CKernelChannel(*_it, *this));
			FChannels.push_back(_channel);
			_channel->MOpen(aP);
		}
	}
	return true;
}
void CKernelIOByTCPClient::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd, const descriptor_t& aFrom)
{
	if (!CDescriptors::sMIsValid(aFrom))
		return;

	smart_channel_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		active_channels_t::iterator _it = FActiveChannels.find(aFrom);
		if (_it != FActiveChannels.end())
		{
			_channel = (_it->second);
		}
	}
	if (_channel.MIs())
		_channel->MReceivedData(aBegin, aEnd);
}
bool CKernelIOByTCPClient::MSend(const data_t& aVal, descriptor_t const& aTo)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_channel_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		active_channels_t::iterator _it = FActiveChannels.find(aTo);
		if (_it != FActiveChannels.end())
		{
			_channel = (_it->second);
		}
	}
	if (_channel.MIs())
		return _channel->MSend(aVal);

	LOG(ERROR)<< "No  channel for " << aTo;
	return false;
}
template<class T>
inline bool CKernelIOByTCPClient::MSendImpl(const T& _id,
		descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_channel_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		active_channels_t::iterator _it = FActiveChannels.find(aTo);
		if (_it != FActiveChannels.end())
		{
			_channel = (_it->second);
		}
	}
	if (_channel.MIs())
			return _channel->MSend(_id,aRoute,aError);

	LOG(ERROR)<< "No  channel for " << aTo;
	return false;
}
inline bool CKernelIOByTCPClient::MSendImpl(const user_data_t& _id,
		descriptor_t const& aTo)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_channel_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		active_channels_t::iterator _it = FActiveChannels.find(aTo);
		if (_it != FActiveChannels.end())
		{
			_channel = (_it->second);
		}
	}
	if (_channel.MIs())
		return _channel->MSend(_id);

	LOG(ERROR)<< "No  channel for " << aTo;
	return false;
}
bool CKernelIOByTCPClient::MSend(const program_id_t& _id,
		descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}

bool CKernelIOByTCPClient::MSend(const fail_send_t& _id,
		descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}
bool CKernelIOByTCPClient::MSend(const demand_dgs_for_t& _id,
		descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}

bool CKernelIOByTCPClient::MSend(const kernel_infos_array_t& _id, descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	return MSendImpl(_id, aTo,aRoute,aError);
}
bool CKernelIOByTCPClient::MSend(const user_data_t& aVal,
		descriptor_t const& aTo)
{
	return MSendImpl(aVal, aTo);
}

bool CKernelIOByTCPClient::MIsOpen() const
{
	return !FActiveChannels.empty();
}
CKernelIOByTCPClient::descriptors_t CKernelIOByTCPClient::MGetDescriptors() const
{
	CRAII<CMutex> _blocked(FMutex);
	descriptors_t _buf;
	active_channels_t::const_iterator _it = FActiveChannels.begin(), _it_end(
			FActiveChannels.end());

	for (; _it!=_it_end; ++_it)
	{
		_buf.push_back(_it->first);
	}
	return _buf;
}
bool CKernelIOByTCPClient::MIs(descriptor_t aVal) const
{
	if (!CDescriptors::sMIsValid(aVal))
		return false;
	CRAII<CMutex> _blocked(FMutex);
	return FActiveChannels.find(aVal)!=FActiveChannels.end();
}
bool CKernelIOByTCPClient::MRemoveChannel(CKernelChannel* aWhat,descriptor_t aId)
{
	smart_channel_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		FActiveChannels.erase(aId);
	}
	FIo->MRemoveChannelFor(aId, this);
	return true;
}

bool CKernelIOByTCPClient::MAddChannel(CKernelChannel* aWhat,descriptor_t aId,split_info const& aLimits)
{
	VLOG(2)<<"Add channel for "<<aId<<" "<<aWhat->MSerialize().MToJSON(true);
	smart_channel_t _channel(aWhat);
	{
		CRAII<CMutex> _blocked(FMutex);
		CHECK(FActiveChannels.find(aId)==FActiveChannels.end());
		FActiveChannels[aId]=_channel;
	}
	FIo->MAddChannelFor(aId, this,aLimits);
	return true;
}
NSHARE::CConfig const& CKernelIOByTCPClient::MBufSettingFor(const descriptor_t& aFor,
		NSHARE::CConfig const& aFrom) const
{
	smart_channel_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		active_channels_t::const_iterator _it = FActiveChannels.find(aFor);
		if (_it != FActiveChannels.end())
			_channel = _it->second;
	}
	if (_channel.MIs())
	{
		NSHARE::CConfig const& _io = _channel->MBufSettingFor(aFrom);
		if (!_io.MIsEmpty())
		{
				return _io;
		}
		NSHARE::CConfig const& _def = aFrom.MChild(CKernelIo::DEFAULT);
		if (!_def.MIsEmpty())
		{
			return _def;
		}
	}
	return NSHARE::CConfig::sMGetEmpty();
}
NSHARE::CConfig CKernelIOByTCPClient::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	CRAII<CMutex> _blocked(FMutex);
	active_channels_t::const_iterator _it = FActiveChannels.begin(), _it_end(
			FActiveChannels.end());
	for (; _it != _it_end; ++_it)
	{
		_conf.MAdd((_it->second)->MSerialize());
	}
	return _conf;
}

NSHARE::CText const CKernelIOByTCPClientRegister::NAME="tcp_client_io_manager";
CKernelIOByTCPClientRegister::CKernelIOByTCPClientRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 3))
{

}
void CKernelIOByTCPClientRegister::MUnregisterFactory() const
{
	CKernelIo::sMGetInstance().MRemoveFactory(CKernelIOByTCPClient::NAME);
}
void CKernelIOByTCPClientRegister::MAdding() const
{
//CKernelIOByTCPClient* _p =
	CKernelIo::sMAddFactory<CKernelIOByTCPClient>();
}
bool CKernelIOByTCPClientRegister::MIsAlreadyRegistered() const
{
	if (CKernelIo::sMGetInstancePtr())
		return CKernelIo::sMGetInstance().MIsFactoryPresent(
				CKernelIOByTCPClient::NAME);
	return false;

}
}
#if !defined(RRD_STATIC)
static NSHARE::factory_registry_t g_factory;
extern "C" NSHARE::factory_registry_t* get_factory_registry()
{
	if (g_factory.empty())
	{
		g_factory.push_back(new NUDT::CKernelIOByTCPClientRegister());
	}
	return &g_factory;
}
#endif
