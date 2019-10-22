// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * IIntrusived.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 20.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#include <deftype>
#include <SHARE/UType/IIntrusived.h>

namespace NSHARE
{
struct IIntrusived::_w_counter_t
{
	_w_counter_t(IIntrusived* aPtr) :
			FWPtr(aPtr)
	{
		FCount = 0;
	}

	/*_w_counter_t(_w_counter_t const& aRht):FWPtr(aRht.FWPtr)
	{
		FCount = 0;
		//copy
		MRef();
	}*/

	int MRef() 
	{
		//todo first
		++FCount;
		return FCount;
	}
	int MUnref() 
	{
		--FCount;
		CHECK_LT(FCount, std::numeric_limits<unsigned>::max() / 2);
		if (FCount == 0)
		{
			VLOG(2) << "Remove ";
			delete this;
			return 0;
		}
		else
			return FCount;
	}
	 mutable atomic_t FCount;
	IIntrusived* FWPtr;
protected:
	~_w_counter_t()
	{

	}
	_w_counter_t& operator=(_w_counter_t const& aRht)
	{
		return *this;
	}
};

IIntrusived::w_counter_t::w_counter_t(IIntrusived* aP) :
		FWCounter(new _w_counter_t(aP))
{
	CHECK_NOTNULL(FWCounter);
	FWCounter->MRef();
}
IIntrusived::w_counter_t::w_counter_t(w_counter_t const & aRht) :
		FWCounter(aRht.FWCounter)
{
	CHECK_NOTNULL(FWCounter);
	if (FWCounter->MRef() == 0)
		FWCounter = NULL;
}
IIntrusived::w_counter_t& IIntrusived::w_counter_t::operator =(
		const w_counter_t& aRht)
{
	if (this != &aRht)
	{
		if (FWCounter && FWCounter->MUnref() == 0)
			FWCounter = NULL;
		FWCounter = aRht.FWCounter;
		CHECK_NOTNULL(FWCounter);
		if (FWCounter->MRef() == 0)
			FWCounter = NULL;
	}
	return *this;
}
IIntrusived* IIntrusived::w_counter_t::MGet() const
{
	IIntrusived* _ptr = FWCounter ? FWCounter->FWPtr : NULL;
	return _ptr;
}
IIntrusived::w_counter_t::~w_counter_t()
{
	if (FWCounter && FWCounter->MUnref() == 0)
		FWCounter = NULL;
}

IIntrusived::IIntrusived() :
		 FWn(this)
{
	FCount=0;
	FReferedCount = 0;
	FIsFirst=P_NOT_INITED;
	VLOG(2) << "Construct object " << this;
}
IIntrusived::IIntrusived(const IIntrusived& aRht) :
		FWn(this)
{
	FCount = 0;
	FReferedCount = 0;
	FIsFirst = P_NOT_INITED;
	VLOG(2) << "Constract object " << this;
	DCHECK(aRht.MCountRef() > 0);
}
IIntrusived::~IIntrusived()
{
	LOG_IF(DFATAL, MCountRef() != 0) << "Ref not null";
	VLOG(2) << "Destruct object " << this;
}
IIntrusived& IIntrusived::operator =(const IIntrusived& aVal)
{
	LOG(DFATAL)<< "operator = (" << this << ", " << (&aVal);
	return *this;
}

int IIntrusived::MRefImpl() const
{
	if (FIsFirst == P_NOT_INITED)
	{
		FIsFirst= P_INITED;
		++FCount;
		++FReferedCount;
		return FCount;
	}
	else if (FCount && //
			FIsFirst == P_INITED && //
			FCount++ > 0 //
					)
	{
		++FReferedCount;
		return FCount;
	}
	else
	{
		return 0;
	}
}

int IIntrusived::MUnrefImpl() const
{
	if (FIsFirst != P_INITED)
		return -1;
	--FCount;
	if (FCount == 0)
	{
		FIsFirst= P_REMOVING;
		FWn.FWCounter->FWPtr = NULL;
		return 0;
	}
	return FCount;
}
int IIntrusived::MUnrefWithoutDelete() const
{
	VLOG(3) << "MUnrefWithoutDelete() " << this;
	--FCount;
	return FCount;
}
int IIntrusived::MCountRef() const
{
	return FCount;
}
unsigned IIntrusived::MReferedCount() const
{
	return FReferedCount;
}
void IIntrusived::MDelete() const
{
	VLOG(2) << "Delete object " << this;
	FWn.FWCounter->FWPtr = NULL;
	delete this;
}
}
