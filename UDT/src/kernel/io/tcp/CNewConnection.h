/*
 * CNewConnection.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CNEWCONNECTION_H_
#define CNEWCONNECTION_H_
#include "INewConnection.h"

namespace NUDT
{
class CKernelIOByTCP::CConnectionHandler:NSHARE::CDenyCopying,public NSHARE::CIntrusived
{
public:
	typedef CInParser<CKernelIOByTCP::CConnectionHandler> parser_t;

	CConnectionHandler( uint64_t aTime,
			CServerBridge* aKer,handlers_name_t const& aHandlers);
	~CConnectionHandler();

	IConnectionHandler::eState  MReceivedData(data_t::const_iterator aBegin,
				data_t::const_iterator aEnd);

	uint64_t const FTime; //ms
	smart_link_t const&  MGetLink() const;
	NSHARE::intrusive_ptr<CServerBridge> const&  MGetBridge() const;
	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);
private:
	typedef NSHARE::intrusive_ptr<IConnectionHandler> handler_t;


	NSHARE::intrusive_ptr<CServerBridge>  FBridge;
	smart_link_t FLinks;
	descriptor_t Fd;
	handler_t FHandler;
	parser_t FParser;
	handlers_name_t const FHandlerTypes;
};

} /* namespace NUDT */
#endif /* CNEWCONNECTION_H_ */
