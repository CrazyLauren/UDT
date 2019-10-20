// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCSignalEventWIN32.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifdef _WIN32
#include <deftype>
#include <winerror.h>
#include <share/UType/CIPCSem.h>
#include <share/UType/CIPCSignalEvent.h>
namespace NSHARE
{
extern size_t get_unique_name(char const* aPreifix, uint8_t* aTo, size_t aSize);

size_t CIPCSignalEvent::sMRequredBufSize()
{
	return CIPCSignalEvent::eReguredBufSize;
}

struct CIPCSignalEvent::CImpl
{
	/** Data that is stored to shared memory
	 *
	 */
	struct data_t
	{
		NSHARE::atomic_t FWaitNum; //!< waiters
		NSHARE::atomic_t FReleaseCount; //!< number of release thread
		NSHARE::atomic_t FWaitGenNum; //!< current "generation" - it don't allow
		unsigned char FName[12];//!< Name of condvar

		inline bool MIsValid() const;//!< Check for valid

		/** Creates a new data
		 *
		 */
		inline void MCreatedNew();

	};
	HANDLE FSignalEvent;
	NSHARE::CText FName;
	data_t * FData;

	CImpl() :
			FSignalEvent(INVALID_HANDLE_VALUE),//
			FData(NULL)
	{
	}
};
namespace
{
COMPILE_ASSERT(sizeof(CIPCSignalEvent::CImpl::data_t) ==(sizeof(NSHARE::atomic_t)*3//
		+sizeof(uint8_t)*12//
		),IVALID_SIZEOF_DATA);

COMPILE_ASSERT(sizeof(CIPCSignalEvent::CImpl::data_t) >=CIPCSignalEvent::eReguredBufSize//
		,IVALID_SIZEOF_BUFSIZE);

}
bool CIPCSignalEvent::CImpl::data_t::MIsValid() const
{
	void const * const _p = memchr(FName, '\0', sizeof(FName));
	return  _p == NULL//
			|| _p == FName//
			|| ((uint8_t*) _p -FName) < sizeof(FName) / 2//
			|| !CCodeUTF8().MIsBufValid((CCodeUTF8::it_char_t)FName,
						(CCodeUTF8::it_char_t) _p);
}
void CIPCSignalEvent::CImpl::data_t::MCreatedNew()
{
	get_unique_name("cv", FName, sizeof(FName));

	FReleaseCount=0;
	FWaitGenNum=0;
	FWaitNum=0;
}
CIPCSignalEvent::CIPCSignalEvent() :
		FPImpl(new CImpl),//
		FType(E_UNDEF)
{

}
CIPCSignalEvent::CIPCSignalEvent(uint8_t* aBuf, size_t aSize, eOpenType aIsNew) :
		FPImpl(new CImpl)
{
	bool const _is = MInit(aBuf, aSize, aIsNew);
	DCHECK(_is);
}
bool CIPCSignalEvent::MInit(uint8_t* aPBuf, size_t aSize, eOpenType aIsNew)
{
	if (MIsInited())
	{
		LOG(ERROR)<<"Cannot init ipc condvar";
		return false;
	}
	if (aSize < eReguredBufSize)
	{
		LOG(DFATAL) << "Invalid size of buf " << aSize << " min size "
								<< eReguredBufSize;
		return false;
	}

	DCHECK_NOTNULL(FPImpl);
	DCHECK_EQ(FPImpl->FData, NULL);
	FPImpl->FData=(CImpl::data_t*)get_alignment_address<CImpl::data_t>(aPBuf);

	CImpl::data_t& _data(*FPImpl->FData);

	bool const _is_no_string =_data.MIsValid();

	if (aIsNew == E_UNDEF && _is_no_string)
	{
		VLOG(2) << "The buffer is empty. Create the mutex";
		aIsNew = E_HAS_TO_BE_NEW;
	}

	switch (aIsNew)
	{
	case E_HAS_TO_BE_NEW:
		{
			_data.MCreatedNew();
			break;
		}
	case E_UNDEF:
		case E_HAS_EXIST:
		{
			if (_is_no_string)
			{
			LOG(ERROR) << "The buffer is empty.";
			return false;
			}
			break;
		}
	}

	FPImpl->FSignalEvent = ::CreateEvent(NULL, FALSE, FALSE, (char*) _data.FName);
	if (FPImpl->FSignalEvent == INVALID_HANDLE_VALUE)
		return false;

	DWORD const _last_error = GetLastError();
	LOG_IF(FATAL,FPImpl->FSignalEvent==INVALID_HANDLE_VALUE)
																<< "CreateEvent failed. signalEvent:"
																<< FPImpl->FSignalEvent
																<< " error:"
																<< _last_error;

	LOG_IF(DFATAL,_last_error == ERROR_ALREADY_EXISTS && aIsNew==E_HAS_TO_BE_NEW)
																							<< "The signalEvent  "
																							<< (char*) _data.FName
																							<< " is exist.";
	VLOG_IF(2,_last_error != ERROR_ALREADY_EXISTS) << "The New signalEvent "
															<< (char*) _data.FName
															<< " has been created.";
	VLOG_IF(2,_last_error == ERROR_ALREADY_EXISTS) << "The signalEvent "
															<< (char*) _data.FName
															<< " is exist.";

	bool const _is_new = _last_error != ERROR_ALREADY_EXISTS;

	switch (aIsNew)
	{
		case E_HAS_TO_BE_NEW:
		{
			if (!_is_new)
			{
				MFree();
				return false;
			}
		break;
		}
		case E_HAS_EXIST:
		{
			if (_is_new)
			{
				LOG(ERROR)
									<< "The condvar is not to be a new, but it's not true.";
				MFree();
				return false;
			}
			break;
		}
	case E_UNDEF:
		{
			if (_is_new)
				aIsNew = E_HAS_TO_BE_NEW;
			else
				aIsNew = E_HAS_EXIST;

			break;
		}
	}

	FType = aIsNew;
	FPImpl->FName = (char*) _data.FName;
	return true;
}
void CIPCSignalEvent::MFree()
{
	DCHECK_NOTNULL(FPImpl);
	if (MIsInited())
	{
		VLOG(2) << "Event " << FPImpl->FName << " is freed.";
		BOOL _is = CloseHandle(FPImpl->FSignalEvent);
		VLOG(2) << "Close handle return " << _is << " last error "
							<< (_is ? 0 : GetLastError());
		FPImpl->FSignalEvent = INVALID_HANDLE_VALUE;
		FPImpl->FData=NULL;
		FPImpl->FName.clear();
		FType = E_UNDEF;
	}
}
CIPCSignalEvent::~CIPCSignalEvent()
{
	if (FPImpl)
	{
		MFree();
		delete FPImpl;
		FPImpl=NULL;
	}
}
bool CIPCSignalEvent::MTimedwait(IMutex * aMutex)
{
	return MTimedwait(aMutex,-1);
}
bool CIPCSignalEvent::MTimedwait(IMutex *aMutex, double const aTime)
{
	DCHECK_NOTNULL(FPImpl);
	if (!MIsInited())
	{
		LOG(DFATAL)<<"The condition variable is not initialized";
		return false;
	}
	VLOG(2) << "Event " << FPImpl->FName << " is waited for.";

	unsigned msec = (aTime<=0?INFINITE:(unsigned int)(aTime * 1000));

	NSHARE::atomic_t &_wait_num(FPImpl->FData->FWaitNum);
	NSHARE::atomic_t &_release_count(FPImpl->FData->FReleaseCount);
	NSHARE::atomic_t &_wait_gen_num(FPImpl->FData->FWaitGenNum);

	///store my sequence number
	NSHARE::atomic_t::value_type _generation;
	NSHARE::atomic_t::value_type _old_wait_num;

	do
	{
		_old_wait_num=_wait_num;
		_generation=_wait_gen_num;
	}while(_wait_num.MWriteIfEqual(_old_wait_num+1, _old_wait_num)!=_old_wait_num);

	aMutex->MUnlock();
	DWORD ret;
	bool _is_timeout = false;
	bool _wait_done=false;
	for (HANG_INIT;!_is_timeout //
				&& !_wait_done//
				;HANG_CHECK)
	{
		ret = ::WaitForSingleObject(FPImpl->FSignalEvent, msec);
		DVLOG(2) << "Ret = " << ret;


		_is_timeout= ret == WAIT_TIMEOUT;


		LOG_IF(DFATAL,((ret!= NO_ERROR) && !_is_timeout))<<" Error in WaitForSingleObject "<<ret;

		if(!_is_timeout)
		{
			///Check is signal for me
			NSHARE::atomic_t::value_type _old_release_count;
			do
			{
				///store old value
				_old_release_count=_release_count;
				_old_wait_num=_wait_num;

				_wait_done=_old_release_count>0 &&_wait_gen_num!=_generation;

				if(_wait_done)
				{
					if(_release_count.MWriteIfEqual(_release_count-1, _old_release_count)==_old_release_count)
					{
						if(_wait_num.MWriteIfEqual(_wait_num-1, _old_wait_num)==_old_wait_num)
						{
							///May be I is last thread
							if(_old_release_count==1)
								ResetEvent( FPImpl->FSignalEvent);

							break;
						}
						else
							++_release_count;
					}
				}
			}while(_wait_done//
					|| (_old_release_count!=_release_count || _old_wait_num!=_wait_num));
		}else
			--_wait_num;
	}
	bool _is = aMutex->MLock();
	DCHECK(_is);
	(void) _is;
	VLOG(2) << "Event " << FPImpl->FName << " has been singaled.";
	return !_is_timeout;
}

bool CIPCSignalEvent::MSignal()
{
	DCHECK_NOTNULL(FPImpl);
	VLOG(2) << "Event " << FPImpl->FName << " is signaled.";

	NSHARE::atomic_t &_wait_num(FPImpl->FData->FWaitNum);
	NSHARE::atomic_t &_wait_gen_num(FPImpl->FData->FWaitGenNum);
	NSHARE::atomic_t &_release_count(FPImpl->FData->FReleaseCount);
	NSHARE::atomic_t::value_type _old_wait_gen_num;
	NSHARE::atomic_t::value_type _old_release_count;
	NSHARE::atomic_t::value_type _old_wait_num;

	do
	{
		_old_wait_num = _wait_num;
		/// If exist locked thread
		while (_wait_num > _release_count)
		{
			///Unlock next thread
			_old_wait_gen_num = _wait_gen_num;
			_old_release_count = _release_count;
			if (_release_count.MWriteIfEqual(_release_count + 1,
					_old_release_count) == _old_release_count)
			{
				if (_wait_gen_num.MWriteIfEqual(_wait_gen_num + 1,
						_old_wait_gen_num) == _old_wait_gen_num)
				{
					return SetEvent(FPImpl->FSignalEvent) != FALSE;
				}
				else
					--_release_count;
			}
		}
	}
	while (_old_wait_num != _wait_num);

	return false;
}
bool CIPCSignalEvent::MBroadcast(void)
{
	DCHECK_NOTNULL(FPImpl);
	VLOG(2) << "Event " << FPImpl->FName << " is signaled.";

	NSHARE::atomic_t &_wait_num(FPImpl->FData->FWaitNum);
	NSHARE::atomic_t &_wait_gen_num(FPImpl->FData->FWaitGenNum);
	NSHARE::atomic_t &_release_count(FPImpl->FData->FReleaseCount);
	NSHARE::atomic_t::value_type _old_wait_gen_num;
	NSHARE::atomic_t::value_type _old_release_count;
	NSHARE::atomic_t::value_type _old_wait_num;

	do
	{
		_old_wait_num=_wait_num;
		while (_old_wait_num > 0)
		{
			VLOG(1) << "Do broadcast...";
			///Unlock all thread
			_old_wait_gen_num=_wait_gen_num;
			_old_release_count=_release_count;

			if (_wait_gen_num.MWriteIfEqual(_wait_gen_num + 1, _old_wait_gen_num)
					== _old_wait_gen_num)
			{
				if (_release_count.MWriteIfEqual(_old_wait_num,
								_old_release_count) == _old_release_count)
				{
					return SetEvent(FPImpl->FSignalEvent) != FALSE;
				}
				else
				--_wait_gen_num;
			}

			_old_wait_num=_wait_num;
		}
	}while(_old_wait_num!=_wait_num);

	VLOG(1) << "There is not expecting thread.";
	return false;
}
NSHARE::CText const& CIPCSignalEvent::MName() const
{
	DCHECK_NOTNULL(FPImpl);
	return FPImpl->FName;
}
void CIPCSignalEvent::MUnlink()
{
	DVLOG(1)<<"Does not need to implement";
}
bool CIPCSignalEvent::MIsInited() const
{
	return FPImpl->FSignalEvent != INVALID_HANDLE_VALUE;
}
CIPCSignalEvent::eOpenType CIPCSignalEvent::MGetType() const
{
	return FType;
}
} /* namespace NSHARE */
#endif
