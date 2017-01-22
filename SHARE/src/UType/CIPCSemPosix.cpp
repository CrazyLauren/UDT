/*
 * CIPCSemPosix.cpp
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
size_t CIPCSem::sMRequredBufSize()
{
	return sizeof(_sem_t);
}

struct CIPCSem::CImpl
{
	CImpl() :
			FSem(NULL)
	{
	}
	mutable _sem_t* FSem;
	NSHARE::CText  FName;
};
int const CIPCSem::MAX_VALUE=SEM_VALUE_MAX;
CIPCSem::CIPCSem(uint8_t* aBuf, size_t aSize, unsigned int value,
		eOpenType const aHasToBeNew, int aInitvalue) :
		FImpl(new CImpl),//
		FType(E_UNDEF)
{
	bool const _is=MInit(aBuf,aSize, value, aHasToBeNew, aInitvalue);
	if(!_is)
		MASSERT_1(false);
}
CIPCSem::CIPCSem() :
		FImpl(new CImpl),//
		FType(E_UNDEF)
{
	;
}
bool CIPCSem::MInit(uint8_t* aBuf, size_t aSize, unsigned int value,
		eOpenType  aHasToBeNew, int aInitvalue)
{
	CHECK_NOTNULL(FImpl);
	if (MIsInited())
	{
		LOG(ERROR)<<"Cannot init ipc sem";
		return false;
	}
	
	if (aInitvalue < 0)
		aInitvalue = value;
	
	VLOG(2) << "Mutex  is initialized.";
	
	if(aSize<sMRequredBufSize())
	{
		LOG(DFATAL)<<"Invalid size of buf "<<aSize<<" min size "<<sMRequredBufSize();
		return false;
	}
	
	CHECK_NOTNULL(FImpl);
	CHECK_NOTNULL(aBuf);
	CHECK_EQ(FImpl->FSem, NULL);
	FImpl->FSem=(_sem_t*)aBuf;

	CHECK_LE(value, SEM_VALUE_MAX);

	switch (aHasToBeNew)
	{
	case E_HAS_TO_BE_NEW:
	{
		int const oflags = O_CREAT | O_EXCL;
		
		if (FImpl->FSem->MIsValid() || sem_init(&FImpl->FSem->FSem, 1, aInitvalue)!=0)
		{
			if(FImpl->FSem->MIsValid())
				errno=EEXIST;
			FImpl->FSem = NULL;
			LOG(ERROR) << "Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}else
			FImpl->FSem->MCreatedNew();
		break;
	}
	case E_HAS_EXIST:
	{	
		int _val = 0;
		//FImpl->FSem = ;//exist
		if (!FImpl->FSem->MIsValid() ||sem_getvalue(&FImpl->FSem->FSem, &_val)!=0)
		{
			if(!FImpl->FSem->MIsValid())
				errno=ENOENT;
			FImpl->FSem = NULL;
			LOG(ERROR) << " Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
			return false;
		}
		break;
	}

	case E_UNDEF:
	{
		aHasToBeNew = E_HAS_EXIST;
		int _val = 0;
		if (!FImpl->FSem->MIsValid() || sem_getvalue(&FImpl->FSem->FSem, &_val)!=0)
		{
			aHasToBeNew = E_HAS_TO_BE_NEW;
			
			if (sem_init(&FImpl->FSem->FSem, 1, aInitvalue)!=0)
			{
				FImpl->FSem = NULL;
				LOG(ERROR) << "Semaphore has not created as error " << strerror(errno) << "(" << errno << ")";
				return false;
			}else
				FImpl->FSem->MCreatedNew();
		}
		break;
	}
	}

	//	for (; value != static_cast<unsigned>(aInitvalue); --value)
//	{
//		VLOG(2) << "Start sem post " << value;
//		sem_wait(FImpl->FSem);
//	}
	
	FImpl->FSem->MInc();
	FType = aHasToBeNew;
	char _str[16];
	sprintf(_str,"%u",FImpl->FSem->FId);
	FImpl->FName=_str;
	return true;
}
CIPCSem::~CIPCSem()
{
	MFree();
	delete FImpl;
}
bool CIPCSem::MWait(void)
{
	VLOG(2) << "Wait for mutex(lock) " << FImpl->FName;
	CHECK_NOTNULL(FImpl);
	CHECK_NOTNULL(FImpl->FSem);
	int _rval=0;
	do
	{
		_rval=sem_wait(&FImpl->FSem->FSem);
	}while(_rval!=0&& errno==EINVAL && !FImpl->FSem->FIsUnlinked);

	VLOG(2) << "Sem " << FImpl->FName << " hold.";
	LOG_IF(DFATAL,_rval!=0) << "Look error "<<FImpl->FName<<" " << strerror(errno)<<"("<<errno<<")";
	return _rval==0;
}
bool CIPCSem::MWait(double const aTime)
{
	VLOG(2) << "Wait for mutex(lock) " << FImpl->FName<<" Time = "<<aTime;
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
	CHECK_NOTNULL(FImpl);
	CHECK_NOTNULL(FImpl->FSem);

	bool _is = sem_timedwait(&FImpl->FSem->FSem,&tm) == 0;
	VLOG(2) << "Sem " << FImpl->FName << " hold.";
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSem::MTryWait(void)
{
	VLOG(2) << "Try Wait for mutex " << FImpl->FName;
	CHECK_NOTNULL(FImpl);
	CHECK_NOTNULL(FImpl->FSem);
	bool _is = sem_trywait(&FImpl->FSem->FSem) == 0;
	VLOG(2) << "Sem " << FImpl->FName << " hold.";
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is;
}
bool CIPCSem::MPost(void)
{
	VLOG(2) << "Post mutex(unlock) " << FImpl->FName;
	CHECK_NOTNULL(FImpl);
	CHECK_NOTNULL(FImpl->FSem);
	int _rval=0;
	do
	{
		_rval=sem_post(&FImpl->FSem->FSem);
	}while(_rval!=0 && errno==EINVAL&& !FImpl->FSem->FIsUnlinked);
	VLOG(2) << "Sem " << FImpl->FName << " hold.";
	LOG_IF(DFATAL,_rval!=0) << "Look error "<<FImpl->FName<<" " << strerror(errno)<<"("<<errno<<")";
	return _rval==0;
}
int CIPCSem::MValue() const
{
	CHECK_NOTNULL(FImpl);
	CHECK_NOTNULL(FImpl->FSem);
	int __value = -1;
	bool _is = sem_getvalue(&FImpl->FSem->FSem,&__value) == 0;
	VLOG(2) << "Sem " << FImpl->FName << " hold.";
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	return _is?__value:-1;
}
void CIPCSem::MUnlink()
{
//	if(!FName.empty())
//		sem_unlink(FName.c_str());
	CHECK_NOTNULL(FImpl);
	if(!MIsInited())
			return;
	bool const _is = sem_destroy(&FImpl->FSem->FSem) == 0;
	LOG_IF(ERROR,!_is) << "Look error " << strerror(errno)<<"("<<errno<<")";
	if(_is)
		FImpl->FSem->FIsUnlinked= true;
}
bool CIPCSem::MIsInited() const
{
	return FImpl->FSem != NULL;
}
void CIPCSem::MFree()
{
	CHECK_NOTNULL(FImpl);
	if(!MIsInited())
		return;
/*	bool _is = sem_close(FImpl->FSem) == 0;
	VLOG(2) << "Sem " << FImpl->FName << " freed.";
	LOG_IF(ERROR,!_is) << "Free error " << strerror(errno)<<"("<<errno<<")";
	(void)_is;*/
	if(FImpl->FSem->MDec())
	{
		MUnlink();
	}
	{
		FImpl->FSem = NULL;
		FType = E_UNDEF;
		FImpl->FName.clear();
	}
}
NSHARE::CText const& CIPCSem::MName() const
{
	CHECK_NOTNULL(FImpl);
	return FImpl->FName;
}
} /* namespace NSHARE */

#endif
