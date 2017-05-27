/*
 * IProtocols.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IPROTOCOLS_H_
#define IPROTOCOLS_H_

namespace NUDT
{

class IProtocols
{
public:
	virtual ~IProtocols(){
		;
	}
	virtual bool MSend(const NSHARE::CBuffer& aVal, descriptor_t const& aTo)=0;
	inline bool MSend(const NSHARE::CBuffer& aVal, descriptor_t const& aTo,
			const routing_t&, error_info_t const&)
	{
		return MSend(aVal, aTo);
	}

	virtual bool MSend(const program_id_t& aVal, descriptor_t const& aFrom, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t())=0;
	virtual bool MSend(const user_data_t& aVal, descriptor_t const& aFrom)=0;
	virtual bool MSend(const kernel_infos_array_t& aVal, descriptor_t const& aFrom, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t())=0;
	virtual bool MSend(const fail_send_t& aVal, descriptor_t const& aFrom, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t())=0;
	virtual bool MSend(const demand_dgs_for_t& aVal, descriptor_t const& aFrom, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t())=0;
};

} /* namespace NUDT */
#endif /* IPROTOCOLS_H_ */
