// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSharedMemory.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#include <deftype>
#include <share/UType/CSharedAllocator.h>
#include <share/UType/CSharedMemory.h>
#include "impl/CSharedMemoryImpl.h"

namespace NSHARE
{
const NSHARE::CText CSharedMemory::NAME = "shm";
const NSHARE::CText CSharedMemory::KEY_SHM_NAME = "smname";

CSharedMemory::CSharedMemory() :
		FImpl(NULL)
{
}

CSharedMemory::~CSharedMemory()
{
	delete FImpl;
}
CSharedMemory::eError CSharedMemory::MOpen(const NSHARE::CText& aName,bool aIsCleanUp,size_t aReserv)
{
	using namespace boost::interprocess;
	VLOG(2) << "Open shared memory " << aName;
	CHECK(!FImpl);
	eError _error(E_NO_ERROR);
	try
	{
		FImpl=NULL;
		FImpl = new CImpl(aName,aIsCleanUp,aReserv);
	} catch (interprocess_exception const&  aEx)
	{
		LOG(ERROR) << "Cannot open " << aName << " shared memory. as "<<aEx.what();
		CHECK(!FImpl);
		_error=E_CANNOT_READ_FILE;
	}
	catch (...)
	{
		LOG(ERROR) << "Cannot open " << aName << " shared memory.";
		CHECK(!FImpl);
		_error=E_CANNOT_READ_FILE;
	}
	bool _is=MIsOpened();
	if (!_is && FImpl)
	{
		_error=FImpl->FError;
		delete FImpl;
		FImpl = NULL;
	}
	return _error;
}
CSharedMemory::eError CSharedMemory::MOpenOrCreate(const NSHARE::CText& aName, size_t aSize,size_t aReserv)
{
	using namespace boost::interprocess;
	VLOG(2) << "Open shared memory " << aName<<" Size = "<<aSize;
	eError _error(E_NO_ERROR);

	if(!MCheckSize(aSize))
	{
		LOG(DFATAL)<<"Invalid size of memory "<<aSize;
		return E_INVALID_REQUREMNT_SIZE;
	}
	CHECK(!FImpl);
	try
	{
		FImpl=NULL;
		FImpl = new CImpl(aName, aSize,aReserv);
	} catch (interprocess_exception const&  aEx)
	{
		LOG(ERROR) << "Cannot open " << aName << " shared memory. as "<<aEx.what();
		CHECK(!FImpl);
		_error=E_CANNOT_READ_FILE;
	}
	catch (...)
	{
		LOG(ERROR) << "Cannot open " << aName << " shared memory.";
		CHECK(!FImpl);
		_error=E_CANNOT_READ_FILE;
	}

	bool _is=MIsOpened();
	if (!_is && FImpl)
	{
		_error=FImpl->FError;
		delete FImpl;
		FImpl = NULL;
	}
	return _error;
}
bool CSharedMemory::MIsOpened() const
{
	return FImpl && FImpl->FIsInited;
}
bool CSharedMemory::MCheckSize(size_t aVal)const
{
	return aVal<=static_cast<size_t>(std::numeric_limits<boost::interprocess::offset_t>::max())&&//
			aVal<=static_cast<size_t>(std::numeric_limits<CSharedAllocator::offset_t>::max());
}
bool CSharedMemory::MFree()
{
	if(FImpl)
	{ 
		DCHECK_NOTNULL (FImpl);
		bool _is= FImpl->MFree();
		delete FImpl;
		FImpl=NULL;
		return _is;
	}
	return false;
}

size_t CSharedMemory::MGetSize() const
{
	return FImpl->MGetSize();
}

IAllocater* CSharedMemory::MGetAllocator() const
{
	if (!FImpl)
		return NULL;

	return &FImpl->FAllocater;
}
CSharedAllocator* CSharedMemory::MGetMemorManager() const
{
	if (!FImpl)
		return NULL;

	return &FImpl->FAllocater.FAllocater;
}

void* CSharedMemory::MMallocTo(const uint32_t xWantedSize, IAllocater::offset_pointer_t aRefOffset) const
{
	DCHECK_NOTNULL (FImpl);
	return FImpl->FAllocater.FAllocater.MMallocTo(xWantedSize, static_cast<CSharedAllocator::offset_t>(aRefOffset));
}

void* CSharedMemory::MGetIfMalloced(IAllocater::offset_pointer_t aRefOffset) const
{
	DCHECK_NOTNULL (FImpl);
	return FImpl->FAllocater.FAllocater.MGetIfMalloced(static_cast<CSharedAllocator::offset_t>(aRefOffset));
}

void CSharedMemory::MCleanUp()
{
	DCHECK_NOTNULL (FImpl);
	FImpl->FAllocater.FAllocater.MCleanUpResource();
}

bool CSharedMemory::MIsNeedCleanUpResource()
{
	DCHECK_NOTNULL (FImpl);
	return FImpl->FAllocater.FAllocater.MIsNeedCleanUpResource();
}

void CSharedMemory::MFinishCleanUp()
{
	DCHECK_NOTNULL (FImpl);
	if (FImpl->FAllocater.FAllocater.MIsWatchDog())
		FImpl->FAllocater.FAllocater.MResetWatchDog();
}
bool CSharedMemory::MCleanUpResourceByWatchDog(
		int (*aF)(CSharedAllocator* WHO, void* WHAT, void* YOU_DATA),void *YOU_DATA)
{
	DCHECK_NOTNULL(FImpl);
	return FImpl->MCleanUp(aF,YOU_DATA);
}
NSHARE::CConfig CSharedMemory::MSerialize() const
{
	DCHECK_NOTNULL(FImpl);

	NSHARE::CConfig _conf(NAME);

	if(FImpl)
	{
		_conf.MAdd(KEY_SHM_NAME, FImpl->FShedMem.get_name());
		_conf.MAdd(FImpl->FAllocater.FAllocater.MSerialize());
	}
	return _conf;
}
bool CSharedMemory::sMRemove(NSHARE::CText const& aName)
{
	using namespace boost::interprocess;
	return shared_memory_object::remove(aName.c_str());
}
} /* namespace NSHARE */
