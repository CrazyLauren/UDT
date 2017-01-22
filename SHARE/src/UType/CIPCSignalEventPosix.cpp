/*
 * CIPCSignalEventPosix.cpp
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 24.06.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#if   defined(__QNX__)||defined(unix)

#include <deftype>
#include <UType/CIPCSignalEvent.h>
#include <semaphore.h>
#include <fcntl.h>
namespace NSHARE
{
struct _sem_t
{
	inline _sem_t();
	sem_t FSem;
	uint16_t FCount;
	uint16_t FIsUnlinked:1;
	uint16_t :15;
	uint32_t FId;

	inline bool MIsValid() const;
	inline void MInc();
	inline bool MDec();
	inline void MCreatedNew();
};
_sem_t::_sem_t()
{
	memset(this,0,sizeof(*this));
}
bool _sem_t::MIsValid() const
{
	return FCount > 0 && !FIsUnlinked;
}
void _sem_t::MInc()
{
	++FCount;
}
bool _sem_t::MDec()
{
	return --FCount==0;
}
inline void _sem_t::MCreatedNew()
{
	FIsUnlinked=false;
	FCount=0;
	FId=get_uuid().MGetHash();
}
size_t CIPCSignalEvent::sMRequredBufSize()
{
	return sizeof(_sem_t);
}
struct CIPCSignalEvent::CImpl
{
	mutable _sem_t* FSignalEvent;
	eOpenType FType;
	CText FName;

	CImpl() :
			FSignalEvent(NULL),FType(E_UNDEF)
	{

	}
};
CIPCSignalEvent::CIPCSignalEvent() :
		FPImpl(new CImpl)
{

}
CIPCSignalEvent::CIPCSignalEvent(uint8_t* aBuf, size_t aSize, eOpenType aIsNew) :
		FPImpl(new CImpl)
{
	bool const _is=MInit(aBuf,aSize, aIsNew);
	if(!_is)
		MASSERT_1(false);
}
bool CIPCSignalEvent::MInit(uint8_t* aBuf, size_t aSize, eOpenType aHasToBeNew)
{
	CHECK_NOTNULL(FPImpl);
	if (MIsInited())
	{
		LOG(ERROR)<<"Cannot init ipc se";
		return false;
	}

	if(aSize<sMRequredBufSize())
	{
		LOG(DFATAL)<<"Invalid size of buf "<<aSize<<" min size "<<sMRequredBufSize();
		return false;
	}

	CHECK_NOTNULL(aBuf);
	CHECK_EQ(FPImpl->FSignalEvent,NULL);
	FPImpl->FSignalEvent=(_sem_t*)aBuf;

	switch (aHasToBeNew)
	{
	case E_HAS_TO_BE_NEW:
	{

		if (FPImpl->FSignalEvent->MIsValid() || sem_init(&FPImpl->FSignalEvent->FSem, 1, 0)!=0)
		{
			if(FPImpl->FSignalEvent->MIsValid())
				errno=EEXIST;
			FPImpl->FSignalEvent = NULL;
			LOG(ERROR) <<"Signal event has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}else
			FPImpl->FSignalEvent->MCreatedNew();

		break;
	}
	case E_HAS_EXIST:
	{
		int _val = 0;
		//FImpl->FSem = ;//exist
		if ( !FPImpl->FSignalEvent->MIsValid() || sem_getvalue(&FPImpl->FSignalEvent->FSem, &_val)!=0)
		{
			if(!FPImpl->FSignalEvent->MIsValid())
				errno=ENOENT;
			FPImpl->FSignalEvent = NULL;
			LOG(ERROR)<<"Signal event has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}
		break;
	}

	case E_UNDEF:
	{
		aHasToBeNew = E_HAS_EXIST;
		int _val = 0;
		if (!FPImpl->FSignalEvent->MIsValid() || sem_getvalue(&FPImpl->FSignalEvent->FSem, &_val)!=0)
		{
			aHasToBeNew = E_HAS_TO_BE_NEW;

			if (sem_init(&FPImpl->FSignalEvent->FSem, 1, 0)!=0)
			{
				FPImpl->FSignalEvent = NULL;
				LOG(ERROR) << " Signal event  has not created as error " << strerror(errno) << "(" << errno << ")";
				return false;
			}else
				FPImpl->FSignalEvent->MCreatedNew();
		}
		break;
	}
	}
	FPImpl->FSignalEvent->MInc();
	FPImpl->FType=aHasToBeNew;
	char _str[16];
	sprintf(_str,"%u",FPImpl->FSignalEvent->FId);
	FPImpl->FName=_str;
	return true;
}
void CIPCSignalEvent::MFree()
{
	CHECK_NOTNULL(FPImpl);
	if(!MIsInited())
		return;
/*	bool _is = sem_close(FPImpl->FSignalEvent) == 0;
	VLOG(2) << "Event " << MName() << " hold.";
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	(void)_is;*/
	if(FPImpl->FSignalEvent->MDec())
	{
		MUnlink();
	}

	{
		FPImpl->FSignalEvent = NULL;
		FPImpl->FType = E_UNDEF;
		FPImpl->FName.clear();
	}
}
void CIPCSignalEvent::MUnlink()
{
	CHECK_NOTNULL(FPImpl);
	if(!MIsInited())
		return;
	CHECK_NOTNULL(FPImpl->FSignalEvent);
	bool const _is = sem_destroy(&FPImpl->FSignalEvent->FSem) == 0;
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	if(_is)
		FPImpl->FSignalEvent->FIsUnlinked=true;
}
CIPCSignalEvent::~CIPCSignalEvent()
{
	MFree();
	delete FPImpl;
}
//bool CIPCSignalEvent::MTimedwait(CIPCSem * aMutex, const struct timespec* aVal)
//{
//	VLOG(2) << "Event " << FName << " is waited for.";
//	CHECK_NOTNULL(FPImpl);
//	LOG_IF(DFATAL,FPImpl->FSignalEvent==INVALID_HANDLE_VALUE) << "Event "
//																		<< FName
//																		<< " is not init.";
//	if (FPImpl->FSignalEvent == INVALID_HANDLE_VALUE)
//		return false;
//	unsigned msec = INFINITE;
//	if (aVal)
//	{
//		msec = aVal->tv_sec * 1000 + aVal->tv_nsec / 1000 / 1000;
//	}
//	aMutex->MPost();
//	DWORD ret;
//	bool _is_timeout = false;
//	for (HANG_INIT;;HANG_CHECK)
//	{
//		ret = ::WaitForSingleObject(FPImpl->FSignalEvent, msec);
//		_is_timeout=ret == WAIT_TIMEOUT;
//		if(ret == WAIT_OBJECT_0 ||ret == WAIT_TIMEOUT)
//		break;
//	}
//	bool _is = aMutex->MWait();
//	CHECK(_is);
//	(void) _is;
//	VLOG(2) << "Event " << FName << " has been singaled.";
//	return !_is_timeout;
//}
bool CIPCSignalEvent::MTimedwait(CIPCSem * aMutex, const struct timespec* aVal)
{
	if(aVal)
		return MTimedwait(aMutex,aVal->tv_sec+((double)aVal->tv_nsec)/1000.0/1000.0/1000.0);
	VLOG(2) << "Event " << MName()<< " is waited for.";

	CHECK_NOTNULL(FPImpl);
	CHECK_NOTNULL(FPImpl->FSignalEvent);


	aMutex->MPost();
	bool _is = sem_wait(&FPImpl->FSignalEvent->FSem) == 0;
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	aMutex->MWait();

	VLOG(2) << "Sem " << MName() << " hold.";

	return _is;
}
bool CIPCSignalEvent::MTimedwait(CIPCSem *aMutex, double const aTime)
{
	CHECK_GE(aTime,0);
	CHECK_LE(aTime,std::numeric_limits<unsigned>::max());
	VLOG(2) << "Event  TimedWait " << MName()<<" Time = "<<aTime;
	struct timespec tm;
	if (clock_gettime(CLOCK_REALTIME, &tm) < 0)
	{
		LOG(DFATAL)<<"Clock reatime error "<<errno;
		return false;
	}
	add(&tm, aTime);
	CHECK_NOTNULL(FPImpl);
	CHECK_NOTNULL(FPImpl->FSignalEvent);

	aMutex->MPost();
	bool _is = sem_timedwait(&FPImpl->FSignalEvent->FSem,&tm) == 0;
	aMutex->MWait();

	VLOG(2) << "Sem " << MName() << " hold.";
	LOG_IF(ERROR,!_is) << "Event Wait error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}

bool CIPCSignalEvent::MSignal()
{
	CHECK_NOTNULL(FPImpl);
	VLOG(2) << "Event " << MName() << " will signaled.";
	bool _is = sem_post(&FPImpl->FSignalEvent->FSem) == 0;
	VLOG(2) << "Event " << MName() << " is singaled.";
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
NSHARE::CText const& CIPCSignalEvent::MName() const
{
	CHECK_NOTNULL(FPImpl);
	return FPImpl->FName;
}
bool CIPCSignalEvent::MIsInited() const
{
	return FPImpl->FSignalEvent != NULL ;
}
} /* namespace NSHARE */
#endif



