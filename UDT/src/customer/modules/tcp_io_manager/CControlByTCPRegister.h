/*
 * CControlByTCPRegister.h
 *
 *  Created on: 19.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCONTROLBYTCPREGISTER_H_
#define CCONTROLBYTCPREGISTER_H_
namespace NUDT
{
class TCP_IO_MANAGER_EXPORT CControlByTCPRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CControlByTCPRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};

}
#endif /* CCONTROLBYTCPREGISTER_H_ */
