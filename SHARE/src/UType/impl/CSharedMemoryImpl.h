/*
 * CSharedMemoryImpl.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSHAREDMEMORYIMPL_H_
#define CSHAREDMEMORYIMPL_H_

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <share/UType/CSharedMemory.h>
#include "CSharedAllocatorImpl.h"

namespace NSHARE
{
class CSharedAllocatorImpl;

/** As the boost provides User-friendly interface for working
 * this shared memory, this class need to be development
 *
 */
class CSharedMemory::CImpl
{
public:
	/** Create or open (is memory not valid) shared memory
	 *
	 * If error is occurred when the exception boost::interprocess_exception can invoked.
	 * (it's equal E_CAN_NOT_READ_FILE error)
	 * The filed FError is save the error code
	 *
	 * @param aName a name of shared memory
	 * @param aSize requirement size of memory
	 * @param aReserv amount of memory to reserve for allocation by this process
	 */
	CImpl(const NSHARE::CText& aName, size_t aSize,size_t aReserv);

	/** Try only open shared memory
	 *
	 * If error is occurred when the exception boost::interprocess_exception can invoked
	 * (it's equal E_CAN_NOT_READ_FILE error)
	 * The filed FError is save the error code
	 *
	 * @param aName a name of shared memory
	 * @param aIsCleanUp  The cleaning up memory is necessary (remove all memory leak)
	 * @param aReserv amount of memory to reserve for allocation by this process
	 */
	CImpl(const NSHARE::CText& aName,bool aIsCleanUp,size_t aReserv);

	~CImpl();

	bool MFree();
	size_t MGetSize();
	bool MInit(const NSHARE::CText& aName, size_t aSize,bool aIsCleanUp,size_t aReserv);
	bool MCleanUp(CSharedAllocator::clean_up_f_t WHO, void* YOU_DATA);
	bool MCleanUpShareSem();

	bool MLock() const;
	bool MUnlock() const;
	bool MLockAlloc() const;
	bool MUnlockAlloc() const;

	static int sMCleanUp(CSharedAllocator* WHO, void* WHAT, void* YOU_DATA);
//private:
	bool FIsInited;
	mem_info_t* FInfo;
	void* FUserData;
	CSharedAllocator::clean_up_f_t FUserCb;
	boost::interprocess::mapped_region FRegion;
	CSharedAllocatorImpl FAllocater;
	mutable NSHARE::CIPCSem FShareSem;
	mutable NSHARE::CIPCSem FAllocaterSem;

	eError FError;
	boost::interprocess::shared_memory_object FShedMem;

};

template<> class CRAII<CSharedMemory> : public CDenyCopying
{
public:
	explicit CRAII(CSharedMemory::CImpl& aSem) :
			FSem(aSem)
	{
		MLock();
	}

	~CRAII()
	{
		MUnlock();
	}

	inline void MUnlock()
	{
		if (FIsLock)
			FSem.MUnlock();

		FIsLock = false;
	}
private:
	inline void MLock()
	{
		FSem.MLock();
		FIsLock = true;
	}

	CSharedMemory::CImpl& FSem;
	volatile bool FIsLock;
};
}




#endif /* CSHAREDMEMORYIMPL_H_ */
