/*
 * ILinkBridge.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 28.07.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ILINKBRIDGE_H_
#define ILINKBRIDGE_H_

namespace NUDT
{

class ILinkBridge:public NSHARE::CIntrusived
{
public:
	typedef NSHARE::ISocket::data_t data_t;
	virtual ~ILinkBridge()
	{
		;
	}
	virtual bool MSend(const data_t& aVal)=0;
	virtual bool MCloseRequest(descriptor_t )=0;
	virtual bool MInfo(NSHARE::CConfig & aTo)=0;
	virtual bool MConfig(NSHARE::CConfig & aTo)=0;
	virtual NSHARE::net_address MGetAddr()const=0;
};
typedef NSHARE::intrusive_ptr<ILinkBridge> smart_bridge_t;
} /* namespace NUDT */
#endif /* ILINKBRIDGE_H_ */
