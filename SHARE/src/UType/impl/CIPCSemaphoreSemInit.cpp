// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCSemaphoreSemInit.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#if  defined(__QNX__)||defined(unix)
#include <limits.h>
#include <semaphore.h>
#include <fcntl.h>
#include <UType/CIPCSem.h>
#include "CIPCSemaphoreSemInit.h"

namespace NSHARE
{
namespace impl
{
COMPILE_ASSERT((sizeof(CIPCSemaphoreSemInit::_sem_t))<=CIPCSem::eReguredBufSize,InvalidSizeSem_t);
CIPCSemaphoreSemInit::_sem_t::_sem_t()
{
	memset(this,0,sizeof(*this));
}
bool CIPCSemaphoreSemInit::_sem_t::MIsValid() const
{
	return FCount > 0 && !FIsUnlinked;
}
void CIPCSemaphoreSemInit::_sem_t::MInc()
{
	++FCount;
}
bool CIPCSemaphoreSemInit::_sem_t::MDec()
{
	return --FCount==0;
}
inline void CIPCSemaphoreSemInit::_sem_t::MCreatedNew()
{
	FIsUnlinked=false;
	FCount=0;
	FId=get_uuid().MGetHash();
}
CIPCSemaphoreSemInit::CIPCSemaphoreSemInit() ://
		FSem(NULL),
		FType(CIPCSem::CIPCSem::E_UNDEF)
{
	;
}
bool CIPCSemaphoreSemInit::MInit(uint8_t* aBuf, size_t aSize, unsigned int aInitvalue,
		CIPCSem::eOpenType  aHasToBeNew)
{
	DCHECK_NOTNULL(aBuf);
	DCHECK_EQ(FSem, NULL);
	FSem=(_sem_t*)get_alignment_address<sem_t>(aBuf);
	DCHECK_LE((void*)(FSem+1),(void*)(aBuf+aSize));

	DCHECK_LE(aInitvalue, ISemaphore::MAX_VALUE);

	switch (aHasToBeNew)
	{
	case CIPCSem::E_HAS_TO_BE_NEW:
	{

		if (FSem->MIsValid() || sem_init(&FSem->FSem, 1, aInitvalue)!=0)
		{
			if(FSem->MIsValid())
				errno=EEXIST;
			FSem = NULL;
			LOG(ERROR) << "Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}else
			FSem->MCreatedNew();
		break;
	}
	case CIPCSem::E_HAS_EXIST:
	{
		int _val = 0;
		//FSem = ;//exist
		if (!FSem->MIsValid() ||sem_getvalue(&FSem->FSem, &_val)!=0)
		{
			if(!FSem->MIsValid())
				errno=ENOENT;
			FSem = NULL;
			LOG(ERROR) << " Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}
		break;
	}

	case CIPCSem::E_UNDEF:
	{
		aHasToBeNew = CIPCSem::E_HAS_EXIST;
		int _val = 0;
		if (!FSem->MIsValid() || sem_getvalue(&FSem->FSem, &_val)!=0)
		{
			aHasToBeNew = CIPCSem::E_HAS_TO_BE_NEW;

			if (sem_init(&FSem->FSem, 1, aInitvalue)!=0)
			{
				FSem = NULL;
				LOG(ERROR) << "Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
				return false;
			}else
				FSem->MCreatedNew();
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
CIPCSemaphoreSemInit::~CIPCSemaphoreSemInit()
{
	MFree();
}
bool CIPCSemaphoreSemInit::MWait(void)
{
	DVLOG(2) << "Wait for mutex(lock) " << FName;

	DCHECK_NOTNULL(FSem);
	int _rval=sem_wait(&FSem->FSem);

	VLOG(2) << "Sem " << FName << " hold.";
	DLOG_IF(DFATAL,_rval!=0) << "Look error "<<FName<<" " << strerror(errno)<<"("<<errno<<")";
	return _rval==0;
}
bool CIPCSemaphoreSemInit::MWait(double const aTime)
{
	VLOG(2) << "Wait for mutex(lock) " << FName<<" Time = "<<aTime;
	DCHECK_NOTNULL(FSem);

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

	bool _is = sem_timedwait(&FSem->FSem,&tm) == 0;
	DVLOG(2) << "Sem " << FName << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSemaphoreSemInit::MTryWait(void)
{
	VLOG(2) << "Try Wait for mutex " << FName;

	DCHECK_NOTNULL(FSem);

	bool _is = sem_trywait(&FSem->FSem) == 0;
	VLOG(2) << "Sem " << FName << " hold.";
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSemaphoreSemInit::MPost(void)
{
	VLOG(2) << "Post mutex(unlock) " << FName;

	DCHECK_NOTNULL(FSem);

	int _rval=0;
	_rval=sem_post(&FSem->FSem);

	VLOG(2) << "Sem " << FName << " hold.";
	LOG_IF(DFATAL,_rval!=0) << "Look error "<<FName<<" " << strerror(errno)<<"("<<errno<<")";
	return _rval==0;
}
int CIPCSemaphoreSemInit::MValue() const
{
	DCHECK_NOTNULL(FSem);
	int __value = -1;
	bool _is = sem_getvalue(&FSem->FSem,&__value) == 0;
	DVLOG(2) << "Sem " << FName << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is?__value:-1;
}
void CIPCSemaphoreSemInit::MUnlink()
{
	if(!MIsInited())
			return;
	bool const _is = sem_destroy(&FSem->FSem) == 0;
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	if(_is)
		FSem->FIsUnlinked= true;
}
bool CIPCSemaphoreSemInit::MIsInited() const
{
	return FSem != NULL;
}
void CIPCSemaphoreSemInit::MFree()
{
	if(!MIsInited())
		return;
	if(FSem->MDec())
	{
		MUnlink();
	}
	{
		FSem = NULL;
		FType = CIPCSem::E_UNDEF;
		FName.clear();
	}
}
NSHARE::CText const& CIPCSemaphoreSemInit::MName() const
{
	return FName;
}
}
}
#endif
