/*
 * CMainTcpRegister.h
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CMAINTCPREGISTER_H_
#define CMAINTCPREGISTER_H_

namespace NUDT
{
class TCP_MAIN_CHANNEL_EXPORT CMainTcpRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CMainTcpRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
}
#endif /* CMAINTCPREGISTER_H_ */
