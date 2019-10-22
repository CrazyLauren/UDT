/*
 * CSharedMemoryImpl.cpp
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
#include <SHARE/crc8.h>
#include <SHARE/UType/CSharedAllocator.h>
#include <SHARE/UType/CSharedMemory.h>
#include "CSharedMemoryImpl.h"

using namespace boost::interprocess;

namespace NSHARE
{
/** @brief Shared memory info
 *
 */
SHARED_PACKED(
		struct CSharedMemory::mem_info_t
		{
			typedef NSHARE::crc8_t<0x96> crc_t;
			mem_info_t() :
			FCrc(0x1),FPIDOfLockedMutex(0),FPIDOfLockedAllocMutex(0), FSize(0),FPidOffCreator(NSHARE::CThread::sMPid())
			{
				memset(FSharedMutex,0,sizeof(FSharedMutex));
				memset(FAllocMutex,0,sizeof(FAllocMutex));
				memset(FAligment, 0, sizeof(FAligment));
			}

			crc_t::type_t FCrc;//!< Checksum of data see MUpdateCRC()
			uint8_t FAligment[4-sizeof(crc_t::type_t)];//!< Trash for alignment data
			uint8_t FSharedMutex[CIPCSem::eReguredBufSize];//!<Mutex for this data
			uint8_t FAllocMutex[CIPCSem::eReguredBufSize];//!<Mutex for safety data see ::CSharedAllocatorImpl
			CSharedAllocator::pid_type FPIDOfLockedMutex;//!< PID of process that lock mutex FSharedMutex
			CSharedAllocator::pid_type FPIDOfLockedAllocMutex;//!< PID of process that locks mutex FPIDOfLockedAllocMutex

			/// @warning don't change order of FSize & FPidOffCreator fields
			CSharedAllocator::block_size_t FSize;//!< Physical size of memory
			const uint32_t FPidOffCreator;		//!<PID of process that creates shared memory

			bool MUpdateCRC();
			bool MCheckCRC()const;
		});
COMPILE_ASSERT(sizeof(CSharedMemory::mem_info_t) ==(sizeof(CSharedMemory::mem_info_t::crc_t::type_t)*1//
		+sizeof(uint8_t)*(4-sizeof(CSharedMemory::mem_info_t::crc_t::type_t))//
		+sizeof(uint8_t)*CIPCSem::eReguredBufSize//
		+sizeof(uint8_t)*CIPCSem::eReguredBufSize//
		+sizeof(CSharedAllocator::pid_type)*2//
		+sizeof(CSharedAllocator::block_size_t)*1//
		+sizeof(uint32_t)*1//
		),IVALID_SIZEOF_MEM_INFO);

/** Update CRC
 *
 * @warning CRC calculates on two fields FPidOffCreator and FSize
 * @return true if no error
 */
bool CSharedMemory::mem_info_t::MUpdateCRC()
{
	/// \note crc берётся от двух переменных
	const size_t _crc_length = (sizeof(FPidOffCreator) + sizeof(FSize)) / sizeof(mem_info_t::crc_t::type_t);
	const size_t  _offset=(offsetof(struct mem_info_t,FSize))/sizeof(mem_info_t::crc_t::type_t);
	const mem_info_t::crc_t::type_t* _begin =
			(mem_info_t::crc_t::type_t*)this+_offset;
	const mem_info_t::crc_t::type_t* _end =
		_begin+ _crc_length;

	FCrc = crc_t::sMCalcCRCofBuf(_begin, _end);
	return true;
}
/** Check CRC
 *
 * @return true if valid
 */
bool CSharedMemory::mem_info_t::MCheckCRC() const
{
	/// \note crc берётся от двух переменных
	const size_t _crc_length = (sizeof(FPidOffCreator) + sizeof(FSize)) / sizeof(mem_info_t::crc_t::type_t);
	const size_t  _offset=(offsetof(struct mem_info_t,FSize))/sizeof(mem_info_t::crc_t::type_t);
	const mem_info_t::crc_t::type_t* _begin =
			(mem_info_t::crc_t::type_t*)this+_offset;

	crc_t::type_t const _crc = crc_t::sMCalcCRCofBuf(_begin,
			_begin + _crc_length);
	return _crc == FCrc && FPidOffCreator>0 && FSize>0;
}
CSharedMemory::CImpl::CImpl(const NSHARE::CText& aName, size_t aSize,size_t aReserv) :
		FIsInited(false), //
		FInfo(NULL), //
		FUserData(NULL), //
		FUserCb(NULL), //
		FAllocater(*this),//
		FError(E_NO_ERROR),//
		FShedMem(open_or_create, aName.c_str(), read_write)//thank you the boost for User-friendly interface!!!
{
	VLOG(2)<<"Start to  initialize the ShM "<<aName<<" Size ="<<aSize;
	MInit(aName, aSize,true,aReserv);
}

