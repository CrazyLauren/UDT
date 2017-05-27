/*
 * CLocalLinkConnectionHandler.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CLOCALLINKCONNECTIONHANDLER_H_
#define CLOCALLINKCONNECTIONHANDLER_H_

namespace NUDT
{

class CLocalLink::CConnectionHandler:public IConnectionHandler
{
	typedef CInParser<CLocalLink::CConnectionHandler> parser_t;
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

	NSHARE::intrusive_ptr<CLocalLink> FLink;
	bool FProtocolIsValid;
	eConnectionState FConnectionState;
	parser_t FParser;
	smart_bridge_t FBridge;
	descriptor_t  Fd;
	uint64_t FTime;

	NSHARE::smart_field_t<program_id_t> FKernel;
	bool FIsSent;
	const eType FPType;
};
} /* namespace NUDT */
#endif /* CLOCALLINKCONNECTIONHANDLER_H_ */
