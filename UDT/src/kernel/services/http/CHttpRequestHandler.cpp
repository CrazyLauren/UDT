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

#include <core/kernel_type.h>
#include <core/IState.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/CDiagnostic.h>
#include "CHttpRequestHandler.h"
#include <io/CKernelIo.h>
#include <io/http/CHttpRequest.h>
#include <io/http/CHttpResponse.h>
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
	//std::cout << *aRequest << std::endl;

	NSHARE::smart_field_t<CUrl> const& _url = aRequest->MUrl();
	eHtppMethod const _method(aRequest->MMethod());

	CHttpResponse _response;
	eStatusCode _code(E_STARUS_UNINITIALIZED);
	bool _is_file=false;
	if (_method == E_GET || _method == E_POST)
	{
		if (!_url.MGetConst().MQuery().empty()
				|| (_method == E_POST && !aRequest->MBody().empty()))
		{

			if (_method == E_POST)
			{
				_code = E_STARUS_METHOD_NOT_ALLOWED;
			}
			else
			//application/json
			{
				typedef CUrl::qeury_t::const_iterator _it_t;

				CUrl::qeury_t const _query = _url.MGetConst().MQuery();
				std::pair<_it_t,_it_t>  _what=_query.equal_range("query");
				const bool _is = _what.first!=_what.second;
				if (_is)
				{
					bool const _is_deep=_query.find("deep") != _query.end();

					NSHARE::CConfig  _result;
					_code = E_STARUS_OK;
					for (; _what.first != _what.second && _code != E_STARUS_BAD_REQUEST; ++_what.first)
					{
						//std::cout<<"query="<<_what.first<<std::endl;
						NSHARE::CConfig const _conf(
								CDiagnostic::sMGetInstance().MSerialize(_what.first->second,
										_is_deep));
						if (!_conf.MIsEmpty())
						{
							_result.MAdd(_conf);
						}
						else
							_code = E_STARUS_BAD_REQUEST;
					}
					if (_what.first == _what.second)
					{
						_response.MSetBody(_result.MToJSON());
						_response.MAppendHeader("Content-Type",
								"application/json; charset=utf-8");
					}
				}
				else
					_code = E_STARUS_METHOD_NOT_ALLOWED;
			}
		}
		else
		{
			_is_file=true;

			NSHARE::CText const _path =
					"q:/workspaces/netbeans/udt_gui/public_html"
							+ _url.MGetConst().MPath();
			std::cout << "Read file:" << _path << std::endl;
			if (!_response.MWriteFile(_path))
			{
				_response.MSetBody("The page is not found:");
				_response.MAppendHeader("Content-Type",
						"text/html; charset=utf-8");
				std::cerr << "No page" << std::endl;
				_code = E_STARUS_NOT_FOUND;
			}
			else
				_code = E_STARUS_OK;

		}
	}
	bool const _has_to_close=_is_file||_code!=E_STARUS_OK;

	_response.MSetStatus(_code);
	_response.MAppendHeader("Server", "UDT");
	if(!_has_to_close)
		_response.MAppendHeader("Connection", "keep-alive");

	CKernelIo::sMGetInstance().MSendTo(aFrom, _response.MRaw());
	if(_has_to_close)
		CKernelIo::sMGetInstance().MClose(aFrom);
}
} /* namespace NUDT */
