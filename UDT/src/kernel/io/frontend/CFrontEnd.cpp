// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CFrontEnd.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/Socket/diagnostic_io_t.h>
#include <SHARE/Socket/ISocket.h>
#include <SHARE/Socket/socket_parser.h>
#include <UDT/CParserFactory.h>
#include <core/kernel_type.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <services/CInfoService.h>

#include <io/CKernelIo.h>
#include "CExternalChannel.h"
#include "CFrontEnd.h"

namespace NUDT
{
using namespace NSHARE;
const NSHARE::CText CExternalChannel::CFrontEnd::NAME = "frontend";
const NSHARE::CText CExternalChannel::CFrontEnd::FRONTEND_NAME = "name";
const NSHARE::CText CExternalChannel::CFrontEnd::DEMAND = "demand";
const NSHARE::CText CExternalChannel::CFrontEnd::RECV_PROTOCOL = "rpl";
const NSHARE::CText CExternalChannel::CFrontEnd::REPEAT_TIME = "repeat_time";

#define IMPL CExternalChannel::CFrontEnd

CExternalChannel::CFrontEnd::CFrontEnd(NSHARE::CConfig const& aConf,
		CExternalChannel& aThis) :
		FThis(aThis), //
		FConfig(aConf), //
		FProgId(get_my_id()), //
		FRepeatTime(10000),//
		FEndian(NSHARE::E_SHARE_ENDIAN)
{
	FProgId.FId.FUuid = NSHARE::get_uuid(FProgId.FId.FName);
	FProgId.FType = E_CONSUMER;
	Fd = -1;
	FSocket = NULL;
	FPacketNumber = 0;
	MInit(aConf);
}

CExternalChannel::CFrontEnd::~CFrontEnd()
{
	if (FSocket)
	{
		NSHARE::ISocket* _tmp = FSocket;
		FSocket = NULL;
		_tmp->MClose();

		delete _tmp;
	}
}

void CExternalChannel::CFrontEnd::MParseDemands(const NSHARE::CConfig& aConf)
{
	const ConfigSet _dems = aConf.MChildren(DEMAND);
	LOG_IF(WARNING,_dems.empty()) << "No demands ";
	if (!_dems.empty())
	{
		ConfigSet::const_iterator _it = _dems.begin();
		for (; _it != _dems.end(); ++_it)
		{
			VLOG(2) << "dem info " << *_it;
			demand_dg_t _dem(*_it);
			_dem.FEventHandler = 0;
			LOG_IF(ERROR,!_dem.MIsValid()) << "Cannot create demands from "
													<< _it->MToJSON(true);
			if (_dem.MIsValid())
			{

				LOG(INFO)<<"Demand "<<_dem;
				FDemands.push_back(_dem);
				FSendProtocol.insert(_dem.FProtocol);
			}
		}
	}
	if (aConf.MIsChild(RECV_PROTOCOL))
	{

		VLOG(2) << "There is exit protocol";
		FReceiveProtocol=aConf.MChild(RECV_PROTOCOL).MValue();
		LOG_IF(FATAL,!FReceiveProtocol.empty() && !CParserFactory::sMGetInstance().MIsFactoryPresent(FReceiveProtocol))
																						<< "The protocol handler is not exist."<<FReceiveProtocol;
	}
}

void CExternalChannel::CFrontEnd::MInit(const NSHARE::CConfig& aConf)
{
	VLOG(2) << "Init Front ends";
	std::vector<ISocket*> _socks = get_sockets(aConf);
	LOG_IF(ERROR,_socks.empty()) << "No sockets " << aConf.MToJSON(true);
	if (!_socks.empty())
	{
		FSocket = _socks.front();
		LOG_IF(WARNING,_socks.size()>1) << FSocket->MSerialize().MToJSON(true)
												<< ". The other sockets will be ignored. "
												<< aConf.MToJSON(true);
		if (_socks.size() > 1)
		{
			for (unsigned i = 1; i < _socks.size(); ++i)
			{
				delete _socks[i];
				_socks[i] = NULL;
			}
		}

		FSplit.FType.MSetFlag(split_info::CAN_NOT_SPLIT, true);
		NSHARE::CConfig const& _conf = aConf.MChild(split_info::NAME);
		if (!_conf.MIsEmpty())
		{
			VLOG(2) << "Update splite.";
			FSplit = split_info(_conf);
		}
		CText _name;
		aConf.MGetIfSet(FRONTEND_NAME, _name);
		aConf.MGetIfSet(REPEAT_TIME, FRepeatTime);
		unsigned _val=0;
		if(aConf.MGetIfSet<unsigned>(user_data_info_t::KEY_DATA_ENDIAN, _val))
			FEndian=(NSHARE::eEndian)_val;
		LOG_IF(ERROR,_name.empty()) << "No front end name "
											<< aConf.MToJSON(true);
		if (!_name.empty())
		{
			FProgId.FId.FName = _name;
		}

		MParseDemands(aConf);
	}
	//FProgId
}

void IMPL::MOpen()
{
	VLOG_IF(1,!FSocket)<<"The front end is not opened as not socket.";
	if(FSocket)
	{
		NSHARE::operation_t _op(CFrontEnd::sMReceiver, this, NSHARE::operation_t::IO);
		CDataObject::sMGetInstance().MPutOperation(_op);
	}
}
NSHARE::eCBRval IMPL::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CFrontEnd*>(aData)->MReceiver();
	return E_CB_REMOVE;
}
void IMPL::MReceiverLoop()
{
	VLOG(2)<<"Receive data staring";
	for (;FSocket&&FSocket->MIsOpen(); )
	{
		ISocket::data_t _data;
		//ISocket::recvs_from_t _from;
		if (FSocket->MReceiveData( &_data, -1.0/*,&_from*/) > 0)
		{
			LOG_IF(WARNING,_data.empty())<<"Receive empty data";
			if(!_data.empty())
			{
				MReceivedData(_data);
			}
		}
	}
	VLOG(2)<<"Receive data finished";
}
void IMPL::MConnected()
{
	CHECK(!CDescriptors::sMIsValid(Fd));
	Fd = CDescriptors::sMGetInstance().MCreate();

	descriptor_info_t _info;

	_info.FConnectTime=NSHARE::get_unix_time();
	_info.FTypeLink = NAME;
	_info.FProgramm = FProgId;
	_info.FInfo=FSocket->MSettings();

	CDescriptors::sMGetInstance().MOpen(Fd,_info);
	VLOG(2) << "Connected " << NSHARE::get_unix_time();
	split_info _split;
	_split.FType.MSetFlag(split_info::CAN_NOT_SPLIT,true);
	_split.FMaxSize=FSocket->MMaxPacketSize();
	FThis.MAddChannel(this,Fd,_split);

	if(!FDemands.empty())
	{
		VLOG(2)<<"There are demands.";
		CKernelIo::sMGetInstance().MReceivedData(FDemands,Fd,
				routing_t(), error_info_t());
	}
}
void IMPL::MDisconnected()
{
	CHECK(CDescriptors::sMIsValid(Fd));
	FThis.MRemoveChannel(this,Fd);
	CDescriptors::sMGetInstance().MClose(Fd);
	Fd=-1;
}
void IMPL::MReceiver()
{
	VLOG(2) << "Async receive";

	for(;FSocket;NSHARE::usleep(FRepeatTime))
	{
		if(FSocket->MIsOpen())
		{
			MConnected();
			MReceiverLoop();
			MDisconnected();
			LOG(INFO) << "Port is closed";
		}

	}
	VLOG(1) << "Socket closed";
}


