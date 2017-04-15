/*
 * CSelectSocket.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 05.11.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSELECTSOCKET_H_
#define CSELECTSOCKET_H_

namespace NSHARE
{

class SHARE_EXPORT CSelectSocket: CDenyCopying
{
public:
	typedef std::vector<CSocket> socks_t;
	CSelectSocket();
	virtual ~CSelectSocket();
	void MAddSocket(CSocket const&);
	void MRemoveSocket(CSocket const&);
	void MRemoveAll();
	int MWaitData(socks_t& aTo, float const aTime, unsigned aType =
			E_READ_ONLY) const;
	bool MIsSetUp() const
	{
		return FIsSetUp;
	}
	const std::vector<CSocket>& MGetSockets()const
	{
		return FFds;
	}
private:
	struct CImpl;

	CImpl *FImpl;
	std::vector<CSocket> FFds;
	bool FIsSetUp;
	NSHARE::CMutex FMutex;
};

} /* namespace NSHARE */
#endif /* CSELECTSOCKET_H_ */
