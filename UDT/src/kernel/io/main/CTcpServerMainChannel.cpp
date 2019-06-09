// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTcpServerMainChannel.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share_socket.h>
#include <string.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include <parser_in_protocol.h>
#include <core/kernel_type.h>
#include <core/CConfigure.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include "../CChannelDiagnostics.h"
#include <io/IIOManager.h>
#include <io/ILink.h>


#include "CTcpServerMainChannel.h"
#include "CTcpServerMainClient.h"
#include "CTcpServerClientConnector.h"
using namespace NSHARE;
namespace NUDT
{
const NSHARE::CText CTcpServerMainChannel::THREAD_PRIORITY = "priority";
const NSHARE::CText CTcpServerMainChannel::NAME = E_MAIN_CHANNEL_TCPSERVER;
const NSHARE::CText CTcpServerMainChannel::PORT = "port";

static size_t f_calculate_size(user_data_t const& aWhat,void*)
{
	return get_full_size(aWhat);
};

CTcpServerMainChannel::CTcpServerMainChannel() :
		IMainChannel(NAME),FConnector(new CMainClientConnector)
{
	FAddr.FPort = 0;
	FIsOverload = false;
	FBuffers.FSendSMBuffer = 0;

	FServer += CTCPServer::value_t(CTCPServer::EVENT_CONNECTED,
			NSHARE::CB_t(sMConnect, this));
	FServer += CTCPServer::value_t(CTCPServer::EVENT_DISCONNECTED,
			NSHARE::CB_t(sMDisconnect, this));
}

bool CTcpServerMainChannel::MOpenIfNeed()
{
	if (!FServer.MIsOpen())
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FOpenMutex);
		if (!FServer.MIsOpen() && FServer.MOpen(FAddr))
		{
			NSHARE::operation_t _op(CTcpServerMainChannel::sMReceiver, this,
					NSHARE::operation_t::IO);
			CDataObject::sMGetInstance().MPutOperation(_op);
		}
	}
	return FServer.MIsOpen();
}

bool CTcpServerMainChannel::MStart()
{
	CConfig _main_settings = CConfigure::sMGetInstance().MGet().MChild(IMainChannel::CONFIGURE_NAME);
	if (_main_settings.MIsEmpty())
	{
		LOG(ERROR) << "Main channel settings is not exist";
		return false;
	}
	CConfig _settings = _main_settings.MChild(NAME);
	if (_settings.MIsEmpty())
	{
		LOG(WARNING) << "The tcp server main channel is not initialized as no configure.";
		return false;
	}


	if (_settings.MGetIfSet(PORT, FAddr.FPort))
	{
		VLOG(2) << "Tcp main channel is  using port " << FAddr;
	}
	return true;
}
CTcpServerMainChannel::~CTcpServerMainChannel()
{
	FServer.MClose();
}
template<>
void CTcpServerMainChannel::MFill<main_channel_param_t>(data_t* aTo,ILink* aHandler)
{
	VLOG(2) << "Create main channel param DG";
	NSHARE::net_address _addr;
	FServer.MGetInitParam(&_addr);
	_addr.MSetIP(INADDR_LOOPBACK);

	main_ch_param_t _param;
	_param.FType = E_MAIN_CHANNEL_TCP;
	_param.FValue = _addr.MSerialize();
	_param.FValue.MSet("limit", MGetLimits(0, aHandler).FMaxSize);
	serialize<main_channel_param_t, main_ch_param_t>(aTo, _param, routing_t(), error_info_t());
}
template<>
void CTcpServerMainChannel::MFill<request_main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create request main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(request_main_channel_param_t));
	request_main_channel_param_t * _p =
			new ((data_t::value_type*) aTo->ptr() + _befor) request_main_channel_param_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_TCP);
	fill_dg_head(_p, sizeof(request_main_channel_param_t), get_my_id());
	VLOG(2) << (*_p);
}
template<>
void CTcpServerMainChannel::MFill<close_main_channel_t>(data_t* aTo)
{
	VLOG(2) << "Close  main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(close_main_channel_t));
	close_main_channel_t * _p =
			new ((data_t::value_type*) aTo->ptr() + _befor) close_main_channel_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_TCP);
	fill_dg_head(_p, sizeof(close_main_channel_t), get_my_id());
	VLOG(2) << (*_p);
}

