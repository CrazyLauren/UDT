/*
 * CControlByTCP.cpp
 *
 *  Created on: 17.12.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <revision.h>
#include <Socket.h>
#ifdef _WIN32
#include <winsock2.h>                     // sockaddr_in
#else
#include <arpa/inet.h>                      // htons, htonl
#include <sys/socket.h>
#endif
#include <programm_id.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include "receive_from.h"
#include <parser_in_protocol.h>
#include <CCustomer.h>
#include <CDataObject.h>
#include <CIOFactory.h>
#include <CConfigure.h>
#include "CControlByTCP.h"
#include "CControlByTCPRegister.h"
#include "CLocalChannelFactory.h"
DECLARATION_VERSION_FOR(tcp_client_io_manager)
namespace NUDT
{
NSHARE::CText const CControlByTCP::NAME = "tcp_client_io_manager";
NSHARE::CText const CControlByTCP::PORT = "Port";
NSHARE::CText const CControlByTCPRegister::NAME = "tcp_client_io_manager";
const double CControlByTCP::WAIT_ANSWER_BY_KERNEL = 0.1;
using namespace NSHARE;
CControlByTCP::CControlByTCP() :
		IIOConsumer(NAME), FTcpSocket(NULL), FParser(this), FPort(0), FCustomer(
				NULL), FMain(NULL)
{
	FCBServiceConnect = NSHARE::CB_t(sMConnect, this);
	FCBServiceDisconncet = NSHARE::CB_t(sMDisconnect, this);
	FState = E_CLOSED;
	FMainPacketNumber = 0;
}

CControlByTCP::~CControlByTCP()
{
	//fixme remove cb
	delete FTcpSocket;
}

void CControlByTCP::MInit(uint32_t aPort)
{
	VLOG(2) << "Construct IOContol Port=" << aPort << ":" << this;
	FPort = aPort;
	CHECK_NOTNULL(FTcpSocket);
	FTcpSocket->MOpen(net_address(aPort));
}
void CControlByTCP::MInit(ICustomer *aCustomer)
{
	CHECK_NOTNULL(aCustomer);
	LOG_IF(DFATAL,FCustomer!=NULL) << "Customer has been inited already.";

	FCustomer = aCustomer;

	CHECK(!FTcpSocket);
	FTcpSocket = new CTCP;
	*FTcpSocket += CTCPServer::value_t(CTCP::EVENT_CONNECTED,
			FCBServiceConnect);
	*FTcpSocket += CTCPServer::value_t(CTCP::EVENT_DISCONNECTED,
			FCBServiceDisconncet);

	callback_data_t _callbak(sMUpdateList, this);

	CDataObject::value_t _val_channel(req_recv_t::NAME,
			_callbak);

	CDataObject::sMGetInstance() += _val_channel;
}
//
//----------------------
//
template<>
void CControlByTCP::MFill<dg_info2_t>(data_t* aTo)
{
	VLOG(2) << "Create info DG";
	program_id_t  const _id = CCustomer::sMGetInstance().MGetID();
	CHECK_EQ(_id.FType,E_CONSUMER);
	serialize<dg_info2_t>(aTo,_id, routing_t(), error_info_t());
}

template<>
void CControlByTCP::MFill<custom_filters_dg2_t>(data_t* aTo)
{
	CHECK_NOTNULL(FCustomer);
	req_recv_t _data;
	CDataObject::sMGetInstance().MGetLast(_data);
	if (_data.FDemand.empty())
	{
		LOG(ERROR)<< "It does not expect data.";
		return;
	}
	serialize<custom_filters_dg2_t,demand_dgs_t>(aTo,_data.FDemand,routing_t (),error_info_t ());
}

template<>
void CControlByTCP::MFill<protocol_type_dg_t>(data_t* aTo)
{
	VLOG(2) << "Create protocol DG";

	const size_t full_size = sizeof(protocol_type_dg_t);

	//allocate memory
	size_t const _befor = aTo->size();
	aTo->resize(_befor + full_size);
	char * const _begin = (char*) aTo->ptr() + _befor;

	CHECK_LE(full_size, aTo->size() - _befor);

	//fill dg
	protocol_type_dg_t * _request = new (_begin) protocol_type_dg_t();
	_request->FProtocol = E_CONSUMER;

	//calc CRC and fill it
	fill_dg_head(_begin, full_size,CCustomer::sMGetInstance().MGetID());

	//paranoid check
	CHECK_EQ(full_size,
			(size_t )(reinterpret_cast<protocol_type_dg_t*>(_begin)->FHeadSize
					+ reinterpret_cast<protocol_type_dg_t*>(_begin)->FDataSize));

	VLOG(2) << "DG Protocol Info "
						<< *reinterpret_cast<protocol_type_dg_t*>(_begin);
}
//
//-----------------
//
//if you want to receive DG but don't want to handle it, It uncomments the template
//template<class DG_T>
//void CControlByTCP::MProcess(DG_T const* aP, void*)
//{
//	LOG(ERROR)<< "Unknown DG " << NSHARE::CTypeInfo(*aP) << " # "
//	<< aP->FType;
//}

template<>
void CControlByTCP::MProcess(main_channel_param_t const* aP, void*)
{
	VLOG(2) << "Main channel parametrs:" << *aP;

	LOG_IF(ERROR, FState!=E_SETTING ) << "Invalid state" << (unsigned) FState;

	CRAII<CMutex> _block(FMainLock);

	if (!FMain)
	{
		int _rval = MOpenMainChannel((utf8 const*) (aP->FType));
		if (_rval > 0)
		{
			NSHARE::CText _channel_type((utf8*) aP->FType);
			MSendMainChannelError(_channel_type, _rval);
		}
	}
	if (FMain)
	{
		VLOG(2) << "Handling by main channel.";
		//for interface uniformity,repeating parsing
		FMain->MHandleServiceDG(aP);
	}
}
template<>
void CControlByTCP::MProcess(main_channel_error_param_t const* aP, void*)
{
	VLOG(2) << "main channel error " << *aP;

	LOG_IF(ERROR,!(FState==E_SETTING||FState==E_CONNECTED) ) << "Invalid state"
																		<< (unsigned) FState;

	LOG_IF(ERROR,!FMain) << "The main channel " << (const char*) aP->FType
									<< " is not exist.";

	CRAII<CMutex> _block(FMainLock);
	if (FMain)
	{
		LOG_IF(WARNING,NSHARE::CText((utf8 const*) (aP->FType))
				!= FMain->MGetType())
												<< "Warning type of main channel is not equal.";
		if (aP->FError == 0)
			MSetStateConnected();

		FMain->MHandleServiceDG(aP);
	}
	else
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
}

template<>
void CControlByTCP::MProcess(request_main_channel_param_t const* aP, void*)
{
	VLOG(2) << "Request main channel param " << *aP;

	LOG_IF(ERROR,!(FState==E_SETTING||FState==E_CONNECTED) ) << "Invalid state"
																		<< (unsigned) FState;

	int _channel_error = 0;
	LOG_IF(ERROR,!FMain) << "The main channel " << (const char*) aP->FType
									<< " is not exist.";

	CRAII<CMutex> _block(FMainLock);
	bool const _is_equal =
			FMain
					&& NSHARE::CText((utf8 const*) (aP->FType))
							== FMain->MGetType() ? true : false;
	LOG_IF(WARNING,!_is_equal) << "Warning type of main channel is not equal.";

	if (_is_equal)
	{
		if (FMain->MIsOpened())
		{
			//for interface uniformity,repeating parsing
			return (void) FMain->MHandleServiceDG(aP);
		}
		else
			_channel_error = main_channel_error_param_t::E_NOT_OPENED;
	}
	else
	{
		ILocalChannel* _main =
				CLocalChannelFactory::sMGetInstance().MGetFactory(
						(utf8 const*) (aP->FType));
		if (_main)
		{
			return (void) _main->MHandleServiceDG(aP);
		}
		else
			_channel_error = main_channel_error_param_t::E_NO_CHANNEL;

	}
	if (_channel_error > 0)
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType), _channel_error);
}
template<>
void CControlByTCP::MProcess(close_main_channel_t const* aP, void*)
{
	LOG_IF(ERROR,FState!=E_CONNECTED) << "Invalid state" << (unsigned) FState;
	VLOG(2) << "Request close main channel param " << *aP;

	LOG_IF(ERROR,!FMain) << "The main channel " << (const char*) aP->FType
									<< " is not exist.";
	CRAII<CMutex> _block(FMainLock);
	if (FMain)
	{
		LOG_IF(WARNING,NSHARE::CText((utf8 const*) (aP->FType))
				!= FMain->MGetType())
												<< "Warning type of main channel is not equal.";

		FMain->MHandleServiceDG(aP);
	}

	int _rval = MCloseMain();
	if (_rval > 0)
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType), _rval);
}
template<>
void CControlByTCP::MProcess(requiest_info2_t const* aP, void*)
{
	VLOG(2) << "Request info from " << *aP;

	if (FState == E_SERVICE_CONNECTED)
		FState = E_SETTING;

	{
		NSHARE::CRAII<NSHARE::CMutex> _block(FControlLock);
		VLOG(2) << "Current kernel:" << FKernelId.MIs();
		if (!FKernelId.MIs())
		{
			program_id_t _id(
					deserialize<requiest_info2_t, program_id_t>(aP, NULL,
							NULL));
			FKernelId.MSet(_id);
			VLOG(2) << "New kernel " << FKernelId.MGet();
		}
	}
	if (MSendIDInfo() >= 0)
		MSendFilters();
}
template<>
void CControlByTCP::MProcess(custom_filters_dg2_t const* aP, void*)
{
	VLOG(2) << "New  receivers " << *aP;

	routing_t _uuid;
	demand_dgs_t _fail(
			deserialize<custom_filters_dg2_t, demand_dgs_t>(aP, &_uuid, NULL));

	MReceivedData(_fail);
}
template<>
void CControlByTCP::MProcess(clients_info2_t const* aP, void*)//todo replace to json
{
	VLOG(2) << "All clients info from " << *aP;

	routing_t _uuid;
	progs_id_t _fail(
			deserialize<clients_info2_t, progs_id_t>(aP, &_uuid, NULL));

	MReceivedData(_fail);
}
template<>
void CControlByTCP::MProcess(user_data_received_t const* aP, void* aParser)//Depreciated
{
//	LOG_IF(ERROR,FState!=E_CONNECTED) << "Invalid state" << (unsigned) FState;
//
//	//CHECK_EQ(&FMainParser, aParser);
//	VLOG(2) << "Kernel receive user data: " << *aP;
//
//	LOG_IF(DFATAL,aP->FFromUUID!=MGetKernelID().FUuid)
//																<< "USER Data has been received from the unknown channel:"
//																<< aP->FFromUUID
//																<< " Kernel:"
//																<< MGetKernelID().FUuid;
//	VLOG(2) << "The Kernel has received " << aP->FUserDgCounter;
//	MKernelReceived(aP->FUserDgCounter);
	CHECK(false);
}
template<>
void CControlByTCP::MProcess(protocol_type_dg_t const* aP, void* aParser)
{
	CHECK(false);
}
template<>
void CControlByTCP::MProcess(dg_info2_t const* aP, void* aParser)
{
	CHECK(false);
}
template<>
void CControlByTCP::MProcess(user_data_fail_send_t const* aP, void* aParser)
{
	VLOG(2) << "Failed sent info " << *aP;

	routing_t _uuid;
	fail_send_t _fail(
			deserialize<user_data_fail_send_t, fail_send_t>(aP, &_uuid, NULL));

	MReceivedData(_fail);
}
//
//----------------------
//
int CControlByTCP::sMUpdateList(CHardWorker* aWho, args_data_t* aWhat,
		void* aData)
{
	CControlByTCP* _this = (CControlByTCP*) aData;
	CHECK_NOTNULL(_this);
	if (!_this->MIsKernel())
		return 0;
	_this->MSendFilters();
	return 0;
}
//
//----------------------
//
int CControlByTCP::MSendIDInfo()
{
	VLOG(2) << "Send id info.";
	data_t _data;
	MFill<dg_info2_t>(&_data);
	int _rval = MSend(_data); //fixme
	LOG_IF(ERROR,_rval<0) << "Cannot send id info";
	return _rval;
}
void CControlByTCP::MSendFilters()
{
	VLOG(2) << "Send filters.";
	data_t _data;
	MFill<custom_filters_dg2_t>(&_data);
	int _rval = MSend(_data); //fixme
	(void) _rval;
	LOG_IF(ERROR,_rval<0) << "Cannot send filters";
}
int CControlByTCP::MSendMainChannelError(NSHARE::CText const& _channel_type,
		unsigned aError)
{
	NSHARE::CBuffer _data;
	_data.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_data.ptr()) main_channel_error_param_t;
	strcpy((char*) _p->FType, _channel_type.c_str());

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_data.ptr(), _data.size(),CCustomer::sMGetInstance().MGetID());


	//_channel.FDataId.FDestName.push_back(MGetKernelID().FName);
	return MSend(_data);
}
int CControlByTCP::MSend(data_t & aData)
{
	LOG_IF(ERROR,FState==E_CLOSED ||FState==E_OPENED) << "Invalid state"
																<< (unsigned) FState;

	NSHARE::CRAII<NSHARE::CMutex> _block(FControlLock);
	LOG_IF(ERROR,!FKernelId.MIs()) << "Unknown kernel id. Ignoring ...";
	if (!FKernelId.MIs())
		return CCustomer::E_UNKNOWN_ERROR;

	CHECK_NOTNULL(FTcpSocket);
	if (!FTcpSocket->MIsConnected())
	{
		LOG(INFO)<< (*FTcpSocket);
		return CCustomer::E_NOT_CONNECTED;
	}
	return FTcpSocket->MSend(aData.ptr(), aData.size()).FError;
}
int CControlByTCP::MSend(user_data_t & aData)
{
	//if (!MIsConnected())
	if(FState!=E_CONNECTED)
	{
		LOG(ERROR)<< "It's not connected.";
		return -1;
	}
	LOG_IF(ERROR,FState!=E_CONNECTED) << "Invalid state" << (unsigned) FState;

	unsigned const _number=MNextUserPacketNumber();
	aData.FDataId.FPacketNumber= _number;
	VLOG(2) << "Create user data DG";
	{
		CRAII<CMutex> _block(FMainLock);
		if (!FMain || !FMain->MSend(aData))
		return CCustomer::E_UNKNOWN_ERROR;
	}
	return _number;
}
//
//----------------------
//

bool CControlByTCP::MOpen(const NSHARE::CThread::param_t* aParam)
{
	CHECK_NOTNULL(FCustomer);

	NSHARE::CRAII<NSHARE::CMutex> _block(FControlLock);
	CConfig* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);

	LOG_IF(WARNING,!_p)<<" Configure for "<<NAME<<" is not exist.";

	unsigned _port = 18011;
	bool _val = _p && _p->MGetIfSet(PORT, _port);
	(void) _val;
	LOG_IF(WARNING,!_val)
									<< "The port number is not present in the config file"
									<< CConfigure::sMGetInstance().MGetPath()
									<< ".Using standard port " << _port;
	MInit(_port);

	LOG_IF(ERROR,FState!=E_CLOSED) << "Invalid state" << (unsigned) FState;
	if (MIsOpened())
		FState = E_OPENED;

	MStart(aParam);

	return MIsOpened();
}
int CControlByTCP::MCloseMain()
{
	LOG_IF(ERROR,FState!=E_CONNECTED) << "Invalid state" << (unsigned) FState;
	VLOG(2) << "Close main.";
	CRAII<CMutex> _block(FMainLock);
	if (!FMain)
		return main_channel_error_param_t::E_NOT_OPENED;

	if (!FMain->MIsOpened())
		return main_channel_error_param_t::E_NOT_OPENED;

	FMain->MClose();
	FMain = NULL;
	FMainPacketNumber = 0;
	return 0;
}

void CControlByTCP::MClose()
{
	VLOG(2) << "Close control.";
	MCloseMain();

	NSHARE::CRAII<NSHARE::CMutex> _block(FControlLock);
	MStop();

	FTcpSocket->MClose();

	FKernelId.MUnSet();
//todo
}
bool CControlByTCP::MIsConnected() const
{
	CRAII<CMutex> _block(FMainLock);
	return FTcpSocket->MIsConnected() && FMain && FMain->MIsConnected();
}
bool CControlByTCP::MIsOpened() const
{
	return FTcpSocket->MIsOpen();
}
NSHARE::ISocket* CControlByTCP::MGetSocket()
{
	return FTcpSocket;
}
const NSHARE::ISocket* CControlByTCP::MGetSocket() const
{
	return FTcpSocket;
}

bool CControlByTCP::MIsAvailable() const
{
	if (MIsOpened())
		return true;
	struct sockaddr_in _serv;

	NSHARE::CSocket _sock;
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	bool _result = false;
	if (_sock.MIsValid())
	{
		memset(&_serv, 0, sizeof(_sock));
		_serv.sin_family = AF_INET;
		_serv.sin_addr.s_addr = INADDR_LOOPBACK;
		_serv.sin_port = htons(FPort);

		_result = connect(_sock.MGet(), (const struct sockaddr*) &_serv,
				sizeof(_serv)) >= 0;
	}
	_sock.MClose();
	return _result;
}
void CControlByTCP::MReceivedData(const NSHARE::ISocket::data_t& aData)
{
	VLOG(2) << "Receive :" << aData.size();
	LOG_IF(ERROR,FState==E_CLOSED ||FState==E_OPENED) << "Invalid state"
																<< (unsigned) FState;

	MReceivedData(aData, NULL);
}
void CControlByTCP::MReceivedData(data_t const& aData, void*)
{
	FParser.MReceivedData(aData.begin(), aData.end());
	LOG_IF(INFO, aData.empty()) << "Empty Data.";
}

void CControlByTCP::MReceivedData(fail_send_t const & aData)
{
	fail_send_id_t _fail;
	_fail.FData=aData;
	CDataObject::sMGetInstance().MPush(_fail);
}
void CControlByTCP::MReceivedData(demand_dgs_t const & aData)
{
	demand_dgs_id_t _data;
	_data.FData = aData;
	CDataObject::sMGetInstance().MPush(_data);
}
void CControlByTCP::MReceivedData(progs_id_t const & aData)
{
	progs_id_from_t _data;
	_data.FData = aData;
	CDataObject::sMGetInstance().MPush(_data);
}
void CControlByTCP::MReceivedData(user_data_t const & aData)
{
	LOG_IF(ERROR,FState!=E_CONNECTED) << "Invalid state" << (unsigned) FState;
	recv_data_from_t _data;
	LOG_IF(DFATAL,!aData.FDataId.FUUIDTo.MIs()) << "Unrelated packet from "
												<< aData.FDataId.FFrom
												<< " has been received";
	_data.FData = aData;
	CDataObject::sMGetInstance().MPush(_data);
}
void CControlByTCP::MWaitForKernelReceived(unsigned _num)//fixme depreciated
{
	CRAII<CMutex> _block(FMainLock);
	VLOG(2) << "Wait for Kernel received " << _num;
	LOG_IF(DFATAL, FSendUserFIFO.find(_num)!=FSendUserFIFO.end())
																			<< " We has  been waiting "
																			<< _num
																			<< " already";
	FSendUserFIFO.insert(_num);
}
void CControlByTCP::MKernelReceived(unsigned _num)//fixme depreciated
{
	CRAII<CMutex> _block(FMainLock);
	VLOG(2) << "Kernel received " << _num;
	std::set<unsigned>::iterator _it = FSendUserFIFO.find(_num);
	LOG_IF(ERROR,_it==FSendUserFIFO.end()) << " Krenel is received " << _num
													<< " but it did not send it.";
	if (_it != FSendUserFIFO.end())
		FSendUserFIFO.erase(_it);
}
bool CControlByTCP::MIsKernelReceived(unsigned _num) const//fixme depreciated
{
	CRAII<CMutex> _block(FMainLock);
	return FSendUserFIFO.find(_num) == FSendUserFIFO.end();
}
int CControlByTCP::MWaitForSend(unsigned aNumber, unsigned aTime)//fixme depreciated
{
	double const _time = NSHARE::get_time();
	double const _timeout = aTime ? aTime / 1000.0 : WAIT_ANSWER_BY_KERNEL;
	for (HANG_INIT; !MIsKernelReceived(aNumber);
	HANG_CHECK, NSHARE::usleep(1000))
	{
		if ((NSHARE::get_time() - _time) < _timeout)
		{
			LOG(WARNING)<< "The kernel is not received user data. Ignoring...";
			MKernelReceived(aNumber);
			return CCustomer::E_KERNEL_IS_NOT_ANSWER;
		}
	}
	return 0;
}

NSHARE::CBuffer CControlByTCP::MGetNewBuf(unsigned aSize) const
{
	CRAII<CMutex> _block(FMainLock);
	if (FMain)
		return FMain->MGetNewBuf(aSize);
	else
		return NSHARE::CBuffer(aSize);
}

int CControlByTCP::MOpenMainChannel(NSHARE::CText const & aType)
{
	VLOG(2) << "Open main channel " << aType;
	LOG_IF(ERROR,FState!=E_SETTING) << "Invalid state" << (unsigned) FState;

	ILocalChannel* _main = CLocalChannelFactory::sMGetInstance().MGetFactory(
			aType);

	LOG_IF(ERROR,!_main) << "The main channel " << aType << " is not exist.";
	if (!_main)
	{
		return main_channel_error_param_t::E_NO_CHANNEL;
	}
	bool const _is_opend =
			_main->MIsOpened() || _main->MOpen(this) ? true : false;
	if (!_is_opend)
	{
		LOG(ERROR)<< "Cannot open the " << aType << " channel.";
		return main_channel_error_param_t::E_NOT_OPENED;
	}

	LOG(INFO)<< "Main channel " << aType << " opened sucsessfull.";
	MReplaceMainChannelTo(_main);

	return main_channel_error_param_t::E_OK;
}
void CControlByTCP::MReplaceMainChannelTo(ILocalChannel* aNew)
{
	if (FMain)
	{
		LOG(WARNING)<< "The Main channel " << FMain->MGetType()
		<< " has been opened already.";
		FMain->MClose();

	}
	FMain = aNew;

}
int CControlByTCP::sMConnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);

	net_address *_client = reinterpret_cast<net_address*>(aWhat);
	reinterpret_cast<CControlByTCP*>(aThis)->MConnect(_client);
	return 0;
}
int CControlByTCP::sMDisconnect(void* aWho, void* aWhat, void* aThis)
{
	CHECK_NOTNULL(aWhat);
	CHECK_NOTNULL(aThis);

	net_address *_client = reinterpret_cast<net_address*>(aWhat);

	reinterpret_cast<CControlByTCP*>(aThis)->MDisconnect(_client);
	return 0;
}

void CControlByTCP::MConnect(NSHARE::net_address* aVal)
{
	LOG_IF(ERROR,FState!=E_OPENED) << "Invalid state" << (unsigned) FState;
	FState = E_SERVICE_CONNECTED;

	VLOG(2) << "Connected to " << *aVal << ", " << NSHARE::get_unix_time();
	MSendProtocolType();
}
void CControlByTCP::MDisconnect(NSHARE::net_address* aVal)
{
	VLOG(2) << "Disconnected from " << *aVal << ", "
						<< NSHARE::get_unix_time();
	MCloseMain();
	FState = E_OPENED;
	FCustomer->MEventDisconnected();
}
id_t const& CControlByTCP::MGetKernelID() const
{
	return FKernelId.MGetConst().FId;
}
bool CControlByTCP::MIsKernel() const
{
	NSHARE::CRAII<NSHARE::CMutex> _block(FControlLock);
	return FKernelId.MIs();
}
void CControlByTCP::MSendProtocolType()
{
	VLOG(2) << "Send id info.";
	data_t _data;
	MFill<protocol_type_dg_t>(&_data);
	bool _is=FTcpSocket->MSend(_data).MIs();
	LOG_IF(ERROR,!_is) << "Cannot send protocol info";
}
void CControlByTCP::MSetStateConnected()
{
	FState = E_CONNECTED;
	FCustomer->MEventConnected();
}
CControlByTCPRegister::CControlByTCPRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(MAJOR_VERSION_OF(tcp_client_io_manager), MINOR_VERSION_OF(tcp_client_io_manager), REVISION_OF(tcp_client_io_manager)))
{

}
void CControlByTCPRegister::MUnregisterFactory() const
{
	CIOFactory::sMGetInstance().MRemoveFactory(CControlByTCP::NAME);
}
void CControlByTCPRegister::MAdding() const
{
//CControlByTCP* _p =
	CIOFactory::sMAddFactory<CControlByTCP>();
}
bool CControlByTCPRegister::MIsAlreadyRegistered() const
{
	if (CIOFactory::sMGetInstancePtr())
		return CIOFactory::sMGetInstance().MIsFactoryPresent(
				CControlByTCP::NAME);
	return false;

}
unsigned CControlByTCP::MNextUserPacketNumber()
{
	return ++FMainPacketNumber;
}
}
#if !defined(TCP_CLIENT_IO_MANAGER_STATIC)
static NSHARE::factory_registry_t g_factory;
extern "C" TCP_CLIENT_IO_MANAGER_EXPORT NSHARE::factory_registry_t* get_factory_registry()
{
	if (g_factory.empty())
	{
		g_factory.push_back(new NUDT::CControlByTCPRegister());
	}
	return &g_factory;
}
#else
#	include <load_static_module.h>
namespace
{
	static NUDT::CStaticRegister<NUDT::CControlByTCPRegister> _reg;
}
#endif
