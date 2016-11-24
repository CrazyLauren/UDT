/*
 * CExternalChannel.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 26.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "../../core/kernel_type.h"
#include "../../core/CDescriptors.h"
#include "../../core/CConfigure.h"
#include "../CKernelIo.h"
#include "../IIOManager.h"
#include "CExternalChannel.h"
#include "CFrontEnd.h"
using namespace NSHARE;
namespace NUDT
{
const NSHARE::CText CExternalChannel::NAME = "front_end_manager";
CExternalChannel::CExternalChannel() :
		IIOManager(NAME)
{
	FIo = NULL;
}

CExternalChannel::~CExternalChannel()
{
	MClose();
}
void CExternalChannel::MInit(CKernelIo * aVal)
{
	VLOG(2)<<"Init";
	CHECK_NOTNULL(aVal);
	CHECK(FIo==NULL);
	FIo=aVal;
}
void CExternalChannel::MCloseRequest(const descriptor_t& aFrom)
{
	CHECK_NOTNULL(FIo);
	FIo->MClose(aFrom);
}
void CExternalChannel::MClose(const descriptor_t& aFrom)
{
	smart_fe_t _channel;
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
void CExternalChannel::MClose()
{
	for (unsigned i = 0; i < FChannels.size(); ++i)
	{
		FChannels[i]->MClose();
	}
}

bool CExternalChannel::MOpen(const void* aP)
{
	CConfig* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);
	if(_p)
	{
		ConfigSet const _childs = _p->MChildren(CFrontEnd::NAME);
		LOG_IF(WARNING,_childs.empty()) << "There are not front ends.";
		ConfigSet::const_iterator _it = _childs.begin(), _it_end(_childs.end());
		for (; _it != _it_end; ++_it)
		{
			VLOG(2) << "Adding new client " << *_it;
			smart_fe_t _channel (new CFrontEnd(*_it,*this));
			FChannels.push_back(_channel);
			_channel->MOpen();
		}

	}

	return true;
}
void CExternalChannel::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd, const descriptor_t& aFrom)
{
	if (!CDescriptors::sMIsValid(aFrom))
		return;

	smart_fe_t _channel;
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
bool CExternalChannel::MSend(const data_t& aVal, descriptor_t const& aTo)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_fe_t _channel;
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
inline bool CExternalChannel::MSendImpl(const user_data_t& _id,
		descriptor_t const& aTo)
{
	if (!CDescriptors::sMIsValid(aTo))
		return false;
	smart_fe_t _channel;
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
bool CExternalChannel::MSend(const programm_id_t& _id,
		descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	LOG(DFATAL)<<"Front end does't handle programm_id_t";
	return false;
}

bool CExternalChannel::MSend(const fail_send_t& _id,
		descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	LOG(DFATAL)<<"Front end does't handle fail_send_t";
	return false;
}
bool CExternalChannel::MSend(const demand_dgs_for_t& _id,
		descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	LOG(DFATAL)<<"Front end does't handle demand_dgs_for_t";
	return false;
}

bool CExternalChannel::MSend(const kernel_infos_array_t& _id, descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError)
{
	//LOG(DFATAL)<<"Front end does't handle kernel_infos_array_t";
	return true;
}
bool CExternalChannel::MSend(const user_data_t& aVal,
		descriptor_t const& aTo)
{
	return MSendImpl(aVal, aTo);
}

bool CExternalChannel::MIsOpen() const
{
	return !FActiveChannels.empty();
}
CExternalChannel::descriptors_t CExternalChannel::MGetDescriptors() const
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
bool CExternalChannel::MIs(descriptor_t aVal) const
{
	if (!CDescriptors::sMIsValid(aVal))
		return false;
	CRAII<CMutex> _blocked(FMutex);
	return FActiveChannels.find(aVal)!=FActiveChannels.end();
}
bool CExternalChannel::MRemoveChannel(CFrontEnd* aWhat,descriptor_t aId)
{
	smart_fe_t _channel;
	{
		CRAII<CMutex> _blocked(FMutex);
		FActiveChannels.erase(aId);
	}
	FIo->MRemoveChannelFor(aId, this);
	return true;
}

bool CExternalChannel::MAddChannel(CFrontEnd* aWhat,descriptor_t aId,split_info const& aLimits)
{
	VLOG(2)<<"Add channel for "<<aId<<" "<<aWhat->MSerialize().MToJSON(true);
	smart_fe_t _channel(aWhat);
	{
		CRAII<CMutex> _blocked(FMutex);
		CHECK(FActiveChannels.find(aId)==FActiveChannels.end());
		FActiveChannels[aId]=_channel;
	}
	FIo->MAddChannelFor(aId, this,aLimits);
	return true;
}
NSHARE::CConfig const& CExternalChannel::MBufSettingFor(const descriptor_t& aFor,
		NSHARE::CConfig const& aFrom) const
{
	smart_fe_t _channel;
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
NSHARE::CConfig CExternalChannel::MSerialize() const
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
NSHARE::CText const CExternalChannelRegister::NAME=CExternalChannel::NAME;
CExternalChannelRegister::CExternalChannelRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 3))
{

}
void CExternalChannelRegister::MUnregisterFactory() const
{
	CKernelIo::sMGetInstance().MRemoveFactory(CExternalChannel::NAME);
}
void CExternalChannelRegister::MAdding() const
{
//CKernelIOByTCPClient* _p =
	CKernelIo::sMAddFactory<CExternalChannel>();
}
bool CExternalChannelRegister::MIsAlreadyRegistered() const
{
	if (CKernelIo::sMGetInstancePtr())
		return CKernelIo::sMGetInstance().MIsFactoryPresent(
				CExternalChannel::NAME);
	return false;

}
} /* namespace NUDT */