bool CTcpServerMainChannel::MSendSetting(ILink* aHandler, descriptor_t aFor)
{
	data_t _buf;
	MFill<main_channel_param_t>(&_buf,aHandler);
	MFill<request_main_channel_param_t>(&_buf);
	return aHandler->MSend(_buf);
}

split_info CTcpServerMainChannel::MGetLimits(size_t aSize,ILink* aHandler) const
{
	split_info _info = aHandler?aHandler->MLimits():split_info();

	std::pair<size_t, size_t> const _pair = FServer.MBufSize();
	size_t const _tcp_size =
			_pair.first > 0 && _pair.first < _pair.second ?
					_pair.first : _pair.second;
	_info.FMaxSize =
			(_info.FMaxSize > 0 && _info.FMaxSize < _tcp_size) ||_tcp_size==0?
					_info.FMaxSize : _tcp_size;
	_info.FMaxSize =
			(_info.FMaxSize > 0 && _info.FMaxSize < aSize) || aSize==0 ?
					_info.FMaxSize : aSize;
	if (_info.FMaxSize)
		_info.FType.MSetFlag(split_info::LIMITED, true);

	_info.pMCalculate = &f_calculate_size;
	VLOG(2)<<"new limits "<<_info;
	return _info;
}
bool CTcpServerMainChannel::MOpen(ILink* aHandler, program_id_t const& aId,NSHARE::net_address const& aFor)
{
	CHECK_NOTNULL(aHandler);



	if (!MOpenIfNeed())
	{
		LOG(DFATAL)<< "Tcp server channel is not opened.";
		return false;
	}

	//if(_info.FType)
	LOG_IF(ERROR,MIsOpen(aHandler->MGetID())) << "Main channel for "
														<< aHandler->MGetID()
														<< "has been opened already";

	FConnector->MAddNew(aHandler,aId);
	return MSendSetting(aHandler, aHandler->MGetID());
}
bool CTcpServerMainChannel::MIsOpen(descriptor_t aFor) const
{
	r_access _access = FData.MGetRAccess();
	client_by_id_t::const_iterator _it = _access->FById.find(aFor);
	return _it != _access->FById.end() && _it->second->MIsOpened();
}
bool CTcpServerMainChannel::MCloseRequest(descriptor_t aFor)
{
	smart_client_t _param;
	VLOG(2) << "Try close request " << aFor;
	{
		w_access _access = FData.MGetWAccess();
		client_by_id_t::iterator _it = _access->FById.find(aFor);
		LOG_IF(ERROR,_it == _access->FById.end()) << "Main channel for "
															<< aFor
															<< " is not exist.";
		if (_it != _access->FById.end())
		{
			_param = _it->second;
		}
	}
	if (_param)
		return _param->MCloseRequest();
	else
		return FConnector->MCloseRequest(aFor);
}
bool CTcpServerMainChannel::MClose(descriptor_t aFor)
{
	smart_client_t _param;
	VLOG(2) << "Try closing " << aFor;
	{
		w_access _access = FData.MGetWAccess();
		client_by_id_t::iterator _it = _access->FById.find(aFor);
		VLOG_IF(1,_it == _access->FById.end()) << "Main channel for " << aFor
														<< " is not exist.";
		if (_it != _access->FById.end())
		{
			_param = _it->second;
			_access->FByIp.erase(_param->MClient().FAddr);
			_access->FById.erase(_it);
		}
	}
	if (_param)
		return _param->MClose();
	else
		return FConnector->MClose(aFor);
}
bool CTcpServerMainChannel::MSend(user_data_t const& aData, descriptor_t aFor)
{
	CHECK(!aData.FData.MIsDetached());
	VLOG(2) << "Send data " << aData.FDataId.FPacketNumber << " to " << aFor;
	smart_client_t _client;
	{
		r_access _access = FData.MGetRAccess();
		client_by_id_t::const_iterator _it = _access->FById.find(aFor);
		LOG_IF(ERROR,_it == _access->FById.end()) << "Main channel for " << aFor
															<< " is not exist.";
		if (_it != _access->FById.end())
		{
			_client = _it->second;
		}
	}
	if (_client)
	{

		//The iteration method is used instead of the pop_back method
		//for avoiding dead looks
		_client->MSend(aData);
	}
	return true;
}
//bool CTcpServerMainChannel::MSend(user_data_t const& aData, descriptor_t aFor)
//{
//	CHECK(!aData.FData.MIsDeached());
//	VLOG(2) << "Send data " << aData.FDataId.FPacketNumber << " to " << aFor;
//	//	send_buffer_t _current_send_buffers;
//	//Thread blocking protection and buffering limitation.
//	//if another thread is sending data to aFor, Let its send our buffer.
//	{
//		NSHARE::CRAII<CMutex> _block(FBuffers.FMutexSend);
//		buffers_t::iterator _it = FBuffers.FBuffer.find(aFor);
//		if (_it == FBuffers.FBuffer.end())
//		{
//			_it = FBuffers.FBuffer.insert(
//					buffers_t::value_type(aFor,
//							CSendBuffering(_buf_channel_size(), _buf_sm_size(),
//									FBuffers.FSendSMBuffer))).first;
//		}
//		if (!_it->second.MPut(aData))
//			return false;
//		if (!_it->second.MIsWorking())
//		{
//			operation_t _op(sMSend, this, operation_t::IO);
//			CDataObject::sMGetInstance().MPutOperation(_op);
//		}
//	}
//	return true;
//}
//void CTcpServerMainChannel::MSendImpl()
//{
//	descriptor_t _id = -1;
//	CSendBuffering::data_list_t _data;
//	for (;;)
//	{
//		{
//			NSHARE::CRAII<CMutex> _block(FBuffers.FMutexSend);
//			buffers_t::iterator _it = FBuffers.FBuffer.begin();
//			for (; _it != FBuffers.FBuffer.end(); ++_it)
//				if (!_it->second.MIsWorking() && !_it->second.MIsEmpty())
//				{
//					break;
//				}
//			if (_it == FBuffers.FBuffer.end())
//			{
//				VLOG(2) << "There is not buffers.";
//				break;
//			}
//			_id = _it->first;
//			VLOG(2) << "Sent " << _id;
//			_it->second.MTrySend(_data);
//		}
//
//		smart_client_t _client;
//		{
//			r_access _access = FData.MGetRAccess();
//			client_by_id_t::const_iterator _it = _access->FById.find(_id);
//			LOG_IF(ERROR,_it == _access->FById.end()) << "Main channel for "
//																<< _id
//																<< " is not exist.";
//			if (_it != _access->FById.end())
//			{
//				_client=_it->second;
//			}
//		}
//		if (_client)
//		{
//			CHECK(!_data.empty());
//
//			//The iteration method is used instead of the pop_back method
//			//for avoiding dead looks
//			_client->MSend(_data);
//
//		}
//
//		{
//			NSHARE::CRAII<CMutex> _block(FBuffers.FMutexSend);
//			buffers_t::iterator _it = FBuffers.FBuffer.find(_id);
//			if (_it != FBuffers.FBuffer.end())
//			{
//				_it->second.MFinishSend(_data);
//				//todo cannot sent
//			}
//			else
//				LOG(ERROR)<<_id<<" has been closed";
//		}
//	}
//}

