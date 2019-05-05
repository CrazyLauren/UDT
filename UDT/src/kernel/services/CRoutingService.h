/*
 * CRoutingService.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 16.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CROUTINGSERVICE_H_
#define CROUTINGSERVICE_H_

#include <core/ICore.h>
#include "CRequiredDG.h"
namespace NUDT
{
class CDataObject;
struct args_data_t;
class CHardWorker;
class CRoutingService: public NSHARE::CSingleton<CRoutingService>, public ICore
{
public:
	static const NSHARE::CText NAME;
	CRoutingService();
	~CRoutingService();
	bool MStart();

	NSHARE::CConfig MSerialize() const;

	void MNoteFailSend(const fail_send_t&);
	void MNoteFailSend(const fail_send_array_t&);

	/**\brief see CRequiredDG::MFillMsgReceivers
	 *
	 */
	void MFillMsgReceivers(user_datas_t & aFrom,user_datas_t& aTo,fail_send_array_t&);

	/**\brief see CRequiredDG::MFillMsgHandlersFor
	 *
	 */
	void MFillMsgHandlersFor(user_datas_t & aFrom,user_datas_t& aTo,fail_send_array_t & aError);

	/**\brief Send data to uuids
	 *
	*\param [in] aFrom uuids to which the data must sent.
	 *	The argument can be changed (for optimization).
	 *
	*\param [out] aTo uuids cannot to which the data cannot be sent
	 *
	*\param [in] aVal message
	 *
	 */
	template<class T>
	void MSendTo(routing_t & aFrom,routing_t & aTo, const T & aVal);

	/**\brief Send data further
	 *
	*\param [in] aFrom uuids to which the data must sent.
	 *	The argument can be changed (for optimization).
	 *
	*\param [out] aTo uuids cannot to which the message cannot be sent
	 *
	*\param [in] aVal message
	 *
	 */
	template<class T>
	void MRoute(routing_t& aFrom,routing_t& aTo, const T& aDemands);
private:
	struct _route_t
	{
		CRequiredDG FRequiredDG;
	};


	typedef NSHARE::CSafeData<_route_t> route_data_t;
	typedef route_data_t::RAccess<> const r_route_access;
	typedef route_data_t::WAccess<> w_route_access;
	typedef CKernelIo::output_decriptors_for_t output_decriptors_for_t;

	void MInit();

	static int sMHandleDemandId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	static int sMHandleUserDataId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	static int sMHandleDiff(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);

	static int sMHandleDemands(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);

//	static int sMHandleOpenId(CHardWorker* WHO, args_data_t* WHAT,
//			void* YOU_DATA);
//	static int sMHandleCloseId(CHardWorker* WHO, args_data_t* WHAT,
//			void* YOU_DATA);

	void MHandleDiff(kernel_infos_diff_t const*);
	void MHandleFrom(demand_dgs_t const*, descriptor_t aFrom);

	//void MHandleNewId(descriptor_t aFrom,descriptor_info_t const&_info);
	void MHandleNewId(bool aIs, program_id_t const&,
			kern_links_t const& _vertex);
	void MHandleCloseId(bool aIs, program_id_t const&,
			kern_links_t const& _vertex);
	void MHandleFrom(routing_user_data_t& aData);

	void MHandleNewDemands( descriptor_t aFrom, const demand_dgs_for_t&);

	void MGetOutputDescriptors(routing_t& aFrom, routing_t& aTo,
			output_decriptors_for_t& _descr) const;
	void MInformNewReceiver(demand_dgs_for_t &);
	void MClassifyMessages(routing_user_data_t& aData,
			user_datas_t& aCustomers, user_datas_t& aKernels) const;
	void MDistributeMsgByDescriptors(user_datas_t& aHasRoute,
			user_datas_t& aHasNotRoute,
			output_user_data_t& _route_by_desc, fail_send_array_t& _errors_list) const;
	inline void MRoutingMsgToDescriptor(user_data_t const & aMsg,output_decriptors_for_t const& aDesc, output_user_data_t *const aTo) const;
	inline void MSortedSplice(output_user_data_t& aWhat,output_user_data_t& aTo) const;
	inline void MExtractMsgThatHasToBeSwaped(user_datas_t & aFrom,user_datas_t & aTo) const;
	inline void MSwapEndianIfNeed(user_datas_t & aFrom,user_datas_t& aTo,fail_send_array_t& _errors_list) const;
	void MSendMessages(user_datas_t& aFrom, user_datas_t& aTo,
			fail_send_array_t& _errors_list);

	route_data_t FRouteData;
};
template<class T>
inline void CRoutingService::MSendTo(routing_t& aTo,routing_t & aNotSend, const T & aWhat)
{
	VLOG(2)<<"Routing to "<<aTo;
	output_decriptors_for_t _descr;
	MGetOutputDescriptors(aTo, aNotSend, _descr);

	LOG_IF(ERROR, _descr.empty()) << "There are not descriptors. WTF?";

	VLOG_IF(1, _descr.size() < aTo.size())
			<< "It has been reduced the number of packets for "
			<< (_descr.size()) << " from " << aTo.size();

	if (!_descr.empty())
	{
		for (output_decriptors_for_t::const_iterator _it =
				_descr.begin(); _it != _descr.end(); ++_it)
		{
			VLOG(2)<<"Sent to "<<_it->second<<" by "<<_it->first;
			if (!CKernelIo::sMGetInstance().MSendTo(_it->first, aWhat,_it->second))
			{
				LOG(WARNING)<<"Cannot send  packet "<<aWhat<<" to " << _it->first;
				aNotSend.insert(aNotSend.end(), _it->second.begin(),
						_it->second.end());
				CHECK(!aNotSend.empty());
			}
		}
	}
}
template<class T>
inline void CRoutingService::MRoute(routing_t& aRoute,routing_t& aTo, const T& aWhat)
{
	VLOG_IF(1,aRoute.empty()) << "No routing";
	if (!aRoute.empty())
	{
		VLOG(2) << "Route " << aRoute;
		//Loop check, maybe there is my uuid
		for (routing_t::iterator _it = aRoute.begin(); _it != aRoute.end(); ++_it)
		{
			if (*_it == get_my_id().FId.FUuid)
			{
				aRoute.erase(_it);
				break;
			}
		}
		LOG_IF(INFO,aRoute.empty()) << "Finish sending ";

		if (!aRoute.empty())
		{
			MSendTo(aRoute,aTo, aWhat);
			LOG_IF(ERROR,!aTo.empty()) << "Cannot sent " << aWhat
														<< " to " << aTo;
			LOG_IF(INFO,aTo.empty()) << "Sent " << aWhat << " to "
													<< aRoute;
		}
	}
}
} /* namespace NUDT */
#endif /* CROUTINGSERVICE_H_ */
