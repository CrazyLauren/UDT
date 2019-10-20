/*
 * CEvent.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.11.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CEVENT_H_
#define CEVENT_H_

namespace NSHARE
{
template<class TEvent_t>
class  IEventEmpty;

/**\brief Класс содержит список обработчиков события
 *
 * Этот класс предназначен для хранения списка обработчиков события.
 * Вызов обработчиков события осуществляется путём вызова метода  MCall.
 * По историческим причинам он является публичным(хотя должен быть protected).
 * Вызов методов CB осуществляется в порядке их приоритета, от меньшего к большему.
 * "Настройка" класса под конкрентный задачи осуществляется через шаблоны, в том
 * числе и полиморфизм методов класса (возможность перегружать)
 * Сигнатура CB:
 * 		- Первый аргумент -указатель на объект инициализирующего событие (если объекта нет - NULL)
 * 		- Второй аргумент -указатель на параметры события (если параметров нет - NULL)
 * 		- возвращаемое значение \see eCBRval
 *
 *\tparam TSender_type - тип объекта инициализирующего событие (тип первого аргумета CB)
 *\tparam TEvent_t - тип указателя на CB - сигнатура CB ()
 *\tparam TEventArg_type - тип аргумента события
 *\tparam TIEvent - тип интерфейса класса CEvent. Если нужно добавить возможность пергрузки методов класса то в качестве этого параметра укажите IEvent
 *\tparam TMutexType - если нужно сделать класс потокобезопасным укажите в качестве этого параметра CMutex
 *\todo заменить  multiset на vector
 */
template<class TSender_type = void*, class TEvent_t = CB_t,
		class TEventArg_type =CB_t::arg_t,template<class > class TIEvent = IEventEmpty,
		class TMutexType = CNoMutex>
class CEvent: public TIEvent<TEvent_t>
{
public:
	typedef CEvent<TSender_type, TEvent_t, TEventArg_type,TIEvent,TMutexType> my_t;
	typedef TEvent_t value_t;
	typedef TEventArg_type value_arg_t;
	typedef TSender_type sender_t;
	typedef TMutexType mutex_t;

private:
	struct Type
	{
		unsigned int FPrior;
		mutable value_t FCb;
		eCBRval operator ()(sender_t aSender, value_arg_t aWhat) const;
		bool operator <(Type const& right) const;
		bool operator ==(Type const& right) const;
		bool operator !=(Type const& right) const;
		bool operator ==(value_t const& right) const;

	};
	typedef std::vector<Type> MapOfCallback_t;
	typedef typename MapOfCallback_t::iterator iterator;
	typedef typename MapOfCallback_t::const_iterator const_iterator;
public:

	typedef typename MapOfCallback_t::size_type size_t;

	CEvent(sender_t aSender);
	CEvent();
	~CEvent()
	{
	}

	bool operator+=(value_t const &);
	bool operator-=(value_t const &);
	/**\brief добавить CB
	 *
	 * Если CB уже существует, то он заменяется
	 *
	 * Сложность O(n)
	 *\param aCB - СB
	 *\param aPrior - приоритет CB
	 */
	bool MAdd(value_t const & aCB, unsigned int aPrior =
			std::numeric_limits<unsigned int>::max());

	/**\brief удалить CB
	 *
	 * Сложность O(n)
	 *\param aCB - СB
	 */
	bool MErase(value_t const& aCB);

	/**\brief изменить приоритет CB
	 *
	 * Сложность O(n)
	 *\param aCB - СB
	 *\param aPrior - новый приоритет
	 */
	bool MChangePrior(value_t const&aM, unsigned int aPrior);

	bool MIs(value_t const& aM) const;

	size_t MSize() const;
	inline std::ostream& MPrintEvent(std::ostream & aStream) const;
//protected:

	/**\brief  Вызов всех CB
	 *
	 * Сложность O(n)
	 *\param aCallbackArgs - параметры события
	 *\retunr количество вызванных CB
	 */
	int MCall(value_arg_t const& aCallbackArgs);

	/**\brief возвращает кол-во изменение списка CB
	 *
	 *\return число изменений
	 */
	inline atomic_t::value_type MGetNumberOfChange() const;

	/**\brief Сравнивает счётчки кол-ва изменеий списка  CB
	 *
	 *\param aVal -  предыдущее значение счётчика
	 *\return true - если список изменился
	 */
	inline bool MWasChanged(atomic_t::value_type const& aVal) const;

	void MClear();

	CEvent(CEvent const& aRht);
	CEvent& operator=(CEvent const& aRht);
protected:

	/**\brief метод используется в CEvents::MCall для синхронизации
	 * независмых изменений списка CB
	 *
	 *\param aOldValue - старый список
	 *\param aNewValue - новый старый список
	 *\note aOldValue - старый теккущий список из которого удалили события и получили список aNewValue
	 */
	void MSynchronizeChange(CEvent& aOldValue,CEvent& aNewValue);
private:
	bool operator==(CEvent const &)
	{
		return false;
	}
	bool operator!=(CEvent const &)
	{
		return false;
	}

	MapOfCallback_t FCBs;
	sender_t  FSender;
	mutable mutex_t	 FMutex;
	NSHARE::atomic_t FNumberOfArrayChange;///< Счётчик изменений FCBs, используется для оптимизации в методе MSynchronizeChange

	template<class key_type, class event_type,
			template<class, class > class IEvents, class mutex_type>
	friend class CEvents;
};

/**\brief Интерфейс CEvent
 *
 * Если возникла необходимость перегружать методы в CEvent, то в качестве
 * аргумента шаблона нужно указать этот класс
 */
template<class TEvent_t>
class  IEvent
{
public:
	typedef TEvent_t value_t;
	virtual ~IEvent()
	{

	}
	virtual bool MAdd(value_t const & aCB, unsigned int aPrior =
			std::numeric_limits<unsigned int>::max())=0;
	virtual bool MErase(value_t const& aCB)=0;
	virtual bool MChangePrior(value_t const&aM, unsigned int aPrior)=0;
};

/**\brief Интерфейс CEvent без полиморфизма
 *
 */
template<class TEvent_t>
class  IEventEmpty
{
public:
	typedef TEvent_t value_t;
};
}//namespace USHARE
#include <share/UType/CEvent.hpp>

#endif /*CEVENT_H_*/
