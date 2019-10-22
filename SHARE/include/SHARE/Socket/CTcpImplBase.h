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
	/** Create the new TCP socket
	 *
	 * @return TCP socket
	 */
	static CSocket MNewSocket();

	static const unsigned REPEAT_SEND_COUNT; ///< The number of attempt to send data
	static const unsigned NSHARE_TCP_ERROR_TIMEOUT; ///< A timeout before the new attempt to send

	/** @brief Send data to specified socket
	 *
	 * @param aTo Send to the socket
	 * @param pData A pointer to the data
	 * @param nSize The data size
	 * @param aDiag A reference to IO diagnostic object
	 * @param aIsBlockMode true if block until the data will send
	 * @return Send state
	 */
	ISocket::sent_state_t MSendTo(CSocket const & aTo, const void* pData, size_t nSize,
			diagnostic_io_t & aDiag,
			bool aIsBlockMode) const;

	/** Read data from specified socket
	 *
	 * @param aBuf A pointer to the buffer
	 * @param aSock Receive from the socket
	 * @param aDiag A reference to IO diagnostic object
	 * @return
	 */
	int MReadData(ISocket::data_t* aBuf, CSocket const& aSock,diagnostic_io_t & aDiag) const;

private:
	void MWaitForSend(CSocket const & aTo) const;
};

} /* namespace NSHARE */
#endif /* CTCPIMPLBASE_H_ */
