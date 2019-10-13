// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCSemaphoreWin32.cpp
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
namespace NSHARE
{
extern size_t get_unique_name(char const* aPreifix,uint8_t* aTo,size_t aSize);
size_t CIPCSem::sMRequredBufSize()
{
	return CIPCSem::eReguredBufSize;
}

struct CIPCSem::CImpl
{
	CImpl() :
			FSem(INVALID_HANDLE_VALUE),//
			FIs(false)
	{

	}
	mutable HANDLE FSem;
	bool FIs;
	NSHARE::CText FName;
};
CIPCSem::CIPCSem(uint8_t* aBuf, size_t aSize, unsigned int value,
		eOpenType const aHasToBeNew) :
		FImpl(new CImpl),//
		FType(E_UNDEF)
{
	bool const _is=MInit(aBuf, aSize,value, aHasToBeNew);
	(void)_is;
	DCHECK(_is);
}
CIPCSem::CIPCSem() :
		FImpl(new CImpl),//
		FType(E_UNDEF)
{
	;
}
bool CIPCSem::MInit(uint8_t* aBuf, size_t aSize, unsigned int aInitvalue,
		eOpenType  aHasToBeNew)
{
	if (MIsInited())
	{
		LOG(ERROR)<<"Cannot init ipc sem";
		return false;
	}
	CHECK_NOTNULL(aBuf);

	VLOG(2)<<"Mutex  is initialized.";

	if(aSize<eReguredBufSize)
	{
		LOG(DFATAL)<<"Invalid size of buf "<<aSize<<" min size "<< eReguredBufSize;
		return false;
	}
	void *const _p=memchr(aBuf,'\0',aSize);
	bool const _is_no_string=_p== NULL || _p==aBuf|| ((uint8_t*)_p-aBuf)<eReguredBufSize/2 || !CCodeUTF8().MIsBufValid((CCodeUTF8::it_char_t)aBuf,(CCodeUTF8::it_char_t)_p);

	if(aHasToBeNew==E_UNDEF && _is_no_string)
	{
		VLOG(2)<<"The buffer is empty. Create the mutex";
		aHasToBeNew=E_HAS_TO_BE_NEW;
	}

	switch (aHasToBeNew)
	{
		case E_HAS_TO_BE_NEW:
		{
			get_unique_name("cv",aBuf, aSize);		
			break;
		}

		case E_UNDEF:
		case E_HAS_EXIST:
		{
			if(_is_no_string)
			{
				LOG(ERROR)<<"The buffer is empty.";
				return false;
			}
			break;
		}
	}


	VLOG(2)<<"Sem "<< (char*)aBuf;

	FImpl->FSem = ::CreateSemaphore(NULL, aInitvalue, ISemaphore::MAX_VALUE, (char*)aBuf);

	DCHECK_NE(FImpl->FSem, INVALID_HANDLE_VALUE);

	if (FImpl->FSem == INVALID_HANDLE_VALUE)
		return false;

	DWORD const _last_error = GetLastError();
	LOG_IF(DFATAL,_last_error == ERROR_ALREADY_EXISTS && aHasToBeNew==E_HAS_TO_BE_NEW)
																								<< "The mutex  "
																								<<  (char*)aBuf
																								<< " is exist.";
	VLOG_IF(2,_last_error != ERROR_ALREADY_EXISTS) << "The New IPC mutex "
															<<  (char*)aBuf
															<< " has been created.";
	VLOG_IF(2,_last_error == ERROR_ALREADY_EXISTS) << "The IPC mutex " <<  (char*)aBuf
															<< " is exist.";
	bool _is_new = _last_error != ERROR_ALREADY_EXISTS;

	switch (aHasToBeNew)
	{
		case E_UNDEF:
		{
			if (_is_new)
				aHasToBeNew = E_HAS_TO_BE_NEW;
			else
				aHasToBeNew = E_HAS_EXIST;

			break;
		}
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
				LOG(ERROR)<<"The mutex is not to be a new, but it's not true.";
				MFree();
				return false;
			}
			break;
		}
	}
	FType = aHasToBeNew;
	FImpl->FIs = FImpl->FSem!= INVALID_HANDLE_VALUE;
	FImpl->FName=(char*)aBuf;
	return true;
}
CIPCSem::~CIPCSem()
{
	if (FImpl)
	{
		MFree();
		delete FImpl;
	}
}
bool CIPCSem::MWait(void)
{
	DCHECK_NOTNULL(FImpl);
	VLOG(2)<<"Wait for mutex(lock) "<<FImpl->FName;

	HANDLE const _handle = FImpl->FSem;
	DLOG_IF(FATAL,FImpl->FSem== INVALID_HANDLE_VALUE)<<"Cannot wait of "<<FImpl->FName;
	bool _is = WaitForSingleObject(_handle, INFINITE) == WAIT_OBJECT_0;
	DVLOG(2)<<"Sem "<<FImpl->FName<<" hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << GetLastError();
	return _is;
}
bool CIPCSem::MWait(double const aTime)
{
	DCHECK_NOTNULL(FImpl);
	DVLOG(2) << "Wait for mutex(lock) " << FImpl->FName << " Time = " << aTime;

	DCHECK_NE(FImpl->FSem, INVALID_HANDLE_VALUE);
	unsigned msec = static_cast<unsigned>(aTime*1000);


	bool _is = WaitForSingleObject(FImpl->FSem, msec) == WAIT_OBJECT_0;
	DVLOG(2)<<"Sem "<<FImpl->FName<<" hold.";
	DLOG_IF(ERROR,!_is) << "Look error " << GetLastError();
	return _is;
}
bool CIPCSem::MTryWait(void)
{
	DCHECK_NOTNULL(FImpl);
	DVLOG(2)<<"Try Wait for mutex "<<FImpl->FName;
	
	DCHECK_NE(FImpl->FSem, INVALID_HANDLE_VALUE);
	return ::WaitForSingleObject(FImpl->FSem, 0) == WAIT_OBJECT_0;
}
bool CIPCSem::MPost(void)
{
	DCHECK_NOTNULL(FImpl);
	DVLOG(2)<<"Post mutex(unlock) "<<FImpl->FName;

	DCHECK_NE(FImpl->FSem, INVALID_HANDLE_VALUE);
	bool _is = ::ReleaseSemaphore(FImpl->FSem, 1, 0) != 0;
	DVLOG(2)<<"Sem "<<FImpl->FName<<" released";
	DLOG_IF(FATAL,!_is) << "Look error " << GetLastError();
	return _is;
}
int CIPCSem::MValue() const
{
	DCHECK_NOTNULL(FImpl);
	DCHECK_NE(FImpl->FSem, INVALID_HANDLE_VALUE);
	LONG v = -1;
	bool _is = ::ReleaseSemaphore(FImpl->FSem, 0, &v) && v > 0;
	DLOG_IF(ERROR,!_is) << "Look error " << GetLastError();
	(void) _is;
	return v;
}
void CIPCSem::MUnlink()
{
	DVLOG(1)<<"Does not need to implement";
}
bool CIPCSem::MIsInited() const
{
	DCHECK_NOTNULL(FImpl);
	return FImpl->FSem != INVALID_HANDLE_VALUE;
}
void CIPCSem::MFree()
{
	if (MIsInited())
	{
		VLOG(2)<<"free mutex "<<FImpl->FName;
		::CloseHandle(FImpl->FSem);
		FImpl->FSem = INVALID_HANDLE_VALUE;
		FType = E_UNDEF;
	}
}
NSHARE::CText const& CIPCSem::MName() const
{
	DCHECK_NOTNULL(FImpl);
	return FImpl->FName;
}
} /* namespace NSHARE */
#endif
