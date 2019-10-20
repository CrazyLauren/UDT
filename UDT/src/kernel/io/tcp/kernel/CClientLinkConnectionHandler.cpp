// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CClientLinkConnectionHandler.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share/share_socket.h>

#include <udt_share.h>
#include <core/kernel_type.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/CConfigure.h>
#include <io/ILink.h>
#include <io/CKernelIo.h>
#include <io/CLinkDiagnostic.h>
#include <io/main/CMainChannelFactory.h>
#include "../ILinkBridge.h"

#include "../CConnectionHandlerFactory.h"

#include <internel_protocol.h>
#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_PROTOCOL_MSG,protocol_type_dg_t)/*Протокол КД*/\
	RECEIVE(E_REQUEST_INFO,requiest_info2_t)/*Запрос информации*/\
	/*END*/
#include <parser_in_protocol.h>

#include "CKernelClientLink.h"
#include "CClientLinkConnectionHandler.h"

namespace NUDT
{
#define IMPL CKernelClientLink::CConnectionHandler
CKernelClientLink::CConnectionHandler::CConnectionHandler(descriptor_t aFD,
		uint64_t aTime, ILinkBridge* aKer) :
		FParser(this), //
		FBridge(aKer), //
		Fd(aFD), //
		FTime(aTime), //
		FPType(E_KERNEL)
{
	FProtocolIsValid = false;
	FState = E_CONTINUE;
	FIsSent = false;
}

IMPL ::~CConnectionHandler()
{
}
bool IMPL::MConnect()
{
	FProtocolIsValid=false;

	bool _is=MSendProtocolType();
	if (!_is)
	{
		FState = E_ERROR;
	}
	return _is;
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

	return MHandling(aBegin, aEnd);
}
ILink* IMPL::MCreateLink()
{
	return FLink.MGet();
}

bool CKernelClientLink::CConnectionHandler::MIsOpened() const
{
	return FLink->MIsOpened();
}
bool IMPL::MHandling(NSHARE::CBuffer::const_iterator aBegin,
		NSHARE::CBuffer::const_iterator aEnd)
{
	VLOG(2) << "Handle new connection buffer.";
	FParser.MReceivedData(aBegin, aEnd);
	LOG_IF(FATAL,!FKernel.MIs())<<"No kernel INFO";
	if(FKernel.MIs())
	{

		bool const _is=MSendIDInfo();
		if(!_is)
		{
			FState= E_ERROR;
		}
		else
		{
			FState=E_OK;
			MOpenLink();
		}
		return _is;
	}
	return false;
}

bool CKernelClientLink::CConnectionHandler::MSendImpl(const data_t& _buf)
{
	bool _is = FBridge->MSend(_buf);
	LOG_IF(ERROR,!_is) << "Cannot send id info";
	if (!FIsSent)
		FIsSent = _is;
	return _is;
}

bool IMPL::MSendIDInfo()
{
	data_t _buf;
	serialize<dg_info2_t,program_id_t>(&_buf,get_my_id(),routing_t(),error_info_t());

	CHECK(FBridge);
	bool _is = MSendImpl(_buf);
	return _is;
}
inline unsigned IMPL::MFillProtocol(data_t* aTo,
		const eProgramType& aType)
{
	const size_t full_size = sizeof(protocol_type_dg_t);

	//allocate memory
	size_t const _befor = aTo->size();
	aTo->resize(_befor + full_size);
	char * const _begin = (char*) aTo->ptr() + _befor;

	CHECK_LE(full_size, aTo->size() - _befor);

	//fill dg
	protocol_type_dg_t * _request = new (_begin) protocol_type_dg_t();
	_request->MSetProtocol(aType);

	//calc CRC and fill it
	fill_dg_head(_begin, full_size,get_my_id());

	//paranoid check
	CHECK_EQ(full_size,
			(size_t )(reinterpret_cast<protocol_type_dg_t*>(_begin)->FHeadSize
					+ reinterpret_cast<protocol_type_dg_t*>(_begin)->MGetDataSize()));

	VLOG(2) << "DG Protocol Info "
	<< *reinterpret_cast<protocol_type_dg_t*>(_begin);
	return full_size;
}
bool IMPL::MSendProtocolType()
{
	VLOG(2) << "Send id info.";
	data_t _data;
	MFillProtocol(&_data,FPType);
	CHECK(FBridge);
	return MSendImpl(_data);
}
void IMPL::MOpenLink()
{
//add new abonent
	LOG(INFO)<< "Setting create link";
//check for local host
	CHECK(FKernel.MIs());
	FLink=new CKernelClientLink(Fd,FTime,FBridge.MGet(),FKernel.MGetConst());
}
template<>
void IMPL::MProcess(protocol_type_dg_t const* aP, parser_t* aThis)
{
	CHECK(false);
}
template<>
void IMPL::MProcess(requiest_info2_t const* aP, parser_t* aThis)
{
	CHECK_EQ(sizeof(requiest_info2_t) + aP->MGetStrSize(),
			aP->MGetDataSize() + aP->FHeadSize);

	program_id_t const _customer(deserialize<requiest_info2_t,program_id_t>(aP,NULL,NULL));

	VLOG(2)<<_customer;
	if (_customer.FType != FPType)
	{
		LOG(DFATAL)<<_customer<<" is not kernel";
		FState=E_ERROR;
		return;
	}
	CHECK(CDescriptors::sMIsValid(Fd));
	FKernel = _customer;
}}
/* namespace NUDT */
