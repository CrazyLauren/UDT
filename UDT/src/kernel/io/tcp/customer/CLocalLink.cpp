/*
 * CLocalLink.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 04.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share_socket.h>

#include <udt_share.h>
#include <internel_protocol.h>

#include <core/kernel_type.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/CConfigure.h>
#include <io/ILink.h>
#include <io/CKernelIo.h>
#include <io/main/CMainChannelFactory.h>
#include <io/CLinkDiagnostic.h>
#include "../ILinkBridge.h"
#include "../CConnectionHandlerFactory.h"

#include "receive_from_local_link.h"
#include <parser_in_protocol.h>


#include "CLocalLink.h"
#include "CLocalLinkConnectionHandler.h"
#include "CLocalLinkRegister.h"

using namespace NSHARE;
namespace NUDT
{

NSHARE::CText const CLocalLink::MAIN_CHANNEL_TYPE="channel_for";
NSHARE::CText const CLocalLink::DEFAULT="default";
NSHARE::CText const CLocalLink::DEFAULT_MAIN = "sm";
NSHARE::CText const CLocalLink::NAME="consumer";

CLocalLink::CLocalLink(descriptor_t aFD,
		uint64_t aTime, ILinkBridge* aKer, program_id_t const & aCust) :
		ILink( NAME,aTime), FServiceParser(this), FBridge(aKer), Fd(aFD),FCustomer(aCust)
{
	FMainChannel = NULL;
	memset(&FDisgnostic, 0, sizeof(FDisgnostic));
	FState = E_NOT_OPEN;
	VLOG(2) << "FD:" << Fd;
	split_info _split;
	_split.FType.MSetFlag(split_info::CAN_NOT_SPLIT,true);

	MSetLimits(_split);
}

CLocalLink::~CLocalLink()
{
	MCloseImpl();
}
bool CLocalLink::MIsOpened() const
{
	return FState == E_OPEN;
}
bool CLocalLink::MAccept()
{
	VLOG(2) << "Accept " << FCustomer.FId;
	descriptor_info_t _info;
	_info.FProgramm = FCustomer;
	_info.FTypeLink = NAME;
	_info.FConnectTime = FTime;
	FBridge->MInfo(_info.FInfo);

	//todo не нравиться мне это
	CDescriptors::sMGetInstance().MOpen(Fd, _info);
	if (FpDemands)
	{
		CKernelIo::sMGetInstance().MReceivedData(*FpDemands, MGetID(),
				routing_t(), error_info_t());
		FpDemands.reset();
	}

	if (FpDemandsDgFor)
	{
		CKernelIo::sMGetInstance().MReceivedData(*FpDemandsDgFor, MGetID(),
				routing_t(), error_info_t());
		FpDemandsDgFor.reset();
	}
	if (FpUserDataFor)
	{
		CKernelIo::sMGetInstance().MReceivedData(*FpUserDataFor, MGetID());
		FpUserDataFor.reset();
	}
	if (FpFailSent)
	{
		CKernelIo::sMGetInstance().MReceivedData(*FpFailSent, MGetID(),
				routing_t(), error_info_t());
		FpFailSent.reset();
	}

	VLOG(2) << "Open " << _info << " :" << this;
	return true;
}
void CLocalLink::MCloseRequest()
{
	if (MIsOpened())
		FBridge->MCloseRequest(MGetID());
	else
	{
		MClose();
		FState = E_ERROR;
	}
}
void CLocalLink::MCloseImpl()
{
	VLOG(2) << "Close link";
	if(FMainChannel) MCloseMain();
		FState = E_NOT_OPEN;
	FMainChannel = NULL;
	Fd = CDescriptors::INVALID;
	FServiceParser.MCleanBuffer();
	VLOG(2) << "The link has been closed.";
	CHECK(FState == E_NOT_OPEN);;
}
void CLocalLink::MClose()
{
	MCloseImpl();
}
int CLocalLink::MCloseMain()
{
	VLOG(2) << "Close main.";
	if (!FMainChannel)
		return main_channel_error_param_t::E_NOT_OPENED;

	FMainChannel->MClose(MGetID());
	FMainChannel=NULL;
	return 0;
}
bool CLocalLink::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd)
{
	VLOG(2) << "Receive data from service channel.";
	FServiceParser.MReceivedData(aBegin, aEnd);
	return true;
}
bool CLocalLink::MReceivedData(user_data_t const& _user)
{
	FDisgnostic.MInput(_user);
	if (E_OPEN == FState)
		CKernelIo::sMGetInstance().MReceivedData(_user, MGetID());
	else
	{
		CHECK(FpUserDataFor.get()==NULL);
		FpUserDataFor=SHARED_PTR<user_data_t>(new user_data_t(_user));
	}
	return true;
}
bool CLocalLink::MReceivedData(program_id_t const& _customer, const routing_t& aRoute,error_info_t const& aError)
{
	//todo
	CHECK(false);
	return false;
}
bool CLocalLink::MReceivedData(demand_dgs_t const& _demands, const routing_t& aRoute,error_info_t const& aError)
{
	if (E_OPEN == FState)
		CKernelIo::sMGetInstance().MReceivedData(_demands, MGetID(),aRoute,aError);
	else
		FpDemands=SHARED_PTR<demand_dgs_t>(new demand_dgs_t(_demands));
	return true;
}
bool CLocalLink::MReceivedData(fail_send_t const& aInfo, const routing_t& aRoute,error_info_t const& aError)
{
	VLOG(2)<<"Recv "<<aInfo<<" Routing to "<<aRoute;
	if (E_OPEN == FState)
		CKernelIo::sMGetInstance().MReceivedData(aInfo, MGetID(),aRoute,aError);
	else
		FpFailSent=SHARED_PTR<fail_send_t>(new fail_send_t(aInfo));
	return true;
}
bool CLocalLink::MReceivedData(demand_dgs_for_t const& _demands, const routing_t& aRoute,error_info_t const& aError)
{
	if (E_OPEN == FState)
		CKernelIo::sMGetInstance().MReceivedData(_demands, MGetID(),aRoute,aError);
	else
		FpDemandsDgFor=SHARED_PTR<demand_dgs_for_t>(new demand_dgs_for_t(_demands));
	return true;
}
template<>
inline unsigned CLocalLink::MFill<progs_id_t>(data_t* _buf,
		const progs_id_t& _id, const routing_t& aRoute,error_info_t const& aError)
{
	return (unsigned)serialize<clients_info2_t,progs_id_t>(_buf,_id,aRoute,aError);
}
template<>
inline unsigned CLocalLink::MFill<fail_send_t>(data_t* _buf,
		const fail_send_t& _id, const routing_t& aRoute,error_info_t const& aError)
{
	return (unsigned)serialize<user_data_fail_send_t,fail_send_t>(_buf,_id,aRoute,aError);
}
template<>
inline unsigned CLocalLink::MFill<demand_dgs_t>(data_t* _buf,
		const demand_dgs_t& _id, const routing_t& aRoute,error_info_t const& aError)
{
	return (unsigned)serialize<custom_filters_dg2_t,demand_dgs_t>(_buf,_id,aRoute,aError);
}
template<>
inline unsigned CLocalLink::MFill<program_id_t>(data_t* _buf,
		const program_id_t& _id, const routing_t& aRoute,error_info_t const& aError)
{
	return (unsigned)serialize<requiest_info2_t,program_id_t>(_buf,_id,aRoute,aError);

}

template<>
void CLocalLink::MProcess(protocol_type_dg_t const* aP, parser_t* aThis)
{
	LOG(DFATAL) << "Wtf? receive protocol dg ";
}

template<>
void CLocalLink::MProcess(main_channel_param_t const* aP, parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	if (FMainChannel)
	{
		VLOG(2)<<"Handle main channel param";
		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
		{
			FState=E_ERROR;
			MCloseMain();
		}else
			VLOG(2)<<"the main channel param handled successfully.";
	}else
	{
		FState=E_ERROR;
		main_ch_param_t _sparam(deserialize<main_channel_param_t, main_ch_param_t>(aP, (routing_t*)NULL, (error_info_t*)NULL));
		MSendMainChannelError(_sparam.FType,
				main_channel_error_param_t::E_NO_CHANNEL);
	}
}
template<>
void CLocalLink::MProcess(request_main_channel_param_t const* aP,
		parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	if (FMainChannel)
	{

		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
		{
			FState=E_ERROR;
			MCloseMain();
			MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
					main_channel_error_param_t::E_UNKNOWN);
		}else
			VLOG(2)<<"the main channel param handled successfully.";
	}
	else
	{
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
	}
}
template<>
void CLocalLink::MProcess(close_main_channel_t const* aP, parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	if (FMainChannel)
	{
		FMainChannel->MHandleServiceDG(aP, MGetID());
		MCloseMain();
	}
	else
	{
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
	}
}
template<>
void CLocalLink::MProcess(main_channel_error_param_t const* aP, parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	int _channel_error = aP->FError;
	if (FMainChannel)
	{
		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
			_channel_error = main_channel_error_param_t::E_UNKNOWN;
	}else
	{
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
	}

	if (_channel_error > 0)
	{
		MCloseMain();
		FState = E_ERROR;
	}
}
template<>
void CLocalLink::MProcess(dg_info2_t const* aP, parser_t* aThis)
{
	CHECK_EQ(sizeof(dg_info2_t) + aP->MGetStrSize(),
			aP->MGetDataSize() + aP->FHeadSize);
	if(FState==E_NOT_OPEN)
		return;

	program_id_t _customer(deserialize<dg_info2_t,program_id_t>(aP,NULL,NULL));

	MReceivedData(_customer,routing_t(),error_info_t());
}

template<>
void CLocalLink::MProcess(custom_filters_dg2_t const* aP, parser_t* aThis)
{
	VLOG(2) << "Handle custom filter " << *aP;
	routing_t _uuid;
	demand_dgs_t _demands(
			deserialize<custom_filters_dg2_t, demand_dgs_t>(aP, &_uuid, NULL));

	VLOG(3) << _demands;
	MReceivedData(_demands,routing_t(),error_info_t());
}

template<>
void CLocalLink::MProcess(user_data_fail_send_t const* aP, parser_t* aThis)
{

	routing_t _uuid;
	error_info_t _err;
	fail_send_t _customer(deserialize<user_data_fail_send_t,fail_send_t>(aP,&_uuid,&_err));
	MReceivedData(_customer,_uuid,_err);
}
bool CLocalLink::MSetting()
{
	CHECK(FState == E_NOT_OPEN);
//add new abonent
	LOG(INFO)<< "Setting new client ";
//check for local host
	if (MCreateMainChannel())
	{
		return true;
	}
	else
		FState = E_ERROR;

	LOG(ERROR)<<"Can't create main channel for "<< MGetID()
	<<". The client is ignoring...";
	return false;
}
bool CLocalLink::MOpen()
{
	VLOG(2)<<"Open link for "<<Fd;
	CHECK_EQ(FState,E_NOT_OPEN);
	return MSetting();
}
bool CLocalLink::MOpening(NSHARE::CBuffer::const_iterator aBegin,
		NSHARE::CBuffer::const_iterator aEnd)
{
	VLOG(2) << "Handle new connection buffer. State=" << FState;
	DCHECK_EQ(FState, E_NOT_OPEN);
	MReceivedData(aBegin, aEnd);
	if (FState == E_ERROR)
	{
		LOG(ERROR)<<"Error state";
		return false;
	}
	else if(FMainChannel && FMainChannel->MIsOpen(Fd))
	{
		VLOG(2) << "The main channel is opened for " << FCustomer;
		FState = E_OPEN;
		return false;
	}
	return true;
}
bool CLocalLink::MSend(const program_id_t& _id, const routing_t& aRoute,error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, _id,aRoute,aError);
	return MSend(_buf);
}

bool CLocalLink::MSend(const kernel_infos_array_t& aVal, const routing_t& aRoute,error_info_t const& aError)
{
	data_t _buf;
	progs_id_t _ids;
	kernel_infos_array_t::const_iterator _it = aVal.begin();
	for (; _it != aVal.end(); ++_it)
	{
		_ids.insert(_it->FKernelInfo);
		kern_links_t::const_iterator _jt = _it->FCustomerInfo.begin();
		for (; _jt != _it->FCustomerInfo.end(); ++_jt)
			_ids.insert(_jt->FProgramm);
	}
	MFill(&_buf, _ids,aRoute,aError);
	return MSend(_buf);
}
bool CLocalLink::MSend(const fail_send_t& aVal, const routing_t& aRoute,error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, aVal,aRoute,aError);
	return MSend(_buf);
}
bool CLocalLink::MSend(const demand_dgs_for_t& aVal, const routing_t& aRoute,error_info_t const& aError)
{
	DCHECK_EQ(aVal.size(),1);
	data_t _buf;
	MFill(&_buf, aVal.begin()->second, aRoute, aError);
	return MSend(_buf);
}
bool CLocalLink::MSend(const user_data_t& _id)
{

	LOG_IF(ERROR, !MIsOpened()) << "The Channel  is not inited";
	if (!MIsOpened())
		return false;

	LOG_IF(ERROR,FState!=E_OPEN) << "Invalid state" << (unsigned) FState;
	if (FState != E_OPEN)
		return false;
	VLOG(2) << "Create user data DG";
	CHECK_NOTNULL(FMainChannel);
	bool const _is = FMainChannel->MSend(_id, MGetID());
	FDisgnostic.MOutput(_id, _is);
	return _is;
}
bool CLocalLink::MSend(const data_t& aVal)
{
	CHECK(FBridge);
	return MSendService(aVal);
}
bool CLocalLink::MSendService(const data_t& aVal)
{
	CHECK(FBridge);
	return FBridge->MSend(aVal);
}
NSHARE::CText CLocalLink::MGetMainChannelType(bool aDefOnly)
{
	NSHARE::CText _val(DEFAULT_MAIN);
	NSHARE::CConfig _configure;
	FBridge->MConfig(_configure);
	if(!_configure.MIsEmpty())
	{
		NSHARE::CConfig const& _conf=_configure.MChild(NAME).MChild(MAIN_CHANNEL_TYPE);
		if(!_conf.MIsEmpty())
		{
			VLOG(2)<<"Conf: "<<_conf.MToJSON(true);
			_conf.MGetIfSet(DEFAULT,_val);
			if(!aDefOnly)
				{
				VLOG(2)<<"try find "<<FCustomer.FId.FName;
					_conf.MGetIfSet(FCustomer.FId.FName,_val);
				}
		}
	}
	CHECK(!_val.empty());
	return _val;
}
bool CLocalLink::MCreateMainChannel()
{
	NSHARE::CText _can = MGetMainChannelType();
	IMainChannel* _main = CMainChannelFactory::sMGetInstance().MGetFactory(_can);
	LOG_IF(DFATAL,!_main)<<"Cannot create channel "<<_can<<" for "<<FCustomer;
	if(!_main)
	{
		NSHARE::CText _def_can = MGetMainChannelType(true);
		if(_def_can!=_can)
		{
			_main = CMainChannelFactory::sMGetInstance().MGetFactory(_def_can);
			LOG_IF(DFATAL,!_main)<<"Cannot create def channel "<<_def_can<<" for "<<FCustomer;
		}
	}
	if (_main && _main->MOpen(this,FCustomer,FBridge->MGetAddr()))
	{
		FMainChannel = _main;
		return true;
	}
	return false;
}
int CLocalLink::MSendMainChannelError(NSHARE::CText const& _channel_type,
		unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, _channel_type.c_str());

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(),get_my_id());
	return MSendService(_channel);
}
NSHARE::CConfig CLocalLink::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	FServiceParser.MGetState().MSerialize(_conf);
	_conf.MAdd("id",Fd);
	FDisgnostic.MSerialize(_conf);
	_conf.MAdd(FCustomer.MSerialize());
	_conf.MAdd(MLimits().MSerialize());
	return _conf;
}
NSHARE::CConfig const& CLocalLink::MBufSettingFor(
		NSHARE::CConfig const& aFrom) const
{
	NSHARE::CConfig const& _io = aFrom.MChild(FCustomer.FId.FName);
	if (!_io.MIsEmpty())
	{
		return _io;
	}
	NSHARE::CConfig const& _def = aFrom.MChild(CKernelIo::DEFAULT);
	if (!_def.MIsEmpty())
	{
		return _def;
	};
	return NSHARE::CConfig::sMGetEmpty();
}
namespace
{
static const NSHARE::CText NEW_NAME = "consumer";
class CNewConnection: public IConnectionHandlerFactory
{
public:
	CNewConnection() :
			IConnectionHandlerFactory(NEW_NAME)
	{

	}

	IConnectionHandler* MCreateHandler(descriptor_t aFD, uint64_t aTime,
			ILinkBridge* aKer)
	{
		return new CLocalLink::CConnectionHandler(aFD, aTime, aKer);
	}
	unsigned MGetProtocolNumber() const{
		return E_CONSUMER;
	}
};
}
;

NSHARE::CText const CIOLocalLinkRegister::NAME = "io_local_link";
CIOLocalLinkRegister::CIOLocalLinkRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 3))
{

}
void CIOLocalLinkRegister::MUnregisterFactory() const
{
	CConnectionHandlerFactory::sMGetInstance().MRemoveFactory(NEW_NAME);
}
void CIOLocalLinkRegister::MAdding() const
{
	CConnectionHandlerFactory::sMAddFactory<CNewConnection>();
}
bool CIOLocalLinkRegister::MIsAlreadyRegistered() const
{
	if (CConnectionHandlerFactory::sMGetInstancePtr())
		return CConnectionHandlerFactory::sMGetInstance().MIsFactoryPresent(
				NEW_NAME);
	return false;

}
} /* namespace NUDT */
