/*
 * CRequiredDG.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 18.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CREQUIREDDG_H_
#define CREQUIREDDG_H_
#include <udt_types.h>
#include <shared_types.h>

#include <IExtParser.h>

namespace NUDT
{

class CRequiredDG
{
public:
	struct msg_handlers_t:std::vector<demand_dg_t::event_handler_t>
	{
		msg_handlers_t():FNumberOfRealHandlers(0)
		{

		}
		bool MIsRegistrarExist() const;


		NSHARE::version_t FVersion;
		int FNumberOfRealHandlers;
		//see demands_dg_t::FFlags.
		//If At least one handler is not registrator than The consumer is not registator.
	};
	struct uuids_of_receiver_t:std::map<NSHARE::uuid_t,msg_handlers_t >
	{
		uuids_of_receiver_t():FNumberOfRealReceivers(0u)
		{

		}
		unsigned FNumberOfRealReceivers;//!< the number of registrar is size()-FNumberOfRealReceivers
	};
	typedef std::set<NSHARE::uuid_t> unique_uuids_t;
	//typedef std::map<NSHARE::CText,unique_uuids_t> uuids_of_t;
	typedef std::map<required_header_t, uuids_of_receiver_t, CReqHeaderFastLessCompare> uuids_of_expecting_dg_t;
	struct data_routing_t
	{
		NSHARE::CBuffer FBufferedData;
		uuids_of_expecting_dg_t FExpected;
	};
	typedef std::map<NSHARE::CText, data_routing_t,
			NSHARE::CStringFastLessCompare> protocols_t;

	typedef std::map<NSHARE::uuid_t, protocols_t> protocol_of_uuid_t;

	static const NSHARE::CText NAME;
	CRequiredDG();
	 ~CRequiredDG();

	/*!\brief Initialize genealogical tree of
	 * messages
	 *
	 */
	bool MInitializeMsgInheritance();

	/*!< \brief Update requirement messages list for the client
	*
	*	It updates requirement messages list for
	*	the client and subscribes(unsubscribes) it to
	*	currently available messages.
	*
	*	\return first - current messages subscription list
	*			second - list of unsubscribed messages
	*/
	std::pair<demand_dgs_for_t, demand_dgs_for_t> MSetDemandsDGFor(id_t const&, demand_dgs_t const&);

	/*!< \brief Adding a new client to the publisher-
	 * subscribe list
	 *
	 *	It adds a new client and generate the subscriber's
	 *	list on published client's messages.
<<<<<<< HEAD
	 *
	 *	\return list of subscribers on published client's
	 *	 messages
=======

	*\return first - list of subscribers on messages
	 *					which client is published
	 *			second - list of unsubscribed messages
>>>>>>> f3da2cc... see changelog.txt
	 */
	demand_dgs_for_t MAddClient(id_t const& aId );

	/*!< \brief Removing the client from the publisher-
	 * subscribe list
	 *
	 *	It removes the client and unsubscribe the other
	 *	clients to the published client's messages
	 *
<<<<<<< HEAD
	 *	\return list of unsubscribed messages
=======
	*\return first - lis of a new subscribers
	 *			second - list of unsubscribers on messages
	 *					which client has been published
>>>>>>> f3da2cc... see changelog.txt
	 */
	demand_dgs_for_t MRemoveClient(NSHARE::uuid_t const& aUUID);


	/*!< \brief  Split packet to messages and
	 * addition info about message's receivers
	 *
	 *	The "message info" is filled uuids of message's
	 *	receivers including registrars
	 *
	 *\param [In, out] aFrom handling packets.
	 *  If some messages hasn't receivers, it hasn't
	 *  removed from the argument.
	 *
	 *\param [out] aTo List of packets
	 *  that has at the least one receiver.
	 *
	*\param [out] aFail List of error's
	 *
	 */
	void MFillMsgReceivers(user_datas_t* const aFrom, user_datas_t* const aTo,fail_send_array_t* const aFail) const;


	void MFillMsgHandlersFor(user_datas_t & aFrom,user_datas_t &aTo,fail_send_array_t & aError) const;


	/* !< \brief get uuids that satisfy the
	 * Identification name
	 *
<<<<<<< HEAD
	 * 	\param aName Identification name
	 *
	 * 	\return Uuids
=======
	 *\param aName Identification name
	 *\param isInvertGroup if the value (isInvertGroup) is true then the program order@com.ru.putin
	 * 						 is not enter into the order@com.ru(aName),
	 * 						 but is enter into the order@com.ru.putin.vv(aName)
	 * 	\return pair uuid - depth
	 *
	 */
	unique_uuids_t  MGetUUIDFor(NSHARE::CProgramName const& aName,bool isInvertGroup=false) const;

	/*!\brief Gets info about the requirement messages
>>>>>>> f3da2cc... see changelog.txt
	 *
	 */
	unique_uuids_t  MGetUUIDFor(NSHARE::CRegistration const& aName) const;
	demand_dgs_for_t const& MGetDemands() const;

	/*!< \brief Make valid the message's byte order
	 *
	*\param aData Message
	 *
	*\return 0 - no error
	*\note Make valid only body of message that is
	 *		  The message header  stay not valid.
	 */
	static unsigned sMSwapEndian( user_data_t& aData);
