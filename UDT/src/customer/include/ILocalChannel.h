/*
 * ILocalChannel.h
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
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
/** Provide to API for sending
 * to kernel user data
 *
 */
class CUSTOMER_EXPORT ILocalChannel:public IModule
{
public:
	typedef IIOConsumer::data_t data_t;

	virtual bool MOpen(IIOConsumer*)=0;
	virtual bool MIsConnected() const =0;
	virtual bool MSend(user_data_t & aVal)=0;
	virtual void MHandleServiceDG(main_channel_param_t const* aP)=0;
	virtual void MHandleServiceDG(main_channel_error_param_t const* aP)=0;
	virtual void MHandleServiceDG(request_main_channel_param_t const* aP)=0;
	virtual void MHandleServiceDG(close_main_channel_t const* aP)=0;

	/** Allocate a new buffer
	 *
	 * @param aSize size of buffer
	 * @return buffer
	 */
	virtual data_t MGetNewBuf(size_t aSize) const=0;
protected:
	ILocalChannel(const NSHARE::CText& type) :
		IModule(type)
	{
	}
};
}//
#endif /* ILOCALCHANNEL_H_ */
