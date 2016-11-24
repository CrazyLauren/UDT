/*
 * CMainUdpRegister.h
 *
 *  Created on: 25.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CMAINUDPREGISTER_H_
#define CMAINUDPREGISTER_H_

namespace NUDT
{
class UDP_MAIN_CHANNEL_EXPORT CMainUdpRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CMainUdpRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
}
#endif /* CMAINUDPREGISTER_H_ */
