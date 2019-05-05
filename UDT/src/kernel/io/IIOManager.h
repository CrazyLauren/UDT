/*
 * IIOManager.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 29.02.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IIOMANAGER_H_
#define IIOMANAGER_H_

#include <core/IState.h>
#include "IProtocols.h"
namespace NUDT
{
class CKernelIo;
class IIOManager: public IProtocols, public NSHARE::IFactory, public IState
{
public:
	typedef NSHARE::ISocket::data_t data_t;
	typedef std::vector<descriptor_t> descriptors_t;
	virtual ~IIOManager()
	{

	}
	virtual void MInit(CKernelIo *)=0;
	virtual bool MOpen(void const* = NULL)=0;//fixme the argument is depreciated
	virtual bool MIsOpen() const =0;
	virtual void MClose()=0;
	virtual void MClose(const descriptor_t& aFrom)=0;
	using IProtocols::MSend;
	virtual bool MSend(const data_t& aVal, descriptor_t const& aTo)=0;

	virtual descriptors_t MGetDescriptors()const=0;
	virtual bool MIs(descriptor_t)const=0;
	virtual NSHARE::CConfig const&  MBufSettingFor(const descriptor_t&,
			NSHARE::CConfig const& aFrom) const=0;


protected:
	IIOManager(const NSHARE::CText& type) :
			NSHARE::IFactory(type), IState(type)
	{
	}
};
}

#endif /* IIOMANAGER_H_ */
