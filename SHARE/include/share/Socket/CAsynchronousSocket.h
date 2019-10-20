/*
 * CAsynchronousSocket.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 14.03.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CASYNCHRONOUSSOCKET_H_
#define CASYNCHRONOUSSOCKET_H_

class CEvent;
namespace NSHARE
{
class SHARE_EXPORT CAsyncSocket
{
public:
	typedef ISocket::data_t cb_arg_t;
	typedef CThread::param_t param_t;
	CAsyncSocket(ISocket*&);
	CAsyncSocket();//you have to overload MGetSocket() methods;
	virtual ~CAsyncSocket();
	bool MStart(const CThread::param_t* = NULL);
	void MStop();
	bool MJoin();
	virtual ISocket* MGetSocket();
	virtual const ISocket* MGetSocket() const;

protected:
	virtual void MReceivedData(const ISocket::data_t&) = 0;
	virtual void MReceivedData(const ISocket::data_t& aData,
			const ISocket::recvs_from_t& aFrom);
private:
	static eCBRval sMReceiver(void*, void*, void*);
	void MReceiver();
	bool MCancel();
	void MInit();

	CThread FThreadReceiver;
	ISocket*& FSock;
	bool FIsWorking;
};

} //namespace USHARE

#endif /* CASYNCHRONOUSSOCKET_H_ */
