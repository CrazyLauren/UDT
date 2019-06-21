/*
 * CCustomerImpl.h
 *
 *  Created on: 19.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCUSTOMERIMPL_H_
#define CCUSTOMERIMPL_H_

#include <CResources.h>
#include <CConfigure.h>
#include <CIOFactory.h>
#include <CDataObject.h>
#include <ICustomer.h>
namespace NUDT
{
typedef NSHARE::CEvent<CCustomer*, callback_t, void*> event_t;
template<class T,class T1>
class ievents_type
{
public:
	typedef event_handler_info_t value_t;
};
typedef NSHARE::CEvents<NSHARE::CText, event_t, ievents_type> events_t;

struct CCustomer::_pimpl: public ICustomer, public events_t
{
	typedef NSHARE::CSafeData<customers_t> safety_customers_t;
	typedef HASH_MAP<uint32_t,callback_t> cb_event_t;


	_pimpl(CCustomer& aThis);

	int MInitialize(NSHARE::CText const& aProgram, NSHARE::CText const& aName,NSHARE::version_t const& aVersion);
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
	int MSendTo(unsigned aNumber,NSHARE::CBuffer & aBuf, const NSHARE::uuid_t& aTo,NSHARE::version_t const& aVer, eSendToFlags);
	int MSendTo(unsigned aNumber, NSHARE::CBuffer & aBuf,NSHARE::version_t const& aVer, eSendToFlags);
	int MSendTo(required_header_t const& aNumber, NSHARE::CText aProtocolName,
			NSHARE::CBuffer & aBuffer, eSendToFlags);
	int MSendTo(required_header_t const& aNumber, NSHARE::CText aProtocolName,
			NSHARE::CBuffer & aBuffer, const NSHARE::uuid_t& aTo, eSendToFlags);

	void MGetMyWishForMSG(std::vector<request_info_t>& aTo) const;
	int MSettingDgParserFor(requirement_msg_info_t  aNumber,
			const callback_t& aHandler);
	int MRemoveDgParserFor( requirement_msg_info_t  aNumber);
	int MRemoveDgParserFor( demand_dg_t::event_handler_t  aNumber);
	customers_t MCustomers() const;
	program_id_t MCustomer(NSHARE::uuid_t const&) const;
	NSHARE::CBuffer MGetNewBuf(size_t aSize) const;
//	static void sMReceiver(id_t const& aFrom, data_t::value_type const* aData,
//			unsigned aSize);

	int  MUdpateRecvList() const;
	void MEventConnected();
	void MEventDisconnected();
	int MWaitForEvent(NSHARE::CText const& aEvent, double aSec);
	void MJoin();
	uint16_t MNextUserPacketNumber();
private:
	typedef std::set<NSHARE::CText,NSHARE::CStringFastLessCompare> wait_for_t;
	static int sMReceiver(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMReceiveCustomers(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMFailSents(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMDemands(CHardWorker* aWho, args_data_t* aWhat, void* aData);

	void MReceiver(recv_data_from_t & _from);

	int MInitId(NSHARE::CText const& aProgram, NSHARE::CText const& aName,NSHARE::version_t const& aVersion);
	int MInitCallBacks();
	int MLoadLibraries();
	int MInitFactorys();
	int MSendImpl( NSHARE::CBuffer & aBuf,user_data_t& _data);
	inline int MCallImpl(key_t const& aKey, value_arg_t const& aCallbackArgs);
	//-----------------
	CCustomer& FThis;

	program_id_t FMyId; //todo const

	IIOConsumer* FWorker;
	mutable NSHARE::CCondvar FCondvarWaitFor;
	mutable NSHARE::CMutex FMutexWaitFor;
	mutable wait_for_t FWaitFor;
//	NSHARE::CThread FThread;
//	NSHARE::CWaitingQueue<data_from_t> FData;
//	CDataObject FData;

//	protocol_parser_t FParserToCustomer;
	safety_customers_t FCustomers;

	demand_dgs_t FDemands;
	HASH_MAP<uint32_t,callback_t> FEvents;

	mutable NSHARE::CMutex FCommonMutex;
	mutable NSHARE::CMutex FParserMutex;
	uint32_t FUniqueNumber;
	uint16_t FMainPacketNumber;

	friend class CCustomer;
};
} //
#endif /* CCUSTOMERIMPL_H_ */
