/*
 * IIOConsumer.h
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

#include "IModule.h"
#include <udt/programm_id.h>
#include <shared_types.h>
namespace NUDT
{
/** Input output API of customer
 *
 */
class CUSTOMER_EXPORT IIOConsumer:public IModule
{
public:

	/** Returns true if connected
	 *
	 * @return true if connected
	 */
	virtual bool MIsConnected() const =0;

	/** Send data to kernel
	 *
	 * @param aData sent data
	 * @return <0 if error otherwise amount of bytes sent
	 */
	virtual int MSend(data_t & aData)=0;

	/** @copydoc ICustomer::MGetNewBuf()
	 *
	 */
	virtual NSHARE::CBuffer MGetNewBuf(std::size_t aSize) const=0;
protected:
	IIOConsumer(const NSHARE::CText& type) :
		IModule(type)
	{
	}
};
}//

#endif /* IIOCONSUMER_H_ */
