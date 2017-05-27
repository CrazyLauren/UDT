/*
 * CSafeQueue.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef _CSAFEQUEUE_H_
#define _CSAFEQUEUE_H_

namespace NSHARE
{
template<class DataType, typename _Sequence = std::deque<DataType> >
class  CSafeQueue: protected _Sequence
{
public:
	typedef typename _Sequence::value_type value_type;
	typedef typename _Sequence::size_type size_type;
	typedef typename _Sequence::reference reference;
	typedef typename _Sequence::const_reference const_reference;
	typedef _Sequence container_type;

	CSafeQueue()
	{
		;
	}

	CSafeQueue(CSafeQueue const& aRht) :
			_Sequence(aRht) //TODO �����������
	{
		;
	}
	CSafeQueue& operator=(CSafeQueue const& aRht)
	{
		CRAII<CMutex> _block(aRht.FMutex);
		_Sequence::operator=(aRht);
		return *this;
	}

	bool back_pop(value_type&);
	bool front_pop(value_type&);

	value_type& back();
	value_type const& back() const;

	value_type& front();
	value_type const& front() const;

	bool empty() const;

	bool push(value_type const&);
	size_type size() const;

	template<typename _Func>
	int MOptimizePopBack(_Func& aFunc, int aMaxCount = 50)
	{
		if (container_type::empty())
			return aMaxCount;
		CRAII<CMutex> _block(FMutex);
		for (; !container_type::empty() && aMaxCount > 0; --aMaxCount)
		{
			aFunc(container_type::back());
			container_type::pop_back();
		}
		return 0;
	}
	template<typename _Func>
	int MOptimizePopFront(_Func& aFunc, int aMaxCount = 50)
	{
		if (container_type::empty())
			return aMaxCount;
		CRAII<CMutex> _block(FMutex);
		for (; !container_type::empty() && aMaxCount > 0; --aMaxCount)
		{
			aFunc(container_type::front());
			container_type::pop_front();
		}
		return 0;
	}

protected:
	CMutex FMutex;
};

template<class DataType, typename _Sequence>
typename CSafeQueue<DataType, _Sequence>::value_type& CSafeQueue<DataType,
		_Sequence>::back()
{
	CRAII<CMutex> _block(FMutex);
	return container_type::back();
}
template<class DataType, typename _Sequence>
bool CSafeQueue<DataType, _Sequence>::back_pop(
		typename CSafeQueue<DataType, _Sequence>::value_type& aVal)
{
	CRAII<CMutex> _block(FMutex);
	if (container_type::empty())
		return false;
	aVal = container_type::back();
	container_type::pop_back();
	return true;
}
template<class DataType, typename _Sequence>
bool CSafeQueue<DataType, _Sequence>::front_pop(
		typename CSafeQueue<DataType, _Sequence>::value_type& aVal)
{
	CRAII<CMutex> _block(FMutex);
	if (container_type::empty())
		return false;
	aVal = container_type::front();
	container_type::pop_front();
	return true;
}

template<class DataType, typename _Sequence>
typename CSafeQueue<DataType, _Sequence>::value_type const& CSafeQueue<DataType,
		_Sequence>::front() const
{
	CRAII<CMutex> _block(FMutex);
	return container_type::back();
}
template<class DataType, typename _Sequence>
typename CSafeQueue<DataType, _Sequence>::value_type& CSafeQueue<DataType,
		_Sequence>::front()
{
	CRAII<CMutex> _block(FMutex);
	return container_type::front();
}

template<class DataType, typename _Sequence>
bool CSafeQueue<DataType, _Sequence>::empty() const
{
	CRAII<CMutex> _block(FMutex);
	return container_type::front();
}
template<class DataType, typename _Sequence>
bool CSafeQueue<DataType, _Sequence>::push(value_type const&aVal)
{
	CRAII<CMutex> _block(FMutex);
	bool const _is=container_type::empty();
	container_type::push_back(aVal);
	return !_is;
}
template<class DataType, typename _Sequence>
typename CSafeQueue<DataType, _Sequence>::size_type CSafeQueue<DataType,
		_Sequence>::size() const
{
	CRAII<CMutex> _block(FMutex);
	return container_type::size();
}
} //namespace USHARE
//��� ��������� ���������
namespace std
{
template<class T>
class back_insert_iterator<class NSHARE::CSafeQueue<T> > : public iterator<
		output_iterator_tag, void, void, void, void>
{
protected:
	NSHARE::CSafeQueue<T>* container;

public:
	typedef NSHARE::CSafeQueue<T> container_type;
	explicit back_insert_iterator(NSHARE::CSafeQueue<T>& aX) :
			container(&aX)
	{
	}
	back_insert_iterator&
	operator=(typename NSHARE::CSafeQueue<T>::const_reference aValue)
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
class front_insert_iterator<class NSHARE::CSafeQueue<T> > : public iterator<
		output_iterator_tag, void, void, void, void>
{
protected:
	NSHARE::CSafeQueue<T>* container;

public:
	typedef NSHARE::CSafeQueue<T> container_type;

	explicit front_insert_iterator(NSHARE::CSafeQueue<T>& aX) :
			container(&aX)
	{
	}
	front_insert_iterator&
	operator=(typename NSHARE::CSafeQueue<T>::const_reference aValue)
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
#endif 	// _CSAFEQUEUE_H_
