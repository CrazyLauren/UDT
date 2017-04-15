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
	static const NSHARE::CText RECV_PROTOCOL;
	static const NSHARE::CText REPEAT_TIME;

	CFrontEnd(NSHARE::CConfig const&,CExternalChannel&);
	~CFrontEnd();

	void MClose();
	void MOpen();
	//bool MIsOpen();

//	void MReceivedData(const data_t& aVal);

	void MReceivedData(NSHARE::CBuffer& aData);
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

	void MParseDemands(const NSHARE::CConfig& aConf);

	NSHARE::CText FReceiveProtocol;
	std::set<NSHARE::CText> FSendProtocol;
	NSHARE::ISocket* FSocket;
	CExternalChannel& FThis;
	NSHARE::CConfig FConfig;
	descriptor_t Fd;
	split_info FSplit;
	uint16_t FPacketNumber;
	program_id_t FProgId;
	demand_dgs_t FDemands;
	unsigned FRepeatTime;
};

} /* namespace NUDT */
#endif /* CFRONTEND_H_ */
