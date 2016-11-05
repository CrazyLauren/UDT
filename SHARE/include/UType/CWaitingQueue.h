/*
 * CWaitingQueue.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 04.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CWAITINGQUEUE_H_
#define CWAITINGQUEUE_H_

namespace NSHARE
{
template<typename Tp, typename _Sequence = std::list<Tp> >
class  CWaitingQueue: protected _Sequence, CDenyCopying
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

	inline bool empty() const
	{
		return _Sequence::empty();
	}
	inline size_type size() const
	{
		return _Sequence::size();
	}
	value_type& back()
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		return _Sequence::back();
	}
	value_type const& back() const
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		return _Sequence::back();
	}

	value_type& front()
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		return _Sequence::front();
	}
	value_type const& front() const
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		return _Sequence::front();
	}

	void pop()
	{
		CRAII<CMutex> _block(FMutex);
		_Sequence::pop_back();
	}
	void back_pop(value_type& aVal)
	{
		CRAII<CMutex> _block(FMutex);
		MWaitForData();
		if(!empty())
		{
			aVal = _Sequence::back();
			_Sequence::pop_back();
		}
	}
	void push(const value_type& aVal)
	{
		CRAII<CMutex> _block(FMutex);
		_Sequence::push_back(aVal);
		MDataSignal();
	}
protected:
	CCondvar FCond;
	CMutex FMutex;
private:
	inline void MWaitForData()
	{
		for (HANG_INIT; _Sequence::empty();HANG_CHECK)
			FCond.MTimedwait(&FMutex);
	}
	inline void MDataSignal()
	{
		FCond.MSignal();
	}
};
}//namespace USHARE
//��� ��������� ���������
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
