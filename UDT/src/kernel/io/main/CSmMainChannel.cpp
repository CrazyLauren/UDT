// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSmMainChannel.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share_socket.h>
#include <string.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include <parser_in_protocol.h>
#include <shared_types.h>
#include <sm_shared.h>

#include <core/kernel_type.h>
#include <core/CDataObject.h>
#include <core/CConfigure.h>
#include <core/CDescriptors.h>
#include <io/IIOManager.h>
#include <io/ILink.h>


#include "CSmMainChannel.h"

using namespace NSHARE;
namespace NUDT
{
const NSHARE::CText CSmMainChannel::THREAD_PRIORITY = "priority";
const NSHARE::CText CSmMainChannel::NAME = "sm";
const NSHARE::CText CSmMainChannel::SM_REMOVE="remove";
const NSHARE::CText CSmMainChannel::SERVER_NAME = "name";
const NSHARE::CText CSmMainChannel::SERVER_SIZE = "size";
const NSHARE::CText CSmMainChannel::PATH_HASH_TO_NAME = "path_hash_to_name";
const NSHARE::CText CSmMainChannel::USING_AS_DEF="as_def_alloc";
const NSHARE::CText CSmMainChannel::RESERV="reserv";

CSmMainChannel::CSmMainChannel() :
		IMainChannel(NAME),//
		FReserv(1024),//
		FSize(0),//
		FIsUsingAsDef(false)
{
	FName.MMakeRandom(10);
}



bool CSmMainChannel::MStart()
{
	VLOG(2) << "Initialize sm server";
	CConfig _main_settings = CConfigure::sMGetInstance().MGet().MChild(IMainChannel::CONFIGURE_NAME);
	if (_main_settings.MIsEmpty())
	{
		LOG(ERROR) << "Main channel settings is not exist";
		return false;
	}
	CConfig _settings = _main_settings.MChild(NAME);
	if (_settings.MIsEmpty())
	{
		LOG(WARNING)<<"The Shared memory is not initialized as no configure.";
		return false;
	}

	if (!_settings.MGetIfSet(SERVER_NAME, FName))
	{
		LOG(FATAL)<<"Cannot open sm server as the name is not set. Value: "<<SERVER_NAME;
	}
	bool _is_hash=false;
	if (_settings.MGetIfSet(PATH_HASH_TO_NAME, _is_hash))
	{
		std::stringstream _out;
		_out << get_my_id().FPath.MGetHash();
		FName+=_out.str();
		VLOG(2)<<"Name  sm memory with hash "<<FName;
//		LOG(FATAL)<<"Cannot open sm server as the name is not set. Value: "<<SERVER_NAME;
	}

	bool _is=false;
	if(_settings.MGetIfSet(SM_REMOVE, _is) && _is)
	{
		VLOG(2)<<"Remove memory "<<FName;
		NSHARE::CSharedMemoryServer::sMRemove(FName);
	}
	if (!_settings.MGetIfSet(SERVER_SIZE, FSize))
	{
		LOG(FATAL)<<"Cannot open sm server as the size is not set. Value: "<<SERVER_SIZE;
	}
	if (_settings.MGetIfSet(RESERV, FReserv))
	{
		VLOG(2)<<"New reserv == "<<FReserv;
	}

	_settings.MGetIfSet(USING_AS_DEF, FIsUsingAsDef);

	return MOpenIfNeed();
}
bool CSmMainChannel::MOpenIfNeed()
{
	if (!FSmServer.MIsOpen())
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FOpenMutex);
		if (!FSmServer.MIsOpen() && FSmServer.MOpen(FName, FSize, FReserv))
		{
			NSHARE::operation_t _op(CSmMainChannel::sMReceiver, this,
					NSHARE::operation_t::IO);
			CDataObject::sMGetInstance().MPutOperation(_op);
			LOG_IF(FATAL,!FSmServer.MIsOpen())<< "The sm server is not opened";
			if (FIsUsingAsDef)
			{
				CDataObject::sMGetInstance().MSetDefAllocater(MAllocater());
			}
		}
	}
	return FSmServer.MIsOpen();
}
CSmMainChannel::~CSmMainChannel()
{
	FSmServer.MClose();
}
template<>
void CSmMainChannel::MFill<main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create main channel param DG";
	main_ch_param_t _param;
	_param.FType = E_MAIN_CHANNEL_SM;
	_param.FValue.MSet("path", FSmServer.MSharedName());	

	serialize<main_channel_param_t, main_ch_param_t>(aTo, _param, routing_t(), error_info_t());
}
template<>
void CSmMainChannel::MFill<request_main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create request main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(request_main_channel_param_t));
	request_main_channel_param_t * _p =
			new ((data_t::value_type*) aTo->ptr() + _befor) request_main_channel_param_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_SM);
	fill_dg_head(_p, sizeof(request_main_channel_param_t), get_my_id());
	VLOG(2) << (*_p);
}
template<>
void CSmMainChannel::MFill<close_main_channel_t>(data_t* aTo)
{
	VLOG(2) << "Close  main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(close_main_channel_t));
	close_main_channel_t * _p =
			new ((data_t::value_type*) aTo->ptr() + _befor) close_main_channel_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_SM);
	fill_dg_head(_p, sizeof(close_main_channel_t), get_my_id());
	VLOG(2) << (*_p);
}

