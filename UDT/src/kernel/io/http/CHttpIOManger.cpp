/*
 * CHttpIOManger.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.05.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <udt_share.h>
#include <CParserFactory.h>
#include <core/kernel_type.h>
#include <core/CDataObject.h>
#include <core/CDescriptors.h>
#include <core/CDiagnostic.h>
#include <core/CConfigure.h>
#include <io/CKernelIo.h>

#include "CHttpRequest.h"

#include "CHttpIOManger.h"
#include "CHttpResponse.h"

namespace NUDT
{
using namespace NSHARE;
//CHttpIOManger::NAME is definition in CHttpIOManagerRegister.
NSHARE::CText const CHttpIOManger::PORT = "port";
NSHARE::CText const CHttpIOManger::BUF_MAX_SIZE="maxbuf";
NSHARE::CText const CHttpIOManger::PARSER_ERROR = "par_fail";
NSHARE::CText const CHttpIOManger::SNIFFER = "sniffer";
NSHARE::CText const CHttpIOManger::WWW_PATH = "./gui";
NSHARE::CText const CHttpIOManger::I_WHAT_RECEIVE = "i_what_receive";
NSHARE::CText const CHttpIOManger::DO_NOT_RECEIVE_MSG = "do_not_receive_msg";
NSHARE::CText const CHttpIOManger::SNIFFER_STATE = "snif_state";
NSHARE::CText const CHttpIOManger::SNIFFED_DATA = "sniffed_data";
NSHARE::CText const CHttpIOManger::SEQUENCE_NUMBER = "seq_num";
NSHARE::CText const CHttpIOManger::KERNEL_UUID = "server_uuid";
static const NSHARE::CText START_PAGE("/index.html");
CHttpIOManger::CHttpIOManger() :
		IIOManager(NAME), //
		FIo(NULL), //
		Fd(CDescriptors::INVALID), //
		FProgId(get_my_id()), //
		FUniqueNumber(1), //
		FSinffedNum(1), //
		FBufferingBytes(10*1024*1024)//max 10 mb

{
	FProgId.FId.FName="GUI";
	FProgId.FId.FUuid = NSHARE::get_uuid(FProgId.FId.FName);
	FProgId.FType = E_CONSUMER;
}

CHttpIOManger::~CHttpIOManger()
{
	MCloseImpl();
}
void CHttpIOManger::MInit(CKernelIo *aVal)
{
	VLOG(2) << "Init";
	CHECK_NOTNULL(aVal);
	CHECK(FIo==NULL);
	FIo = aVal;

	FCBServiceConnect = NSHARE::CB_t(sMConnect, this);
	FCBServiceDisconncet = NSHARE::CB_t(sMDisconnect, this);

	FTcpServiceSocket += CTCPServer::value_t(CTCPServer::EVENT_CONNECTED,
			FCBServiceConnect);
	FTcpServiceSocket += CTCPServer::value_t(CTCPServer::EVENT_DISCONNECTED,
			FCBServiceDisconncet);
}
bool CHttpIOManger::MOpen(const void*)
{
	CHECK_NE(CConfigure::sMGetInstance().MGet().MKey(), NAME);
	CConfig const* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);

	unsigned _port = 18012;
	split_info _split;
	_split.FType.MSetFlag(split_info::CAN_NOT_SPLIT,true);
	if (_p)
	{
		VLOG(2)<<"From Config "<<_p->MToJSON(true);
		bool _val = _p->MGetIfSet(PORT, _port);
		(void) _val;
		LOG_IF(WARNING,!_val)
										<< "The port number is not present in the config file"
										<< CConfigure::sMGetInstance().MGetPath()
										<< ".Using standard port " << _port;
		_p->MGetIfSet(BUF_MAX_SIZE, FBufferingBytes);
		NSHARE::CConfig const& _conf = _p->MChild(split_info::NAME);
		if (!_conf.MIsEmpty())
		{
			_split = split_info(_conf);
		}
	}
	VLOG(2) << "Construct IOContol Port=" << _port << ":" << this;
	bool const _is=FTcpServiceSocket.MOpen(net_address(_port));
	if(!_is)
		return false;

	NSHARE::operation_t _op(CHttpIOManger::sMReceiver, this,
			NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);

	Fd = CDescriptors::sMGetInstance().MCreate();

	descriptor_info_t _info;

	_info.FConnectTime = NSHARE::get_unix_time();
	_info.FTypeLink = NAME;
	_info.FProgramm = FProgId;
	_info.FInfo = FTcpServiceSocket.MSettings();

	CDescriptors::sMGetInstance().MOpen(Fd, _info);
	VLOG(2) << "Connected " << NSHARE::get_unix_time();





	FIo->MAddChannelFor(Fd, this, _split);
	return true;
}
NSHARE::eCBRval CHttpIOManger::sMReceiver(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CHttpIOManger*>(aData)->MReceiver();
	return E_CB_REMOVE;
}
void CHttpIOManger::MReceiver()
{
	VLOG(2) << "Async receive";
	ISocket::data_t _data;
	LOG_IF(FATAL, !FTcpServiceSocket.MIsOpen()) << "Port is closed";
	for (; FTcpServiceSocket.MIsOpen();)
	{
		_data.clear();
		CTCPServer::recvs_t _from;

		if (FTcpServiceSocket.MReceiveData(&_from, &_data, 0.0) > 0)
			MReceivedImpl(_data, _from);
	}
	VLOG(1) << "Socket closed";
}
void CHttpIOManger::MReceivedImpl(const NSHARE::ISocket::data_t& aData,
		const CTCPServer::recvs_t& aFrom)
{
	VLOG(2) << "Receive :" << aData.size();
	{
		for (CTCPServer::recvs_t::const_iterator _it = aFrom.begin();
				_it != aFrom.end(); ++_it)
		{
			CHttpRequest _request;
			unsigned _count = 0;

			VLOG(2) << "From " << _it->FClient << " " << _it->FSize
								<< " bytes.";
			streamed_data_t::iterator _stream = FStreamedData.find(
					_it->FClient.FAddr);

			bool const _was_streamed = _stream != FStreamedData.end();

			if (!_was_streamed)
			{
				_count = _request.MParse(_it->FBufBegin.base(), _it->FSize);
			}
			else
			{
				VLOG(2) << "Data from " << _it->FClient.FAddr
									<< " was streamed";

				_stream->second.insert(_stream->second.end(), _it->FBufBegin,
						_it->FBufBegin + _it->FSize);

				_count = _request.MParse(_stream->second.ptr_const(),
						_stream->second.size());
			}

			VLOG(2) << "Parse " << _count << " bytes by http. Error="
								<< _request.MError();

			bool _is_streamed = false;
			switch (_request.MError())
			{
			case E_HTTP_OK:
			{
				CHttpResponse _response;
				MHandleFrom(&_request, _response);
				NSHARE::CBuffer const _buffer(_response.MRaw());
				VLOG(2) << "Sending answer";
				CHECK(!_buffer.empty());
				FTcpServiceSocket.MSend(_buffer.ptr_const(), _buffer.size(),
						_it->FClient.FAddr);
				break;
			}
			case E_INVALID_EOF_STATE:
			{
				VLOG(1) << "Invalid EOF state ";
				_is_streamed = true;

				if (!_was_streamed)
				{
					ISocket::data_t _data(NULL, _it->FBufBegin,
							_it->FBufBegin + _it->FSize);
					FStreamedData[_it->FClient.FAddr] = _data;
				}
				break;
			}
			default:
			{
				LOG(ERROR)<<"Parsing http error."<<_count<<"("<<_request.MError()<<")";
				//
				break;
			}
			}

			if (!_is_streamed && _was_streamed)
				FStreamedData.erase(_stream);
			//VLOG(2)<<_it->FClient<<" handled";
		}
	}
	LOG_IF(INFO, aData.empty()) << "Empty Data.";
}

eStatusCode CHttpIOManger::MReadFile(const NSHARE::CText& aPath,
		CHttpResponse& _response)
{
	const NSHARE::CText _path = WWW_PATH
			+ ((aPath.size() <= 1) ? START_PAGE : aPath);

	eStatusCode _code(E_STARUS_UNINITIALIZED);
	if (!_response.MWriteFile(_path))
	{
		LOG(ERROR)<<"The page is not found:"<<_path;
		_response.MSetBody("The page is not found:");
		_response.MAppendHeader("Content-Type", "text/html; charset=utf-8");
		_code = E_STARUS_NOT_FOUND;
	}
	else
		_code = E_STARUS_OK;

	return _code;
}

eStatusCode CHttpIOManger::MGetStateInfo(const CUrl& _url,
		CHttpResponse& _response)
{
	//application/json
	typedef CUrl::qeury_t::const_iterator _it_t;
	eStatusCode _code(E_STARUS_UNINITIALIZED);

	const CUrl::qeury_t _query = _url.MQuery();
	std::pair<_it_t, _it_t> _what = _query.equal_range("query");
	const bool _is = _what.first != _what.second;
	if (_is)
	{
		const bool _is_deep = _query.find("deep") != _query.end();
		NSHARE::CConfig _result;
		_code = E_STARUS_OK;
		for (; _what.first != _what.second && _code != E_STARUS_BAD_REQUEST;
				++_what.first)
		{

			const NSHARE::CConfig _conf(
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
			MPutAsJson(_result, _response);
		}
	}
	else
		_code = E_STARUS_METHOD_NOT_ALLOWED;

	return _code;
}

bool CHttpIOManger::MHandleFrom(CHttpRequest const* aRequest,
		CHttpResponse & _response)
{
	//std::cout << *aRequest << std::endl;

	NSHARE::smart_field_t<CUrl> const& _url = aRequest->MUrl();
	NSHARE::CText const _path = _url.MGetConst().MPath().MGetConst();
	CHttpRequest::header_t const& _header = aRequest->MHeader();

	eHtppMethod const _method(aRequest->MMethod());

	eStatusCode _code(E_STARUS_UNINITIALIZED);
	bool _has_to_close = false;
	switch (_method)
	{
	case E_GET:
	{
		if (!_url.MGetConst().MQuery().empty())
		{
			VLOG(2) << "Get request";
			_code = MGetStateInfo(_url.MGetConst(), _response);
		}
		else
		{
			VLOG(2) << "Read file";
			_code = MReadFile(_path, _response);
			_has_to_close = true;
		}
		break;
	}

	case E_POST:
	{

		CHttpRequest::header_t::const_iterator _it = _header.find(
				"content-type");
		bool const _is_json = _it != _header.end()
				&& _it->second.find("application/json") != CText::npos;
		NSHARE::CBuffer const& _body = aRequest->MBody();
		NSHARE::CConfig _conf;

		if (_is_json && _conf.MFromJSON(_body))
		{
			CText::size_type _pos = CText::npos;
			if ((_pos = _path.find(SNIFFER)) != CText::npos)
			{
				VLOG(2) << "It Is sniffer";
				NSHARE::CText const _req(_path, _pos);
				if (!_conf.MIsEmpty())
				{
					_code = MHandleSniffer(_conf, _req, _response);
				}
				else
				{
					LOG(ERROR)<<"Cannot parse post request.";
					_code = E_STARUS_BAD_REQUEST;
				}
			}
			else
			{
				LOG(ERROR)<<"The page is not found:";
				_response.MSetBody("The page is not found:");
				_response.MAppendHeader("Content-Type", "text/html; charset=utf-8");
				_code = E_STARUS_NOT_FOUND;
			}
		}
		else
		{
			LOG(ERROR)<<"Cannot parse post request.";
			_code = E_STARUS_BAD_REQUEST;
		}

		break;
	}
	}
	_has_to_close = _has_to_close || _code != E_STARUS_OK;

	_response.MSetStatus(_code);
	_response.MAppendHeader(KERNEL_UUID, get_my_id().FId.FUuid.MSerialize().MValue());
	if (!_has_to_close)
		_response.MAppendHeader("Connection", "keep-alive");

	return _has_to_close;
}

void CHttpIOManger::MPutAsJson(const NSHARE::CConfig& _data,
		CHttpResponse& _response)
{
	NSHARE::CBuffer _buf;
	_data.MToJSON(_buf);
	_response.MSetBody(_buf);
	_response.MAppendHeader("Content-Type", "application/json; charset=utf-8");
}

void CHttpIOManger::MPutSniffedDataFrom(const size_t _number,
		NSHARE::CConfig& aTo) const
{

	NSHARE::CRAII<NSHARE::CMutex> _block(FSniffedMutex);
	data_fifo_t::const_reverse_iterator _it = FSniffedData.rbegin(),
			_it_end = FSniffedData.rend();
	for (; _it != _it_end && _it->FSinffedNum > _number; ++_it)
	{
		aTo.MAdd(_it->FData);
		VLOG(6)<<_it->FData;
	}
}

eStatusCode CHttpIOManger::MHandleSniffer(const NSHARE::CConfig& aConf,
		const NSHARE::CText& aPath, CHttpResponse& _response)
{
	VLOG(2) << "Request " << aPath << " Data " << aConf;

	eStatusCode _code(E_STARUS_UNINITIALIZED);

	CText::size_type _pos = CText::npos;
	if ((_pos = aPath.find(I_WHAT_RECEIVE)) != CText::npos)
	{
		demand_dg_t _demamd(aConf.MChild(demand_dg_t::NAME));
		_demamd.FHandler = 0; //will changed
		if (_demamd.MIsValid())
		{
			CText const _parser=aConf.MValue(user_data_t::PARSER, CText());
			VLOG(2) << "Can receive "<<_parser;
			if (MReceive(_demamd, _parser))
				_code = E_STARUS_OK;
			else
				_code = E_STARUS_BAD_REQUEST;
		}
		else
		{
			LOG(ERROR)<<"Cannot parse demand.";
			_code = E_STARUS_BAD_REQUEST;
		}
	}
	else if ((_pos = aPath.find(DO_NOT_RECEIVE_MSG)) != CText::npos)
	{
		demand_dg_t::event_handler_t _handler=0;
		if(aConf.MGetIfSet(demand_dg_t::HANDLER,_handler))
		{
			if(MRemoveDgParserFor(_handler))
			_code=E_STARUS_OK;
			else
			{
				LOG(ERROR)<<"Cannot remove handler "<<_handler;
				_code = E_STARUS_BAD_REQUEST;
			}
		}
		else
		{
			LOG(ERROR)<<"No handler.";
			_code = E_STARUS_BAD_REQUEST;
		}
	}
	else if((_pos = aPath.find(SNIFFER_STATE)) != CText::npos)
	{
		NSHARE::CText const _req(aPath, _pos);
		size_t const _number=aConf.MValue(SEQUENCE_NUMBER, 0);

		NSHARE::CConfig _data(SNIFFER_STATE);
		bool _is_partial_state=false;

		if((_pos = _req.find(demand_dgs_t::NAME)) != CText::npos && (_is_partial_state=true))
		{
			_data.MAdd(FDemands.MSerialize(false));
		}
		if((_pos = _req.find(SNIFFED_DATA)) != CText::npos && (_is_partial_state=true))
		{
			MPutSniffedDataFrom(_number, _data);
		}

		if(!_is_partial_state)
		{
			_data.MAdd(FDemands.MSerialize(false));
			MPutSniffedDataFrom(_number, _data);
		}
		VLOG(7)<<_data;
		VLOG(7)<<_data.MToJSON(true);
		MPutAsJson(_data, _response);
		_code=E_STARUS_OK;
	}
	else
	{
		LOG(ERROR)<<"Cannot parse post request.";
		_code = E_STARUS_BAD_REQUEST;
	}
	return _code;
}
bool CHttpIOManger::MSend(const data_t& aVal, descriptor_t const&)
{
	return false;
}
bool CHttpIOManger::MSend(const program_id_t& aVal, descriptor_t const&,
		const routing_t& aRoute, error_info_t const&)
{
	return false;
}



bool CHttpIOManger::MSend(const user_data_t& aVal, descriptor_t const&)
{
	std::vector<demand_dg_t::event_handler_t>::const_iterator _it =
			aVal.FDataId.FEventsList.begin(), _it_end(
			aVal.FDataId.FEventsList.end());

	for (; _it != _it_end; ++_it) //the data can be parsed by different parsers
	{
		serializator_t _ser;
		{
			CRAII<CMutex> _block(FParserMutex);
			serializators_t::const_iterator _jt = FSerializators.find(*_it);
			if (_jt != FSerializators.end())
				_ser = _jt->second;
		}


		data_fifo_t::value_type _conf;
		_conf.FData=NSHARE::CConfig(SNIFFED_DATA);
		_conf.FSize=aVal.FData.size();

		_conf.FData.MAdd(demand_dg_t::HANDLER, *_it);
		//_conf.second.MAdd(aVal.FDataId.MSerialize());

		_conf.FData.MAdd(user_data_t::PARSER, _ser.second);

		const NSHARE::CConfig _data = aVal.MSerialize(
		/*_ser.first,*/_ser.second);
		_conf.FData.MAdd(_data);
		VLOG(7)<<_data;
		VLOG(7)<<_data.MToJSON(true);

		MPutToFifo(_conf);
	}
	return true;
}
void CHttpIOManger::MPutToFifo(data_fifo_t::value_type & _new_data)
{
	CRAII<CMutex> _block(FSniffedMutex);
	const size_t _size=_new_data.FSize;
	for (;_size && (FBufferingBytes < (int)_size) && !FSniffedData.empty();)
	{
		FBufferingBytes += FSniffedData.front().FSize;
		FSniffedData.pop_front();
	}
	FBufferingBytes-=_new_data.FSize;

	_new_data.FSinffedNum=++FSinffedNum;
	_new_data.FData.MAdd(SEQUENCE_NUMBER, _new_data.FSinffedNum);

	FSniffedData.push_back(_new_data);
}
bool CHttpIOManger::MSend(const fail_send_t& aVal, descriptor_t const&,
		const routing_t& aRoute, error_info_t const&)
{
	data_fifo_t::value_type _conf;
	_conf.FData=NSHARE::CConfig(SNIFFED_DATA);
	_conf.FData.MAdd(aVal.MSerialize());
	_conf.FSize=0;
	MPutToFifo(_conf);

	return true;
}
bool CHttpIOManger::MSend(const kernel_infos_array_t& aVal, descriptor_t const&,
		const routing_t& aRoute, error_info_t const&)
{
	return false;
}
bool CHttpIOManger::MSend(const demand_dgs_for_t& aVal, descriptor_t const&,
		const routing_t& aRoute, error_info_t const&)
{
	return false;
}
void CHttpIOManger::MCloseImpl()
{
	//todo
	Fd = CDescriptors::INVALID;

}

