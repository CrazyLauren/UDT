/*
 * CInfoService.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 19.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CINFOSERVICE_H_
#define CINFOSERVICE_H_
#include "CGraph.h"
#include <core/ICore.h>
namespace NUDT
{
class CHardWorker;
class CInfoService:public NSHARE::CSingleton<CInfoService>,public ICore
{
public:
	static const NSHARE::CText NAME;
	CInfoService();
	~CInfoService();
	NSHARE::CConfig MSerialize() const;
	bool MStart();

	std::vector<descriptor_t> MNextDestinations(CRouteGraph::node_t const&  aTo) const;
	bool MIsVertex(const CRouteGraph::node_t& name) const;
	uuids_t MGetOtherKernelds() const;

private:
	typedef std::map<NSHARE::uuid_t, std::set<descriptor_t> > uuid_from_t;

	typedef std::map<CRouteGraph::node_t, std::vector<descriptor_t> > next_destination_t;

	struct _data_info_t
	{
		kernel_infos_array_t FNet;
		uuid_from_t FUUIDFrom;
		CRouteGraph FGraph;
		next_destination_t FDestianationCache;
		descriptors_t FAllDescriptors;
		void MResetCache();
	};
	typedef NSHARE::CSafeData<_data_info_t> info_data_t;
	typedef const info_data_t::RAccess<> r_access;
	typedef info_data_t::WAccess<> w_access;//don't forgot reset cache
	//typedef std::list<std::pair<descriptor_t,kernel_infos_array_t const*> > new_infos_t;
	void MInit();
	void MHandleOpen(const descriptor_t& aFrom, const descriptor_info_t&);
	void MHandleClose(const descriptor_t& aFrom, const descriptor_info_t&);
	void MHandle(const kernel_infos_array_t& aFrom, descriptor_t const& aId);
	static int sMHandleOpenId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	static int sMHandleCloseId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	static int sMHandleKernelInfo(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	bool MAddOrUpdateClientTo(kernel_infos_t& aTo,
			const kernel_link& aCustomer);
	//bool MDeprecatedRemoveCustomer(const kernel_link& _what, _data_info_t& _d_info);
	bool MIsCustomer(const kernel_link& _what, _data_info_t& _d_info) const;


	void MDebugPrintState() const;
	void MSendNet(const descriptors_t& _sent_to);
	void MChangeInform(const kernel_infos_diff_t& _removed_info);
	bool MRemoveKernel(const kernel_link& _what, const descriptor_t& aFrom,
			_data_info_t& _d_info, kernel_infos_diff_t& _diff);
	bool MAddKernel(const kernel_link& _what, const descriptor_t& aFrom,
			_data_info_t& _d_info, kernel_infos_diff_t& _diff);

	void MRebuildGraph(_data_info_t& _new_array);
	void MSynchronize(const descriptors_t& _sent_to,
			const kernel_infos_diff_t& _diff);
	bool MSynchronizeNetInfo(_data_info_t& _d_info,
			const kernel_infos_array_t& _diff, descriptor_t const& aFrom,
			kernel_infos_diff_t& diff);
	void MGetCustomerDiff(kernel_infos_t const&,
			kernel_infos_t const&, k_diff_t& aRemoved,
		k_diff_t& aAdded);
	void MPutKernel(kernel_infos_t const& _it_new,
			const descriptor_t& aFrom,
			k_diff_t& aAdded, uuid_from_t& _from,
			kernel_infos_array_t& aTo);
	void MPopKernel(
			kernel_infos_t const& _it_old, const descriptor_t& aFrom,
			k_diff_t& aRemoved, uuid_from_t& _from,
			kernel_infos_array_t& aTo);
	void MPutConsumerToMyInfo(const descriptor_t& aFrom,
			const kernel_link& _new, _data_info_t& _d_info, k_diff_t& aAdded);
	void MPopConsumerFromMyInfo(const descriptor_t& aFrom,const kernel_link& _new, _data_info_t& _d_info,
			k_diff_t& aRemoved);

	mutable info_data_t FData;
	NSHARE::CMutex FUpdateMutex;

};

} /* namespace NUDT */
#endif /* CINFOSERVICE_H_ */