CSharedMemory::CImpl::CImpl(const NSHARE::CText& aName,bool aIsCleanUp,size_t aReserv) :
		FIsInited(false), //
		FInfo(NULL), //
		FUserData(NULL), //
		FUserCb(NULL), //
		FAllocater(*this),//
		FError(E_NO_ERROR),//
		FShedMem(open_only, aName.c_str(), read_write)//thank you the boost for User-friendly interface!!!
{
	VLOG(2)<<"Start to  initialize the ShM "<<aName;
	MInit(aName, 0,aIsCleanUp,aReserv);
}
bool CSharedMemory::CImpl::MInit(const NSHARE::CText& aName, size_t aSize,bool aIsCleanUp,size_t aReserv)
{
	VLOG(2) << "Open shared memory " << aName<<" Size "<<aSize;
	FIsInited=false;
	FError=E_NO_ERROR;

	bool _is_new_header=false;
	bool _is_creator_exist=false;
	bool _was_exist=false;
	uint8_t* _r_addr=NULL;
	void* _allocate_to=NULL;

	{
		VLOG(2)<<"Initialize Shared memory "<<aName<<" path "<<FShedMem.get_name();
		offset_t _size = -1;
		bool const _is = FShedMem.get_size(_size);
		VLOG_IF(2, _is) << "Shared memory size " << _size;
		if (_is)
		{
			_was_exist=_size>0;
			if(!_was_exist)
			{
				if(aSize == 0)
				{
					VLOG(2) << "Cannot create ShM as is not exist " << aSize;
					FError=E_NO_EXIST;
					goto error;
				}else
				{
					LOG(INFO)<< "Create for the first time.";

					FShedMem.truncate(aSize);
					_is_new_header = true;
				}
			}else if(aSize > 0)
			{
				LOG(WARNING)<<"The Shm "<<aName<<" has been created already.";

				mapped_region const _region= mapped_region(FShedMem, read_only);

				uint8_t* _addr = (uint8_t*) _region.get_address();

				if (!_addr)
				{
					VLOG(2) << "shared memory is creating by the other process now ";
					FError=E_IS_USING;
					goto error;
				}

				mem_info_t const* _info = (mem_info_t const*) (_addr);
				const bool _is_valid =is_process_exist(_info->FPidOffCreator);

				if(_is_valid)
				{
					LOG(DFATAL)<<" The Shm "<<aName<<" is using by "<<_info->FPidOffCreator;
					FError=E_IS_USING;
					goto error;
				}
				else
				{
					VLOG(2)<<"Change size of SM "<<aSize;
					FShedMem.truncate(aSize);
					_is_new_header = true;
				}

			}
		}else if(aSize>0)
		{
			VLOG(2)<<"Change size of SM "<<aSize;
			FShedMem.truncate(aSize);
			_is_new_header = true;
		}
		FRegion = mapped_region(FShedMem, read_write);
		FRegion.flush();
		VLOG(2)<<"Region size "<<FRegion.get_size();
	}
	 _r_addr = (uint8_t*) FRegion.get_address();
	CHECK_NOTNULL(_r_addr);
	if(!_r_addr)
	{
		VLOG(2) << "Shared memory is creating by the other process now";
		FError=E_IS_USING;
		goto error;
	}

	FInfo = (mem_info_t*) (_r_addr);
	_allocate_to = _r_addr	+ sizeof(mem_info_t);

	if (_is_new_header)
	{
		VLOG(2)
				<< "The new shared memory " << aName << " is new.";

		FInfo = new (_r_addr) mem_info_t();
		FInfo->FSize = MGetSize();

		//creating shared mutex
		if (!FShareSem.MInit(FInfo->FSharedMutex,sizeof(FInfo->FSharedMutex), 1,
				CIPCSem::E_HAS_TO_BE_NEW)
				|| !FAllocaterSem.MInit(FInfo->FAllocMutex,sizeof(FInfo->FAllocMutex), 1,
						CIPCSem::E_HAS_TO_BE_NEW))
		{
			LOG(DFATAL) << "Cannot create the semaphores as it's exist in Shm. Thus cannot open Shm.";
			FError=E_CANNOT_CREATE_SEMAPHORE;
			goto error;
		}
		CRAII<CSharedMemory> _block(*this);
		FAllocater.FAllocater.MCreateHeap(_allocate_to,
				aSize - sizeof(mem_info_t), aReserv);


		FInfo->MUpdateCRC();
	}
	else
	{
		VLOG (2) << "The new shared memory " << aName << " is exist.";
		VLOG(2) << "Initialize shared memory head from " << _r_addr;
		const bool _is_valid = FInfo->MCheckCRC()
				&& (FInfo->FSize == MGetSize()) /*&& is_process_exist(FInfo->FPidOffCreator)*/;

		VLOG(1) << "Sm Valid = " << _is_valid;

		if (!_is_valid)
		{
			VLOG(2)<<"The ShM head is not valid "<<FInfo->FSize<<" "<<MGetSize()<<" crc="<<FInfo->FCrc;
			FError=E_SHM_ISNOT_VALID;
			goto error;
		}
		if (!FShareSem.MInit(FInfo->FSharedMutex,sizeof(FInfo->FSharedMutex), 1,
				CIPCSem::E_HAS_EXIST)
				|| !FAllocaterSem.MInit( FInfo->FAllocMutex,sizeof(FInfo->FAllocMutex), 1,
						CIPCSem::E_HAS_EXIST))
		{
			VLOG(2) << "Cannot init Sems";
			FError=E_CANNOT_INITIALIZE_SEMAPHORE;
			goto error;
		}
		MCleanUpShareSem();

		CRAII < CSharedMemory > _block(*this);

		if(!FAllocater.FAllocater.MInitFromCreatedHeap(_allocate_to,aIsCleanUp,aReserv))
		{
			VLOG(2) << "Cannot init allocator";
			_block.MUnlock();
			FError=E_SHM_ISNOT_VALID;
			goto error;
		}
	}

	VLOG(2)<<"The ShM is full valid";

	FIsInited = true;
	return true;

	//goto operator using for  decrease algorithm complexity
error:
	VLOG(2)<<"Clean up ShM";
	DCHECK_NE(FError,E_NO_ERROR);

	FShareSem.MFree();
	FAllocaterSem.MFree();
	if (aSize>0 && !_was_exist)
		FShedMem.remove(FShedMem.get_name());
	FInfo = NULL;
	FIsInited = false;

	return false;
}
CSharedMemory::CImpl::~CImpl()
{
	MFree();
}
/** Block to change memory info
 *
 * @return true if EOK
 */
