// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CKernelClientLink.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 29.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <share/share_socket.h>

#include <udt_share.h>
#include <internel_protocol.h>
#include "receive_from_client_link.h"
#include <parser_in_protocol.h>
#include <core/kernel_type.h>
#include <core/CDescriptors.h>
#include <core/CDataObject.h>
#include <core/CConfigure.h>
#include <io/main/CMainChannelFactory.h>
#include <io/CKernelIo.h>
#include <io/ILink.h>
#include <io/CLinkDiagnostic.h>
#include "../ILinkBridge.h"
#include "../CConnectionHandlerFactory.h"
#include "CKernelLinkRegister.h"
#include "CKernelClientLink.h"
#include "CClientLinkConnectionHandler.h"

using namespace NSHARE;
namespace NUDT
{

NSHARE::CText const CKernelClientLink::DEFAULT = "default";
NSHARE::CText const CKernelClientLink::DEFAULT_MAIN = "tcp";
NSHARE::CText const CKernelClientLink::NAME = "udt_client";
NSHARE::CText const CKernelClientLink::MAIN_CHANNEL_TYPE = "channel_for";

CKernelClientLink::CKernelClientLink(descriptor_t aFD, uint64_t aTime,
		ILinkBridge* aKer, program_id_t const & aKernel) :
		ILink(NAME, aTime), //
		FServiceParser(this), //
		FBridge(aKer), //
		Fd(aFD), //
		FKernel(aKernel)
{
	FMainChannel = NULL;
	MChangeState(E_OPEN);
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
	FIsAccepted=false;
	//FBridge
//	split_info _split;
//	_split.FType.MSetFlag(split_info::CAN_NOT_SPLIT,true);
//	return _split;
}
CKernelClientLink::CKernelClientLink(CKernelClientLink const & aRht) :
		ILink(aRht), FServiceParser(this), FBridge(aRht.FBridge), Fd(aRht.Fd), FMainChannel(
				aRht.FMainChannel),FIsAccepted(aRht.FIsAccepted)
{
	FState = aRht.FState;
}
CKernelClientLink::~CKernelClientLink()
{
	MClose();
}
bool CKernelClientLink::MIsOpened() const
{
	return FState == E_OPEN;
}
bool CKernelClientLink::MAccept()
{
	VLOG(2) << "Accept " << FKernel.FId;
	return true;
}
void CKernelClientLink::MCloseRequest()
{
	if (MIsOpened())
		FBridge->MCloseRequest(MGetID());
	else
	{
		MClose();
		MChangeState(E_ERROR);
	}
}
void CKernelClientLink::MClose()
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
int CKernelClientLink::MCloseMain()
{
	LOG_IF(ERROR,!MIsOpened()) << "Invalid state" << (unsigned) FState;
	VLOG(2) << "Close main.";
	if (!FMainChannel)
		return main_channel_error_param_t::E_NOT_OPENED;

	FMainChannel->MClose(MGetID());
	FMainChannel = NULL;

	return 0;
}
bool CKernelClientLink::MReceivedData(data_t::const_iterator aBegin,
		data_t::const_iterator aEnd)
{
	VLOG(2) << "Receive data from service channel.";
	FServiceParser.MReceivedData(aBegin, aEnd);
	if (FState == E_ERROR)
	{
		LOG(ERROR)<<"Cannot handle data";
		return false;
	}
	/*	else if(FMainChannel && FMainChannel->MIsOpen(Fd))
	 {
	 VLOG(2) << "The main channel is opened for " << FKernel;
	 MChangeState(E_OPEN);
	 return false;
	 }*/
	return true;
}
bool CKernelClientLink::MReceivedData(user_data_t& _user)
{
	LOG_IF(DFATAL,!MIsOpened()) << "Receiving data by closed link.";
	FDiagnostic.MInput(_user);
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(_user, MGetID());
		return true;
	}
	return false;
}
bool CKernelClientLink::MReceivedData(program_id_t const& _customer,
		const routing_t& aRoute, error_info_t const& aError)
{
	//todo update
	CHECK(CDescriptors::sMIsValid(MGetID()));

	VLOG(2) << "New Kernel " << _customer;
	//FKernel = _customer;
	return false;
}
bool CKernelClientLink::MReceivedData(fail_send_t const& aInfo,
		const routing_t& aFrom, error_info_t const& aError)
{
	LOG_IF(ERROR,!MIsOpened()) << "Cannot receive fail sent";
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(aInfo, MGetID(), aFrom,
				aError);
		return true;
	}
	return false;

}
bool CKernelClientLink::MReceivedData(kernel_infos_array_t const& aInfo,
		const routing_t& aRoute, error_info_t const& aError)
{
	LOG_IF(ERROR,!MIsOpened()) << "Cannot receive info array";

	if (MIsOpened())
	{
		CHECK(FIsAccepted);
		CKernelIo::sMGetInstance().MReceivedData(aInfo, MGetID(), aRoute,
				aError);
		return true;
	}
	return false;
}