bool CSmMainChannel::MSendSetting(ILink* aHandler, descriptor_t aFor)
{
	data_t _buf;
	MFill<main_channel_param_t>(&_buf);
	MFill<request_main_channel_param_t>(&_buf);
	return aHandler->MSend(_buf);
}

bool CSmMainChannel::MIsNew(descriptor_t const& aFor) const
{
	r_access _access = FData.MGetRAccess();
	for (id_to_ip_t::const_iterator _it = _access->FNew.begin();
			_it != _access->FNew.end(); ++_it)
	{
		if (CDescriptors::sMIsValid(_it->first))
		{
			DCHECK_NOTNULL(_it->second.FHandler);
			if (_it->second.FHandler && _it->first == aFor)
				return true;
		}
	}
	return false;
}

bool CSmMainChannel::MOpen(ILink* aHandler,program_id_t const&,NSHARE::net_address const& aFor)
{
	CHECK_NOTNULL(aHandler);

	if (!MOpenIfNeed())
	{
		LOG(DFATAL) << "Sm channel is not opened.";
		return false;
	}
	split_info _info=aHandler->MLimits();
	_info.FType.MSetFlag(split_info::CAN_NOT_SPLIT,true);

	size_t const _max = FSmServer.MGetAllocator()->MMaxSize();

	_info.FMaxSize =
			(_info.FMaxSize > 0 && _info.FMaxSize < _max) || _max == 0 ?
					_info.FMaxSize : _max;
	aHandler->MSetLimits(_info);
	//aHandler->MSetLimits()
	LOG_IF(ERROR,MIsOpen(aHandler->MGetID())) << "Main channel for "
														<< aHandler->MGetID()
														<< "has been opened already";

	bool _is = MIsNew(aHandler->MGetID());
	LOG_IF(ERROR,_is) << "Retrying open main channel for "
								<< aHandler->MGetID();

	if (!_is)
	{
		w_access _access = FData.MGetWAccess();

		param_t _param;
		_param.FHandler = aHandler;
		_param.FState = param_t::E_REQUEST_PARAM; //in MSendSetting The request will be sent;
		_access->FNew[aHandler->MGetID()] = _param;
	}
	return MSendSetting(aHandler, aHandler->MGetID());
}
bool CSmMainChannel::MIsOpen(descriptor_t aFor) const
{
	r_access _access = FData.MGetRAccess();
	id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
	return _it != _access->FIdToIP.end()
			&& FSmServer.MIsClient(_it->second.FAddr);
}
bool CSmMainChannel::MClose(descriptor_t aFor)
{
	VLOG(2) << "Close " << aFor;
	param_t _param;
	bool _is = false;
	{
		w_access _access = FData.MGetWAccess();
		id_to_ip_t::iterator _it = _access->FIdToIP.find(aFor);
		VLOG_IF(1,_it == _access->FIdToIP.end()) << "Main channel for " << aFor
														<< " is not exist.";
		_is = _it != _access->FIdToIP.end();
		if (_is)
		{
			//MThreadSafetyAssign(_param.FIo,_it->second.FIo);
			{
				CRAII<CMutex> _block(FMutex);
				_param = _it->second;
			}
			_param.FState = param_t::E_CLOSING;

			MThreadSafetyReset(_it->second.FIo);
			_access->FIpToId.erase(_param.FAddr);
			_access->FIdToIP.erase(aFor);
		}
	}
	if (_is)
	{
		VLOG(2) << "Send and close " << aFor;
//		data_t _buf;
//		MFill<close_main_channel_t>(&_buf);
//		if (FSmServer.MIsClient(_param.FAddr) && _param.FHandler)
//			_param.FHandler->MSend(_buf);

		FSmServer.MClose(_param.FAddr);
		return true;
	}
	else
	{
		w_access _access = FData.MGetWAccess();
		for (id_to_ip_t::iterator _it = _access->FNew.begin();
				_it != _access->FNew.end(); ++_it)
		{
			if (_it->first == aFor)
			{
				_access->FNew.erase(_it);
				break;
			}
		}
	}
	return false;
}
NSHARE::CBuffer CSmMainChannel::MGetNewBuf(std::size_t aSize,NSHARE::eAllocatorType aType) const
{
	if (FSmServer.MIsOpen())
		return FSmServer.MAllocate(aSize,0,aType);
	return NSHARE::CBuffer(aSize);
}
NSHARE::CBuffer CSmMainChannel::MGetValidBuffer(NSHARE::CBuffer const& aVal)
{
	NSHARE::CBuffer _data_buf;
	if (aVal.MIsAllocatorEqual(FSmServer.MGetAllocator()))
	{
		VLOG(2) << "It's the recommended  allocator";
		_data_buf = aVal;
	}
	else
	{
		LOG(WARNING)<< "It's not the recommended  allocator";
		_data_buf = MGetNewBuf(aVal.size(),NSHARE::ALLOCATE_FROM_COMMON);
		if (_data_buf.size() != aVal.size())
		{
			VLOG(2) << "Cannot allocate buffer.";
			_data_buf.clear();
		}else
		_data_buf.deep_copy(aVal);
	}
	return _data_buf;
}
bool CSmMainChannel::MSendImpl(NSHARE::intrusive_ptr<sm_io_t>& _io,
		const user_data_t& aVal, const NSHARE::shared_identify_t & _addr)
{
	CHECK_NOTNULL(_io.MGet());
	NSHARE::CBuffer _data_buf(MGetValidBuffer(aVal.FData));
	//CRAII<CMutex> _gurante_sending_sequence(_io->FSendMutex);
	if (_data_buf.empty())
	{
		LOG(DFATAL)<<"Cannot allocate  the buffer for "<<_addr;
		return false;
	}
	size_t const _size=get_full_size(aVal.FDataId);

//	if (_size >= _data_buf.capacity())
//	{
	//send data in two part
	//The first is data info
	//The second is data
	NSHARE::CBuffer _buf(MGetNewBuf(_size,NSHARE::ALLOCATE_FROM_COMMON_AND_RESERV));
	for (HANG_INIT;_buf.empty();HANG_CHECK)
	{
		NSHARE::CThread::sMYield();
		MGetNewBuf(_size,NSHARE::ALLOCATE_FROM_COMMON_AND_RESERV).MMoveTo(_buf);
	}
	fill_header(_buf.ptr(), aVal.FDataId, _data_buf.offset());

	unsigned &_counter = _io->FTo;
	bool _is_overload = false;
	for (bool _is_info_sended = false; FSmServer.MIsOpen();)
	{
		flag_mask_t _mask;
		_mask.FData.FCounter = _counter;
		_mask.FData.FType = _is_info_sended ? E_SM_DATA : E_SM_INFO;
		NSHARE::CSharedMemoryServer::eSendState _state =
				_is_info_sended ?
						FSmServer.MSend(_addr, _data_buf, false, _mask.FMask) :
						FSmServer.MSend(_addr, _buf, false, _mask.FMask);

		switch (_state)
		{
		case sent_state_t::E_SENDED:
		{
			VLOG(4) << "Sent counter "<<_mask.FData.FCounter;
			++_mask.FData.FCounter;
			_counter = _mask.FData.FCounter;
			if (_is_info_sended)
			{
				VLOG(0) << aVal.FData.size()
									<< " bytes sent successfully Counter="
									<< _mask.FData.FCounter;
				_io->FSendBytes += (unsigned)_data_buf.size();
				return true;
			}
			else
				_is_info_sended = true;

			break;
		}
		case sent_state_t::E_ERROR:
		{
			VLOG(1) << " Send error Counter=" << _mask.FData.FCounter;
			++_io->FSendError;
			return false;
			break;
		}
		case sent_state_t::E_AGAIN:
		{
			if (!_is_overload)
			{
				++_io->FOverloades;
			}
			_is_overload = true;
			VLOG(1) << "Try send again." << _mask.FData.FCounter;
			NSHARE::CThread::sMYield();
			break;
		}
		default:
			CHECK(false);
			break;
		} //
	}
//	}else
//	{
//		//send info and data by one buffer
//
	//bug fix - abstraction problem: when The  buffer is changing,
	//it's detached as there are more than one holder
//	}
	return false;
}
bool CSmMainChannel::MSend(user_data_t const& aVal, descriptor_t aFor)
{
	CHECK(!aVal.FData.MIsDetached());

	NSHARE::smart_field_t<NSHARE::shared_identify_t> _addr;
	NSHARE::intrusive_ptr<sm_io_t> _io;
	{
		r_access _access = FData.MGetRAccess();
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		LOG_IF(DFATAL,_it == _access->FIdToIP.end()) << "Main channel for "
																<< aFor
																<< " is not exist.";
		if (_it != _access->FIdToIP.end())
		{
			LOG_IF(ERROR,_it->second.FState!=param_t::E_CONNECTED)
																			<< " Ivalid state "
																			<< _it->second.FState;
			_addr = _it->second.FAddr;
			MThreadSafetyAssign(_io, _it->second.FIo);
		}
	}
	if (!_addr.MIs())
		return false;

	bool _is = MSendImpl(_io, aVal, _addr.MGetConst());

	MThreadSafetyReset(_io);
	return _is;
}
bool CSmMainChannel::MSend(data_t const& aVal, descriptor_t aFor)
{
	VLOG(2) << "Send data to " << aFor << " by main channel";

	NSHARE::smart_field_t<NSHARE::shared_identify_t> _addr;
	{
		r_access _access = FData.MGetRAccess();
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		LOG_IF(DFATAL,_it == _access->FIdToIP.end()) << "Main channel for "
																<< aFor
																<< " is not exist.";
		if (_it != _access->FIdToIP.end())
		{
			LOG_IF(ERROR,_it->second.FState!=param_t::E_CONNECTED)
																			<< " Ivalid state "
																			<< _it->second.FState;
			_addr = _it->second.FAddr;
		}
	}
	data_t _data(aVal);
	if (_addr.MIs())
		return FSmServer.MSend(_addr.MGetConst(), _data, false, 0)
				== sent_state_t::E_SENDED; //todo EAGAIN
	return false;
}
bool CSmMainChannel::MSendToService(const data_t& aVal, descriptor_t aFor)
{
	VLOG(2) << "Send data to " << aFor << " by service channel";

	{
		r_access _access = FData.MGetRAccess();
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		LOG_IF(DFATAL, _it == _access->FIdToIP.end()) << "Service channel for "
																<< aFor
																<< " is not exist.";
		if (_it != _access->FIdToIP.end())
		{
			DCHECK_NOTNULL(_it->second.FHandler);
			if (!_it->second.FHandler)
				return false;
			return _it->second.FHandler->MSend(aVal);
		}
	}
	return false;
}
NSHARE::ISocket* CSmMainChannel::MGetSocket()
{
	return &FSmServer;
}
const NSHARE::ISocket* CSmMainChannel::MGetSocket() const
{
	return &FSmServer;
}
NSHARE::eCBRval CSmMainChannel::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*aData)
{
	reinterpret_cast<CSmMainChannel*>(aData)->MReceiver();
	return E_CB_REMOVE;
}
size_t CSmMainChannel::MReceiveImpl(unsigned aType, NSHARE::CBuffer& _data, NSHARE::shared_identify_t const& _from,ILink* aLink)
{
	size_t _num=0;
	switch (aType)
	{
	case E_SM_INFO:
	{
		user_data_info_t _info;
		NSHARE::CBuffer::offset_pointer_t const _point =
				deserialize_dg_head(_info,_data.ptr_const()).second;
		FRecv[_point] = _info;
		VLOG(4) << "Data info:" << FRecv[_point];
		break;
	}
	case E_SM_DATA:
	{
		recv_info_t::iterator _it = FRecv.find(_data.offset());
		CHECK(_it != FRecv.end());

		user_data_t _user_data;
		_data.MMoveTo(_user_data.FData);
		_user_data.FDataId = _it->second;
		FRecv.erase(_it);

		_num+=_user_data.FData.size();
		LOG(INFO)<< "Receive packet #" << _user_data.FDataId.FPacketNumber<<" from "<<_from;

		DCHECK_NOTNULL(aLink);
		if (aLink)
			aLink->MReceivedData(_user_data);
		break;
	}
	case E_SM_DATA_INFO:
	{
		const size_t _full_size=_data.size();
		NSHARE::CBuffer::const_pointer const _p=_data.ptr_const();

		NSHARE::CBuffer::const_pointer const _p_size=_p+_full_size-sizeof(uint32_t);
		uint32_t const _header_size=*(uint32_t const *)_p_size;

		NSHARE::CBuffer::const_pointer const _header=_p_size-_header_size;

		user_data_t _info;
		NSHARE::CBuffer::offset_pointer_t const _data_size = deserialize_dg_head(_info.FDataId,_header).second;
		CHECK_EQ(_data_size+_header_size+sizeof(uint32_t),_full_size);
		//CHECK_EQ(_data.use_count(),1);

		_data.resize(_data_size,false,false);
		_data.MMoveTo(_info.FData);

		_num+=_info.FData.size();

		LOG(INFO)<< "Receive packet #" << _info.FDataId.FPacketNumber<<" from "<<_from;

		DCHECK_NOTNULL(aLink);
		if (aLink)
			aLink->MReceivedData(_info);

		break;
	}
	};
	return _num;
};
void CSmMainChannel::MCheckPacketSequence(const unsigned aPacket,
		unsigned & aLast)
{
	flag_mask_t _last_counter;
	_last_counter.FData.FCounter = aLast;
	++_last_counter.FData.FCounter;
	LOG_IF(FATAL,aLast&&_last_counter.FData.FCounter!=aPacket)
															<< "The packet has been lost. "
															<< " Counter="
															<< aPacket
															<< " Last counter="
															<< aLast;
	aLast = aPacket;
}
void CSmMainChannel::MReceiver()
{
	using namespace NSHARE;
	LOG(WARNING)<< "Async receive";
	NSHARE::shared_identify_t _from;
	for (; FSmServer.MIsOpen();)
	{
		VLOG(5) << "Receive data by SM";

		shared_identify_t _from;
		flag_mask_t _mask;
		CBuffer _data;
		bool _is=FSmServer.MReceiveData(_data,&_from,&_mask.FMask);
		VLOG(2) << "Receive data from " << _from<<" is ="<<_is;
		VLOG_IF(2,_data.empty()) << "data empty from "<<_from;
		DCHECK((!_is) || (_is&&!_data.empty()));
		if ( !_data.empty())
		{
			param_t _param; //The pointer to FTarget can be changed
			{
				r_access _access = FData.MGetRAccess();
				ip_to_id_t::const_iterator _it = _access->FIpToId.find(_from);

				LOG_IF(ERROR,_it == _access->FIpToId.end()) //
				<< "Main channel for " << _from << " is not exist.";

				if (_it == _access->FIpToId.end())
					continue;

				//MThreadSafetyAssign(_param.FIo, _it->second.FIo);
				{
					CRAII<CMutex> _block(FMutex);
					_param = _it->second;
				}
			}
			MCheckPacketSequence(_mask.FData.FCounter, _param.FIo->FFrom);
			size_t _num = MReceiveImpl(_mask.FData.FType, _data, _from,
					_param.FHandler);

			_param.FIo->FRecvBytes += (unsigned)_num;

			MThreadSafetyReset(_param.FIo);
		}
	}
	LOG(WARNING)<< "Async receive ended";
}

