/*
 * CLocalLinkRegister.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 18.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CLOCALLINKREGISTER_H_
#define CLOCALLINKREGISTER_H_

namespace NUDT
{
class CIOLocalLinkRegister: public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CIOLocalLinkRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const;
	bool MIsAlreadyRegistered() const;
};
}

#endif /* CLOCALLINKREGISTER_H_ */
