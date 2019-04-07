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
#include <fdir.h>
#include <revision.h>
#include <programm_id.h>

#include <CCustomer.h>
#include <CCustomerImpl.h>
#include <CLocalChannelFactory.h>

namespace NUDT
{
using namespace NSHARE;
CCustomer::_pimpl::_pimpl(CCustomer& aThis) :
		my_t(&aThis), FThis(aThis), FWorker(NULL),  FMutexWaitFor(
				NSHARE::CMutex::MUTEX_NORMAL), FUniqueNumber(0), FMainPacketNumber(
				0)
{

}
int CCustomer::_pimpl::MInitialize(NSHARE::CText const& aProgram,
		NSHARE::CText const& aName, NSHARE::version_t const& aVersion)
{
	if (int _rval = MInitId(aProgram, aName, aVersion))
	{
		LOG_IF(ERROR,_rval!=0) << "Cannot initialize id as " << _rval;
		return _rval;
	}

	if (int _rval = MInitFactorys())
	{
		LOG_IF(ERROR,_rval!=0) << "Cannot initialize id as " << _rval;
		return _rval;
	}

	if (int _rval = MLoadLibraries())
	{
		LOG_IF(ERROR,_rval!=0) << "Cannot initialize id as " << _rval;
		return _rval;
	}

	if (int _rval = MInitCallBacks())
	{
		LOG_IF(ERROR,_rval!=0) << "Cannot initialize id as " << _rval;
		return _rval;
	}
	if (int _rval = MUdpateRecvList())
	{
		LOG_IF(ERROR,_rval!=0) << "Cannot initialize id as " << _rval;
		return _rval;
	}

	return 0;
}
CCustomer::_pimpl::~_pimpl()
{
	//fixme remove cb
	MClose();
}

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
void CCustomer::_pimpl::sMInitConfigure(const NSHARE::CConfig& aConf)
{
	if (CConfigure::sMGetInstancePtr())
		return;

	VLOG(1) << "Read Internal config " << aConf;

	new CConfigure();

	CConfigure::sMGetInstance().MGet().MMerge(aConf);
	VLOG(1) << "Final config " << CConfigure::sMGetInstance().MGet();
}

int CCustomer::_pimpl::MLoadLibraries()
{
	if (CResources::sMGetInstancePtr())
		return 0;
	//MODULE
	CConfig const* const _p = CConfigure::sMGetInstance().MGet().MChildPtr(MODULES);
	std::vector<NSHARE::CText> _text;

#ifndef CUSTOMER_WITH_STATIC_MODULES
	LOG_IF(DFATAL,!_p) << "Invalid config file.Key " << MODULES
	<< " is not exist.";

	if (!_p)
	return -static_cast<int>(E_CONFIGURE_IS_INVALID);
	ConfigSet::const_iterator _it = _p->MChildren().begin();

	for (; _it != _p->MChildren().end(); ++_it)
	_text.push_back((_it)->MKey());
#endif

	NSHARE::CText _ext_path;
	CConfigure::sMGetInstance().MGet().MGetIfSet(MODULES_PATH,_ext_path);

	new CResources(_text,_ext_path);

	CResources::sMGetInstance().MLoad();
	return 0;
}
int CCustomer::_pimpl::MInitFactorys()
{

	//init factories
	if (!CIOFactory::sMGetInstancePtr())
		new CIOFactory();

	if (!CLocalChannelFactory::sMGetInstancePtr())
		new CLocalChannelFactory();
	if (!CDataObject::sMGetInstancePtr())
		new CDataObject();
	return 0;
}

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
	return 0;
}

