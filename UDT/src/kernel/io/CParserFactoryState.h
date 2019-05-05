/*
 * CParserFactoryState.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 12.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CPARSERFACTORYSTATE_H_
#define CPARSERFACTORYSTATE_H_

#include <core/IState.h>

/*!\brief wrapper for  NUDT::CParserFactory class
 * for realization IState interface (It cannot
 * do it by oneself as its in the library)
 *
 */
namespace NUDT
{
class CParserFactoryState: public IState
{
public:
	CParserFactoryState() :
			IState(CParserFactory::NAME)
	{
	}
	NSHARE::CConfig MSerialize() const
	{
		CHECK_NOTNULL(CParserFactory::sMGetInstancePtr());
		return static_cast<CParserFactory*>(CParserFactory::sMGetInstancePtr())->MSerialize();
	}
};
}
#endif /* CPARSERFACTORYSTATE_H_ */

