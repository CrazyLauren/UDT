/*
 * CTcpClientMainImpl.cpp
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
#include <internel_protocol.h>
#include <udt_share.h>
#include "../../core/kernel_type.h"
#include "../../core/CConfigure.h"
#include "../../core/CDescriptors.h"
#include "../../core/CDataObject.h"
#include "../CChannelDiagnostics.h"
#include "../ITcpIOManager.h"
#include "../ILink.h"
#include "../CLinkDiagnostic.h"
#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_USER_DATA,user_data_dg_t)/*Данные пользователя*/\
	/*END*/

#include <parser_in_protocol.h>
#include "CTcpClientMainChannel.h"
#include "CTcpClientMainClient.h"
namespace NUDT
{
using namespace NSHARE;

#define IMPL CTcpClientMainChannel::CMainClient
static size_t f_calculate_size(user_data_t const& aWhat,void*)
{
	return get_full_size(aWhat);
};


CTcpClientMainChannel::CMainClient::CMainClient(CTcpClientMainChannel& aThis,
		NSHARE::net_address const& aClient, ILink* aLink,
		NSHARE::IAllocater * const aBufferAlloc,size_t aLimit) :
		FTcp(aClient), //
		FAddr(aClient), FHandler(aLink), //
		FParser(this), //
		FThis(aThis), //
		/*FBuffer(_buf_channel_size(), _buf_sm_size(), aSendSMBuffer), //*/
		FBufferAlloc(aBufferAlloc)
{
	FParser.FUserData = this;
	FTcp += CTCP::value_t(CTCP::EVENT_CONNECTED, NSHARE::CB_t(sMConnect, this));
	FTcp += CTCP::value_t(CTCP::EVENT_DISCONNECTED,
			NSHARE::CB_t(sMDisconnect, this));

	FTcp.MConnect();

	if (FTcp.MIsConnected())
	{
		FState = E_OPENED;
		aLink->MSetLimits(MGetLimits(aLimit));
	}
	else
		FState = E_CLOSED;
}
void IMPL::MReceivedData(data_t::const_iterator const& _buf_begin,
		data_t::const_iterator const& aEnd)
{
	FParser.MReceivedData(_buf_begin, aEnd);
}
split_info IMPL::MGetLimits(size_t aSize) const
{
	//update split
	split_info _info = FHandler->MLimits();

	std::pair<size_t, size_t> const _pair = FTcp.MBufSize();
	size_t const _tcp_size =
	_pair.first > 0 && _pair.first < _pair.second ?
	_pair.first : _pair.second;
	_info.FMaxSize =
	(_info.FMaxSize > 0 && _info.FMaxSize < _tcp_size) || _tcp_size==0 ?
	_info.FMaxSize : _tcp_size;
	_info.FMaxSize =( _info.FMaxSize > 0 && _info.FMaxSize < aSize)|| aSize==0 ?
	_info.FMaxSize : aSize;
	if(_info.FMaxSize)
	_info.FType.MSetFlag(split_info::LIMITED, true);
	_info.pMCalculate=&f_calculate_size;
	VLOG(2)<<"new limits "<<_info;
	return _info;
}
int IMPL::sMConnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);
	IMPL* _p=reinterpret_cast<IMPL*>(aThis);
	_p->MAddToSelect();
	return 0;
}
int IMPL::sMDisconnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);

	IMPL* _p=reinterpret_cast<IMPL*>(aThis);
	_p->MRemoveFromSelect();
	_p->MCloseRequest();
	VLOG(2)<<"Disconnected";
	return 0;
}
template<>
void IMPL::MFill<close_main_channel_t>(data_t* aTo) const
{
	VLOG(2) << "Close  main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(close_main_channel_t));
	close_main_channel_t * _p =
	new ((data_t::value_type*) aTo->ptr() + _befor) close_main_channel_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_TCPSERVER);
	fill_dg_head(_p, sizeof(close_main_channel_t), get_my_id());
	VLOG(2) << (*_p);
}
template<>
void IMPL::MFill<main_channel_param_t>(data_t* aTo) const
{
	VLOG(2) << "Create main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(main_channel_param_t));
	main_channel_param_t * _p = new (aTo->ptr()) main_channel_param_t();
	_p->FLimit=MGetLimits(0).FMaxSize;
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_TCPSERVER);
	fill_dg_head(_p, sizeof(main_channel_param_t),get_my_id());
	VLOG(2) << (*_p);
}

