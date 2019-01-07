/*
 * load_static_module.h
 *
 *  Created on: 26.11.2016
 *      Author:  https://github.com/CrazyLauren
 *	
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef LOAD_STATIC_MODULE_H_
#define LOAD_STATIC_MODULE_H_

namespace NUDT
{
extern "C" NSHARE::factory_registry_t* static_factory_registry(NSHARE::CFactoryRegisterer* aVal);

template<class TStaticRegister>
class CStaticRegister
{
public:
	CStaticRegister()
	{
		NUDT::static_factory_registry(new TStaticRegister);
	}
};
}
#endif /* LOAD_STATIC_MODULE_H_ */
