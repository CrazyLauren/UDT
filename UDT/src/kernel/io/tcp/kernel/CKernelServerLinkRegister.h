/*
 * CKernelServerLinkRegister.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 18.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CKENRELSERVERLINKREGISTER_H_
#define CKENRELSERVERLINKREGISTER_H_

namespace NUDT
{
class CKernelServerLinkRegister: public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CKernelServerLinkRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const;
	bool MIsAlreadyRegistered() const;
};
}

#endif /* CKENRELSERVERLINKREGISTER_H_ */
