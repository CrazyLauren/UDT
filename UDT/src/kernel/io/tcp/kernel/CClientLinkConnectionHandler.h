/*
 * CClientLinkConnectionHandler.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CCLIENTLINKCONNECTIONHANDLER_H_
#define CCLIENTLINKCONNECTIONHANDLER_H_

namespace NUDT
{

class CKernelClientLink::CConnectionHandler:public IConnectionHandler
{
	typedef CInParser<CKernelClientLink::CConnectionHandler> parser_t;
public:
	CConnectionHandler(descriptor_t aFD,
			uint64_t aTime, ILinkBridge* aKer);
	~CConnectionHandler();
	bool MConnect();
	bool MIsOpened() const;

	bool MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);
	ILink* MCreateLink();

	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);
private:

	bool MHandling(NSHARE::CBuffer::const_iterator aBegin,
			NSHARE::CBuffer::const_iterator aEnd);
	bool MSendIDInfo();
	void MOpenLink();
	bool MSetting();
	bool MSendProtocolType();
	inline unsigned MFillProtocol(data_t* aTo,
			const eType& _id);
	bool MSendImpl(const data_t& _buf);

	NSHARE::intrusive_ptr<CKernelClientLink> FLink;
	bool FProtocolIsValid;
	parser_t FParser;
	smart_bridge_t FBridge;
	descriptor_t  Fd;
	uint64_t FTime;
	bool FIsSent;

	NSHARE::smart_field_t<program_id_t> FKernel;
	const eType FPType;
};


} /* namespace NUDT */
#endif /* CCLIENTLINKCONNECTIONHANDLER_H_ */