void IMPL::MReceivedData(NSHARE::CBuffer& aData)
{
	VLOG(2) << "Receive :" << aData.size();
	LOG_IF(INFO, aData.empty()) << "Empty Data.";
	if(aData.empty())return;

	LOG_IF(INFO,FReceiveProtocol.empty())<<"No protocol. Using raw protocol ...";

	user_data_t _user;
	_user.FDataId.FProtocol=FReceiveProtocol;
	_user.FDataId.FPacketNumber=++FPacketNumber;
	_user.FDataId.FRouting.FFrom =FProgId.FId;
	aData.MMoveTo(_user.FData);
	bool const _is_raw=FReceiveProtocol.empty() || (FReceiveProtocol==RAW_PROTOCOL_NAME);
	if(_is_raw)
		_user.FDataId.FWhat.FNumber=1;

	CKernelIo::sMGetInstance().MReceivedData(_user,Fd);


}
bool IMPL::MSend(const data_t& aVal)
{
	return FSocket!=NULL && FSocket->MSend(aVal).MIs();
}

bool IMPL::MSend(const user_data_t& aVal)
{
	VLOG(2)<<"Send "<<aVal.FDataId;
	LOG_IF(ERROR,FSendProtocol.find(aVal.FDataId.FProtocol)==FSendProtocol.end())<<"Send protocol by unknown protocol "<<aVal.FDataId.FProtocol;

	if(!FSocket || !FSocket->MIsOpen())
	{
		return false;
	}
	CHECK(!aVal.FDataId.FSplit.MIsSplited());
	const size_t _max_size=FSplit.FMaxSize;
	const size_t _size=aVal.FData.size();
	NSHARE::sent_state_t _error(sent_state_t::E_SENDED,0);

	if(FSplit.FType.MGetFlag(split_info::LIMITED) && _max_size<_size)
	{
		const uint8_t*_begin=(const uint8_t*)aVal.FData.ptr_const();
		const uint8_t*_end=_begin + _size;

		size_t  _send_size=0;
		for(;_begin!=_end && //
				_error.MIs();_begin+=_send_size)
		{
			_send_size=std::min(_max_size,(size_t)(_end-_begin));
			do
			{
				_error=FSocket->MSend(_begin,_send_size);
			}while(_error.MIs(sent_state_t::E_AGAIN) && NSHARE::usleep(FRepeatTime));
		}
	}
	else
	{
		do
		{
			_error=FSocket->MSend(aVal.FData);
		}while(_error.MIs(sent_state_t::E_AGAIN) && NSHARE::usleep(FRepeatTime));
	}
	if(!_error.MIs())
	{
		LOG(ERROR)<<"Cannot Send data From "<<aVal.FDataId<<" as "<<_error;
		return false;
	}
	else
		return true;

}
void IMPL::MClose()
{
	VLOG(2) << "Close client.";
	if(FSocket)
	FSocket->MClose();
}
NSHARE::CConfig IMPL::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	if(FSocket)
	{
		_conf.MAdd(FSocket->MSerialize());
	}
	_conf.MAdd("conf",FConfig);
	_conf.MAdd("desc",Fd);
	_conf.MAdd("Again",FRepeatTime);
	_conf.MAdd(/*DEMAND,*/FDemands.MSerialize());
	_conf.MAdd(RECV_PROTOCOL,FReceiveProtocol);
	_conf.MAdd(/*"info",*/FProgId.MSerialize());

	return _conf;
}
NSHARE::CConfig const& IMPL::MBufSettingFor(
		NSHARE::CConfig const& aFrom) const
{
	return aFrom;			//todo Обдумать
}} /* namespace NUDT */
