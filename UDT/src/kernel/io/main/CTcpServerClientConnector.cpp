// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CTcpServerClientConnector.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 01.11.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/share_socket.h>
#include <string.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include <core/kernel_type.h>
#include <core/CConfigure.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <io/IIOManager.h>
#include "../CChannelDiagnostics.h"
#include <io/ILink.h>

#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_MAIN_CHANNEL_PARAM,main_channel_param_t)/*Настройки основного канала*/\
	/*END*/
#include <parser_in_protocol.h>

#include "CTcpServerMainChannel.h"
#include "CTcpServerMainClient.h"
#include "CTcpServerClientConnector.h"

namespace NUDT
{
using namespace NSHARE;
#define IMPL CTcpServerMainChannel::CMainClientConnector

CTcpServerMainChannel::CMainClientConnector::CMainClientConnector():FParser(this)
{
	FParser.FUserData = this;
}
void IMPL::MReceivedData(data_t::const_iterator const& _buf_begin,
		data_t::difference_type aSize)
{
	FLastLink.MUnSet();
	FParser.MReceivedData(_buf_begin, _buf_begin + aSize);
	FParser.MCleanBuffer();
}
NSHARE::smart_field_t<IMPL::new_link_t> const& IMPL::MGetLastNewLink() const
{
	return FLastLink;
}
void IMPL::MProcess(
		main_channel_param_t const* aP, parser_t* aThis)
{
	VLOG(2) << "Handle main channel param";	
	NSHARE::CRAII < NSHARE::CMutex > _block(FMutexNewLinks);

	new_links_t::iterator _it = FNewLinks.begin(), _it_end(
			FNewLinks.end());

	for (; _it != _it_end; ++_it)
	{
		VLOG(2) << "Link " << _it->first;
		if (_it->second.FId.FId.FUuid == aP->MGetFromUUID())
		{
			VLOG(2) << "The client is founded.";
			CHECK(!FLastLink.MIs());
			FLastLink=_it->second;
			FNewLinks.erase(_it);
			break;//todo
		}
	}
}
bool IMPL::MAddNew(ILink* aHandler, program_id_t const& aId)
{
	CRAII<CMutex> _block(FMutexNewLinks);

	bool _is = FNewLinks.find(aHandler->MGetID()) != FNewLinks.end();
	LOG_IF(ERROR,_is) << "Retrying open main channel for "
								<< aHandler->MGetID();

	if (!_is)
	{
		new_link_t _val;
		_val.FId = aId;
		_val.FLink = aHandler;
		FNewLinks[aHandler->MGetID()] = _val;
	}
	return !_is;
}
bool IMPL::MHandleServiceDG(main_channel_error_param_t const* aP,
		descriptor_t aFor)
{
	CRAII<CMutex> _block(FMutexNewLinks);
	new_links_t::iterator _it = FNewLinks.find(aFor);
	if (_it != FNewLinks.end()
			&& aP->FError != main_channel_error_param_t::E_OK)		//todo
	{
		VLOG(2) << "Close " << aFor << " as new client. Error = "
		<< aP->FError;
		FNewLinks.erase(_it);
		return false;
	}
	return true;
}
bool IMPL::MClose(descriptor_t aFor)
{
	CRAII<CMutex> _block(FMutexNewLinks);
	new_links_t::iterator _it = FNewLinks.find(aFor);
	if (_it != FNewLinks.end())
	{
		VLOG(2) << "Close " << aFor << " as new client.";
		FNewLinks.erase(_it);
		return true;
	}
	LOG(ERROR)<<"Cannot close  "<<aFor;
	return false;
}
bool IMPL::MCloseRequest(descriptor_t aFor)
{
	CRAII<CMutex> _block(FMutexNewLinks);
	new_links_t::iterator _it = FNewLinks.find(aFor);
	if (_it != FNewLinks.end())
	{
		VLOG(2) << "Close " << aFor << " as new client.";
		_it->second.FLink->MCloseRequest();
		return true;
	}
	LOG(ERROR)<<"Cannot close request "<<aFor;
	return false;
}

} /* namespace NUDT */
