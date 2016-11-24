/*
 * CNewConncetionFactory.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 18.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CNEWCONNCETIONFACTORY_H_
#define CNEWCONNCETIONFACTORY_H_

#include "INewConnection.h"
namespace NUDT
{

class CConnectionHandlerFactory: public NSHARE::CFactoryManager<IConnectionHandlerFactory>
{
public:
	CConnectionHandlerFactory();
	virtual ~CConnectionHandlerFactory();
};

} /* namespace NUDT */
#endif /* CNEWCONNCETIONFACTORY_H_ */
