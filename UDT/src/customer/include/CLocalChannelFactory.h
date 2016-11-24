/*
 * CLocalChannelFactory.h
 *
 *  Created on: 25.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CLOCALCHANNELFACTORY_H_
#define CLOCALCHANNELFACTORY_H_

#include "customer_export.h"
#include "ILocalChannel.h"
namespace NUDT
{
class CUSTOMER_EXPORT CLocalChannelFactory: public NSHARE::CFactoryManager<ILocalChannel>
{
public:
	CLocalChannelFactory();
	virtual ~CLocalChannelFactory();
};
}//
#endif /* CLOCALCHANNELFACTORY_H_ */
