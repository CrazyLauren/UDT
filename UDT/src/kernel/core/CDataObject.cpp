/*
 * CDataObject.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 03.02.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include "kernel_type.h"
#include "IState.h"
#include "CDataObject.h"
#include "CConfigure.h"

#include <boost/version.hpp>
#include <boost/interprocess/detail/atomic.hpp>

#if (BOOST_VERSION / 100000 >=1) &&(BOOST_VERSION / 100 % 1000<=47)
using namespace boost::interprocess::detail;
#else
using namespace boost::interprocess::ipcdetail;
#endif

using namespace NSHARE;
template<>
NUDT::CDataObject::singleton_pnt_t NUDT::CDataObject::singleton_t::sFSingleton =
		NULL;
namespace NUDT
{
const NSHARE::CText CDataObject::NAME="hw";
CDataObject::CDataObject():IState(NAME),FAllocater(NULL)
{
	NSHARE::CThread::eThreadPriority _priority;
	if (CConfigure::sMGetInstance().MGet().MGetIfSet("priority", _priority))
		MCreate( _priority,-1);
	else
		MCreate(-1);
}

CDataObject::~CDataObject()
{
}


void CDataObject::MPush(const destroy_descriptor & aVal)
{
	MPushImpl(aVal, true);
}

void CDataObject::MPush(const create_descriptor & aVal)
{
	MPushImpl(aVal, true);
}
void CDataObject::MPush(const open_descriptor & aVal)
{
	MPushImpl(aVal, true);
}
void CDataObject::MPush(const close_descriptor & aVal)
{
	MPushImpl(aVal, true);
}
void CDataObject::MPush(const kernel_infos_diff_t& aVal)
{
	MPushImpl(aVal, true);
	//CHardWorker::MPushData(kernel_infos_diff_t::NAME, aVal, true,false,true);
}
void CDataObject::MPush(const new_id_t & aVal)
{
	MPushImpl(aVal,false);
}
void CDataObject::MPush(const fail_send_by_id_t & aVal)
{
	MPushImpl(aVal,false);
}
void CDataObject::MPush(const user_data_id_t & aVal)
{
	VLOG(2) << "USER DATA Push " << user_data_id_t::NAME << " data size = "
						<< sizeof(aVal);
	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FFifoMutex);
		std::map<descriptor_t,user_operation_t>::iterator _it=FUserDataFIFO.find(aVal.FId);
		if(_it==FUserDataFIFO.end())
		{
			std::map<descriptor_t,user_operation_t>::value_type const _val(aVal.FId,*this);
			_it=FUserDataFIFO.insert(_val).first;
		}
		_it->second.MPush(aVal);
	}

//		MNewDataFor(user_data_id_t::NAME, sMUserDataOperation);
}
void CDataObject::user_operation_t::MPush(const user_data_id_t & aVal)
{
	bool _need_call = false;
	{
		_need_call = !FIsWorking && FUserDataFIFO.empty();
		VLOG_IF(4,_need_call) << "Need call handling data";
		FUserDataFIFO.push_back(aVal);
	}

	if (_need_call)
	{
		operation_t _op(sMUserDataOperation, this, operation_t::AS_LOWER);
		FThis.MPutOperation(_op);
	}
}

int CDataObject::user_operation_t::sMUserDataOperation(const NSHARE::CThread* WHO,
		NSHARE::operation_t* WHAT, void* YOU_DATA)
{
	VLOG(2) << "User data Operation ";
	user_operation_t* _p = reinterpret_cast<user_operation_t*>(YOU_DATA);
	CHECK_NOTNULL(_p);
	_p->MUserOperation(WHO, WHAT,user_data_id_t::NAME );
	return 0;
}
void CDataObject::user_operation_t::MUserOperation(const NSHARE::CThread* WHO,
		NSHARE::operation_t* WHAT, const NSHARE::CText& aWhat)
{
	VLOG(2) << "Operation for user data " << aWhat;
	CHECK_EQ(user_data_id_t::NAME, aWhat);
	args_data_t _args;
	_args.FType = aWhat;
	user_data_id_t _data;

	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FThis.FFifoMutex);
		CHECK(!FUserDataFIFO.empty());
		_data = FUserDataFIFO.front();
		FUserDataFIFO.pop_front();
		VLOG_IF(2,FUserDataFIFO.empty()) << "No more data";
		FIsWorking = true;
	}
	_args.FPointToData = &_data;
	int _count = FThis.MCall(aWhat, &_args);
	VLOG(2) << "EOK:" << _count;

	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FThis.FFifoMutex);
		if (!FUserDataFIFO.empty())
		{
			WHAT->MKeep(true);
		}
		else
		{
			FIsWorking = false;
			FThis.FUserDataFIFO.erase(_data.FId);
		}
	}
	VLOG(2) << "Finish handle";
}
void CDataObject::MPush(const kernel_infos_array_id_t & aVal)
{
	MPushImpl(aVal,false);
}
void CDataObject::MPush(const demands_id_t & aVal)
{
	MPushImpl(aVal,false);
}
void CDataObject::MPush(const demand_dgs_for_by_id_t & aVal)
{
	MPushImpl(aVal,false);
}
NSHARE::CConfig CDataObject::MSerialize() const
{
	NSHARE::CRAII<NSHARE::CMutex> _block(FUserDataMutex);
	NSHARE::CConfig _conf(NAME);
	_conf.MAdd(CHardWorker::MSerialize());
	_conf.MAdd(user_data_id_t::NAME,FUserDataFIFO.size());
	return _conf;

}
NSHARE::IAllocater* CDataObject::MDefAllocater() const
{
	return FAllocater;
}
NSHARE::IAllocater* CDataObject::MSetDefAllocater(NSHARE::IAllocater* aAlloc)
{
	VLOG(2)<<"Update allocator";
	return FAllocater = aAlloc;
}
} /* namespace NUDT */
