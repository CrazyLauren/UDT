/*
 * ILink.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 08.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ILINK_H_
#define ILINK_H_

#include <core/IState.h>
namespace NUDT
{
class ILink: public NSHARE::IIntrusived, public NSHARE::IFactory,public IState
{
public:
	typedef NSHARE::CBuffer data_t;

	ILink(const NSHARE::CText& type,uint64_t aTime) :
			NSHARE::IFactory(type), IState(type), FTime(aTime)
	{

	}
	ILink(const ILink& aRht) :
			NSHARE::IFactory(aRht.FType), IState(aRht.FType), FTime(aRht.FTime)
	{

	}
	virtual ~ILink()
	{

	}
	virtual bool MSend(const data_t& aVal )=0;
	virtual bool MReceivedData(NSHARE::CBuffer::const_iterator aBegin,
			NSHARE::CBuffer::const_iterator aEnd)
	{
		return false;
	}

	virtual bool MSend(const program_id_t& aVal, const routing_t& aRoute,error_info_t const&)=0;
	virtual bool MReceivedData(program_id_t const&, const routing_t& aFrom,error_info_t const&)
	{
		return false;
	}

	virtual bool MSend(const user_data_t& aVal)=0;
	virtual bool MReceivedData(user_data_t& )
	{
		return false;
	}


	virtual bool MSend(const kernel_infos_array_t& aVal, const routing_t& aRoute,error_info_t const&)=0;
	virtual bool MReceivedData(kernel_infos_array_t const&, const routing_t& aFrom,error_info_t const&)
	{
		return false;
	}
	virtual bool MReceivedData(demand_dgs_t const&, const routing_t& aRoute,error_info_t const&)
	{
		return false;
	}
	virtual bool MSend(const fail_send_t& aVal, const routing_t& aRoute,error_info_t const&)=0;
	virtual bool MReceivedData(fail_send_t const&, const routing_t& aFrom,error_info_t const&)
	{
		return false;
	}
	virtual bool MSend(const demand_dgs_for_t& aVal, const routing_t& aRoute,error_info_t const&)=0;
	virtual bool MReceivedData(demand_dgs_for_t const&, const routing_t& aFrom,error_info_t const&)
	{
		return false;
	}
	virtual NSHARE::CConfig const& MBufSettingFor(NSHARE::CConfig const& aFrom) const=0;

	virtual void MClose()=0;
	virtual void MCloseRequest()=0;
	virtual descriptor_t MGetID() const=0;
	virtual bool MIsOpened() const=0;
	virtual bool MAccept()=0;
	split_info MLimits() const
	{
		return FSpitInfo;
	}
	void MSetLimits(split_info const& aLim)
	{
		VLOG(2)<<"Set new limits "<<aLim;
		FSpitInfo=aLim;
	}
//	virtual bool MHandleNewConnection(NSHARE::CBuffer::const_iterator aBegin,
//			NSHARE::CBuffer::const_iterator aEnd)=0; //false - handling link connection is finished
	uint64_t const FTime; //ms
private:
	void operator=(const ILink & copy)
	{
	}
	
	split_info FSpitInfo;
};
typedef NSHARE::intrusive_ptr<ILink> smart_link_t;
}

#endif /* ILINK_H_ */
