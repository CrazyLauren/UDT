/*
 * CHttpRequestHandler.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 13.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>

#include "../core/kernel_type.h"
#include "../core/IState.h"
#include "../core/CDescriptors.h"
#include "../core/CDataObject.h"
#include "../core/CDiagnostic.h"
#include "CHttpRequestHandler.h"
#include "../io/CKernelIo.h"
#include "../io/http/CHttpRequest.h"
#include "../io/http/CHttpResponse.h"
namespace NUDT
{

CHttpRequestHandler::CHttpRequestHandler()
{
	MInit();
}

CHttpRequestHandler::~CHttpRequestHandler()
{

}
void CHttpRequestHandler::MInit()
{
	{
		callback_data_t _cb(sMHandleRequestId, this);
		CDataObject::value_t _val(http_request_id_t::NAME, _cb);
		CDataObject::sMGetInstance() += _val;
	}
}
int CHttpRequestHandler::sMHandleRequestId(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA)
{
	CHttpRequestHandler* _this =
			reinterpret_cast<CHttpRequestHandler*>(YOU_DATA);
	CHECK_NOTNULL(_this);
	CHECK_EQ(http_request_id_t::NAME, WHAT->FType);
	http_request_id_t const* _p =
			reinterpret_cast<http_request_id_t*>(WHAT->FPointToData);
	CHECK_NOTNULL(_p);
	_this->MHandleFrom(&_p->FVal, _p->FId);
	return 0;
}
void CHttpRequestHandler::MHandleFrom(CHttpRequest const* aRequest,
		descriptor_t aFrom)
{
	std::cout << *aRequest << std::endl;

	NSHARE::smart_field_t<CUrl> const& _url = aRequest->MUrl();
	eHtppMethod const _method(aRequest->MMethod());

	CHttpResponse _response;
	eStatusCode _code(E_STARUS_UNINITIALIZED);

	if (_method == E_GET || _method == E_POST)
	{
		if (!_url.MGetConst().MQuery().empty())
		{
			CUrl::qeury_t  _query = _url.MGetConst().MQuery();
			NSHARE::CText const _what = _query["query"];
			if (!_what.empty())
			{
				NSHARE::CConfig const _conf(
						CDiagnostic::sMGetInstance().MSerialize(_what,
								_query.find("deep") != _query.end()));
				if (!_conf.MIsEmpty())
				{
					_code = E_STARUS_OK;
					_response.MSetBody(_conf.MToJSON());
					_response.MAppendHeader("Content-Type", "application/json; charset=utf-8");
				}
				else
					_code = E_STARUS_BAD_REQUEST;
			}
			else
				_code = E_STARUS_METHOD_NOT_ALLOWED;
		}
		else
		{
			_code=E_STARUS_OK;
			_response.MSetBody("UDT core welcome you!!!");
			_response.MAppendHeader("Content-Type", "text/html; charset=utf-8");
		}
	}
	_response.MSetStatus(_code);
	_response.MAppendHeader("Server", "UDT");
	_response.MAppendHeader("Connection", "keep-alive");

	CKernelIo::sMGetInstance().MSendTo(aFrom,  _response.MRaw());

	CKernelIo::sMGetInstance().MClose(aFrom);
}
} /* namespace NUDT */
