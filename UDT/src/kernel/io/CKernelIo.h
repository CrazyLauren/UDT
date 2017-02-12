/*
 * CKernelIo.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 29.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CKERNELIO_H_
#define CKERNELIO_H_

#include "IIOManager.h"
#include "CKernelIo.h"
#include "CBuffering.h"
namespace NUDT
{
//todo remove IOManager
//Now supposing that the IOManger will not removed
//make IOmanger using smart pointer
class CKernelIo: public NSHARE::CFactoryManager<IIOManager>,public IState
{
public:
	static CKernelIo& sMGetInstance();
	static CKernelIo* sMGetInstancePtr();
	static const NSHARE::CText BUFFERS;
	static const NSHARE::CText DEFAULT;
	static const NSHARE::CText NAME;
	typedef std::map<descriptor_t, routing_t> output_decriptors_for_t;

	CKernelIo();
	virtual ~CKernelIo();
	void MInit();

//	template<class T>
//	bool MSendTo(NSHARE::uuid_t const&, const T &);

//	bool MSendTo(descriptor_t const&, const NSHARE::CBuffer& aVal); //
//	bool MSendTo(descriptor_t const&, const NSHARE::CBuffer& aVal,routing_t const& aTo); //

	template<class T>
	bool MSendTo(descriptor_t const&, const T &); //

	template<class T>
	bool MSendTo(descriptor_t const& aBy, const T &,routing_t const& aTo); //

	template<class T>
	bool MSendTo(descriptor_t const& aBy, const T & aVal, routing_t const& aTo,
			error_info_t const& aError);

	void MSendTo(output_user_data_t& aBy,
			fail_send_array_t & _non_sent,user_datas_t& aFailedData); //warning! All arguments can be changed(optimization)
	fail_send_t::eError MSendUserData(descriptor_t const& _by, user_datas_t& _data);
	//void MSendTo(output_user_data_t& aBy, uuids_t & _non_sent); //warning! All arguments can be changed(optimization)


	void MReceivedData(program_id_t const& aWhat, const descriptor_t& aFrom, const routing_t& aRoute,error_info_t const& aError);
	void MReceivedData(user_data_t const& aWhat, const descriptor_t& aFrom);
	void MReceivedData(demand_dgs_t const& aWhat, const descriptor_t& aFrom, const routing_t& aRoute,error_info_t const& aError);
	void MReceivedData(demand_dgs_for_t const& aWhat, const descriptor_t& aFrom, const routing_t& aRoute,error_info_t const& aError);
	void MReceivedData(kernel_infos_array_t const& aWhat, const descriptor_t& aFrom, const routing_t& aRoute,error_info_t const& aError);
	void MReceivedData(fail_send_t const& aWhat, const descriptor_t& aFrom, const routing_t& aRoute,error_info_t const& aError);


	void MAddChannelFor(descriptor_t const&, IIOManager* aWhere,
			split_info const&);
	void MRemoveChannelFor(descriptor_t const&, IIOManager* aWhere);
	void MClose(descriptor_t const&);
	NSHARE::CConfig MSerialize() const;
protected:
	virtual void MFactoryAdded(factory_t* factory);
	virtual void MFactoryRemoved(factory_t* factory);
private:
	struct routing_op_t
	{
		descriptor_t FDesc;
		routing_t FTo;
		void* FData;
		void (CKernelIo::*FOperation)(void*,const routing_t&,descriptor_t const&);
	};
	struct manager_t:NSHARE::CIntrusived
	{
		manager_t(IIOManager& aWho,CBuffering const& aBuffering) :
				FWho(aWho), //
				FDataBuffer(aBuffering) //
		{

		}
		IIOManager& FWho;
		mutable CBuffering FDataBuffer;
		mutable NSHARE::CMutex FBufMutex;
	};

	typedef NSHARE::CSafeQueue<routing_op_t> routings_t;
	typedef std::map<descriptor_t, NSHARE::intrusive_ptr<manager_t> > managers_t;
	typedef NSHARE::CSafeData<managers_t> safe_manager_t;
	bool MIsForMe(const routing_t& aRoute) const;

	template<class T>
	void MRouteOperation(void*,const routing_t& aTo,descriptor_t const&);
	static int sMRouteOperation(const NSHARE::CThread* WHO,
			NSHARE::operation_t* WHAT, void* YOU_DATA);
	void MRoute();
	template<class T>
	void MRouting(const routing_t& aRoute, const T& aWhat,descriptor_t const& aFrom);

	template<class T>
	void MRecvImpl(const routing_t& aRoute, const descriptor_t& aFrom,
			const T& aWhat,error_info_t const& aError);

	static int sMSendUser(NSHARE::CThread const* WHO, NSHARE::operation_t * WHAT,	void* YOU_DATA);
	void MSendUserDataImpl();

	std::pair<descriptor_t,IIOManager*> MBeginSend(CBuffering::data_list_t& _data);
	bool MNextSend(descriptor_t aDesc,CBuffering::data_list_t& _data);

	NSHARE::CConfig  MBufferingConfFor(descriptor_t const& aName, IIOManager* aWhere);

	void MSendingUserDataTo(descriptor_t aTo, IIOManager* aBy,
			CBuffering::data_list_t& _data,
			fail_send_array_t& _non_sent) const;
	fail_send_t::eError MPutUserDataToSendFifo(descriptor_t const& _by, user_datas_t& _data);
	void MRemoveManagerFor(const descriptor_t& aVal,
			CBuffering::data_list_t& _not_sent_data);


	bool FIsInited;
	safe_manager_t FIoManagers;
	//mutable NSHARE::CMutex FBlockChangingIOMangersList;
	mutable NSHARE::CMutex FMutexOperation;
	routings_t FRoutings;
	std::set<descriptor_t> FOperartionSendFor;
};
template<class T>
inline bool CKernelIo:: MSendTo(descriptor_t const& aBy, const T & aVal, routing_t const& aTo,
		error_info_t const& aError)
{
	if (!CDescriptors::sMIsValid(aBy))
		return false;
	//NSHARE::CRAII<NSHARE::CMutex> _lock_io(FBlockChangingIOMangersList);
	IIOManager* _p = NULL;
	{
		safe_manager_t::RAccess<> const _access = FIoManagers.MGetRAccess();
		//NSHARE::CRAII<NSHARE::CMutex> _block(FBlocked); //
		managers_t const& _man = _access.MGet();
		managers_t::const_iterator _it = _man.find(aBy);
		if (_it == _man.end())
		{
			LOG(ERROR)<<"There is not manager for "<<aBy;
			return false;
		}
		_p=&_it->second->FWho;
	}
	return  _p != NULL &&  _p->MSend(aVal, aBy, aTo, aError);
}


template<class T>
inline bool CKernelIo::MSendTo(descriptor_t const& aBy, const T &aVal,
		routing_t const& aTo)
{
	return MSendTo(aBy, aVal, aTo, error_info_t());
}
template<class T>
inline bool CKernelIo::MSendTo(descriptor_t const& aVal, const T &aWhat)
{
	return MSendTo(aVal,aWhat,routing_t());
}
//template<class T>
//inline bool CKernelIo::MSendTo(NSHARE::uuid_t const& aVal, const T & aWhat)
//{
//	return MSendTo(CDescriptors::sMGetInstance().MGet(aVal), aWhat);
//}
inline void CKernelIo::MClose(descriptor_t const& aVal)
{
	if (!CDescriptors::sMIsValid(aVal))
		return;
	//NSHARE::CRAII<NSHARE::CMutex> _lock_io(FBlockChangingIOMangersList);
	IIOManager* _p = NULL;
	{
		safe_manager_t::RAccess<> const _access = FIoManagers.MGetRAccess();
		managers_t const& _man=_access.MGet();
		managers_t::const_iterator _it = _man.find(aVal);
		if (_it == _man.end())
		{
			LOG(ERROR)<<"There is not manager for "<<aVal;
			return;
		}
		_p = &_it->second.MGet()->FWho;
	}
	//if (_p)
	CHECK_NOTNULL(_p);
	_p->MClose(aVal);
}
} /* namespace NUDT */
#endif /* CKERNELIO_H_ */
