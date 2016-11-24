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
void CHttpRequestHandler::MHandleFrom(CHttpRequest const* aRequest, descriptor_t aFrom)
{
	CHttpResponse _response;
	_response.MSetStatus(E_STARUS_OK);
	_response.MAppendHeader("Server", "UDT");
	_response.MAppendHeader("Content-Type", "text/html; charset=utf-8");
	_response.MAppendHeader("Connection", "keep-alive");

	NSHARE::CBuffer _data;
	NSHARE::smart_field_t<CUrl> const& _url=aRequest->MUrl();
	if(_url.MIs())
	{
		std::cout<<_url.MGetConst().MPath()<<std::endl;
		NSHARE::CText _path="q:/workspaces/workspace-aptana/kernel/WebContent"+_url.MGetConst().MPath();
		std::ifstream _stream;
		_stream.open(_path.c_str());
		if(_stream.is_open())
		{
			std::stringstream buffer;
			buffer << _stream.rdbuf();

			_response.MSetBody( buffer.str());
			_data = _response.MRaw();

		}
	}
	if (_data.empty())
	{
		_response.MSetBody("UDT core welcome you!!!");

		_data = _response.MRaw();
	}
	if(!_data.empty())
		CKernelIo::sMGetInstance().MSendTo(aFrom, _data);
	CKernelIo::sMGetInstance().MClose(aFrom);
}
} /* namespace NUDT */