int CTcpServerMainChannel::sMSend(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* YOU_DATA)
{
	//reinterpret_cast<CTcpServerMainChannel*>(YOU_DATA)->MSendImpl();
	return 0;
}

bool CTcpServerMainChannel::MSend(data_t const& aVal, descriptor_t aFor)
{
	VLOG(2) << "Send data to " << aFor << " by main channel";

	NSHARE::smart_field_t<NSHARE::net_address> _addr;
	{
		r_access _access = FData.MGetRAccess();
		client_by_id_t::const_iterator _it = _access->FById.find(aFor);
		LOG_IF(DFATAL,_it == _access->FById.end()) << "Main channel for "
															<< aFor
															<< " is not exist.";
		if (_it != _access->FById.end())
		{
			_addr = _it->second->MClient().FAddr;
		}
	}
	if (_addr.MIs())
		return MSendImpl(&aVal.front(), aVal.size(), _addr);
	return false;
}
bool CTcpServerMainChannel::MSendImpl(const void* pData, size_t nSize,
		const NSHARE::net_address& aAddr)
{
	sent_state_t _state;
	HANG_INIT;
	do
	{
		HANG_CHECK;
		_state = FServer.MSend(pData, nSize, aAddr);
		VLOG_IF(1,_state.MIs(sent_state_t::E_AGAIN)) << "Again " << _state;
	}while(_state.MIs(sent_state_t::E_AGAIN));
	LOG_IF(ERROR,!_state.MIs())<<"Cannot sent: "<<_state;
	return _state.MIs();
}
bool CTcpServerMainChannel::MSendToService(const data_t& aVal,
		descriptor_t aFor)
{
	VLOG(2) << "Send data to " << aFor << " by service channel";

	{
		r_access _access = FData.MGetRAccess();
		client_by_id_t::const_iterator _it = _access->FById.find(aFor);
		LOG_IF(DFATAL, _it == _access->FById.end()) << "Service channel for "
															<< aFor
															<< " is not exist.";
		if (_it != _access->FById.end())
		{
			CHECK(_it->second);
			if (!_it->second)
				return false;
			return _it->second->MSendService(aVal);
		}
	}
	return false;
}
NSHARE::ISocket* CTcpServerMainChannel::MGetSocket()
{
	return &FServer;
}
const NSHARE::ISocket* CTcpServerMainChannel::MGetSocket() const
{
	return &FServer;
}
NSHARE::eCBRval CTcpServerMainChannel::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*aData)
{
	reinterpret_cast<CTcpServerMainChannel*>(aData)->MReceiver();
	return E_CB_REMOVE;
}

