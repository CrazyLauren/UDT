/*
 * CWaitingQueue.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 04.04.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CWAITINGQUEUE_H_
#define CWAITINGQUEUE_H_

namespace NSHARE
{
/**\brief Production-consumer realization
 *
 */
template<typename Tp, typename _Sequence = std::list<Tp> >
class  CWaitingQueue: private _Sequence
{
public:
	typedef typename _Sequence::value_type value_type;
	typedef typename _Sequence::size_type size_type;
	typedef typename _Sequence::reference reference;
	typedef typename _Sequence::const_reference const_reference;
	typedef _Sequence _Sequence_type;

	CWaitingQueue() :
		FMutex(CMutex::MUTEX_NORMAL)
	{
	}

	bool empty() const
	{
		CRAII<CMutex> _block(FMutex);
		return _Sequence_type::empty();
	}
	size_type size() const
	{
		CRAII<CMutex> _block(FMutex);
		return _Sequence_type::size();
	}

	/**\brief return the last element of queue, if it isn't exit
	 * then expect it
	 *
	 *\warning returning reference to element
	 */
	value_type const& back() const
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		return _Sequence_type::back();
	}

	/**\brief copy the last element of queue, if it isn't exit
	 * then expect it
	 *
	 */
	void back(value_type & aTo) const
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		aTo=_Sequence_type::back();
	}

	/**\brief return the first element of queue, if it isn't exit
	 * then expect it
	 *
	 *\warning returning reference to element
	 */
	value_type const& front() const
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		return _Sequence_type::front();
	}

	/**\brief copy the first element of queue, if it isn't exit
	 * then expect it
	 *
	 */
	void front(value_type & aTo) const
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		aTo = _Sequence_type::front();
	}

	/**\brief copy and remove the last element of queue,
	 *  if it isn't exit then expect it.
	 *
	 *\return true - EOK
	 */
	bool back_pop(value_type& aVal)
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		if(!empty())
		{
			aVal = _Sequence_type::back();
			_Sequence_type::pop_back();
			return true;
		}
		return false;
	}

	/**\brief copy and remove the first element of queue,
	 *  if it isn't exit then expect it.
	 *
	 *\return true - EOK
	 */
	bool front_pop(value_type& aVal)
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		if(!empty())
		{
			aVal = _Sequence_type::front();
			_Sequence_type::pop_front();
			return true;
		}
		return false;
	}

	/**\brief pop queue from end
	 */
	void pop()
	{
		pop_back();
	}

	/**\brief pop queue from end
	 */
	void pop_back()
	{
		CRAII<CMutex> _block(FMutex);
		_Sequence_type::pop_back();
	}

	/**\brief pop queue from begin
	 */
	void pop_front()
	{
		CRAII<CMutex> _block(FMutex);
		_Sequence_type::pop_front();
	}

	/**\brief push value to queue to end
	 *
	 *\return false if queue was empty
	 */
	bool push_back(const value_type& aVal)
	{
		CRAII<CMutex> _block(FMutex);
		bool const _is=_Sequence_type::empty();
		_Sequence_type::push_back(aVal);
		MDataSignal();
		return !_is;
	}

	bool push(const value_type& aVal)
	{
		return push_back(aVal);
	}


	/**\brief push value to queue to begin
	 *
	 *\return false if queue was empty
	 */
	bool push_front(const value_type& aVal)
	{
		CRAII<CMutex> _block(FMutex);
		bool const _is=_Sequence_type::empty();
		_Sequence_type::push_front(aVal);
		MDataSignal();
		return !_is;
	}


	CWaitingQueue(CWaitingQueue const& aRht) :
		FMutex(CMutex::MUTEX_NORMAL)
	{
		;// \note don't copy mutex!
		CRAII<CMutex> _block(aRht.FMutex);
		_Sequence::operator=(aRht);
	}
	CWaitingQueue& operator=(CWaitingQueue const& aRht)
	{
		if (&aRht != this)
		{
			CRAII<CMutex> _block(FMutex);

			CRAII<CMutex> _block2(aRht.FMutex);
			_Sequence::operator=(aRht);
			_block2.MUnlock();

			if(!_Sequence_type::empty())
				MDataSignal();
		}
		return *this;
	}

protected:
	mutable CCondvar FCond;
	mutable CMutex FMutex;
private:
	inline void MWaitForData() const
	{
		for (HANG_INIT; _Sequence_type::empty(); HANG_CHECK)
			FCond.MTimedwait(&FMutex);
	}
	inline void MDataSignal()
	{
		FCond.MSignal();
	}
};
}//namespace USHARE
namespace std
{
template<class T>
class back_insert_iterator<class NSHARE::CWaitingQueue<T> >
: public iterator<output_iterator_tag, void, void, void, void>
{
protected:
	NSHARE::CWaitingQueue<T>* container;

public:
	typedef NSHARE::CWaitingQueue<T> container_type;
	explicit back_insert_iterator(NSHARE::CWaitingQueue<T>& aX) :
			container(&aX)
	{
	}
	back_insert_iterator&
	operator=(typename NSHARE::CWaitingQueue<T>::const_reference aValue)
	{
		container->push(aValue);
		return *this;
	}
	back_insert_iterator&
	operator*()
	{
		return *this;
	}
	back_insert_iterator&
	operator++()
	{
		return *this;
	}
	back_insert_iterator operator++(int)
	{
		return *this;
	}
};

template<class T>
class front_insert_iterator<class NSHARE::CWaitingQueue<T> >: public iterator<output_iterator_tag, void, void,
		void, void>
{
protected:
	NSHARE::CWaitingQueue<T>* container;

public:
	typedef NSHARE::CWaitingQueue<T> container_type;

	explicit front_insert_iterator(NSHARE::CWaitingQueue<T>& aX) :
			container(&aX)
	{
	}
	front_insert_iterator&
	operator=(typename NSHARE::CWaitingQueue<T>::const_reference aValue)
	{
		container->front(aValue);
		return *this;
	}
	front_insert_iterator&
	operator*()
	{
		return *this;
	}
	front_insert_iterator&
	operator++()
	{
		return *this;
	}
	front_insert_iterator operator++(int)
	{
		return *this;
	}
};

} //namespace std

#endif /* CWAITINGQUEUE_H_ */