bool CSharedMemory::CImpl::MLock() const
{

	bool _is = FShareSem.MWait();
	if (_is)
	{
		CHECK_NOTNULL (FInfo);
		const unsigned _pid = NSHARE::CThread::sMPid();
		VLOG(2) << "Lock Sem by " << _pid << "; previous="
				<< FInfo->FPIDOfLockedMutex;
		FInfo->FPIDOfLockedMutex = _pid;
		//FInfo->MUpdateCRC();
	}
	return _is;
}
/** Unblock to change memory info
 *
 * @return true if EOK
 */
bool CSharedMemory::CImpl::MUnlock() const
{
	CHECK_NOTNULL (FInfo);
	CHECK_EQ(FInfo->FPIDOfLockedMutex, NSHARE::CThread::sMPid());
	FInfo->FPIDOfLockedMutex = 0;
	return FShareSem.MPost();
}
/** Block to change data in memory
 *
 * @return true if EOK
 */
bool CSharedMemory::CImpl::MLockAlloc() const
{
	bool _is = FAllocaterSem.MWait();
	if (_is)
	{
		CHECK_NOTNULL(FInfo);
		if (FInfo)
		{
			const unsigned _pid = NSHARE::CThread::sMPid();
			VLOG(2) << "Lock Sem by " << _pid << "; previous="
				<< FInfo->FPIDOfLockedAllocMutex;
			FInfo->FPIDOfLockedAllocMutex = _pid;
		}
	}
	return _is;
}
/** Unblock to change data in memory
 *
 * @return true if EOK
 */
