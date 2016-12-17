/*
 * CHttpLink.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.05.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CHTTPLINK_H_
#define CHTTPLINK_H_


namespace NUDT
{

class CHttpLink: public ILink,NSHARE::CDenyCopying
{
public:
	CHttpLink(descriptor_t aFD, uint64_t FTime,
			ILinkBridge*);
	virtual ~CHttpLink();
	void MReceivedData(CHttpRequest const& FRequest);
	virtual bool MSend(const data_t& aVal);
	virtual bool MSend(const program_id_t& aVal, const routing_t& aRoute,error_info_t const&);
	virtual bool MSend(const user_data_t& aVal);
	virtual bool MSend(const kernel_infos_array_t& aVal, const routing_t& aRoute,error_info_t const&);
	virtual bool MSend(const demand_dgs_for_t& aVal, const routing_t& aRoute,error_info_t const&);
	virtual bool MSend(const fail_send_t& aVal, const routing_t& aRoute,error_info_t const&);

	virtual void MClose();
	void MCloseRequest();
	virtual descriptor_t MGetID() const;
	virtual bool MIsOpened() const;
	virtual bool MAccept();
	void MReceivedData(NSHARE::CBuffer::const_iterator aBegin,
			NSHARE::CBuffer::const_iterator aEnd);
	NSHARE::CConfig MSerialize() const;
	virtual NSHARE::CConfig const& MBufSettingFor(NSHARE::CConfig const& aFrom) const
	{
		return NSHARE::CConfig::sMGetEmpty();
	}
private:


	smart_bridge_t FKernel;
	descriptor_t  Fd;
	bool FIsAccept;//fix bug
	std::vector<CHttpRequest> FRequests;
	CHttpRequest FRequest;
};

} /* namespace NUDT */
#endif /* CHTTPLINK_H_ */
