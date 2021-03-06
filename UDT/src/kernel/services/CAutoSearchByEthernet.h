/*
 * CAutoSearchByEthernet.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 02.06.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CAUTOSEARCHBYETHERNET_H_
#define CAUTOSEARCHBYETHERNET_H_

#include <core/ICore.h>
#include <SHARE/UType/CSingleton.h>
#include <SHARE/share_socket.h>
#include <parser_in_protocol.h>

namespace NUDT
{

/** @brief This class is autosearching the other kernel by UDP
 * and connected it by TCP
 *
 */
class CAutoSearchByEthernet: public NSHARE::CSingleton<CAutoSearchByEthernet>,public ICore
{
public:
	static const NSHARE::CText NAME;///< A serialization key
	static const NSHARE::CText PORT;

	static const NSHARE::network_port_t DEFAULT_PORT;///<The default port number

	CAutoSearchByEthernet();
	virtual ~CAutoSearchByEthernet();

	NSHARE::CConfig MSerialize() const;

	/*! @brief Start working (has to be non-blocking)
	 *
	 *	@return true if started successfully
	 */
	bool MStart();
	void MStop();
private:
	typedef CInParser<CAutoSearchByEthernet,NSHARE::net_address> parser_t;///< type of protcol parser
	typedef std::vector<auto_search_info_t> list_of_kernels_t; ///< A list of NUDT::auto_search_info_t type

	inline void MInit();
	static NSHARE::eCBRval sMMainLoop(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	void MMainLoop();
	static NSHARE::eCBRval sMNewKernelHandler(NSHARE::CThread const* WHO,
			NSHARE::operation_t * WHAT, void*);
	void MHandleConncections();

	void MSendBroadcast();
	void MReceiveLoop();

	template<class DG_T> void MFill(NSHARE::ISocket::data_t*);
	template<class DG_T>
	void MProcess(DG_T const* aP, parser_t*);

	bool MConnectTo(auto_search_info_t const&);
	void MWaitForServerStarted();
	bool MPushInfo(const auto_search_info_t& aInfo);
	CKernelIOByTCPClient* MGetTCPClientIOManger() const;
	CKernelIOByTCP* MGetTCPServerIOManger() const;
	bool MIsWoking() const;

	NSHARE::CUDP FUdp;///< A broadcast port
	parser_t FProtocolParse;///<A protocol parser
	list_of_kernels_t FListOfNotHandledKernel; ///< A list of new kernel which hasn't handled yet
	NSHARE::CMutex FMutex; ///< A mutex for lock FListOfNotHandledKernel
	list_of_kernels_t FListOfID; ///< List of known program, the first is the own search info
	NSHARE::CMutex FThreadMutex; ///<It's used for lock receive operation

	friend class CInParser<CAutoSearchByEthernet,NSHARE::net_address> ;
};

} /* namespace NUDT */

#endif /* CAUTOSEARCHBYETHERNET_H_ */
