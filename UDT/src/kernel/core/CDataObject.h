/*
 * CDataObject.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CDATAOBJECT_H_
#define CDATAOBJECT_H_
#include <UDT/programm_id.h>
#include <UDT/udt_types.h>
#include <CHardWorker.h>
#include "ICore.h"
#include "kernel_type.h"
namespace NUDT
{
class CDataObject: public CHardWorker, public NSHARE::CSingleton<CDataObject>,public ICore
{
public:
	NSHARE::IAllocater* MDefAllocater() const;
	NSHARE::IAllocater* MSetDefAllocater(NSHARE::IAllocater*);
	static const NSHARE::CText NAME;
	CDataObject();
	//The resources can be leaked in destructor
	virtual ~CDataObject();

	bool MStart();
	void MStop();
	//T has to be movable type. I.e.
	//The memory of T can be moved without calling
	//coping constuctor.
	void MPush(const create_descriptor &);
	void MPush(const destroy_descriptor &);

	void MPush(const open_descriptor &);
	void MPush(const close_descriptor &);
	void MPush(const new_id_t &);
	void MPush(routing_user_data_t &);//moved
	void MPush(const demands_id_t &);

	void MPush(const kernel_infos_array_id_t &);
	void MPush(const demand_dgs_for_by_id_t&);
	void MPush(const kernel_infos_diff_t&);//warning running in current thread
	void MPush(const fail_send_by_id_t&);






	template<class T>
	inline void MPush(const T& aVal,bool isPrior);

	/** Push data for specified receiver
	 *
	 * @param aVal A what you want to send
	 * @param aId An id of receive
	 * @param isPrior priority
	 */
	template<class T>
	inline void MPush(const data_to_id<T>& aVal, bool isPrior);

	template<class T>
	bool MGetLast(T&) const;

	NSHARE::CConfig MSerialize() const;
private:
//	struct sequence_t
//	{
//		sequence_t() :
//				FMutex(new NSHARE::CMutex),FOrdinalNumber(0),FNextHandler(0)
//		{
//
//		}
//		SHARED_PTR<NSHARE::CMutex> FMutex;
//		uint32_t FOrdinalNumber;//
//		uint32_t FNextHandler;
//	};
//	typedef std::map<descriptor_t,sequence_t> sequence_protect_t;
	struct user_operation_t
	{
		user_operation_t(CDataObject& aThis) :
				FThis(aThis),FIsWorking(false)
		{

		}

		void MPush(routing_user_data_t & aVal);
		static NSHARE::eCBRval sMUserDataOperation(NSHARE::CThread const* WHO,
				NSHARE::operation_t* WHAT, void* YOU_DATA);
		NSHARE::eCBRval MUserOperation(NSHARE::CThread const* WHO, NSHARE::operation_t* WHAT,
				NSHARE::CText const &aWhat);

		routing_user_data_t FUserData;
		CDataObject& FThis;
		bool FIsWorking;
	};

	template<class T>
	inline void MPushImpl(const T& aVal, bool isPrior);

	mutable NSHARE::CMutex FFifoMutex;
//	mutable NSHARE::CMutex FSeqMutex;
	std::map<descriptor_t,user_operation_t> FUserDataFIFO;
//	sequence_protect_t FHandlingSequence;
	//std::list<user_data_id_t> FUserDataFIFO;
	NSHARE::IAllocater* FAllocater;
};
template<class T>
inline void CDataObject::MPush(const T& aVal, bool isPrior)
{
	return MPushImpl(aVal, isPrior);
}
template<class T>
inline void CDataObject::MPush(const data_to_id<T>& aVal, bool isPrior)
{
	CHardWorker::MPushData(aVal.MName(), aVal, isPrior);
}
template<class T>
inline bool CDataObject::MGetLast(T & aVal) const
{
	return MGet(T::NAME, aVal);
}
template<class T>
inline void CDataObject::MPushImpl(const T& aVal, bool isPrior)
{
	CHardWorker::MPushData(T::NAME, aVal, isPrior);
}

}
/* namespace NUDT */
#endif /* CDATAOBJECT_H_ */
