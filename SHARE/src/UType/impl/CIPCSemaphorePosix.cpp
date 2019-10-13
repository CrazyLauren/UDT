// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CIPCSemaphorePosix.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 24.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#if defined(__QNX__)||defined(unix)
#include <deftype>

#include <limits.h>
#include <semaphore.h>
#include <fcntl.h>
#include <UType/CIPCSem.h>
#include "CIPCSemaphoreFutex.h"
#include "CIPCSemaphoreSemOpen.h"
#include "CIPCSemaphoreSemInit.h"

namespace NSHARE
{
CIPCSem::CIPCSem(uint8_t* aBuf, size_t aSize, unsigned int value,
		eOpenType const aHasToBeNew) :
		FImpl(NULL),//
		FType(E_UNDEF)
{
	bool const _is=MInit(aBuf,aSize, value, aHasToBeNew);
	if(!_is)
		MASSERT_1(false);
}
CIPCSem::CIPCSem() :
		FImpl(NULL),//
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

	VLOG(2) << "Mutex  is initialized.";

	if(aSize<sMRequredBufSize())
	{
		LOG(DFATAL)<<"Invalid size of buf "<<aSize<<" min size "<<sMRequredBufSize();
		return false;
	}
	void *const _p=memchr(aBuf,'\0',aSize);
	bool const _is_no_string= _p== NULL || _p==aBuf//
			||((uint8_t*)_p-aBuf)<eReguredBufSize/2//
			|| !CCodeUTF8().MIsBufValid((CCodeUTF8::it_char_t)aBuf,(CCodeUTF8::it_char_t)_p);//

	if(_is_no_string)
	{
#ifdef SEM_USING_FUTEX
		FImpl=new impl::CIPCSemaphoreFutex();
#else
		FImpl=new impl::CIPCSemaphoreSemInit();
#endif
	}
	else
		FImpl=new impl::CIPCSemaphoreSemOpen();

	bool const _is=FImpl->MInit(aBuf, aSize, aInitvalue, aHasToBeNew);
	if(!_is)
	{
		delete FImpl;
		FImpl=NULL;
	}
	return _is;
}
CIPCSem::~CIPCSem()
{
	MFree();
	delete FImpl;
	FImpl=NULL;
}
bool CIPCSem::MWait(void)
{
	DCHECK_NOTNULL(FImpl);
	if(FImpl)
		return FImpl->MWait();
	return false;
}
bool CIPCSem::MWait(double const aTime)
{
	DCHECK_NOTNULL(FImpl);
	if(FImpl)
		return FImpl->MWait(aTime);
	return false;
}
bool CIPCSem::MTryWait(void)
{
	DCHECK_NOTNULL(FImpl);
	if(FImpl)
		return FImpl->MTryWait();
	return false;
}
bool CIPCSem::MPost(void)
{
	DCHECK_NOTNULL(FImpl);
	if(FImpl)
		return FImpl->MPost();

	return false;
}
int CIPCSem::MValue() const
{
	DCHECK_NOTNULL(FImpl);

	if(FImpl)
		return FImpl->MValue();
	return -1;
}
void CIPCSem::MUnlink()
{
	DCHECK_NOTNULL(FImpl);

	if(FImpl)
		return FImpl->MUnlink();
}
bool CIPCSem::MIsInited() const
{
	if(FImpl)
		return FImpl->MIsInited();

	return false;
}
void CIPCSem::MFree()
{
	if(FImpl)
		FImpl->MFree();

}
NSHARE::CText const& CIPCSem::MName() const
{
	DCHECK_NOTNULL(FImpl);
	if(FImpl)
		return FImpl->MName();

	static NSHARE::CText _fix;
	return _fix;
}
size_t CIPCSem::sMRequredBufSize()
{
	return 	eReguredBufSize;
}
}
#endif
