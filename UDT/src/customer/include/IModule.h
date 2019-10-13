/*
 * IModule.h
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IMODULE_H_
#define IMODULE_H_
#include "customer_export.h"
#include "ICustomer.h"

namespace NUDT
{
/** Base interface for all modules
 *
 */
class CUSTOMER_EXPORT IModule:public NSHARE::IFactory
{
public:
	typedef ICustomer::data_t data_t;

	/** Initialize module
	 *
	 */
	virtual void MInit(ICustomer *)=0;

	/** Open module (can start thread)
	 *
	 */
	virtual bool MOpen(const NSHARE::CThread::param_t* = NULL)=0;

	/** Wait for closed
	 *
	 */
	virtual void MJoin()=0;

	/** Returns true if is opened
	 *
	 * @return true if opened
	 */
	virtual bool MIsOpened() const =0;

	/** Close module
	 *
	 */
	virtual void MClose()=0;

protected:
	IModule(const NSHARE::CText& type);
	~IModule();

};
}



#endif /* IMODULE_H_ */
