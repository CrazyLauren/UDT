/*
 * CLocalChannelFactory.h
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CLOCALCHANNELFACTORY_H_
#define CLOCALCHANNELFACTORY_H_

#include "udt/customer_export.h"
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
