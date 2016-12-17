/*
 * IMainChannel.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 13.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IMAINCHANNEL_H_
#define IMAINCHANNEL_H_

#include "../../core/IState.h"
namespace NUDT
{
class ILink;

class IMainChannel: public NSHARE::IFactory,public IState
{
public:
	typedef NSHARE::CBuffer data_t;
	static const NSHARE::CText CONFIGURE_NAME;
	virtual ~IMainChannel()
	{

	}
	virtual void MInit()
	{

	}
	;
	virtual bool MOpen(ILink* aHandler, program_id_t const&,
			NSHARE::net_address const& aFor)=0;
	virtual bool MIsOpen(descriptor_t aFor) const =0;
	virtual bool MClose(descriptor_t aFor)=0;

	virtual bool MSend(user_data_t const& aVal, descriptor_t)=0;
	//virtual bool MSend(data_t const& aVal,descriptor_t)=0;
	virtual bool MHandleServiceDG(main_channel_param_t const* aP,
			descriptor_t)=0;
	virtual bool MHandleServiceDG(main_channel_error_param_t const* aP,
			descriptor_t)=0;
	virtual bool MHandleServiceDG(request_main_channel_param_t const* aP,
			descriptor_t)=0;
	virtual bool MHandleServiceDG(close_main_channel_t const* aP, descriptor_t)=0;
	virtual bool MIsOveload(descriptor_t aFor) const=0;
	//todo receive
protected:
	IMainChannel(const NSHARE::CText& type) :
			NSHARE::IFactory(type),IState(type)
	{
	}
};

}
#endif /* IMAINCHANNEL_H_ */