bool CSharedMemory::CImpl::MUnlockAlloc() const
{
	CHECK_NOTNULL(FInfo);
	if (FInfo)
	{
		CHECK_EQ(FInfo->FPIDOfLockedAllocMutex, NSHARE::CThread::sMPid());
		FInfo->FPIDOfLockedAllocMutex = 0;
	}
	return FAllocaterSem.MPost();
}
/** Close share memory
 *
 * @return true if has been removed
 */
bool CSharedMemory::CImpl::MFree()
{
	if (!FIsInited)
	{
		VLOG(2) << "Cannot free Shm";
		return false;
	}

	bool _is = false;
	{
		CRAII < CSharedMemory > _block(*this);
		if (FAllocater.FAllocater.MIsInited())
			_is = FAllocater.FAllocater.MReleaseHeap();
	}
	if (_is)
	{
		LOG(INFO) << "The allocator has been destroyed.";
		FShareSem.MFree();
		FAllocaterSem.MFree();
		_is=FShedMem.remove(FShedMem.get_name());
	}
	FInfo = NULL;
	FIsInited = false;
	return _is;
}

size_t CSharedMemory::CImpl::MGetSize()
{
	offset_t _val = 0;
	bool _is = FShedMem.get_size(_val);
	VLOG_IF(2, !_is) << "Cannot get size " << _val;
	if (_is)
		return _val;

	return 0;
}

bool CSharedMemory::CImpl::MCleanUpShareSem()
{
	unsigned _process = FInfo->FPIDOfLockedMutex;
	unsigned _count=0;
	if (_process)
		if (!is_process_exist(_process))
		{
			LOG(ERROR)<<"Process "<<_process<<"is not exist, but the mutex "<<FShareSem.MName()
			<<"still is locked by"<< FInfo->FPIDOfLockedMutex<<". Unlocking immediately ...";
			FInfo->FPIDOfLockedMutex= 0;
			FShareSem.MPost();
			++_count;
		}

	_process = FInfo->FPIDOfLockedAllocMutex;
	if (_process)
		if (!is_process_exist(_process))
		{
			LOG(ERROR)<<"Process "<<_process<<"is not exist, but the mutex "<<FAllocaterSem.MName()
			<<"still is locked by"<< FInfo->FPIDOfLockedAllocMutex<<". Unlocking immediately ...";
			FInfo->FPIDOfLockedAllocMutex= 0;
			FAllocaterSem.MPost();
			++_count;
		}
	return _count>0;
}
int CSharedMemory::CImpl::sMCleanUp(CSharedAllocator* WHO, void* aWHAT,
		void* YOU_DATA)
{
	CSharedMemory::CImpl* _p = (CSharedMemory::CImpl*) (YOU_DATA);
	CHECK_NOTNULL(_p);
	CSharedAllocator::clean_up_resources_t* _res =
			(CSharedAllocator::clean_up_resources_t*) (aWHAT);
	CSharedAllocator::clean_up_resources_t::const_iterator _it = _res->begin();
	for (; _it != _res->end(); ++_it)
	{
		if (_it->FPid == _p->FInfo->FPIDOfLockedMutex)
		{
			LOG(ERROR) << "The Mutex is locked.";
			_p->FInfo->FPIDOfLockedMutex = NSHARE::CThread::sMPid();
			_p->MUnlock();
		}
		if (_it->FPid == _p->FInfo->FPIDOfLockedAllocMutex)
		{
			LOG(ERROR) << "The Mutex is locked.";
			_p->FInfo->FPIDOfLockedAllocMutex = NSHARE::CThread::sMPid();
			_p->MUnlockAlloc();
		}
	}
	if (_p->FUserCb != NULL)
		return (_p->FUserCb)(WHO, aWHAT, _p->FUserData);

	return 0;
}

bool CSharedMemory::CImpl::MCleanUp(CSharedAllocator::clean_up_f_t WHO,
		void* YOU_DATA)
{
	FUserCb = WHO;
	FUserData = YOU_DATA;
	return FAllocater.FAllocater.MCleanUpResourceByWatchDog(&sMCleanUp, this);
}
}

