// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CCustomer.cpp
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#	include <fstream>
#ifdef _WIN32
#	include <winsock2.h>
#endif
#include <SHARE/fdir.h>
#include <SHARE/revision.h>
#include <UDT/programm_id.h>
#include <UDT/CCustomer.h>

#include <CCustomerImpl.h>
#include <shared_types.h>
#include <CRTCFactory.h>

DECLARATION_VERSION_FOR(customer)

static const NSHARE::version_t g_cutomer_version(MAJOR_VERSION_OF (customer),
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
//const NSHARE::CText CCustomer::CONFIG_PATH = "path";
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
const NSHARE::CText CCustomer::EVENT_RECEIVER_SUBSCRIBE = "event_receiver";
const NSHARE::CText CCustomer::EVENT_RECEIVER_UNSUBSCRIBE = "remove_receiver";

const NSHARE::CText CCustomer::EVENT_UPDATE_RTC_INFO = "rtc_info_updated";

//kernel's error
COMPILE_ASSERT(sizeof(CCustomer::error_t)==sizeof(error_type),E_INVALID_ERROR_SIZE);
COMPILE_ASSERT(sizeof(fail_sent_args_t::error_t)==sizeof(error_type),E_INVALID_FAIL_ERROR_SIZE);


const CCustomer::error_t CCustomer::E_HANDLER_IS_NOT_EXIST=NUDT::E_HANDLER_IS_NOT_EXIST;
const CCustomer::error_t CCustomer::E_NO_ROUTE=NUDT::E_NO_ROUTE;
const CCustomer::error_t CCustomer::E_UNKNOWN_ERROR=NUDT::E_UNKNOWN_ERROR;
const CCustomer::error_t CCustomer::E_PARSER_IS_NOT_EXIST=NUDT::E_PARSER_IS_NOT_EXIST;
const CCustomer::error_t CCustomer::E_CANNOT_PARSE_BUFFER=NUDT::E_CANNOT_PARSE_BUFFER;
const CCustomer::error_t CCustomer::E_SOCKET_CLOSED=NUDT::E_SOCKET_CLOSED;
const CCustomer::error_t CCustomer::E_BUFFER_IS_FULL=NUDT::E_BUFFER_IS_FULL;
const CCustomer::error_t CCustomer::E_PACKET_LOST=NUDT::E_PACKET_LOST;
const CCustomer::error_t CCustomer::E_DATA_TOO_LARGE=NUDT::E_DATA_TOO_LARGE;
const CCustomer::error_t CCustomer::E_MERGE_ERROR=NUDT::E_MERGE_ERROR;
const CCustomer::error_t CCustomer::E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE=NUDT::E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE;

const CCustomer::error_t CCustomer::E_USER_ERROR_EXIST=NUDT::E_USER_ERROR_BEGIN;
const unsigned CCustomer::MAX_SIZE_USER_ERROR=sizeof(error_type)*8-eUserErrorStartBits;

CCustomer::CCustomer() :
		FImpl(new _pimpl(*this))
{
}
CCustomer::~CCustomer()
{
	delete FImpl;
}

const program_id_t& CCustomer::MGetID() const
{
	return MGetImpl().FMyId;
}
static void f_init_log(int argc, char const* argv[])
{
	static bool _is=false;
	if(_is)
		return;
	_is=true;
#ifdef _WIN32
	WSADATA _data;
	WSAStartup(MAKEWORD(2,2), &_data);
#endif
	//init trace
	init_trace(argc, argv);
}

int CCustomer::sMInit(int argc, char const* argv[], char const* aName,NSHARE::version_t const& aVersion,
		const NSHARE::CText& aConf)
{
	f_init_log(argc, argv);
	NSHARE::CConfig _conf;

	int _rval = 0;
	const char* envModuleDir = getenv(ENV_CONFIG_PATH.c_str());
	if (!aConf.empty() || envModuleDir)
	{
		std::ifstream _stream;
		NSHARE::CText _name=aConf;
		if (!_name.empty())
		{
			_stream.open(_name.c_str());
#ifndef CUSTOMER_WITH_STATIC_MODULES
			LOG_IF(DFATAL,!_stream.is_open() && !envModuleDir)
														<< "***ERROR***:Configuration file - "
														<< _name << ".";
#endif
		}

		if (!_stream.is_open() && envModuleDir)
		{
			_name=envModuleDir;
			_stream.open(envModuleDir);
#ifndef CUSTOMER_WITH_STATIC_MODULES
			LOG_IF(DFATAL,!_stream.is_open())
														<< "***ERROR***:Cannot open configuration file from Environment and from local path"
														<< ENV_CONFIG_PATH
														<< " == "<< envModuleDir<< ".";
#endif
		}


		if (_stream.is_open())
		{
			if (_name.find(".xml") != NSHARE::CText::npos)
			{
				LOG(INFO)<<"Initialize from xml configuration file "<<_name;
				_rval=_conf.MFromXML(_stream)?_rval:static_cast<int>(ERROR_CONFIGURE_IS_INVALID);
			}
			else if(_name.find(".json")!=NSHARE::CText::npos)
			{
				LOG(INFO)<<"Initialize from json configuration file "<<_name;
				_rval=_conf.MFromJSON(_stream)?_rval:static_cast<int>(ERROR_CONFIGURE_IS_INVALID);
			}
			else
			{
				LOG(DFATAL)<<"Unknown file format "<<_name;
				_rval= static_cast<int>(ERROR_CANNOT_READ_CONFIGURE);
			}
			_stream.close();
		}
#ifndef CUSTOMER_WITH_STATIC_MODULES
		else
			_rval=static_cast<int>(ERROR_CANNOT_READ_CONFIGURE);
#endif
	}
#ifndef CUSTOMER_WITH_STATIC_MODULES
	else
		_rval=static_cast<int>(ERROR_CANNOT_READ_CONFIGURE);
#endif

	if(_rval==0)
		_rval= sMInit(argc, argv, aName,aVersion, _conf);

	return _rval;
}
int CCustomer::sMInit(int argc, char const* argv[], char const* aName,NSHARE::version_t const& aVersion,
		const NSHARE::CConfig& aConf)
{
	f_init_log(argc, argv);

	new CConfigure();

	VLOG(1) << "Read Internal config " << aConf;

	CConfigure::sMGetInstance().MGet().MMerge(aConf);
	VLOG(1) << "Final config " << CConfigure::sMGetInstance().MGet();

	CCustomer* _p=new CCustomer();

	const int _rval = _p->MGetImpl().MInitialize(argv[0], aName,aVersion);

	if (_rval != 0)
	{
		sMFree();
	}
	return _rval;
}
void CCustomer::sMFree()
{
	delete CCustomer::sMGetInstancePtr();
	delete CConfigure::sMGetInstancePtr();
	sFSingleton=NULL;
	/// \TODO it
}
bool CCustomer::MIsOpened() const
{
	return MGetImpl().MIsOpened();
}
bool CCustomer::MIsConnected() const
{
	return MGetImpl().MIsConnected();
}
bool CCustomer::MOpen()
{
	return MGetImpl().MOpen();
}

void CCustomer::MClose()
{
	MGetImpl().MClose();
}

bool CCustomer::MAvailable(const NSHARE::CText& aModule) const
{
	return MGetImpl().MAvailable(aModule);
}
CCustomer::modules_t CCustomer::MModules() const
{
	return MGetImpl().MAllAvailable();
}

std::vector<request_info_t> CCustomer::MGetMyWishForMSG() const
{
	std::vector<request_info_t> _wish;
	MGetImpl().MGetMyWishForMSG(_wish);
	return _wish;
}
int CCustomer::MIWantReceivingMSG(const NSHARE::CText& aFrom,
		const unsigned& aHeader, const callback_t& aCB,NSHARE::version_t const& aVer,requirement_msg_info_t::eFLags const& aFlags)
{
	requirement_msg_info_t _msg;
	_msg.FRequired.FVersion = aVer;
	_msg.FRequired.FNumber = aHeader;
	_msg.FProtocolName = "";
	_msg.FFlags=aFlags;
	_msg.FFrom=aFrom;
	//It's not need

	return MIWantReceivingMSG( _msg, aCB);
}
int CCustomer::MIWantReceivingMSG(const requirement_msg_info_t& aHeader, const callback_t& aCB)
{
	return MGetImpl().MSettingDgParserFor( aHeader, aCB);
}

int CCustomer::MDoNotReceiveMSG(const requirement_msg_info_t& aNumber,callback_t * aTo)
{
	return MGetImpl().MRemoveDgParserFor(aNumber, aTo);
}
int CCustomer::MDoNotReceiveMSG(unsigned aHandlerId, request_info_t* aTo)
{
	return MGetImpl().MRemoveDgParserFor(aHandlerId, aTo);
}
int CCustomer::MDoNotReceiveMSG(const NSHARE::CText& aFrom,
		const unsigned& aNumber)
{
	requirement_msg_info_t _msg;
	_msg.FRequired.FVersion = MGetID().FVersion;
	_msg.FRequired.FNumber = aNumber;
	_msg.FFrom=aFrom;
	return MGetImpl().MRemoveDgParserFor( _msg, NULL);
}

int CCustomer::MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
		eSendToFlags aFlag)
{
	int _result = MGetImpl().MSendTo(aProtocolName.MToLowerCase(), aBuffer, "",
			aFlag);
	return _result;
}
int CCustomer::MSend(NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
		const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{

	int _result = MGetImpl().MSendTo(aProtocolName.MToLowerCase(), aBuffer, aTo,
			aFlag);
	return _result;
}

int CCustomer::MSend(NSHARE::CText aProtocolName, void const* aBuffer, size_t aSize,
		eSendToFlags aFlag)
{

	NSHARE::CBuffer _buf = MGetImpl().MGetNewBuf(aSize);
	if (_buf.size() != aSize)
		return static_cast<int>(ERROR_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE);
	memcpy(_buf.ptr(), aBuffer, aSize);

	int _result = MGetImpl().MSendTo(aProtocolName.MToLowerCase(), _buf, "", aFlag);
	return _result;
}
int CCustomer::MSend(NSHARE::CText aProtocolName, void const* aBuffer, size_t aSize,
		const NSHARE::uuid_t& aTo, eSendToFlags aFlag)
{

	NSHARE::CBuffer _buf = MGetImpl().MGetNewBuf(aSize);
	if (_buf.size() != aSize)
		return static_cast<int>(ERROR_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE);
	memcpy(_buf.ptr(), aBuffer, aSize);

	int _result = MGetImpl().MSendTo(aProtocolName.MToLowerCase(), _buf, aTo,
			aFlag);
	return _result;
}
int CCustomer::MSend(unsigned aNumber, NSHARE::CBuffer & aBuffer,NSHARE::version_t const& aVer,
		eSendToFlags aFlag)
{


	int _result = MGetImpl().MSendTo(aNumber, aBuffer, aVer,aFlag);
	return _result;
}
int CCustomer::MSend(unsigned aNumber, NSHARE::CBuffer & aBuffer,
		const NSHARE::uuid_t& aTo,NSHARE::version_t const& aVer, eSendToFlags aFlag)
{

	int _result = MGetImpl().MSendTo(aNumber, aBuffer, aTo,aVer, aFlag);
	return _result;
}
int CCustomer::MSend(required_header_t const& aNumber,
		NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
		const NSHARE::uuid_t& aTo,
		eSendToFlags aFlag)
{

	int _result = MGetImpl().MSendTo(aNumber,aProtocolName, aBuffer, aTo,aFlag);
	return _result;
}
int CCustomer::MSend(required_header_t const& aNumber,
		NSHARE::CText aProtocolName, NSHARE::CBuffer & aBuffer,
		eSendToFlags aFlag)
{


	int _result = MGetImpl().MSendTo(aNumber, aProtocolName,aBuffer,aFlag);
	return _result;
}
program_id_t CCustomer::MCustomer(NSHARE::uuid_t const& aUUID) const
{
	return MGetImpl().MCustomer(aUUID);
}
CCustomer::customers_t CCustomer::MCustomers() const
{
	return MGetImpl().MCustomers();
}

bool CCustomer::operator+=(event_handler_info_t const & aVal)
{
	return MGetImpl().operator +=(aVal);
}
bool CCustomer::operator-=(event_handler_info_t const & aVal)
{
	return MGetImpl().operator -=(aVal);
}
bool CCustomer::MAdd(event_handler_info_t const & aVal, unsigned int aPrior)
{
	return MGetImpl().MAdd(aVal, aPrior);
}
bool CCustomer::MErase(event_handler_info_t const& aVal)
{
	return MGetImpl().MErase(aVal);
}

bool CCustomer::MChangePrior(event_handler_info_t const&aVal, unsigned int aPrior)
{
	return MGetImpl().MChangePrior(aVal, aPrior);
}
bool CCustomer::MIs(event_handler_info_t const& aVal) const
{
	return MGetImpl().MIs(aVal);
}
bool CCustomer::MIsKey(NSHARE::CText const& aVal) const
{
	return MGetImpl().MIsKey(aVal);
}
std::ostream& CCustomer::MPrintEvents(std::ostream & aStream) const
{
	return MGetImpl().MPrintEvents(aStream);
}

bool CCustomer::MEmpty() const
{
	return MGetImpl().MEmpty();
}
NSHARE::CBuffer CCustomer::MGetNewBuf(std::size_t aSize) const
{

	return MGetImpl().MGetNewBuf(aSize);
}
const NSHARE::version_t& CCustomer::sMVersion()
{
	return g_cutomer_version;
}

int CCustomer::MWaitForEvent(NSHARE::Strings const& aEvents,double aSec)
{
	return MGetImpl().MWaitForEvent(aEvents,aSec);
}
int CCustomer::MWaitForEvent(NSHARE::CText const& aEvent, double aSec)
{
	NSHARE::Strings _list;
	_list.push_back(aEvent);
	return MGetImpl().MWaitForEvent(_list,aSec);
}
void CCustomer::MJoin()
{

	return MGetImpl().MJoin();
}
IRtc* CCustomer::MGetRTC(NSHARE::CText const& aName) const
{
	return MGetImpl().MGetRTC(aName);
}
CCustomer::rtc_list_t CCustomer::MGetListOfRTC() const
{
	return MGetImpl().MGetListOfRTC();
}
std::ostream& CCustomer::sMPrintError(std::ostream& aStream, error_t const& aVal)
{
	return aStream<<static_cast<eErrorBitwiseCode>(encode_inner_error(aVal));
}
CCustomer::_pimpl& CCustomer::MGetImpl() const
{
	DCHECK_NOTNULL(FImpl);
	return *FImpl;
}
#define SERIALIZE_FUNCTION_OF(aType)\
		NSHARE::CConfig serialize_##aType(NUDT:: aType const& aObject)\
		/*END*/

#define DESERIALIZE_FUNCTION_OF(aType)\
		NUDT:: aType deserialize_##aType(NSHARE::CConfig const& aConf)\
		/*END*/

const NSHARE::CText requirement_msg_info_t::NAME = "req";
const NSHARE::CText requirement_msg_info_t::KEY_PROTOCOL_NAME = "protocol";
const NSHARE::CText requirement_msg_info_t::KEY_FLAGS = "flags";
const NSHARE::CText requirement_msg_info_t::KEY_FROM = "from";

SERIALIZE_FUNCTION_OF(requirement_msg_info_t)
{
	NSHARE::CConfig _conf(requirement_msg_info_t::NAME);
	_conf.MAdd(requirement_msg_info_t::KEY_PROTOCOL_NAME, aObject.FProtocolName);
	_conf.MAdd(NSHARE::serialize(aObject.FRequired));
	_conf.MAdd(requirement_msg_info_t::KEY_FLAGS, aObject.FFlags);
	_conf.MAdd(requirement_msg_info_t::KEY_FROM, aObject.FFrom);
	return _conf;
}
DESERIALIZE_FUNCTION_OF(requirement_msg_info_t)
{
	requirement_msg_info_t _rval;
	aConf.MGetIfSet(requirement_msg_info_t::KEY_PROTOCOL_NAME, _rval.FProtocolName);
	_rval.FRequired = NSHARE::deserialize<required_header_t>(
			aConf.MChild(required_header_t::NAME));
	aConf.MGetIfSet(requirement_msg_info_t::KEY_FLAGS, _rval.FFlags);
	aConf.MGetIfSet(requirement_msg_info_t::KEY_FROM, _rval.FFrom);
	return _rval;
}


const NSHARE::CText received_data_t::NAME = "receive_data";
const NSHARE::CText received_data_t::KEY_HEADER_BEGIN = "header_begin";
SERIALIZE_FUNCTION_OF(received_data_t)
{
	NSHARE::CConfig _conf(received_data_t::NAME);
	if(aObject.FHeaderBegin != NULL)
		_conf.MAdd(received_data_t::KEY_HEADER_BEGIN, aObject.FHeaderBegin - aObject.FBegin);
	else
		_conf.MAdd(received_data_t::KEY_HEADER_BEGIN, 0);

	_conf.MAdd(NSHARE::serialize(aObject.FBuffer));
	return _conf;
}
DESERIALIZE_FUNCTION_OF(received_data_t)
{
	received_data_t _rval;
	_rval.FBuffer = NSHARE::deserialize<NSHARE::CBuffer>(
			aConf.MChild(NSHARE::CBuffer::NAME));
	if(_rval.FBuffer.empty())
	{
		_rval.FBegin = NULL;
		_rval.FEnd = NULL;
		_rval.FHeaderBegin = NULL;
	}else
	{
		_rval.FBegin = (const uint8_t*)_rval.FBuffer.ptr_const();
		_rval.FEnd = _rval.FBegin + _rval.FBuffer.size();
		_rval.FHeaderBegin = _rval.FBegin;
		unsigned _offset = 0;
		if(aConf.MGetIfSet(received_data_t::KEY_HEADER_BEGIN, _offset))
		{
			_rval.FHeaderBegin += _offset;
		}
	}
	return _rval;
}


const NSHARE::CText received_message_info_t::NAME = "received_info";
const NSHARE::CText received_message_info_t::KEY_FROM = "from";
const NSHARE::CText received_message_info_t::KEY_PROTOCOL_NAME = "protocol";
const NSHARE::CText received_message_info_t::KEY_PACKET_NUMBER = "packet_number";
const NSHARE::CText received_message_info_t::KEY_TO = "to";
const NSHARE::CText received_message_info_t::KEY_OCCUR_USER_ERROR = "user_error";
const NSHARE::CText received_message_info_t::KEY_ENDIAN = "endian";
const NSHARE::CText received_message_info_t::KEY_REMAIN_CALLBACKS = "remain_cb";
const NSHARE::CText received_message_info_t::KEY_CBS = "cbs";
const NSHARE::CText received_message_info_t::KEY_FLAGS = "flags";
SERIALIZE_FUNCTION_OF(received_message_info_t)
{
	NSHARE::CConfig _conf(received_message_info_t::NAME);

	_conf.MAdd(received_message_info_t::KEY_FROM,
			NSHARE::serialize(aObject.FFrom));
	_conf.MAdd(received_message_info_t::KEY_PROTOCOL_NAME,
			aObject.FProtocolName);
	_conf.MAdd(received_message_info_t::KEY_PACKET_NUMBER,
				aObject.FPacketNumber);

	_conf.MAdd(
			NSHARE::serialize(aObject.FHeader));

	{
		received_message_info_t::uuids_t::const_iterator _it(aObject.FTo.begin()),
				 _it_end(aObject.FTo.end());
		for(;_it!=_it_end;++_it)
			_conf.MAdd(received_message_info_t::KEY_TO,
					NSHARE::serialize<NSHARE::uuid_t>(*_it));
	}

	_conf.MAdd(received_message_info_t::KEY_OCCUR_USER_ERROR,
				aObject.FOccurUserError);

	_conf.MAdd(received_message_info_t::KEY_ENDIAN,
				aObject.FEndian);


	_conf.MAdd(received_message_info_t::KEY_REMAIN_CALLBACKS,
					aObject.FRemainCallbacks);
	_conf.MAdd(received_message_info_t::KEY_CBS,
					aObject.FCbs);
	_conf.MAdd(received_message_info_t::KEY_FLAGS,
					aObject.FFlags);
	return _conf;
}
DESERIALIZE_FUNCTION_OF(received_message_info_t)
{
	received_message_info_t _rval;
	_rval.FFrom = NSHARE::deserialize<NSHARE::uuid_t>(
			aConf.MChild(received_message_info_t::KEY_FROM));

	aConf.MGetIfSet(received_message_info_t::KEY_PROTOCOL_NAME,
			_rval.FProtocolName);
	aConf.MGetIfSet(received_message_info_t::KEY_PACKET_NUMBER,
			_rval.FPacketNumber);

	_rval.FHeader = NSHARE::deserialize<required_header_t>(
			aConf.MChild(required_header_t::NAME));

	{
		NSHARE::ConfigSet _set = aConf.MChildren(received_message_info_t::KEY_TO);
		NSHARE::ConfigSet::const_iterator _it = _set.begin();
		for (; _it != _set.end(); ++_it)
		{
			_rval.FTo.push_back(NSHARE::deserialize<NSHARE::uuid_t>(*_it));
		}
	}

	aConf.MGetIfSet(received_message_info_t::KEY_OCCUR_USER_ERROR,
			_rval.FOccurUserError);

	aConf.MGetIfSet(received_message_info_t::KEY_ENDIAN,
			_rval.FEndian);

	aConf.MGetIfSet(received_message_info_t::KEY_REMAIN_CALLBACKS,
			_rval.FRemainCallbacks);

	aConf.MGetIfSet(received_message_info_t::KEY_CBS,
			_rval.FCbs);

	aConf.MGetIfSet(received_message_info_t::KEY_FLAGS,
			_rval.FFlags);

	return _rval;
}
const NSHARE::CText received_message_args_t::NAME = "received_msg";
SERIALIZE_FUNCTION_OF(received_message_args_t)
{
	NSHARE::CConfig _conf(received_message_args_t::NAME,
			NSHARE::serialize<received_message_info_t>(
					static_cast<received_message_info_t const>(aObject))
			);
	_conf.MAdd(NSHARE::serialize(aObject.FMessage));
	return _conf;
}
DESERIALIZE_FUNCTION_OF(received_message_args_t)
{
	received_message_args_t _rval;
	_rval.FMessage = NSHARE::deserialize<received_data_t>(
			aConf.MChild(received_data_t::NAME));

	_rval.FFrom = NSHARE::deserialize<NSHARE::uuid_t>(
			aConf.MChild(received_message_args_t::KEY_FROM));

	aConf.MGetIfSet(received_message_args_t::KEY_PROTOCOL_NAME,
			_rval.FProtocolName);
	aConf.MGetIfSet(received_message_args_t::KEY_PACKET_NUMBER,
			_rval.FPacketNumber);

	_rval.FHeader = NSHARE::deserialize<required_header_t>(
			aConf.MChild(required_header_t::NAME));

	{
		NSHARE::ConfigSet _set = aConf.MChildren(received_message_args_t::KEY_TO);
		NSHARE::ConfigSet::const_iterator _it = _set.begin();
		for (; _it != _set.end(); ++_it)
		{
			_rval.FTo.push_back(NSHARE::deserialize<NSHARE::uuid_t>(*_it));
		}
	}

	aConf.MGetIfSet(received_message_args_t::KEY_OCCUR_USER_ERROR,
			_rval.FOccurUserError);

	aConf.MGetIfSet(received_message_args_t::KEY_ENDIAN,
			_rval.FEndian);

	aConf.MGetIfSet(received_message_args_t::KEY_REMAIN_CALLBACKS,
			_rval.FRemainCallbacks);

	aConf.MGetIfSet(received_message_args_t::KEY_CBS,
			_rval.FCbs);

	aConf.MGetIfSet(received_message_args_t::KEY_FLAGS,
			_rval.FFlags);

	return _rval;
}

const NSHARE::CText customers_updated_args_t::NAME = "customers_updated";
const NSHARE::CText customers_updated_args_t::KEY_DISCONNECTED = "disconnected";
const NSHARE::CText customers_updated_args_t::KEY_CONNECTED = "connected";

SERIALIZE_FUNCTION_OF(customers_updated_args_t)
{
	NSHARE::CConfig _conf(customers_updated_args_t::NAME);

	{
		std::set<program_id_t>::const_iterator _it(aObject.FDisconnected.begin()),
				 _it_end(aObject.FDisconnected.end());
		for(;_it!=_it_end;++_it)
			_conf.MAdd(customers_updated_args_t::KEY_DISCONNECTED,
					NSHARE::serialize<program_id_t>(*_it));
	}
	{
		std::set<program_id_t>::const_iterator _it(aObject.FConnected.begin()),
				 _it_end(aObject.FConnected.end());
		for(;_it!=_it_end;++_it)
			_conf.MAdd(customers_updated_args_t::KEY_CONNECTED,
					NSHARE::serialize<program_id_t>(*_it));
	}
	return _conf;
}
DESERIALIZE_FUNCTION_OF(customers_updated_args_t)
{
	customers_updated_args_t _rval;
	{
		NSHARE::ConfigSet _set = aConf.MChildren(customers_updated_args_t::KEY_DISCONNECTED);
		NSHARE::ConfigSet::const_iterator _it = _set.begin();
		for (; _it != _set.end(); ++_it)
		{
			_rval.FDisconnected.insert(NSHARE::deserialize<program_id_t>(*_it));
		}
	}
	{
			NSHARE::ConfigSet _set = aConf.MChildren(customers_updated_args_t::KEY_CONNECTED);
			NSHARE::ConfigSet::const_iterator _it = _set.begin();
			for (; _it != _set.end(); ++_it)
			{
				_rval.FConnected.insert(NSHARE::deserialize<program_id_t>(*_it));
			}
	}
	return _rval;
}


const NSHARE::CText subcribe_receiver_args_t::what_t::NAME = "req";
const NSHARE::CText subcribe_receiver_args_t::what_t::KEY_WHO = "who";
NSHARE::CConfig serialize_deserialize_subcribe_receiver_args_t_what_t(
		subcribe_receiver_args_t::what_t const& aObject)
{
	NSHARE::CConfig _conf(subcribe_receiver_args_t::what_t::NAME);

	_conf.MAdd(NSHARE::serialize(aObject.FWhat));
	_conf.MAdd(subcribe_receiver_args_t::what_t::KEY_WHO,
			NSHARE::serialize(aObject.FWho));

	return _conf;
}
NUDT::subcribe_receiver_args_t::what_t deserialize_subcribe_receiver_args_t_what_t(
		NSHARE::CConfig const& aConf)
{
	subcribe_receiver_args_t::what_t _rval;
	_rval.FWhat = NSHARE::deserialize<requirement_msg_info_t>(
			aConf.MChild(requirement_msg_info_t::NAME));

	_rval.FWho = NSHARE::deserialize<NSHARE::uuid_t>(
			aConf.MChild(subcribe_receiver_args_t::what_t::KEY_WHO));

	return _rval;
}
const NSHARE::CText subcribe_receiver_args_t::NAME = "receivers_info";
const NSHARE::CText subcribe_receiver_args_t::KEY_RECEIVERS = "receivers";

SERIALIZE_FUNCTION_OF(subcribe_receiver_args_t)
{
	NSHARE::CConfig _conf(subcribe_receiver_args_t::NAME);

	{
		subcribe_receiver_args_t::receivers_t::const_iterator _it(
				aObject.FReceivers.begin()),
				 _it_end(aObject.FReceivers.end());
		for(;_it!=_it_end;++_it)
		{
			_conf.MAdd(subcribe_receiver_args_t::KEY_RECEIVERS,
					NSHARE::serialize<NUDT::subcribe_receiver_args_t::what_t>(*_it));
		}
	}
	return _conf;
}
DESERIALIZE_FUNCTION_OF(subcribe_receiver_args_t)
{
	subcribe_receiver_args_t _rval;
	{
		NSHARE::ConfigSet _set = aConf.MChildren(subcribe_receiver_args_t::KEY_RECEIVERS);
		NSHARE::ConfigSet::const_iterator _it = _set.begin();
		for (; _it != _set.end(); ++_it)
		{
			_rval.FReceivers.push_back(NSHARE::deserialize<NUDT::subcribe_receiver_args_t::what_t>(*_it));
		}
	}
	return _rval;
}




const NSHARE::CText fail_sent_args_t::NAME = "fail_sent";
const NSHARE::CText fail_sent_args_t::KEY_FROM = "from";
const NSHARE::CText fail_sent_args_t::KEY_PROTOCOL_NAME = "protocol";
const NSHARE::CText fail_sent_args_t::KEY_PACKET_NUMBER = "packet_number";
const NSHARE::CText fail_sent_args_t::KEY_ERROR_CODE = "error_code";
const NSHARE::CText fail_sent_args_t::KEY_SENT_TO = "to";
const NSHARE::CText fail_sent_args_t::KEY_FAILS = "fail_to";
const NSHARE::CText fail_sent_args_t::KEY_USER_ERROR = "user_error";

SERIALIZE_FUNCTION_OF(fail_sent_args_t)
{
	NSHARE::CConfig _conf(fail_sent_args_t::NAME);

	_conf.MAdd(fail_sent_args_t::KEY_FROM,
			NSHARE::serialize(aObject.FFrom));
	_conf.MAdd(fail_sent_args_t::KEY_PROTOCOL_NAME,
			aObject.FProtocolName);
	_conf.MAdd(fail_sent_args_t::KEY_PACKET_NUMBER,
				aObject.FPacketNumber);

	_conf.MAdd(
			NSHARE::serialize(aObject.FHeader));

	_conf.MAdd(fail_sent_args_t::KEY_ERROR_CODE,
				aObject.FErrorCode);

	_conf.MAdd(fail_sent_args_t::KEY_USER_ERROR,
				aObject.FUserCode);

	{
		fail_sent_args_t::uuids_t::const_iterator _it(aObject.FSentTo.begin()),
				 _it_end(aObject.FSentTo.end());
		for(;_it!=_it_end;++_it)
			_conf.MAdd(fail_sent_args_t::KEY_SENT_TO,
					NSHARE::serialize<NSHARE::uuid_t>(*_it));
	}
	{
		fail_sent_args_t::uuids_t::const_iterator _it(aObject.FFails.begin()),
				 _it_end(aObject.FFails.end());
		for(;_it!=_it_end;++_it)
			_conf.MAdd(fail_sent_args_t::KEY_FAILS,
					NSHARE::serialize<NSHARE::uuid_t>(*_it));
	}
	return _conf;
}
DESERIALIZE_FUNCTION_OF(fail_sent_args_t)
{
	fail_sent_args_t _rval;
	_rval.FFrom = NSHARE::deserialize<NSHARE::uuid_t>(
			aConf.MChild(fail_sent_args_t::KEY_FROM));

	aConf.MGetIfSet(fail_sent_args_t::KEY_PROTOCOL_NAME,
			_rval.FProtocolName);
	aConf.MGetIfSet(fail_sent_args_t::KEY_PACKET_NUMBER,
			_rval.FPacketNumber);

	_rval.FHeader = NSHARE::deserialize<required_header_t>(
			aConf.MChild(required_header_t::NAME));
	aConf.MGetIfSet(fail_sent_args_t::KEY_ERROR_CODE,
			_rval.FErrorCode);

	aConf.MGetIfSet(fail_sent_args_t::KEY_USER_ERROR,
			_rval.FUserCode);

	{
		NSHARE::ConfigSet _set = aConf.MChildren(fail_sent_args_t::KEY_SENT_TO);
		NSHARE::ConfigSet::const_iterator _it = _set.begin();
		for (; _it != _set.end(); ++_it)
		{
			_rval.FSentTo.push_back(NSHARE::deserialize<NSHARE::uuid_t>(*_it));
		}
	}
	{
		NSHARE::ConfigSet _set = aConf.MChildren(fail_sent_args_t::KEY_FAILS);
		NSHARE::ConfigSet::const_iterator _it = _set.begin();
		for (; _it != _set.end(); ++_it)
		{
			_rval.FFails.push_back(NSHARE::deserialize<NSHARE::uuid_t>(*_it));
		}
	}

	return _rval;
}
} //

namespace std
{
std::ostream& operator<<(std::ostream & aStream,
		NUDT::requirement_msg_info_t::eFLags const& aFlags)
{
	using namespace NUDT;
	NSHARE::CFlags<NUDT::requirement_msg_info_t::eFLags,uint32_t> const _flags(aFlags);

	unsigned i=0;
	if(_flags.MGetFlag(requirement_msg_info_t::E_REGISTRATOR))
	{
		aStream<<"registrator";
		++i;
	}
	if(_flags.MGetFlag(requirement_msg_info_t::E_INVERT_GROUP))
	{
		if(i!=0)
			aStream<<", ";

		aStream<<"invert group";
		++i;
	}
	if(_flags.MGetFlag(requirement_msg_info_t::E_NEAREST))
	{
		if(i!=0)
			aStream<<", ";

		aStream<<"nearest";
		++i;
	}
	if(i==0)
		aStream<<"no flags";

	return aStream;
}
}
