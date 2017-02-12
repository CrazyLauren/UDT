/*
 * CServerLinkConnectionHandler.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 04.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSERVERLINKCONNECTIONHANDLER_H_
#define CSERVERLINKCONNECTIONHANDLER_H_

namespace NUDT
{

class CKernelServerLink::CConnectionHandler:public IConnectionHandler
{
	typedef CInParser<CKernelServerLink::CConnectionHandler> parser_t;
public:
	CConnectionHandler(descriptor_t aFD,
			uint64_t aTime, ILinkBridge* aKer);
	~CConnectionHandler();
	bool MConnect();

	bool MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);
	ILink* MCreateLink();
	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);
private:
	enum eConnectionState //don't use pattern
	//as it's  difficult for the class
	{
		E_NO_STATE,
		E_CONNECTED, //
		E_REQUEST_ID,
		E_HAS_ID
	};


	bool MHandling(NSHARE::CBuffer::const_iterator aBegin,
			NSHARE::CBuffer::const_iterator aEnd);
	bool MSendIDInfo();
	bool MOpenLink();
	bool MSetting();
	NSHARE::CText MGetMainChannelType();
	bool MSendImpl(const data_t& _buf);

	NSHARE::intrusive_ptr<CKernelServerLink> FLink;
	bool FProtocolIsValid;
	eConnectionState FConnectionState;
	parser_t FParser;
	smart_bridge_t FBridge;
	descriptor_t  Fd;
	uint64_t FTime;

	NSHARE::smart_field_t<program_id_t> FKernel;
	bool FIsSent;
};

} /* namespace NUDT */
#endif /* CSERVERLINKCONNECTIONHANDLER_H_ */