bool CTcpServerMainChannel::MCheckingForOverload(NSHARE::CTCPServer::recvs_t const& _from) const
{
	NSHARE::CTCPServer::recvs_t::const_iterator _it = _from.begin(), _it_end(
			_from.end());
	for (; _it != _it_end; ++_it)
	{
		FOverload.MCheking(_it->FClient.FAddr, _it->FSize);
	}

	net_overload_t::over_t _over;
	FOverload.MFinishCheking(_over);
	if (_over.FIsExist && (FIsOverload = _over.FResult))
	{
		LOG(WARNING)<<"Overloaded Tcp detected."<<_over;
		return true;
	}
	return false;
}

void CTcpServerMainChannel::MAddingNew(
		std::list<smart_client_t> const& _new_clients)
{
	w_access _access = FData.MGetWAccess();
	link_data_t & _this_data = _access.MGet();
	std::list<smart_client_t>::const_iterator _jt = _new_clients.begin(), _jt_end(
			_new_clients.end());

	for (; _jt != _jt_end; ++_jt)
	{
		//adding the new
		{
			_this_data.FByIp[(*_jt)->MClient().FAddr] = *_jt;
			_this_data.FById[(*_jt)->MId()] = *_jt;
			(*_jt)->MSendMainChannelError(main_channel_error_param_t::E_OK);

		}
	}
}
void CTcpServerMainChannel::MClosing(std::vector<NSHARE::net_address> const& _need_close)
{
	std::vector<NSHARE::net_address>::const_iterator _it=_need_close.begin(), _it_end=_need_close.end();

	for(;_it!=_it_end;++_it)
	{
		LOG(WARNING)<<"The clients "<<*_it<<" should be closed.";
		FServer.MCloseClient(*_it);
	}
}

