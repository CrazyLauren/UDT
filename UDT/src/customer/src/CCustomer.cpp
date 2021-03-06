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
#include <udt/programm_id.h>
#include <udt/CCustomer.h>

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
			LOG_IF(DFATAL,!_stream.is_open() && !envModuleDir)
														<< "***ERROR***:Configuration file - "
														<< _name << ".";
		}

		if (!_stream.is_open() && envModuleDir)
		{
			_name=envModuleDir;
			_stream.open(envModuleDir);
			LOG_IF(DFATAL,!_stream.is_open())
														<< "***ERROR***:Cannot open configuration file from Environment and from local path"
														<< ENV_CONFIG_PATH
														<< " == "
														<< envModuleDir<< ".";
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
		else
		_rval=static_cast<int>(ERROR_CANNOT_READ_CONFIGURE);
	}
	else
		_rval=static_cast<int>(ERROR_CANNOT_READ_CONFIGURE);

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

int CCustomer::MDoNotReceiveMSG(const requirement_msg_info_t& aNumber)
{
	return MGetImpl().MRemoveDgParserFor(aNumber);
}
int CCustomer::MDoNotReceiveMSG(unsigned aHandlerId)
{
	return MGetImpl().MRemoveDgParserFor(aHandlerId);
}
int CCustomer::MDoNotReceiveMSG(const NSHARE::CText& aFrom,
		const unsigned& aNumber)
{
	requirement_msg_info_t _msg;
	_msg.FRequired.FVersion = MGetID().FVersion;
	_msg.FRequired.FNumber = aNumber;
	_msg.FFrom=aFrom;
	return MGetImpl().MRemoveDgParserFor( _msg);
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

int CCustomer::MSend(NSHARE::CText aProtocolName, void* aBuffer, size_t aSize,
		eSendToFlags aFlag)
{

	NSHARE::CBuffer _buf = MGetImpl().MGetNewBuf(aSize);
	if (_buf.size() != aSize)
		return static_cast<int>(ERROR_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE);
	memcpy(_buf.ptr(), aBuffer, aSize);

	int _result = MGetImpl().MSendTo(aProtocolName.MToLowerCase(), _buf, "", aFlag);
	return _result;
}
int CCustomer::MSend(NSHARE::CText aProtocolName, void* aBuffer, size_t aSize,
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
std::vector<IRtc*> CCustomer::MGetListOfRTC() const
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
