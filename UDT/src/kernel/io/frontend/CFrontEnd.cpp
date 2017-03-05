/*
 * CFrontEnd.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 26.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket/diagnostic_io_t.h>
#include <Socket/ISocket.h>
#include <Socket/socket_parser.h>
#include <CParserFactory.h>
#include <core/kernel_type.h>
#include <core/IState.h>
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
const NSHARE::CText CExternalChannel::CFrontEnd::EXIT_PROTOCOL = "expl";
#define IMPL CExternalChannel::CFrontEnd

CExternalChannel::CFrontEnd::CFrontEnd(NSHARE::CConfig const& aConf,
		CExternalChannel& aThis) :
		FThis(aThis), //
		FConfig(aConf), //
		FProgId(get_my_id()) //
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
			_dem.FHandler = 0;
			LOG_IF(ERROR,!_dem.MIsValid()) << "Cannot create demands from "
													<< _it->MToJSON(true);
			if (_dem.MIsValid())
			{

				if (_it->MIsChild(EXIT_PROTOCOL))
				{

					VLOG(2) << "There is exit protocol";
					LOG_IF(FATAL,_it->MChild(EXIT_PROTOCOL).MValue()!=RAW_PROTOCOL_NAME)
																									<< "The Method is not implemented."; //todo
					//NSHARE::CText _exit_protocol(RAW_PROTOCOL_NAME);
				}
				LOG(INFO)<<"Demand "<<_dem;
				FDemands.push_back(_dem);
				FSendProtocol.insert(_dem.FProtocol);
			}
		}
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
				delete _socks[i];
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
int IMPL::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CFrontEnd*>(aData)->MReceiver();
	return 0;
}
void IMPL::MReceiverLoop()
{
	VLOG(2)<<"Receive data staring";
	ISocket::data_t _data;					//todo allocate to def memory
	for (;FSocket&&FSocket->MIsOpen(); )
	{
		_data.clear();
		//ISocket::recvs_from_t _from;
		if (FSocket->MReceiveData( &_data, -1.0/*,&_from*/) > 0)
		{
			LOG_IF(WARNING,_data.empty())<<"Receive empty data";
			if(!_data.empty())
			{
				MReceivedData(_data.cbegin(),_data.cend());	//todo optimize remove copy operation
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

	FThis.MAddChannel(this,Fd,FSplit);

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
	FBuf.clear();
	FDataSequence.clear();
}
void IMPL::MReceiver()
{
	VLOG(2) << "Async receive";

	for(;FSocket;NSHARE::sleep(1)/*todo to config*/) //todo still work
	{
		if(FSocket->MIsOpen())
		{
			MConnected();
			MReceiverLoop();
			MDisconnected();
		}
		LOG(INFO) << "Port is closed";

	}
	VLOG(1) << "Socket closed";
}
void IMPL::MRawReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd)
{
	VLOG(2)<<"Receive data by raw protocol.";
	user_data_t _user;
	_user.FDataId.FProtocol=RAW_PROTOCOL_NAME;
	_user.FDataId.FPacketNumber=++FPacketNumber;
	_user.FDataId.FRouting.FFrom =FProgId.FId;
	NSHARE::CBuffer _data(CDataObject::sMGetInstance().MDefAllocater(),aBegin,aEnd);
	_data.MMoveTo(_user.FData);

	CKernelIo::sMGetInstance().MReceivedData(_user,Fd);
}

bool IMPL::MReceiveByProtocol(
		data_t::const_iterator aBegin, data_t::const_iterator aEnd)
{
	IExtParser* _p = CParserFactory::sMGetInstance().MGetFactory(FProtocol);
	if(!_p)
	{
		LOG(ERROR)<<"No protocol for "<<FProtocol;
		return false;
	}

	IExtParser::result_t _result = _p->MParserData(
			(const uint8_t*) (aBegin.base()), (const uint8_t*) (aEnd.base()));
	VLOG(1) << "Founded " << _result.size() << " dg.";
	{
		IExtParser::result_t::const_iterator _jt = _result.begin();
		for (; _jt != _result.end(); ++_jt)
		{
			user_data_t _user;
			_user.FDataId.FProtocol = FProtocol;
			_user.FDataId.FPacketNumber = ++FPacketNumber;
			_user.FDataId.FRouting.FFrom = FProgId.FId;
			NSHARE::CBuffer _data(CDataObject::sMGetInstance().MDefAllocater(),
					_jt->FBegin, _jt->FEnd);
			_data.MMoveTo(_user.FData);
			CKernelIo::sMGetInstance().MReceivedData(_user, Fd);
		}
		if (*aEnd.base() != *_result.back().FEnd)
		{
			VLOG(2) << "Buffer Size:" << FBuf.size();
			NSHARE::CBuffer::const_iterator _end(
					(NSHARE::CBuffer::const_pointer) _result.back().FEnd);
			if (FBuf.empty())
			FBuf.insert(FBuf.end(), _end, aEnd);
			else
			FBuf.erase(FBuf.begin(), FBuf.begin() + (_end - FBuf.cbegin()));
		}
		else if (!FBuf.empty())
		FBuf.clear();
	}
	return true;
}

