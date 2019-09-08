// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCSemaphoreSemOpen.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#if (defined(__QNX__)||defined(unix))

#include <deftype>
#include <limits.h>
#include <semaphore.h>
#include <fcntl.h>
#include <UType/CIPCSem.h>
#include "CIPCSemaphoreSemOpen.h"

namespace NSHARE
{
extern size_t get_unique_name(char const* aPreifix,uint8_t* aTo,size_t aSize);

namespace impl
{
CIPCSemaphoreSemOpen::CIPCSemaphoreSemOpen() :
		FSem(SEM_FAILED ),//
		FType(CIPCSem::CIPCSem::E_UNDEF)
{
	;
}
bool CIPCSemaphoreSemOpen::MInit(uint8_t* aBuf, size_t aSize, unsigned int aInitvalue,
		CIPCSem::CIPCSem::eOpenType  aHasToBeNew)
{
	DCHECK_NOTNULL(aBuf);
	DCHECK_EQ(FSem, SEM_FAILED);

	void *const _p=memchr(aBuf,'\0',aSize);
	bool const _is_empty=_p== NULL || _p==aBuf;

	if(aHasToBeNew==CIPCSem::CIPCSem::E_UNDEF && _is_empty)
	{
		VLOG(2)<<"The buffer is empty. Create the mutex";
		aHasToBeNew=CIPCSem::E_HAS_TO_BE_NEW;
	}

	DCHECK_LE(aInitvalue, CIPCSem::MAX_VALUE);

	switch (aHasToBeNew)
	{
	case CIPCSem::E_HAS_TO_BE_NEW:
	{

		get_unique_name("/sem",aBuf, aSize);

		int const oflags = O_CREAT | O_EXCL;
		FSem = sem_open((char*)aBuf, oflags, 0666, aInitvalue);
		if (FSem == SEM_FAILED)
		{
			LOG(ERROR) << aBuf << " has not created as error " << strerror(errno) << "(" << errno << ")";
			if (errno == EEXIST)
			{
				VLOG(2) << "The IPC mutex  is exist.";
			}
			return false;
		}
		break;
	}
	case CIPCSem::E_HAS_EXIST:
	{
		if(_is_empty)
		{
			LOG(ERROR)<<"The buffer is empty.";
			return false;
		}
		int const 	oflags = 0;
		FSem = sem_open((char*)aBuf, oflags);//exist
		if (FSem == SEM_FAILED)
		{
			LOG(ERROR) << aBuf << " has not created as error " << strerror(errno) << "(" << errno << ")";
			//CHECK_NE(errno, EEXIST);
			return false;
		}
		break;
	}

	case CIPCSem::CIPCSem::E_UNDEF:
	{
		aHasToBeNew = CIPCSem::E_HAS_TO_BE_NEW;
		int oflags = O_CREAT | O_EXCL;
		FSem = sem_open((char*)aBuf, oflags, 0666, aInitvalue);
		if (FSem == SEM_FAILED && errno == EEXIST)
		{
			VLOG(2) << "It's exist";
			aHasToBeNew = CIPCSem::E_HAS_EXIST;
			oflags = O_CREAT;
			FSem = sem_open((char*)aBuf, oflags, 0666, aInitvalue);
		}
		if (FSem == SEM_FAILED)
		{
			LOG(ERROR) << aBuf << " has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}
		break;
	}
	}

	FType = aHasToBeNew;
	FName=(char*)aBuf;
	return true;
}
CIPCSemaphoreSemOpen::~CIPCSemaphoreSemOpen()
{
	MFree();
}
bool CIPCSemaphoreSemOpen::MWait(void)
{
	VLOG(2) << "Wait for mutex(lock) " << MName();
	DCHECK_NE(FSem, SEM_FAILED);
	bool _is = sem_wait(FSem) == 0;
	DVLOG(2) << "Sem " << MName() << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSemaphoreSemOpen::MWait(double const aTime)
{
	DVLOG(2) << "Wait for mutex(lock) " << MName()<<" Time = "<<aTime;
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
	DCHECK_NE(FSem, SEM_FAILED);

	bool _is = sem_timedwait(FSem,&tm) == 0;
	DVLOG(2) << "Sem " << MName() << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSemaphoreSemOpen::MTryWait(void)
{
	VLOG(2) << "Try Wait for mutex " << MName();
	DCHECK_NE(FSem, SEM_FAILED);
	bool _is = sem_trywait(FSem) == 0;
	VLOG(2) << "Sem " << MName() << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSemaphoreSemOpen::MPost(void)
{
	VLOG(2) << "Post mutex(unlock) " << MName();
	DCHECK_NE(FSem, SEM_FAILED);
	bool _is = sem_post(FSem) == 0;
	VLOG(2) << "Sem " << MName() << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
int CIPCSemaphoreSemOpen::MValue() const
{
	DCHECK_NE(FSem, SEM_FAILED);
	int __value = -1;
	bool _is = sem_getvalue(FSem,&__value) == 0;
	VLOG(2) << "Sem " << MName() << " hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is?__value:-1;
}
void CIPCSemaphoreSemOpen::MUnlink()
{
	if(!MName().empty())
		sem_unlink(MName().c_str());
}
bool CIPCSemaphoreSemOpen::MIsInited() const
{
	return FSem != SEM_FAILED;
}
void CIPCSemaphoreSemOpen::MFree()
{
	if(!MIsInited())
		return;
	DCHECK_NE(FSem, SEM_FAILED);
	bool _is = sem_close(FSem) == 0;
	DVLOG(2) << "Sem " << MName() << " freed.";
	DLOG_IF(ERROR,!_is) << "Free error " << strerror(errno)<<"("<<errno<<")";
	(void)_is;
	FSem= SEM_FAILED;
	FName.clear();
	FType = CIPCSem::CIPCSem::E_UNDEF;
}
NSHARE::CText const& CIPCSemaphoreSemOpen::MName() const
{
	return FName;
}
}
} /* namespace NSHARE */

#endif
