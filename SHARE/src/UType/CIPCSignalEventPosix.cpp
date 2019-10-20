// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCSignalEventPosix.cpp
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
#include <fcntl.h>
#include <share/UType/CIPCSignalEvent.h>
#include <share/UType/CIPCMutex.h>

namespace NSHARE
{
struct CIPCSignalEvent::CImpl
{
	/** it's stored in shared memory
	 *
	 */
	struct data_t
	{
		pthread_cond_t FPCond;
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
	CImpl(uint8_t* aBuf, size_t aSize,CIPCSignalEvent::eOpenType const aType) :
		FData(NULL)
	{
	}
	static void MConditionCleanup(void *arg)
	{
		pthread_mutex_t *mutex = static_cast<pthread_mutex_t *>(arg);
		pthread_mutex_unlock(mutex);
	}
	data_t* FData;//!< Pointer to data in shared memory
	CText FName;//!< Name of mutex
};
namespace
{
COMPILE_ASSERT((sizeof(CIPCSignalEvent::CImpl::data_t))<=CIPCSignalEvent::eReguredBufSize,InvalidSizeCondvar_t);
COMPILE_ASSERT(sizeof(CIPCSignalEvent::CImpl::data_t) ==(sizeof(uint32_t)*2//
		+sizeof(pthread_cond_t)//
		),IVALID_SIZEOF_DATA);
}
bool CIPCSignalEvent::CImpl::data_t::MIsValid() const
{
	return FCount > 0 && !FIsUnlinked;
}
void CIPCSignalEvent::CImpl::data_t::MInc()
{
	++FCount;
}
bool CIPCSignalEvent::CImpl::data_t::MDec()
{
	return --FCount==0;
}
inline void CIPCSignalEvent::CImpl::data_t::MCreatedNew()
{
	FIsUnlinked=false;
	FCount=0;
	FId=get_uuid().MGetHash();
}
CIPCSignalEvent::CIPCSignalEvent() ://
		FPImpl(new CImpl),
		FType(E_UNDEF)
{
	;
}
CIPCSignalEvent::CIPCSignalEvent(uint8_t* aBuf, size_t aSize,
		eOpenType const aHasToBeNew) :
		FPImpl(new CImpl),//
		FType(E_UNDEF)
{
	bool const _is=MInit(aBuf,aSize, aHasToBeNew);
	DCHECK(_is);
}
bool CIPCSignalEvent::MInit(uint8_t* aBuf, size_t aSize, 	eOpenType  aHasToBeNew)
{
	DCHECK_NOTNULL(aBuf);

	if (MIsInited())
	{
		LOG(ERROR)<<"Cannot init ipc sem";
		return false;
	}

	VLOG(2) << "Mutex  is initialized.";

	if(aSize<sMRequredBufSize())
	{
		LOG(DFATAL)<<"Invalid size of buf "<<aSize<<" min size "<<sMRequredBufSize();
		return false;
	}

	FPImpl->FData=(CImpl::data_t*)get_alignment_address<CImpl::data_t>(aBuf);
	DCHECK_LE((void*)(FPImpl->FData+1),(void*)(aBuf+aSize));
	CImpl::data_t &_data=*FPImpl->FData;

	pthread_condattr_t attr;
	int r = pthread_condattr_init(&attr);
	if(r != 0)
		goto error;


	r = pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if(r != 0)
		goto error;

	switch (aHasToBeNew)
	{
	case CIPCSignalEvent::E_HAS_TO_BE_NEW:
	{
		if(_data.MIsValid())
		{
			errno=EEXIST;
			goto error;
		}
		r = pthread_cond_init(&(_data.FPCond), &attr);

		if (r != 0)
			goto error;
		else
			_data.MCreatedNew();
		break;
	}
	case CIPCSignalEvent::E_HAS_EXIST:
	{
		if (!_data.MIsValid())
		{
			errno=ENOENT;
			goto error;
		}
		break;
	}

	case CIPCSignalEvent::E_UNDEF:
	{
		aHasToBeNew = CIPCSignalEvent::E_HAS_EXIST;
		if (!_data.MIsValid())
		{
			aHasToBeNew = CIPCSignalEvent::E_HAS_TO_BE_NEW;
			r = pthread_cond_init(&(_data.FPCond), &attr);

			if (r!=0)
				goto error;
			else
				_data.MCreatedNew();
		}
		break;
	}
	}
	pthread_condattr_destroy(&attr);
	_data.MInc();

	FType = aHasToBeNew;
	char _str[16];
	sprintf(_str,"%u",_data.FId);
	FPImpl->FName=_str;
	return true;
error:
 	LOG(ERROR) << "Mutex has not created as it exist" << strerror(errno) << "(" << errno << ")";
 	pthread_condattr_destroy(&attr);

 	FPImpl->FData = NULL;
	FType = CIPCSignalEvent::E_UNDEF;
	FPImpl->FName.clear();

	return false;
}
CIPCSignalEvent::~CIPCSignalEvent()
{
	MFree();
	delete FPImpl;
}
void CIPCSignalEvent::MFree()
{
	if(!MIsInited())
		return;
	if(FPImpl->FData->MDec())
	{
		MUnlink();
	}
	{
		FPImpl->FData = NULL;
		FType = CIPCSignalEvent::E_UNDEF;
		FPImpl->FName.clear();
	}
}
void CIPCSignalEvent::MUnlink()
{
	if(!MIsInited())
		return;
	bool const _is = pthread_cond_destroy(&FPImpl->FData->FPCond) == 0;
	DLOG_IF(ERROR,!_is) << "Unllink error " << strerror(errno)<<"("<<errno<<")";
	if(_is)
	{
		FPImpl->FData->FIsUnlinked= true;
	}
}

bool CIPCSignalEvent::MTimedwait(IMutex *aMutex)
{
	return MTimedwait(aMutex, -1);
}
bool CIPCSignalEvent::MTimedwait(IMutex *aIMutex, double const aTime)
{
	DCHECK(dynamic_cast<CIPCMutex*>(aIMutex)!=NULL);
	CIPCMutex *aMutex=static_cast<CIPCMutex *>(aIMutex);

	VLOG_IF(2,aTime>=0.0)
			<< "Condvar is Waited for  Mutex " << aTime << " sec.";
	VLOG_IF(2,aTime<0)
			<< "Condvar is Waited for  Mutex an infinitely long time";
//	aMutex->FIsForCondvar = true;
#ifdef __QNX__
	if (aMutex->MGetMutexType() != CIPCMutex::MUTEX_NORMAL)
	{
		std::cerr << "The Condvar is not  working with recursive mutex."
				<< std::endl;
		LOG(FATAL)<<"The Condvar is not  working with recursive mutex.";
		return false;
	}
#endif
	int _status=-1;
	pthread_cleanup_push(CImpl::MConditionCleanup, aMutex->MGetPtr())
		;
		if (aTime > 0.0)
		{
			struct timespec tm;
			if (clock_gettime(CLOCK_REALTIME, &tm) < 0)
			{
				LOG(DFATAL)<<"Clock reatime error "<<errno;
				return false;
			}
			VLOG(2)<<"Wait for from "<<tm.tv_sec<<"."<<tm.tv_nsec;
			add(&tm, aTime);
			VLOG(2)<<"Wait for to "<<tm.tv_sec<<"."<<tm.tv_nsec;
			_status = pthread_cond_timedwait(&FPImpl->FData->FPCond,
					static_cast<pthread_mutex_t*>(aMutex->MGetPtr()), &tm);
		}
		else
			_status = pthread_cond_wait(&FPImpl->FData->FPCond,
					static_cast<pthread_mutex_t*>(aMutex->MGetPtr()));
		VLOG_IF(1,_status<0) << "Condvar error " << strerror(errno)<<"("<<errno<<")";
		pthread_cleanup_pop(0);
	return !_status;
}
 bool CIPCSignalEvent::MSignal()
{
	return !pthread_cond_signal(&FPImpl->FData->FPCond);
}
 bool CIPCSignalEvent::MBroadcast()
{
	return !pthread_cond_broadcast(&FPImpl->FData->FPCond);
}

bool CIPCSignalEvent::MIsInited() const
{
	return FPImpl->FData != NULL;
}

NSHARE::CText const& CIPCSignalEvent::MName() const
{
	return FPImpl->FName;
}
size_t CIPCSignalEvent::sMRequredBufSize()
{
	return eReguredBufSize;
}
}
#endif

