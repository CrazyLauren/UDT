/*
 * CNewConnection.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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
	typedef std::list<NSHARE::CText> handlers_name_t;

	CConnectionHandler( uint64_t aTime,
			CServerBridge* aKer,handlers_name_t const& aHandlers );
	~CConnectionHandler();
	bool MConnect();

	IConnectionHandler::eState  MReceivedData(data_t::const_iterator aBegin,
				data_t::const_iterator aEnd);

	uint64_t const FTime; //ms
	NSHARE::CText const& MCurrentHandler() const;
	smart_link_t const&  MGetLink() const;
	NSHARE::intrusive_ptr<CServerBridge> const&  MGetBridge() const;
private:
	typedef NSHARE::intrusive_ptr<IConnectionHandler> handler_t;

	bool MCreateConnectionHandler();

	NSHARE::intrusive_ptr<CServerBridge>  FBridge;
	smart_link_t FLinks;
	descriptor_t Fd;
	handler_t FHandler;
	handlers_name_t FHandlerTypes;
	NSHARE::CText FCurrent;
	unsigned FIteration;
};

} /* namespace NUDT */
#endif /* CNEWCONNECTION_H_ */
