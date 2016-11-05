/*
 * SerialFunction.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 23.03.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef SERIALFUNCTION_H_
#define SERIALFUNCTION_H_

#include <deftype>
namespace NSHARE
{
namespace NSerial
{
struct port_info_t
{

	/*! Address of the serial port (this can be used in MOpen function of CSerial). */
	std::string FPort;

	/*! Description of  device. */
	std::string FDescription;

	/*! Hardware ID (e.g. VID:PID of USB serial devices) or "n/a" if not available. */
	std::string FHardwareId;

};
typedef std::vector<port_info_t> ports_t;
extern SHARE_EXPORT void get_all_ports(ports_t *aTo);
} /* namespace NSerial */
}//namespace USHARE

#endif /* SERIALFUNCTION_H_ */
