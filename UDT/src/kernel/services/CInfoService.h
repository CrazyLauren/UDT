/*
 * CInfoService.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 19.04.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CINFOSERVICE_H_
#define CINFOSERVICE_H_
#include "CGraph.h"
namespace NUDT
{
class CHardWorker;
class CInfoService:public NSHARE::CSingleton<CInfoService>,public IState
{
public:
	static const NSHARE::CText NAME;
	CInfoService();
	~CInfoService();
	NSHARE::CConfig MSerialize() const;

	CRouteGraph::path_t MShortestPath(CRouteGraph::node_t const&  aTo) const;
	bool MIsVertex(const CRouteGraph::node_t& name) const;
	uuids_t MGetOtherKernelds() const;

private:
	struct k_info_t
	{
		k_info_t()
		{
			FPriority = 0;
		}

		k_info_t(const kernel_infos_array_t& aInfo, unsigned aPrior) :
				FInfo(aInfo), //
				FPriority(aPrior) //
		{
		}

		kernel_infos_array_t FInfo;
		unsigned FPriority;
	};
	struct k_counter
	{
		explicit k_counter() :
				FCount(1)
		{
		}

		unsigned FCount;
	};
	typedef std::map<descriptor_t, k_info_t> k_info_by_descriptor_t;
	typedef std::map<id_t, k_counter> kernels_counter_t; //the identical kernel can be available by several kernels
	struct _data_info_t
	{
		kernel_infos_array_t FInfo;
		k_info_by_descriptor_t FInfoForDesc;
		kernels_counter_t FWayCounter;
		CRouteGraph FGraph;
	};
	typedef NSHARE::CSafeData<_data_info_t> info_data_t;
	typedef const info_data_t::RAccess<> r_access;
	typedef info_data_t::WAccess<> w_access;
	//typedef std::list<std::pair<descriptor_t,kernel_infos_array_t const*> > new_infos_t;
	void MInit();
	void MSynchronize2(descriptors_t& aNeedSent, _data_info_t&, bool aIsChange);
	void MHandleOpen(const descriptor_t& aFrom, const descriptor_info_t&);
	void MHandleClose(const descriptor_t& aFrom, const descriptor_info_t&);
	void MHandle(const kernel_infos_array_t& aFrom, descriptor_t aId);
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
	bool MRebuild(_data_info_t& _d_info, kernel_infos_diff_t& _diff);
	bool MAddNewKernel(const kernel_link& _new, _data_info_t& _d_info,
			kernel_infos_diff_t& _diff);
	bool MEraseKernel2(const kernel_link& _info, const descriptor_t& aFrom,
			_data_info_t& _d_info);
	void MAddingNewKernelTo(kernel_infos_t& _new, _data_info_t& _new_array,
			const kernel_infos_array_t& _looking_for_in);
	bool MSetDiff(const kernel_infos_array_t& _old,
			const kernel_infos_array_t& _new_array, k_diff_t& aRemoved,
			k_diff_t& aAdded);
	void MDebugPrintState() const;
	void MSincAll(bool aIsChange, _data_info_t& _d_info,
			descriptors_t& _sent_to);
	void MSendInfo(const descriptors_t& _sent_to);
	void MChangeInform(const kernel_infos_diff_t& _removed_info);
	//	void MRemoveCustomerOptimizing(const kernel_link& _what, kernel_infos_diff_t& _diff,
	//			_data_info_t& _d_info);
	bool MAddCustomerOptimizing(const kernel_link& _what,
			kernel_infos_diff_t& _diff);
	bool MUpdateMyInfo(_data_info_t& _d_info);
	bool MRemoveCustomer(const kernel_link& _what, _data_info_t& _d_info,
			kernel_infos_diff_t& _diff);
	bool MRemoveKernelImpl(const kernel_link& _what, const descriptor_t& aFrom,
			_data_info_t& _d_info, kernel_infos_diff_t& _diff);

	void MAddNewKernelTo(const kernel_link& _info, _data_info_t& _new_array,
			const kernel_infos_array_t& aKernelList);
	void MRebuildGraph(_data_info_t& _new_array);
	void MRebuldKernels(const kern_links_t& _kinfo,
			const kernel_infos_array_t& _actual_array,
			_data_info_t& _new_array);
	void MGetActualInfo(_data_info_t& _new_array,
			kernel_infos_array_t& _actual_array);

	unsigned FNumberOfChange;
	unsigned FInfoPriority;
	info_data_t FData;
	NSHARE::CMutex FUpdateMutex;

};

} /* namespace NUDT */
#endif /* CINFOSERVICE_H_ */
