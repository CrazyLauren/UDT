/*
 * ICore.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ICORE_H_
#define ICORE_H_

#include <core/IState.h>
namespace NUDT
{
/*!\brief It's interface of basic class
 *
 */
class ICore: public IState
{
public:
	/*! \brief The destructor autoremoves
	 * pointer from #NUDT::CCore object
	 *
	 */
	virtual ~ICore();

	/*!\brief Start working (has to be non-blocking)
	 *
	 *\return true if started successfully
	 */
	virtual bool MStart()=0;

	/*!\brief Stop working (has to be non-blocking)
	 *
	 *\return true if started successfully
	 */
	virtual void MStop()=0;

protected:

	/*! \brief The constructor is passed
	 * pointer to NUDT::CCore::MAddState method
	 *
	 *\param aName -A non-unique name
	 */
	ICore(NSHARE::CText const& aName);
};
} /* namespace NUDT */
#endif /* ICORE_H_ */
