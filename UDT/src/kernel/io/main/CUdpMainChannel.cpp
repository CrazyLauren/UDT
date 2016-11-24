/*
 * CUdpMainChannel.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 13.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <Socket.h>
#include <string.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include "../../core/kernel_type.h"
#include "../../core/CConfigure.h"
#include "../../core/CDescriptors.h"
#include "../../core/CDataObject.h"

#include "../CChannelDiagnostics.h"
#include "../ITcpIOManager.h"
#include "../ILink.h"

#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_USER_DATA,user_data_dg_t)/*Данные пользователя*/\
	/*END*/
#include <parser_in_protocol.h>
#include "CUdpMainChannel.h"

using namespace NSHARE;
namespace NUDT
{
const NSHARE::CText CUdpMainChannel::THREAD_PRIORITY = "priority";
const NSHARE::CText CUdpMainChannel::NAME = "udp";

CUdpMainChannel::CUdpMainChannel() :
		IMainChannel(NAME)
{

	FIsOverload = false;
}
void CUdpMainChannel::MInit()
{
//	NSHARE::CThread::param_t _param;
//	NSHARE::CThread::eThreadPriority _priority =
//			NSHARE::CThread::THREAD_PRIORITY_MAX;
//	if (CConfigure::sMGetInstance().MGet().MGetIfSet(THREAD_PRIORITY,
//			_priority))
//	{
//		VLOG(2)<<"Set up priority from config."<<_priority;	//_param.FPrior = _priority;
//	}
//	_param.FPrior = _priority;
	FUdp.MOpen();
	NSHARE::operation_t _op(CUdpMainChannel::sMReceiver, this, NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);
}
CUdpMainChannel::~CUdpMainChannel()
{
	FUdp.MClose();
}
template<>
void CUdpMainChannel::MFill<main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(main_channel_param_t));
	main_channel_param_t * _p =
			new ((data_t::value_type*) aTo->ptr() + _befor) main_channel_param_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_UDP);
	_p->FUdp.FAddr = INADDR_LOOPBACK;
	_p->FUdp.FPort = FUdp.MGetPort();
	fill_dg_head(_p, sizeof(main_channel_param_t),get_my_id());
	VLOG(2) << (*_p);
}
template<>
void CUdpMainChannel::MFill<request_main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create request main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(request_main_channel_param_t));
	request_main_channel_param_t * _p =
			new ((data_t::value_type*) aTo->ptr() + _befor) request_main_channel_param_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_UDP);
	fill_dg_head(_p, sizeof(request_main_channel_param_t),get_my_id());
	VLOG(2) << (*_p);
}
template<>
void CUdpMainChannel::MFill<close_main_channel_t>(data_t* aTo)
{
	VLOG(2) << "Close  main channel param DG";
	size_t const _befor = aTo->size();
	aTo->resize(_befor + sizeof(close_main_channel_t));
	close_main_channel_t * _p =
			new ((data_t::value_type*) aTo->ptr() + _befor) close_main_channel_t();
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, E_MAIN_CHANNEL_UDP);
	fill_dg_head(_p, sizeof(close_main_channel_t),get_my_id());
	VLOG(2) << (*_p);
}
template<>
inline void CUdpMainChannel::MFill(data_t* _data, const user_data_t& _id)
{
	serialize(_data,_id,false);
}
bool CUdpMainChannel::MSendSetting(ILink* aHandler, descriptor_t aFor)
{
	data_t _buf;
	MFill<main_channel_param_t>(&_buf);
	MFill<request_main_channel_param_t>(&_buf);
	return aHandler->MSend(_buf);
}

bool CUdpMainChannel::MIsNew(descriptor_t const& aFor) const
{
	r_access _access = FData.MGetRAccess();
	for (id_to_ip_t::const_iterator _it = _access->FNew.begin();
			_it != _access->FNew.end(); ++_it)
	{
		if (CDescriptors::sMIsValid(_it->first))
		{
			DCHECK_NOTNULL(_it->second.get());
			if (_it->second->FParser.FUserData && _it->first == aFor)
				return true;
		}
	}
	return false;
}

