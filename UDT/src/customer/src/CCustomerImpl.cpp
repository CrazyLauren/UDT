// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CCustomerImpl.cpp
 *
 *  Created on: 02.02.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <fstream>
#ifdef _WIN32
#include <winsock2.h>
#endif
#include <UDT/programm_id.h>

#include <UDT/CCustomer.h>
#include <CCustomerImpl.h>
#include <CLocalChannelFactory.h>
#include <CRTCFactory.h>
#include <config/customer/customer_config.h>

namespace NUDT
{
using namespace NSHARE;
CCustomer::_pimpl::_pimpl(CCustomer& aThis) :
		my_t(&aThis),//
		FThis(aThis),//
		FMainIO(NULL),//
		FMutexWaitFor(NSHARE::CMutex::MUTEX_NORMAL),//
		FUniqueNumber(0),//
		FMainPacketNumber(0)
{
	FDemandPriority.MWrite(0);

	FEventsNextFreeNumber = 0 ;
	FEventsSize = 0;
	FEventsData.reserve(demand_dg_t::MAX_HANDLER);

	for(unsigned i=0; i < demand_dg_t::MAX_HANDLER;++i)
	{
		FEventsData.push_back(callback_t(NULL, NULL));

		if(i != 0)
			FEventsData[i- 1].FYouData = &FEventsData.back();
	}
}
int CCustomer::_pimpl::MInitialize(NSHARE::CText const& aProgram,
		NSHARE::CText const& aName, NSHARE::version_t const& aVersion)
{
	if (int _rval = MInitId(aProgram, aName, aVersion))
	{
		LOG(ERROR) << "Cannot initialize id as " << _rval;
		return _rval;
	}

	if (int _rval = MInitFactorys())
	{
		LOG(ERROR)<< "Cannot initialize id as " << _rval;
		return _rval;
	}

	if (int _rval = MLoadLibraries())
	{
		LOG(ERROR)<< "Cannot initialize id as " << _rval;
		return _rval;
	}

	if (int _rval = MInitCallBacks())
	{
		LOG(ERROR) << "Cannot initialize id as " << _rval;
		return _rval;
	}
	if(!MInitializeModules())
	{
		LOG(ERROR)<< "Invalid IO mamanger";
		return  ERROR_CONFIGURE_IS_INVALID;
	}
	if (int _rval = MUdpateRecvList())
	{
		LOG(ERROR)<< "Cannot initialize id as " << _rval;
		return _rval;
	}
	return 0;
}
CCustomer::_pimpl::~_pimpl()
{
	//fixme remove cb
	MClose();
}

/** Initialize program ID of customer
 *
 *
 * @param aProgram a name of program
 * @param aName a wish name of customer
 * @param aVersion a version
 * @return 0 if no error
 */
int CCustomer::_pimpl::MInitId(NSHARE::CText const& aProgram,
		NSHARE::CText const& aName,NSHARE::version_t const& aVersion)
{
	if (is_id_initialized())
		return 0;

	LOG_IF(WARNING,aName.empty()) << "The name of program is not present"
			" trying to take it from config file.";
	int _rval = 0;
	CText _name(aName);

	bool _val = CConfigure::sMGetInstance().MGet().MGetIfSet(RRD_NAME, _name);

	LOG_IF(WARNING,_val && !aName.empty())
													<< "The name of program is redefined in the config file";
	LOG_IF(DFATAL,_name.empty() && !aName.empty())
													<< "Invalid program name  in the config file";
	if (_name.empty())
	{
		if (!aProgram.empty())
		{
			CText _programm(aProgram);
			CText::size_type slash = _programm.find_last_of("/\\");
			if (slash != std::string::npos)
			{
				_name = CText(_programm, slash);
				LOG(WARNING)<<"using program name "<<_name<<"as customer name";
			}
		}
		if(_name.empty())
		{
			_rval = static_cast<int>(ERROR_NO_NAME);
			LOG(ERROR)<<"Invalid customer name";
		}
	}
	if (_rval == 0)
	{
		if (init_id((char*) _name.c_str(), E_CONSUMER, aVersion) != 0)
		{
			_rval = static_cast<int>(ERROR_NAME_IS_INVALID);
		}
		else
			FMyId = get_my_id();
	}
	return _rval;
}
/** Load all libraries
 *
 * @return 0 if no error
 */
int CCustomer::_pimpl::MLoadLibraries()
{
	if (CResources::sMGetInstancePtr())
		return 0;
	//MODULE
	CConfig const* const _p = CConfigure::sMGetInstance().MGet().MChildPtr(MODULES);
#ifndef CUSTOMER_WITH_STATIC_MODULES
	LOG_IF(DFATAL,!_p) << "Invalid config file.Key " << MODULES
	    << " is not exist.";

	if (!_p)
	    return static_cast<int>(ERROR_CONFIGURE_IS_INVALID);
#endif
	new CResources(_p !=NULL ? *_p :NSHARE::CConfig::sMGetEmpty());

	CResources::sMGetInstance().MLoad();
	return 0;
}
/** Create all factories
 *
 * @return
 */
int CCustomer::_pimpl::MInitFactorys()
{

	//init factories
	if (!CIOFactory::sMGetInstancePtr())
		new CIOFactory();

	if (!CLocalChannelFactory::sMGetInstancePtr())
		new CLocalChannelFactory();
	if (!CDataObject::sMGetInstancePtr())
		new CDataObject();
	if (!CRTCFactory::sMGetInstancePtr())
		new CRTCFactory();
	return 0;
}

/** Subscribes to callbacks
 *
 * @return 0 if no error
 */
int CCustomer::_pimpl::MInitCallBacks()
{
	//FThread += CB_t(sMReceiver, this);
	{
		callback_data_t _callbak(sMReceiver, this);
		CDataObject::value_t _val_channel(recv_data_from_t::NAME, _callbak);
		CDataObject::sMGetInstance() += _val_channel;
	}
	{
		callback_data_t _callbak(sMReceiveCustomers, this);
		CDataObject::value_t _val_channel(progs_id_from_t::NAME, _callbak);
		CDataObject::sMGetInstance() += _val_channel;
	}
	{
		callback_data_t _callbak(sMFailSents, this);
		CDataObject::value_t _val_channel(fail_send_id_t::NAME, _callbak);
		CDataObject::sMGetInstance() += _val_channel;
	}
	{
		callback_data_t _callbak(sMDemands, this);
		CDataObject::value_t _val_channel(demand_dgs_id_t::NAME, _callbak);
		CDataObject::sMGetInstance() += _val_channel;
	}
	{
		callback_data_t _callbak(sMConnect, this);
		CDataObject::value_t _val_channel(connected_to_kernel_t::NAME, _callbak);
		CDataObject::sMGetInstance() += _val_channel;
	}
	{
		callback_data_t _callbak(sMDisconnect, this);
		CDataObject::value_t _val_channel(disconnected_from_kernel_t::NAME, _callbak);
		CDataObject::sMGetInstance() += _val_channel;
	}
	{
		callback_data_t _callbak(sMRTCUpdated, this);
		CDataObject::value_t _val_channel(real_time_clocks_updated_t::NAME,
				_callbak);
		CDataObject::sMGetInstance() += _val_channel;
	}
	return 0;
}

bool CCustomer::_pimpl::MIsOpened() const
{
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";

	return FMainIO && FMainIO->MIsOpened();
}
bool CCustomer::_pimpl::MIsConnected() const
{
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(4) << "Our turn.";

	return FMainIO && FMainIO->MIsConnected();
}

/** Initialize modules
 *
 * @return true if no error
 */
bool CCustomer::_pimpl::MInitializeModules()
{
	const array_of_modules_t _modules(MGetModules());
	for (array_of_modules_t::const_iterator _it = _modules.begin(); //
	_it != _modules.end() ; //
			++_it)
	{
		(*_it)->MInit(this);
	}

	if (!FMainIO)
	{
		CText _name(DEFAULT_IO_MANAGER);
		bool _val = CConfigure::sMGetInstance().MGet().MGetIfSet(DOING_MODULE,
				_name);
		LOG_IF(WARNING, !_val)  << "The operative module is not present in the config file";
		FMainIO = CIOFactory::sMGetInstance().MGetFactory(_name);
		LOG_IF(FATAL, !FMainIO)<< "The module " << _name << " is not exist.";
	}

	return FMainIO!=NULL;
}

/** Open all modules
 *
 * @param aParam thread param
 * @return true if no error
 */
bool CCustomer::_pimpl::MOpenModules(NSHARE::CThread::param_t* aParam)
{
	bool _result = true;
	const array_of_modules_t _modules(MGetModules());
	for (array_of_modules_t::const_iterator _it = _modules.begin(); //
	_it != _modules.end() && _result; //
			++_it)
	{
		_result = (*_it)->MOpen(aParam);
	}
	return _result;
}

/** Start working all modules
 *
 * @return true if no error
 * @todo open all libraries
 */
bool CCustomer::_pimpl::MOpen()
{
	VLOG(2) << "Open customer";

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";

	NSHARE::CThread::param_t _param;
	NSHARE::CThread::param_t* _pparam=NULL;
	{
		unsigned _priority =
				NSHARE::CThread::THREAD_PRIORITY_DEFAULT;

		if (CConfigure::sMGetInstance().MGet().MGetIfSet(THREAD_PRIORITY,
				_priority))
		{
			NSHARE::CThread::param_t _param;
			_param.priority = static_cast<NSHARE::CThread::eThreadPriority> (_priority);
			_pparam = &_param;
		}
	}

	bool _result = MOpenModules(_pparam);

	if(!_result)
		MClose();

	return _result;
}

void CCustomer::_pimpl::MClose()
{
	VLOG(2) << "Close customer";

	MCloseModules();
}
/** Close all modules except Main IO manager
 *
 */
void CCustomer::_pimpl::MCloseModules()
{
	const array_of_modules_t _modules(MGetModules());
	for (array_of_modules_t::const_iterator _it = _modules.begin(); //
	_it != _modules.end(); ++_it)
	{
		(*_it)->MClose();
	}
}
int CCustomer::_pimpl::sMConnect(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	DCHECK_EQ(aWhat->FType, connected_to_kernel_t::NAME);
	_pimpl* _impl = reinterpret_cast<_pimpl*>(aData);
	_impl->MEventConnected();
	return E_CB_SAFE_IT;
}
int CCustomer::_pimpl::sMDisconnect(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	DCHECK_EQ(aWhat->FType, disconnected_from_kernel_t::NAME);
	_pimpl* _impl = reinterpret_cast<_pimpl*>(aData);
	_impl->MEventDisconnected();
	return E_CB_SAFE_IT;
}
int CCustomer::_pimpl::sMRTCUpdated(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	DCHECK_EQ(aWhat->FType, real_time_clocks_updated_t::NAME);
	_pimpl* _impl = reinterpret_cast<_pimpl*>(aData);
	_impl->MCallImpl(EVENT_UPDATE_RTC_INFO,NULL);
	return E_CB_SAFE_IT;
}
int CCustomer::_pimpl::sMFailSents(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	CHECK_EQ(aWhat->FType, fail_send_id_t::NAME);
	_pimpl* _impl = reinterpret_cast<_pimpl*>(aData);
	fail_send_t const& _prog =
			reinterpret_cast<fail_send_id_t*>(aWhat->FPointToData)->FData;
	fail_sent_args_t _fail;
	_fail.FFrom=_prog.FRouting.FFrom.FUuid;
	_fail.FProtocolName = _prog.FProtocol;
	_fail.FPacketNumber = _prog.FPacketNumber&std::numeric_limits<uint16_t>::max();
	_fail.FHeader=_prog.FWhat;
	_fail.FSentTo = _prog.FDestination;
	_fail.FFails = _prog.FRouting;
	_fail.FUserCode=_prog.MGetUserError();
	_fail.FErrorCode=_prog.MGetInnerError();

	_impl->MCallImpl(EVENT_FAILED_SEND, &_fail);
	return 0;

}
int CCustomer::_pimpl::sMDemands(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	CHECK_EQ(aWhat->FType, demand_dgs_id_t::NAME);
	_pimpl* _impl = reinterpret_cast<_pimpl*>(aData);
	demand_dgs_t const& _prog =
			reinterpret_cast<demand_dgs_id_t*>(aWhat->FPointToData)->FData;
	subcribe_receiver_args_t _new,_old;
	_new.FReceivers.reserve(_prog.size());
	_old.FReceivers.reserve(_prog.size());
	demand_dgs_t::const_iterator _it = _prog.begin(), _it_end(_prog.end());

	for (; _it != _it_end; ++_it)
	{
		subcribe_receiver_args_t::what_t _what;
		_what.FWhat.FFrom = _it->FNameFrom.MGetRawName();
		_what.FWhat.FProtocolName = _it->FProtocol;
		_what.FWhat.FRequired = _it->FWhat;
		_what.FWhat.FFlags=_it->FFlags.MGetMask();

		if (_it->FUUIDFrom.MIs())
			_what.FWho = _it->FUUIDFrom.MGetConst();

		if(_it->FFlags.MGetFlag(demand_dg_t::E_REMOVED))
			_old.FReceivers.push_back(_what);
		else
			_new.FReceivers.push_back(_what);
	}

	int _count=0;
	if(!_new.FReceivers.empty())
		_count+= _impl->MCallImpl(EVENT_RECEIVER_SUBSCRIBE, &_new);
	if(!_old.FReceivers.empty())
		_count+=_impl->MCallImpl(EVENT_RECEIVER_UNSUBSCRIBE, &_old);

	VLOG(2) << "Event recv=" << _count << " num=" << _prog.size();
	(void) _count;
	return 0;

}

int CCustomer::_pimpl::sMReceiveCustomers(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	CHECK_EQ(aWhat->FType, progs_id_from_t::NAME);
	_pimpl* _impl = reinterpret_cast<_pimpl*>(aData);

	progs_id_t const& _prog =
			reinterpret_cast<progs_id_from_t*>(aWhat->FPointToData)->FData;
	progs_id_t _old;

	{
		safety_customers_t::WAccess<> _acc = _impl->FCustomers.MGetWAccess();
		_old.swap(*_acc);
		*_acc = _prog;
	}
	customers_updated_args_t _args;
	std::set_difference(_old.begin(), _old.end(), _prog.begin(), _prog.end(),
			std::inserter(_args.FDisconnected, _args.FDisconnected.end()));

	std::set_difference(_prog.begin(), _prog.end(), _old.begin(), _old.end(),
			std::inserter(_args.FConnected, _args.FConnected.end()));

	_impl->MCallImpl(EVENT_CUSTOMERS_UPDATED, &_args);
	return 0;
}
int  CCustomer::_pimpl::MCallImpl(key_t const& aKey, value_arg_t const& aCallbackArgs)
{
	VLOG(2)<<"The event: "<<aKey<<" is called ";
	int const _val = events_t::MCall(aKey, aCallbackArgs);
	VLOG(2)<<"Result of calling event "<<aKey<<" is "<<_val;
	if (!FWaitFor.empty())
	{
		NSHARE::CRAII<NSHARE::CMutex> _lock(FMutexWaitFor);
		wait_for_t::size_type const _v = FWaitFor.erase(aKey);
		if (_v != 0)
		{
			VLOG(2) << "At last event " << aKey;
			FCondvarWaitFor.MBroadcast();
		}
	}
	return _val;
}
void CCustomer::_pimpl::MEventConnected()
{
	MCallImpl(EVENT_CONNECTED, NULL);
}
void CCustomer::_pimpl::MEventDisconnected()
{
	MCallImpl(EVENT_DISCONNECTED, NULL);
}

int CCustomer::_pimpl::sMReceiver(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	DCHECK_EQ(aWhat->FType, recv_data_from_t::NAME);
	reinterpret_cast<_pimpl*>(aData)->MReceiver(
			*reinterpret_cast<recv_data_from_t*>(aWhat->FPointToData));
	return 0;
}

/** Fills the output customer message (#received_message_args_t)
 *  #from recv_data_from_t object
 *
 * @param aTo store to it
 * @param aFrom move from
 */
void CCustomer::_pimpl::MFillOutputMessage(received_message_args_t* aTo,
		recv_data_from_t & aFrom)
{
	received_message_args_t& _raw_args(*aTo);
	VLOG(4) << "Handle data from " << aFrom.FData.FDataId.FRouting.FFrom
						<< " ; size:" << aFrom.FData.FData.size()
						<< " Packet # "
						<< aFrom.FData.FDataId.FPacketNumber;

	_raw_args.FPacketNumber = aFrom.FData.FDataId.FPacketNumber&std::numeric_limits<uint16_t>::max();

	_raw_args.FFrom = aFrom.FData.FDataId.FRouting.FFrom.FUuid; //may be remove?
	_raw_args.FProtocolName =
			aFrom.FData.FDataId.MIsRaw() ?
					RAW_PROTOCOL_NAME : aFrom.FData.FDataId.FProtocol;
	_raw_args.FHeader =aFrom.FData.FDataId.FWhat;
	_raw_args.FTo=aFrom.FData.FDataId.FDestination;
	_raw_args.FOccurUserError=0;
	_raw_args.FEndian=aFrom.FData.FDataId.FEndian;


	aFrom.FData.FData.MMoveTo(_raw_args.FMessage.FBuffer);

	size_t const _data_offset =
			aFrom.FData.FDataId.MIsRaw() ? 0 : aFrom.FData.FDataId.FDataOffset;

	LOG_IF(ERROR, _raw_args.FMessage.FBuffer.empty()) << "Empty data";

	if (!_raw_args.FMessage.FBuffer.empty())
	{
		_raw_args.FMessage.FHeaderBegin =
				reinterpret_cast<const uint8_t*>(&_raw_args.FMessage.FBuffer.front());
		_raw_args.FMessage.FBegin=_raw_args.FMessage.FHeaderBegin+_data_offset;
		_raw_args.FMessage.FEnd =
				reinterpret_cast<const uint8_t*>(&_raw_args.FMessage.FBuffer.back());
		++_raw_args.FMessage.FEnd; //last equal end
	}
}

/** Inform that message have error
 *
 * @param aDataInfo Info about message
 * @param aError error code
 */
void CCustomer::_pimpl::MInformInvalidMessage(const user_data_info_t& aDataInfo,
		user_error_type aError) const
{
	fail_send_t _fsent(aDataInfo);
	_fsent.FRouting.clear();
	_fsent.FRouting.push_back(get_my_id().FId.FUuid);
	_fsent.MSetUserError(aError);
	fail_send_id_from_me_t _sent;
	_sent.FData = _fsent;
	CDataObject::sMGetInstance().MPush(_sent);
}
/** Handle subscription
 *
 * @param aHandler an Id of callback
 * @param aDataInfo Info about data
 * @param aMessage message
 * @return true if no error
 */
bool CCustomer::_pimpl::MHandleSubscription(demand_dg_t::event_handler_value_t const& aHandler,
		user_data_info_t const& aDataInfo,
		received_message_args_t& aMessage)
{
	LOG(INFO) << "Handling #" << aMessage.FPacketNumber << " by "
							<< aMessage.FProtocolName << " protocol from "
							<< aMessage.FFrom << " Raw="
							<< aMessage.FHeader.FNumber << " by CB #"
							<< aHandler;
	FEventsData[aHandler].operator ()(&FThis, &aMessage);

	if (!aMessage.FOccurUserError)
	{
		VLOG(2) << "The packet #" << aMessage.FPacketNumber << " by " << aMessage.FProtocolName << " protocol from " << aMessage.FFrom << " handled";

		return true;
	}
	else
	{
		LOG(ERROR) << "The packet #" << aMessage.FPacketNumber << " by " << aMessage.FProtocolName << " protocol from " << aMessage.FFrom << " is not handled as " << (int)(((aMessage.FOccurUserError)));

		MInformInvalidMessage(aDataInfo, aMessage.FOccurUserError);
		return false;
	}
}

/** Inform subscribers about new data
 *
 * @param _raw_args
 * @param aFrom
 */
void CCustomer::_pimpl::MInformSubscriber(received_message_args_t* const aData,
		user_data_info_t const& aDataInfo)
{
	DCHECK_NOTNULL(aData);

	received_message_args_t& _raw_args(*aData);

	VLOG(1) << "Handle data from " << _raw_args.FFrom << " ; size:"
						<< _raw_args.FMessage.FBuffer.size() << " Packet # "
						<< _raw_args.FPacketNumber;

	CRAII<CMutex> _block(FParserMutex);//!< May be problem as lock thread

	int _count = 0;
	_count += MCallImpl(EVENT_RAW_DATA, &_raw_args);

	if (!aDataInfo.FHandlerEventsList.empty() && FEventsSize > 0)
	{
		user_data_info_t::handler_id_array_t::const_iterator _hit =
				aDataInfo.FHandlerEventsList.begin(), _hit_end(
				aDataInfo.FHandlerEventsList.end());
		_raw_args.FRemainCallbacks = (unsigned)aDataInfo.FHandlerEventsList.size();
		_raw_args.FCbs = _raw_args.FRemainCallbacks;
		for (; _hit != _hit_end; ++_hit)
		{
			--_raw_args.FRemainCallbacks;
			demand_dg_t::event_handler_value_t const _handler =
					user_data_info_t::sMGeHandlerOf(*_hit);
			_raw_args.FFlags = user_data_info_t::sMGeFlagsOf(*_hit);
			//cb_event_t::const_iterator _jt = FEvents.find(*_it);
			if (FEventsData[_handler].MIs())
			{
				if(MHandleSubscription(_handler, aDataInfo, _raw_args))
					++_count;
				else
				{
					LOG_IF(DFATAL,_count!=0) << "Some handlers were handled packet #"
														<< _raw_args.FPacketNumber
														<< ", but the error "
														<< _raw_args.FOccurUserError
														<< " is occurred in handler "
														<< *_hit;
					break;
				}
			}
			else
			{
				LOG(ERROR) << " CB " << _handler
										<< " is not founded. Ignoring";
			}
		}
		LOG_IF(ERROR,_count==0) << "It does not expect packet from "
										<< _raw_args.FFrom
										<< ". Ignoring ...";
	}
	LOG_IF(INFO,_count==0) << _raw_args.FMessage.FBuffer.size()
									<< " bytes of  data from "
									<< _raw_args.FFrom
									<< " is not handled.";
}

void CCustomer::_pimpl::MReceiver(recv_data_from_t & aFrom)
{
	VLOG(1) << "Thread data handler started.";
	LOG_IF(ERROR,!FMainIO) << " There is not working module";
	received_message_args_t _raw_args;

	MFillOutputMessage(&_raw_args,aFrom);
	MInformSubscriber(&_raw_args, aFrom.FData.FDataId);
}
program_id_t  CCustomer::_pimpl::MCustomer(NSHARE::uuid_t const& aUUID) const
{
	safety_customers_t::RAccess<> const _acc = FCustomers.MGetRAccess();
	customers_t const& _cus = _acc.MGet();
	program_id_t _id;
	_id.FId.FUuid = aUUID;
	customers_t::const_iterator _it = _cus.find(_id);
	LOG_IF(ERROR,_it==_cus.end()) << "Cannot find " << aUUID;
	return _it != _cus.end() ? *_it: program_id_t();
}
int CCustomer::_pimpl::MSettingDgParserFor(
		requirement_msg_info_t aNumber, const callback_t& aHandler)
{
	const NSHARE::CText& aReq=aNumber.FFrom;
	VLOG(2) << "Setting for " << aReq << " parser";
	if (aNumber.FProtocolName.empty())
		aNumber.FProtocolName = RAW_PROTOCOL_NAME;

	aNumber.FProtocolName.MToLowerCase();

	demand_dgs_t::value_type _val;
	_val.FNameFrom = NSHARE::CProgramName(aReq);
	_val.FProtocol = aNumber.FProtocolName;
	_val.FWhat = aNumber.FRequired;
	_val.FFlags.MSetFlag(aNumber.FFlags,true);

	VLOG(2)<<"New demand:"<<_val;
	CRAII<CMutex> _block(FParserMutex);
	if(FEventsNextFreeNumber == demand_dg_t::NO_HANDLER)
		return -1;

	DCHECK_EQ(FDemands.size(), FEventsSize);

	VLOG(2) << "Our turn.";

	size_t _i = 0;
	size_t const _size = FDemands.size();
	for (; _i != _size && !(FDemands[_i] == _val); ++_i)
		;

	if (_i == _size)
	{
		LOG(INFO)<< "Add additional parser for channel '" << aNumber.FRequired << "' by "<< aNumber.FProtocolName<<"'";

		_val.FEventHandler = FEventsNextFreeNumber;

		unsigned _next_free = demand_dg_t::NO_HANDLER;
		if(FEventsData[FEventsNextFreeNumber].FYouData != NULL)
		{
			_next_free = (unsigned)( (callback_t*)FEventsData[FEventsNextFreeNumber].FYouData
					- (callback_t*)(&FEventsData.front()));
		}
		FEventsNextFreeNumber = _next_free;

		//FEventsNextFreeNumber = FEventsData[];

		FDemands.push_back(_val);
		FEventsData[_val.MGetHandler()] = aHandler;
		++FEventsSize;
	}
	else
	{
		LOG(WARNING) << "Replace handler "<<FEventsData[(uint32_t)_i]<< " to " << aHandler;
		CHECK(FEventsData[_val.MGetHandler()].MIs());
		FEventsData[_val.MGetHandler()] = aHandler;
	}

	MUpdateDemandPriority();

	DVLOG_IF(2,!FMainIO) << " The channel has not opened yet.";
	MUdpateRecvList();

	DCHECK_EQ(FDemands.size(), FEventsSize);
	return _val.MGetHandler();
}
int CCustomer::_pimpl::MRemoveDgParserFor( demand_dg_t::event_handler_value_t  aNumber, request_info_t* aTo)
{
	size_t _i = 0;
	size_t const _size = FDemands.size();
	for (; _i != _size && !(FDemands[_i].MGetHandler() == aNumber); ++_i)
		;
	if (_i == _size)
	{
		return ERROR_HANDLER_IS_NOT_EXIST;
	}
	const uint32_t _handler = FDemands[_i].MGetHandler();

	callback_t& _event_data = FEventsData[FDemands[_i].MGetHandler()];
	if(aTo != NULL)
	{
		MGet(aTo,_event_data, FDemands[_i]);
	}

	if( FEventsNextFreeNumber == demand_dg_t::NO_HANDLER )
	{
		_event_data = callback_t(NULL, NULL);
		FEventsNextFreeNumber = FDemands[_i].MGetHandler();
	}else
	{
		_event_data = callback_t(NULL, &FEventsData[FEventsNextFreeNumber]);
	}
	FDemands.erase(FDemands.begin() + _i);
	--FEventsSize;

	MUpdateDemandPriority();

	DVLOG_IF(2,!FMainIO) << " The channel has not opened yet.";
	MUdpateRecvList();

	DCHECK_EQ(FDemands.size(), FEventsSize);
	return _handler;
}
int CCustomer::_pimpl::MRemoveDgParserFor(
		requirement_msg_info_t aNumber,callback_t * aTo)
{
	const NSHARE::CText& aReq=aNumber.FFrom;
	VLOG(2) << "Remove parser for " << aReq;

	if (aNumber.FProtocolName.empty())
		aNumber.FProtocolName = RAW_PROTOCOL_NAME;

	aNumber.FProtocolName.MToLowerCase();
	CRAII<CMutex> _block(FParserMutex);

	VLOG(2) << "Our turn.";
	CHECK_EQ(FDemands.size(), FEventsSize);

	demand_dgs_t::value_type _val;
	_val.FNameFrom = NSHARE::CProgramName(aReq);
	_val.FProtocol = aNumber.FProtocolName;
	_val.FWhat = aNumber.FRequired;

	size_t _i = 0;
	size_t const _size = FDemands.size();
	for (; _i != _size && !(FDemands[_i] == _val); ++_i)
		;
	if (_i == _size)
	{
		return ERROR_HANDLER_IS_NOT_EXIST;
	}
	const uint32_t _handler = FDemands[_i].MGetHandler();

	callback_t& _event_data = FEventsData[FDemands[_i].MGetHandler()];
	if(aTo != NULL)
	{
		*aTo = _event_data;
	}
	if (FEventsNextFreeNumber == demand_dg_t::NO_HANDLER)
	{
		_event_data = callback_t(NULL, NULL);
		FEventsNextFreeNumber = FDemands[_i].MGetHandler();
	}
	else
	{
		_event_data = callback_t(NULL, &FEventsData[FEventsNextFreeNumber]);
	}

	FDemands.erase(FDemands.begin() + _i);
	--FEventsSize;
	MUpdateDemandPriority();

	VLOG_IF(2,!FMainIO) << " The channel has not opened yet.";
	MUdpateRecvList();

	CHECK_EQ(FDemands.size(), FEventsSize);
	return _handler;
}
void CCustomer::_pimpl::MUpdateDemandPriority()
{
	++FDemandPriority;
	if(FDemandPriority > (unsigned)demand_dgs_t::MAX_DEM_PRIORITY)
		FDemandPriority.MWrite(1);

	FDemands.FPriority = FDemandPriority;
}
void CCustomer::_pimpl::MGet(request_info_t* aTo,callback_t const& aCb, demand_dg_t const& aDemand) const
{
	DCHECK_NOTNULL(aTo);
	request_info_t& _msg(*aTo);
	_msg.FWhat.FFlags = aDemand.FFlags.MGetMask();
	_msg.FWhat.FProtocolName = aDemand.FProtocol;
	_msg.FWhat.FRequired = aDemand.FWhat;
	_msg.FWhat.FFrom = aDemand.FNameFrom.MGetRawName();
	_msg.FHandler = aCb;
}
void CCustomer::_pimpl::MGetMyWishForMSG(std::vector<request_info_t>& aTo) const
{
	CRAII<CMutex> _block(FParserMutex);

	size_t const _size = FDemands.size();
	for (size_t i = 0; i != _size;++i)
	{
		request_info_t _msg;
		MGet(&_msg,FEventsData[FDemands[i].MGetHandler()], FDemands[i]);
		aTo.push_back(_msg);
	}
}
int CCustomer::_pimpl::MUdpateRecvList() const
{
	VLOG(2) << "Receiving list";

	req_recv_t _list;
	_list.FDemand = FDemands;

	CDataObject::sMGetInstance().MPush(_list);

	return 0;
}
int CCustomer::_pimpl::MSendTo(const NSHARE::CText& aProtocolName,
		NSHARE::CBuffer& aBuf, const NSHARE::CText& aTo, eSendToFlags aFlag)
{
	if (!FMainIO)
	{
		LOG(WARNING)<<"Cannot send to "<<aTo<<" as library is not opened.";
		return static_cast<int>(ERROR_NOT_OPEN);
	}

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData=aBuf;
	_data.FDataId.FProtocol=aProtocolName;
	aBuf.release(); //optimization
	return MSendImpl(_data);
}
int CCustomer::_pimpl::MSendTo(const NSHARE::CText& aProtocolName,
		NSHARE::CBuffer& aBuf, const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{
	if (!FMainIO)
	{
		LOG(WARNING)<<"Cannot send to "<<aTo<<" as library is not opened.";
		return static_cast<int>(ERROR_NOT_OPEN);
	}

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData = aBuf;
	_data.FDataId.FProtocol = aProtocolName;
	_data.FDataId.FDestination.push_back(aTo);
	aBuf.release(); //optimization
	return MSendImpl(_data);
}
int CCustomer::_pimpl::MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf,
		const NSHARE::uuid_t& aTo,NSHARE::version_t const& aVer, eSendToFlags)
{

	if (!FMainIO)
	{
		LOG(WARNING)<<"Cannot send to "<<aTo<<" as library is not opened.";
		return static_cast<int>(ERROR_NOT_OPEN);
	}
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData = aBuf;
	_data.FDataId.FWhat.FNumber = aNumber;
	_data.FDataId.FWhat.FVersion=aVer;
	_data.FDataId.FDestination.push_back(aTo);
	aBuf.release(); //optimization
	return MSendImpl(_data);
}
int CCustomer::_pimpl::MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf,NSHARE::version_t const& aVer,
		eSendToFlags)
{
	if (!FMainIO)
	{
		LOG(WARNING)<<"Cannot send to "<<aNumber<<" as library is not opened.";
		return static_cast<int>(ERROR_NOT_OPEN);
	}
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData=aBuf;
	_data.FDataId.FWhat.FNumber=aNumber;
	_data.FDataId.FWhat.FVersion=aVer;
	aBuf.release(); //optimization
	return MSendImpl(_data);
}
int CCustomer::_pimpl::MSendTo(required_header_t const& aNumber,
		NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuf, eSendToFlags)
{
	if (!FMainIO)
	{
		LOG(WARNING)<<"Cannot send to "<<aNumber<<" as library is not opened.";
		return static_cast<int>(ERROR_NOT_OPEN);
	}

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData=aBuf;
	_data.FDataId.FWhat=aNumber;
	_data.FDataId.FProtocol = aProtocolName;
	aBuf.release(); //optimization
	return MSendImpl(_data);
}
int CCustomer::_pimpl::MSendTo(required_header_t const& aNumber,
		NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuf,
		const NSHARE::uuid_t& aTo, eSendToFlags)
{
	if (!FMainIO)
	{
		LOG(WARNING)<<"Cannot send to "<<aTo<<" as library is not opened.";
		return static_cast<int>(ERROR_NOT_OPEN);
	}
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData = aBuf;
	_data.FDataId.FWhat = aNumber;
	_data.FDataId.FDestination.push_back(aTo);
	_data.FDataId.FProtocol = aProtocolName;
	aBuf.release(); //optimization
	return MSendImpl(_data);
}
int CCustomer::_pimpl::MSendImpl(user_data_t& aUserData)
{
	aUserData.FDataId.FRouting.FFrom=FMyId.FId;
	aUserData.FDataId.FPacketNumber=MNextUserPacketNumber();

	send_data_to_t _data;
	_data.FData = aUserData;
	aUserData.FData.release();
	CDataObject::sMGetInstance().MPush(_data);

	return aUserData.FDataId.FPacketNumber;
}
NSHARE::CBuffer CCustomer::_pimpl::MGetNewBuf(size_t aSize) const
{
	if (FMainIO)
		return FMainIO->MGetNewBuf(aSize);
	else
		return NSHARE::CBuffer(aSize);
}
CCustomer::customers_t CCustomer::_pimpl::MCustomers() const
{
	return FCustomers.MGetRAccess().MGet();
}
void CCustomer::_pimpl::MJoin()
{
	if(FMainIO)//!<\todo resource race (it can be removed before we call it)
		FMainIO->MJoin();
}