void CTcpServerMainChannel::MReceivingData(NSHARE::CTCPServer::recvs_t const& _from,std::vector<NSHARE::net_address>& _need_close,std::list<smart_client_t>& _new_clients)
{

	r_access _access = FData.MGetRAccess();
	link_data_t const & _this_data = _access.MGet();

	NSHARE::CTCPServer::recvs_t::const_iterator _jt = _from.begin(), _jt_end(
			_from.end());
	for (; _jt != _jt_end; ++_jt)
	{
		NSHARE::net_address const& _addr = _jt->FClient.FAddr;
		data_t::const_iterator const _buf_begin(_jt->FBufBegin.base());

		client_by_ip_t::const_iterator _it;

		if ((_it = _this_data.FByIp.find(_addr)) != _this_data.FByIp.end())
		{
			VLOG(3) << "Our client.";
			smart_client_t const& _param = _it->second;
			CHECK(_param);
			if (!_param)
			{
				LOG(ERROR)<<"Unknown error.";
				continue;
			}
			else
			_param->MReceivedData(_buf_begin, _jt->FSize);
		}
		else //maybe it's the new client
		{
			FConnector->MReceivedData(_buf_begin, _jt->FSize);
			if(FConnector->MGetLastNewLink().MIs())
			{
				smart_client_t _new_client(new CMainClient(*this, //
								_jt->FClient,//
								CDataObject::sMGetInstance().MDefAllocater(),//
								FConnector->MGetLastNewLink().MGetConst().FLink)//
				);
				_new_client->MReceivedData(_buf_begin, _jt->FSize);//if it's the new client it send specialized msg
				CHECK(_new_client->MIsOpened());
				_new_clients.push_back(_new_client);
			}
			else
			_need_close.push_back(_addr);
		}
	}
}
//Adding new clients, closing clients, receiving data is doing in this method
void CTcpServerMainChannel::MReceiver()
{
	using namespace NSHARE;
	VLOG(2) << "Async receive";
	ISocket::data_t _data;
	_data.reserve(std::numeric_limits<uint16_t>::max());

	for (/*HANG_INIT*/; FServer.MIsOpen(); /*HANG_CHECK*/)
	{
		//todo connection cleanup
		_data.clear();
		NSHARE::CTCPServer::recvs_t _from;
		FOverload.MStartCheking();
		if (FServer.MReceiveData(&_from, &_data, 0.0) > 0)
		{
			MCheckingForOverload(_from);

			std::vector<NSHARE::net_address> _need_close;
			std::list<smart_client_t> _new_clients;
			MReceivingData(_from,_need_close,_new_clients);

			if(!_need_close.empty())
			{
				MClosing(_need_close);
			}

			if(!_new_clients.empty())
			{
				MAddingNew(_new_clients);
			}
		}
	}
	VLOG(1) << "Socket closed";
}
bool CTcpServerMainChannel::MIsOveload(descriptor_t aFor) const
{
	return FIsOverload;
}

bool CTcpServerMainChannel::MHandleServiceDG(main_channel_param_t const* aData,
		descriptor_t aVal)
{
	return true;
}
bool CTcpServerMainChannel::MHandleServiceDG(
		main_channel_error_param_t const* aP, descriptor_t aFor)
{
	return  FConnector->MHandleServiceDG(aP,aFor);
}
bool CTcpServerMainChannel::MHandleServiceDG(
		request_main_channel_param_t const* aP, descriptor_t aFor)
{
	data_t _buf;
	MFill<main_channel_param_t>(&_buf,NULL);
	return MSendToService(_buf, aFor);
}
bool CTcpServerMainChannel::MHandleServiceDG(close_main_channel_t const* aP,
		descriptor_t aFor)
{
	LOG(ERROR)<<"The customer "<<aFor<<" demand of closing channel";
	return MClose(aFor);
}
NSHARE::CConfig CTcpServerMainChannel::MSerialize() const
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
			_it->second->MGetState().MSerialize(_cust);
			_state.MAdd(_cust);
		}
	}
	_state.MAdd(FServer.MSerialize());
	return _state;
}
NSHARE::eCBRval CTcpServerMainChannel::sMConnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);
	CTCPServer::client_t* _client =
			reinterpret_cast<CTCPServer::client_t*>(aWhat);
	CTcpServerMainChannel* _p = reinterpret_cast<CTcpServerMainChannel*>(aThis);
	VLOG(2) << "Connecting  " << *_client;
	(void) _p;

	return E_CB_SAFE_IT;
}
NSHARE::eCBRval CTcpServerMainChannel::sMDisconnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);
	CTCPServer::client_t* _client =
			reinterpret_cast<CTCPServer::client_t*>(aWhat);
	CTcpServerMainChannel* _p = reinterpret_cast<CTcpServerMainChannel*>(aThis);
	VLOG(2) << "Disconnecting  " << *_client;
	descriptor_t _des = -1;
	{
		r_access _access = _p->FData.MGetRAccess();
		client_by_ip_t::const_iterator _it = _access->FByIp.find(
				_client->FAddr);
		if (_it != _access->FByIp.end())
		{
			_des = _it->second->MId();
		}

	}
	if (CDescriptors::sMIsValid(_des))
		_p->MCloseRequest(_des);

	return E_CB_SAFE_IT;
}
}