bool CUdpMainChannel::MOpen(ILink* aHandler,programm_id_t const&,NSHARE::net_address const& aFor)
{
	CHECK_NOTNULL(aHandler);

	LOG_IF(DFATAL,!FUdp.MIsOpen()) << "Udp channel is not opened.";

	if (!FUdp.MIsOpen())
		return false;

	LOG_IF(ERROR,MIsOpen(aHandler->MGetID())) << "Main channel for "
														<< aHandler->MGetID()
														<< "has been opened already";

	bool _is = MIsNew(aHandler->MGetID());
	LOG_IF(ERROR,_is) << "Retrying open main channel for "
								<< aHandler->MGetID();

	if (!_is)
	{
		w_access _access = FData.MGetWAccess();
		smart_param_t _param(new param_t);
		_param->FParser.MSetTarget(this);
		_param->FParser.FUserData = aHandler;
		_param->FState = param_t::E_REQUEST_PARAM; //in MSendSetting The request will be sent;
		_access->FNew[aHandler->MGetID()]=_param;
	}
	return MSendSetting(aHandler, aHandler->MGetID());
}
bool CUdpMainChannel::MIsOpen(descriptor_t aFor) const
{
	r_access _access = FData.MGetRAccess();
	id_to_ip_t::const_iterator _it=_access->FIdToIP.find(aFor);
	return _it != _access->FIdToIP.end() && _it->second->FState==param_t::E_CONNECTED;
}
bool CUdpMainChannel::MClose(descriptor_t aFor)
{
	smart_param_t _param;
	data_t _buf;
	MFill<close_main_channel_t>(&_buf);

	bool _is = false;
	{
		w_access _access = FData.MGetWAccess();
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		LOG_IF(DFATAL,_it == _access->FIdToIP.end()) << "Main channel for " << aFor
													<< " is not exist.";
		_is = _it != _access->FIdToIP.end();
		if (_is)
		{
			_param = _it->second;
			_param->FState = param_t::E_CLOSING;
		}
	}
	if (_is)
	{
		bool _rval = _param->FParser.FUserData->MSend(_buf);
		{
			w_access _access = FData.MGetWAccess();
			_access->FIpToId.erase(_param->FAddr);
			_access->FIdToIP.erase(aFor);
		}
		return _rval;
	}else
	{
		w_access _access = FData.MGetWAccess();
		for (id_to_ip_t::iterator _it = _access->FNew.begin(); _it != _access->FNew.end();
				++_it)
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

bool CUdpMainChannel::MSend(user_data_t const& _id, descriptor_t aFor)
{
	CHECK(!_id.FData.MIsDeached());
	data_t _buf;
	MFill(&_buf, _id);
	return MSend(_buf, aFor);
}
bool CUdpMainChannel::MSend(data_t const& aVal, descriptor_t aFor)
{
	VLOG(2) << "Send data to " << aFor << " by main channel";

	NSHARE::smart_field_t<NSHARE::net_address> _addr;
	{
		r_access _access = FData.MGetRAccess();
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		LOG_IF(DFATAL,_it == _access->FIdToIP.end()) << "Main channel for " << aFor
													<< " is not exist.";
		if (_it != _access->FIdToIP.end())
		{
			LOG_IF(ERROR,_it->second->FState!=param_t::E_CONNECTED)
																			<< " Ivalid state "
																			<< _it->second->FState;
			_addr = _it->second->FAddr;
		}
	}
	if (_addr.MIs())
		return FUdp.MSend(&aVal.front(), aVal.size(), _addr).MIs();
	//todo replace state to Send
	//and wait for The state is not E_CONNECTED;
	//When accepting received than switching from Send to E_CONNECTED state
	return false;
}
bool CUdpMainChannel::MSendToService(const data_t& aVal, descriptor_t aFor)
{
	VLOG(2) << "Send data to " << aFor << " by service channel";

	{
		r_access _access = FData.MGetRAccess();
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		LOG_IF(DFATAL, _it == _access->FIdToIP.end())<< "Service channel for " << aFor
		<< " is not exist.";
		if(_it != _access->FIdToIP.end())
		{
			CHECK_NOTNULL(_it->second.get());
			CHECK_NOTNULL(_it->second->FParser.FUserData);
			if(!_it->second.get() || !_it->second->FParser.FUserData)
				return false;
			return _it->second->FParser.FUserData->MSend(aVal);
		}
	}
	return false;
}
NSHARE::ISocket* CUdpMainChannel::MGetSocket()
{
	return &FUdp;
}
const NSHARE::ISocket* CUdpMainChannel::MGetSocket() const
{
	return &FUdp;
}
int CUdpMainChannel::sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*aData)
{
	reinterpret_cast<CUdpMainChannel*>(aData)->MReceiver();
	return 0;
}

void CUdpMainChannel::MReceiver()
{
	using namespace NSHARE;
	VLOG(2) << "Async receive";
	ISocket::data_t _data;
	_data.reserve(std::numeric_limits<uint16_t>::max());

	for (HANG_INIT; FUdp.MIsOpen(); HANG_CHECK)
	{
		//pthread_testcancel();
		_data.clear();
		net_address _from;
		FOverload.MStartCheking();
		if (FUdp.MReceiveData(&_from, &_data, 0.0) > 0)
		{

			FOverload.MCheking(_from, _data.size());
			net_overload_t::over_t _over;
			FOverload.MFinishCheking(_over);
			if (_over.FIsExist && (FIsOverload = _over.FResult))
			{
				LOG(WARNING)<<"Overloaded Udp detected."<<_over;
			}

			smart_param_t _param;//The pointer to FTarget can be changed
			bool _is = false;
			{
				r_access _access = FData.MGetRAccess();
				ip_to_id_t::const_iterator _it = _access->FIpToId.find(_from);

				LOG_IF(ERROR,_it == _access->FIpToId.end()) //
				<< "Main channel for " << _from << " is not exist.";

				_is = _it != _access->FIpToId.end();
				if (_is)
					_param = _it->second;

				CHECK_NOTNULL(_param.get());
				CHECK_NOTNULL(_param->FParser.FUserData);
				if(!_param.get())
					continue;
				_param->FParser.MReceivedData(_data.cbegin(), _data.cend());
			}

		}
	}
	VLOG(1) << "Socket closed";
}
template<>
void CUdpMainChannel::MProcess(user_data_dg_t const* aP, parser_t* aThis)
{
	VLOG(2) << "Handle user data " << *aP;
	user_data_t _user;
	deserialize(_user,aP,NULL);
	aThis->FUserData->MReceivedData(_user);
	//
}
bool CUdpMainChannel::MIsOveload(descriptor_t aFor) const
{
	return FIsOverload;
}
bool CUdpMainChannel::MAddNewImpl(descriptor_t aFor, udp_param_t const& aParam)
{
	smart_param_t _param;
	{
		w_access _access = FData.MGetWAccess();
		VLOG(2)<<"Access obtained";
		for (id_to_ip_t::iterator _it = _access->FNew.begin(); _it != _access->FNew.end();
				++_it)
		{
			DCHECK_NOTNULL(_it->second.get());
			DCHECK_NOTNULL(_it->second->FParser.FUserData);
			VLOG(2)<<"handle new channel "<<_it->first;
			if (_it->first == aFor)
			{
				VLOG(2)<<" handle "<<aFor;
				LOG_IF(DFATAL,_it->second->FState!=param_t::E_REQUEST_PARAM)
																				<< aParam.FAddr
																				<< " Invalid state :"
																				<< _it->second->FState
																				<< " Required "
																				<< param_t::E_REQUEST_PARAM;

				LOG_IF(DFATAL,_access->FIdToIP.find(aFor)!=_access->FIdToIP.end()) << "The main channel for " << aFor
														<< " has been opened already for "
														<< aParam;

				CHECK_EQ(aParam.FAddr, INADDR_LOOPBACK);
				_param = _it->second;
				_access->FNew.erase(_it);
				break;
			}
		}
		LOG_IF(ERROR,!_param.get())<<"Invalid descriptor  for "<<aFor;
		if(_param.get())
		{
			VLOG(2) << "Add new main channel ";
			_param->FState = param_t::E_CONNECTED;
			_param->FAddr=NSHARE::net_address(aParam.FAddr, aParam.FPort);
			{
				_access->FIpToId[_param->FAddr] = _param;
				_access->FIdToIP[aFor] = _param;
			}
		}
	}
	VLOG(2)<<"Access return";
	return _param.get()!=NULL;
}
int CUdpMainChannel::MSendMainChannelError(descriptor_t aFor, unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, MGetType().c_str());

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(),get_my_id());
	return MSendToService(_channel, aFor);
}
bool CUdpMainChannel::MHandleServiceDG(main_channel_param_t const* aData,
		descriptor_t aVal)
{
	if (MIsNew(aVal))
	{
		VLOG(2)<<"MHandleServiceDG for "<<aVal;
		bool _val = MAddNewImpl(aVal, aData->FUdp);
		VLOG(2)<<"Is added  "<<_val;
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
bool CUdpMainChannel::MHandleServiceDG(main_channel_error_param_t const* aP,
		descriptor_t aFor)
{
	{
		w_access _access = FData.MGetWAccess();
		SHARED_PTR<param_t> _p_param;
		id_to_ip_t::const_iterator _it = _access->FIdToIP.find(aFor);
		if (_it != _access->FIdToIP.end())
		{
			_p_param = _it->second;
		}
		else //may be a new
		{
			id_to_ip_t::iterator _jt = _access->FNew.begin();
			for (;
					_jt !=  _access->FNew.end()
							&& _jt->second->FParser.FUserData->MGetID() != aFor; ++_jt)
			{
			}
			LOG_IF(ERROR,_jt == _access->FNew.end()) << " The main channel " << aFor
													<< " is not exist.";
			if (_jt ==  _access->FNew.end())
				return false;
			else
			{
				_p_param = _jt->second;
			}
		}
		if (!_p_param)
			return false;
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
bool CUdpMainChannel::MHandleServiceDG(request_main_channel_param_t const* aP,
		descriptor_t aFor)
{
	data_t _buf;
	MFill<main_channel_param_t>(&_buf);
	return MSendToService(_buf, aFor);
}
bool CUdpMainChannel::MHandleServiceDG(close_main_channel_t const* aP,
		descriptor_t aFor)
{
	LOG(ERROR)<<"The customer "<<aFor<<" demand of closing channel";
	return MClose(aFor);
}
bool CUdpMainChannel::MChangeSettingChannel(descriptor_t aFor, udp_param_t)
{
	return false; //TODO
}
NSHARE::CConfig CUdpMainChannel::MSerialize() const
{
	r_access _access = FData.MGetRAccess();
	NSHARE::CConfig _state(this->MGetType());
	_state.MAdd("Name",this->MGetType());


	NSHARE::CConfig _current("Cutomers");
	id_to_ip_t::const_iterator _it = _access->FIdToIP.begin();
	for(;_it!=_access->FIdToIP.end();++_it)
	{
		std::pair<descriptor_info_t, bool> _info =
				CDescriptors::sMGetInstance().MGet(_it->first);

		NSHARE::CConfig _cust(_info.first.FProgramm.FId.FName);
		_cust.MAdd("Info",_info.first.MSerialize());

		parser_t::state_t const& _state=_it->second->FParser.MGetState();
		_cust.MAdd("St",(unsigned)_it->second->FState);
		_cust.MAdd("ev",_state.FInvalidVersion);
		_cust.MAdd("se",_state.FESize);
		_cust.MAdd("sb",_state.FEBuffer);
		_cust.MAdd("ce",_state.FECrc);
		_cust.MAdd("hdg",_state.FDGCounter);
		_cust.MAdd("dat",_state.FNumberOfData);
		_cust.MAdd("re",_state.FReadBytes);
		_cust.MAdd("ps",_state.FPackets);
		//_cust.MAdd()
		_current.MAdd(_cust);
	}
	_state.MAdd(_current);
	return _state;
}

}
