/*
 * CStaticList.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.12.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CCOUNTER_H_
#define CCOUNTER_H_

namespace NSHARE
{
template<class T, class Sequence = std::vector<T*> >
class  CStaticList//deprecated
{
public:
	typedef Sequence sequence_t;
	CStaticList()
	{
		MInit();
	}
	CStaticList(CStaticList<T> const& aRht)
	{
		MInit();
	}
	CStaticList<T>& operator=(CStaticList<T> const& aRht) //TODO продумать этот вопрос
	{
//		MInit();
		return *this;
	}
	~CStaticList()
	{
		MDelete();
	}
	static typename sequence_t::size_type sMSize()
	{
		return sMData().size();
	}
	template<class U>
	static T* sMGetObject(U const& aVal)
	{
		typename sequence_t::iterator _it = std::find(sMData().begin(),
				sMData().end(), aVal);

		if (_it == sMData().end())
			return NULL;
		else
			return *_it;
	}
	template<class Pred>
	static T* sMGetIfObject(Pred aPred)
	{
		typename sequence_t::iterator _it = std::find_if(sMData().begin(),
				sMData().end(), aPred);
		if (_it == sMData().end())
			return NULL;
		else
			return *_it;
	}
	template<class Pred>
	static void sMForEach(Pred aPred)
	{
		std::for_each(sMData().begin(),
				sMData().end(), aPred);
	}
	static T* sMGetObject(size_t aNumber)
	{
		if (aNumber < sMSize())
			return sMData()[aNumber];
		else
			return NULL;
	}
	static bool sMIsObject(T* aThis)
	{
		for (typename sequence_t::iterator _it = sMData().begin();
				_it != sMData().end(); ++_it)
		{
			if ((*_it) == aThis)
			{
				return true;
			}
		}
		return false;
	}
protected:
	static sequence_t & sMList()
	{
		return sMData();
	}
	inline void MDelete()
	{
		for (typename sequence_t::iterator _it = sMData().begin();
				_it != sMData().end(); ++_it)
		{
			if ((*_it) == this)
			{
				sMData().erase(_it);
				sMDirty()=true;
				break;
			}
		}
	}
	static bool& sMDirty()
	{
		static  bool _is_dirty=false;
		return _is_dirty;
	}
private:
	inline void MInit()
	{
		sMDirty()=true;
		sMData().push_back(static_cast<T*>(this));
	}
	static sequence_t& sMData()
	{
		static sequence_t _data;
		return _data;
	}
}
;
template<class T>
class  CCounter
{
public:
	CCounter()
	{
		++sMCounter();
	}
	~CCounter()
	{
		--sMCounter();
	}
	static int sMCount()
	{
		return sMCounter();
	}
private:
	static int& sMCounter()
	{
		static int _i=0;
		return _i;
	}
};
} //namespace USHARE

#endif /* CCOUNTER_H_ */
