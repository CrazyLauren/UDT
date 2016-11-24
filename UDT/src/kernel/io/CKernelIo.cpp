/*
 * CKernelIo.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 29.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "../core/CDataObject.h"
#include "../core/CDescriptors.h"
#include "../core/CConfigure.h"
#include "../core/kernel_type.h"
#include "CKernelIo.h"
#include "../services/CRoutingService.h"
#include "../services/CPacketDivisor.h"


template<>
NUDT::CKernelIo::singleton_pnt_t NUDT::CKernelIo::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CKernelIo::BUFFERS = "buffers";
const NSHARE::CText CKernelIo::DEFAULT = "default";
const NSHARE::CText CKernelIo::NAME = "io";
CKernelIo::CKernelIo():IState(NAME)
{
	FIsInited = false;
}

CKernelIo::~CKernelIo()
{
}
bool CKernelIo::MIsForMe(const routing_t& aRoute) const
{
	routing_t::const_iterator _it = aRoute.begin(), _it_end = aRoute.end();
	for (; _it != _it_end && *_it != get_my_id().FId.FUuid; ++_it)
		;
	return _it != _it_end;
}

template<class T>
void CKernelIo::MRouting(const routing_t& aRoute, const T& aWhat,
		descriptor_t const& aFrom)
{
	VLOG(2) << "Routing to " << aRoute;
	NSHARE::operation_t _op(sMRouteOperation, this, NSHARE::operation_t::IO);

	//NSHARE::CRAII<NSHARE::CMutex> _block(FBlocked);
	routing_op_t _routing;
	_routing.FDesc = aFrom;
	_routing.FTo = aRoute;
	_routing.FData = new T(aWhat);
	_routing.FOperation = &CKernelIo::MRouteOperation<T>;
	if (!FRoutings.push(_routing)) //if not working
		CDataObject::sMGetInstance().MPutOperation(_op);
}

template<class T>
void CKernelIo::MRouteOperation(void* aP, const routing_t& aTo,
		descriptor_t const& aFrom)
{
	T* _p = reinterpret_cast<T*>(aP);
	uuids_t const _r(CRoutingService::sMGetInstance().MRoute(aTo, *_p));

	LOG_IF(ERROR,!_r.empty()) << "Cannot routing to " << _r << " from "
										<< aTo.FFrom;
	if (!_r.empty())
	{
		error_info_t _error;
		_error.FError = E_ROUTING_ERROR;
		_error.FWhere = get_my_id().FId;
		_error.FTo = _r;

		uuids_t _to;
		_to.push_back(aTo.FFrom.FUuid);
		MSendTo(aFrom, *_p, routing_t(get_my_id().FId, _to), _error);
	}
	delete _p;
	//return _r;
}
int CKernelIo::sMRouteOperation(const NSHARE::CThread* WHO,
		NSHARE::operation_t* WHAT, void* YOU_DATA)
{
	CKernelIo* _p = reinterpret_cast<CKernelIo*>(YOU_DATA);
	CHECK_NOTNULL(_p);
	_p->MRoute();
	return 0;
}
void CKernelIo::MRoute()
{
	VLOG(2) << "Operation route";
	routing_op_t _op;
	for (; FRoutings.front_pop(_op);)
	{
		(this->*_op.FOperation)(_op.FData, _op.FTo, _op.FDesc);
	}
	VLOG(2) << "No data";
}
template<class T>
void CKernelIo::MRecvImpl(const routing_t& aRoute, const descriptor_t& aFrom,
		const T& aWhat, error_info_t const& aError)
{
	if (!aError.MIsValid())
	{
		VLOG(2) << "Error " << aError;
		const bool _is_for_me = MIsForMe(aRoute);
		if (aRoute.empty() || _is_for_me)
			CDataObject::sMGetInstance().MPush(make_data_from(aFrom, aWhat));

		if (!aRoute.empty() && (!_is_for_me || aRoute.size() > 1))
		{
			MRouting(aRoute, aWhat, aFrom);
		}
	}
	else
	{
		LOG(ERROR)<<"Sent error to "<<aError.FTo<<" by "<<aError.FWhere<<" code:"<<aError.FError;
		switch (aError.FError)
		{
			//todo error sent of
			default:
			break;
		}
	}
}

void CKernelIo::MReceivedData(programm_id_t const& aWhat,
		const descriptor_t& aFrom, const routing_t& aRoute,
		error_info_t const& aError)
{
	MRecvImpl(aRoute, aFrom, aWhat, aError);
}
void CKernelIo::MReceivedData(kernel_infos_array_t const& aWhat,
		const descriptor_t& aFrom, const routing_t& aRoute,
		error_info_t const& aError)
{
	MRecvImpl(aRoute, aFrom, aWhat, aError);
}

void CKernelIo::MReceivedData(fail_send_t const& aWhat,
		const descriptor_t& aFrom, const routing_t& aRoute,
		error_info_t const& aError)
{
	MRecvImpl(aRoute, aFrom, aWhat, aError);
	//todo handle split packet
}
void CKernelIo::MReceivedData(demand_dgs_for_t const& aWhat,
		const descriptor_t& aFrom, const routing_t& aRoute,
		error_info_t const& aError)
{
	MRecvImpl(aRoute, aFrom, aWhat, aError);
}
void CKernelIo::MReceivedData(demand_dgs_t const& aWhat,
		const descriptor_t& aFrom, const routing_t& aRoute,
		error_info_t const& aError)
{
	//MRecvImpl(aRoute, aFrom, aWhat);
	CDataObject::sMGetInstance().MPush(make_data_from(aFrom, aWhat));
}
void CKernelIo::MReceivedData(user_data_t const& aWhat,
		const descriptor_t& aFrom)
{
	{
		LOG(INFO)<< "Receive packet #" << aWhat.FDataId.FPacketNumber<<" from "<<aWhat.FDataId.FFrom;
		VLOG_IF(5,aWhat.FData.size()<100)<<aWhat.FData;
		user_data_id_t _data;
		_data.FId=aFrom;
		_data.FVal=aWhat;
		CDataObject::sMGetInstance().MPush(_data);
	}
}
CKernelIo& CKernelIo::sMGetInstance()
{
	return static_cast<CKernelIo&>(fac_manager_t::sMGetInstance());
}
CKernelIo* CKernelIo::sMGetInstancePtr()
{
	return static_cast<CKernelIo*>(fac_manager_t::sMGetInstancePtr());
}
void CKernelIo::MInit()
{
	LOG_IF(DFATAL,FIsInited) << "Reinited ";
	if (FIsInited)
		return;
	FIsInited = true;
	factory_its_t _its = MGetIterator();
	for (; _its.FBegin != _its.FEnd; ++_its.FBegin)
	{
		VLOG(2) << "Next factory";
		MFactoryAdded(_its.FBegin->second);
	}
	VLOG(2) << "Kernel IO is initialized";
}
NSHARE::CConfig CKernelIo::MBufferingConfFor(descriptor_t const& aName,
		IIOManager* aWhere)
{

	NSHARE::CConfig const& _buf_conf =
			CConfigure::sMGetInstance().MGet().MChild(BUFFERS);
	if (!_buf_conf.MIsEmpty())
	{
		NSHARE::CConfig const& _io = _buf_conf.MChild(aWhere->MGetType());
		if (!_io.MIsEmpty())
		{
			NSHARE::CConfig const& _for = aWhere->MBufSettingFor(aName, _io);
			if (!_for.MIsEmpty())
			{
				VLOG(2) << "Setting buffer for " << aName << " by "
									<< aWhere->MGetType();
				return _for;
			}

			NSHARE::CConfig const& _for_def = _buf_conf.MChild(DEFAULT);
			if (!_for_def.MIsEmpty())
			{
				VLOG(2) << "Setting buffer for " << aName
									<< " by default of " << aWhere->MGetType();
				return _for_def;
			}

		}
//		NSHARE::CConfig const& _for = _buf_conf.MChild(aName);
//		if (!_for.MIsEmpty())
//		{
//			VLOG(2) << "Setting buffer for " << aName
//								<< " by global setting";
//			return _for;
//		}
//
		NSHARE::CConfig const& _def = _buf_conf.MChild(DEFAULT);
		if (!_def.MIsEmpty())
		{

			VLOG(2) << "Setting buffer for " << aName
								<< " by global default";
			return _def;
		}
	}

	static NSHARE::CConfig const _fix;
	return _fix;
}
void CKernelIo::MAddChannelFor(descriptor_t const& aVal, IIOManager* aWhere,
		split_info const& aLimits)
{
	CHECK_NOTNULL(aWhere);
	VLOG(2) << "Adding channel for " << aVal;
	safe_manager_t::WAccess<> _access = FIoManagers.MGetWAccess();
	managers_t & _man = _access.MGet();
	managers_t::iterator _it = _man.find(aVal);
	DCHECK(_it == _man.end());
	if (_it != _man.end())
	{
		LOG(ERROR)<<"Reinit manager for "<<aVal;
		return;
	}
	CHECK(MIsFactoryPresent(aWhere->MGetType()));
	NSHARE::CConfig const& aBufConf = MBufferingConfFor(aVal, aWhere);
	VLOG(2) << "Adding manager " << aWhere->MGetType() << " for " << aVal
						<< " BufConf " << aBufConf;
	NSHARE::intrusive_ptr<manager_t> const _manager(
			new manager_t(*aWhere, CBuffering(aBufConf)));

	_man.insert(managers_t::value_type(aVal, _manager));
	CPacketDivisor::sMGetInstance().MSetLimitsFor(aVal, aLimits);

}

void CKernelIo::MRemoveManagerFor(const descriptor_t& aVal,
		CBuffering::data_list_t& _not_sent_data)
{
	{
		safe_manager_t::WAccess<> _access = FIoManagers.MGetWAccess();
		{
			NSHARE::CRAII<NSHARE::CMutex> _lock(FMutexOperation);
			const size_t _num = FOperartionSendFor.erase(aVal);
			VLOG_IF(2,_num>0) << "Remove operation for " << aVal;
		}
		{
			managers_t& _man = _access.MGet();
			managers_t::iterator _it = _man.find(aVal);
			if (_it != _man.end())
			{
				manager_t const & _m = *_it->second.MGetConst();
				{
					VLOG(2) << "Clearing up buffer.";
					NSHARE::CRAII<NSHARE::CMutex> _lock(_m.FBufMutex);
					_m.FDataBuffer.MClearImidiatle(_not_sent_data);
				}
				_man.erase(aVal);
			}
		}
	}
}

void CKernelIo::MRemoveChannelFor(descriptor_t const& aVal, IIOManager* aWhere)
{
	CHECK_NOTNULL(aWhere);
	CHECK(MIsFactoryPresent(aWhere->MGetType()));
	VLOG(2) << "Removing manager " << aWhere->MGetType() << " for " << aVal;

	CBuffering::data_list_t _not_sent_data;
	MRemoveManagerFor(aVal, _not_sent_data);

	std::vector<user_data_info_t> _fail;
	CBuffering::data_list_t::const_iterator _it = _not_sent_data.begin(),
			_it_end(_not_sent_data.end());
	for(;_it!=_it_end;++_it)
	{
		_fail.push_back(_it->FDataId);
	}
	_not_sent_data.clear();

	CPacketDivisor::sMGetInstance().MRemoveLimitsFor(aVal);

	if(!_fail.empty())
	{
		VLOG(2)<<"Some data is not sent.";
		MNoteFailSend(_fail,aVal);
	}
}
void CKernelIo::MFactoryAdded(factory_t* factory)
{
	CHECK_NOTNULL(factory);
	if (FIsInited)
	{
		VLOG(2) << "Init " << factory->MGetType();
		factory->MInit(this);

		bool _is = factory->MOpen();
		VLOG_IF(2,_is) << "Opened " << factory->MGetType() << " IO manager.";
		LOG_IF(ERROR,!_is) << "Cannot open " << factory->MGetType()
									<< " IO manager.";

//		IIOManager::descriptors_t _descriptors = factory->MGetDescriptors();
//		for (IIOManager::descriptors_t::const_iterator _it =
//				_descriptors.begin(); _it != _descriptors.end(); ++_it)
//			;;
	}
}
void CKernelIo::MFactoryRemoved(factory_t* factory)
{
	if (FIsInited)
	{
		//NSHARE::CRAII<NSHARE::CMutex> _lock_io(FBlockChangingIOMangersList);
		IIOManager::descriptors_t _descriptors = factory->MGetDescriptors();
		for (IIOManager::descriptors_t::const_iterator _it =
				_descriptors.begin(); _it != _descriptors.end(); ++_it)
			MRemoveChannelFor(*_it, factory);
	}
}
NSHARE::CConfig CKernelIo::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	factory_its_t _its=MGetIterator();
	for (; _its.FBegin != _its.FEnd; ++_its.FBegin)//todo mutex
	{
		_conf.MAdd((*_its.FBegin).second->MSerializeRef());
	}
	return _conf;
}
int CKernelIo::sMSendUser(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* YOU_DATA)
{
	CKernelIo* _client = reinterpret_cast<CKernelIo*>(YOU_DATA);
	_client->MSendUserDataImpl();
	return 0;
}



void CKernelIo::MSendingUserDataTo(descriptor_t aTo, IIOManager* aBy,
		CBuffering::data_list_t& _data,
		std::vector<user_data_info_t>& _non_sent) const
{
	//The iteration method is used instead of the pop_back method
	//for avoiding dead looks
	CBuffering::data_list_t::iterator _it = _data.begin();
	DCHECK(!_data.empty());
	VLOG(3) << "Sending data to " << aTo;
	bool _is_not_closed=true;
	for (; _it != _data.end();)
	{
		VLOG(2) << "Send #" <<_it->FDataId.FPacketNumber<<" offset"<< _it->FData.offset() << " use="
							<< _it->FData.use_count();
		if (!aBy->MSend(*_it, aTo))
		{
			if (_is_not_closed && (_is_not_closed=CDescriptors::sMGetInstance().MGet(aTo).second))	//may be closed
			{
				LOG(ERROR)<<"Cannot send "<<_it->FDataId << "  to "<<aTo;
				_non_sent.push_back(_it->FDataId);

				++_it;
			}
			else
			{
				LOG(ERROR)<<"Cannot send "<<_it->FDataId << "  to "<<aTo<<" as closed";
				for(;_it!=_data.end();++_it)
				{
					_non_sent.push_back(_it->FDataId);
				}
			}
		}
		else
		_it=_data.erase(_it);
	}
}

void CKernelIo::MNoteFailSend(std::vector<user_data_info_t> const& aWhat,
		descriptor_t aTo)
{
	std::pair<descriptor_info_t, bool> const _info =
			CDescriptors::sMGetInstance().MGet(aTo);

	std::vector<user_data_info_t>::const_iterator _it =
			aWhat.begin(), _it_end(aWhat.end());
	std::set<unsigned> _repeaters;
	for (; _it != _it_end; ++_it)
	{
		VLOG(5) << "Next fail #" << _it->FPacketNumber << " from "
							<< _it->FFrom;
		if (!_repeaters.insert(_it->FPacketNumber).second)
		{
			VLOG(2) << " Fail info for " << _it->FPacketNumber
								<< " has been set already. Ignoring....";
			continue;
		}
		if (_it->FUUIDTo.MIs())
			CRoutingService::sMGetInstance().MNoteFailSend(*_it,
					_it->FUUIDTo.MGetConst());
		else if (_info.second)
		{
			uuids_t _fail;
			_fail.push_back(_info.first.FProgramm.FId.FUuid);
			CRoutingService::sMGetInstance().MNoteFailSend(*_it, _fail);
		}
		else
		{
			LOG(ERROR)<<"Cannot send "<<*_it<<" to closed descriptor "<<aTo;
		}
	}
}
bool CKernelIo::MNextSend(descriptor_t aDesc,
		CBuffering::data_list_t& _data)
{
	_data.clear();
	 //fix the clients were noted about the unsuccessful sending data
	//but it are not removed from sending queue, they added to the end, therefore it were send again in next time

	safe_manager_t::RAccess<> const _access = FIoManagers.MGetRAccess();
	managers_t const& _man = _access.MGet();
	managers_t::const_iterator _man_it = _man.find(aDesc);
	if(_man_it==_man.end())
		return false;
	manager_t const & _m=*_man_it->second.MGetConst();
	bool _is=false;
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(_m.FBufMutex);
				_m.FDataBuffer.MFinish(_data, false);
		_is=_m.FDataBuffer.MTry(_data);
	}
	VLOG_IF(2,_is) << "There is some more data";
	return _is;
}
std::pair<descriptor_t, IIOManager*> CKernelIo::MBeginSend(
		CBuffering::data_list_t& _data)
{
	typedef std::pair<descriptor_t, IIOManager*> _rval_t;

	safe_manager_t::RAccess<> const _access = FIoManagers.MGetRAccess();
	managers_t const& _man = _access.MGet();
	descriptor_t _desc = -1;
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FMutexOperation);
		if (FOperartionSendFor.empty())
		{
			VLOG(2) << "No data";
			return _rval_t(-1, NULL);
		}
		else
		{
			_desc = *FOperartionSendFor.begin();
			FOperartionSendFor.erase(FOperartionSendFor.begin());
		}
	}
	VLOG(2) << "Handling by " << _desc;
	managers_t::const_iterator _m_it = _man.find(_desc);
	if (_m_it == _man.end())
	{
		LOG(ERROR)<<"There is not manager for "<<_desc;
		return _rval_t(-1,NULL);
	}
	manager_t const & _m=*_m_it->second.MGetConst();
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(_m.FBufMutex);
		if (_m.FDataBuffer.MIsWorking())
		{
			LOG(WARNING)<< "The buffers is sending "<<_desc;
			return _rval_t(-1,NULL);
		}
		if (!_m.FDataBuffer.MTry(_data))
		{
			LOG(WARNING)<<"Cannot send data to "<<_desc;
			return _rval_t(-1,NULL);
		}
	}
	VLOG(2) << "Send to " << _desc;
	return _rval_t(_desc, &_m.FWho);
}
void CKernelIo::MSendUserDataImpl()
{
	VLOG(2)<<"Start sending";
	//NSHARE::CRAII<NSHARE::CMutex> _lock_io(FBlockChangingIOMangersList);
	VLOG(5)<<"Our turn";
	CBuffering::data_list_t _data;

	std::pair<descriptor_t, IIOManager*> _d = MBeginSend(_data);
	if (CDescriptors::sMIsValid(_d.first))
	{
		do
		{
			std::vector<user_data_info_t> _non_sent;
			MSendingUserDataTo(_d.first, _d.second, _data, _non_sent);
			if (!_non_sent.empty())
			{
				MNoteFailSend(_non_sent, _d.first);
			}
		} while (MNextSend(_d.first, _data));

	}
	else
	{
		VLOG(1) << "Wtf?! No data.";
	}
	VLOG(2) << "End send of "<<_d.first;
}

bool CKernelIo::MPutUserDataToSendFifo(const descriptor_t& _by,
		user_data_t const& _data)
{
	bool _does_sent;
	const safe_manager_t::RAccess<> _access = FIoManagers.MGetRAccess();
	const managers_t& _man = _access.MGet();
	managers_t::const_iterator _it_man = _man.find(_by);
	if (_it_man == _man.end())
	{
		LOG(ERROR)<<"There is not manager for "<<_by;
		_does_sent=false;
	}
	else
	{
		manager_t const & _m=*_it_man->second.MGetConst();
		CHECK(!_data.FData.empty());
		{
			NSHARE::CRAII<NSHARE::CMutex> _lock(_m.FBufMutex);

			if (!_m.FDataBuffer.MPut(_data))
			{
				_does_sent=false;
				LOG(ERROR)<<"Cannot put the data to buffer.";
			}
			else
			{
				VLOG(2) << "Put data to buffer";
				_does_sent=true;
				if (!_m.FDataBuffer.MIsWorking())
				{
					VLOG(2) << "Is not working.";
					NSHARE::CRAII<NSHARE::CMutex> _lock(FMutexOperation);
					if (FOperartionSendFor.insert(_by).second)
					{
						VLOG(2) << "Adding operation for " << _by;
						CDataObject::sMGetInstance().MPutOperation(
								NSHARE::operation_t(sMSendUser, this,
										NSHARE::operation_t::IO));
					}
					else
					VLOG(2)<<"There is the handler.";
				}
			}
		}
	}
	return _does_sent;
}

bool CKernelIo::MSendUserData(descriptor_t const& _by, uuids_t const& _to,
		user_data_t & _data, uuids_t& _non_sent)
{
	VLOG(4) << "Send to " << _to << " by " << _by << " " << _data.FDataId;
	bool _does_put = false;
	if (CDescriptors::sMIsValid(_by))
	{
		//NSHARE::CRAII<NSHARE::CMutex> _lock(FMutex);
		_data.FDataId.FUUIDTo = _to;		//by historical reason
		_does_put = MPutUserDataToSendFifo(_by,  _data);
	}

	if (!_does_put)
	{
		_non_sent.insert(_non_sent.end(), _to.begin(), _to.end());
	}
	return _does_put;
}
void CKernelIo::MSendTo(output_vector_t& aBy, std::vector<user_data_t> & _data,
		uuids_t & _non_sent)
{
	CPacketDivisor::limits_t::const_iterator _jt = aBy.begin(), _jt_end(
			aBy.end());
	for (; _jt != _jt_end; ++_jt)
	{
		std::vector<user_data_t>::iterator _uit = _data.begin(), _uit_end(
				_data.end());
		VLOG(2) << "Number of parts " << _data.size();
		for (; _uit != _uit_end; ++_uit)
			if (!MSendUserData(_jt->first, _jt->second, *_uit, _non_sent))
			{
				LOG(ERROR)<<"Cannot sent split packet #"<<_uit->FDataId.FPacketNumber<<" Part:"<<_uit->FDataId.FSplit;
				//todo send fail to packet destination
				break;
			}
	}
}

void CKernelIo::MSendTo(output_decriptors_for_t & aBy, user_data_t & _data, //the data is not const
		//to does not copy it for every descriptor
		fail_send_array_t & aNoSent)
{
	CPacketDivisor::packed_user_data_t _list;
	CPacketDivisor::eError _error;
	uuids_t _not_sent;
	switch ((_error = CPacketDivisor::sMGetInstance().MSplitOrMergeIfNeed(_data,
			aBy, _list, aNoSent)))
	{
	case CPacketDivisor::E_EOK:
	{
		CPacketDivisor::packed_user_data_t::iterator _it = _list.begin(),
				_it_end(_list.end());		//it's not const for optimization!
		for (; _it != _it_end; ++_it)
		{
			VLOG(2) << "The packet #" << _data.FDataId.FPacketNumber
								<< " is split. ";

			MSendTo(_it->FFor, _it->FDataList, _not_sent);
		}
	}
		break;
		case CPacketDivisor::E_DOES_NOT_NEED:
		{
			VLOG(2) << "Send packet #" << _data.FDataId.FPacketNumber<< " directly";
			for (output_decriptors_for_t::iterator _it = aBy.begin();
					_it != aBy.end(); ++_it)
			{
				MSendUserData(_it->first, _it->second, _data, _not_sent);
			}
		}
		break;
		case CPacketDivisor::E_PACKET_LOST:
		{
			LOG(ERROR)<<"Some part of #"<< _data.FDataId.FPacketNumber<<" has been lost.";
			//todo clean send fifo
			//send fail to packet destination
		}
		break;
	}
	if (!_not_sent.empty())
	{
		aNoSent.push_back(fail_send_t(_data.FDataId));
		aNoSent.back().FUUIDTo = _not_sent;
	}

}
} /* namespace NUDT */
