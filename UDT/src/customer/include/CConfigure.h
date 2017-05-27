/*
 * CConfigure.h
 *
 *  Created on: 20.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CCONFIGURE_H_
#define CCONFIGURE_H_

#include "customer_export.h"

namespace NUDT
{
class CUSTOMER_EXPORT CConfigure:public NSHARE::CSingleton<CConfigure>
{
public:
	enum  eType
	{
		JSON,
		XML
	};
	CConfigure();
	CConfigure(NSHARE::CText const&,eType const& =JSON);

	NSHARE::CConfig & MGet();
	NSHARE::CText const&  MGetPath() const;

private:
	NSHARE::CText FPath;
	NSHARE::CConfig FConf;
};
}//
#endif /* CCONFIGURE_H_ */
