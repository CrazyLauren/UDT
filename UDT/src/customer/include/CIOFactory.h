/*
 * CIOFactory.h
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CIOFACTORY_H_
#define CIOFACTORY_H_

#include "customer_export.h"
#include "IIOConsumer.h"
namespace NUDT
{
class CUSTOMER_EXPORT CIOFactory: public NSHARE::CFactoryManager<IIOConsumer>
{
public:
	CIOFactory();
	virtual ~CIOFactory();
};
}
#endif /* CIOFACTORY_H_ */
