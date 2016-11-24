/*
 * CMainChannelFactory.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 13.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CMAINCHANNELFACTORY_H_
#define CMAINCHANNELFACTORY_H_

#include "IMainChannel.h"
namespace NUDT
{
class CMainChannelFactory: public NSHARE::CFactoryManager<IMainChannel>,public IState
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText NAME_MAIN;
	static const NSHARE::CText MAIN_CHANNEL;
	CMainChannelFactory();
	virtual ~CMainChannelFactory();
	NSHARE::CConfig MSerialize() const;
};
}
#endif /* CMAINCHANNELFACTORY_H_ */
