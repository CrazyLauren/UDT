/*
 * ICustomer.h
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ICUSTOMER_H_
#define ICUSTOMER_H_

#include <udt/programm_id.h>
#include <share/Socket/ISocket.h>
namespace NUDT
{
class IModule;
class CDataObject;
class CResources;
class CConfigure;
/** Interface of customer
 *
 */
class ICustomer
{
public:
	typedef NSHARE::ISocket::data_t data_t;
	typedef std::vector<IModule*> array_of_modules_t;//!< to hold pointer to modules
	typedef std::set<program_id_t> customers_t;///< An information about program

	virtual ~ICustomer()
	{
		;
	}
	/** Returns true if opened
	 *
	 * @return true if open
	 */
	virtual bool MIsOpened() const=0;

	/** Returns true if connected
	 *
	 * @return true if connected
	 */
	virtual bool MIsConnected() const=0;

	/** Open customer (start connecting to kernel )
	 *
	 * @return true if conencted
	 */
	virtual bool MOpen()=0;

	/** Close customer (disconnect from kernel )
	 *
	 * @return
	 */
	virtual void MClose()=0;

	/*!\brief Check for available module
	 *
	*\param aModule Name of module
	*\return true if exist
	 */
	virtual bool MAvailable(const NSHARE::CText& aModule) const=0;

	/*!\brief Gets module by name
	 *
	*\param aModule Name of module
	*\return pointer to module or NULL
	 */
	virtual IModule* MGetModule(const NSHARE::CText& aModule) const=0;

	/*!\brief Get list of available modules
	 *
	*\return modules
	 */
	virtual array_of_modules_t MGetModules() const=0;

	/*!\brief Get information about all program
	 *
	 *\return information about all program
	 */
	virtual customers_t MCustomers() const=0;

	/*!\brief Get information about program by uuid
	 *
	 *\param aUUID uuid of program
	 *\return A program info if the program is exist\n
	 *		  Invalid object of type program_id_t (see program_id_t::MIsValid())\n
	 *		  if program is not exist
	 */
	virtual program_id_t MCustomer(NSHARE::uuid_t const&) const=0;

	/*!\brief Allocate a new buffer(block of memory)
	 *
	 *The function allocates a buffer of size bytes.
	 *If shared memory is available then the buffer is
	 *allocated from it, else the buffer is allocated
	 *from the heap.
	 *
	 *\param aSize The number of bytes to allocate
	 *
	 *\return Return true if no event handler
	 */
	virtual NSHARE::CBuffer MGetNewBuf(size_t aSize) const=0;

	/** Gets reference to Data object
	 *
	 * @return reference to data object
	 */
	virtual CDataObject& MGetDataObject() const=0;

	/** Gets reference to Library info
	 *
	 * @return reference to library info
	 */
	virtual CResources& MGetResourceObject() const=0;

	/** Gets reference to configure
	 *
	 * @return reference to configure
	 */
	virtual CConfigure& MGetConfigureObject() const=0;
};
}//
#endif /* ICUSTOMER_H_ */
