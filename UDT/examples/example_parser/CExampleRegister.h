/*
 * CExampleRegister.h
 *
 *  Created on: 22.01.2016
 *      Author: Sergey
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
