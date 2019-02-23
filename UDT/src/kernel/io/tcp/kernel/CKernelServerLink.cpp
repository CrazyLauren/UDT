/*
 * CKernelServerLink.cpp
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
#include <core/kernel_type.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/CConfigure.h>
#include <io/ILink.h>
#include <io/CKernelIo.h>
#include <io/CLinkDiagnostic.h>
#include <io/main/CMainChannelFactory.h>
#include "../ILinkBridge.h"
#include "../CConnectionHandlerFactory.h"
#include <internel_protocol.h>
#include "receive_from_server_link.h"
#include <parser_in_protocol.h>

#include "CKernelServerLink.h"
#include "CServerLinkConnectionHandler.h"
#include "CKernelServerLinkRegister.h"

using namespace NSHARE;
namespace NUDT
{

NSHARE::CText const CKernelServerLink::MAIN_CHANNEL_TYPE = "channel_for";
NSHARE::CText const CKernelServerLink::DEFAULT = "default";
NSHARE::CText const CKernelServerLink::DEFAULT_MAIN = "tcpser";
NSHARE::CText const CKernelServerLink::NAME = "udt_server";
static const NSHARE::CText NEW_NAME = CKernelServerLink::NAME;

CKernelServerLink::CKernelServerLink(descriptor_t aFD, uint64_t aTime,
		ILinkBridge* aKer, program_id_t const & aKernel) :
		ILink(NAME, aTime), //
		FServiceParser(this), //
		FBridge(aKer), //
		Fd(aFD), //
		FKernel(aKernel)
{
	FMainChannel = NULL;
	MChangeState(E_OPEN);
	FIsAccepted=false;
	VLOG(2) << "FD:" << Fd;

	CHECK_NOTNULL(aKer);
	{
		CConfig _configure;
		aKer->MConfig(_configure);
		if (!_configure.MIsEmpty())
		{
			NSHARE::CConfig const& _conf = _configure.MChild(NAME).MChild(
					split_info::NAME);
			if (!_conf.MIsEmpty())
			{
				MSetLimits(split_info(_conf));
			}
		}
	}
}

CKernelServerLink::~CKernelServerLink()
{
	MClose();
}
bool CKernelServerLink::MIsOpened() const
{
	return FState == E_OPEN;
}
bool CKernelServerLink::MAccept()
{
	VLOG(2) << "Accept " << FKernel.FId;
	return MSetting();
}
void CKernelServerLink::MCloseRequest()
{
	if (MIsOpened())
		FBridge->MCloseRequest(MGetID());
	else
	{
		MClose();
		MChangeState(E_ERROR);
	}
}
void CKernelServerLink::MClose()
{
	VLOG(2) << "Closing clink";
	if (FMainChannel)
		MCloseMain();
	MChangeState(E_NOT_OPEN);
	Fd = CDescriptors::INVALID;
	FServiceParser.MCleanBuffer();
	VLOG(2) << "The link has been closed.";
	CHECK(FState == E_NOT_OPEN);
}
int CKernelServerLink::MCloseMain()
{
	VLOG(2) << "Close main.";
	if (!FMainChannel)
		return main_channel_error_param_t::E_NOT_OPENED;

	FMainChannel->MClose(MGetID());
	FMainChannel = NULL;
	return 0;
}
bool CKernelServerLink::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd)
{
	VLOG(2) << "Receive data from service channel.";
	FServiceParser.MReceivedData(aBegin, aEnd);
	if (FState == E_ERROR)
	{
		LOG(ERROR)<<"Cannot handle data";
		//MClose();
		return false;
	}
	return true;
}
void CKernelServerLink::MSendOpenedIfNeed()
{
	if (!FIsAccepted && FMainChannel && FMainChannel->MIsOpen(Fd))
	{
		data_t _buf;
		serialize<accept_info_t>(&_buf, FKernel, routing_t(), error_info_t());
		FIsAccepted= MSend(_buf);
	}
}
bool CKernelServerLink::MReceivedData(user_data_t& _user)
{
	LOG_IF(DFATAL,!MIsOpened()) << "Receiving data by closed link.";
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(_user, MGetID());
		return true;
	}
	return false;
}
bool CKernelServerLink::MReceivedData(program_id_t const& _customer,
		const routing_t& aRoute, error_info_t const& aError)
{
	if (FKernel != _customer)
		//todo update
		CHECK(false);
	return false;
}
bool CKernelServerLink::MReceivedData(fail_send_t const& aInfo,
		const routing_t& aRoute, error_info_t const& aError)
{
	LOG_IF(ERROR,!MIsOpened()) << "Cannot receive fail send";
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(aInfo, MGetID(), aRoute,
				aError);
		return true;
	}
	return false;
}
bool CKernelServerLink::MReceivedData(kernel_infos_array_t const& aInfo,
		const routing_t& aRoute, error_info_t const& aError)
{
	LOG_IF(INFO,!MIsOpened()) << "Cannot receive info array";
	if (MIsOpened())
	{
		CHECK(FIsAccepted);
		CKernelIo::sMGetInstance().MReceivedData(aInfo, MGetID(), aRoute,
				aError);
		return true;
	}
	return false;
}

bool CKernelServerLink::MReceivedData(demand_dgs_for_t const& _demands,
		const routing_t& aRoute, error_info_t const& aError)
{
	VLOG(2) << "Receive " << _demands;
	LOG_IF(ERROR,!MIsOpened()) << "Cannot receive demands info";
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(_demands, MGetID(), aRoute,
				aError);
		return true;
	}
	return false;
}
bool CKernelServerLink::MReceivedData(demand_dgs_t const& _demands,
		const routing_t& aRoute, error_info_t const& aError)
{
	LOG_IF(ERROR,!MIsOpened()) << "Cannot receive demands";
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(_demands, MGetID(), aRoute,
				aError);
		return true;
	}
	return false;
}

template<>
inline unsigned CKernelServerLink::MFill<fail_send_t>(data_t* _buf,
		const fail_send_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<user_data_fail_send_t>(_buf, _id, aRoute, aError);
}

template<>
inline unsigned CKernelServerLink::MFill<program_id_t>(data_t* _buf,
		const program_id_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<requiest_info2_t>(_buf, _id, aRoute, aError);
}
template<>
inline unsigned CKernelServerLink::MFill<kernel_infos_array_t>(data_t* _buf,
		const kernel_infos_array_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<kernels_info_t>(_buf, _id, aRoute, aError);
}
template<>
inline unsigned CKernelServerLink::MFill<demand_dgs_for_t>(data_t* _buf,
		const demand_dgs_for_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<customers_demands_t>(_buf, _id, aRoute, aError);
}
template<>
void CKernelServerLink::MProcess(accept_info_t const* aP, parser_t* aThis)
{
	program_id_t const _pr(
			deserialize<accept_info_t, program_id_t>(aP, NULL,
					NULL));
	CHECK_EQ(_pr.FId,get_my_id().FId);
	VLOG(2)<<"Receive "<<_pr;

	descriptor_info_t _info;
	_info.FProgramm = FKernel;
	_info.FTypeLink = NAME;
	_info.FConnectTime = FTime;
	FBridge->MInfo(_info.FInfo);
	CDescriptors::sMGetInstance().MOpen(Fd, _info);

	//MReceivedData(_customer, _uuid, _err);
}
template<>
void CKernelServerLink::MProcess(kernels_info_t const* aP, parser_t* aThis)
{
	routing_t _uuid;
	error_info_t _err;
	kernel_infos_array_t _customer(
			deserialize<kernels_info_t, kernel_infos_array_t>(aP, &_uuid,
					&_err));
	MReceivedData(_customer, _uuid, _err);
}

template<>
void CKernelServerLink::MProcess(protocol_type_dg_t const* aP, parser_t* aThis)
{
	LOG(DFATAL)<< "Wtf? receive protocol dg ";
}

template<>
void CKernelServerLink::MProcess(main_channel_param_t const* aP,
		parser_t* aThis)
{

	DCHECK_NOTNULL(FMainChannel);
	if (FMainChannel)
	{
		VLOG(2) << "Handle main channel param";
		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
		{
			FState = E_ERROR;
			MCloseMain();
		}
		else
			VLOG(2) << "the main channel param handled successfully.";
		MSendOpenedIfNeed();
	}
	else
	{
		FState = E_ERROR;
		main_ch_param_t _sparam(
				deserialize<main_channel_param_t, main_ch_param_t>(aP,
						(routing_t*) NULL, (error_info_t*) NULL));
		MSendMainChannelError(_sparam.FType,
				main_channel_error_param_t::E_NO_CHANNEL);
	}
}
template<>
void CKernelServerLink::MProcess(request_main_channel_param_t const* aP,
		parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	if (FMainChannel)
	{

		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
		{
			FState = E_ERROR;
			MCloseMain();
			MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
					main_channel_error_param_t::E_UNKNOWN);
		}
		else
			VLOG(2) << "the main channel param handled successfully.";
		MSendOpenedIfNeed();
	}
	else
	{
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
	}
}
template<>
void CKernelServerLink::MProcess(close_main_channel_t const* aP,
		parser_t* aThis)
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
void CKernelServerLink::MProcess(main_channel_error_param_t const* aP,
		parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	int _channel_error = aP->FError;
	if (FMainChannel)
	{
		VLOG(2) << "Handle main channel error";
		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
			_channel_error = main_channel_error_param_t::E_UNKNOWN;
		MSendOpenedIfNeed();
	}
	else
	{
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
	}
	if (_channel_error > 0)
	{

		MCloseMain();
		MChangeState(E_ERROR);
	}
}
template<>
void CKernelServerLink::MProcess(dg_info2_t const* aP, parser_t* aThis)
{
	routing_t _uuid;
	error_info_t _err;
	program_id_t _customer(
			deserialize<dg_info2_t, program_id_t>(aP, &_uuid, &_err));
	MReceivedData(_customer, _uuid, _err);
}
template<>
void CKernelServerLink::MProcess(customers_demands_t const* aP, parser_t* aThis)
{
	routing_t _uuid;
	error_info_t _err;
	demand_dgs_for_t _customer(
			deserialize<customers_demands_t, demand_dgs_for_t>(aP, &_uuid,
					&_err));
	MReceivedData(_customer, _uuid, _err);
}
template<>
void CKernelServerLink::MProcess(user_data_fail_send_t const* aP,
		parser_t* aThis)
{

	routing_t _uuid;
	error_info_t _err;
	fail_send_t _customer(
			deserialize<user_data_fail_send_t, fail_send_t>(aP, &_uuid, &_err));
	MReceivedData(_customer, _uuid, _err);
}
template<>
void CKernelServerLink::MProcess(custom_filters_dg2_t const* aP,
		parser_t* aThis)
{
	VLOG(2) << "Handle custom filter " << *aP;
	routing_t _uuid;
	demand_dgs_t _demands(
			deserialize<custom_filters_dg2_t, demand_dgs_t>(aP, &_uuid, NULL));

	VLOG(3) << _demands;
	MReceivedData(_demands, routing_t(), error_info_t());
}

bool CKernelServerLink::MSetting()
{
//add new abonent
	LOG(INFO)<< "Setting new client ";
//check for local host
	if (MCreateMainChannel())
	{
		return true;
	}
	else
		MChangeState(E_ERROR);

	LOG(ERROR)<<"Can't create main channel for "<< MGetID()
	<<". The client is ignoring...";
	return false;
}
bool CKernelServerLink::MSendIDInfo()
{
	data_t _buf;
	MFill(&_buf, get_my_id(), routing_t(), error_info_t());
	CHECK(FBridge);
	bool _is = MSendService(_buf);
	LOG_IF(ERROR,!_is) << "Cannot send id info";
	return _is;

}
bool CKernelServerLink::MIsMainChannel() const
{
	return FMainChannel!=NULL;
}
void CKernelServerLink::MChangeState(eState aNew)
{
	VLOG(2) << "New state " << aNew << " Previous " << FState;
	FState = aNew;
}
bool CKernelServerLink::MSend(const program_id_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, _id, aRoute, aError);
	return MSend(_buf);
}

bool CKernelServerLink::MSend(const kernel_infos_array_t& aVal,
		const routing_t& aRoute, error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, aVal, aRoute, aError);
	return MSend(_buf);
}
bool CKernelServerLink::MSend(const demand_dgs_for_t& aVal,
		const routing_t& aRoute, error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, aVal, aRoute, aError);
	return MSend(_buf);
}
bool CKernelServerLink::MSend(const fail_send_t& aVal, const routing_t& aRoute,
		error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, aVal, aRoute, aError);
	return MSend(_buf);
}

bool CKernelServerLink::MSend(const user_data_t& _id)
{

	LOG_IF(ERROR, !MIsOpened()) << "The Channel  is not inited";
	if (!MIsOpened())
		return false;

	LOG_IF(ERROR,!MIsOpened()) << "Invalid state" << (unsigned) FState;
	if (!MIsOpened())
		return false;
	VLOG(2) << "Create user data DG";
	CHECK_NOTNULL(FMainChannel);
	return FMainChannel->MSend(_id, MGetID());
}
bool CKernelServerLink::MSend(const data_t& aVal)
{

	CHECK(FBridge);
	return MSendService(aVal);
}
bool CKernelServerLink::MSendService(const data_t& aVal)
{
	CHECK(FBridge);
	return FBridge->MSend(aVal);
}
NSHARE::CText CKernelServerLink::MGetMainChannelType(bool aDefOnly)
{
	NSHARE::CText _val(DEFAULT_MAIN);
	NSHARE::CConfig _configure;
	FBridge->MConfig(_configure);
	if (!_configure.MIsEmpty())
	{
		NSHARE::CConfig const& _conf = _configure.MChild(NAME).MChild(
				MAIN_CHANNEL_TYPE);
		if (!_conf.MIsEmpty())
		{
			_conf.MGetIfSet(DEFAULT, _val);
			if (!aDefOnly)
				_conf.MGetIfSet(FKernel.FId.FName, _val);
		}
	}
	CHECK(!_val.empty());
	return _val;
}
bool CKernelServerLink::MCreateMainChannel()
{
	NSHARE::CText _can = MGetMainChannelType();
	IMainChannel* _main = CMainChannelFactory::sMGetInstance().MGetFactory(
			_can);
	LOG_IF(DFATAL,!_main) << "Cannot create channel " << _can << " for "
									<< FKernel;
	if (!_main)
	{
		NSHARE::CText _def_can = MGetMainChannelType(true);
		if (_def_can != _can)
		{
			_main = CMainChannelFactory::sMGetInstance().MGetFactory(_def_can);
			LOG_IF(DFATAL,!_main) << "Cannot create def channel " << _def_can
											<< " for " << FKernel;
		}
	}
	if (_main && _main->MOpen(this, FKernel, FBridge->MGetAddr()))
	{
		FMainChannel = _main;
		return true;
	}
	return false;
}
int CKernelServerLink::MSendMainChannelError(NSHARE::CText const& _channel_type,
		unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, _channel_type.c_str());

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(), get_my_id());
	return MSendService(_channel);
}
NSHARE::CConfig CKernelServerLink::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	FServiceParser.MGetState().MSerialize(_conf);
	_conf.MAdd(CDescriptors::DESCRIPTOR_NAME, Fd);
	_conf.MAdd(FKernel.MSerialize());
	_conf.MAdd(MLimits().MSerialize());
	FDiagnostic.MSerialize(_conf);
	return _conf;
}
NSHARE::CConfig const& CKernelServerLink::MBufSettingFor(
		NSHARE::CConfig const& aFrom) const
{
	NSHARE::CConfig const& _io = aFrom.MChild(FKernel.FId.FName);
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
		return new CKernelServerLink::CConnectionHandler(aFD, aTime, aKer);
	}
	virtual unsigned MGetProtocolNumber() const
	{
		return E_KERNEL;
	}
};
}
;

NSHARE::CText const CKernelServerLinkRegister::NAME = "io_kernel_server_link";
CKernelServerLinkRegister::CKernelServerLinkRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 3))
{

}
void CKernelServerLinkRegister::MUnregisterFactory() const
{
	CConnectionHandlerFactory::sMGetInstance().MRemoveFactory(NEW_NAME);
}
void CKernelServerLinkRegister::MAdding() const
{
	CConnectionHandlerFactory::sMAddFactory<CNewConnection>();
}
bool CKernelServerLinkRegister::MIsAlreadyRegistered() const
{
	if (CConnectionHandlerFactory::sMGetInstancePtr())
		return CConnectionHandlerFactory::sMGetInstance().MIsFactoryPresent(
				NEW_NAME);
	return false;

}
} /* namespace NUDT */