bool CKernelClientLink::MReceivedData(demand_dgs_for_t const& _demands,
		const routing_t& aRoute, error_info_t const& aError)
{
	VLOG(2) << "Receive " << _demands << " Route " << aRoute << " Error:"
						<< aError;
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(_demands, MGetID(), aRoute,
				aError);
		return true;
	}
	return false;
}
bool CKernelClientLink::MReceivedData(demand_dgs_t const& _demands,
		const routing_t& aRoute, error_info_t const& aError)
{
	if (MIsOpened())
	{
		CKernelIo::sMGetInstance().MReceivedData(_demands, MGetID(), aRoute,
				aError);
		return true;
	}
	return false;
}

template<>
inline unsigned CKernelClientLink::MFill<fail_send_t>(data_t* _buf,
		const fail_send_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<user_data_fail_send_t>(_buf, _id, aRoute, aError);
}

template<>
inline unsigned CKernelClientLink::MFill<program_id_t>(data_t* _buf,
		const program_id_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<dg_info2_t>(_buf, _id, aRoute, aError);

}
template<>
inline unsigned CKernelClientLink::MFill<kernel_infos_array_t>(data_t* _buf,
		const kernel_infos_array_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<kernels_info_t>(_buf, _id, aRoute, aError);
}
template<>
inline unsigned CKernelClientLink::MFill<demand_dgs_for_t>(data_t* _buf,
		const demand_dgs_for_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	return (unsigned)serialize<customers_demands_t>(_buf, _id, aRoute, aError);
}
template<>
void CKernelClientLink::MProcess(accept_info_t const* aP, parser_t* aThis)
{
	program_id_t const _pr(
			deserialize<accept_info_t, program_id_t>(aP, NULL,
					NULL));
	VLOG(2) << "Receive " << _pr;
	CHECK_EQ(_pr.FId,get_my_id().FId);
	descriptor_info_t _info;
	_info.FProgramm = FKernel;
	_info.FTypeLink = NAME;
	_info.FConnectTime = FTime;
	FBridge->MInfo(_info.FInfo);

	CDescriptors::sMGetInstance().MOpen(Fd, _info);

	//MReceivedData(_customer, _uuid, _err);
}
template<>
void CKernelClientLink::MProcess(protocol_type_dg_t const* aP, parser_t* aThis)
{
	LOG(DFATAL)<<"Wtf?";
}
template<>
void CKernelClientLink::MProcess(requiest_info2_t const* aP, parser_t*)
{
	VLOG(2) << "Request info from " << *aP;

	routing_t _r;
	error_info_t _error;
	program_id_t _customer(
			deserialize<requiest_info2_t, program_id_t>(aP, &_r, &_error));
	MReceivedData(_customer, _r, _error);
}
template<>
void CKernelClientLink::MProcess(main_channel_param_t const* aP,
		parser_t* aThis)
{
	if (!FMainChannel)
	{
		main_ch_param_t _sparam(
				deserialize<main_channel_param_t, main_ch_param_t>(aP,
						(routing_t*) NULL, (error_info_t*) NULL));
		int const _rval = MOpenMainChannel(_sparam.FType);
		if (_rval != main_channel_error_param_t::E_OK)
		{
			MSendMainChannelError(_sparam.FType, _rval);
		}
		MSendOpenedIfNeed();
	}
	if (FMainChannel)
	{
		VLOG(2) << "Handling by main channel.";
		//for interface uniformity,repeating parsing
		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
		{
			MCloseMain();
			MChangeState(E_ERROR);
		}
	}
}
template<>
void CKernelClientLink::MProcess(request_main_channel_param_t const* aP,
		parser_t* aThis)
{
	bool _is_error = false;
	DCHECK_NOTNULL(FMainChannel);
	if (FMainChannel)
	{
		if (FMainChannel->MIsOpen(MGetID()))
		{
			if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
				_is_error = true;
		}
		else
			_is_error = true;
		MSendOpenedIfNeed();
	}
	else
	{
		_is_error = true;
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
	}

	if (_is_error)
	{

		MCloseMain();
		MChangeState(E_ERROR);
	}
}
template<>
void CKernelClientLink::MProcess(close_main_channel_t const* aP,
		parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	if (FMainChannel)
	{
		FMainChannel->MHandleServiceDG(aP, MGetID());
		MCloseMain();
	}
}
template<>
void CKernelClientLink::MProcess(main_channel_error_param_t const* aP,
		parser_t* aThis)
{
	DCHECK_NOTNULL(FMainChannel);
	bool _is_error = aP->FError != main_channel_error_param_t::E_OK;
	if (FMainChannel)
	{
		if (!FMainChannel->MHandleServiceDG(aP, MGetID()))
			_is_error = true;
		MSendOpenedIfNeed();
	}
	else
	{
		MSendMainChannelError(NSHARE::CText((utf8*) aP->FType),
				main_channel_error_param_t::E_NO_CHANNEL);
		_is_error = true;
	}

	if (_is_error)
	{
		LOG(ERROR)<<"Cannot open main channel for "<<FKernel;

		MCloseMain();
		MChangeState(E_ERROR);
	}
}

