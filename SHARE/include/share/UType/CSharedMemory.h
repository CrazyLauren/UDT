/*
 * CSharedMemory.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSHAREDMEMORY_H_
#define CSHAREDMEMORY_H_

namespace NSHARE
{
class CSharedAllocator;

/** @brief The class provides API for working with shared memory
 *
 *	It bases on boost shared memory realization and allows: \n
 *		- auto validation uses of the memory \n
 *		- adds and removes shared memory \n
 *		- clean up memory for killed process \n
 *		- reserved memory for current process \n
 *		- allocated memory into specified location \n
 *		- check if memory into specified location was allocated \n
 *		- block change of allocated block (inner faster mutex
 *				for all allocated blocks) \n
 *
 *
 * It works even on QNX, linux, windows
 *
 * For detail @see #NSHARE::IAllocater and NSHARE::CSharedAllocator
 */
class SHARE_EXPORT CSharedMemory:NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;//!< A serialization key
	static const NSHARE::CText KEY_SHM_NAME;//!< Key of shared memory name

	/** Enumeration of error types.
	 *
	 */
	enum eError
	{
		E_NO_ERROR=0,//!< no error
		E_NO_EXIST=-255,//!< The shared memory of specified name is not exist
		E_IS_USING,//!<Shared memory is using by the other process now. (usual is logical error)
		E_CANNOT_READ_FILE,//!< Cannot open or create file
		E_CANNOT_CREATE_SEMAPHORE,/*!<Cannot create the semaphores.
		 	 	 	 	 	 	 Thus cannot open Shm (magical error!).*/
		E_CANNOT_INITIALIZE_SEMAPHORE,/*!<Cannot initialize the semaphores specified
									in shared memory*/
		E_SHM_ISNOT_VALID,//!<The ShM is not valid but is exist
		E_INVALID_REQUREMNT_SIZE//!<The requirement size more than max size (see ::MCheckSize )
	};
	/** The default the default constructor
	 *
	 */
	CSharedMemory();

	/** The default the default destructor
	 *
	 * If the shared memory isn't used by other process
	 * it will removed, in the other case - no.
	 */
	~CSharedMemory();

	/** Open or create shared memory
	 *
	 *	If the memory is not exist or not valid it will be created.
	 *	If it's used by the other process the error will occurred.
	 *
	 *
	 * @note Really available memory for allocation always is decreases  than specified
	 * (rough -128 bytes)
	 *
	 * @param aName name of shared memory
	 * @param aSize  requirement size of memory
	 * @param aReserv amount of memory to reserve for allocation by this process
	 * @return E_NO_ERROR if memory was created
	 */
	eError MOpenOrCreate(NSHARE::CText const& aName,size_t aSize=0,size_t aReserv=0);

	/** Open shared memory
	 *
	 * @param aName name of shared memory
	 * @param aIsCleanUp  start up clean up
	 * @param aReserv amount of memory to reserve for allocation by this process
	 */
	eError MOpen(NSHARE::CText const& aName,bool aIsCleanUp=true,size_t aReserv=0);

	/** Return true if Shm open
	 *
	 * @return true if open
	 */
	bool MIsOpened() const;

	/** Close shared memory
	 *
	 * @return true if the shared memory has been destroyed
	 */
	bool MFree();

	/** Returns pointer allocator
	 *
	 * It's used for malloc, free memory
	 * see CSharedMemory::IAllocater
	 *
	 * @return pointer or NULL
	 */
	IAllocater* MGetAllocator() const;

	/** Returns pointer shared memory manager
	 *
	 * @warning be careful when uses memory manager
	 * @return pointer or NULL
	 */
	CSharedAllocator* MGetMemorManager() const;

	/** Returns physical size of memory
	 *
	 * If you what gets available memory size wee NSHARE::IAllocater
	 * @return Physical size of memory
	 */
	size_t MGetSize() const;

	/** Allocate memory to fixed location
	 *
	 *	If You want to only allocate memory see @see ::NSHARE::IAllocater interface
	 *	and method MGetAllocator().
	 *
	 * @param xWantedSize required block size
	 * @param aRefOffset Does where it allocate memory? (offset from memory begin)
	 * @return  pointer to the start of the allocated memory, or NULL if an error occurred
	 */
	void* MMallocTo(uint32_t const xWantedSize, IAllocater::offset_pointer_t aRefOffset) const;

	/** Returns pointer to memory block if it
	 * has been allocated before
	 *
	 * @param aRefOffset - where searching block
	 * @return pointer to the start of the allocated memory, or NULL if doesn't allocate
	 */
	void* MGetIfMalloced(IAllocater::offset_pointer_t aRefOffset) const;

	/*!\brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/** Force start to detect of leaked block memory
	 * 	and give back theirs
	 *
	 * for detail @see #MCleanUpResourceByWatchDog method.
	 */
	void MCleanUp();

	/** Function which gives back leak memory.
	 *
	 * for detail @see #CSharedAllocator::MCleanUpResourceByWatchDog method.
	 *
	 * 	@param aCleanUpFunction - Function for given back resources
	 *	@param aData Pointer which is passed to third argument of function
	 */
	bool MCleanUpResourceByWatchDog(int (* aCleanUpFunction)(CSharedAllocator*, /*clean_up_resources_t*/void*, void*),void *aData);

	/** Force unlock #MCleanUpResourceByWatchDog method
	 *
	 */
	void MFinishCleanUp();


	/** Check for memory to leak
	 *
	 *	@return true if leaked blocks memory are exists
	 */
	bool MIsNeedCleanUpResource();

	/** Check for override specified value
	 * maximal allowed size of shared memory
	 *
	 * @param aSize checked size
	 * @return true if aSize less then allowed size
	 */
	bool MCheckSize(size_t aSize) const;

	/** Force remove specified shared memory
	 *
	 * @param aName shared memory name
	 * @return true if removed
	 */
	static bool sMRemove(NSHARE::CText const& aName);

	class CImpl;
	struct mem_info_t;
private:
	CImpl* FImpl;

	friend class CSharedAllocatorImpl;
};

} /* namespace NSHARE */
namespace std
{
inline std::ostream& operator<<(std::ostream& aStream,
		NSHARE::CSharedMemory::eError const& aVal)
{
	switch (aVal)
	{
	case NSHARE::CSharedMemory::E_NO_ERROR:
		aStream<<"no error";
		break;
	case NSHARE::CSharedMemory::E_NO_EXIST:
		aStream<<"error:The shared memory of specified name is not exist";
		break;
	case NSHARE::CSharedMemory::E_IS_USING:
		aStream<<"error:Shared memory is using by the other process no";
		break;
	case NSHARE::CSharedMemory::E_CANNOT_READ_FILE:
		aStream<<"error:Cannot open or create file";
		break;
	case NSHARE::CSharedMemory::E_CANNOT_CREATE_SEMAPHORE:
		aStream<<"error:Cannot create the semaphores";
		break;
	case NSHARE::CSharedMemory::E_CANNOT_INITIALIZE_SEMAPHORE:
		aStream<<"error:Cannot initialize the semaphores specified in shared memory";
		break;
	case NSHARE::CSharedMemory::E_SHM_ISNOT_VALID:
		aStream<<"error:The ShM is not valid but is exist";
		break;
	case NSHARE::CSharedMemory::E_INVALID_REQUREMNT_SIZE:
		aStream<<"error:The requirement size more than max size";
		break;
	}
	return aStream;
}
}
#endif /* CSHAREDMEMORY_H_ */
