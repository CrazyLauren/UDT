/*
 * ILocalChannel.h
 *
 *  Created on: 25.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ILOCALCHANNEL_H_
#define ILOCALCHANNEL_H_

#include "IIOConsumer.h"
struct main_channel_param_t;
struct main_channel_error_param_t;
struct request_main_channel_param_t;
struct close_main_channel_t;
namespace NUDT
{
class ILocalChannel:public NSHARE::IFactory
{
public:
	typedef IIOConsumer::data_t data_t;

	virtual bool MOpen(IIOConsumer*)=0;
	virtual bool MIsOpened() const =0;
	virtual bool MIsConnected() const =0;
	virtual void MClose()=0;
	virtual bool MSend(user_data_t & aVal)=0;
	virtual void MHandleServiceDG(main_channel_param_t const* aP)=0;
	virtual void MHandleServiceDG(main_channel_error_param_t const* aP)=0;
	virtual void MHandleServiceDG(request_main_channel_param_t const* aP)=0;
	virtual void MHandleServiceDG(close_main_channel_t const* aP)=0;

	virtual data_t MGetNewBuf(unsigned aSize) const=0;
protected:
	ILocalChannel(const NSHARE::CText& type) :
		NSHARE::IFactory(type)
	{
	}
};
}//
#endif /* ILOCALCHANNEL_H_ */