template<>
void CKernelClientLink::MProcess(kernels_info_t const* aP, parser_t* aThis)
{
	routing_t _r;
	error_info_t _error;
	kernel_infos_array_t _customer(
			deserialize<kernels_info_t, kernel_infos_array_t>(aP, &_r,
					&_error));
	MReceivedData(_customer, _r, _error);
}

template<>
void CKernelClientLink::MProcess(dg_info2_t const* aP, parser_t* aThis)
{
	CHECK(false);
}
template<>
void CKernelClientLink::MProcess(customers_demands_t const* aP, parser_t* aThis)
{

	routing_t _uuid;
	error_info_t _error;
	demand_dgs_for_t _customer(
			deserialize<customers_demands_t, demand_dgs_for_t>(aP, &_uuid,
					&_error));
	MReceivedData(_customer, _uuid, _error);
}
template<>
void CKernelClientLink::MProcess(user_data_fail_send_t const* aP,
		parser_t* aThis)
{

	routing_t _uuid;
	error_info_t _error;
	fail_send_t _customer(
			deserialize<user_data_fail_send_t, fail_send_t>(aP, &_uuid,
					&_error));
	MReceivedData(_customer, _uuid, _error);
}

template<>
void CKernelClientLink::MProcess(custom_filters_dg2_t const* aP,
		parser_t* aThis)
{
	CHECK(false); //todo
}

bool CKernelClientLink::MSendIDInfo()
{
	VLOG(2) << "Send id info.";

	data_t _buf;
	MFill(&_buf, get_my_id(), routing_t(), error_info_t());
	CHECK(FBridge);
	bool _is = MSendService(_buf);
	LOG_IF(ERROR,!_is) << "Cannot send id info";
	return _is;
}
bool CKernelClientLink::MSend(const program_id_t& _id, const routing_t& aRoute,
		error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, _id, aRoute, aError);
	return MSend(_buf);
}

