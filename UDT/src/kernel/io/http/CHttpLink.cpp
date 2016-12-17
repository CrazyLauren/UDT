/*
 * CHttpLink.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.05.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <udt_share.h>
#include "../../core/kernel_type.h"
#include "../../core/CDataObject.h"
#include "../../core/CDescriptors.h"
#include "../ILinkBridge.h"
#include "../ILink.h"
#include "CHttpRequest.h"
#include "CHttpLink.h"

namespace NUDT
{

CHttpLink::CHttpLink(descriptor_t aFD,
		uint64_t aTime, ILinkBridge* aKer) :
		ILink("http", aTime),//
		FKernel(aKer),//
		Fd(aFD)
{
	FIsAccept = false;
	split_info _split;
	_split.FType.MSetFlag(split_info::CAN_NOT_SPLIT,true);
	MSetLimits(_split);
}

CHttpLink::~CHttpLink()
{
	Fd = CDescriptors::INVALID;
}
void CHttpLink::MReceivedData(CHttpRequest const& aRequest)
{
	if (!FIsAccept)
	{
		VLOG(2)
							<< "The Request is  received before calling  the accept method.";
		FRequests.push_back(aRequest);
		return;
	}
	VLOG(2)<<"Receive request";
	http_request_id_t _request(Fd, aRequest);
	CDataObject::sMGetInstance().MPush(_request, false);
}
bool CHttpLink::MSend(const data_t& aVal)
{
	return FKernel->MSend(aVal);
}
bool CHttpLink::MSend(const program_id_t& aVal, const routing_t& aRoute,error_info_t const&)
{
	return false;
}
bool CHttpLink::MSend(const user_data_t& aVal)
{
	return false;
}
bool CHttpLink::MSend(const fail_send_t& aVal, const routing_t& aRoute,error_info_t const&)
{
	return false;
}
bool CHttpLink::MSend(const kernel_infos_array_t& aVal, const routing_t& aRoute,error_info_t const&)
{
	return false;
}
bool CHttpLink::MSend(const demand_dgs_for_t& aVal, const routing_t& aRoute,error_info_t const&)
{
	return false;
}

void CHttpLink::MCloseRequest()
{
	FKernel->MCloseRequest(Fd);
}
void CHttpLink::MClose()
{
	Fd = CDescriptors::INVALID;
}
descriptor_t CHttpLink::MGetID() const
{
	return Fd;
}
bool CHttpLink::MIsOpened() const
{
	return true;
}
void CHttpLink::MReceivedData(NSHARE::CBuffer::const_iterator aBegin,
		NSHARE::CBuffer::const_iterator aEnd)
{
	unsigned _count = FRequest.MParse(&(*aBegin), aEnd - aBegin);
	VLOG(2) << "Parse " << _count << " bytes by http. Error="
						<< FRequest.MError();
	switch (FRequest.MError())
	{
	case E_HTTP_OK:
	{
		MReceivedData(FRequest);
		break;
	}
	case E_INVALID_EOF_STATE:
		break;
	default:
		LOG(ERROR)<<"Parsing http error."<<_count;
		MCloseRequest();
		break;
	}
}
NSHARE::CConfig CHttpLink::MSerialize() const
{
	return NSHARE::CConfig("http");
}
bool CHttpLink::MAccept()
{
	VLOG(2)<<"Accepting";
	FIsAccept = true;
	for (std::vector<CHttpRequest>::const_iterator _it = FRequests.begin(),
			_end = FRequests.end(); _it != _end; ++_it)
	{
		MReceivedData(*_it);
	}
	FRequests.clear();
	return true;
}

} /* namespace NSHARE */
