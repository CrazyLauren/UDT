/*
 * CHttpIOManger.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.05.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CHTTPLINK_H_
#define CHTTPLINK_H_
#include <io/IIOManager.h>
#include "CHttpRequest.h"
namespace NUDT
{
class CHttpRequest;
class CHttpResponse;
class IExtParser;
class CHttpIOManger:public IIOManager
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText PORT;
	static const NSHARE::CText PARSER_NAME;
	static const NSHARE::CText PARSER_ERROR;
	static const NSHARE::CText SNIFFER;
	static const NSHARE::CText I_WHAT_RECEIVE;
	static const NSHARE::CText WWW_PATH;
	static const NSHARE::CText SEQUENCE_NUMBER;
	CHttpIOManger();
	virtual ~CHttpIOManger();


	void MInit(CKernelIo *);
	bool MOpen(const void* = NULL);
	bool MIsOpen() const;
	void MClose();
	void MClose(const descriptor_t& aFrom);
	descriptors_t MGetDescriptors() const;
	bool MIs(descriptor_t)const;

	bool MSend(const data_t& aVal, descriptor_t const&);
	bool MSend(const program_id_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const kernel_infos_array_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const fail_send_t& aVal, descriptor_t const& aTo, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());
	bool MSend(const user_data_t& aVal, descriptor_t const& aTo);
	bool MSend(const demand_dgs_for_t& aVal, descriptor_t const& aFrom, const routing_t& aRoute=routing_t(),error_info_t const& =error_info_t());


	NSHARE::CConfig MSerialize() const;
	virtual NSHARE::CConfig const& MBufSettingFor(const descriptor_t&,
			NSHARE::CConfig const& aFrom) const
	{
		return NSHARE::CConfig::sMGetEmpty();
	}
private:
	typedef std::pair<required_header_t,IExtParser*> serializator_t;
	typedef std::map<uint32_t,serializator_t > serializators_t;

	typedef std::list<NSHARE::CConfig> sniffed_data_t;
	inline void MCloseImpl();
	static int sMConnect(void* aWho, void* aWhat, void* aThis);
	static int sMDisconnect(void* aWho, void* aWhat, void* aThis);

	bool MHandleFrom(CHttpRequest const* aRequest, CHttpResponse & _response);
	void MReceivedImpl(
			const NSHARE::ISocket::data_t& aData, const NSHARE::CTCPServer::recvs_t& aFrom);
	void MReceiver();
	static int sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void*);
	int MSettingDgParserFor(const NSHARE::CText& aReq,
			NSHARE::CText FProtocolName, required_header_t aNumber,NSHARE::CText aParser);
	bool MRemoveDgParserFor(uint32_t aNumber);
	bool MReceive( demand_dgs_t::value_type& _val,NSHARE::CText aParser);
	eStatusCode MReadFile(const NSHARE::CText& aPath, CHttpResponse& _response);
	eStatusCode MGetStateInfo(const CUrl& _url,CHttpResponse& _response);
	eStatusCode MHandleSniffer(const NSHARE::CConfig& aConf,const NSHARE::CText& aPath,CHttpResponse& _response);
	CKernelIo * FIo;

	descriptor_t Fd;
	program_id_t FProgId;
	user_datas_t FDataSequence;
	std::vector<CHttpRequest> FRequests;
	NSHARE::CTCPServer FTcpServiceSocket;
	NSHARE::CB_t FCBServiceConnect;
	NSHARE::CB_t FCBServiceDisconncet;
	uint32_t FUniqueNumber;

	demand_dgs_t FDemands;
	serializators_t FSerializators;
	NSHARE::CMutex FParserMutex;

	NSHARE::CMutex FSniffedMutex;
	sniffed_data_t FSniffedData;
	size_t FSinffedNum;
	size_t FRemainderSize;
};

} /* namespace NUDT */
#endif /* CHTTPLINK_H_ */