void CHttpIOManger::MClose()
{
	MCloseImpl();
}
void CHttpIOManger::MClose(const descriptor_t& aFrom)
{
	MCloseImpl();
}
CHttpIOManger::descriptors_t CHttpIOManger::MGetDescriptors() const
{
	return CDescriptors::sMIsValid(Fd) ? descriptors_t(1, Fd) : descriptors_t();
}
bool CHttpIOManger::MIs(descriptor_t aVal) const
{
	if (!CDescriptors::sMIsValid(aVal))
		return false;
	return Fd == aVal;
}

NSHARE::CConfig CHttpIOManger::MSerialize() const
{
	return NSHARE::CConfig("http");
}

bool CHttpIOManger::MIsOpen() const
{
	return FTcpServiceSocket.MIsOpen();
}

NSHARE::eCBRval CHttpIOManger::sMConnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);
//	CTCPServer::client_t* _client =
//			reinterpret_cast<CTCPServer::client_t*>(aWhat);
//	reinterpret_cast<CKernelIOByTCP*>(aThis)->MConnect(_client);
	return E_CB_SAFE_IT;
}
NSHARE::eCBRval CHttpIOManger::sMDisconnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);
//	CTCPServer::client_t* _client =
//			reinterpret_cast<CTCPServer::client_t*>(aWhat);
//	CRAII<CMutex> _blocked(reinterpret_cast<CKernelIOByTCP*>(aThis)->FMutex);
//	reinterpret_cast<CKernelIOByTCP*>(aThis)->MDisconnectImpl(_client->FAddr);
	return E_CB_SAFE_IT;
}

