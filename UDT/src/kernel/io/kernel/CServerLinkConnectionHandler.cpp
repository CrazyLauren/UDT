/*
 * CServerLinkConnectionHandler.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 04.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>

#include <udt_share.h>
#include "../../core/kernel_type.h"
#include "../../core/CDescriptors.h"
#include "../../core/CDataObject.h"
#include "../../core/CConfigure.h"
#include "../ILinkBridge.h"
#include "../ILink.h"
#include "../main/CMainChannelFactory.h"
#include "../CKernelIo.h"
#include "../CConnectionHandlerFactory.h"
#include "../CLinkDiagnostic.h"
#include "internel_protocol.h"
#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_PROTOCOL_MSG,protocol_type_dg_t)/*Протокол КД*/\
	RECEIVE(E_INFO,dg_info2_t)/*Ответ на КД Запрос информации*/\
	/*END*/
#include "parser_in_protocol.h"

#include "CKernelServerLink.h"
#include "CServerLinkConnectionHandler.h"

namespace NUDT
{
#define IMPL CKernelServerLink::CConnectionHandler
CKernelServerLink::CConnectionHandler::CConnectionHandler(descriptor_t aFD,
		uint64_t aTime, ILinkBridge* aKer) :
		FParser(this), //
		FBridge(aKer), //
		Fd(aFD), //
		FTime(aTime)
{
	//FLink = new CKernelServerLink(aFD, aTime, aKer);
	FProtocolIsValid = false;
	FState = E_CONTINUE;
	FConnectionState = E_NO_STATE;
	FIsSent=false;
}

IMPL ::~CConnectionHandler()
{
}
bool IMPL::MConnect()
{
	FIsSent=false;
	FProtocolIsValid=false;
	FConnectionState = E_CONNECTED;
	return FIsSent;
}
bool IMPL::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd)
{
	if (!FProtocolIsValid)
	{
		if(!CInParser<void>::sMIsValidProtocol(aBegin, aEnd))
		{
			FState= E_ERROR;
			return false;
		}
		FProtocolIsValid = true;
	}

	MHandling(aBegin, aEnd);
	VLOG(2)<<"Server state ="<<FState;
	return FIsSent;
}
ILink* IMPL::MCreateLink()
{
	return FLink.MGet();
}
bool IMPL::MHandling(NSHARE::CBuffer::const_iterator aBegin,
		NSHARE::CBuffer::const_iterator aEnd)
{
	VLOG(2) << "Handle new connection buffer. State=" << FConnectionState;
	switch (FConnectionState)
	{
		case E_CONNECTED:
		{
			VLOG(2) << "Receive protocol type";
			FParser.MReceivedData(aBegin, aEnd);
			if (FConnectionState == E_REQUEST_ID)
			return MSendIDInfo();
			else
			return false;
			break;
		}
		case E_REQUEST_ID:
		{
			VLOG(2) << "request id";
			FParser.MReceivedData(aBegin, aEnd);
			if (FConnectionState == E_HAS_ID)
			{
				bool _is = MOpenLink();
				if (_is)
				{
					VLOG(2)<<"Continue";
					FState= E_CONTINUE;
					_is=FLink->MOpening(aBegin, aEnd);
				}
				if (_is)
				{
					VLOG(2)<<"Continue";
					FState= E_CONTINUE;
				}
				else if (FLink->MIsOpened())
				{
					VLOG(2)<<"Adding new Kernel";
					FState= E_OK;
				}
				else
				FState= E_ERROR;
			}
			else
			{
				LOG(ERROR)<<"There is not id info.";
				FState= E_ERROR;
			}
			break;
		}
		case E_HAS_ID:
		{
			VLOG(2) << "has id";
			bool _is = FLink->MOpening(aBegin, aEnd);
			if (_is)
			{
				VLOG(2)<<"Continue";
				FState= E_CONTINUE;
			}
			else if (FLink->MIsOpened())
			{
				VLOG(2)<<"Adding new Kernel";
				FState= E_OK;
			}
			else
				FState= E_ERROR;
			break;
		}
		default:
		LOG(FATAL)<<"Invalid state.";
		break;
	}
	return false;
}

bool CKernelServerLink::CConnectionHandler::MSendImpl(const data_t& _buf)
{
	bool _is = FBridge->MSend(_buf);
	LOG_IF(ERROR,!_is) << "Cannot send id info";
	if(!FIsSent)
		FIsSent=_is;
	return _is;
}

bool IMPL::MSendIDInfo()
{
	data_t _buf;
	serialize<requiest_info2_t,programm_id_t>(&_buf,get_my_id(),routing_t(),error_info_t());

	CHECK(FBridge);
	bool _is = MSendImpl(_buf);
	return _is;
}
bool IMPL::MOpenLink()
{
	CHECK(FConnectionState == E_HAS_ID);
//add new abonent
	LOG(INFO)<< "Setting create link";
//check for local host
	CHECK(FKernel.MIs());
	FLink=new CKernelServerLink(Fd,FTime,FBridge.MGet(),FKernel.MGetConst());
	return FLink->MOpen();
}
NSHARE::CText IMPL::MGetMainChannelType()
{
	NSHARE::CText _val;
	NSHARE::CConfig _configure;
	FBridge->MConfig(_configure);
	if(!_configure.MIsEmpty())
	{
		NSHARE::CConfig const& _conf=_configure.MChild(NAME).MChild(MAIN_CHANNEL_TYPE);
		if(!_conf.MIsEmpty())
		{
			_conf.MGetIfSet(DEFAULT,_val);
			CHECK(FKernel.MIs());
			_conf.MGetIfSet(FKernel.MGetConst().FId.FName,_val);
		}
	}
	CHECK(!_val.empty());
	return _val;
}
template<>
void IMPL::MProcess(protocol_type_dg_t const* aP, parser_t* aThis)
{
	LOG_IF(DFATAL,FConnectionState != E_CONNECTED) << "Error state: " << FState;
	if (FConnectionState == E_CONNECTED)
	{
		VLOG_IF(2,aP->FProtocol != E_KERNEL)<< "Invalid protocol type.";
		if (aP->FProtocol == E_KERNEL)
		{
			VLOG(2) << "Change state from Not Opened to Request info";
			FConnectionState = E_REQUEST_ID;
		}
		else
		{
			VLOG(2)<<"The protocol is invalid.";
			FState=E_ERROR;
		}
	}
}
template<>
void IMPL::MProcess(dg_info2_t const* aP, parser_t* aThis)
{
	CHECK_EQ(sizeof(dg_info2_t) + aP->FStrSize,
			aP->FDataSize + aP->FHeadSize);
	CHECK(FConnectionState != E_CONNECTED);

	programm_id_t _customer(deserialize<dg_info2_t,programm_id_t>(aP,NULL,NULL));

	VLOG(2)<<_customer;
	if (_customer.FType != E_KERNEL)
	{
		LOG(DFATAL)<<_customer<<" is not kernel";
		FState=E_ERROR;
		return;
	}
	CHECK(CDescriptors::sMIsValid(Fd));
	FKernel = _customer;
	FConnectionState = E_HAS_ID;
}
} /* namespace NUDT */
