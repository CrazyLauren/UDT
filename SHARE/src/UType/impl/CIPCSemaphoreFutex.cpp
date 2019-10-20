// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCSemaphoreFutex.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifdef HAVE_FUTEX_H
#include <deftype>
#include <limits.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <share/UType/CIPCSem.h>
#include "CIPCSemaphoreFutex.h"

namespace NSHARE
{
namespace impl
{
COMPILE_ASSERT(
		(sizeof(CIPCSemaphoreFutex::_sem_t) + __alignof(int32_t))
				<= CIPCSem::eReguredBufSize, InvalidSizeSem_t);
CIPCSemaphoreFutex::_sem_t::_sem_t()
{
	memset(this,0,sizeof(*this));
}
bool CIPCSemaphoreFutex::_sem_t::MIsValid() const
{
	return FCount > 0 && !FIsUnlinked;
}
void CIPCSemaphoreFutex::_sem_t::MInc()
{
	++FCount;
}
bool CIPCSemaphoreFutex::_sem_t::MDec()
{
	return --FCount==0;
}
inline void CIPCSemaphoreFutex::_sem_t::MCreatedNew(int aVal)
{
	FIsUnlinked=false;
	FCount=0;
	FId=get_uuid().MGetHash();
	FFutex=aVal;
}
CIPCSemaphoreFutex::CIPCSemaphoreFutex()://
		FSem(NULL)
{
	;
}
bool CIPCSemaphoreFutex::MInit(uint8_t* aBuf, size_t aSize, unsigned int aInitvalue,
		CIPCSem::eOpenType  aHasToBeNew)
{
	DCHECK_NOTNULL(aBuf);
	DCHECK_EQ(FSem, NULL);
	FSem=(_sem_t*)(((uintptr_t)aBuf+4)& ~(4-1));

	DCHECK_EQ((uintptr_t)FSem%4,0);

	DCHECK_LE((void*)(FSem+1),(void*)(aBuf+aSize));

	DCHECK_LE(aInitvalue, ISemaphore::MAX_VALUE);

	switch (aHasToBeNew)
	{
	case CIPCSem::E_HAS_TO_BE_NEW:
	{

		if (FSem->MIsValid())
		{
			errno=EEXIST;
			FSem = NULL;
			LOG(ERROR) << "Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}else
			FSem->MCreatedNew(aInitvalue);
		break;
	}
	case CIPCSem::E_HAS_EXIST:
	{
		//FSem = ;//exist
		if (!FSem->MIsValid())
		{
			errno=ENOENT;
			FSem = NULL;
			LOG(ERROR) << " Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}
		break;
	}

	case CIPCSem::CIPCSem::E_UNDEF:
	{
		aHasToBeNew = CIPCSem::E_HAS_EXIST;
		if (!FSem->MIsValid())
		{
			aHasToBeNew = CIPCSem::E_HAS_TO_BE_NEW;
			FSem->MCreatedNew(aInitvalue);
		}
		break;
	}
	}

	FSem->MInc();
	FType = aHasToBeNew;
	char _str[16];
	sprintf(_str,"%u",FSem->FId);
	FName=_str;
	return true;
}
CIPCSemaphoreFutex::~CIPCSemaphoreFutex()
{
	MFree();
}
bool CIPCSemaphoreFutex::MWait(void)
{
	VLOG(2) << "Wait for mutex(lock) " << FName;
	DCHECK_NOTNULL(FImpl);
	DCHECK_NOTNULL(FSem);
    int _rval=-1;
    bool  _is =false;
    do {

          /* Is the futex available? */

          if ((_is=__sync_bool_compare_and_swap(&FSem->FFutex, 1, 0)))
              break;      /* Yes */

          /* Futex is not available; wait */

          _rval = syscall(SYS_futex,&FSem->FFutex, FUTEX_WAIT, 0, NULL, NULL, 0);

     }while(!_is && (_rval != -1 || errno == EAGAIN));

	VLOG(2) << "Sem " << FName << " hold.";
	LOG_IF(DFATAL,!_is) << "Look error "<<FName<<" " << strerror(errno)<<"("<<errno<<") rval="<<_rval;
	return _is;
}
bool CIPCSemaphoreFutex::MWait(double const aTime)
{
	VLOG(2) << "Wait for mutex(lock) " << FName<<" Time = "<<aTime;
	struct timespec tm;
	if (clock_gettime(CLOCK_REALTIME, &tm) < 0)
	{
		LOG(DFATAL)<<"Clock reatime error "<<errno;
		return false;
	}
	uint64_t _time=tm.tv_nsec+ static_cast<uint64_t>((aTime - (unsigned)aTime) * 1000.0 * 1000.0 * 100.0);

	static const unsigned _nano_time=1000*1000*1000;
	tm.tv_sec += static_cast<unsigned>(aTime)+(_time/_nano_time);
	tm.tv_nsec = (_time - (_time/_nano_time));
	DCHECK_NOTNULL(FSem);

	int _rval;
	bool  _is =false;
    do {

          /* Is the futex available? */

          if ((_is=__sync_bool_compare_and_swap(&FSem->FFutex, 1, 0)))
              break;      /* Yes */

          /* Futex is not available; wait */

          _rval = syscall(SYS_futex,&FSem->FFutex, FUTEX_WAIT, 0, &tm, NULL, 0);

    }while(!_is && (_rval != -1 || errno == EAGAIN));

	VLOG(2) << "Sem " << FName << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSemaphoreFutex::MTryWait(void)
{
	VLOG(2) << "Try Wait for mutex " << FName;
	DCHECK_NOTNULL(FSem);
	bool const _is = __sync_bool_compare_and_swap(&FSem->FFutex, 1, 0);
	VLOG(2) << "Sem " << FName << " hold.";
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSemaphoreFutex::MPost(void)
{
	VLOG(2) << "Post mutex(unlock) " << FName;
	DCHECK_NOTNULL(FSem);
	int _rval=0;
	if (__sync_bool_compare_and_swap(&FSem->FFutex, 0, 1))
	{

		_rval = syscall(SYS_futex, &FSem->FFutex, FUTEX_WAKE, 1, NULL, NULL, 0);
	}
	VLOG(2) << "Sem " << FName << " hold.";
	LOG_IF(DFATAL,_rval==-1) << "Look error "<<FName<<" " << strerror(errno)<<"("<<errno<<")"<<" rval="<<_rval;
	return _rval != -1;
}
int CIPCSemaphoreFutex::MValue() const
{
	DCHECK_NOTNULL(FImpl);
	DCHECK_NOTNULL(FSem);
	return FSem->FFutex;
}
void CIPCSemaphoreFutex::MUnlink()
{
}
bool CIPCSemaphoreFutex::MIsInited() const
{
	return FSem != NULL;
}
void CIPCSemaphoreFutex::MFree()
{
	if(!MIsInited())
		return;
	if(FSem->MDec())
	{
		MUnlink();
	}
	{
		FSem = NULL;
		FType = CIPCSem::CIPCSem::E_UNDEF;
		FName.clear();
	}
}
NSHARE::CText const& CIPCSem::MName() const
{
	return FName;
}
}
}
#endif