bool CHttpIOManger::MReceive(demand_dg_t& _val, NSHARE::CText aParser)
{
	CRAII<CMutex> _block(FParserMutex);
	if (aParser.empty())
		aParser = _val.FProtocol;
	IExtParser* _ser = CParserFactory::sMGetInstance().MGetFactory(aParser);
	if (!_ser && _val.FProtocol==aParser)
		return false;
	unsigned _i = 0;
	unsigned const _size = FDemands.size();
	for (; _i != _size && !FDemands[_i].MIsEqual(_val); ++_i)
		;
	if (_i == _size)
	{
		LOG(INFO)<< "Add additional parser for channel '" << _val.FNameFrom << "' by "<< aParser<<"'";

		_val.FHandler=++FUniqueNumber;
		_val.FFlags = demand_dg_t::E_REGISTRATOR;
		FSerializators[_val.FHandler] = serializator_t(_val.FWhat, aParser);
		FDemands.push_back(_val);
		CKernelIo::sMGetInstance().MReceivedData(FDemands, Fd, routing_t(),
				error_info_t());
		return true;
	}
	else
	{
		LOG(ERROR) << "Replace handler ";
		//FSerializators[_val.FHandler].second=aParser;
		return false;
	}

}

int CHttpIOManger::MSettingDgParserFor(const NSHARE::CText& aReq,
		NSHARE::CText FProtocolName, required_header_t aNumber,
		NSHARE::CText aParser)
{
	VLOG(2) << "Setting for " << aReq << " parser";

	if (FProtocolName.empty())
		FProtocolName = RAW_PROTOCOL_NAME;

	FProtocolName.MToLowerCase();

	demand_dgs_t::value_type _val;
	_val.FNameFrom = NSHARE::CRegistration(aReq);
	_val.FProtocol = FProtocolName;
	_val.FWhat = aNumber;

	VLOG(2) << "New demand:" << _val;
	MReceive(_val, aParser);
	return _val.FHandler;
}
std::pair<demand_dg_t, bool> CHttpIOManger::MGetDemand(uint32_t aNumber)
{
	CRAII<CMutex> _block(FParserMutex);
	unsigned _i = 0;
	unsigned const _size = FDemands.size();
	for (; _i != _size && (FDemands[_i].FHandler != aNumber); ++_i)
		;
	if (_i == _size)
	{
		return std::make_pair(demand_dg_t(), false);
	}
	return std::make_pair(FDemands[_i], true);
}
bool CHttpIOManger::MRemoveDgParserFor(uint32_t aNumber)
{
	VLOG(2) << "Remove parser for " << aNumber;

	{
		CRAII<CMutex> _block(FParserMutex);

		unsigned _i = 0;
		unsigned const _size = FDemands.size();
		for (; _i != _size && (FDemands[_i].FHandler != aNumber); ++_i)
			VLOG(5) << FDemands[_i].FHandler << " != " << aNumber;
		if (_i == _size)
		{
			return false;
		}
		FSerializators.erase(aNumber);
		FDemands.erase(FDemands.begin() + _i);
		CKernelIo::sMGetInstance().MReceivedData(FDemands, Fd, routing_t(),
				error_info_t());
	}
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(FSniffedMutex);
		data_fifo_t::iterator _it = FSniffedData.begin(), _it_end =
				FSniffedData.end();
		for (; _it != _it_end;)
		{
			if (_it->FData.MValue(demand_dg_t::HANDLER,
					demand_dg_t::NO_HANDLER) == aNumber)
				_it = FSniffedData.erase(_it);
			else
				++_it;
		}
	}
	return true;
}
} /* namespace NSHARE */
