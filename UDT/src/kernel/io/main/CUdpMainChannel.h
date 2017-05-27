/*
 * CUdpMainChannel.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CUDPMAINCHANNEL_H_
#define CUDPMAINCHANNEL_H_

#include "IMainChannel.h"
namespace NUDT
{
class CUdpMainChannel: public IMainChannel
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText THREAD_PRIORITY;

	CUdpMainChannel();
	virtual ~CUdpMainChannel();
	void MInit();
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

	NSHARE::CConfig MSerialize() const;
private:
	typedef CInParser<CUdpMainChannel, ILink*> parser_t;
	typedef COverload<NSHARE::net_address> net_overload_t;
	struct param_t
	{
		enum eState
		{
			E_TRY_OPEN,
			E_REQUEST_PARAM,
			E_CONNECTED,
			E_CLOSING
		}FState;
		parser_t FParser; //the user data store pointer to ILink
		NSHARE::net_address FAddr;
		param_t():FState(E_TRY_OPEN)
		{

		}
	};
	typedef SHARED_PTR<param_t> smart_param_t;
	typedef std::map<NSHARE::net_address,smart_param_t ,
			NSHARE::net_address::CFastLessCompare> ip_to_id_t;
	typedef std::map<descriptor_t, SHARED_PTR<param_t> > id_to_ip_t;

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
	void MReceiver();
	bool MSendSetting(ILink* aHandler, descriptor_t aFor);
	bool MIsNew(const descriptor_t& aFor) const;
	bool MChangeSettingChannel(descriptor_t aFor, NSHARE::net_address);
	bool MAddNewImpl(descriptor_t aFor, NSHARE::net_address const&);
	int MSendMainChannelError(descriptor_t aFor,
			unsigned aError);

	template<class DG_T> void MFill(data_t*);
	template<class T>
	inline void MFill(data_t* _buf, const T& _id);

	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);

	NSHARE::CUDP FUdp;

	safe_data_t FData;

	net_overload_t FOverload; //fixme to base class
	bool FIsOverload;

	friend class CInParser<CUdpMainChannel, ILink*>;
	friend std::ostream& operator<<(std::ostream & aStream,  NUDT::CUdpMainChannel::param_t::eState const& aVal);
};
inline std::ostream& operator<<(std::ostream & aStream, NUDT::CUdpMainChannel::param_t::eState const& aVal)
{
	return aStream << static_cast<unsigned>(aVal);
}
}
#endif /* CUDPMAINCHANNEL_H_ */
