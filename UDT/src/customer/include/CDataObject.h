/*
 * CDataObject.h
 *
 *  Created on: 03.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CDATAOBJECT_H_
#define CDATAOBJECT_H_
#include "customer_export.h"
#include <programm_id.h>
#include <udt_types.h>
#include <shared_types.h>
#include <CHardWorker.h>
namespace NUDT
{
struct CUSTOMER_EXPORT recv_data_from_t
{
	static const data_events_t::key_t NAME;
	user_data_t FData;
};
struct CUSTOMER_EXPORT progs_id_from_t
{
	static const data_events_t::key_t NAME;
	progs_id_t FData;
};
struct CUSTOMER_EXPORT req_recv_t
{
	demand_dgs_t FDemand;
	static const data_events_t::key_t NAME;
};
struct CUSTOMER_EXPORT fail_send_id_t
{
	static const data_events_t::key_t NAME;
	fail_send_t FData;
};
struct CUSTOMER_EXPORT fail_send_id_from_me_t
{
	static const data_events_t::key_t NAME;
	fail_send_t FData;
};
struct CUSTOMER_EXPORT demand_dgs_id_t
{
	static const data_events_t::key_t NAME;
	demand_dgs_t FData;
};
/** Object of this class is used for communication
 * inner library.
 *
 * One of plugins push the data that has to be handled by the other
 * plugin. The other plugin subscribes to receive the data.
 *
 */
class CUSTOMER_EXPORT CDataObject: public CHardWorker,public NSHARE::CSingleton<CDataObject>
{
public:
	CDataObject();
	//The resources can be leaked in destructor
	virtual ~CDataObject();

	//T has to be movable type. I.e.
	//The memory of T can be moved without calling
	//coping constuctor.
	void MPush(const progs_id_from_t &);
	void MPush(const recv_data_from_t &);
	void MPush(const req_recv_t &);
	void MPush(const fail_send_id_t &);
	void MPush(const fail_send_id_from_me_t &);
	void MPush(const demand_dgs_t &);
	template<class T>
	inline void MPush(const T& aVal,bool isPrior=false);

	template<class T>
	bool MGetLast(T&) const;

private:

	static NSHARE::eCBRval sMUserDataOperation(NSHARE::CThread const* WHO,
			NSHARE::operation_t* WHAT, void* YOU_DATA);
	void MUserOperation(NSHARE::CThread const* WHO, NSHARE::operation_t* WHAT,
			NSHARE::CText const &aWhat);
	template<class T>
	inline void MPushImpl(const T& aVal, bool isPrior);

	mutable NSHARE::CMutex FUserDataMutex;
	std::list<recv_data_from_t> FUserDataFIFO;
};
template<class T>
inline bool CDataObject::MGetLast(T & aVal) const
{
	return MGet(T::NAME, aVal);
}
template<class T>
inline void CDataObject::MPush(const T& aVal, bool isPrior)
{
	return MPushImpl(aVal, isPrior);
}
template<class T>
inline void CDataObject::MPushImpl(const T& aVal, bool isPrior)
{
	CHardWorker::MPushData(T::NAME, aVal, isPrior);
}
}
/* namespace NUDT */
#endif /* CDATAOBJECT_H_ */
