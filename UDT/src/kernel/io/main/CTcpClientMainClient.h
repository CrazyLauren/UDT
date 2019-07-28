/*
 * CTcpClientMainImpl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTCPCLIENTMAINIMPL_H_
#define CTCPCLIENTMAINIMPL_H_
//#include "CSendBuffering.h"
namespace NUDT
{

class CTcpClientMainChannel::CMainClient: public NSHARE::IIntrusived
{

public:
	typedef CInParser<CMainClient, CMainClient*> parser_t;

	enum eSendResult
	{
		E_SENT, E_SENT_ERROR, E_MUST_CLOSE,
	};
	enum eState
	{
		E_OPENED, //
		E_CLOSED, //
		E_BAN_SENT

	};

	CMainClient(CTcpClientMainChannel& aThis,
			NSHARE::net_address const& aClient,ILink*,NSHARE::IAllocater * const,size_t aLimit);

	int  MReceive();
	void MReceivedData(data_t::const_iterator const& _buf_begin,
			data_t::const_iterator const& aEnd);

	bool MIsOpened() const
	{
		return FState == E_OPENED;
	}

	bool MClose();
	bool MCloseRequest();
	void MProcess(user_data_dg_t const* aP, parser_t*);
	void MProcess(main_channel_param_t const* aP, parser_t*);

	descriptor_t MId() const
	{
		CHECK_NOTNULL(FHandler);
		return FHandler->MGetID();
	}

	bool MSendMainChannelParam() const;
	bool MSendService(const data_t& aVal) const;
	bool MSend(user_data_t const& aData);


	bool MSendMainChannelError(unsigned aError);
	void MAddToSelect();
	void MRemoveFromSelect();
	void MSerialize(NSHARE::CConfig &) const;

	mutable NSHARE::CTCP FTcp;
	NSHARE::net_address const FAddr;
private:
	typedef COverload<unsigned> net_overload_t;

	template<class DG_T> void MFill(data_t*) const;
	split_info MGetLimits(size_t aSize) const;
	static NSHARE::eCBRval sMConnect(void* aWho, void* aWhat, void* aThis);
	static NSHARE::eCBRval sMDisconnect(void* aWho, void* aWhat, void* aThis);
	static int sMSend(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT,	void* YOU_DATA);
	//void MSendImpl();
	eSendResult MSendImpl(
			user_data_t const& _id) const;
	bool MSendImpl(data_t const&) const;

	mutable eState FState;
	ILink* FHandler;
	parser_t FParser;
	CTcpClientMainChannel& FThis;
	net_overload_t FOverload;
	NSHARE::CSocket FCurrentSocket;
	//CSendBuffering FBuffer;
	NSHARE::CMutex FMutexSend;
	NSHARE::IAllocater * const FBufferAlloc;

	friend class CInParser<CMainClient, CMainClient*> ;
};

} /* namespace NUDT */
#endif /* CTCPCLIENTMAINIMPL_H_ */
