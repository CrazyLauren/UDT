/*
 * CTcpClientMainChannel.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 02.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include "../../core/kernel_type.h"
#include "../../core/CDataObject.h"

#include "../../core/CConfigure.h"
#include "../../core/CDescriptors.h"
#include "../ITcpIOManager.h"
#include "../ILink.h"
#include "../CChannelDiagnostics.h"

#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_USER_DATA,user_data_dg_t)/*Данные пользователя*/\
	/*END*/

#include <parser_in_protocol.h>
#include "CTcpClientMainChannel.h"
#include "CTcpClientMainClient.h"

using namespace NSHARE;

namespace NUDT
{
const NSHARE::CText CTcpClientMainChannel::THREAD_PRIORITY = "priority";
const NSHARE::CText CTcpClientMainChannel::NAME = E_MAIN_CHANNEL_TCP;


CTcpClientMainChannel::CTcpClientMainChannel() :
		IMainChannel(NAME)
{
}
void CTcpClientMainChannel::MInit()
{
	CConfig _main_settings = CConfigure::sMGetInstance().MGet().MChild(IMainChannel::CONFIGURE_NAME);
	if (_main_settings.MIsEmpty())
	{
		LOG(ERROR) << "Main channel settings is not exist";
		return;
	}
	CConfig _settings = _main_settings.MChild(NAME);
	if (_settings.MIsEmpty())
	{
		LOG(WARNING) << "The tcp client main channel is not initialized as no configure.";
		return;
	}
	//CConfig const& _conf = CConfigure::sMGetInstance().MGet().MChild(NAME);
	if (MOpenLoopSocket())
	{
		NSHARE::operation_t _op(CTcpClientMainChannel::sMReceiver, this, NSHARE::operation_t::IO);
		CDataObject::sMGetInstance().MPutOperation(_op);
	}
	DCHECK(FSelectSock.MIsSetUp());
}
CTcpClientMainChannel::~CTcpClientMainChannel()
{
	MCloseImpl();
}
void CTcpClientMainChannel::MCloseImpl()
{
	FLoopBack.MClose();
	w_access _access = FData.MGetWAccess();
	_access->FBySocket.clear();
	_access->FById.clear();
	FSelectSock.MRemoveSocket(FLoopBack.MGetSocket());
	FSelectSock.MRemoveAll();
}
bool CTcpClientMainChannel::MOpen(ILink* aHandler, program_id_t const& aId,NSHARE::net_address const& aFor)
{
	CHECK_NOTNULL(aHandler);

	LOG_IF(ERROR,MIsOpen(aHandler->MGetID())) << "Main channel for "
														<< aHandler->MGetID()
														<< "has been opened already";

	CRAII<CMutex> _block(FMutexNewLinks);

	bool _is = FNewLinks.find(aHandler->MGetID()) != FNewLinks.end();
	LOG_IF(ERROR,_is) << "Retrying open main channel for "
								<< aHandler->MGetID();

	if (!_is)
	{
		new_link_t _val;
		_val.FId = aId;
		_val.FLink = aHandler;
		_val.FAddr=aFor;
		FNewLinks[aHandler->MGetID()] = _val;
	}
	return !_is;
}
bool CTcpClientMainChannel::MIsOpen(descriptor_t aFor) const
{
	r_access _access = FData.MGetRAccess();
	client_by_id_t::const_iterator _it = _access->FById.find(aFor);
	return _it != _access->FById.end() && _it->second->MIsOpened();
}
bool CTcpClientMainChannel::MCloseRequest(descriptor_t aFor)
{
	smart_client_t _param;
	VLOG(2) << "Try close request " << aFor;
	{
		w_access _access = FData.MGetWAccess();
		link_data_t & _this_data = _access.MGet();
		client_by_id_t::iterator _it = _this_data.FById.find(aFor);
		LOG_IF(ERROR,_it == _this_data.FById.end()) << "Main channel for "
															<< aFor
															<< " is not exist.";
		if (_it != _this_data.FById.end())
		{
			_param = _it->second;
		}
	}
	if (_param)
		return _param->MCloseRequest();
	else
	{
		CRAII<CMutex> _block(FMutexNewLinks);
		new_links_t::iterator _it = FNewLinks.find(aFor);
		if (_it != FNewLinks.end())
		{
			VLOG(2) << "Close " << aFor << " as new client.";
			_it->second.FLink->MCloseRequest();
			return true;
		}
	}
	LOG(ERROR)<<"Cannot close request "<<aFor;
	return false;
}
bool CTcpClientMainChannel::MClose(descriptor_t aFor)
{
	smart_client_t _param;
	VLOG(2) << "Try closing " << aFor;
	{
		w_access _access = FData.MGetWAccess();
		link_data_t & _this_data = _access.MGet();
		client_by_id_t::iterator _it = _this_data.FById.find(aFor);
		LOG_IF(DFATAL,_it == _this_data.FById.end()) << "Main channel for "
																<< aFor
																<< " is not exist.";
		if (_it != _this_data.FById.end())
		{
			_param = _it->second;
			MEraseClient(_this_data, _param);
		}
	}
	VLOG(4)<<"Closing  "<<aFor;
	if (_param)
		return _param->MClose();
	else
	{
		CRAII<CMutex> _block(FMutexNewLinks);
		new_links_t::iterator _it = FNewLinks.find(aFor);
		if (_it != FNewLinks.end())
		{
			VLOG(2) << "Close " << aFor << " as new client.";
			FNewLinks.erase(_it);
			return true;
		}
	}
	LOG(ERROR)<<"Cannot close "<<aFor;
	return false;
}

bool CTcpClientMainChannel::MSend(user_data_t const& aData, descriptor_t aFor)
{

	CHECK(!aData.FData.MIsDeached());
	VLOG(2) << "Send data " << aData.FDataId.FPacketNumber << " to " << aFor;
	smart_client_t _client;
	{
		r_access _access = FData.MGetRAccess();
		client_by_id_t::const_iterator _it = _access->FById.find(aFor);
		LOG_IF(ERROR,_it == _access->FById.end()) << "Main channel for "
															<< aFor
															<< " is not exist.";
		if (_it != _access->FById.end())
		{
			_client = _it->second;

		}
	}
	if (_client.MIs())
		return _client->MSend(aData);
	return false;
}
NSHARE::ISocket* CTcpClientMainChannel::MGetSocket()
{
	return NULL;
}
const NSHARE::ISocket* CTcpClientMainChannel::MGetSocket() const
{
	return NULL;
}

bool CTcpClientMainChannel::MHandleServiceDG(const main_channel_param_t* aData,
		descriptor_t aFor)
{
	VLOG(2) << "Main channel handling \"Main channel parametrs\":";
	CText _name((utf8 const*) aData->FType);
	CHECK_EQ(_name, E_MAIN_CHANNEL_TCP);
	NSHARE::net_address _addr(aData->FUdp.FAddr, aData->FUdp.FPort);
	VLOG(2)<<"Connect to "<<_addr;
	NSHARE::smart_field_t<new_link_t> _new_link;
	{
		CRAII<CMutex> _block(FMutexNewLinks);
		new_links_t::iterator _it = FNewLinks.begin(), _it_end(FNewLinks.end());
		for (; _it != _it_end; ++_it)
		{
			VLOG(2) << "Link " << _it->first;
			if (_it->first == aFor)
			{
				_new_link = _it->second;
				_addr.ip = _new_link.MGetConst().FAddr.ip;
				FNewLinks.erase(_it);
				break;
			}
		}
	}
	if (_new_link.MIs())
	{
		smart_client_t _link(
				new CMainClient(*this, _addr, _new_link.MGetConst().FLink,
						CDataObject::sMGetInstance().MDefAllocater(),
						aData->FLimit));
		LOG_IF(DFATAL,!_link->MIsOpened()) << "Cannot open client for "
													<< _addr;
		CHECK_EQ(aFor, _link->MId());
		if (_link->MIsOpened())
		{
			w_access _access = FData.MGetWAccess();

			link_data_t & _this_data = _access.MGet();

			bool _is = MAddNewClient(_this_data, _link);
			return _is;
		}
	}
	return false;
}
inline bool CTcpClientMainChannel::MOpenLoopSocket()
{
	bool _rval = FLoopBack.MOpen();
	CHECK(_rval);
	if (_rval)
		FSelectSock.MAddSocket(FLoopBack.MGetSocket());
	return _rval;
}
void CTcpClientMainChannel::MUnLockSelect()
{
	bool _val = false;
	_val = FLoopBack.MSend(&_val, sizeof(_val)).MIs();
	VLOG_IF(1,_val) << "Select unlocked successfully. ";
	LOG_IF(WARNING,!_val) << "Cannot unlock select. ";
}
bool CTcpClientMainChannel::MEraseClient(link_data_t& _this_data,
		smart_client_t &_link)
{
	VLOG(5)<<"Erase client";
	_this_data.FBySocket.erase(_link->FTcp.MGetSocket()); //todo
	_this_data.FById.erase(_link->MId());

	return true;
}
bool CTcpClientMainChannel::MAddNewClient(link_data_t& _this_data,
		smart_client_t &_link)
{
	VLOG(3) << "Adding the new client.";
	_this_data.FById[_link->MId()] = _link;
	_this_data.FBySocket[_link->FTcp.MGetSocket()] = _link;

	return true;
}
int CTcpClientMainChannel::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CTcpClientMainChannel*>(aData)->MReceiver();
	return 0;
}
void CTcpClientMainChannel::MReceive(NSHARE::CSelectSocket::socks_t const& _to)
{
	r_access _access = FData.MGetRAccess();
	link_data_t const& _this_data = _access.MGet();
	CSelectSocket::socks_t::const_iterator _it = _to.begin(), _it_end(
			_to.end());
	for (; _it != _it_end; ++_it)
	{
		VLOG(5)<<"Next socket "<<*_it;
		client_by_socket_t::const_iterator _jt = _this_data.FBySocket.find(
				*_it);

		if (_jt == _this_data.FBySocket.end())
		{
			LOG(ERROR)<<"The client is not exist.";
			FSelectSock.MRemoveSocket(*_it);
		}
		else
		{
			VLOG(5)<<"Receiving by "<<_jt->first;
			_jt->second->MReceive();//fixme can be problem if MReceive() will be locked as we have  _access
			//todo state
		}
	}
}
void CTcpClientMainChannel::MReceiver()
{
	VLOG(2) << "Async receive";
	CHECK(FSelectSock.MIsSetUp());
	for (; FSelectSock.MIsSetUp();)
	{
		CSelectSocket::socks_t _to;
		int _val = FSelectSock.MWaitData(_to, -1);
		VLOG(2) << "Wait status " << _val;
		if (_val == 0) //timeout
		{
			LOG(ERROR)<< "WTF? Timeout";
			continue;
		}
		else if (_val < 0)
		{
			LOG(ERROR) << "Unknown error";
			continue;
		}
		else if (_val > 0)
		{
			CSelectSocket::socks_t::iterator _it = _to.begin(),_it_end(_to.end());
			for (; _it != _it_end; ++_it)
			if(FLoopBack.MGetSocket() == *_it) //looking for loopback socket
			{
				VLOG(1) << "It's internal msg";
				FLoopBack.MReadAll();
				_to.erase(_it);
				break;
			}
			if(_to.empty())
			{
				VLOG(2)<<"Only internal MSG has been received.";
				continue;
			}
			MReceive(_to);
			VLOG(5)<<"Receive all";
		}
	}
}
bool CTcpClientMainChannel::MHandleServiceDG(
		main_channel_error_param_t const* aP, descriptor_t aFor)
{
	VLOG(2) << "main channel error " << *aP;
	if (aP->FError != 0)
	{
		return false;
	}
	smart_client_t _client;
	{
		r_access _access = FData.MGetRAccess();
		link_data_t const& _this_data = _access.MGet();
		client_by_id_t::const_iterator _it = _this_data.FById.find(aFor);

		if (_it == _this_data.FById.end())
		{
			LOG(ERROR)<<"The tcp client for "<<aFor<<" is not exist";
			return false;
		}
		_client = _it->second;
	}
	if (_client.MIs())
	{
		if (_client->MIsOpened())
			return _client->MSendMainChannelError(
					main_channel_error_param_t::E_OK);
		else
		{
			_client->MSendMainChannelError(
					main_channel_error_param_t::E_NOT_OPENED);
		}
	}
	return false;
}
bool CTcpClientMainChannel::MHandleServiceDG(
		request_main_channel_param_t const* aP, descriptor_t aFor)
{
	VLOG(2) << "Request main channel param " << *aP;
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);
	smart_client_t _client;
	{
		r_access _access = FData.MGetRAccess();
		link_data_t const& _this_data = _access.MGet();
		client_by_id_t::const_iterator _it = _this_data.FById.find(aFor);

		if (_it == _this_data.FById.end())
		{
			LOG(ERROR)<<"The tcp client for "<<aFor<<" is not exist";
			return false;
		}
		_client = _it->second;
	}
	if (_client.MIs())
	{
		return _client->MSendMainChannelParam();
	}
	return false;
}
bool CTcpClientMainChannel::MHandleServiceDG(close_main_channel_t const* aP,
		descriptor_t aFor)
{
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);
	return MCloseRequest(aFor);
}
bool CTcpClientMainChannel::MIsOveload(descriptor_t aFor) const
{
	//todo
	return false;
}
NSHARE::CConfig CTcpClientMainChannel::MSerialize() const
{
	NSHARE::CConfig _state(NAME);
	_state.MAdd("Name", MGetType());
	{
		r_access _access = FData.MGetRAccess();

		client_by_id_t::const_iterator _it = _access->FById.begin(), _it_end(
				_access->FById.end());
		for (; _it != _it_end; ++_it)
		{
			std::pair<descriptor_info_t, bool> _info =
					CDescriptors::sMGetInstance().MGet(_it->first);

			NSHARE::CConfig _cust(_info.first.FProgramm.FId.FName);
			_it->second->MSerialize(_cust);
			_state.MAdd(_cust);
		}
	}
	return _state;
}
} /* namespace NUDT */
