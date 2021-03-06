/*
 * CKernelIOByTCPClientClient.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CKERNELIOBYTCPCLIENT_H_
#define CKERNELIOBYTCPCLIENT_H_

#include "ITcpIOManager.h"
#include <io/ILink.h>

namespace NUDT
{
using namespace NSHARE;
/** @brief Client of TCP communication
 *
 */
class CKernelIOByTCPClient:public ITcpIOManager
{
public:
	static const NSHARE::CText NAME;

	CKernelIOByTCPClient();
	virtual ~CKernelIOByTCPClient();

	void MInit(CKernelIo *);

	/** Adds the new client
	 *
	 * @param aWhat The client you want to add
	 * @return true if added
	 */
	bool MAddClient(network_channel_t const& aWhat);

	/** Removes client
	 *
	 * @param aWhat A removed client
	 * @return true if removed
	 */
	bool MRemoveClient(network_channel_t const& aWhat);

	bool MOpen(const void* = NULL);
	bool MIsOpen() const;
	void MClose();
	void MClose(const descriptor_t& aFrom);
	void MCloseRequest(const descriptor_t& aFrom);
	descriptors_t MGetDescriptors() const;
	bool MIs(descriptor_t)const;

	bool MSend(const data_t& aVal, descriptor_t const&);
	bool MSend(const program_id_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const kernel_infos_array_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const fail_send_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const user_data_t& aVal, descriptor_t const& aTo);
	bool MSend(const demand_dgs_for_t& aVal, descriptor_t const& aFrom, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());

	void MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd, const descriptor_t& aFrom);

	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig const& MBufSettingFor(const descriptor_t& aFor,
			NSHARE::CConfig const& aFrom) const;
private:
	struct CKernelChannel;
	typedef NSHARE::intrusive_ptr<CKernelChannel> smart_channel_t;

	typedef std::vector<smart_channel_t> channels_t; ///< list of Channels
	typedef NSHARE::CSafeData<channels_t> thread_safety_channels_t; ///<  RW lock for type channels_t

	typedef std::map<descriptor_t, smart_channel_t> active_channels_t; ///< list of connected channels
	typedef NSHARE::CSafeData<active_channels_t> th_safety_active_channels_t; ///<  RW lock for type active_channels_t



	template<class T> inline bool MSendImpl(const T& _id,
			descriptor_t const& aTo, const routing_t& aRoute,error_info_t const& aError);
	inline bool MSendImpl(const user_data_t& _id,
			descriptor_t const& aTo);

	bool MAddChannel(CKernelChannel*,descriptor_t aId,split_info const& aLimits);
	bool MRemoveChannel(CKernelChannel*);

	CKernelIo * FIo;
	thread_safety_channels_t FChannels;
	th_safety_active_channels_t FActiveChannels;
};
class CKernelIOByTCPClientRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CKernelIOByTCPClientRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
}

#endif /* CKERNELIOBYTCPCLIENT_H_ */
