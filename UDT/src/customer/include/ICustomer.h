/*
 * ICustomer.h
 *
 *  Created on: 25.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ICUSTOMER_H_
#define ICUSTOMER_H_

#include <programm_id.h>
namespace NUDT
{
class ICustomer
{
public:
	typedef NSHARE::CBuffer data_t;
	struct requred_recv_t
	{
		NSHARE::CText FFrom;
		NSHARE::CText FProtocolType;
		required_header_t FHeader;
	};

	virtual ~ICustomer(){
		;
	}
	virtual void MEventConnected() =0;
	virtual void MEventDisconnected() =0;
};
}//
#endif /* ICUSTOMER_H_ */
