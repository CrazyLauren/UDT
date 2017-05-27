/*
 * CExampleRegister.h
 *
 *  Created on:  22.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CEXAMPLEREGISTER_H_
#define CEXAMPLEREGISTER_H_

class EXAMPLE_PARSER_EXPORT CExampleRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CExampleRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
#endif /* CEXAMPLEREGISTER_H_ */