/** Checks for ingnoring events
 *
 * @param aEvent Event for checking
 * @return true if has to be ignored
 */
bool CCustomer::_pimpl::MIsEventIngnored(const CText& aEvent) const
{
	bool _is_ignoring=false;
	if ((aEvent == EVENT_CONNECTED && MIsConnected())
			|| (aEvent == EVENT_DISCONNECTED && !MIsConnected())) //
	{
		VLOG(2) << "Ingnoring event " << aEvent;
		_is_ignoring = true;
	}
	return _is_ignoring;
}
/** Add event to wait list
 *
 * @param aEvents Events for added
 * @param aAddedList list of added events
 * @return count of added or 0 if error
 */
unsigned CCustomer::_pimpl::MPutEvents(const NSHARE::Strings& aEvents,
		NSHARE::Strings* aAddedList)
{
	NSHARE::Strings& _putted_event(*aAddedList);
	unsigned _count = 0;

	NSHARE::Strings::const_iterator _it = aEvents.begin();
	for (; _it != aEvents.end(); ++_it)
	{
		const CText& _event(*_it);

		const bool _is_ignoring = MIsEventIngnored(_event);

		if (!_is_ignoring)
		{
			++_count;

			const bool _is = FWaitFor.insert(_event).second;
			if (_is)
				_putted_event.push_back(_event);
			else
				VLOG(1) << "Event " << _event << " has expected already";
		}
	}
	return _count;
}

