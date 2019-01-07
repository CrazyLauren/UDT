/*
 * CMainSmRegister.h
 *
 *  Created on: 01.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CMAINSMREGISTER_H_
#define CMAINSMREGISTER_H_

namespace NUDT
{
class SM_MAIN_CHANNEL_EXPORT CMainSmRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CMainSmRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
}
#endif /* CMAINSMREGISTER_H_ */
