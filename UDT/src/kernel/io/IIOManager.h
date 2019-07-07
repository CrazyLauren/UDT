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
#include <core/kernel_type.h>
#include "IProtocols.h"
namespace NUDT
{
class CKernelIo;

/** Interface of communication between kernel and another program
 *
 */
class IIOManager: public IProtocols, public NSHARE::IFactory, public IState
{
public:
	typedef NSHARE::ISocket::data_t data_t;
	typedef std::vector<descriptor_t> descriptors_t;
	virtual ~IIOManager()
	{

	}
	/** @brief Initialize IOManager for
	 * specified IO
	 *
	 * @param aKernelIO A pointer to kernel IO
	 */
	virtual void MInit(CKernelIo * aKernelIO)=0;

	/** @brief Start IO manager
	 *
	 * @param aParam - @deprecated
	 * @return true if started
	 */
	virtual bool MOpen(void const* = NULL)=0;

	/** @brief Check for working
	 *
	 * @return true if working
	 */
	virtual bool MIsOpen() const =0;

	/** @brief Stop (close) IO manager
	 *
	 */
	virtual void MClose()=0;

	/** @brief Disconnect (close) specified client
	 *
	 *	@param aWhat The client descriptor you want to close.
	 */
	virtual void MClose(const descriptor_t& aFrom)=0;

	using IProtocols::MSend;

	/** @brief  Send to the specified client
	 *
	 * @param aVal The data you want to send
	 * @param aTo Specifies the descriptor of the client.
	 * @return
	 */
	virtual bool MSend(const data_t& aVal, descriptor_t const& aTo)=0;

	/** @brief Returns descriptors of connected clients
	 *
	 * @return List of descriptors
	 */
	virtual descriptors_t MGetDescriptors()const=0;

	/** @brief Determine if the client descriptor is exist
	 *
	 * @param aDescriptor The descriptor you want to test.
	 * @return true if exist
	 */
	virtual bool MIs(descriptor_t aDescriptor) const=0;

	/** @brief Returns requirement setting of buffering
	 *
	 * @param aDescriptor The descriptor you want to set up settings.
	 * @param aFrom A settings which is specified into configuration file
	 * 				for this IO manager
	 * @return Settings of buffering
	 */
	virtual NSHARE::CConfig const& MBufSettingFor(
			const descriptor_t& aDescriptor,
			NSHARE::CConfig const& aFrom) const=0;

protected:
	IIOManager(const NSHARE::CText& type) :
			NSHARE::IFactory(type), IState(type)
	{
	}
};
}

#endif /* IIOMANAGER_H_ */
