/*
 *
 * CTraceToSocket.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.8.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTRACETOSOCKET_H_
#define CTRACETOSOCKET_H_
#include <log4cplus/config.hxx>
#include <log4cplus/appender.h>
#include <log4cplus/helpers/socket.h>
namespace NSHARE
{
class ISocket;
class SHARE_EXPORT CTraceToSocket: public log4cplus::Appender, NSHARE::CDenyCopying
{
public:
	CTraceToSocket(NSHARE::CConfig const&);
	~CTraceToSocket();
	virtual void close();
protected:
	void MOpen(NSHARE::CConfig const&);
	virtual void append(const log4cplus::spi::InternalLoggingEvent& event);
private:
	SHARED_PTR<ISocket> FSocket;
};

}
/* namespace NSHARE */
#endif /* CTRACETOSOCKET_H_ */
