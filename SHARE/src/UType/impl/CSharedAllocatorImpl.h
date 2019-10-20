/*
 * CSharedAllocatorImpl.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSHAREDALLOCATORIMPL_H_
#define CSHAREDALLOCATORIMPL_H_

#include <share/UType/CSharedMemory.h>
#include <share/UType/CSharedAllocator.h>

namespace NSHARE
{
class CSharedAllocatorImpl: public IAllocater
{
public:
	typedef uint8_t common_allocater_t;
	COMPILE_ASSERT((sizeof(NULL_OFFSET)>=sizeof(CSharedAllocator::NULL_OFFSET)),InvalidNullOffsetofSM);

	CSharedAllocatorImpl(CSharedMemory::CImpl&);
	virtual ~CSharedAllocatorImpl();
	virtual void* MAllocate(size_type aSize, uint8_t aAlignment = 4,eAllocatorType = ALLOCATE_FROM_COMMON);
	virtual void* MReallocate(void* p, size_type aSize, uint8_t aAlignment = 4,eAllocatorType = ALLOCATE_FROM_COMMON);
	virtual void MDeallocate(void* p, size_type aSize);
	virtual size_type MMaxSize() const;
	virtual size_t MGetUsedMemory() const;
	virtual size_t MGetNumberOfAllocations() const;

	virtual offset_pointer_t MOffset(void* aP) const;

	virtual void* MPointer(offset_pointer_t aOffset) const;

	virtual eAllocationState MIsMemAllocated(void* aP) const;

	virtual bool MAtomicLock() const;

	virtual bool MAtomicUnlock() const;

	virtual bool MLockBlock(void* aP) const;
	virtual bool MUnlockBlock(void* aP) const;


	CSharedAllocator FAllocater;
	CSharedMemory::CImpl& FThis;
};
}

#endif /* CSHAREDALLOCATORIMPL_H_ */