<<<<<<< HEAD
	NSHARE::CConfig MSerialize() const;
private:
=======

	/*!\brief serializes object
	 *
	 *\return serialized object, base key is equal the NAME field.
	 */
	NSHARE::CConfig MSerialize() const;

	/*!\brief gets list of identifier
	 *
	 */
	unique_id_t const& MGetUniquieID() const;

	/*!\brief Returns demands for
	 *
	 *\param aUUID - looking for
	 *\return demand or empty array
	 */
	demand_dgs_t const& MGetDemandFor(NSHARE::uuid_t const& aUUID) const;


	/*!\brief Returns the request info
	 * by which the message will be sent from
	 *
	 *\param aFrom - the message is sent from
	 *\param [out] aTo - where the result is saved
	 *
	 *\return The amount of demands which  has been added
	 *
	 */
	unsigned MGetWhatMsgSentFrom(NSHARE::uuid_t const & aFrom,
				demand_dgs_for_t * aTo) const;

	/*!\brief Returns ID info by UUID
	 *
	 *\param aUUID looked for UUID
	 *
	 *\return ID or invalid (!MIsValid()) id_t if uuid not found
	 */
	id_t const& MGetIdBy(NSHARE::uuid_t const & aUUID) const;
private:
	struct msg_handlers_t: std::vector<demand_dg_t::event_handler_t> //todo sort by messages inherent priority
	{
		msg_handlers_t():FNumberOfRealHandlers(0)
		{

		}
		bool MIsRegistrarExist() const;


		NSHARE::version_t FVersion;
		int FNumberOfRealHandlers;
		//see demands_dg_t::FFlags.
		//If At least one handler is not registrator than The consumer is not registator.
	};
	struct uuids_of_receiver_t:std::map<NSHARE::uuid_t,msg_handlers_t >
	{
		uuids_of_receiver_t():
			FNumberOfRealReceivers(0u)//
			//,FCurrentUUIDLevel(std::numeric_limits<unsigned>::max())
		{

		}
		unsigned FNumberOfRealReceivers;///< the amount of registrar is size()-FNumberOfRealReceivers
	};
	//typedef std::map<NSHARE::CText,unique_uuids_t> uuids_of_t;
	typedef std::map<required_header_t, uuids_of_receiver_t, CReqHeaderFastLessCompare> uuids_of_expecting_dg_t;
	struct data_routing_t
	{
		NSHARE::CBuffer FBufferedData;
		uuids_of_expecting_dg_t FExpected;
	};
	typedef std::map<NSHARE::CText, data_routing_t,
			NSHARE::CStringFastLessCompare> protocols_t;

	typedef std::map<NSHARE::uuid_t, protocols_t> protocol_of_uuid_t;

	typedef std::pair<NSHARE::CText, required_header_t> msg_header_t;
	typedef std::vector<msg_header_t> msg_heritance_t;

	struct msg_family_t
	{
		msg_heritance_t FChildren;
		msg_heritance_t FParents;
	};
	typedef std::map<required_header_t, msg_family_t, CReqHeaderFastLessCompare> msg_inheritance_tree_t;///< key - message type, value - its hierarchy
	typedef std::map<NSHARE::CText, msg_inheritance_tree_t> msg_inheritances_t;	///< key message protocol ,
																				//value -  genealogy tree of this tree

	typedef std::map<demand_dg_t::event_handler_t, unsigned> current_nearest_t;
	typedef std::map<NSHARE::uuid_t, current_nearest_t> nearest_info_t;

	struct demand_for_info_t
	{
		NSHARE::uuid_t FFor;
		demand_dg_t FWhat;
		unsigned FDistance;
	};
	struct way_info_t
	{
		NSHARE::uuid_t FFrom;
		demand_for_info_t FTo;
	};
	typedef std::vector<way_info_t> array_of_demand_for_t;
	struct unique_compare_t;
