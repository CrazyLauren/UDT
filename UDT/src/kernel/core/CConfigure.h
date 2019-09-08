/*
 * CConfigure.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 20.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CCONFIGURE_H_
#define CCONFIGURE_H_

#include "ICore.h"
namespace NUDT
{
class CConfigure:public NSHARE::CSingleton<CConfigure>,public ICore
{
public:
	static const NSHARE::CText NAME;
	enum  eType
	{
		JSON,
		XML
	};
	CConfigure();
	CConfigure(NSHARE::CText const&,eType const& =JSON);
	bool MStart();
	void MStop();

	NSHARE::CConfig & MGet();
	NSHARE::CText const&  MGetPath() const;
	NSHARE::CConfig MSerialize() const;
private:
	NSHARE::CConfig FConf;
	NSHARE::CText FPath;
};
}//
#endif /* CCONFIGURE_H_ */
