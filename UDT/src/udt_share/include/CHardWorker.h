/*
 * CHardWorker.h
 *
 *  Created on: 03.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CHARDWORKER_H_
#define CHARDWORKER_H_
#include <udt_share.h>
#include <programm_id.h>
#include <udt_types.h>
#include <shared_types.h>
namespace NUDT
{
class UDT_SHARE_EXPORT CHardWorker;

struct UDT_SHARE_EXPORT args_data_t
{
	NSHARE::CText FType;
	void* FPointToData;
};
typedef int (*data_signal_t)(CHardWorker* WHO, args_data_t* WHAT,
		void* YOU_DATA);
typedef NSHARE::Callback_t<data_signal_t> callback_data_t;
typedef NSHARE::CEvent<CHardWorker*, callback_data_t, args_data_t*> data_event_t;
typedef NSHARE::CEvents<NSHARE::CText, data_event_t, NSHARE::IEventsEmpty,
		NSHARE::CMutex> data_events_t;


class UDT_SHARE_EXPORT CHardWorker:  public data_events_t
{
public:
	CHardWorker();
	CHardWorker(int aNum);
	CHardWorker(NSHARE::CThread::eThreadPriority _priority,int aNum=-1);
	void MCreate(int aNum=-1);
	void MCreate(NSHARE::CThread::eThreadPriority _priority,int aNum=-1);

	//The resources can be leaked in destructor
	virtual ~CHardWorker();

	//T has to be movable type. I.e.
	//The memory of T can be moved without calling
	//coping constuctor.

	template<class T>
	inline void MPushData(NSHARE::CText const &aWhat, const T& aVal,
			bool isPrior, bool aAddLatest = false, bool aIgnorePool = false);

	template<class T>
	bool MGet(NSHARE::CText const& aType, T&) const;
	bool MPutOperation(NSHARE::operation_t const&);
	NSHARE::CConfig MSerialize() const;
protected:

	struct UDT_SHARE_EXPORT object_data_t
	{
		object_data_t() :
				FData(NULL), FDataSize(0),FDestroy(0)
		{

		}
		void* FData;
		size_t FDataSize;
		template<class T>
		inline T const* MCast() const;
	private:
		void (CHardWorker::*FDestroy)(void*);
		//todo fixme save operation
		friend class CHardWorker;
	};
	typedef std::list<object_data_t> object_fifo_t;
	struct UDT_SHARE_EXPORT fifo_data_t//fixme fifo by name does not need
	//
	{
		object_fifo_t FFifo;
		object_data_t FLatest;
	};
	struct UDT_SHARE_EXPORT data_for
	{
		NSHARE::CText FWhat;
		CHardWorker* FThis;
	};
	typedef std::map<NSHARE::CText, fifo_data_t, NSHARE::CStringFastLessCompare> array_t;

	void MCleanUP();
	template<class T>
	void MDataCleanUp(void*);

	static NSHARE::eCBRval sMOperation(NSHARE::CThread const* WHO,
			NSHARE::operation_t* WHAT, void* YOU_DATA);
	NSHARE::eCBRval MOperation(NSHARE::CThread const* WHO, NSHARE::operation_t* WHAT,
			NSHARE::CText const &aWhat);
	NSHARE::eCBRval MExecuteOperationFor(const NSHARE::CText& aWhat,
			object_data_t& _user_data);

	void MNewDataFor(NSHARE::CText const &aWhat,NSHARE::op_signal_t const& aCb=sMOperation,bool isPrior=false);
	template<class T>
	static inline void sMPutData(object_data_t  &aTo, const T& aVal);
	void MFree(object_data_t& aWhat);

	template<class T>
	inline void MPushToMap(NSHARE::CText const &aWhat, const T& aVal, bool aAddLatest);
	template<class T>
	inline void MPutLatest(const T& aVal, fifo_data_t& _fifo);

	fifo_data_t& MGetOrCreateFifoImpl(const NSHARE::CText& aWhat);

	NSHARE::CThreadPool FPool;
	mutable NSHARE::CMutex FMutex;
	mutable NSHARE::CMutex FUserDataMutex;
	array_t FData;
	volatile bool FIsDestoy;


};


template<class T>
inline T const* CHardWorker::object_data_t::MCast() const
{
	CHECK_EQ(sizeof(T), FDataSize);
	return reinterpret_cast<T const *>(FData);
}
template<class T>
inline void CHardWorker::MPushData(const NSHARE::CText& aWhat, const T& aVal,
		bool isPrior, bool aAddLatest ,bool aIgnorePool)
{
	VLOG(2) << "Push " << aWhat << " data size = " << sizeof(aVal);
	//VLOG(2) << aVal;
	if (FIsDestoy)
	{
		LOG(ERROR)<<"Try to add "<<aWhat<<" to destroing DataObject.";
		return;
	}
	if (!aIgnorePool)
	{
		MPushToMap(aWhat, aVal,aAddLatest);
		MNewDataFor(aWhat, sMOperation, isPrior);
	}else
	{
		VLOG(2)<<"Call in current thread";

		object_data_t _data;
		sMPutData(_data, aVal);
		if (aAddLatest)
		{
			NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);
			fifo_data_t &_fifo = MGetOrCreateFifoImpl(aWhat);
			MPutLatest(aVal, _fifo);
		}
		MExecuteOperationFor(aWhat, _data);
	}
}
template<class T>
inline void CHardWorker::sMPutData(object_data_t &_data, const T& aVal)
{
	_data.FData=malloc(sizeof(aVal));
	_data.FDataSize=sizeof(aVal);
	new (_data.FData) T(aVal);
	_data.FDestroy = &CHardWorker::MDataCleanUp<T>;
}

template<class T>
inline void CHardWorker::MPushToMap(const NSHARE::CText& aWhat, const T& aVal, bool aAddLatest)
{
	NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);
	fifo_data_t &_fifo=MGetOrCreateFifoImpl(aWhat);
	object_data_t _data;
	sMPutData(_data,aVal);

	if(aAddLatest)
		MPutLatest(aVal,_fifo);

	//The inserting to the list operation can be incorrect
	//if the T is not movable type. I.e. The memory of T can be moved without calling
	//coping constuctor.
	_fifo.FFifo.push_back(_data);
}
template<class T>
inline void CHardWorker::MPutLatest(const T& aVal, fifo_data_t& _fifo)
{
	//free the latest data
	if (_fifo.FLatest.FData)
	{
		VLOG(2) << "The latest is exist";
		MFree(_fifo.FLatest);
	}
	sMPutData(_fifo.FLatest,aVal);
}
template<class T>
inline void CHardWorker::MDataCleanUp(void* aP)
{
	T* _p = reinterpret_cast<T*>(aP);
	_p->~T();
}
template<class T>
inline bool CHardWorker::MGet(NSHARE::CText const& aType, T & aVal) const
{
	VLOG(2) << "Get " << aType;
	NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);
	array_t::const_iterator _it = FData.find(aType);
	if (_it == FData.end())
	{
		LOG(ERROR)<< "The " << aType << " is not exist.";
		return false;
	}
	else
	{
		T const* _pval=_it->second.FLatest. template MCast<T>();
		aVal = *_pval;
		return true;
	}
}

template<>
inline bool CHardWorker::MGet<CHardWorker::object_data_t>(const NSHARE::CText& aType,
		object_data_t& aVal) const
{
	VLOG(2) << "Get " << aType;
	NSHARE::CRAII<NSHARE::CMutex> _blocked(FMutex);
	array_t::const_iterator _it = FData.find(aType);
	if (_it == FData.end())
	{
		LOG(ERROR)<< "The " << aType << " is not exist.";
		return false;
	}
	else
	{
		aVal = _it->second.FLatest;
		return true;
	}
}
}
/* namespace NUDT */
#endif /* CHARDWORKER_H_ */
