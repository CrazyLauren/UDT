/*
 * CKernelLinkRegister.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 29.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CKERNELLINKREGISTER_H_
#define CKERNELLINKREGISTER_H_


namespace NUDT
{
class CIOKernelLinkRegister: public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CIOKernelLinkRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const;
	bool MIsAlreadyRegistered() const;
};
}


#endif /* CKERNELLINKREGISTER_H_ */
