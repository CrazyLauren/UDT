/*
 * CSeleclSocketPosix.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CSELECLSOCKETPOSIX_H_
#define CSELECLSOCKETPOSIX_H_
namespace NSHARE
{
struct CSelectSocket::CImpl
{
	CImpl(CSelectSocket& aThis);

	int MWaitData(socks_t& aTo, float const aTime, unsigned aType) const;

	mutable fd_set readset;
	mutable fd_set writeset;
	CSelectSocket& FThis;
};
}

#endif /* CSELECLSOCKETPOSIX_H_ */
