/*
 * CTcpImplBase.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.09.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CTCPIMPLBASE_H_
#define CTCPIMPLBASE_H_

namespace NSHARE
{

struct CTcpImplBase:public CNetBase
{
	static CSocket MNewSocket();
	ISocket::sent_state_t MSendTo(CSocket const & aTo, const void* pData, size_t nSize,
			diagnostic_io_t const & aDiag, bool aIsBlockMode,unsigned& aNumberOfRepeats) const;
	int MReadData(ISocket::data_t* aBuf, CSocket const& aSock) const;
private:
	void MWaitForSend(CSocket const & aTo) const;
};

} /* namespace NSHARE */
#endif /* CTCPIMPLBASE_H_ */
