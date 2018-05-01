/*
 * CTcpServerMainClient.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.08.2016
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
#include <core/kernel_type.h>
#include <core/CConfigure.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <io/IIOManager.h>
#include <io/ILink.h>
#include "../CChannelDiagnostics.h"

#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_USER_DATA,user_data_dg_t)/*Данные пользователя*/\
	RECEIVE(E_MAIN_CHANNEL_PARAM,main_channel_param_t)/*Настройки основного канала*/\
	/*END*/
#include <parser_in_protocol.h>
#include "CTcpServerMainChannel.h"
#include "CTcpServerMainClient.h"
using namespace NSHARE;
namespace NUDT
{
#define IMPL CTcpServerMainChannel::CMainClient
CTcpServerMainChannel::CMainClient::CMainClient(CTcpServerMainChannel& aThis,
		NSHARE::CTCPServer::client_t const& aClient, NSHARE::IAllocater * const aAlloc,ILink* aLink) :
		FHandler(aLink), //
		FParser(this), //
		FThis(aThis), //
		FState(E_CLOSED), //
		FClient(aClient),
		FBufferAlloc(aAlloc)
{
	FParser.FUserData = this;
}
void IMPL::MReceivedData(data_t::const_iterator const& _buf_begin,
		data_t::difference_type aSize)
{
	FParser.MReceivedData(_buf_begin, _buf_begin + aSize);
}
IMPL::parser_t::state_t const& IMPL::MGetState() const
{
	return FParser.MGetState();
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

	VLOG(2) << "Close " << FClient;
	data_t _buf;
	FThis.MFill<close_main_channel_t>(&_buf);
	FState = E_CLOSED;
	return MSendService(_buf);
}
void IMPL::MProcess(
		main_channel_param_t const* aP, parser_t* aThis)
{
	VLOG(2) << "Handle main channel param";
	main_ch_param_t _sparam(deserialize<main_channel_param_t, main_ch_param_t>(aP, (routing_t*)NULL, (error_info_t*)NULL));
	CHECK_EQ(_sparam.FType, E_MAIN_CHANNEL_TCPSERVER);

	size_t _limit = 0;
	_sparam.FValue.MGetIfSet("limit", _limit);
	VLOG(2) << "The client is founded.";
	FHandler->MSetLimits(FThis.MGetLimits(_limit,FHandler));
	FState = E_OPENED;
}
bool IMPL::MSendService(const data_t& aVal) const
{
	VLOG(2)<<"Send service msg.";
	if (FHandler)
		return FHandler->MSend(aVal);
	return false;
}
IMPL::eSendResult IMPL::MSend(user_data_t const & _id)
{

	CMainClient::eSendResult _result= CMainClient::E_SENT_ERROR;
	VLOG(3) << "Sending data to " << FClient<<" "<<_id.FDataId;
	HANG_INIT;
	do
	{

		data_t _head;
		serialize(&_head, _id.FDataId, _id.FData.size());
		if (FThis.MSendImpl(_head.ptr_const(), _head.size(), FClient.FAddr))
		{
			VLOG(5)<<"Sending data";
			if(!FThis.MSendImpl(_id.FData.ptr_const(), _id.FData.size(),
							FClient.FAddr))
			{
				if(FState == E_OPENED)
				{
					LOG(DFATAL)<<"Must close";
					_result = CMainClient::E_MUST_CLOSE;
				}
				else
				{
					VLOG(2)<<"finish sent";
					return CMainClient::E_SENT_ERROR;
				}
			}
			else
			_result = CMainClient::E_SENT;
		}
		else
		{
			LOG(ERROR)<<"Sent error";
			_result = CMainClient::E_SENT_ERROR;
			HANG_CHECK;
		}

	}while (_result == CMainClient::E_SENT_ERROR
			&&MIsOpened());
	if (_result == CMainClient::E_MUST_CLOSE)
	{
		LOG(WARNING)<<"Closing "<<FClient<<" Immediately.";
		MCloseRequest();
	}
	return _result;
}

void IMPL::MProcess(user_data_dg_t const* aP,
		parser_t* aThis)
{
	LOG_IF(DFATAL, !MIsOpened()) << "The client " << FClient
			<< " is not opened.";
	if (!MIsOpened())
		return;

	user_data_t _user;
	if(deserialize(_user,aP,FBufferAlloc))
		FHandler->MReceivedData(_user);
}
bool IMPL::MSendMainChannelError(unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_TCP);

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(), get_my_id());
	return MSendService(_channel);
}
} /* namespace NUDT */
