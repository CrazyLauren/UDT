/*
 * CIPCMutex.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <share/UType/CIPCMutex.h>

#ifdef USING_MUTEX_ON_SEMAPHORE
namespace NSHARE
{
struct CIPCMutex::CImpl: CIPCSem
{
	CImpl()
	{

	}
	CImpl(uint8_t* aBuf, size_t aSize,CIPCMutex::eOpenType const aType) :
				CIPCSem(aBuf,aSize,1,static_cast<CIPCSem::eOpenType>(aType))
	{
	}
};

CIPCMutex::CIPCMutex(uint8_t* aBuf, size_t aSize,
		eOpenType const aHasToBeNew) :
		FImpl(new CImpl(aBuf,aSize,aHasToBeNew)),//
		FType(static_cast<eOpenType>(FImpl->MGetType()))
{

}
CIPCMutex::CIPCMutex() :
		FImpl(new CImpl),//
		FType(E_UNDEF)
{
	;
}
bool CIPCMutex::MInit(uint8_t* aBuf, size_t aSize,
		eOpenType  aHasToBeNew)
{
	bool _is = FImpl->MInit(aBuf, aSize, 1,
			static_cast<CIPCSem::eOpenType>(aHasToBeNew));
	FType=static_cast<eOpenType>(FImpl->MGetType());
	return _is;
}
CIPCMutex::~CIPCMutex()
{
	if (FImpl)
	{
		MFree();
		delete FImpl;
	}
}
void CIPCMutex::MFree()
{
	FImpl->MFree();
	FType=static_cast<eOpenType>(FImpl->MGetType());
}
bool CIPCMutex::MLock(void)
{
	return FImpl->MWait();
}
bool CIPCMutex::MCanLock(void)
{
	return FImpl->MTryWait();
}
bool CIPCMutex::MUnlock(void)
{
	return FImpl->MPost();
}
void CIPCMutex::MUnlink()
{
	DCHECK_NOTNULL(FImpl);
	FImpl->MUnlink();
}
bool CIPCMutex::MIsInited() const
{
	DCHECK_NOTNULL(FImpl);
	return FImpl->MIsInited();
}
size_t CIPCMutex::sMRequredBufSize()
{

	return CImpl::sMRequredBufSize();
}
NSHARE::CText const& CIPCMutex::MName() const
{
	DCHECK_NOTNULL(FImpl);
	return FImpl->MName();
}
CIPCMutex::eMutexType CIPCMutex::MGetMutexType() const
{
	return MUTEX_NORMAL;
}
}
#endif
