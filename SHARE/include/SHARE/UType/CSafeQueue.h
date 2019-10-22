/*
 * CThreadSafeQueue.h
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
/**\brief Thread-safe queue
 *
 */
template<class DataType, typename _Sequence = std::deque<DataType>,
		class TMutexType = CMutex >
class  CThreadSafeQueue: private _Sequence
{
	typedef TMutexType mutex_t;
public:
	typedef typename _Sequence::value_type value_type;
	typedef typename _Sequence::size_type size_type;
	typedef typename _Sequence::reference reference;
	typedef typename _Sequence::const_reference const_reference;
	typedef _Sequence container_type;

	CThreadSafeQueue()
	{
		;
	}

	explicit CThreadSafeQueue(container_type const& aRht) :
			_Sequence(aRht)
	{
		;
	}
	CThreadSafeQueue(CThreadSafeQueue const& aRht)
	{
		;// \note don't copy mutex!
		CRAII<mutex_t> _block(aRht.FMutex);
		_Sequence::operator=(aRht);
	}
	CThreadSafeQueue& operator=(CThreadSafeQueue const& aRht)
	{
		if (&aRht != this)
		{
			CRAII<mutex_t> _block(FMutex);
			CRAII<mutex_t> _block2(aRht.FMutex);
			_Sequence::operator=(aRht);
		}
		return *this;
	}

	bool back_pop(value_type&);
	bool front_pop(value_type&);

	value_type back() const;

	value_type front() const;

	bool empty() const;

	/**\brief push value to queue to end
	 *
	 *\return false if queue was empty
	 */
	bool push(value_type const&);
	bool push_back(value_type const&);

	/**\brief push value to queue to begin
	 *
	 *\return false if queue was empty
	 */
	bool push_front(value_type const&);


	size_type size() const;

	template<typename _Func>
	int MOptimizePopBack(_Func& aFunc, int aMaxCount = 50)
	{
		if (container_type::empty())
			return aMaxCount;
		CRAII<mutex_t> _block(FMutex);
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
		CRAII<mutex_t> _block(FMutex);
		for (; !container_type::empty() && aMaxCount > 0; --aMaxCount)
		{
			aFunc(container_type::front());
			container_type::pop_front();
		}
		return 0;
	}

protected:
	mutex_t FMutex;
};

template<class DataType, typename _Sequence,class TMutexType>
typename CThreadSafeQueue<DataType, _Sequence,TMutexType>::value_type CThreadSafeQueue<DataType,
		_Sequence,TMutexType>::back() const
{
	CRAII<mutex_t> _block(FMutex);
	CHECK(!container_type::empty());

	return container_type::back();
}
template<class DataType, typename _Sequence,class TMutexType>
bool CThreadSafeQueue<DataType, _Sequence,TMutexType>::back_pop(
		typename CThreadSafeQueue<DataType, _Sequence,TMutexType>::value_type& aVal)
{
	CRAII<mutex_t> _block(FMutex);
	if (container_type::empty())
		return false;
	aVal = container_type::back();
	container_type::pop_back();
	return true;
}
template<class DataType, typename _Sequence,class TMutexType>
bool CThreadSafeQueue<DataType, _Sequence,TMutexType>::front_pop(
		typename CThreadSafeQueue<DataType, _Sequence,TMutexType>::value_type& aVal)
{
	CRAII<mutex_t> _block(FMutex);
	if (container_type::empty())
		return false;
	aVal = container_type::front();
	container_type::pop_front();
	return true;
}

template<class DataType, typename _Sequence,class TMutexType>
typename CThreadSafeQueue<DataType, _Sequence,TMutexType>::value_type  CThreadSafeQueue<DataType,
		_Sequence,TMutexType>::front() const
{
	CRAII<mutex_t> _block(FMutex);
	CHECK(!container_type::empty());

	return container_type::front();
}

template<class DataType, typename _Sequence,class TMutexType>
bool CThreadSafeQueue<DataType, _Sequence,TMutexType>::empty() const
{
	CRAII<mutex_t> _block(FMutex);
	return container_type::empty();
}
template<class DataType, typename _Sequence,class TMutexType>
bool CThreadSafeQueue<DataType, _Sequence,TMutexType>::push(value_type const&aVal)
{
	return push_back(aVal);
}
template<class DataType, typename _Sequence,class TMutexType>
bool CThreadSafeQueue<DataType, _Sequence,TMutexType>::push_back(value_type const&aVal)
{
	CRAII<mutex_t> _block(FMutex);
	bool const _is=container_type::empty();
	container_type::push_back(aVal);
	return !_is;
}
template<class DataType, typename _Sequence,class TMutexType>
bool CThreadSafeQueue<DataType, _Sequence,TMutexType>::push_front(value_type const&aVal)
{
	CRAII<mutex_t> _block(FMutex);
	bool const _is=container_type::empty();
	container_type::push_front(aVal);
	return !_is;
}

template<class DataType, typename _Sequence,class TMutexType>
typename CThreadSafeQueue<DataType, _Sequence,TMutexType>::size_type CThreadSafeQueue<DataType,
		_Sequence,TMutexType>::size() const
{
	CRAII<mutex_t> _block(FMutex);
	return container_type::size();
}
} //namespace USHARE
namespace std
{
template<class T>
class back_insert_iterator<class NSHARE::CThreadSafeQueue<T> > : public iterator<
		output_iterator_tag, void, void, void, void>
{
protected:
	NSHARE::CThreadSafeQueue<T>* container;

public:
	typedef NSHARE::CThreadSafeQueue<T> container_type;
	explicit back_insert_iterator(NSHARE::CThreadSafeQueue<T>& aX) :
			container(&aX)
	{
	}
	back_insert_iterator&
	operator=(typename NSHARE::CThreadSafeQueue<T>::const_reference aValue)
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
class front_insert_iterator<class NSHARE::CThreadSafeQueue<T> > : public iterator<
		output_iterator_tag, void, void, void, void>
{
protected:
	NSHARE::CThreadSafeQueue<T>* container;

public:
	typedef NSHARE::CThreadSafeQueue<T> container_type;

	explicit front_insert_iterator(NSHARE::CThreadSafeQueue<T>& aX) :
			container(&aX)
	{
	}
	front_insert_iterator&
	operator=(typename NSHARE::CThreadSafeQueue<T>::const_reference aValue)
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
