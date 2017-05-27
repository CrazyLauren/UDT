/*
 * CHttpIOManagerRegister.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.05.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CHTTPNEWCONNCETION_H_
#define CHTTPNEWCONNCETION_H_

namespace NUDT
{
class CHttpIOManagerRegister: public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CHttpIOManagerRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const;
	bool MIsAlreadyRegistered() const;
};
}
#endif /* CHTTPNEWCONNCETION_H_ */
