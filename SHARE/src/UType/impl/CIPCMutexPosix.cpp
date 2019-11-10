// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCMutex.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#if  defined(__QNX__)||defined(unix)
#include <deftype>
#include <limits.h>
#include <semaphore.h>
#include <fcntl.h>
#include <SHARE/UType/CIPCMutex.h>

namespace NSHARE
{
struct CIPCMutex::CImpl
{
	/** it's stored in shared memory
	 *
	 */
	struct data_t
	{
		pthread_mutex_t FPMutex;
		uint16_t FCount;//!< The number of "user"
		uint16_t FIsUnlinked:1;
		uint16_t :15;
		uint32_t FId;

		inline bool MIsValid() const;
		inline void MInc();
		inline bool MDec();
		inline void MCreatedNew();
	};

	CImpl():FData(NULL)
	{

	}
	CImpl(uint8_t* aBuf, size_t aSize,CIPCMutex::eOpenType const aType) :
		FData(NULL)
	{
	}
	data_t* FData;//!< Pointer to data in shared memory
	CText FName;//!< Name of mutex
};
namespace
{
COMPILE_ASSERT((__alignof(CIPCMutex::CImpl::data_t))<=__alignof(void*),InvalidAligOnData_t);
COMPILE_ASSERT((sizeof(CIPCMutex::CImpl::data_t))<=CIPCMutex::eReguredBufSize,InvalidSizeMutex_t);
COMPILE_ASSERT(sizeof(CIPCMutex::CImpl::data_t) ==(sizeof(uint32_t)*2//
		+sizeof(pthread_mutex_t)//
		),IVALID_SIZEOF_DATA);
}
bool CIPCMutex::CImpl::data_t::MIsValid() const
{
	return FCount > 0 && !FIsUnlinked;
}
void CIPCMutex::CImpl::data_t::MInc()
{
	++FCount;
}
bool CIPCMutex::CImpl::data_t::MDec()
{
	return --FCount==0;
}
inline void CIPCMutex::CImpl::data_t::MCreatedNew()
{
	FIsUnlinked=false;
	FCount=0;
	FId=get_uuid().MGetHash();
}
CIPCMutex::CIPCMutex() ://
		FImpl(new CImpl),
		FType(CIPCMutex::E_UNDEF)
{
	;
}
CIPCMutex::CIPCMutex(uint8_t* aBuf, size_t aSize,
		eOpenType const aHasToBeNew) :
		FImpl(new CImpl),//
		FType(E_UNDEF)
{
	bool const _is=MInit(aBuf,aSize, aHasToBeNew);
	DCHECK(_is);
}
bool CIPCMutex::MInit(uint8_t* aBuf, size_t aSize, 	CIPCMutex::eOpenType  aHasToBeNew)
{
	DCHECK_NOTNULL(aBuf);

	FImpl->FData=(CImpl::data_t*)get_alignment_address<CImpl::data_t>(aBuf);
	DCHECK_LE((void*)(FImpl->FData+1),(void*)(aBuf+aSize));
	CImpl::data_t &_data=*FImpl->FData;

	pthread_mutexattr_t attr;
	int r = pthread_mutexattr_init(&attr);
	if(r != 0)
		goto error;


	r = pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if(r != 0)
		goto error;

	switch (aHasToBeNew)
	{
	case CIPCMutex::E_HAS_TO_BE_NEW:
	{
		if(_data.MIsValid())
		{
			errno=EEXIST;
			goto error;
		}
		r = pthread_mutex_init(&(_data.FPMutex), &attr);

		if (r != 0)
			goto error;
		else
			_data.MCreatedNew();
		break;
	}
	case CIPCMutex::E_HAS_EXIST:
	{
		if (!_data.MIsValid())
		{
			errno=ENOENT;
			goto error;
		}
		break;
	}

	case CIPCMutex::E_UNDEF:
	{
		aHasToBeNew = CIPCMutex::E_HAS_EXIST;
		if (!_data.MIsValid())
		{
			aHasToBeNew = CIPCMutex::E_HAS_TO_BE_NEW;
			r = pthread_mutex_init(&(_data.FPMutex), &attr);

			if (r!=0)
				goto error;
			else
				_data.MCreatedNew();
		}
		break;
	}
	}
	pthread_mutexattr_destroy(&attr);
	_data.MInc();

	FType = aHasToBeNew;
	char _str[16];
	sprintf(_str,"%u",_data.FId);
	FImpl->FName=_str;
	return true;
error:
 	LOG(ERROR) << "Mutex has not created as it exist" << strerror(errno) << "(" << errno << ")";
 	pthread_mutexattr_destroy(&attr);

 	FImpl->FData = NULL;
	FType = CIPCMutex::E_UNDEF;
	FImpl->FName.clear();

	return false;
}
CIPCMutex::~CIPCMutex()
{
	MFree();
	delete FImpl;
}
void CIPCMutex::MFree()
{
	if(!MIsInited())
		return;
	if(FImpl->FData->MDec())
	{
		MUnlink();
	}
	{
		FImpl->FData = NULL;
		FType = CIPCMutex::E_UNDEF;
		FImpl->FName.clear();
	}
}
void CIPCMutex::MUnlink()
{
	if(!MIsInited())
		return;
	bool const _is = pthread_mutex_destroy(&FImpl->FData->FPMutex) == 0;
	DLOG_IF(ERROR,!_is) << "Unllink error " << strerror(errno)<<"("<<errno<<")";
	if(_is)
	{
		FImpl->FData->FIsUnlinked= true;
	}
}

bool CIPCMutex::MLock(void)
{
	DVLOG(2) << "Wait for mutex(lock) " << FImpl->FName;

	DCHECK_NOTNULL(FImpl->FData);
	bool _rval = !pthread_mutex_lock(&FImpl->FData->FPMutex);
	return _rval;
}
bool CIPCMutex::MUnlock()
{
	DVLOG(2) << "Wait for mutex(lock) " << FImpl->FName;
	DCHECK_NOTNULL(FImpl->FData);

	return !pthread_mutex_unlock(&FImpl->FData->FPMutex);
}
bool CIPCMutex::MCanLock(void)
{
	VLOG(2) << "Try Wait for mutex " << FImpl->FName;

	DCHECK_NOTNULL(FImpl->FData);

	bool _is= !pthread_mutex_trylock(&FImpl->FData->FPMutex);
	return _is;
}
void* CIPCMutex::MGetPtr() const
{
	return &FImpl->FData->FPMutex;
}

bool CIPCMutex::MIsInited() const
{
	return FImpl->FData != NULL;
}

NSHARE::CText const& CIPCMutex::MName() const
{
	return FImpl->FName;
}
CIPCMutex::eMutexType CIPCMutex::MGetMutexType() const
{
	return MUTEX_NORMAL;
}
}
#endif