bool CKernelClientLink::MSend(const kernel_infos_array_t& aVal,
		const routing_t& aRoute, error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, aVal, aRoute, aError);
	return MSend(_buf);
}
bool CKernelClientLink::MSend(const fail_send_t& aVal, const routing_t& aRoute,
		error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, aVal, aRoute, aError);
	return MSend(_buf);
}
bool CKernelClientLink::MSend(const demand_dgs_for_t& aVal,
		const routing_t& aRoute, error_info_t const& aError)
{
	data_t _buf;
	MFill(&_buf, aVal, aRoute, aError);
	return MSend(_buf);
}
bool CKernelClientLink::MSend(const user_data_t& _id)
{

	LOG_IF(ERROR, !MIsOpened()) << "The Channel  is not inited";
	if (!MIsOpened())
		return false;

	LOG_IF(ERROR,!MIsOpened()) << "Invalid state" << (unsigned) FState;
	if (!MIsOpened())
		return false;
	VLOG(2) << "Sending user data ";
	CHECK_NOTNULL(FMainChannel);
	bool const _is = FMainChannel->MSend(_id, MGetID());
	FDiagnostic.MOutput(_id, _is);
	return _is;
}
bool CKernelClientLink::MSend(const data_t& aVal)
{

	CHECK(FBridge);
	return MSendService(aVal);
}
bool CKernelClientLink::MSendService(const data_t& aVal)
{
	CHECK(FBridge);
	return FBridge->MSend(aVal);
}

void CKernelClientLink::MChangeState(eState aNew)
{
	VLOG(2) << "New state " << aNew << " Previous " << FState;
	FState = aNew;
}
int CKernelClientLink::MOpenMainChannel(NSHARE::CText const& aType)
{
	VLOG(2) << "Open main channel " << aType;

	IMainChannel* _main = CMainChannelFactory::sMGetInstance().MGetFactory(
			aType);
	LOG_IF(ERROR,!_main) << "The main channel " << aType << " is not exist.";
	if (!_main)
	{
		return main_channel_error_param_t::E_NO_CHANNEL;
	}
	else
	{
		//trying to open
		if (_main->MOpen(this, FKernel, FBridge->MGetAddr()))
		{
			LOG(INFO)<< "Main channel " << aType
			<< " opened sucsessfull.";
			FMainChannel = _main;
		}
		else
		{
			LOG(ERROR) << "Cannot open the " << aType << " channel.";
			return main_channel_error_param_t::E_NOT_OPENED;
		}

	}
	return main_channel_error_param_t::E_OK;
}
int CKernelClientLink::MSendMainChannelError(NSHARE::CText const& _channel_type,
		unsigned aError)
{
	VLOG(2) << "Sending error= " << aError << " for channel "
						<< _channel_type;
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
NSHARE::CConfig CKernelClientLink::MSerialize() const
{
	NSHARE::CConfig _conf(NAME);
	FServiceParser.MGetState().MSerialize(_conf);
	_conf.MAdd(CDescriptors::DESCRIPTOR_NAME, Fd);
	_conf.MAdd(FKernel.MSerialize());
	FDiagnostic.MSerialize(_conf);
	return _conf;
}
NSHARE::CConfig const& CKernelClientLink::MBufSettingFor(
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
void CKernelClientLink::MSendOpenedIfNeed()
{
	if (!FIsAccepted && FMainChannel && FMainChannel->MIsOpen(Fd))
	{
		data_t _buf;
		serialize<accept_info_t>(&_buf, FKernel, routing_t(), error_info_t());
		FIsAccepted= MSend(_buf);
	}
}
namespace
{

class CNewConnectionClient: public IConnectionHandlerFactory
{
public:
	CNewConnectionClient() :
			IConnectionHandlerFactory(CKernelClientLink::NAME)
	{

	}

	IConnectionHandler* MCreateHandler(descriptor_t aFD, uint64_t aTime,
			ILinkBridge* aKer)
	{
		return new CKernelClientLink::CConnectionHandler(aFD, aTime, aKer);
	}
	virtual unsigned MGetProtocolNumber() const
	{
		return E_KERNEL;
	}
};
}
;

NSHARE::CText const CIOKernelLinkRegister::NAME = "io_kernel_client_link";
CIOKernelLinkRegister::CIOKernelLinkRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(0, 3))
{

}
void CIOKernelLinkRegister::MUnregisterFactory() const
{
	CConnectionHandlerFactory::sMGetInstance().MRemoveFactory(CKernelClientLink::NAME);
}
void CIOKernelLinkRegister::MAdding() const
{
	CConnectionHandlerFactory::sMAddFactory<CNewConnectionClient>();
}
bool CIOKernelLinkRegister::MIsAlreadyRegistered() const
{
	if (CConnectionHandlerFactory::sMGetInstancePtr())
		return CConnectionHandlerFactory::sMGetInstance().MIsFactoryPresent(
				CKernelClientLink::NAME);
	return false;

}
} /* namespace NUDT */
