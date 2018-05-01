/*
 * CEvent.hpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.11.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CEVENT_HPP_
#define CEVENT_HPP_

namespace NSHARE
{
template<class TSender_type, class TEvent_t, class TEventArg_type, template<
		class > class TIEvent, class TMutexType>
inline CEvent<TSender_type, TEvent_t, TEventArg_type, TIEvent, TMutexType>::CEvent(
		CEvent const& aRht)
{
	;// \note don't copy mutex!
	CRAII<mutex_t> _blocked(aRht.FMutex);
	FCBs=aRht.FCBs;
	FSender=aRht.FSender;
	FNumberOfArrayChange=aRht.FNumberOfArrayChange;
}
template<class TSender_type, class TEvent_t, class TEventArg_type, template<
		class > class TIEvent, class TMutexType>
inline CEvent<TSender_type, TEvent_t, TEventArg_type, TIEvent, TMutexType>&CEvent<
		TSender_type, TEvent_t, TEventArg_type, TIEvent, TMutexType>::operator=(
		CEvent const& aRht)
{
	if (&aRht != this)
	{
		CRAII<mutex_t> _blocked(FMutex);
		CRAII<mutex_t> _blocked2(aRht.FMutex);
		FCBs=aRht.FCBs;
		FSender=aRht.FSender;
		FNumberOfArrayChange=aRht.FNumberOfArrayChange;
	}
	return *this;
}

template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::CEvent() :
		FSender(NULL)
{
	;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::CEvent(sender_t aSender) :
		FSender(aSender)
{
	;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::MAdd(
		value_t const & aCB, unsigned int aPrior)
{
	CRAII<mutex_t> _blocked(FMutex);
	iterator _it=FCBs.begin();
	for (; _it != FCBs.end() && !(*_it == aCB);++_it)
		;
	if (_it != FCBs.end())
	{
		if(aPrior==_it->FPrior)
			return true;
		else
			FCBs.erase(_it);
	}

	Type _val =
	{ aPrior, aCB };

	iterator const _instert_to=std::upper_bound(FCBs.begin(),FCBs.end(),_val);

	FCBs.insert(_instert_to,_val);
	++FNumberOfArrayChange;
	return true;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::MErase(
		value_t const& aCB)
{
	CRAII<mutex_t> _blocked(FMutex);
	iterator _it=FCBs.begin();
	for (; _it != FCBs.end() && !(*_it == aCB);++_it)
		;
	if (_it != FCBs.end())
	{
		FCBs.erase(_it);
		++FNumberOfArrayChange;
		return true;
	}
	return false;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::MIs(
		value_t const& aCB) const
{
	CRAII<mutex_t> _blocked(FMutex);
	const_iterator _it=FCBs.begin();
	for (; _it != FCBs.end() && !(*_it == aCB);++_it)
		;
	return _it != FCBs.end();
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline int CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::MCall(
		value_arg_t const& aCallbackArgs)
{
	CRAII<mutex_t> _blocked(FMutex);
	int _count = 0;
	for (iterator _it(FCBs.begin()); _it != FCBs.end();)
	{
		eCBRval const _rval=(*_it)(FSender, aCallbackArgs);

		switch(_rval)
		{
			case E_CB_REMOVE:
				++FNumberOfArrayChange;
				_it=FCBs.erase(_it);
				break;

			case E_CB_SAFE_IT:
				++_count;
				++_it;
			break;

			case E_CB_BLOCING_OTHER:
				VLOG(2)<<"passing cb";
				return _count;
			break;
			default:
				LOG(DFATAL) << "Unknown code " << (int)_rval;
				++_it;
			break;
		}
	}
	return _count;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::MChangePrior(
		value_t const& aCB, unsigned int aPrior)
{
	CRAII<mutex_t> _blocked(FMutex);
	iterator _it=FCBs.begin();
	for (; _it != FCBs.end() && !(*_it == aCB);++_it)
		;
	if (_it != FCBs.end())
	{
		Type _val(*_it);
		FCBs.erase(_it);
		_val.FPrior = aPrior;

		iterator const _instert_to=std::upper_bound(FCBs.begin(),FCBs.end(),_val);
		FCBs.insert(_instert_to,_val);
		++FNumberOfArrayChange;
		return true;
	}
	return false;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline eCBRval CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::Type::operator ()(
		sender_t aSender, value_arg_t aWhat) const
{
	return (eCBRval) FCb(aSender, aWhat);
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::Type::operator <(
		Type const& right) const
{
	return FPrior < right.FPrior;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::Type::operator ==(
		Type const& right) const
{
	return FPrior == right.FPrior;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::Type::operator !=(
		Type const& right) const
{
	return FPrior != right.FPrior;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::Type::operator ==(
		value_t const& right) const
{
	return FCb == right;
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::operator+=(
		value_t const & aCB)
{
	return MAdd(aCB);
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::operator-=(
		value_t const & aCB)
{
	return MErase(aCB);
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline typename CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::size_t CEvent<
		TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::MSize() const
{
	return FCBs.size();
}
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline std::ostream& CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType>::MPrintEvent(
		std::ostream & aStream) const
{
	CRAII<mutex_t> _blocked(FMutex);
	const_iterator _it = FCBs.begin();
	const_iterator const _it_end = FCBs.end();

	for (; _it != _it_end; ++_it)
		aStream << "Prior = " << _it->FPrior << ". \t" << _it->FCb << std::endl;
	return aStream;
}
template<class TSender_type, class TEvent_t, class TEventArg_type, template<
		class > class TIEvent, class TMutexType>
inline void CEvent<TSender_type, TEvent_t, TEventArg_type, TIEvent, TMutexType>::MSynchronizeChange(
		CEvent& aOldValue, CEvent& aNewValue)
{
	CRAII<mutex_t> _blocked(FMutex);

	if (!aOldValue.MWasChanged(MGetNumberOfChange())) //Мы не изменились
	{
		FCBs.swap(aNewValue.FCBs);
		FNumberOfArrayChange = aNewValue.FNumberOfArrayChange;
	}
	else//синхронизировать списки
	{
		//1) ищем, что было удалено
		//2) удаляем их в текущем списке

		iterator _it_old = aOldValue.FCBs.begin();
		iterator _it_new = aNewValue.FCBs.begin();
		for (; _it_new != aNewValue.FCBs.end() &&		//
				_it_old != aOldValue.FCBs.end();)
		{
			if(_it_old->FCb==_it_new->FCb)
			{
				++_it_new;
				++_it_old;
			}else//removed
			{
				MErase(_it_old->FCb);
				++_it_old;
			}
		}
		for(;_it_old != aOldValue.FCBs.end();++_it_old)
		{
			MErase(_it_old->FCb);
		}
	}
}


template<class TSender_type, class TEvent_t, class TEventArg_type, template<
		class > class TIEvent, class TMutexType>
inline atomic_t::value_type CEvent<TSender_type, TEvent_t, TEventArg_type,
		TIEvent, TMutexType>::MGetNumberOfChange() const
{
	return FNumberOfArrayChange;
}

template<class TSender_type, class TEvent_t, class TEventArg_type, template<
		class > class TIEvent, class TMutexType>
inline bool CEvent<TSender_type, TEvent_t, TEventArg_type, TIEvent, TMutexType>::MWasChanged(
		atomic_t::value_type const& aVal) const
{
	return MGetNumberOfChange()!=aVal;
}
template<class TSender_type, class TEvent_t, class TEventArg_type, template<
		class > class TIEvent, class TMutexType>
inline void CEvent<TSender_type, TEvent_t, TEventArg_type, TIEvent, TMutexType>::MClear()
{
	CRAII<mutex_t> _blocked(FMutex);
	FCBs.clear();
}
} //namespace USHARE
namespace std
{
template<class TSender_type, class TEvent_t, class TEventArg_type,template<class > class TIEvent, class TMutexType>
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType> const& aEvent)
{
	return aEvent.MPrintEvent(aStream);
}
} //namespace std

#endif /* CEVENT_HPP_ */
