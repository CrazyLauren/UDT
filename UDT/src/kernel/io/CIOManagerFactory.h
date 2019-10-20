/*
 * CIOManagerFactory.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 07.07.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IO_CIOMANAGERFACTORY_H_
#define IO_CIOMANAGERFACTORY_H_

#include <io/IIOManager.h>

namespace NUDT
{
class CIOManagerFactory: public NSHARE::CFactoryManager<IIOManager,NSHARE::CMutex>
{
public:
	CIOManagerFactory();
	virtual ~CIOManagerFactory();
private:
};
}
#endif /* IO_CIOMANAGERFACTORY_H_ */
