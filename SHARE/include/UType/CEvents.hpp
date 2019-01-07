/*
 * CEvents.hpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.11.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CEVENTS_HPP_
#define CEVENTS_HPP_

namespace NSHARE
{
template<>
struct CEventLessComapare<CText> : NSHARE::CStringFastLessCompare
{

};
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline CEvents<key_type, event_type, TIEvents, mutex_type>::CEvents(CEvents const& aRht)
{
	;// \note don't copy mutex!
	CRAII<mutex_t> _blocked(aRht.FMutex);
	FCBs=aRht.FCBs;
	FSender=aRht.FSender;
	FNumberOfArrayChange=aRht.FNumberOfArrayChange;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline CEvents<key_type, event_type, TIEvents, mutex_type>&
CEvents<key_type, event_type, TIEvents, mutex_type>::operator=(
		CEvents const& aRht)
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
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline CEvents<key_type, event_type, TIEvents, mutex_type>::CEvents(
		sender_t const& aSender) :
		FSender(aSender)
{
	FNumberOfArrayChange=0;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline CEvents<key_type, event_type, TIEvents, mutex_type>::CEvents(
		CEvents const& aRht, sender_t const& aSender) :
		FCBs(aRht.FCBs), FSender(aSender)
{
	FNumberOfArrayChange=0;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline CEvents<key_type, event_type, TIEvents, mutex_type>::CEvents() :
		FSender(NULL)
{
	FNumberOfArrayChange=0;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::operator+=(
		value_t const & aVal)
{
	return MAdd(aVal);
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::operator-=(
		value_t const & aVal)
{
	return MErase(aVal);
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::MAdd(
		value_t const & aVal, unsigned int aPrior)
{
	CRAII<mutex_t> _blocked(FMutex);
	iterator _it = FCBs.find(aVal.FKey);
	if (_it == FCBs.end())
	{
		_it =
				FCBs.insert(
						typename Sequence_t::value_type(aVal.FKey,
								event_t(FSender))).first;
		++FNumberOfArrayChange;
	}
	bool const _is=_it->second.MAdd(aVal.FCb, aPrior);
	return _is;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::MErase(
		value_t const& aVal)
{
	CRAII<mutex_t> _blocked(FMutex);
	iterator _it = FCBs.find(aVal.FKey);
	if (_it == FCBs.end())
		return false;
	if (!_it->second.MErase(aVal.FCb))
		return false;
	if (_it->second.MSize() == 0)
	{
		FCBs.erase(_it);
		++FNumberOfArrayChange;
	}
	return true;
}

template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::MChangePrior(
		value_t const&aVal, unsigned int aPrior)
{
	CRAII<mutex_t> _blocked(FMutex);
	iterator _it = FCBs.find(aVal.FKey);
	if (_it == FCBs.end())
		return false;
	return _it->second.MChangePrior(aVal.FCb, aPrior);
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::MIs(
		value_t const& aVal) const
{
	CRAII<mutex_t> _blocked(FMutex);
	const_iterator _it = FCBs.find(aVal.FKey);
	if (_it == FCBs.end())
		return false;
	return _it->second.MIs(aVal.FCb);
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::MIsKey(
		key_t const& aVal) const
{
	CRAII<mutex_t> _blocked(FMutex);
	const_iterator _it = FCBs.find(aVal);
	if (_it == FCBs.end())
		return false;
	return true;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline int CEvents<key_type, event_type, TIEvents, mutex_type>::MCall(
		key_t const& aKey, value_arg_t const& aCallbackArgs)
{
	CRAII<mutex_t> _blocked(FMutex);
	iterator _it = FCBs.find(aKey);
	if (_it == FCBs.end())
			return 0;

	const atomic_t::value_type _number=MGetNumberOfChange();
	event_t _event (_it->second);
	event_t _event_copy (_event);
	_blocked.MUnlock();// \note Для того чтобы не блокировать вызов других событий

	int _rval = _event.MCall(aCallbackArgs);

	if(_event.MWasChanged(_event_copy.MGetNumberOfChange()))// список был изменён
	{
		CRAII<mutex_t> _blocked(FMutex);

		if(MWasChanged(_number))
			_it=FCBs.find(aKey);

		if (_it != FCBs.end())
		{
			_it->second.MSynchronizeChange(_event_copy,_event);


			if (_it->second.MSize() == 0)
			{
				FCBs.erase(_it);
				++FNumberOfArrayChange;
			}
		}
	}

	return _rval;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline std::ostream& CEvents<key_type, event_type, TIEvents, mutex_type>::MPrintEvents(
		std::ostream & aStream) const
{
	CRAII<mutex_t> _blocked(FMutex);
	const_iterator _it = FCBs.begin();
	const_iterator const _it_end = FCBs.end();

	for (; _it != _it_end; ++_it)
		aStream << "Key: \"" << _it->first << "\". \n" << _it->second
				<< std::endl;
	return aStream;
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::MEmpty() const
{
	return FCBs.empty();
}
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline atomic_t::value_type CEvents<key_type, event_type, TIEvents, mutex_type>::MGetNumberOfChange() const
{
	return FNumberOfArrayChange;
}

template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline bool CEvents<key_type, event_type, TIEvents, mutex_type>::MWasChanged(
		atomic_t::value_type const& aVal) const
{
	return MGetNumberOfChange()!=aVal;
}
} //namespace USHARE

namespace std
{
template<class key_type, class event_type,
		template<class, class > class TIEvents, class mutex_type>
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CEvents<key_type, event_type, TIEvents, mutex_type> const& aEvent)
{
	return aEvent.MPrintEvents(aStream);
}
} //namespace std

#endif /* CEVENTS_HPP_ */
