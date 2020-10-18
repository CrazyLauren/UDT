/*
 * CRTCFactory.h
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CRTCFACTORY_H_
#define CRTCFACTORY_H_

#include "UDT/customer_export.h"
#include "UDT/IRtcControl.h"
namespace NUDT
{
class CUSTOMER_EXPORT CRTCFactory: public NSHARE::CFactoryManager<IRtcControl>
{
public:
	CRTCFactory();
	virtual ~CRTCFactory();
};
}//
#endif /* CRTCFACTORY_H_ */