/** Puts events to wait for list
 *
 * @param aEvents List of event
 * @return error code or 0
 */
int CCustomer::_pimpl::MAddEventToWaitList(NSHARE::Strings const& aEvents)
{
	int _error_code = 0;
	NSHARE::Strings _putted_event;

	unsigned const _count = MPutEvents(aEvents, &_putted_event);
	if (_count == 0)
		_error_code = static_cast<int>(ERROR_UNEXPECETED);

	if (!_putted_event.empty() && _error_code != 0)
	{
		NSHARE::Strings::const_iterator _rit(_putted_event.begin());
		for (; _rit != _putted_event.end(); ++_rit)
			FWaitFor.erase(*_rit);
	}
	return _error_code;
}

/** Gets if need to wait events in the list
 *
 * @param [in,out] aEvents event list
 * @return true if need
 */
bool CCustomer::_pimpl::MIsKeepOnWaiting(
		NSHARE::Strings* aEvents) const
{
	NSHARE::Strings& _events(*aEvents);

	NSHARE::Strings::iterator _it = _events.begin();
	for (; _it != _events.end();)
		if (FWaitFor.find(*_it) == FWaitFor.end())
			_it = _events.erase(_it);
		else
			++_it;
	return !_events.empty();
}

int CCustomer::_pimpl::MWaitForEvent(NSHARE::Strings  aEvents,double aSec)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutexWaitFor);
	int _rval=MAddEventToWaitList(aEvents);

	if (_rval == 0)
	{
		if (aSec < 0)
		{
			for (HANG_INIT;MIsKeepOnWaiting(&aEvents);HANG_CHECK)
			{
				FCondvarWaitFor.MTimedwait(&FMutexWaitFor);
			}
		}
		else
		{
			double const _cur_time=NSHARE::get_time();
			for (HANG_INIT;
					MIsKeepOnWaiting(&aEvents) && //
					aSec>(NSHARE::get_time()-_cur_time);//
			HANG_CHECK)
			{
				FCondvarWaitFor.MTimedwait(&FMutexWaitFor,aSec-(NSHARE::get_time()-_cur_time));

				NSHARE::Strings::iterator _it = aEvents.begin();
				for (; _it != aEvents.end(); )
					if(FWaitFor.find(*_it)==FWaitFor.end())
						_it=aEvents.erase(_it);
					else
						++_it;
			}
		}
	}
	return _rval;
}
uint16_t CCustomer::_pimpl::MNextUserPacketNumber()
{
	return ++FMainPacketNumber;
}
bool CCustomer::_pimpl::MAvailable(const NSHARE::CText& aModule) const
{
	CRAII<CMutex> _block(FCommonMutex);

	array_of_modules_t::const_iterator _to= FModules.begin();
	for(;_to!=FModules.end() && aModule!= (*_to)->MGetType(); ++_to)
		;
	return _to != FModules.end();
}
CCustomer::modules_t CCustomer::_pimpl::MAllAvailable() const
{
	modules_t _module;
	array_of_modules_t::const_iterator _to = FModules.begin();
	for (; _to != FModules.end(); ++_to)
		_module.push_back((*_to)->MGetType());
	return _module;
}
void CCustomer::_pimpl::MPutModule(IModule* aModule)
{
	CRAII<CMutex> _block(FCommonMutex);

	array_of_modules_t::iterator _to= std::lower_bound(FModules.begin(),
			FModules.end(), aModule);
	FModules.insert(_to, aModule);
}
void CCustomer::_pimpl::MPopModule(IModule* aModule)
{
	CRAII<CMutex> _block(FCommonMutex);
	FModules.erase(std::remove(FModules.begin(), FModules.end(), aModule), FModules.end());
}
ICustomer::array_of_modules_t CCustomer::_pimpl::MGetModules() const
{
	CRAII<CMutex> _block(FCommonMutex);
	return FModules;
}
IModule* CCustomer::_pimpl::MGetModule(const NSHARE::CText& aModule) const
{
	CRAII<CMutex> _block(FCommonMutex);

	array_of_modules_t::const_iterator _to = FModules.begin();
	for (; _to != FModules.end() && aModule != (*_to)->MGetType(); ++_to)
		;
	return _to == FModules.end()?NULL:*_to;
}

