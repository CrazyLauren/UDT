/*
 * CMainChannelFactoryRegister.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CMAINCHANNELFACTORYREGISTER_H_
#define CMAINCHANNELFACTORYREGISTER_H_
namespace NUDT
{
/*!
\brief
    Template of FactoryRegisterer that allows easy
    registration of a factory for any channel.
*/
template <typename T>
class CMainFactoryRegisterer : public NSHARE::CFactoryRegisterer
{
public:
    //! Constructor.
    CMainFactoryRegisterer();

    void MUnregisterFactory() const;

protected:
    void MAdding() const;
    bool MIsAlreadyRegistered() const;
};


//----------------------------------------------------------------------------//
template <typename T>
CMainFactoryRegisterer<T>::CMainFactoryRegisterer() :
NSHARE::CFactoryRegisterer(T::NAME,NSHARE::version_t(0,1))
{}

//----------------------------------------------------------------------------//
template <typename T>
void CMainFactoryRegisterer<T>::MUnregisterFactory() const
{
	CMainChannelFactory::sMGetInstance().MRemoveFactory(FType);
}

//----------------------------------------------------------------------------//
template <typename T>
void CMainFactoryRegisterer<T>::MAdding() const
{
	CMainChannelFactory::sMAddFactory<T>();
}

//----------------------------------------------------------------------------//
template <typename T>
bool CMainFactoryRegisterer<T>::MIsAlreadyRegistered() const
{
	if(CMainChannelFactory::sMGetInstancePtr())
		return CMainChannelFactory::sMGetInstance().MIsFactoryPresent(FType);
	return false;
}

//----------------------------------------------------------------------------//

}
#endif /* CMAINCHANNELFACTORYREGISTER_H_ */
