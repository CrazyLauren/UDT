/*
 * CTCP.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 04.03.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CTCP_H_
#define CTCP_H_

#ifndef TCP_SOCKET_EXIST
#	define TCP_SOCKET_EXIST
#endif 
namespace NSHARE
{
class SHARE_EXPORT CTCP: public ISocket, NSHARE::CDenyCopying, public NSHARE::CEvents<
		NSHARE::CText>
{
public:
	static const NSHARE::CText NAME;
	typedef NSHARE::CEvents<NSHARE::CText> events_t;

	//The argument of event is  "net_address"
	static events_t::key_t const EVENT_CONNECTED;
	static events_t::key_t const EVENT_DISCONNECTED;

	CTCP(net_address const& aParam = net_address());
	CTCP(NSHARE::CConfig const& aConf);
	virtual ~CTCP();
	virtual bool MOpen(net_address const& aParam, int aFlags = 0);
	bool MOpen();
	bool MConnect();

	virtual void MClose();
	bool MGetInitParam(net_address*) const;//deprecated. using MGetSetting
	net_address MGetSetting() const;

	virtual NSHARE::CConfig MSettings(void) const
	{
		net_address _addr;
		MGetInitParam(&_addr);
		return _addr.MSerialize();
	}
	sent_state_t MSend(void const* pData, size_t nSize);
	sent_state_t MSend(void const*  aData, std::size_t aSize,
			NSHARE::CConfig  const& aTo)
	{
		return MSend(aData, aSize);
	}
	sent_state_t MSend(data_t const& aVal)
	{
		return ISocket::MSend(aVal);
	}
	sent_state_t MSend(data_t const& aVal, NSHARE::CConfig  const& aTo)
	{
		return MSend(aVal.ptr_const(),aVal.size(),aTo);
	}
	bool MIsOpen() const;
	bool MIsConnected() const;
	bool MReOpen();
	const CSocket& MGetSocket(void) const;

	ssize_t MReceiveData(data_t *, float const aTime);
	std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
	size_t MAvailable() const;
	std::pair<size_t,size_t> MBufSize()const;//first send
private:
	struct CClientImpl;
	//Method
	//Field
	volatile bool FIsWorking;
	volatile bool FIsConnected;
	CClientImpl* FClientImpl;

};
} //namespace USHARE

#endif /* CTCP_H_ */
