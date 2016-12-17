/*
 * CCustomer.cpp
 *
 *  Created on: 19.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
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

#include "CResources.h"
#include "CConfigure.h"
#include "CCustomer.h"
#include "CIOFactory.h"
#include "CDataObject.h"
#include "CCustomerImpl.h"
#include "CLocalChannelFactory.h"

DECLARATION_VERSION_FOR(customer)

static const NSHARE::version_t g_version(MAJOR_VERSION_OF (customer),
		MINOR_VERSION_OF (customer), REVISION_OF (customer));
using namespace NSHARE;

template<>
NUDT::CCustomer::singleton_pnt_t NUDT::CCustomer::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CCustomer::DEFAULT_IO_MANAGER = "tcp_client_io_manager";
const NSHARE::CText CCustomer::RAW_PROTOCOL = RAW_PROTOCOL_NAME;
const NSHARE::CText CCustomer::ENV_CONFIG_PATH = "UDT_CUSTOMER_CONFIG_PATH";
const NSHARE::CText CCustomer::CONFIG_PATH = "path";
const NSHARE::CText CCustomer::MODULES = "modules";
const NSHARE::CText CCustomer::MODULES_PATH = "modules_path";
const NSHARE::CText CCustomer::DOING_MODULE = "worker";
const NSHARE::CText CCustomer::RRD_NAME = "rrdname";
const NSHARE::CText CCustomer::THREAD_PRIORITY = "priority";
const NSHARE::CText CCustomer::NUMBER_OF_THREAD = "num_of_thread";

const NSHARE::CText CCustomer::EVENT_RAW_DATA = "event_raw_data";
const NSHARE::CText CCustomer::EVENT_CONNECTED = "event_connected";
const NSHARE::CText CCustomer::EVENT_DISCONNECTED = "event_disconnected";
;
const NSHARE::CText CCustomer::EVENT_CUSTOMERS_UPDATED =
		"event_customers_updated"; //+
const NSHARE::CText CCustomer::EVENT_FAILED_SEND = "event_failed_send";
const NSHARE::CText CCustomer::EVENT_NEW_RECEIVER = "event_receiver";

CCustomer::_pimpl::_pimpl(CCustomer& aThis) :
		my_t(&aThis), FThis(aThis), FWorker(NULL), FIsReady(false), FMutexWaitFor(
				NSHARE::CMutex::MUTEX_NORMAL), FUniqueNumber(0)
{

}
int CCustomer::_pimpl::MInitialize(NSHARE::CText const& aProgram,
		NSHARE::CText const& aName)
{
	if (int _rval = MInitId(aProgram, aName))
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
		NSHARE::CText const& aName)
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
			_rval = E_NO_CUSTOMER_NAME;
			LOG(ERROR)<<"Invalid customer name";
		}
	}
	if (_rval == 0)
	{
		if (init_id((char*) _name.c_str(), E_CONSUMER, g_version) != 0)
		{
			_rval = E_INVALID_NAME;
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
	CConfig const* _p = CConfigure::sMGetInstance().MGet().MChildPtr(MODULES);
	std::vector<NSHARE::CText> _text;

#ifndef CUSTOMER_WITH_STATIC_MODULES
	LOG_IF(DFATAL,!_p) << "Invalid config file.Key " << MODULES
	<< " is not exist.";

	if (!_p)
	return E_INVALID_CONFIG;
	ConfigSet::const_iterator _it = _p->MChildren().begin();

	for (; _it != _p->MChildren().end(); ++_it)
	_text.push_back(_it->MKey());
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
		_param.FPrior = _priority;
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
	_fail.FPacketNumber = _prog.FPacketNumber;
	_fail.FProtocolName = _prog.FProtocol;
	_fail.FTo = _prog.FUUIDTo.MGetConst();
	_impl->MCall(EVENT_FAILED_SEND, &_fail);
	VLOG(2) << "Fail sent";
	return 0;

}
int CCustomer::_pimpl::sMDemands(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	CHECK_EQ(aWhat->FType, demand_dgs_id_t::NAME);
	_pimpl* _impl = reinterpret_cast<_pimpl*>(aData);
	demand_dgs_t const& _prog =
			reinterpret_cast<demand_dgs_id_t*>(aWhat->FPointToData)->FData;
	new_receiver_args_t _arg;
	_arg.FReceivers.reserve(_prog.size());
	demand_dgs_t::const_iterator _it = _prog.begin(), _it_end(_prog.end());

	for (; _it != _it_end; ++_it)
	{
		new_receiver_args_t::what_t _what;
		_what.FRegExp = _it->FNameFrom.MGetRawName();
		_what.FWhat.FProtocolName = _it->FProtocol;
		_what.FWhat.FRequired = _it->FWhat;
		if (_it->FUUIDFrom.MIs())
			_what.FWho = _it->FUUIDFrom.MGetConst();
		_arg.FReceivers.push_back(_what);
	}
	int const _count = _impl->MCall(EVENT_NEW_RECEIVER, &_arg);
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

	_impl->MCall(EVENT_CUSTOMERS_UPDATED, &_args);
	VLOG(2) << "New Customers array";
	return 0;
}
void CCustomer::_pimpl::MEventConnected()
{
	VLOG(2) << "Connected ";
	int _val = MCall(EVENT_CONNECTED, NULL);
	VLOG(2) << "Count " << _val;
}
void CCustomer::_pimpl::MEventDisconnected()
{
	VLOG(2) << "Disconnected";
	int _val = MCall(EVENT_DISCONNECTED, NULL);
	VLOG(2) << "Count " << _val;
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
	args_t _raw_args;

	{ //filling _raw_args
		VLOG(4) << "Handle data from " << aFrom.FData.FDataId.FFrom
							<< " ; size:" << aFrom.FData.FData.size()
							<< " Packet # "
							<< aFrom.FData.FDataId.FPacketNumber;
		_raw_args.FPacketNumber = aFrom.FData.FDataId.FPacketNumber;
		_raw_args.FFrom = MGetIdFor(aFrom.FData.FDataId.FFrom.FUuid); //may be remove?
		_raw_args.FProtocolName =
				aFrom.FData.FDataId.FProtocol.empty() ?
						RAW_PROTOCOL_NAME : aFrom.FData.FDataId.FProtocol;
		_raw_args.FRawProtocolNumber = aFrom.FData.FDataId.FRawProtocolNumber;

		aFrom.FData.FData.MMoveTo(_raw_args.FBuffer);

		LOG_IF(ERROR, _raw_args.FBuffer.empty()) << "Empty data";

		if (!_raw_args.FBuffer.empty())
		{
			_raw_args.FBegin =
					reinterpret_cast<const uint8_t*>(&_raw_args.FBuffer.front());
			_raw_args.FEnd =
					reinterpret_cast<const uint8_t*>(&_raw_args.FBuffer.back());
			++_raw_args.FEnd; //last equal end
		}
		else
			_raw_args.FBegin = _raw_args.FEnd = NULL;
	}
	{ //handle _raw_args
		CRAII<CMutex> _block(FParserMutex);
		VLOG(1) << "Handle data from " << _raw_args.FFrom << " ; size:"
							<< _raw_args.FBuffer.size() << " Packet # "
							<< _raw_args.FPacketNumber;

		int _count = 0;
		_count += MCall(EVENT_RAW_DATA, &_raw_args);

		_count += MCall(_raw_args.FFrom.FName, &_raw_args);

		NSHARE::CRegistration const _r(_raw_args.FFrom.FName);
		NSHARE::CAddress _addr(_r.MGetAddress());

		if (!_addr.MIsEmpty())
		{
			//calling CB for subaddress
			NSHARE::CText const _name(_r.MGetName());
			for (; !_addr.MIsEmpty(); _addr.MRemoveLastPath())
			{
				_count += MCall(
						NSHARE::CRegistration(_name, _addr).MGetRawName(),
						&_raw_args);
			}
			_count += MCall(_name, &_raw_args);
		}

		if (aFrom.FData.FDataId.FUUIDTo.MIs() && !FEvents.empty())
		{
			uuids_t::const_iterator _it =
					aFrom.FData.FDataId.FUUIDTo.MGetConst().begin(), _it_end(
					aFrom.FData.FDataId.FUUIDTo.MGetConst().end());

			for (; _it != _it_end; ++_it)
			{
//				CHECK_LT(_it->FVal, FDemands.size());
//				CHECK_LT(_it->FVal,FEvents.size());
//				CHECK_EQ(_it->FVal,FDemands[_it->FVal].FHandler);

				cb_event_t::const_iterator _jt = FEvents.find(_it->FVal);
				if (_jt != FEvents.end())
				{
					LOG(INFO)<< "Handling #"<<_raw_args.FPacketNumber<<" by "
					<<_raw_args.FProtocolName<<" protocol from "<<_raw_args.FFrom.FName<<" Raw="<<_raw_args.FRawProtocolNumber<<" by CB #"<<_it->FVal;

					FEvents[_it->FVal].operator ()(&FThis, &_raw_args);

					VLOG(2) << "The packet #" << _raw_args.FPacketNumber
					<< " by " << _raw_args.FProtocolName
					<< " protocol from "
					<< _raw_args.FFrom.FName << " handled";
					++_count;
				}
				else
				{
					LOG(ERROR)<<" CB "<<_it->FVal<<" is not founded. Ignoring";
				}
			}

			LOG_IF(ERROR,_count==0) << "It does not expect packet from "
											<< _raw_args.FFrom.FName
											<< ". Ignoring ...";

		}

		LOG_IF(INFO,_count==0) << _raw_args.FBuffer.size()
										<< " bytes of  data from "
										<< _raw_args.FFrom
										<< " is not handled.";
	}
}
id_t const CCustomer::_pimpl::MGetIdFor(NSHARE::uuid_t const& aUUID) const
{
	safety_customers_t::RAccess<> const _acc = FCustomers.MGetRAccess();
	customers_t const& _cus = _acc.MGet();
	program_id_t _id;
	_id.FId.FUuid = aUUID;
	customers_t::const_iterator _it = _cus.find(_id);
	LOG_IF(ERROR,_it==_cus.end()) << "Cannot find " << aUUID;
	return _it != _cus.end() ? _it->FId : id_t();
}
int CCustomer::_pimpl::MSettingDgParserFor(const NSHARE::CText& aReq,
		dg_parser_t aNumber, const callback_t& aHandler)
{
	VLOG(2) << "Setting for " << aReq << " parser";

	if (aNumber.FProtocolName.empty())
		aNumber.FProtocolName = RAW_PROTOCOL_NAME;

	aNumber.FProtocolName.MToLowerCase();

	demand_dgs_t::value_type _val;
	_val.FNameFrom = NSHARE::CRegistration(aReq);
	_val.FProtocol = aNumber.FProtocolName;
	_val.FWhat = aNumber.FRequired;

	CRAII<CMutex> _block(FParserMutex);

	CHECK_EQ(FDemands.size(), FEvents.size());

	VLOG(2) << "Our turn.";
	unsigned _i = 0;
	unsigned const _size = FDemands.size();
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
		LOG(WARNING) << "Replace handler "<<FEvents[_i]<< " to " << aHandler;
		CHECK(FEvents.find(_val.FHandler)!=FEvents.end());
		FEvents[_val.FHandler]=aHandler;
	}

	VLOG_IF(2,!FWorker) << " The channel has not opened yet.";
	MUdpateRecvList();

	CHECK_EQ(FDemands.size(), FEvents.size());
	return _val.FHandler;
}
int CCustomer::_pimpl::MRemoveDgParserFor(const NSHARE::CText& aReq,
		dg_parser_t aNumber)
{
	VLOG(2) << "Remove parser for " << aReq;

	if (aNumber.FProtocolName.empty())
		aNumber.FProtocolName = RAW_PROTOCOL_NAME;

	aNumber.FProtocolName.MToLowerCase();
	CRAII<CMutex> _block(FParserMutex);

	VLOG(2) << "Our turn.";
	CHECK_EQ(FDemands.size(), FEvents.size());

	demand_dgs_t::value_type _val;
	_val.FNameFrom = NSHARE::CRegistration(aReq);
	_val.FProtocol = aNumber.FProtocolName;
	_val.FWhat = aNumber.FRequired;

	unsigned _i = 0;
	unsigned const _size = FDemands.size();
	for (; _i != _size && !(FDemands[_i] == _val); ++_i)
		;
	if (_i == _size)
	{
		return E_HANDLER_IS_NOT_EXIST;
	}
	const uint32_t _handler = FDemands[_i].FHandler;
	FEvents.erase(_handler);
	FDemands.erase(FDemands.begin() + _i);

	VLOG_IF(2,!FWorker) << " The channel has not opened yet.";
	MUdpateRecvList();

	CHECK_EQ(FDemands.size(), FEvents.size());
	return _handler;
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
		return E_NOT_OPEND;
	}

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData=aBuf;
	aBuf.release(); //optimization

	_data.FDataId.FProtocol=aProtocolName;
	//_data.FDataId.FDestName.push_back(aTo);
	int _number= FWorker->MSend(_data);
	LOG_IF(INFO,_number>0) << FMyId.FId.FUuid << " sent packet #" << _number;
	if(_number<0)
	{
		//some error, repair buffer
		aBuf=_data.FData;
	}
	return _number;
}
int CCustomer::_pimpl::MSendTo(const NSHARE::CText& aProtocolName,
		NSHARE::CBuffer& aBuf, const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{
	if (aTo == FMyId.FId.FUuid)
	{
		VLOG(1) << "Loopback send";
		recv_data_from_t _data;
		_data.FData.FData = aBuf;
		aBuf.release();
		_data.FData.FDataId.FFrom = FMyId.FId;
		CDataObject::sMGetInstance().MPush(_data);
		return 0;
	}
	if (!FWorker)
	{
		LOG(WARNING)<<"Cannot send to "<<aTo<<" as library is not opened.";
		return E_NOT_OPEND;
	}

	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData = aBuf;
	aBuf.release(); //optimization
	_data.FDataId.FProtocol = aProtocolName;
	_data.FDataId.FUUIDTo.MGet().push_back(aTo);
	int _number = FWorker->MSend(_data);
	LOG_IF(INFO,_number>0) << FMyId.FId.FUuid << " sent packet #" << _number;
	if (_number < 0)
	{
		//some error, repair buffer
		aBuf = _data.FData;
	}
	return _number;
}
int CCustomer::_pimpl::MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf,
		const NSHARE::uuid_t& aTo, eSendToFlags)
{
	if (aTo == FMyId.FId.FUuid)
	{
		VLOG(1) << "Loopback send";
		recv_data_from_t _data;
		_data.FData.FData = aBuf;
		aBuf.release();
		_data.FData.FDataId.FFrom = FMyId.FId;
		CDataObject::sMGetInstance().MPush(_data);
		return 0;
	}
	if (!FWorker)
	{
		LOG(WARNING)<<"Cannot send to "<<aTo<<" as library is not opened.";
		return E_NOT_OPEND;
	}
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData = aBuf;
	aBuf.release(); //optimization
	_data.FDataId.FRawProtocolNumber = aNumber;
	_data.FDataId.FUUIDTo.MGet().push_back(aTo);
	int _number = FWorker->MSend(_data);
	LOG_IF(INFO,_number>0) << FMyId.FId.FUuid << " sent packet #" << _number;
	if (_number < 0)
	{
		//some error, repair buffer
		aBuf = _data.FData;
	}
	return _number;
}
int CCustomer::_pimpl::MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf,
		eSendToFlags)
{
	if (!FWorker)
	{
		LOG(WARNING)<<"Cannot send to "<<aNumber<<" as library is not opened.";
		return E_NOT_OPEND;
	}
	CRAII<CMutex> _block(FCommonMutex);
	VLOG(2) << "Our turn.";
	user_data_t _data;
	_data.FData=aBuf;
	aBuf.release(); //optimization

	_data.FDataId.FRawProtocolNumber=aNumber;
	//_data.FDataId.FDestName.push_back(aTo);
	int _number= FWorker->MSend(_data);
	LOG_IF(INFO,_number>0) << FMyId.FId.FUuid << " sent packet #" << _number;
	if(_number<0)
	{
		//some error, repair buffer
		aBuf=_data.FData;
	}
	return _number;
}

NSHARE::CBuffer CCustomer::_pimpl::MGetNewBuf(unsigned aSize) const
{
	if (FWorker)
		return FWorker->MGetNewBuf(aSize);
	else
		return NSHARE::CBuffer();
}
CCustomer::customers_t CCustomer::_pimpl::MCustomers() const
{
	return FCustomers.MGetRAccess().MGet();
}
void CCustomer::_pimpl::MWaitForReady(double aSec)
{
	NSHARE::CRAII<NSHARE::CMutex> _lock(FMutexWaitFor);
	for (HANG_INIT;!FIsReady;HANG_CHECK)
	FCondvarWaitFor.MTimedwait(&FMutexWaitFor,aSec);

}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
CCustomer::CCustomer() :
		FImpl(new _pimpl(*this))
{
}
int CCustomer::MInitialize(NSHARE::CText const& aProgram,
		NSHARE::CText const& aName)
{
	CHECK_NOTNULL(FImpl);
	return FImpl->MInitialize(aProgram, aName);
}
CCustomer::~CCustomer()
{
	delete FImpl;
}

const program_id_t& CCustomer::MGetID() const
{
	return FImpl->FMyId;
}
int CCustomer::sMInit(int argc, char* argv[], char const* aName)
{
	return sMInit(argc, argv, aName, NSHARE::CConfig());
}
int CCustomer::sMInit(int argc, char* argv[], char const* aName,
		const NSHARE::CText& aConf)
{
	NSHARE::CConfig _conf;

	std::fstream _stream;
	_stream.open(aConf.c_str());
	LOG_IF(DFATAL,!_stream.is_open()) << "***ERROR***:Configuration file - "
												<< aConf << ".";
	if (!_stream.is_open())
	{
		if(const char* envModuleDir = getenv(ENV_CONFIG_PATH.c_str()))
		{
			_stream.open(envModuleDir);
			LOG_IF(DFATAL,!_stream.is_open())
														<< "***ERROR***:Cannot open configuration file from Environment "
														<< ENV_CONFIG_PATH
														<< " - " << aConf << ".";
		}
	}
	int _rval=0;

	if (_stream.is_open())
	{
		if (aConf.find_last_of(".xml") != NSHARE::CText::npos)
		{
			LOG(INFO)<<"Initialize from xml configuration file "<<aConf;
			_rval=_conf.MFromXML(_stream)?_rval:E_INVALID_CONFIG;
		}
		else if(aConf.find_last_of(".json")!=NSHARE::CText::npos)
		{
			LOG(INFO)<<"Initialize from json configuration file "<<aConf;
			_rval=_conf.MFromJSON(_stream)?_rval:E_INVALID_CONFIG;
		}
		else
		{
			LOG(DFATAL)<<"Unknown file format "<<aConf;
			_rval= E_CANNOT_OPEN_CONFIG;
		}
		_stream.close();
	}else
		_rval=E_CANNOT_OPEN_CONFIG;

	if(_rval==0)
		_rval= sMInit(argc, argv, aName, _conf);

	return _rval;
}
int CCustomer::sMInit(int argc, char* argv[], NSHARE::CText const& aName,
		const NSHARE::CConfig& aConf)
{
#ifdef _WIN32
	WSADATA _data;
	WSAStartup(MAKEWORD(2,2), &_data);
#endif
	//init trace
	init_trace(argc, argv);
	//read config
	_pimpl::sMInitConfigure(aConf);
	//fixme to constructor
	//init rrd client
	CCustomer* _p = new CCustomer();
	const int _rval = _p->MInitialize(argv[0], aName);
	if (_rval != 0)
		delete _p;
	return _rval;
}
void CCustomer::sMFree()
{
	delete CCustomer::sMGetInstancePtr();
	//TODO
}
bool CCustomer::MIsOpened() const
{
	return FImpl->MIsOpened();
}
bool CCustomer::MIsConnected() const
{
	return FImpl->MIsConnected();
}
bool CCustomer::MOpen()
{
	return FImpl->MOpen();
}

void CCustomer::MClose()
{
	FImpl->MClose();
}

bool CCustomer::MAvailable(const NSHARE::CText& aModule) const
{
	return FImpl->MAvailable(aModule);
}
bool CCustomer::MAvailable() const
{
	return FImpl->MAvailable();
}
CCustomer::modules_t CCustomer::MModules() const
{
	return FImpl->MAllAvailable();
}

int CCustomer::MSettingDgParserFor(const NSHARE::CText& aFrom,
		const unsigned& aHeader, const callback_t& aCB)
{
	dg_parser_t _msg;
	_msg.FRequired.FVersion = g_version;
	_msg.FRequired.FNumber = aHeader;
	_msg.FProtocolName = "";
	//It's not need

	return MSettingDgParserFor(aFrom, _msg, aCB);
}
int CCustomer::MSettingDgParserFor(const NSHARE::CText& aTo,
		const dg_parser_t& aNumber, const callback_t& aHandler)
{
	return FImpl->MSettingDgParserFor(aTo, aNumber, aHandler);
}
int CCustomer::MRemoveDgParserFor(const NSHARE::CText& aFrom,
		const unsigned& aNumber)
{
	dg_parser_t _msg;
	_msg.FRequired.FVersion = g_version;
	_msg.FRequired.FNumber = aNumber;
	return MRemoveDgParserFor(aFrom, _msg);
}
int CCustomer::MRemoveDgParserFor(const NSHARE::CText& aTo,
		const dg_parser_t& aNumber)
{
	return FImpl->MRemoveDgParserFor(aTo, aNumber);
}
int CCustomer::MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
		eSendToFlags aFlag)
{
	CHECK_NOTNULL(FImpl);
	int _result = FImpl->MSendTo(aProtocolName.MToLowerCase(), aBuffer, "",
			aFlag);
	return _result;
}
int CCustomer::MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
		const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{
	CHECK_NOTNULL(FImpl);
	int _result = FImpl->MSendTo(aProtocolName.MToLowerCase(), aBuffer, aTo,
			aFlag);
	return _result;
}

int CCustomer::MSend(NSHARE::CText aProtocolName, void* aBuffer, size_t aSize,
		eSendToFlags aFlag)
{
	CHECK_NOTNULL(FImpl);
	NSHARE::CBuffer _buf = FImpl->MGetNewBuf(aSize);
	if (_buf.size() != aSize)
		return E_CANNOT_ALLOCATE_BUFFER;
	memcpy(_buf.ptr(), aBuffer, aSize);

	int _result = FImpl->MSendTo(aProtocolName.MToLowerCase(), _buf, "", aFlag);
	return _result;
}
int CCustomer::MSend(NSHARE::CText aProtocolName, void* aBuffer, size_t aSize,
		const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{
	CHECK_NOTNULL(FImpl);
	NSHARE::CBuffer _buf = FImpl->MGetNewBuf(aSize);
	if (_buf.size() != aSize)
		return E_CANNOT_ALLOCATE_BUFFER;
	memcpy(_buf.ptr(), aBuffer, aSize);

	int _result = FImpl->MSendTo(aProtocolName.MToLowerCase(), _buf, aTo,
			aFlag);
	return _result;
}
int CCustomer::MSend(unsigned aNumber, NSHARE::CBuffer & aBuffer,
		eSendToFlags aFlag)
{
	CHECK_NOTNULL(FImpl);

	int _result = FImpl->MSendTo(aNumber, aBuffer, aFlag);
	return _result;
}
int CCustomer::MSend(unsigned aNumber, NSHARE::CBuffer & aBuffer,
		const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{
	CHECK_NOTNULL(FImpl);
	int _result = FImpl->MSendTo(aNumber, aBuffer, aTo, aFlag);
	return _result;
}
CCustomer::customers_t CCustomer::MCustomers() const
{
	return FImpl->MCustomers();
}

bool CCustomer::operator+=(value_t const & aVal)
{
	return FImpl->operator +=(aVal);
}
bool CCustomer::operator-=(value_t const & aVal)
{
	return FImpl->operator -=(aVal);
}
bool CCustomer::MAdd(value_t const & aVal, unsigned int aPrior)
{
	return FImpl->MAdd(aVal, aPrior);
}
bool CCustomer::MErase(value_t const& aVal)
{
	return FImpl->MErase(aVal);
}

bool CCustomer::MChangePrior(value_t const&aVal, unsigned int aPrior)
{
	return FImpl->MChangePrior(aVal, aPrior);
}
bool CCustomer::MIs(value_t const& aVal) const
{
	return FImpl->MIs(aVal);
}
bool CCustomer::MIsKey(NSHARE::CText const& aVal) const
{
	return FImpl->MIsKey(aVal);
}
std::ostream& CCustomer::MPrintEvents(std::ostream & aStream) const
{
	return FImpl->MPrintEvents(aStream);
}

bool CCustomer::MEmpty() const
{
	return FImpl->MEmpty();
}
NSHARE::CBuffer CCustomer::MGetNewBuf(unsigned aSize) const
{
	CHECK_NOTNULL(FImpl);
	return FImpl->MGetNewBuf(aSize);
}
const NSHARE::version_t& CCustomer::sMVersion()
{
	return g_version;
}
void CCustomer::MWaitForEvent(NSHARE::CText const& aEvent, double aSec)
{
	CHECK_NOTNULL(FImpl);
	return FImpl->MWaitForReady(aSec);
}
} //
