/*
 * CCustomerImpl.h
 *
 *  Created on: 19.01.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCUSTOMERIMPL_H_
#define CCUSTOMERIMPL_H_

#include "ICustomer.h"
namespace NUDT
{
typedef NSHARE::CEvent<CCustomer*, callback_t, void*> event_t;
template<class T,class T1>
class ievents_type
{
public:
	typedef CCustomer::value_t value_t;
};
typedef NSHARE::CEvents<NSHARE::CText, event_t, ievents_type> events_t;

struct CCustomer::_pimpl: public ICustomer, public events_t
{
	typedef NSHARE::CSafeData<customers_t> safety_customers_t;

//	typedef std::map<required_header_t, callback_t, CReqHeaderFastLessCompare> dg_handlers_t;
//	typedef std::map<NSHARE::CText, dg_handlers_t, NSHARE::CStringFastLessCompare> parser_for_protocol_t;
//	//1 - customer name,2 - protocols
//	typedef std::map<NSHARE::CRegistration, parser_for_protocol_t> customers_parser_t;//fixme optimized. The first has to be by protocol
//	//then by required_header_t and then vector of pair<NSHARE::CRegistration,callback_t>

	typedef std::map<NSHARE::CRegistration, callback_t> customer_handlers_t;
	typedef std::map<required_header_t, customer_handlers_t, CReqHeaderFastLessCompare> header_for_protocol_t;
	typedef std::map<NSHARE::CText, header_for_protocol_t, NSHARE::CStringFastLessCompare> protocol_parser_t;

	_pimpl(CCustomer& aThis);
	int MInitialize(NSHARE::CText const& aProgram, NSHARE::CText const& aName);
	~_pimpl();


	static void sMInitConfigure(const NSHARE::CConfig& aConf);

	bool MIsOpened() const;
	bool MIsConnected() const;
	bool MOpen();
	void MClose();

	bool MAvailable(const NSHARE::CText& aModule) const;
	bool MAvailable() const;
	modules_t MAllAvailable() const;

	int MSendTo(const NSHARE::CText& aProtocolName, NSHARE::CBuffer & aBuf, const NSHARE::CText& aTo, eSendToFlags);
	int MSendTo(const NSHARE::CText& aProtocolName,NSHARE::CBuffer & aBuf, const NSHARE::uuid_t& aTo, eSendToFlags);
	int MSendTo(unsigned aNumber,NSHARE::CBuffer & aBuf, const NSHARE::uuid_t& aTo, eSendToFlags);
	int MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf, eSendToFlags);

	int MParseData(args_t & _raw_args) const;

	int MSettingDgParserFor(const NSHARE::CText& aTo, dg_parser_t aNumber,
			const callback_t& aHandler);
	int MRemoveDgParserFor(const NSHARE::CText& aTo, dg_parser_t aNumber);
	customers_t MCustomers() const;
	NSHARE::CBuffer MGetNewBuf(unsigned aSize) const;
//	static void sMReceiver(id_t const& aFrom, data_t::value_type const* aData,
//			unsigned aSize);

	int  MUdpateRecvList() const;
	void MEventConnected();
	void MEventDisconnected();
	void MWaitForReady(double aSec);
	id_t const MGetIdFor(NSHARE::uuid_t const&) const;
private:
	static int sMReceiver(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMReceiveCustomers(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMFailSents(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMDemands(CHardWorker* aWho, args_data_t* aWhat, void* aData);

	void MReceiver(recv_data_from_t & _from);
	int MCallCBForUserProtocol(IExtParser*,IExtParser::result_t const& _result,header_for_protocol_t const& _par, args_t& _raw_args) const;
	int MCallCBForRawProtocol(header_for_protocol_t const& _par,args_t& _raw_args) const;

	int MInitId(NSHARE::CText const& aProgram, NSHARE::CText const& aName);
	int MInitCallBacks();
	int MLoadLibraries();
	int MInitFactorys();
	bool MDoesIdConformTo(id_t const& _id,
			NSHARE::CRegistration const& _reg) const;
	int MCallCBFor(customer_handlers_t const& _handler,
			args_t& _raw_args) const;

	//-----------------
	CCustomer& FThis;

	programm_id_t FMyId; //todo const

	IIOConsumer* FWorker;
	bool FIsReady;
	mutable NSHARE::CCondvar FCondvarWaitFor;
	mutable NSHARE::CMutex FMutexWaitFor;
//	NSHARE::CThread FThread;
//	NSHARE::CWaitingQueue<data_from_t> FData;
//	CDataObject FData;

	protocol_parser_t FParserToCustomer;
	safety_customers_t FCustomers;

	mutable NSHARE::CMutex FCommonMutex;
	mutable NSHARE::CMutex FParserMutex;


	friend class CCustomer;
};
} //
#endif /* CCUSTOMERIMPL_H_ */
