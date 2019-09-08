/*
 * CMainChannelFactory.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CMAINCHANNELFACTORY_H_
#define CMAINCHANNELFACTORY_H_

#include <core/ICore.h>
#include "IMainChannel.h"
namespace NUDT
{
class CMainChannelFactory: public NSHARE::CFactoryManager<IMainChannel>,public ICore
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText NAME_MAIN;
	static const NSHARE::CText MAIN_CHANNEL;
	CMainChannelFactory();
	virtual ~CMainChannelFactory();
	NSHARE::CConfig MSerialize() const;
	bool MStart();
	void MStop();
};
}
#endif /* CMAINCHANNELFACTORY_H_ */
