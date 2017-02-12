/*
 * INewConnection.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 18.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef INEWCONNECTION_H_
#define INEWCONNECTION_H_

namespace NUDT
{
class ILink;
class ILinkBridge;

class IConnectionHandler:public NSHARE::CIntrusived
{
public:
	typedef NSHARE::CBuffer data_t;
	enum eState
	{
		E_ERROR, E_OK, E_CONTINUE, E_CLOSE//for http
	};
	virtual ~IConnectionHandler()
	{

	}
	virtual bool MConnect()=0; //true if change state of client(sent data)
	virtual bool MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd)=0; //true if change state of client(sent data)
	eState MState()
	{
		return FState;
	}
	virtual ILink* MCreateLink()=0;
protected:
	eState FState;
};
class IConnectionHandlerFactory: public NSHARE::IFactory
{
public:
	virtual ~IConnectionHandlerFactory()
	{

	}
	virtual IConnectionHandler* MCreateHandler(descriptor_t aFD, uint64_t aTime,
			ILinkBridge* aKer)=0;

protected:
	IConnectionHandlerFactory(const NSHARE::CText& type) :
			NSHARE::IFactory(type)
	{
	}
};
} //
#endif /* INEWCONNECTION_H_ */
