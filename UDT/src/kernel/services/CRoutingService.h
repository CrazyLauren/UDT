/*
 * CRoutingService.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 16.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CROUTINGSERVICE_H_
#define CROUTINGSERVICE_H_

#include "CRequiredDG.h"
namespace NUDT
{
class CDataObject;
struct args_data_t;
class CHardWorker;
class CRoutingService: public NSHARE::CSingleton<CRoutingService>, public IState
{
public:
	static const NSHARE::CText NAME;
	CRoutingService();
	~CRoutingService();
	NSHARE::CConfig MSerialize() const;

	void MNoteFailSend(const user_data_info_t& aP, const uuids_t& _non_sent);
	void MNoteFailSend(const fail_send_t&);
	CRequiredDG::req_uuids_t MGetCustomersFor(user_data_t const&);

	template<class T>
	uuids_t MSendTo(routing_t const&, const T &);
	template<class T>
	uuids_t MRoute(const routing_t& aRoute, const T& aDemands);
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
	void MHandleNewId(bool aIs, programm_id_t const&,
			kern_links_t const& _vertex);
	void MHandleCloseId(bool aIs, programm_id_t const&,
			kern_links_t const& _vertex);
	void MHandleFrom(user_data_t const*, descriptor_t aFrom);

	void MHandleNewDemands( descriptor_t aFrom, const demand_dgs_for_t&);

	void MSendTo(user_data_t const* aP, routing_t const& aTo, fail_send_array_t & aNonSent,
			descriptor_t aFrom);
	descriptor_t MNextDestinationNode(NSHARE::uuid_t const& aUUID) const;
	void MGetOutputDescriptors(const routing_t& aSendTo,
			output_decriptors_for_t& _descr, uuids_t& _non_sent) const;
	void MInformNewReceiver(demand_dgs_for_t &);

	route_data_t FRouteData;
};
template<class T>
inline uuids_t CRoutingService::MSendTo(routing_t const& aTo, const T & aWhat)
{
	uuids_t _non_sent;
	output_decriptors_for_t _descr;
	MGetOutputDescriptors(aTo, _descr, _non_sent);

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
				_non_sent.insert(_non_sent.end(), _it->second.begin(),
						_it->second.end());
				CHECK(!_non_sent.empty());
			}
		}
	}
	return _non_sent;

}
template<class T>
inline uuids_t CRoutingService::MRoute(const routing_t& aRoute, const T& aWhat)
{
	VLOG_IF(1,aRoute.empty()) << "No routing";
	if (!aRoute.empty())
	{
		VLOG(2) << "Route " << aRoute;
		routing_t _filtered(aRoute); //Loop check, maybe there is my uuid
		routing_t::iterator _it = _filtered.begin(), _it_end = _filtered.end();
		for (; _it != _it_end; ++_it)
		{
			if (*_it == get_my_id().FId.FUuid)
			{
				_filtered.erase(_it);
				break;
			}
		}
		LOG_IF(INFO,_filtered.empty()) << "Finish sending ";

		if (!_filtered.empty())
		{
			uuids_t _not_sent(MSendTo(_filtered, aWhat));
			LOG_IF(ERROR,!_not_sent.empty()) << "Cannot sent " << aWhat
														<< " to " << _not_sent;
			LOG_IF(INFO,_not_sent.empty()) << "Sent " << aWhat << " to "
													<< _filtered;
			return _not_sent;
		}
	}
	return aRoute;
}
} /* namespace NUDT */
#endif /* CROUTINGSERVICE_H_ */
