/*
 * ICustomer.h
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IIOCONSUMER_H_
#define IIOCONSUMER_H_

#include "ICustomer.h"
#include <programm_id.h>
#include <shared_types.h>
namespace NUDT
{
class IIOConsumer:public NSHARE::IFactory
{
public:
	typedef ICustomer::data_t data_t;

	virtual void MInit(ICustomer *)=0;
	virtual bool MOpen(const NSHARE::CThread::param_t* = NULL)=0;
	virtual bool MIsAvailable() const=0;
	virtual bool MIsConnected() const =0;
	virtual bool MIsOpened() const =0;
	//virtual void MRecvListUpdated()=0 ;//update filters
	virtual void MClose()=0;

	virtual int MSend(user_data_t &)=0;
	virtual int MSend(data_t &)=0;

	virtual void MReceivedData(data_t const& aData,void* =NULL)=0;
	virtual void MReceivedData(user_data_t const & aData)=0;
	virtual void MReceivedData(progs_id_t const & aData)=0;
	virtual void MReceivedData(fail_send_t const & aData)=0;

	virtual NSHARE::CBuffer MGetNewBuf(std::size_t aSize) const=0;
protected:
	IIOConsumer(const NSHARE::CText& type) :
		NSHARE::IFactory(type)
	{
	}
};
}//

#endif /* IIOCONSUMER_H_ */
