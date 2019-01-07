/*
 * CExternalChannel.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CEXTERNALCHANNEL_H_
#define CEXTERNALCHANNEL_H_

namespace NUDT
{

class CExternalChannel:public IIOManager
{
public:
	static const NSHARE::CText NAME;
	CExternalChannel();
	~CExternalChannel();

	void MInit(CKernelIo *);
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
	class CFrontEnd;

	typedef NSHARE::intrusive_ptr<CFrontEnd> smart_fe_t;
	typedef std::vector<smart_fe_t> channels_t;
	typedef std::map<descriptor_t,smart_fe_t> active_channels_t;

	inline bool MSendImpl(const user_data_t& _id,
			descriptor_t const& aTo);

	bool MAddChannel(CFrontEnd*,descriptor_t aId,split_info const& aLimits);
	bool MRemoveChannel(CFrontEnd*,descriptor_t aId);

	CKernelIo * FIo;
	channels_t FChannels;
	active_channels_t FActiveChannels;

	mutable NSHARE::CMutex FMutex;
};
class CExternalChannelRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CExternalChannelRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
} /* namespace NUDT */
#endif /* CEXTERNALCHANNEL_H_ */
