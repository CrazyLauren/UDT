// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CAutoSearchByEthernet.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 02.06.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <core/CDataObject.h>
#include <core/CConfigure.h>

#include <internel_protocol.h>
#include "receive_from_autosearchbyethernet.h"
#include <parser_in_protocol.h>


#include "CAutoSearchByEthernet.h"

template<>
NUDT::CAutoSearchByEthernet::singleton_pnt_t NUDT::CAutoSearchByEthernet::singleton_t::sFSingleton =
NULL;
namespace NUDT
{
using namespace NSHARE;

const NSHARE::CText CAutoSearchByEthernet::NAME = "auto_search";
NSHARE::CText const CAutoSearchByEthernet::PORT = "port";

NSHARE::network_port_t const CAutoSearchByEthernet::DEFAULT_PORT =18013;
CAutoSearchByEthernet::CAutoSearchByEthernet()://
		ICore(NAME),//
		FProtocolParse(this)
{
	MInit();
}

CAutoSearchByEthernet::~CAutoSearchByEthernet()
{
}
bool CAutoSearchByEthernet::MStart()
{
	CConfig const* _p = CConfigure::sMGetInstance().MGet().MFind(NAME);

	CUDP::settings_t _settings(DEFAULT_PORT,net_address(),CUDP::settings_t::eBROADCAST);
	if (_p && _p->MIsChild(CUDP::settings_t::NAME))
	{
		VLOG(2) << "From Config " << _p->MToJSON(true);
		CUDP::settings_t _new_settings(_p->MChild(CUDP::settings_t::NAME));

		LOG_IF(WARNING,!_new_settings.MIsValid())
										<< "The port number is not present in the config file"
										<< CConfigure::sMGetInstance().MGetPath()
										<< ".Using standard port " << DEFAULT_PORT;
		if(_new_settings.MIsValid())
			_settings=_new_settings;
	}
	VLOG(2)<<"Settings: "<<_settings;
	bool const _is=FUdp.MOpen(_settings);
	if(!_is)
	{
		LOG(DFATAL)<<"Cannot open socket for auto_search. Settings:"<<_settings;
		return false;
	}

	/** Start thread of receive data. */
	NSHARE::operation_t _op(CAutoSearchByEthernet::sMMainLoop, this,
			NSHARE::operation_t::IO);
	CDataObject::sMGetInstance().MPutOperation(_op);

	return true;
}

/** The static method of thread pool
 *
 * @param WHO
 * @param WHAT
 * @param aData
 * @return The operation has to be removed
 */
NSHARE::eCBRval CAutoSearchByEthernet::sMMainLoop(NSHARE::CThread const* WHO,
		NSHARE::operation_t * WHAT, void* aData)
{
	reinterpret_cast<CAutoSearchByEthernet*>(aData)->MMainLoop();
	return E_CB_REMOVE;
}

/** Infinite loop of receiving message
 * from new kernel
 *
 */
void CAutoSearchByEthernet::MReceiveLoop()
{
	ISocket::data_t _data;
	for (; FUdp.MIsOpen();)
	{
		_data.clear();
		if (FUdp.MReceiveData(&FProtocolParse.FUserData, &_data, 0.0) > 0)
		{
			FProtocolParse.MReceivedData(_data.cbegin(),_data.cend());
		}

	}
}

/** @brief Loop of receive data
 *
 */
void CAutoSearchByEthernet::MMainLoop()
{
	VLOG(2) << "Async receive";

	LOG_IF(FATAL, !FUdp.MIsOpen()) << "Port is closed";

	MSendBroadcast();
	MReceiveLoop();

	VLOG(1) << "Socket closed";

}

/** @brief Create message type of #auto_search_dg_t
 *
 * @param aTo A pointer to buffer
 */
template<>
void CAutoSearchByEthernet::MFill<auto_search_dg_t>(ISocket::data_t* aTo)
{
	VLOG(2) << "Create main channel param DG";

	auto_search_info_t _param;
	_param.FProgramm = get_my_id();

	serialize<auto_search_dg_t, auto_search_info_t>(aTo, _param, routing_t(), error_info_t());

}

/** Handler message auto_search_dg_t
 *
 *
 *	see RECEIVES macro
 * @param aP A pointer to message
 * @param aThis A pointer to parser
 */
template<>
void CAutoSearchByEthernet::MProcess(auto_search_dg_t const* aP, parser_t* aThis)
{
	auto_search_info_t const _sparam(
			deserialize<auto_search_dg_t, auto_search_info_t>(aP,
					(routing_t*) NULL, (error_info_t*) NULL));
	LOG(INFO)<<"Receive info from "<<aThis->FUserData<<" about :"<<_sparam;
}
/** Send information about me
 * to all kernel
 *
 */
void CAutoSearchByEthernet::MSendBroadcast()
{
	ISocket::data_t _buf;
	MFill<auto_search_dg_t>(&_buf);

	LOG(INFO)<<"Send info about me to: "<<FUdp.MGetSetting().FSendTo;

	ISocket::sent_state_t const _error= FUdp.MSend(_buf);

	LOG_IF(DFATAL,!_error.MIs())<<"Cannot send broadcast info as "<<_error;
}
inline void CAutoSearchByEthernet::MInit()
{
	//FUdp.MOpen()
}
NSHARE::CConfig CAutoSearchByEthernet::MSerialize() const
{
	NSHARE::CConfig _config(NAME);
	return _config;
}
} /* namespace NUDT */