void IMPL::MProcess(user_data_dg_t const* aP,
		parser_t* aThis)
{
	LOG_IF(DFATAL,!FTcp.MIsConnected()) << "The client " << FAddr
	<< " is not opened.";
	if (!FTcp.MIsConnected())
	return;


	user_data_t _user;
	if(deserialize(_user,aP,FBufferAlloc))
		FHandler->MReceivedData(_user);
}
int IMPL::MSendMainChannelError(unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
	new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_TCPSERVER);

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(), get_my_id());
	return MSendService(_channel);
}
bool IMPL::MCloseRequest()
{
	if (FState == E_CLOSED)
	return false;
	FHandler->MCloseRequest();
	return true;
}
bool IMPL::MClose()
{
	if (FState == E_CLOSED)
	return false;

	FTcp.MClose();

	VLOG(2) << "Close " << FAddr;
	data_t _buf;
	MFill<close_main_channel_t>(&_buf);
	FState = E_CLOSED;

	return MSendService(_buf);
}
int IMPL::MReceive()
{
	ISocket::data_t _data; //fixme shared memory
	FOverload.MStartCheking();
	VLOG(5)<<"Receive by "<<FAddr;
	int _size=FTcp.MReceiveData(&_data, 0.0);
	VLOG(5)<<"Receive  "<<_size<<" bytes.";
	if ( _size> 0)
	{
		FOverload.MCheking(0, _size);

		net_overload_t::over_t _over;
		FOverload.MFinishCheking(_over);

		VLOG(2) << "Receiver " << _data.size() << " bytes form "<<FAddr;
		ISocket::data_t const& _fix_data=_data;
		MReceivedData(_fix_data.begin(),_fix_data.end());
	}
	return _size;
}
bool IMPL::MSendService(const data_t& aVal) const
{
	if (FHandler)
	return FHandler->MSend(aVal);
	return false;
}
bool IMPL::MSendMainChannelParam() const
{
	data_t _buf;
	MFill<main_channel_param_t>(&_buf);

	bool _is =FTcp.MSend(_buf).MIs();
	(void) _is;
	LOG_IF(ERROR,!_is) << "Cannot send main channel params.";
	return _is;
}
//bool IMPL::MSend(user_data_t const& aData)
//{
//	CHECK(!aData.FData.MIsDeached());
//	VLOG(2) << "Send data " << aData.FDataId.FPacketNumber << " to " << FAddr;
//	//	send_buffer_t _current_send_buffers;
//	//Thread blocking protection and buffering limitation.
//	//if another thread is sending data to aFor, Let its send our buffer.
//	{
//		NSHARE::CRAII<CMutex> _block(FMutexSend);
//		CHECK(!aData.FData.empty());
//		if (!FBuffer.MPut(aData))
//			return false;
//		if (!FBuffer.MIsWorking())
//		{
//			operation_t _op(sMSend, this, operation_t::IO);
//			CDataObject::sMGetInstance().MPutOperation(_op);
//		}
//	}
//	return true;
//}
bool IMPL::MSend(user_data_t const& aData)
{
	CHECK(!aData.FData.MIsDetached());
	VLOG(2) << "Send data " << aData.FDataId.FPacketNumber << " to " << FAddr;

	CMainClient::eSendResult _result= CMainClient::E_SENT_ERROR;



	do
	{
		_result = MSendImpl(aData);
		if(_result == CMainClient::E_SENT_ERROR) //todo default action to config
		{
			NSHARE::CThread::sMYield();		//fixme magic number
		}
	}while (_result == CMainClient::E_SENT_ERROR && MIsOpened());
	if (_result == CMainClient::E_MUST_CLOSE)
	{
		LOG(WARNING)<<"Closing "<<FAddr<<" Immediately.";
		MCloseRequest();
		//todo cannot sent
	}
	return _result==CMainClient::E_SENT;
}
int IMPL::sMSend(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* YOU_DATA)
{
	CMainClient* _client=reinterpret_cast<CMainClient*>(YOU_DATA);
	NSHARE::intrusive_ptr<CMainClient> _lock(_client);
	//_client->MSendImpl();
	return 0;
}
IMPL::eSendResult IMPL::MSendImpl(
		user_data_t const& _id) const
{
	if (FState != E_OPENED)
	{
		LOG(ERROR)<<"Cannot send data as "<<FAddr<<" is not opened.";
		return E_SENT_ERROR;
	}

	data_t _head;
	serialize(&_head, _id.FDataId, _id.FData.size());
	if (MSendImpl(_head))
	{
		VLOG(5)<<"Sending data";
		if (MSendImpl(_id.FData))	//todo sequence send
		return E_SENT;
		LOG(DFATAL)<<"Cannot send data to "<<FAddr<<". Disconnecting it.";
		FState=E_BAN_SENT;
		return E_MUST_CLOSE;
	}
	return E_SENT_ERROR;
}
bool IMPL::MSendImpl(data_t const& aData) const
{
	CTCP::sent_state_t _state;
	do
	{
		_state=FTcp.MSend(aData);
	}while(_state.FError==CTCP::E_AGAIN);
	LOG_IF(ERROR,!_state.MIs())<<"Cannot sent: "<<_state;
	return _state.MIs();
}
//void IMPL::MSendImpl()
//{
//	CSendBuffering::data_list_t _data;
//	for (;;)
//	{
//		{
//			NSHARE::CRAII<CMutex> _block(FMutexSend);
//			if (FBuffer.MIsWorking())
//			{
//				LOG(WARNING) << "The buffers is sending "<<FAddr;
//				return;
//			}
//			if(!FBuffer.MTrySend(_data))
//			{
//				LOG(WARNING)<<"Cannot send data to "<<FAddr;
//				return;
//			}
//			VLOG(2) << "Sent " << FAddr;
//		}
//		CMainClient::eSendResult _result= CMainClient::E_SENT_ERROR;
//
//		//The iteration method is used instead of the pop_back method
//		//for avoiding dead looks
//		CSendBuffering::data_list_t::iterator _s_it = _data.begin(),_s_it_end(_data.end());
//		{
//			CHECK(!_data.empty());
//			VLOG(3) << "Sending data to " << FAddr;
//			for (; _s_it != _s_it_end && _result!=CMainClient::E_MUST_CLOSE && MIsOpened(); ++_s_it)
//			{
//				do
//				{
//					_result = MSendImpl(*_s_it);
//					if(_result == CMainClient::E_SENT_ERROR)//todo default action to config
//					{
//						NSHARE::CThread::sMYield();		//fixme magic number
//					}
//				}while (_result == CMainClient::E_SENT_ERROR && MIsOpened());
//			}
//			if(_s_it == _s_it_end)
//			{
//				_data.clear();
//			}
//			else
//			{
//				_data.erase(_data.begin(),_s_it);
//			}
//		}
//		{
//			NSHARE::CRAII<CMutex> _block(FMutexSend);
//			FBuffer.MFinishSend(_data);
//		}
//		if (_result == CMainClient::E_MUST_CLOSE)
//		{
//			LOG(WARNING)<<"Closing "<<FAddr<<" Immediately.";
//			MCloseRequest();
//			//todo cannot sent
//		}
//	}
//}
void IMPL::MSerialize(NSHARE::CConfig &aTo) const
{
	aTo.MAdd(FTcp.MSerialize());
	//aTo.MAdd(FBuffer.MSerialize());
	FParser.MGetState().MSerialize(aTo);
}
void IMPL::MAddToSelect()
{
	FThis.FSelectSock.MAddSocket(FTcp.MGetSocket());
	FThis.MUnLockSelect();
}
void IMPL::MRemoveFromSelect()
{
	FThis.FSelectSock.MRemoveSocket(FTcp.MGetSocket());
	FThis.MUnLockSelect();
}} /* namespace NUDT */
