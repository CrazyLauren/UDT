/*
 * CSmMainChannel.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSMMAINCHANNEL_H_
#define CSMMAINCHANNEL_H_


#include "IMainChannel.h"
namespace NUDT
{

class CSmMainChannel: public IMainChannel
{
public:
	static const NSHARE::CText PATH_HASH_TO_NAME;
	static const NSHARE::CText SERVER_SIZE;
	static const NSHARE::CText SERVER_NAME;
	static const NSHARE::CText SM_REMOVE;
	static const NSHARE::CText NAME;
	static const NSHARE::CText THREAD_PRIORITY;
	static const NSHARE::CText USING_AS_DEF;
	static const NSHARE::CText RESERV;

	CSmMainChannel();
	virtual ~CSmMainChannel();
	bool MStart();
	bool MOpen(ILink* aHandler,program_id_t const&,NSHARE::net_address const& aFor);
	bool MIsOpen(descriptor_t aFor) const;
	bool MClose(descriptor_t aFor);

	bool MSendToService(const data_t& aVal, descriptor_t);
	bool MSend(const data_t& aVal, descriptor_t);

	bool MSend(user_data_t const& aVal, descriptor_t);
	virtual NSHARE::ISocket* MGetSocket();
	const virtual NSHARE::ISocket* MGetSocket() const;

	bool MHandleServiceDG(const main_channel_param_t* aData, descriptor_t);
	bool MHandleServiceDG(main_channel_error_param_t const* aP,
			descriptor_t);
	bool MHandleServiceDG(request_main_channel_param_t const* aP,
			descriptor_t);
	bool MHandleServiceDG(close_main_channel_t const* aP, descriptor_t);
	bool MIsOveload(descriptor_t aFor) const;
	NSHARE::CBuffer MGetNewBuf(std::size_t aSize,NSHARE::eAllocatorType) const;

	NSHARE::CConfig MSerialize() const;
	NSHARE::IAllocater* MAllocater() const;
private:
	typedef CInParser<CSmMainChannel, ILink*> parser_t;
	struct sm_io_t:NSHARE::IIntrusived
	{
		sm_io_t():FFrom(0),FTo(1)
		{
			FSendError=0;
			FSendBytes=0;
			FRecvBytes=0;
			FOverloades=0;
		}
		NSHARE::CConfig MSerialize() const;

		uint32_t FFrom;
		uint32_t FTo;
		unsigned FSendError;
		unsigned FSendBytes;
		unsigned FRecvBytes;
		unsigned FOverloades;

		//mutable NSHARE::CMutex FSendMutex;
	};

	struct param_t
	{
		enum eState
		{
			E_TRY_OPEN, E_REQUEST_PARAM, E_CONNECTED, E_CLOSING
		} ;

		param_t();

		ILink* FHandler;
		NSHARE::shared_identify_t FAddr;
		NSHARE::intrusive_ptr<sm_io_t> FIo;
		eState FState;
	};
	typedef std::map<NSHARE::shared_identify_t,param_t > ip_to_id_t;
	typedef std::map<descriptor_t, param_t > id_to_ip_t;
	typedef std::map<NSHARE::CBuffer::offset_pointer_t, user_data_info_t>  recv_info_t;

	struct _data_t
	{
		id_to_ip_t FIdToIP;
		ip_to_id_t FIpToId;
		id_to_ip_t FNew;
	};
	typedef NSHARE::CSafeData<_data_t> safe_data_t;
	typedef safe_data_t::RAccess<> const r_access;
	typedef safe_data_t::WAccess<> w_access;

	static NSHARE::eCBRval sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	void MThreadSafetyAssign(NSHARE::intrusive_ptr<sm_io_t> &aFrom,NSHARE::intrusive_ptr<sm_io_t> const& aTo) const;
	void MThreadSafetyReset(NSHARE::intrusive_ptr<sm_io_t> &aFrom)const;
	void MReceiver();
	bool MSendSetting(ILink* aHandler, descriptor_t aFor);
	bool MIsNew(const descriptor_t& aFor) const;
	bool MAddNewImpl(descriptor_t aFor, NSHARE::shared_identify_t const&);
	bool MSendImpl(NSHARE::intrusive_ptr<sm_io_t>& _io,
			const user_data_t& aVal, const NSHARE::shared_identify_t & _addr);
	int MSendMainChannelError(descriptor_t aFor,
			unsigned aError);
	NSHARE::CBuffer  MGetValidBuffer(NSHARE::CBuffer const& aVal);
	size_t MReceiveImpl(unsigned aType, NSHARE::CBuffer& _data, NSHARE::shared_identify_t const& _from,ILink*);
	void MCheckPacketSequence(const unsigned aPacket, unsigned & aLast);
	bool MOpenIfNeed();
	template<class DG_T> void MFill(data_t*);
	template<class T>
	inline void MFill(data_t* _buf, const T& _id);


	NSHARE::CSharedMemoryServer FSmServer;
	safe_data_t FData;
	std::map<NSHARE::CBuffer::offset_pointer_t, user_data_info_t> FRecv;
	mutable NSHARE::CMutex FMutex;
	size_t FReserv;
	size_t FSize;
	bool FIsUsingAsDef;
	NSHARE::CText FName;
	NSHARE::CMutex FOpenMutex;

};
} /* namespace NUDT */
#endif /* CSMMAINCHANNEL_H_ */
