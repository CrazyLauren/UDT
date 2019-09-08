// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CHardWorker.cpp
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
using namespace NSHARE;
namespace NUDT
{
CHardWorker::CHardWorker() :
		FIsDestoy(false)
{
}

CHardWorker::CHardWorker(int aNum) :
		FIsDestoy(false)
{
	MCreate(aNum);
}
CHardWorker::CHardWorker(NSHARE::CThread::eThreadPriority _priority, int aNum) :
		FIsDestoy(false)
{

	MCreate(_priority,aNum);
}
void CHardWorker::MStop()
{
	FPool.MCancel();
}
bool CHardWorker::MCreate(int aNum)
{
	return FPool.MCreate(aNum);
}
bool CHardWorker::MCreate(NSHARE::CThread::eThreadPriority _priority, int aNum)
{
	NSHARE::CThread::param_t _param;
	_param.priority = _priority;
	return FPool.MCreate(aNum, &_param);
}

CHardWorker::~CHardWorker()
{
	FIsDestoy = true;
	FPool.MCancel();
	MCleanUP();
}
void CHardWorker::MCleanUP()
{
	NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);
	array_t::iterator _it = FData.begin();
	for (; _it != FData.end(); ++_it)
	{
		fifo_data_t &_fifo=_it->second;
		std::list<object_data_t>::iterator _jt = _fifo.FFifo.begin();
		for (; _jt != _fifo.FFifo.end(); ++_jt)
			MFree(*_jt);
		MFree(_fifo.FLatest);
	}
}

NSHARE::eCBRval CHardWorker::sMOperation(const NSHARE::CThread* WHO,
		NSHARE::operation_t* WHAT, void* YOU_DATA)
{
	VLOG(2) << "Operation ";
	data_for* _p = reinterpret_cast<data_for*>(YOU_DATA);
	CHECK_NOTNULL(_p);
	eCBRval const _rval=_p->FThis->MOperation(WHO, WHAT, _p->FWhat);
	delete _p;
	return _rval;
}
bool CHardWorker::MPutOperation(NSHARE::operation_t const& aOp)
{
	return FPool+=aOp;
}

CHardWorker::fifo_data_t& CHardWorker::MGetOrCreateFifoImpl(
		const NSHARE::CText& aWhat)
{
	array_t::iterator _it = FData.find(aWhat);
	if (_it == FData.end())
	{
		VLOG(2) << "The " << aWhat << " will be added for the first time.";
		array_t::value_type _val(aWhat, fifo_data_t());
		std::pair<array_t::iterator, bool> _reuslt = FData.insert(_val);
		CHECK(_reuslt.second);
		_it = _reuslt.first;
	}
	CHECK(_it!=FData.end());
	DCHECK(FData.find(aWhat)==_it);
	return _it->second;
}
NSHARE::eCBRval CHardWorker::MExecuteOperationFor(const NSHARE::CText& aWhat,
		object_data_t& _user_data)
{
	args_data_t _args;
	_args.FType = aWhat;
	CHECK(_user_data.FData);
	_args.FPointToData = _user_data.FData;
	VLOG(1) << "Call";
	int _count = MCall(aWhat, &_args);
	VLOG(1) << "EOK:" << _count;
	MFree(_user_data);
	return E_CB_REMOVE;
}

NSHARE::eCBRval CHardWorker::MOperation(const NSHARE::CThread* WHO,
		NSHARE::operation_t* WHAT, const NSHARE::CText& aWhat)
{
	VLOG(2) << "Operation for " << aWhat;
	object_data_t _user_data;
	{
		NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);
		array_t::iterator _it = FData.find(aWhat);
		if (_it == FData.end())
		{
			LOG(ERROR) << "The " << aWhat << " is not exist.";
			return E_CB_REMOVE;
		}
		else
		{
			object_fifo_t& _ob_fifo=_it->second.FFifo;
			DCHECK(!_ob_fifo.empty());
			if (_ob_fifo.empty())
				return E_CB_REMOVE;

			//moving the memory of  type T
			_user_data = _ob_fifo.front();
			_ob_fifo.pop_front();
		}
	}
	return MExecuteOperationFor(aWhat, _user_data);
}
void CHardWorker::MNewDataFor(const NSHARE::CText& aWhat,
		NSHARE::op_signal_t const& aCb, bool isPrior)
{
	VLOG(2) << "New data for " << aWhat;
	data_for* _for = new data_for; //I don't want to use FIFO algorithm,
	//therefore transmit data to "operation" func
	//directly
	_for->FWhat = aWhat;
	_for->FThis = this;
	operation_t _op(aCb, _for,
			isPrior ? operation_t::IMMEDIATE : operation_t::AS_LOWER);
	if (!(FPool += _op))
	{
		LOG(ERROR) << "Cannot add operation for " << aWhat;
		delete _for;
	}
}
void CHardWorker::MFree(object_data_t& aWhat)
{
	CHECK(aWhat.FData);
	(this->*aWhat.FDestroy)(aWhat.FData);
	free(aWhat.FData);	//fixme
}
NSHARE::CConfig CHardWorker::MSerialize() const
{
	NSHARE::CConfig _conf("hw");
	_conf.MAdd(/*NSHARE::CThreadPool::NAME,*/ FPool.MSerialize());

	NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);

	array_t::const_iterator _it=FData.begin(),_it_end(FData.end());
	for(;_it!=_it_end;++_it)
	{
		_conf.MAdd(_it->first,_it->second.FFifo.size());
	}
	return _conf;
}
} /* namespace NUDT */