IRtc* CCustomer::_pimpl::MGetRTC(NSHARE::CText const& aName) const
{
	IRtc* _p=NULL;
	CRTCFactory::factory_its_t _its =
			CRTCFactory::sMGetInstance().MGetIterator();
	for (; _its.FBegin != _its.FEnd; ++_its.FBegin)
		if((_p=_its.FBegin->second->MGetRTC(name_rtc_t(aName)))!=NULL)
			break;
	VLOG_IF(2,_p==NULL)<<"Rtc is not founded";
	return _p;
}
CCustomer::rtc_list_t CCustomer::_pimpl::MGetListOfRTC() const
{
	std::vector<IRtc*> _rval;
	CRTCFactory::factory_its_t _its=CRTCFactory::sMGetInstance().MGetIterator();
	for(;_its.FBegin!=_its.FEnd;++_its.FBegin)
	{
		IRtcControl::array_of_RTC_t const _array(
				_its.FBegin->second->MGetAllRTC());
		_rval.insert(_rval.end(), _array.begin(),_array.end());
	}
	return _rval;
}

CDataObject& CCustomer::_pimpl::MGetDataObject() const
{
	return CDataObject::sMGetInstance();
}
CResources& CCustomer::_pimpl::MGetResourceObject() const
{
	return CResources::sMGetInstance();
}

CConfigure& CCustomer::_pimpl::MGetConfigureObject() const
{
	return CConfigure::sMGetInstance();
}
}
