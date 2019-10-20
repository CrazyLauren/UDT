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
#include <udt/CCustomer.h>
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

/** Implementation of customer
 *
 * The architecture is based on plugins architecture. There are three
 * main singleton object\n
 *  - #CDataObject, that is responsible to communication between plugins, \n
 *  - #CConfigure, that is store configuration \n
 *  - #CResources, that is load the plugins. \n
 *
 *  All plugins is available by its "factory", singleton object that
 *  store pointer to interface of plugin. Of course, the factories
 *  are created  against type of plugin interface.
 *
 *  @todo remove ICustomer, move events to CDataObject
 */
struct CCustomer::_pimpl: public ICustomer, public events_t
{
	typedef NSHARE::CSafeData<customers_t> safety_customers_t;
	typedef HASH_MAP<uint32_t,callback_t> cb_event_t;

	_pimpl(CCustomer& aThis);

	/** Initialzie library
	 *
	 * @param aProgram Non-unique program name (in FS)
	 * @param aName Non-unique program name in NET
	 * @param aVersion Version of program
	 * @return 0 - EOK
	 *			else bitwise error code
	 */
	int MInitialize(NSHARE::CText const& aProgram, NSHARE::CText const& aName,NSHARE::version_t const& aVersion);
	~_pimpl();


	/** @copydoc ICustomer::MIsOpened()
	 *
	 */
	bool MIsOpened() const;

	/** @copydoc ICustomer::MIsConnected()
	 *
	 */
	bool MIsConnected() const;

	/** @copydoc ICustomer::MOpen()
	 *
	 */
	bool MOpen();

	/** @copydoc ICustomer::MClose()
	 *
	 */
	void MClose();

	/** @copydoc ICustomer::MAvailable()
	 *
	 */
	bool MAvailable(const NSHARE::CText& aModule) const;

	/** @copydoc ICustomer::MAllAvailable()
	 *
	 */
	modules_t MAllAvailable() const;

	/** @copydoc ICustomer::MGetModules()
	 *
	 */
	array_of_modules_t MGetModules() const;

	/** @copydoc ICustomer::MGetModule()
	 *
	 */
	IModule* MGetModule(const NSHARE::CText& aModule) const;

	/** @copydoc ICustomer::MCustomers()
	 *
	 */
	customers_t MCustomers() const;

	/** @copydoc ICustomer::MCustomer()
	 *
	 */
	program_id_t MCustomer(NSHARE::uuid_t const&) const;

	/** @copydoc ICustomer::MGetNewBuf()
	 *
	 */
	NSHARE::CBuffer MGetNewBuf(size_t aSize) const;

	/** @copydoc ICustomer::MGetDataObject()
	 *
	 */
	CDataObject& MGetDataObject() const;

	/** @copydoc ICustomer::MGetResourceObject()
	 *
	 */
	CResources& MGetResourceObject() const;

	/** @copydoc ICustomer::MGetConfigureObject()
	 *
	 */
	CConfigure& MGetConfigureObject() const;

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


	int  MUdpateRecvList() const;

	int MWaitForEvent(NSHARE::Strings aEvent, double aSec);
	void MJoin();
	uint16_t MNextUserPacketNumber();

	/** Gets RTC for working
	 *
	 * @param aName Name of RTC
	 * @return pointer to RTC or NULL
	 * @warning Call this method as little as possible.
	 * 			It's looking for the specified RTC.
	 */
	IRtc* MGetRTC(NSHARE::CText const& aName) const;

	/** Gets list of available RTC
	 *
	 * @return list of RTC
	 */
	std::vector<IRtc*> MGetListOfRTC() const;

	/** Store info about new module
	 *
	 * @param aModule An mew module
	 */
	void MPutModule(IModule* aModule);

	/** Remove info about new module
	 *
	 * @param aModule An mew module
	 */
	void MPopModule(IModule* aModule);
private:
	typedef std::set<NSHARE::CText,NSHARE::CStringFastLessCompare> wait_for_t;
	static int sMReceiver(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMReceiveCustomers(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMFailSents(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMDemands(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMConnect(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMDisconnect(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMRTCUpdated(CHardWorker* aWho, args_data_t* aWhat, void* aData);

	void MReceiver(recv_data_from_t & _from);

	int MInitId(NSHARE::CText const& aProgram, NSHARE::CText const& aName,NSHARE::version_t const& aVersion);
	int MInitCallBacks();
	int MLoadLibraries();
	int MInitFactorys();
	int MSendImpl(user_data_t& _data);
	inline int MCallImpl(key_t const& aKey, value_arg_t const& aCallbackArgs);
	bool MInitializeIOManager(NSHARE::CThread::param_t* aParam);
	bool MOpenModules(NSHARE::CThread::param_t* _pparam);
	bool MInitializeModules();
	void MCloseModules();
	void MEventConnected();
	void MEventDisconnected();

	void MFillOutputMessage(received_message_args_t* aTo,
			recv_data_from_t & aFrom);
	void MInformSubscriber(received_message_args_t* const aData,
			user_data_info_t const& aFrom);
	bool MHandleSubscription(
			demand_dg_t::event_handler_t const& aHandler,
			 user_data_info_t const& aDataInfo,
			received_message_args_t& aMessage);
	void MInformInvalidMessage(const user_data_info_t& aDataInfo,
			user_error_type aError) const;
	int MAddEventToWaitList(NSHARE::Strings const& aEvents);
	bool MIsEventIngnored(const NSHARE::CText& _event) const;
	bool MIsKeepOnWaiting(NSHARE::Strings* aEvents) const;
	unsigned MPutEvents(const NSHARE::Strings& aEvents,
			NSHARE::Strings* aAddedList);

	//-----------------
	CCustomer& FThis;

	program_id_t FMyId; //!< Info about me

	IIOConsumer* FMainIO;//!< IO communication with kernel
	mutable NSHARE::CCondvar FCondvarWaitFor;
	mutable NSHARE::CMutex FMutexWaitFor;
	mutable wait_for_t FWaitFor;


	safety_customers_t FCustomers;

	demand_dgs_t FDemands;
	HASH_MAP<uint32_t,callback_t> FEvents;

	mutable NSHARE::CMutex FCommonMutex;
	mutable NSHARE::CMutex FParserMutex;
	uint32_t FUniqueNumber;
	uint16_t FMainPacketNumber;
	array_of_modules_t FModules;//!< Info about modules (sorted)

	friend class CCustomer;
};
} //
#endif /* CCUSTOMERIMPL_H_ */
