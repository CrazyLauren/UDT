/*
 * CTcpServerMainClient.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTCPSERVERMAINCLIENT_H_
#define CTCPSERVERMAINCLIENT_H_

//#include "CSendBuffering.h"
namespace NUDT
{

class CTcpServerMainChannel::CMainClient: public NSHARE::CIntrusived
{
public:
	typedef CInParser<CMainClient, CMainClient*> parser_t;
	enum eState
	{
		E_OPENED, //
		E_CLOSED, //
		E_BAN_SENT

	};
	enum eSendResult
	{
		E_SENT, E_SENT_ERROR, E_MUST_CLOSE,
	};

	CMainClient(CTcpServerMainChannel& aThis,
			NSHARE::CTCPServer::client_t const& aClient, NSHARE::IAllocater * const aAlloc,ILink* );

	void MReceivedData(data_t::const_iterator const& _buf_begin,
			data_t::difference_type aSize);

	bool MClose();
	bool MCloseRequest();
	void MProcess(user_data_dg_t const* aP, parser_t*);
	void MProcess(main_channel_param_t const* aP, parser_t*);

	bool MIsOpened() const
	{
		return FState == E_OPENED;
	}
	NSHARE::CTCPServer::client_t const& MClient() const
	{
		return FClient;
	}
	descriptor_t MId() const
	{
		CHECK_NOTNULL(FHandler);
		return FHandler->MGetID();
	}
	parser_t::state_t const& MGetState() const;
	bool MSendService(const data_t& aVal) const;


	//eSendResult MSend(CSendBuffering::data_list_t & aWhat);
	eSendResult MSend(user_data_t const & aWhat);

	int MSendMainChannelError(unsigned aError);
private:
//	eSendResult MSendImpl(user_data_t const& _id) const;

	ILink* FHandler;
	parser_t FParser;
	CTcpServerMainChannel& FThis;
	mutable eState FState;
	NSHARE::CTCPServer::client_t const FClient;
	NSHARE::IAllocater * const FBufferAlloc;

	friend class CInParser<CMainClient, CMainClient*> ;
};
} /* namespace NUDT */
#endif /* CTCPSERVERMAINCLIENT_H_ */