bool CSmMainChannel::MIsOveload(descriptor_t aFor) const
{
	NSHARE::shared_identify_t _addr;
	{
		r_access _access = FData.MGetRAccess();
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		LOG_IF(ERROR,_it == _access->FIdToIP.end()) << "Main channel for "
															<< aFor
															<< " is not exist.";
		if (_it == _access->FIdToIP.end())
			return false;
		_addr=_it->second.FAddr;
	}
	return FSmServer.MIsOverload(_addr);
}
bool CSmMainChannel::MAddNewImpl(descriptor_t aFor,
		NSHARE::shared_identify_t const& aParam)
{
	VLOG(2) << "Adding new channel for " << aFor << " id = " << aParam;
	{
		w_access _access = FData.MGetWAccess();
		VLOG(2) << "Access obtained";
		for (id_to_ip_t::iterator _it = _access->FNew.begin();
				_it != _access->FNew.end(); ++_it)
		{
			DCHECK_NOTNULL(_it->second.FHandler);
			VLOG(2) << "handle new channel " << _it->first;
			if (_it->first == aFor)
			{
				VLOG(2) << " handle " << aFor;
				LOG_IF(DFATAL,_it->second.FState!=param_t::E_REQUEST_PARAM)
																					<< aParam
																					<< " Invalid state :"
																					<< _it->second.FState
																					<< " Required "
																					<< param_t::E_REQUEST_PARAM;

				LOG_IF(DFATAL,_access->FIdToIP.find(aFor)!=_access->FIdToIP.end())
																							<< "The main channel for "
																							<< aFor
																							<< " has been opened already for "
																							<< aParam;
				param_t _param = _it->second;
				_access->FNew.erase(_it);

				LOG_IF(ERROR,!_param.FHandler) << "Invalid descriptor  for "
														<< aFor;
				if (_param.FHandler)
				{
					VLOG(2) << "Add new main channel ";
					_param.FState = param_t::E_CONNECTED;
					_param.FAddr = aParam;
					{
						_access->FIpToId[_param.FAddr] = _param;
						_access->FIdToIP[aFor] = _param;
					}
				}

				return true;
			}
		}
	}
	VLOG(2) << "Access return";
	return false;
}
int CSmMainChannel::MSendMainChannelError(descriptor_t aFor, unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, MGetType().c_str());

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(), get_my_id());
	return MSendToService(_channel, aFor);
}
bool CSmMainChannel::MHandleServiceDG(main_channel_param_t const* aData,
		descriptor_t aVal)
{
	if (MIsNew(aVal))
	{
		VLOG(2) << "MHandleServiceDG for " << aVal;

		main_ch_param_t _param(deserialize<main_channel_param_t, main_ch_param_t>(aData,(routing_t*)NULL, (error_info_t*)NULL));

		VLOG(5) << _param;
		NSHARE::shared_identify_t _id(_param.FValue);
		bool _val = _id.MIsValid() && MAddNewImpl(aVal, _id);
		VLOG(2) << "Is added  " << _val;
		if (!_val)
			MSendMainChannelError(aVal,
					main_channel_error_param_t::E_NOT_OPENED);
		else
			MSendMainChannelError(aVal, main_channel_error_param_t::E_OK);
		return _val;
	}
	else
	{
		MSendMainChannelError(aVal,
				main_channel_error_param_t::E_CANNOT_RESETTING);
		return false;
	}
}
bool CSmMainChannel::MHandleServiceDG(main_channel_error_param_t const* aP,
		descriptor_t aFor)
{
	{
		w_access _access = FData.MGetWAccess();
		param_t* _p_param = NULL;
		id_to_ip_t::iterator _it = _access->FIdToIP.find(aFor);
		if (_it != _access->FIdToIP.end())
		{
			_p_param = &_it->second;
		}
		else //may be a new
		{
			id_to_ip_t::iterator _jt = _access->FNew.begin();
			for (;
					_jt != _access->FNew.end()
							&& _jt->second.FHandler->MGetID() != aFor; ++_jt)
			{
			}
			LOG_IF(ERROR,_jt == _access->FNew.end()) << " The main channel "
																<< aFor
																<< " is not exist.";
			if (_jt == _access->FNew.end())
				return false;
			else
			{
				_p_param = &_jt->second;
			}
		}

		DCHECK_NOTNULL(_p_param);		
		switch (_p_param->FState)
		{
		case param_t::E_TRY_OPEN:
			_p_param->FState = param_t::E_REQUEST_PARAM;
			break;
		case param_t::E_REQUEST_PARAM:
			_p_param->FState = param_t::E_CONNECTED;
			break;
		case param_t::E_CONNECTED:
			break;
		case param_t::E_CLOSING:
			break;
		}
		return true;
	}
}
bool CSmMainChannel::MHandleServiceDG(request_main_channel_param_t const* aP,
		descriptor_t aFor)
{
	data_t _buf;
	MFill<main_channel_param_t>(&_buf);
	return MSendToService(_buf, aFor);
}
bool CSmMainChannel::MHandleServiceDG(close_main_channel_t const* aP,
		descriptor_t aFor)
{
	LOG(ERROR)<<"The customer "<<aFor<<" demand of closing channel";
	return MClose(aFor);
}

