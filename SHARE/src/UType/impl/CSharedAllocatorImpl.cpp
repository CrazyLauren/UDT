/*
 * CSharedAllocatorImpl.cpp
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/tasks.h>
#include "CSharedAllocatorImpl.h"
#include "CSharedMemoryImpl.h"

namespace NSHARE
{
CSharedAllocatorImpl::CSharedAllocatorImpl(CSharedMemory::CImpl& aThis) :
		FThis(aThis)
{
}

CSharedAllocatorImpl::~CSharedAllocatorImpl()
{
}

CSharedAllocatorImpl::offset_pointer_t CSharedAllocatorImpl::MOffset(void* aP) const
{
	if(!aP)
		return NULL_OFFSET;

	CSharedAllocator::offset_t const _offset=FAllocater.MOffset(aP);

	if(CSharedAllocator::sMIsNullOffset(_offset))
		return NULL_OFFSET;
	return _offset;
}

void* CSharedAllocatorImpl::MPointer(offset_pointer_t aOffset) const
{
	if(aOffset==NULL_OFFSET)
		return NULL;

	CHECK_LE(aOffset, std::numeric_limits<CSharedAllocator::offset_t>::max());

	void* _p = FAllocater.MPointer(static_cast<CSharedAllocator::offset_t>(aOffset));
	return _p;
}

CSharedAllocatorImpl::eAllocationState CSharedAllocatorImpl::MIsMemAllocated(void* aP) const
{
	return FAllocater.MIsAllocated(aP)?E_IS_ALLOCATED:E_ISNOT_ALLOCATED;
}

void* CSharedAllocatorImpl::MReallocate(void* p, size_type aSize,uint8_t aAlignment,eAllocatorType aType)
{
	void* _p=NULL;
	switch(aType)
	{
	case ALLOCATE_FROM_COMMON:
		_p = FAllocater.MReallocate(p,static_cast<CSharedAllocator::block_size_t>(aSize),true,false);//fixme to config
		break;
	case ALLOCATE_FROM_COMMON_AND_RESERV:
		_p = FAllocater.MReallocate(p, static_cast<CSharedAllocator::block_size_t>(aSize),true,true);//fixme to config
			break;
	};
	return _p;
}
void* CSharedAllocatorImpl::MAllocate(size_type aSize,
		uint8_t aAlignment,eAllocatorType aType)
{
	if (0 == aSize)
		return NULL;
	void* _p=NULL;
	switch(aType)
	{
	case ALLOCATE_FROM_COMMON:
		_p = FAllocater.MMalloc(aSize,true,false);//fixme to config
		break;
	case ALLOCATE_FROM_COMMON_AND_RESERV:
		_p = FAllocater.MMalloc(aSize,true,true);//fixme to config
			break;
	};

	return _p;
}

void CSharedAllocatorImpl::MDeallocate(void* _p, size_type aSize)
{
	if (!_p)
		return;

	FAllocater.MFree(_p);
}
CSharedAllocatorImpl::size_type CSharedAllocatorImpl::MMaxSize() const
{
	return FAllocater.MMaxBlockSize();
	//return FAllocater.MGetFreeBytesRemaining();
}

size_t CSharedAllocatorImpl::MGetUsedMemory() const
{
	return FAllocater.MMaxBlockSize()-FAllocater.MGetFreeBytesRemaining();
}

size_t CSharedAllocatorImpl::MGetNumberOfAllocations() const
{
	return FAllocater.MGetNumberOfAllocations();
}

bool CSharedAllocatorImpl::MAtomicLock() const
{
	//NSHARE::CRAII<NSHARE::CMutex> _block(FMutex);
	//return FAllocater.MLock();//fixme use another mutex for lock CBuffer
	return FThis.MLockAlloc();
}

bool CSharedAllocatorImpl::MAtomicUnlock() const
{
	//NSHARE::CRAII<NSHARE::CMutex> _block(FMutex);
	return FThis.MUnlockAlloc();
}
bool CSharedAllocatorImpl::MLockBlock(void* aP) const
{
	return FAllocater.MLockBlock(aP);
}
bool CSharedAllocatorImpl::MUnlockBlock(void* aP) const
{
	return FAllocater.MUnLockBlock(aP);
}
}