>>>>>>> f3da2cc... see changelog.txt

	bool MFillRouteAndDestanationInfo(uuids_of_receiver_t const& aRoute,
			user_data_info_t* const aInfo,
			fail_send_array_t* const aFail) const;

	inline void MFillByUserProtocol(user_datas_t* const aFrom,user_datas_t* const aTo,
			fail_send_array_t * const aFail) const;
	inline void MFillByRawProtocol(user_datas_t*const aFrom,user_datas_t*const aTo,
			fail_send_array_t * const aFail) const;

	inline void MSendPacketFromTo(NSHARE::uuid_t const& aFrom,
			NSHARE::uuid_t const& aTo, demand_dg_t const& aWhat,
			demand_dgs_for_t* aNew /*is null - registrator*/);
	inline void MUnSendPacketFromTo(NSHARE::uuid_t const& aFrom,
			NSHARE::uuid_t const& aTo, demand_dg_t const& aWhat,
			demand_dgs_for_t* aOld /*is null - registrator*/);

	void MRemoveDemandsFor(NSHARE::uuid_t const& aFor,demand_dgs_t const& aFrom,demand_dgs_for_t* aRemoved);
	void MAddDemandsFor(NSHARE::uuid_t const& aFor,const demand_dgs_t& aReqDgs, demand_dgs_for_t* aNew);

	void MAddReceiversFor(id_t const& aId,demand_dgs_for_t& aNew);
	bool MRemoveReceiversFor(NSHARE::uuid_t const&,demand_dgs_for_t* aRemoved);

	bool MGetDiffDemandsDG(id_t const& aFor, demand_dgs_t const& aNew, demand_dgs_t& aRem,demand_dgs_t& aAdd);
	bool MDoesIdConformTo(id_t const& aFor,NSHARE::CRegistration const&) const;
	uint32_t MNextMsgMask() const;
	bool MRegisteringReceiverForMsg(const demand_dg_t& aWhat,
			const NSHARE::uuid_t& aFrom, const NSHARE::uuid_t& aTo,
			uuids_of_expecting_dg_t* const _uuids, demand_dgs_for_t* aNew);
	bool MUnRegisteringReceiverForMsg(demand_dg_t const & aWhat,
			NSHARE::uuid_t const &  aFrom, NSHARE::uuid_t const &aTo,
			 uuids_of_expecting_dg_t* const aUuids,
			demand_dgs_for_t* aOld);
	bool MCheckVersion(const NSHARE::version_t& _req_version,
			const NSHARE::uuid_t& _uuid,  user_data_info_t& aInfo) const;
	void MAddUUIDToRoute(msg_handlers_t const&  _kt,
			NSHARE::uuid_t const& _uuid, user_data_info_t& aInfo) const;
	inline error_type MAddReceiverOfMsg(msg_handlers_t const& aHandlers,
			NSHARE::uuid_t const & _uuid, user_data_info_t& aInfo) const;
	inline user_datas_t::iterator MExtractMessages(IExtParser::result_t const & _msgs,
			user_datas_t *const aFrom,
			user_datas_t::iterator aWhat,
			user_datas_t *const aTo,IExtParser const& aP) const;
	inline void MRemoveDataWithUserErrors(IExtParser::result_t *const _msgs,
			user_datas_t *const aFrom, user_datas_t *const aTo, fail_send_array_t *const aFail) const;
	inline void MAddReceiversForMessages(IExtParser::result_t const& _msgs,
			user_datas_t* const aFrom, user_datas_t * const aTo,
			uuids_of_expecting_dg_t const& aUUIDs, IExtParser const& _p,
			fail_send_array_t* aFail) const;
	inline NSHARE::CBuffer MParseData(IExtParser::result_t* _msgs,
			user_data_t& _data, IExtParser& _p) const;
	data_routing_t& MGetOrCreateExpectedListFor(NSHARE::uuid_t const & aFrom,
			NSHARE::CText const & aProtocol) const;
	inline bool MAddHandler(demand_dg_t const & aWhat,
			msg_handlers_t& _handlers) const;
	inline bool MRemoveHandler(demand_dg_t const & aWhat,
			msg_handlers_t& _handlers) const;
	inline bool MGetValidHeader(const demand_dg_t& aWhat,demand_dg_t * aTo) const;
	inline uuids_of_receiver_t const& MGetUUIDsOfReceivers(uuids_of_expecting_dg_t const & aUUIDs,	required_header_t const & _msg) const;
	inline uuids_of_receiver_t & MGetOrCreateUUIDsOfReceivers(uuids_of_expecting_dg_t & aUUIDs,	required_header_t const & _msg);
	inline void MAppendBufferedData(data_routing_t& _routing,
			user_data_t& _data) const;
	inline bool MBufferingDataIfNeed(data_routing_t& _routing,NSHARE::CBuffer const & _tail	) const;
	inline static bool sMSwapHeaderEndian(NSHARE::CText const & aProtocol, required_header_t* aTo);
	inline static bool sMSwapMessageEndian(NSHARE::CText const & aProtocol,
<<<<<<< HEAD
			required_header_t const& aType, NSHARE::CBuffer* aTO);
	msg_handlers_t const* MGetHandlers(user_data_info_t & aFrom) const;

	inline void MInitializeMsgInheritance();//todo

