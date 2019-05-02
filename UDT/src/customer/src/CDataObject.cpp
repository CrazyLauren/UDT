// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CDataObject.cpp
 *
 *  Created on: 03.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <CHardWorker.h>
#include "CDataObject.h"
#include "CCustomer.h"
#include "CConfigure.h"
using namespace NSHARE;
template<>
NUDT::CDataObject::singleton_pnt_t  NUDT::CDataObject::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText progs_id_from_t::NAME = "ProgsIdFrom";
const NSHARE::CText recv_data_from_t::NAME = "ReceiveFrom";
const NSHARE::CText req_recv_t::NAME = "ListOfFilters";
const NSHARE::CText fail_send_id_t::NAME = "FailSent";
const NSHARE::CText fail_send_id_from_me_t::NAME = "FailSent2";
const NSHARE::CText demand_dgs_id_t::NAME = "Demands";

static int number_of_thread()
{
	int _num = 1;
	CConfigure::sMGetInstance().MGet().MGetIfSet(CCustomer::NUMBER_OF_THREAD,
			_num);
	return _num;
}
CDataObject::CDataObject() :CHardWorker(number_of_thread())
{
}

CDataObject::~CDataObject()
{
}
NSHARE::eCBRval CDataObject::sMUserDataOperation(const NSHARE::CThread* WHO,
		NSHARE::operation_t* WHAT, void* YOU_DATA)
{
	VLOG(2) << "User data Operation ";
	data_for* _p = reinterpret_cast<data_for*>(YOU_DATA);
	CHECK_NOTNULL(_p);
	static_cast<CDataObject*>(_p->FThis)->MUserOperation(WHO, WHAT, _p->FWhat);
	delete _p;
	return NSHARE::E_CB_REMOVE;
}
void CDataObject::MUserOperation(const NSHARE::CThread* WHO,
		NSHARE::operation_t* WHAT, const NSHARE::CText& aWhat)
{
	VLOG(2) << "Operation for user data " << aWhat;
	CHECK_EQ(recv_data_from_t::NAME, aWhat);
	args_data_t _args;
	_args.FType = aWhat;
	recv_data_from_t _data;
	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FUserDataMutex);
		CHECK(!FUserDataFIFO.empty());
		_data = FUserDataFIFO.front();
		FUserDataFIFO.pop_front();
	}

	_args.FPointToData = &_data;
	VLOG(1) << "Call";
	int _count = MCall(aWhat, &_args);
	VLOG(1) << "EOK:" << _count;
}

void CDataObject::MPush(const demand_dgs_t & aVal)
{
	MPushImpl(aVal, true);
}

void CDataObject::MPush(const fail_send_id_from_me_t & aVal)
{
	MPushImpl(aVal, true);
}
void CDataObject::MPush(const fail_send_id_t & aVal)
{
	MPushImpl(aVal, true);
}
void CDataObject::MPush(const progs_id_from_t & aVal)
{
	MPushImpl(aVal,false);
}
void CDataObject::MPush(const recv_data_from_t & aVal)
{
	VLOG(2) << "USER DATA Push " << recv_data_from_t::NAME << " data size = "
						<< aVal.FData.FData.size();
	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FUserDataMutex);
		FUserDataFIFO.push_back(aVal);
	}
	MNewDataFor(recv_data_from_t::NAME, sMUserDataOperation);
}
void CDataObject::MPush(const req_recv_t & aVal)
{
	CHardWorker::MPushData(req_recv_t::NAME, aVal, false,true);
}
} /* namespace NUDT */