void CSmMainChannel::MThreadSafetyAssign(NSHARE::intrusive_ptr<sm_io_t> &aFrom,
		NSHARE::intrusive_ptr<sm_io_t> const& aTo) const
{
	CRAII<CMutex> _block(FMutex);
	aFrom = aTo;
}
void CSmMainChannel::MThreadSafetyReset(
		NSHARE::intrusive_ptr<sm_io_t> &aFrom) const
{
	MThreadSafetyAssign(aFrom, NSHARE::intrusive_ptr<sm_io_t>());
}
NSHARE::CConfig CSmMainChannel::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(FSmServer.MSerialize());
	{
		r_access const _acs=FData.MGetRAccess();
		_data_t const& _data=_acs.MGet();
		id_to_ip_t::const_iterator _it = _data.FIdToIP.begin(), _it_end(
				_data.FIdToIP.end());
		for(;_it!=_it_end;++_it)
		{
			std::pair<descriptor_info_t, bool> _info =
					CDescriptors::sMGetInstance().MGet(_it->first);

			NSHARE::CConfig _mch(_info.first.FProgramm.FId.FName);
			_mch.MAdd("d",_it->first);
			_mch.MAdd(_it->second.FAddr.MSerialize());
			_mch.MAdd("state",_it->second.FState);
			if (_it->second.FIo)
			{

				_mch.MAdd(_it->second.FIo->MSerialize());
			}
			_conf.MAdd(_mch);
		}
	}
	return _conf;
}
NSHARE::IAllocater* CSmMainChannel::MAllocater() const
{
	if (FSmServer.MIsOpen())
		return FSmServer.MGetAllocator();
	return NULL;
}
NSHARE::CConfig CSmMainChannel::sm_io_t::MSerialize() const
{
	NSHARE::CConfig _conf("io");
	_conf.MAdd("from",FFrom);
	_conf.MAdd("to",FTo);
	_conf.MAdd("err",FSendError);
	_conf.MAdd("sbyte",FSendBytes);
	_conf.MAdd("rbyte",FRecvBytes);
	_conf.MAdd("overs",FOverloades);
	return _conf;
}

CSmMainChannel::param_t::param_t() :
		 FHandler(NULL), FIo(new sm_io_t),FState(E_TRY_OPEN)
{
}
}
