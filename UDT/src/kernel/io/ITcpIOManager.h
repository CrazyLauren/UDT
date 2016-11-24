/*
 * ITcpIOManager.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IKERNELIO_H_
#define IKERNELIO_H_

#include "IIOManager.h"
namespace NUDT
{
class ITcpIOManager:public IIOManager
{
public:
	typedef NSHARE::ISocket::data_t data_t;
	typedef std::vector<descriptor_t> descriptors_t;

	virtual void MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd, const descriptor_t& aFrom)=0;
protected:
	ITcpIOManager(const NSHARE::CText& type) :
			IIOManager(type)
	{
	}
};
}

#endif /* IKERNELIO_H_ */
