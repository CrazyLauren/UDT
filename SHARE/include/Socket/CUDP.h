/*
 * CUDP.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CUDP_H_
#define CUDP_H_

#ifndef UDP_SOCKET_EXIST
#	define UDP_SOCKET_EXIST
#endif 

namespace NSHARE
{
class SHARE_EXPORT CUDP: public ISocket, protected CNetBase, NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;

	struct SHARE_EXPORT param_t
	{
		static const CText UDP_PORT;
		static const CText ADDR_TO;

		param_t(NSHARE::CConfig const& aConf);
		param_t();
		param_t(in_port_t aPort, net_address const& aParam=net_address());

		bool MIsValid() const;

		in_port_t FPort;
		smart_field_t<net_address>  FAddr;
		CConfig MSerialize() const;
	};
	CUDP(NSHARE::CConfig const& aConf);
	CUDP(const param_t& = param_t());
	virtual ~CUDP();

	bool MOpen(const param_t&, int aFlags = 0);
	bool MOpen();
	bool MReOpen();
	bool MIsOpen() const;

	void MClose();
	const CSocket& MGetSocket(void) const;
	sent_state_t MSend(void const* pData, size_t nSize);
	sent_state_t MSend(void const* pData, size_t nSize,NSHARE::CConfig  const& aTo);
	sent_state_t MSend(void const* pData, size_t nSize, net_address const&);
	sent_state_t MSend(void const* pData, size_t nSize,	struct sockaddr_in const& aAddress);
	sent_state_t MSend(data_t const& aVal)
	{
		return ISocket::MSend(aVal);
	}
	sent_state_t MSend(data_t const& aVal, NSHARE::CConfig  const& aTo)
	{
		return MSend(aVal.ptr_const(),aVal.size(),aTo);
	}
	void MSetSendAddress(net_address const& aAddress) ;
	ssize_t MReceiveData(net_address* aFrom,data_t *, float const aTime);
	ssize_t MReceiveData(data_t *aBuf, float const aTime)
	{
		return MReceiveData(NULL,aBuf, aTime);
	}

	param_t const& MGetInitParam() const;//deprecated using MGetSetting
	param_t const& MGetSetting() const
	{
		return MGetInitParam();
	}

	in_port_t MGetPort() const;
	std::ostream & MPrint(std::ostream & aStream) const;
	size_t MAvailable() const;
	NSHARE::CConfig MSerialize() const;
	virtual NSHARE::CConfig MSettings(void) const
	{
		return MGetInitParam().MSerialize();
	}
private:
	//Method

	inline CSocket MNewSocket();
	//Field
	CSocket FSock;
	smart_field_t<struct sockaddr_in> FAddr;
	diagnostic_io_t FDiagnostic;
	param_t FParam;
	uint8_t* FBuffer;
	size_t FBufferSize;
	size_t FMaxSize;
	//WTF? Fucking windows. If the received msg is more than the buffer size,
	//it generates the WSAEMSGSIZE error
	//If you using MSG_PEEK flags for big buffer, it will be egnored by winsock;
	//As my ideal code with dynamic allocating buffer is not working.
	//Now We receive msg as in the old times.
};
} //namespace USHARE

#endif /* CUDP_H_ */
