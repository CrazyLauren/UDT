/*
 * IIntrusived.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.04.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CREFER_H_
#define CREFER_H_

namespace NSHARE
{
class IIntrusived;
}
namespace boost
{
inline void intrusive_ptr_add_ref(NSHARE::IIntrusived* p);
inline void intrusive_ptr_release(NSHARE::IIntrusived* p);
}

namespace NSHARE
{
/**\brief base class for intrusive pointer to object
 *
 * Stores an embedded counter.
 *
 *\note rename to IIntrusived
 */
class  SHARE_EXPORT IIntrusived
{
public:
	IIntrusived();
	IIntrusived(const IIntrusived& aRht);

	unsigned MReferedCount() const;
protected:
	int MUnrefWithoutDelete() const;
	int MCountRef() const;

	virtual ~IIntrusived();
	IIntrusived& operator =(const IIntrusived& aVal);
private:
	struct _w_counter_t;
	enum
	{
		P_INITED = 0, P_NOT_INITED = 1, P_REMOVING = 2
	};
	struct SHARE_EXPORT w_counter_t
	{
		w_counter_t(IIntrusived* aP);
		w_counter_t(w_counter_t const &);
		w_counter_t& operator =(const w_counter_t& aVal);
		~w_counter_t();
		IIntrusived* MGet() const;
		_w_counter_t* FWCounter;
	};

	template<class T>
	static int sMRef(T*);
	template<class T>
	static int sMUnref(T*);

	int MRefImpl() const;
	int MUnrefImpl() const;
	void MDelete() const;

	mutable atomic_t FCount;
	mutable atomic_t FReferedCount;
	mutable atomic_t FIsFirst;
	w_counter_t FWn;

	template<class U> friend class w_ptr;
	template<class U> friend class intrusive_ptr;

	friend void boost::intrusive_ptr_add_ref(IIntrusived* p);
	friend void boost::intrusive_ptr_release(IIntrusived* p);

};
template<class T>
inline int IIntrusived::sMRef(T* aP)
{
	int _val = 0;
	{
		if (!aP)
			return 0;
		_val = ((IIntrusived*)aP)->MRefImpl();
		if (_val <= 0)
			aP = NULL;
	}
	return _val;
}
template<class T>
inline int IIntrusived::sMUnref(T* aP)
{
	int _val;
	T* _tmp = NULL;
	{
		if (!aP)
			return 0;
		if ((_val = ((IIntrusived*)aP)->MUnrefImpl()) == 0)
		{
			_tmp = aP;
			aP = NULL;
		}
	}
	if (_val == 0)
	{		
		((IIntrusived*)_tmp)->MDelete();
	}
	return _val;
}
}
namespace boost
{
inline void intrusive_ptr_add_ref(NSHARE::IIntrusived* p)
{
	NSHARE::IIntrusived::sMRef(p);
}
inline void intrusive_ptr_release(NSHARE::IIntrusived* p)
{
	NSHARE::IIntrusived::sMUnref(p);
}
}

#endif /* CREFER_H_ */
