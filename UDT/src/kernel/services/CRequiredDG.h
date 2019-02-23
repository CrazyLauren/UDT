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
	 *
	 *	\return list of subscribers on published client's
	 *	 messages
	 */
	demand_dgs_for_t MAddClient(id_t const& aId );

	/*!< \brief Removing the client from the publisher-
	 * subscribe list
	 *
	 *	It removes the client and unsubscribe the other
	 *	clients to the published client's messages
	 *
	 *	\return list of unsubscribed messages
	 */
	demand_dgs_for_t MRemoveClient(NSHARE::uuid_t const& aUUID);


	/*!< \brief  Split packet to messages and
	 * addition info about message's receivers
	 *
	 *	The "message info" is filled uuids of message's
	 *	receivers including registrars
	 *
	 *  \param [In, out] aFrom handling packets.
	 *  If some messages hasn't receivers, it hasn't
	 *  removed from the argument.
	 *
	 * \param [out] aTo List of packets
	 *  that has at the least one receiver.
	 *
	 *	\param [out] aFail List of error's
	 *
	 */
	void MFillMsgReceivers(user_datas_t* const aFrom, user_datas_t* const aTo,fail_send_array_t* const aFail) const;


	void MFillMsgHandlersFor(user_datas_t & aFrom,user_datas_t &aTo,fail_send_array_t & aError) const;


	/* !< \brief get uuids that satisfy the
	 * Identification name
	 *
	 * 	\param aName Identification name
	 *
	 * 	\return Uuids
	 *
	 */
	unique_uuids_t  MGetUUIDFor(NSHARE::CRegistration const& aName) const;
	demand_dgs_for_t const& MGetDemands() const;

	/*!< \brief Make valid the message's byte order
	 *
	 *	\param aData Message
	 *
	 *	\return 0 - no error
	 *	\note Make valid only body of message that is
	 *		  The message header  stay not valid.
	 */
	static unsigned sMSwapEndian( user_data_t& aData);
	NSHARE::CConfig MSerialize() const;
private:

	void MFillRouteAndDestanationInfo(uuids_of_receiver_t const& aRoute,user_data_info_t* const aInfo,fail_send_array_t*const aFail) const;

	inline void MFillByUserProtocol(user_datas_t*const aFrom,user_datas_t* const aTo,
			fail_send_array_t * const aFail) const;
	inline void MFillByRawProtocol(user_datas_t& aTo,user_datas_t& aFailed,
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
			required_header_t const& aType, NSHARE::CBuffer* aTO);
	msg_handlers_t const* MGetHandlers(user_data_info_t & aFrom) const;

	inline void MInitializeMsgInheritance();//todo


	mutable protocol_of_uuid_t FWhatIsSendingBy;
	demand_dgs_for_t FDGs;
	std::set<id_t> FIds;
	mutable uint16_t FMsgID;
};
inline bool CRequiredDG::msg_handlers_t::MIsRegistrarExist() const
{
	return !empty() && FNumberOfRealHandlers!=size();
}

} /* namespace NUDT */
#endif /* CREQUIREDDG_H_ */
