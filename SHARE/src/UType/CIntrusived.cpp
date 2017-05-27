/*
 * CIntrusived.cpp
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
#include <UType/CIntrusived.h>

namespace NSHARE
{
struct CIntrusived::_w_counter_t
{
	_w_counter_t(CIntrusived* aPtr) :
			FCount(0), FWPtr(aPtr)
	{

	}

	_w_counter_t(_w_counter_t const& aRht):FWPtr(aRht.FWPtr)
	{
		//copy
		MRef();
	}

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
	CIntrusived* FWPtr;
protected:
	~_w_counter_t()
	{

	}
	_w_counter_t& operator=(_w_counter_t const& aRht)
	{
		return *this;
	}
};

CIntrusived::w_counter_t::w_counter_t(CIntrusived* aP) :
		FWCounter(new _w_counter_t(aP))
{
	CHECK_NOTNULL(FWCounter);
	FWCounter->MRef();
}
CIntrusived::w_counter_t::w_counter_t(w_counter_t const & aRht) :
		FWCounter(aRht.FWCounter)
{
	CHECK_NOTNULL(FWCounter);
	if (FWCounter->MRef() == 0)
		FWCounter = NULL;
}
CIntrusived::w_counter_t& CIntrusived::w_counter_t::operator =(
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
CIntrusived* CIntrusived::w_counter_t::MGet() const
{
	CIntrusived* _ptr = FWCounter ? FWCounter->FWPtr : NULL;
	return _ptr;
}
CIntrusived::w_counter_t::~w_counter_t()
{
	if (FWCounter && FWCounter->MUnref() == 0)
		FWCounter = NULL;
}

CIntrusived::CIntrusived() :
		FCount(0), FReferedCount(0), FIsFirst(P_NOT_INITED), FWn(this)
{
	VLOG(2) << "Construct object " << this;
}
CIntrusived::CIntrusived(const CIntrusived& aRht) :
		FCount(0), FReferedCount(0), FIsFirst(P_NOT_INITED), FWn(this)
{
	VLOG(2) << "Constract object " << this;
	DCHECK(aRht.MCountRef() > 0);
}
CIntrusived::~CIntrusived()
{
	LOG_IF(DFATAL, MCountRef() != 0) << "Ref not null";
	VLOG(2) << "Destruct object " << this;
}
CIntrusived& CIntrusived::operator =(const CIntrusived& aVal)
{
	LOG(DFATAL)<< "operator = (" << this << ", " << (&aVal);
	return *this;
}

int CIntrusived::MRefImpl() const
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

int CIntrusived::MUnrefImpl() const
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
int CIntrusived::MUnrefWithoutDelete() const
{
	VLOG(3) << "MUnrefWithoutDelete() " << this;
	--FCount;
	return FCount;
}
int CIntrusived::MCountRef() const
{
	return FCount;
}
unsigned CIntrusived::MReferedCount() const
{
	return FReferedCount;
}
void CIntrusived::MDelete() const
{
	VLOG(2) << "Delete object " << this;
	FWn.FWCounter->FWPtr = NULL;
	delete this;
}
}