void IMPL::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd)
{
	VLOG(2) << "Receive :" << aEnd-aBegin;
	LOG_IF(INFO, aEnd==aBegin) << "Empty Data.";
	if(aEnd==aBegin)return;

	if (!FBuf.empty())
	{
		FBuf.insert(FBuf.end(), aBegin, aEnd);
		aBegin = FBuf.cbegin();
		aEnd = FBuf.cend();
	}
	LOG_IF(INFO,FProtocol.empty())<<"No protocol. Using raw protocol ...";

	if(FProtocol.empty()|| (FProtocol==RAW_PROTOCOL_NAME) || !MReceiveByProtocol(aBegin, aEnd))
	{
		MRawReceivedData(aBegin,aEnd);
	}

}
bool IMPL::MSend(const data_t& aVal)
{
	return FSocket!=NULL && FSocket->MSend(aVal).FError==ISocket::E_SENDED; //todo handle EAGAIN
}
bool IMPL::MSendPacketsFromAnotherCustomer()
{
	VLOG(2)<<"Send previous packets.";
	//todo
	for(;!FDataSequence.empty();)
	{
		LOG(FATAL)<<"The Method is not implemented.";
//		VLOG(2)<<"Send previous packet.";
//		user_data_t const _d=FDataSequence.front();
//		FDataSequence.pop_front();
//		if(!MSend(_d))
//		{
//			FDataSequence.clear();//todo return error as fail_sent_packet
//			return false;
//		}
	}
	return true;
}

bool CExternalChannel::CFrontEnd::MSendSplitedPacket(const user_data_t& aVal)
{
	if (FLastSplitedPacket.MGetConst().FRouting.FFrom
			== aVal.FDataId.FRouting.FFrom)
	{
		if (FLastSplitedPacket.MGetConst().FPacketNumber
				!= aVal.FDataId.FPacketNumber)
		{
			LOG(ERROR)<<" Packet lost for "<<FLastSplitedPacket.MGetConst();
			FLastSplitedPacket.MUnSet();

			MSendPacketsFromAnotherCustomer();

			if(aVal.FDataId.FSplit.MIsSplited()) //fixme call MSend again
			return MSendFirstSplitedPacket(aVal);
		}
		else if(aVal.FDataId.FSplit.FIsLast)
		{
			VLOG(2)<<"Send all packet ";
			FLastSplitedPacket.MUnSet();
			bool _is=MSend(aVal.FData);
			MSendPacketsFromAnotherCustomer();
			return _is;
		}
		else
		FLastSplitedPacket=aVal.FDataId;
	}
	else
	{
		if(!CInfoService::sMGetInstance().MIsVertex(FLastSplitedPacket.MGetConst().FRouting.FFrom.FUuid))
		{
			LOG(ERROR)<<"Not all data sent last packet "<<FLastSplitedPacket.MGetConst();
			FLastSplitedPacket.MUnSet();
			MSendPacketsFromAnotherCustomer();

			if(aVal.FDataId.FSplit.MIsSplited()) return MSendFirstSplitedPacket(aVal); //fixme call MSend again
		}
		else
		{
			VLOG(2)<<"Put to sequence buffer.";
			FDataSequence.push_back(aVal);
			return true;
		}
	}

	return MSend(aVal.FData);
}

bool IMPL::MSendFirstSplitedPacket(
		const user_data_t& aVal)
{
	if (aVal.FDataId.FSplit.FCounter != 1)
	{
		LOG(ERROR)<<"Invalid packet counter. "<<aVal.FDataId;
		return false;

	}
	VLOG(2) << "Now receive split packet.";
	FLastSplitedPacket = aVal.FDataId;
	return MSend(aVal.FData);
}

bool IMPL::MSend(const user_data_t& aVal)
{
	VLOG(2)<<"Send "<<aVal.FDataId;
	LOG_IF(ERROR,FSendProtocol.find(aVal.FDataId.FProtocol)==FSendProtocol.end())<<"Send protocol by unknown protocol "<<aVal.FDataId.FProtocol;

	if(!FSocket || !FSocket->MIsOpen())
	{
		FLastSplitedPacket.MUnSet();
		return false;
	}

	if(FSplit.FType.MGetFlag(split_info::CAN_NOT_SPLIT))
	{
		DCHECK(!aVal.FDataId.FSplit.MIsSplited());
		if(aVal.FDataId.FSplit.MIsSplited())
		return false;
	}
	else if(FSplit.FType.MGetFlag(split_info::LIMITED))
	{
		if(FLastSplitedPacket.MIs()) return MSendSplitedPacket(aVal);
		else if( aVal.FDataId.FSplit.MIsSplited()) return MSendFirstSplitedPacket(aVal);
		else if(!FDataSequence.empty())
		{
			if(!MSendPacketsFromAnotherCustomer())
			return false;
		}
		else
		VLOG(2)<<"Can send directly";
	}

	return MSend(aVal.FData);			//todo converting protocol
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
	_conf.MAdd(/*DEMAND,*/FDemands.MSerialize());
	_conf.MAdd(EXIT_PROTOCOL,FProtocol);
	_conf.MAdd(/*"info",*/FProgId.MSerialize());

	return _conf;
}
NSHARE::CConfig const& IMPL::MBufSettingFor(
		NSHARE::CConfig const& aFrom) const
{
	return aFrom;			//todo Обдумать
}} /* namespace NUDT */