bool CCustomer::_pimpl::MIsOpened() const
{
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";

	return FWorker && FWorker->MIsOpened();
}
bool CCustomer::_pimpl::MIsConnected() const
{
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(4) << "Our turn.";

	return FWorker && FWorker->MIsConnected();
}
bool CCustomer::_pimpl::MOpen()
{
	VLOG(2) << "Open customer";

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";

	if (!FWorker)
	{
		CText _name(DEFAULT_IO_MANAGER);
		bool _val = CConfigure::sMGetInstance().MGet().MGetIfSet(DOING_MODULE,
				_name);
		LOG_IF(WARNING, !_val)
										<< "The operative module is not present in the config file";

		FWorker = CIOFactory::sMGetInstance().MGetFactory(_name);
		LOG_IF(FATAL, !FWorker) << "The module " << _name << " is not exist.";
		FWorker->MInit(this);
	}
	if (!FWorker)
		return false;

	bool _result = false;
	NSHARE::CThread::eThreadPriority _priority =
			NSHARE::CThread::THREAD_PRIORITY_DEFAULT;
	if (CConfigure::sMGetInstance().MGet().MGetIfSet(THREAD_PRIORITY,
			_priority))
	{
		NSHARE::CThread::param_t _param;
		_param.priority = _priority;
		_result = FWorker->MOpen(&_param);
//		if (_result)
//			FThread.MCreate(&_param);
	}
	else
	{
		_result = FWorker->MOpen();
//		if (_result)
//			FThread.MCreate();
	}
	return _result;

//	return true;
}
void CCustomer::_pimpl::MClose()
{
	VLOG(2) << "Close customer";

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";

	if (FWorker)
	{
		VLOG(2) << "Close worker";
		FWorker->MClose();
		delete FWorker;
//		FThread.MCancel();
		VLOG(2) << "EOK";
	}
}
bool CCustomer::_pimpl::MAvailable(const NSHARE::CText& aModule) const
{
	if (IIOConsumer* _p = CIOFactory::sMGetInstance().MGetFactory(aModule))
		return _p->MIsAvailable();

	LOG(INFO)<< "The IO module " << aModule << " is not exist.";
	return false;
}
bool CCustomer::_pimpl::MAvailable() const
{
	CText _name;
	bool _val = CConfigure::sMGetInstance().MGet().MGetIfSet(DOING_MODULE,
			_name);
	LOG_IF(ERROR, !_val)
									<< "The operative module is not present in the config file";
	if (!_val)
		return false;

	return MAvailable(_name);
}
CCustomer::modules_t CCustomer::_pimpl::MAllAvailable() const
{
	modules_t _module;
	CIOFactory::factory_its_t _its = CIOFactory::sMGetInstance().MGetIterator();
	for (; _its.FBegin != _its.FEnd; ++_its.FBegin)
		if (_its.FBegin->second->MIsAvailable())
			_module.push_back(_its.FBegin->first);
	return _module;
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

	CHECK_EQ(aWhat->FType, recv_data_from_t::NAME);
	reinterpret_cast<_pimpl*>(aData)->MReceiver(
			*reinterpret_cast<recv_data_from_t*>(aWhat->FPointToData));
	return 0;
}
void CCustomer::_pimpl::MReceiver(recv_data_from_t & aFrom)
{
	VLOG(1) << "Thread data handler started.";
	LOG_IF(ERROR,!FWorker) << " There is not working module";
	received_message_args_t _raw_args;

	{ //filling _raw_args
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
	{ //handle _raw_args
		CRAII<CMutex> _block(FParserMutex);
		VLOG(1) << "Handle data from " << _raw_args.FFrom << " ; size:"
							<< _raw_args.FMessage.FBuffer.size() << " Packet # "
							<< _raw_args.FPacketNumber;

		int _count = 0;
		_count += MCallImpl(EVENT_RAW_DATA, &_raw_args);

		if (!aFrom.FData.FDataId.FEventsList.empty() && !FEvents.empty())
		{
			std::vector<demand_dg_t::event_handler_t>::const_iterator _it =
					aFrom.FData.FDataId.FEventsList.begin(), _it_end(
							aFrom.FData.FDataId.FEventsList.end());

			for (; _it != _it_end; ++_it)
			{
//				CHECK_LT(_it->FVal, FDemands.size());
//				CHECK_LT(_it->FVal,FEvents.size());
//				CHECK_EQ(_it->FVal,FDemands[_it->FVal].FHandler);

				cb_event_t::const_iterator _jt = FEvents.find(*_it);
				if (_jt != FEvents.end())
				{
					LOG(INFO)<< "Handling #"<<_raw_args.FPacketNumber<<" by "
					<<_raw_args.FProtocolName<<" protocol from "<<_raw_args.FFrom<<" Raw="<<_raw_args.FHeader.FNumber<<" by CB #"<<(*_it);

					FEvents[*_it].operator ()(&FThis, &_raw_args);
					if(!_raw_args.FOccurUserError)
					{
						VLOG(2) << "The packet #" << _raw_args.FPacketNumber
						<< " by " << _raw_args.FProtocolName
						<< " protocol from "
						<< _raw_args.FFrom << " handled";
						++_count;
					}
					else
					{
						LOG(ERROR)<<"The packet #" << _raw_args.FPacketNumber
								<< " by " << _raw_args.FProtocolName
								<< " protocol from "
								<< _raw_args.FFrom << " is not handled as "<<(int)_raw_args.FOccurUserError;
						fail_send_t _fsent(aFrom.FData.FDataId);
						_fsent.FRouting.clear();
						_fsent.FRouting.push_back(get_my_id().FId.FUuid);
						_fsent.MSetUserError(_raw_args.FOccurUserError);
						fail_send_id_from_me_t _sent;
						_sent.FData=_fsent;
						CDataObject::sMGetInstance().MPush(_sent);
						LOG_IF(DFATAL,_count!=0)<<"Some handlers were handled packet #"<< _raw_args.FPacketNumber<<", but the error "
								<<_raw_args.FOccurUserError<<" is occurred in handler "<<*_it;
						break;
					}

				}
				else
				{
					LOG(ERROR)<<" CB "<<(*_it)<<" is not founded. Ignoring";
				}
			}

			LOG_IF(ERROR,_count==0) << "It does not expect packet from "
											<< _raw_args.FFrom
											<< ". Ignoring ...";

		}

		LOG_IF(INFO,_count==0) << _raw_args.FMessage.
				FBuffer.size()
										<< " bytes of  data from "
										<< _raw_args.FFrom
										<< " is not handled.";
	}
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

	CHECK_EQ(FDemands.size(), FEvents.size());

	VLOG(2) << "Our turn.";
	size_t _i = 0;
	size_t const _size = FDemands.size();
	for (; _i != _size && !(FDemands[_i] == _val); ++_i)
		;

	if (_i == _size)
	{
		LOG(INFO)<< "Add additional parser for channel '" << aNumber.FRequired << "' by "<< aNumber.FProtocolName<<"'";

		_val.FHandler=++FUniqueNumber;

		FDemands.push_back(_val);
		FEvents[_val.FHandler]=aHandler;
	}
	else
	{
		LOG(WARNING) << "Replace handler "<<FEvents[(uint32_t)_i]<< " to " << aHandler;
		CHECK(FEvents.find(_val.FHandler)!=FEvents.end());
		FEvents[_val.FHandler]=aHandler;
	}

	VLOG_IF(2,!FWorker) << " The channel has not opened yet.";
	MUdpateRecvList();

	CHECK_EQ(FDemands.size(), FEvents.size());
	return _val.FHandler;
}
int CCustomer::_pimpl::MRemoveDgParserFor( demand_dg_t::event_handler_t  aNumber)
{
	size_t _i = 0;
	size_t const _size = FDemands.size();
	for (; _i != _size && !(FDemands[_i].FHandler == aNumber); ++_i)
		;
	if (_i == _size)
	{
		return ERROR_HANDLER_IS_NOT_EXIST;
	}
	const uint32_t _handler = FDemands[_i].FHandler;
	FEvents.erase(_handler);
	FDemands.erase(FDemands.begin() + _i);

	VLOG_IF(2,!FWorker) << " The channel has not opened yet.";
	MUdpateRecvList();

	CHECK_EQ(FDemands.size(), FEvents.size());
	return _handler;
}
int CCustomer::_pimpl::MRemoveDgParserFor(
		requirement_msg_info_t aNumber)
{
	const NSHARE::CText& aReq=aNumber.FFrom;
	VLOG(2) << "Remove parser for " << aReq;

	if (aNumber.FProtocolName.empty())
		aNumber.FProtocolName = RAW_PROTOCOL_NAME;

	aNumber.FProtocolName.MToLowerCase();
	CRAII<CMutex> _block(FParserMutex);

	VLOG(2) << "Our turn.";
	CHECK_EQ(FDemands.size(), FEvents.size());

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
	const uint32_t _handler = FDemands[_i].FHandler;
	FEvents.erase(_handler);
	FDemands.erase(FDemands.begin() + _i);

	VLOG_IF(2,!FWorker) << " The channel has not opened yet.";
	MUdpateRecvList();

	CHECK_EQ(FDemands.size(), FEvents.size());
	return _handler;
}
void CCustomer::_pimpl::MGetMyWishForMSG(std::vector<request_info_t>& aTo) const
{
	CRAII<CMutex> _block(FParserMutex);

	size_t const _size = FDemands.size();
	for (size_t i = 0; i != _size;++i)
	{
		request_info_t _msg;
		_msg.FWhat.FFlags=FDemands[i].FFlags.MGetMask();
		_msg.FWhat.FProtocolName=FDemands[i].FProtocol;
		_msg.FWhat.FRequired=FDemands[i].FWhat;
		_msg.FWhat.FFrom=FDemands[i].FNameFrom.MGetRawName();
		_msg.FHandler=FEvents.find(FDemands[i].FHandler)->second;
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
	if (!FWorker)
	{
		LOG(WARNING)<<"Cannot send to "<<aTo<<" as library is not opened.";
		return static_cast<int>(ERROR_NOT_OPEN);
	}

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData=aBuf;
	_data.FDataId.FProtocol=aProtocolName;

	return MSendImpl(aBuf,_data);
}
int CCustomer::_pimpl::MSendTo(const NSHARE::CText& aProtocolName,
		NSHARE::CBuffer& aBuf, const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{
	if (!FWorker)
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

	return MSendImpl(aBuf,_data);
}
int CCustomer::_pimpl::MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf,
		const NSHARE::uuid_t& aTo,NSHARE::version_t const& aVer, eSendToFlags)
{

	if (!FWorker)
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

	return MSendImpl(aBuf,_data);
}
int CCustomer::_pimpl::MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf,NSHARE::version_t const& aVer,
		eSendToFlags)
{
	if (!FWorker)
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

	return MSendImpl(aBuf,_data);
}
int CCustomer::_pimpl::MSendTo(required_header_t const& aNumber,
		NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuf, eSendToFlags)
{
	if (!FWorker)
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

	return MSendImpl(aBuf,_data);
}
int CCustomer::_pimpl::MSendTo(required_header_t const& aNumber,
		NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuf,
		const NSHARE::uuid_t& aTo, eSendToFlags)
{

	if (!FWorker)
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

	return MSendImpl(aBuf,_data);
}
int CCustomer::_pimpl::MSendImpl( NSHARE::CBuffer & aBuf,user_data_t& _data)
{
	aBuf.release(); //optimization
	_data.FDataId.FRouting.FFrom=FMyId.FId;
	_data.FDataId.FPacketNumber=MNextUserPacketNumber();

	int _number = FWorker->MSend(_data);
	LOG_IF(INFO,_number>=0) << FMyId.FId.FUuid << " sent packet #" << _number;
	if(_number<0)
	{
		//some error, repair buffer
		aBuf=_data.FData;
		return _number;
	}
	return _data.FDataId.FPacketNumber;
}
NSHARE::CBuffer CCustomer::_pimpl::MGetNewBuf(size_t aSize) const
{
	if (FWorker)
		return FWorker->MGetNewBuf(aSize);
	else
		return NSHARE::CBuffer(aSize);
}
CCustomer::customers_t CCustomer::_pimpl::MCustomers() const
{
	return FCustomers.MGetRAccess().MGet();
}
void CCustomer::_pimpl::MJoin()
{
	if(FWorker)//!<\todo resource race (it can be removed before we call it)
		FWorker->MJoin();
}
int CCustomer::_pimpl::MWaitForEvent(NSHARE::CText const& aEvent,double aSec)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutexWaitFor);
	bool const _is=FWaitFor.insert(aEvent).second;
	if(!_is)
		return static_cast<int>(ERROR_UNEXPECETED);
	if(
			(aEvent==EVENT_CONNECTED && MIsConnected()) ||
			(aEvent==EVENT_DISCONNECTED && !MIsConnected())
		)
		return static_cast<int>(ERROR_NOT_CONNECTED_TO_KERNEL);
	if(aSec<0)
	{
		for (HANG_INIT;FWaitFor.find(aEvent)!=FWaitFor.end();HANG_CHECK)
			FCondvarWaitFor.MTimedwait(&FMutexWaitFor);
	}else
	{
		double const _cur_time=NSHARE::get_time();
		for (HANG_INIT;FWaitFor.find(aEvent)!=FWaitFor.end() &&//
					  aSec>(NSHARE::get_time()-_cur_time);
			HANG_CHECK)
			FCondvarWaitFor.MTimedwait(&FMutexWaitFor,aSec-(NSHARE::get_time()-_cur_time));
	}

	return 0;
}
uint16_t CCustomer::_pimpl::MNextUserPacketNumber()
{
	return ++FMainPacketNumber;
}
}
