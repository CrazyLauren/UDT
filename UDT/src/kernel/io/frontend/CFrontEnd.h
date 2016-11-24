/*
 * CFrontEnd.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 26.09.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CFRONTEND_H_
#define CFRONTEND_H_

namespace NUDT
{

class CExternalChannel::CFrontEnd: public NSHARE::CIntrusived
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText FRONTEND_NAME;
	static const NSHARE::CText DEMAND;
	static const NSHARE::CText EXIT_PROTOCOL;

	CFrontEnd(NSHARE::CConfig const&,CExternalChannel&);
	~CFrontEnd();

	void MClose();
	void MOpen();
	//bool MIsOpen();

//	void MReceivedData(const data_t& aVal);

	void MReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);
	bool MSend(const data_t& aVal);
	bool MSend(const user_data_t& aVal);//todo cannot sent if protocol is not valid
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig const& MBufSettingFor(NSHARE::CConfig const& aFrom) const;
private:
	static int sMReceiver(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT, void* aData);
	void MReceiver();
	void MReceiverLoop();
	void MInit(const NSHARE::CConfig& aConf);
	void MConnected();
	void MDisconnected();
	void MRawReceivedData(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);
	void MParseDemands(const NSHARE::CConfig& aConf);
	bool MSendPacketsFromAnotherCustomer();
	bool MReceiveByProtocol(data_t::const_iterator aBegin,
			data_t::const_iterator aEnd);
	bool MSendSplitedPacket(const user_data_t& aVal);
	bool MSendFirstSplitedPacket(const user_data_t& aVal);

	NSHARE::CText FProtocol;
	std::set<NSHARE::CText> FSendProtocol;
	NSHARE::ISocket* FSocket;
	CExternalChannel& FThis;
	NSHARE::CConfig FConfig;
	descriptor_t Fd;
	split_info FSplit;
	unsigned FPacketNumber;
	programm_id_t FProgId;
	NSHARE::CBuffer FBuf;
	demand_dgs_t FDemands;
	std::list<user_data_t> FDataSequence;
	NSHARE::smart_field_t<user_data_info_t> FLastSplitedPacket;

	//bool FIsWorking;
};

} /* namespace NUDT */
#endif /* CFRONTEND_H_ */
