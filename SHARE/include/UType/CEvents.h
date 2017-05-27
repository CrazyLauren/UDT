/*
 * CEvents.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 07.04.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CEVENTS_H_
#define CEVENTS_H_

namespace NSHARE
{

template<class key_type, class value_type>
class  IEventsEmpty;
template<typename _Tp>
struct  CEventLessComapare:std::less<_Tp>
{
};

/** \brief Класс реализует идеологию обратных вызовов
 *
 * Этот класс предназначен для упрощения реализация идеологии обратных вызовов.
 * Чтобы реализавать CB идеологию  достаточно отнаследовать этот класс.
 * Инициализация события осуществляется путём вызова метода  MCall.
 * Вызов методов обработчиков осуществляется в порядке установленному в event_type.
 * "Настройка" класса под конкрентный задачи осуществляется через шаблоны, в том
 * числе и полиморфизм методов класса (возможность перегружать)
 *
 * Сигнатура CB определяется event_type.
 *
 * \tparam key_type - тип ключа события
 * \tparam event_type - тип CB
 * \tparam TIEvents - тип интерфейса класса CEvents. Если нужно добавить возможность пергрузки методов класса то в качестве этого параметра укажите IEvents
 * \tparam mutex_type - если нужно сделать класс потокобезопасным укажите в качестве этого параметра CMutex
 */
template<class key_type = NSHARE::CText, class event_type = CEvent<>,
		template<class, class > class TIEvents = IEventsEmpty,
		class mutex_type = CNoMutex>
class  CEvents: public TIEvents<key_type, typename event_type::value_t>
{
public:
	typedef std::map<key_type, event_type> Sequence_t;
	typedef CEvents<key_type, event_type, TIEvents, mutex_type> my_t;
	typedef event_type event_t;
	typedef key_type key_t;
	typedef typename event_t::sender_t sender_t;
	typedef typename event_t::value_arg_t value_arg_t;
	typedef typename TIEvents<key_type, typename event_type::value_t>::value_t value_t;
	typedef mutex_type mutex_t;

	explicit CEvents(sender_t const& aSender);
	explicit CEvents(CEvents const&,sender_t const& aSender);//Copy
	CEvents();
	bool operator+=(value_t const & aVal);
	bool operator-=(value_t const & aVal);
	/** \brief добавить CB
	 *
	 * Если CB уже существует, то он заменяется
	 *
	 *	\param aVal - пара ключ - CB
	 *	\param aPrior - приоритет CB
	 */
	bool MAdd(value_t const & aVal, unsigned int aPrior = std::numeric_limits<unsigned int>::max());

	/** \brief удалить CB
	 *
	 *	\param aVal -  пара ключ - CB
	 */
	bool MErase(value_t const& aVal);

	bool MChangePrior(value_t const&aVal, unsigned int aPrior);
	bool MIs(value_t const& aVal) const;
	bool MIsKey(key_t const& aVal) const;
	inline std::ostream& MPrintEvents(std::ostream & aStream) const;

	inline bool MEmpty  ()const;

	/** \brief возвращает кол-во изменение списка
	 *
	 *	\return число изменений
	 */
	inline atomic_t::value_type MGetNumberOfChange() const;

	/** \brief Сравнивает счётчки кол-ва изменеий списка
	 *
	 *	\param aVal -  предыдущее значение счётчика
	 *	\return true - если список изменился
	 */
	inline bool MWasChanged(atomic_t::value_type const& aVal) const;
#ifndef TEST
protected:
#endif
	int MCall(key_t const& aKey, value_arg_t const& aCallbackArgs);
	~CEvents()
	{
		;
	}
private:
	typedef typename Sequence_t::iterator iterator;
	typedef typename Sequence_t::const_iterator const_iterator;
	Sequence_t FCBs;//<! Array of CB
	sender_t FSender;//<! pointer to Event's sender
	mutable mutex_t	 FMutex;//<! mutex
	NSHARE::atomic_t FNumberOfArrayChange;//<! Счётчик изменений FCBs, используется для оптимизации в методе MCall
};


/** \brief Интерфейс CEvents
 *
 * Если возникла необходимость перегружать методы в CEvents, то в качестве
 * аргумента шаблона нужно указать этот класс
 */
template<class key_type, class value_type>
class  IEvents
{
public:
	struct  value_t
	{
		value_t():FKey(key_type()), FCb(value_type())
		{

		}
		value_t (key_type const& aKey,value_type const& aCb):
			FKey(aKey),FCb(aCb)
		{

		}
		key_type FKey;
		value_type FCb;
	};

	virtual ~IEvents()
	{

	}
	virtual bool MAdd(value_t const & aVal, unsigned int aPrior = std::numeric_limits<unsigned int>::max())=0;
	virtual bool MErase(value_t const& aVal)=0;
	virtual bool MChangePrior(value_t const&aVal, unsigned int aPrior)=0;
};
/** \brief Интерфейс CEvents без полиморфизма
 *
 */
template<class key_type, class value_type>
class  IEventsEmpty
{
public:
	struct  value_t
	{
		value_t():FKey(),FCb()
		{

		}
		value_t (key_type const& aKey,value_type const& aCb):
			FKey(aKey),FCb(aCb)
		{

		}
		key_type FKey;
		value_type FCb;
	};
};
} //namespace USHARE
#include <UType/CEvents.hpp>
#endif /* CEVENTS_H_ */
