/*
 * CParserFactory.h
 *
 *  Created on: 21.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CPARSERFACTORY_H_
#define CPARSERFACTORY_H_

#include <IExtParser.h>
namespace NSHARE
{
template class UDT_SHARE_EXPORT CFactoryManager<NUDT::IExtParser>;
}
namespace NUDT
{
class UDT_SHARE_EXPORT CParserFactory:public  NSHARE::CFactoryManager<NUDT::IExtParser>
{
public:
	CParserFactory();
};
}//
#endif /* CPARSERFACTORY_H_ */