=======
			required_header_t const& aType, NSHARE::CBuffer* const aTO);
	msg_handlers_t const* MGetHandlers(user_data_info_t const & aFrom) const;

	msg_heritance_t const& MGetMessageChildren(
			NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType) const;
	static msg_heritance_t const& sMGetMessageChildren(
			NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType,msg_inheritances_t const& aFrom);
	static void sMAddMessageChildren(
			NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType,
			msg_heritance_t const& aChildren,msg_inheritances_t *const aTo);

	msg_heritance_t const& MGetMessageParents(
			NSHARE::CText const& aMsgProtocol, required_header_t const & aMsgType) const;

	unsigned MIncludeMessageChildren(NSHARE::uuid_t const & aFrom,
			NSHARE::uuid_t const & aTo, demand_dg_t const & aWhat,
			uuids_of_expecting_dg_t*  aIncludeTo, demand_dgs_for_t* const aNew, demand_dgs_for_t* const aOld);
	unsigned MUnincludeMessageChildren(NSHARE::uuid_t const & aFrom,
			NSHARE::uuid_t const & aTo, demand_dg_t const & aWhat,
			uuids_of_expecting_dg_t*  aIncludeTo,demand_dgs_for_t* const aNew, demand_dgs_for_t* const aOld);
	inline void MSerializeMsgExpectedList(NSHARE::CConfig* const aTo) const;
	inline void MReadMsgChild(msg_inheritances_t * const aTo) const;
	inline unsigned MCreateGenealogyTreeFromChildInfo(
			msg_inheritances_t * const aChildInfo,msg_inheritances_t * const aTo) const;
	inline void MParentInfo(msg_inheritances_t * const aTo) const;
	void  MGetOnlyNearestUUIDFor(unique_uuids_t * const aTo) const;
	inline bool MRemoveAllReceiversForMsgsFrom(id_t const& aFrom,demand_dgs_for_t* const aNew,demand_dgs_for_t* const aRemoved);
	static unsigned sMGetDemandsByHandlers(msg_handlers_t const & aHandlers,
			demand_dgs_t const & aDemand, demand_dgs_t* aTo);
	void MTakeIntoAccountOnlyNearestSendersIfNeed(NSHARE::uuid_t const& aFrom, array_of_demand_for_t* const aNew,
			array_of_demand_for_t* const aRemoved);
	unsigned MUpdateSendInfoFor(NSHARE::uuid_t const& aFor,
			array_of_demand_for_t const& aAdd, array_of_demand_for_t const& aRemove, demand_dgs_for_t* const aNew,
			demand_dgs_for_t*  constaRemoved);
	void MGetListOfReceiversOfMsgsFrom(id_t const & aFrom,
			array_of_demand_for_t*const aTo) const;
	void MGetDistance(id_t const & aFrom, demand_dgs_for_t const& aFor,
			array_of_demand_for_t* aTo);
	void MRemoveSenderIfNeed(NSHARE::uuid_t const& aFrom, array_of_demand_for_t*const aAdded,
			array_of_demand_for_t*const aRemoved);
	void MAddSenderIfNeed(NSHARE::uuid_t const & aFrom,
			array_of_demand_for_t const& aRemoved, array_of_demand_for_t*const aAdded);

	static unsigned sMCheckCorrectionOfGenealogyTree(msg_inheritances_t const& aWhat,msg_inheritances_t* aTo);
>>>>>>> 5d2f97a... see ChangeLog.txt

	mutable protocol_of_uuid_t FWhatIsSendingBy;
<<<<<<< HEAD
	demand_dgs_for_t FDGs;
	std::set<id_t> FIds;
	mutable uint16_t FMsgID;
=======
	demand_dgs_for_t FDGs;///< list of the requirement messages for uuid
	unique_id_t FIds;
	mutable uint16_t FMsgID;
	msg_inheritances_t FMsgsGenealogy;
	unsigned FMaxInheritanceDepth;
	nearest_info_t FNearestInfo;///< contains current "group depth" of demand
>>>>>>> f3da2cc... see changelog.txt
};
inline bool CRequiredDG::msg_handlers_t::MIsRegistrarExist() const
{
	return !empty() && FNumberOfRealHandlers!=size();
}

} /* namespace NUDT */
#endif /* CREQUIREDDG_H_ */
