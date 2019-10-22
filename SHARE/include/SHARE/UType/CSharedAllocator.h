/*
 * CSharedAllocator.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSHAREDALLOCATER_H_
#define CSHAREDALLOCATER_H_

namespace NSHARE
{
/** The memory manager for shared memory.
 *
 * There are two type with 4 byte pointer (max 2 GB) and with 8 byte
 * pointer. Instead of "pointer" used offset from memory begin as used shared
 * memory.
 * There are two type of offset.
 * The first offset is offset relative to param aBase of
 * #CSharedAllocator::MCreateHeap method.
 * The second offset is offset relative to heap(aBase + memory manager header)
 * begin. Unless otherwise specified, then in public interface
 * the heap offset is used.
 *
 * If You want to block change memory call
 * @code
 * {
 * 		CRAII<CSharedAllocator> _lock;//here block change memory
 * }//here  unblock change memory
 * @endcode
 * @warning Maximal size of shared memory for x86 mode (4 byte pointer)
 * 2 gb.
 * @todo condvar for MLockBlock
 */
class SHARE_EXPORT CSharedAllocator: NSHARE::CDenyCopying
{
public:
#ifdef SHARED_ALLOCATOR_X64
	typedef uint64_t offset_t;//!< Type of pointer
	typedef uint64_t block_size_t;//!<additional data for every malloc
#elif defined(SHARED_ALLOCATOR_X32)
	typedef uint32_t offset_t;//!< Type of pointer
	typedef uint32_t block_size_t;//!<additional data for every malloc
#else
	typedef IAllocater::offset_pointer_t offset_t;//!< Type of pointer
	typedef IAllocater::size_type block_size_t;//!<additional data for every malloc
#endif
	typedef offset_t offset_heap_t;//!< offset from heap (for detail @see #CSharedAllocator)
	typedef offset_t offset_base_t;//!< offset from base (for detail @see #CSharedAllocator)

	typedef uint32_t pid_type;//!< Type for process ID (PID)
	typedef uint16_t index_type; //!< Type for array index of process node (inner type).

	typedef  uint32_t free_index_t;//!< type of first index of array (inner type)
	typedef uint32_t reserve_t;//!< type of reserve (inner type)

	static const NSHARE::CText NAME;//!< A serialization key
	static const index_type NULL_INDEX;//!< The value of array which indicate not used index
	static const offset_t NULL_OFFSET;//!< The value equal NULL pointer  (max 2GB for x86)

	static const double PROCESS_ARRAY_FACTOR;/*!<How the array is growing*/
	static const block_size_t DEF_PROCESS_SIZE;/*!< begin process node size*/
	static const uint8_t MINIMAL_OFFSET;//!< The minimal data offset

	/** Check offset is NULL
	 *
	 * @param aWhat Checked offset
	 *
	 * @return true if NULL
	 */
	static bool sMIsNullOffset(const offset_t& aWhat);


	/** Info about memory block
	 *
	 */
	struct memory_info_t
	{
		/** State of memory block
		 *
		 */
		enum eType
		{
			E_FREE,//!< It's free
			E_ALLOCATED,//!< It's allocated
		};
		offset_base_t FOffset;//!< Pointer to block (offset from base)
		size_t FBlockSize;//!< Size of block
		eType FType;//!< State of block
		NSHARE::CConfig FAdditionalInfo;//!< Additional info about block if it's allocated
	};
	typedef std::vector<memory_info_t> shared_info_t;//!< Info about memory structure

	/** Info about leaked memory
	 *
	 */
	struct clean_up_t
	{
		/** Info about block
		 *
		 */
		struct _erased_block_t
		{
			offset_heap_t FOffset;//!< Pointer of block
			size_t FSize;//!< Size of memory
		};
		pid_type FPid;/*!< PID of memory owner (Who has allowed leaks.
						If block has been gives back then PID of new owner)
		 	 	 	 	 */
		smart_field_t<_erased_block_t> FBlock;/*!< Info about memory block
		 	 	 	 	 	 	 	 	 	 	 (if not exist than mean no more
		 	 	 	 	 	 	 	 	 	 	 leaked memory blocks
		 	 	 	 	 	 	 	 	 	 	 )*/
	};
	typedef std::vector<clean_up_t> clean_up_resources_t;//!< Info about leaked memory

	typedef std::vector<pid_type> leak_processes_t;//!< List of dead process

	/*!	@brief type of callback function which used for clean up
	 *
	 *
	 *	@param [in] WHO - pointer to structure CSharedAllocator
	 *	@param [in,out] WHAT - A pointer to a structure clean_up_resources_t that containes
	 *				 info about leaked memory
	 *	@param [in] YOU_DATA -A pointer to data that you wanted to pass
	 *					 as the third parameter(FYouData)
	 *
	 *	@return any
	 *
	 *	@see callback_t#operator()()
	 */
	typedef int (*clean_up_f_t)(CSharedAllocator* WHO, /*clean_up_resources_t*/void* WHAT, void* YOU_DATA);


	/** The default constructor which to can passed
	 * memory begin
	 *
	 * @param aBase pointer to memory begin (if not null calls #MInitFromCreatedHeap method)
	 *
	 */
	explicit CSharedAllocator(void * aBase = NULL);
	~CSharedAllocator();

	/** Initialize memory from exist memory manager (which was created by #MCreateHeap)
	 *
	 * @param aVal pointer to memory
	 * @param aCleanUp call method #MCleanUpResource
	 * @param aReserv amount of memory to reserve for allocation by this process
	 * @return if no error
	 */
	bool MInitFromCreatedHeap(void* aVal,bool aCleanUp=true,size_t aReserv=0);

	/** Release memory manager
	 *
	 * @return true if the memory manager has been destroyed
	 */
	bool MReleaseHeap();
	//uninit

	/** Create memory manager
	 *
	 * @param aBase pointer to memory
	 * @param aSize size of memory
	 * @param aReserv amount of memory to reserve for allocation by this process
	 */
	void MCreateHeap(void* aBase, block_size_t aSize,size_t aReserv=0);

	/** Check for initialization
	 *
	 * @return true if initialized
	 */
	bool MIsInited()const;

	/** Allocate memory to fixed location
	 *
	 * @param xWantedSize required block size
	 * @param aBlock if no memory, wait for it's freed
	 * @param aFromReserv If no free memory, using reserve
	 * @return  pointer to the start of the allocated memory, or NULL if an error occurred
	 */
	void* MMalloc(block_size_t const xWantedSize,bool aBlock=false,bool aFromReserv=false);

	/** Allocate memory to fixed location
	 *
	 * @param xWantedSize required block size
	 * @param aRefOffset Does where it allocate memory? (offset relative to heap begin)
	 * @return  pointer to the start of the allocated memory, or NULL if an error occurred
	 */
	void* MMallocTo(block_size_t const xWantedSize, offset_heap_t aRefOffset);

	/** Returns pointer to memory block if it
	 * has been allocated before
	 *
	 * @param aRefOffset - where searching block
	 * @return pointer to the start of the allocated memory, or NULL if doesn't allocate
	 */
	void* MGetIfMalloced(offset_heap_t aRefOffset);

	/** Reallocation memory block
	 *
	 * @param aP pointer to block
	 * @param aSize Requirement size (if size ==0 #MFree is called)
	 * @param aBlock if no memory, wait for it's freed
	 * @param aFromReserv If no free memory, using reserve
	 * @return pointer to memory or NULL
	 */
	void* MReallocate(void* aP,block_size_t aSize,bool aBlock=false,bool aFromReserv=false);

	/**  Free allocated memory
	 *
	 * @param pointer to allocated block
	 *
	 * @return block size or 0 - if error
	 */
	size_t MFree(void *aP);

	/** Check for allocation memory block
	 *
	 * @param pointer to allocated block
	 * @return true if allocated
	 */
	bool MIsAllocated(void *aP) const;

	/** Lock change of memory block
	 *
	 * @param pointer to allocated block
	 * @return true if locked
	 */
	bool MLockBlock(void *aP) const;

	/** Unlock change of memory block
	 *
	 * @param pointer to allocated block
	 * @return true if unlocked
	 */
	bool MUnLockBlock(void *aP) const;

	/** Check permission to change of memory block
	 *
	 * @param pointer to allocated block
	 * @return true if can lock
	 */
	bool MIsLockBlock(void *aP) const;

	/** Convert pointer to offset from base
	 *
	 * @param aP pointer to data
	 * @return offset from base or NULL_OFFSET
	 */
	offset_base_t MOffsetOfBase(void* aP) const;

	/** Convert offset from base to pointer
	 *
	 * @param aOffset offset to data from base
	 * @return pointer or NULL
	 */
	void* MPointerRefBaseOf(offset_base_t aOffset) const;

	/** Convert pointer to offset from heap
	 *
	 * @param aP pointer to data
	 * @return offset from heap or NULL_OFFSET
	 */
	offset_heap_t MOffset(void* aP) const;

	/** Convert offset from heap to pointer
	 *
	 * @param aOffset offset to data from heap
	 * @return pointer or NULL
	 */
	void* MPointer(offset_heap_t aOffset) const;


	/** Return the amount of remaining memory for allocation
	 *
	 *	@return the amount of remaining memory for allocation
	 */
	block_size_t MGetFreeBytesRemaining() const;

	/** Size of buffer where  Heap has been created
	 *
	 * @return size of buffer
	 */
	block_size_t MBufSize() const;

	/** Maximal size of memory which can be allocated
	 *
	 * @return maximal size
	 */
	block_size_t MMaxBlockSize() const;

	/** Is watch dog stands guard
	 *
	 */
	bool MIsWatchDog() const;

	/** Function which gives back leak memory.
	 *
	 * This function blocks until the leaked resource will detected
	 * (during calls method #MCleanUpResource()). Next to specified
	 * function will pass pointer to leaked memory info.
	 * If it gives back some block memory by oneself (want to keep on it),
	 * it has to specified PID of the new memory owner (field #clean_up_t::FPid).
	 * It the other case the memory will removed by the
	 * allocator automatically.
	 *
	 *	If dead process hasn't keep hold memory block, the function
	 *	will called too without info about leaked blocks.
	 *	@warning The function clean_up_f_t has been own 2 inner mutex (may be dead lock)!!
	 *
	 *	@param aCleanUpFunction - Function for given back resources
	 *	@param aData Pointer which is passed to third argument of function
	 */
	bool MCleanUpResourceByWatchDog(clean_up_f_t aCleanUpFunction, void *aData);

	/** Force start to detect of leaked block memory
	 * 	and give back theirs
	 *
	 * for detail @see #MCleanUpResourceByWatchDog method.
	 */
	void MCleanUpResource();

	/** Check for memory to leak
	 *
	 *	@return true if leaked blocks memory are exists
	 */
	bool MIsNeedCleanUpResource();

	/** Force unlock #MCleanUpResourceByWatchDog method
	 *
	 */
	void MResetWatchDog();

	/** Return true if memory is used (there is living process)
	 *
	 * @return true if is used
	 */
	bool MIsMemoryUsed();

	/** Gives amount of allocation
	 *
	 *	@return amount of allocation
	 */
	size_t MGetNumberOfAllocations() const;

	/** Gives info about shared memory (structure)
	 *
	 * @param [out] aTo The data is passed to this object
	 */
	void MGetInfo(shared_info_t & aTo) const;

	/*!\brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 *\return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	struct block_node_t;
	struct process_node_t;
	struct heap_head_t;
	struct pid_offset_t;
private:
	bool MLock() const;
	bool MUnlock() const;

	typedef std::pair<process_node_t *, process_node_t *> nodes_proc_t;
	typedef std::pair<block_node_t *, block_node_t *> nodes_block_t;

	template<class T>
	static offset_t sMOffsetFromBase(T const* const aFrom, void const * const aBase);
	template<class T>
	inline static T* sMPointerFromBase(offset_t aOffset, void* aBase);
	template<class T>
	static void* sMNextCoorectAddr(void* aAddr);
	template<class T>
	static void* sMPrevCoorectAddr(void* aAddr);

	offset_t MConvertOffsetToBase(heap_head_t const* aHeap,offset_t aOffset) const;
	offset_t MConvertOffsetToHead(heap_head_t const* aHeap,offset_t aOffset) const;

	void* MMallocImpl(heap_head_t* const _p_head, block_size_t const xWantedSize,
			offset_t aOffset,bool aUseReserv);
	process_node_t * const MGetOrCreateProcessNode(
			heap_head_t* const aHead, //pointer to head
			int aCount //the number of the new blocks
			) const;

	process_node_t * const MGetOrRealocateProcessNode(process_node_t *aProccess,//
			heap_head_t* const aHead, //pointer to head
			int aCount //the number of the new blocks
			) const;

	process_node_t * const MCreateProcessNode(
			heap_head_t* const aHead
			) const;
	block_node_t * const MCreateReservForProcess(
			process_node_t*  ,heap_head_t* const aHead
			) const;

	bool MIsAllocatedImpl(offset_t const aBaseOffset) const;


	//first the node,second -prev
	nodes_proc_t MSearchProcessNode(heap_head_t* const aHead,
			unsigned aPid) const;
	process_node_t * MSearchProcessOfBlockNode(heap_head_t* const aHead,
			block_node_t * aNode) const;

	nodes_proc_t& MInsertProcessNode(heap_head_t* const aHead,
			nodes_proc_t&) const;
	nodes_proc_t& MEraseProcessNode(heap_head_t* const aHead,
			nodes_proc_t&) const;
	void *MMallocBlock(heap_head_t* const aHead,
			block_size_t const xWantedSize, bool aFromEnd,
			offset_t aOffset = 0) const;
	void * MMallocBlockFromReserv( heap_head_t* const _p_head,process_node_t * const, block_size_t const xWantedSize) const;

	nodes_block_t MGetFreeBlockFromBegin(offset_t const aOffset,
			block_size_t const _alligment_size, heap_head_t* const _p_head) const;
	nodes_block_t MGetFreeBlockFromEnd(block_size_t _alligment_size,
			heap_head_t* const _p_head) const;

	size_t MFreeBlock(
			heap_head_t* const _p_head, //pointer to head
			void *aP //the pointer it has to be freed
			) const;
	void MEraseNodeFromFreeList(const nodes_block_t& _nodes,
			heap_head_t* const _p_head) const;
	void MInsertIntoFreeList(
			heap_head_t* const aHead, //pointer to head
			block_node_t *aInsertBlock, //pointer for the inserting block
			block_node_t *aPrev //pointer to the previous block of the free list
			) const;
	void MSplitBlockAndUpdateList(nodes_block_t& _nodes, block_size_t _alligment_size,
			bool aFromEnd, heap_head_t* const _p_head) const;
	block_node_t* MSplitInto2Block(block_node_t *& aWho,block_size_t _alligment_size,
			bool aFromEnd,
			heap_head_t* const _p_head) const;
	static block_node_t* sMGetBlockNode(void * const aPointer);
	static void * sMGetPointerOfBlockNode(block_node_t* aNode);
	block_node_t* MGetBlockNode(offset_t  aBaseOffset) const;

	static heap_head_t* const sMGetHead(void* const aBase);
	void MCleaunUp( clean_up_t const& _cleanup);
	void MCleaunUpBlock( heap_head_t* const _p_head,
			clean_up_t const& aProc,process_node_t *);
	size_t MFreeImpl(void* aP,
			heap_head_t* const _p_head,process_node_t* _p);
	void MFreeReservedBlock(const offset_t _offset, block_node_t* _p_node, process_node_t* _p);

	void MCheckResources(clean_up_resources_t &);
	void MGetLeakResources(leak_processes_t const&,clean_up_resources_t &);
	void MGetLeakProcesses(leak_processes_t &);
	void MCleanUpMemoryImpl( clean_up_resources_t const& _list);
	void MCleanUpMemoryImpl( leak_processes_t const& _list);
	void MCleanUpImpl();
	bool MIsWatchDogLive(heap_head_t* const _p_head,leak_processes_t const& _list);
	void MCleanUpByWatchDogImpl(leak_processes_t const& _list);
	void MResetWatchDogImpl(heap_head_t* const _p_head);
	bool MWatchDogLock() const;
	void MRemoveWatchDog(heap_head_t*const  _p_head);

	void MInitProcessNode(heap_head_t* const _p_head);
	void MFillInfo(memory_info_t &,heap_head_t* const _p_head) const;
	void MFillInfo(memory_info_t &,process_node_t* const _p_head) const;
	void MFillInfo(memory_info_t &,block_node_t* const _p_node) const;
	void MGetInfoImpl(shared_info_t &) const;
	void MSerializeImpl(NSHARE::CConfig&,shared_info_t const&) const;
	static std::string  sMPrintAsXml(CSharedAllocator const* aFrom);
	bool MSafetyLockImpl(heap_head_t* const _p_head) const;
	bool MLockImpl(heap_head_t* const _p_head) const;
	bool MUnlockImpl(heap_head_t* const _p_head) const;
	void MInitIfNeedFreeSem(heap_head_t* const _p_head);
	void MInitIfLockBlockSem(heap_head_t* const _p_head);
	void MInformMemFreed(heap_head_t* const _p_head);
	bool MWaitFreeOperation( heap_head_t*const _p_head);
	void MCleanUpFreeSemaphore();

	void MInformMemUnlocked(heap_head_t* const _p_head,const offset_t aWhat) const;
	bool MWaitUnlockOperation( heap_head_t*const _p_head,const offset_t aWhat) const;

	void MKeepBlock(block_node_t * _alloc, process_node_t*const  _process) const;
	void MUnkeepBlock(block_node_t * _alloc, process_node_t*const  _process) const;


	void* FBase;
	mutable process_node_t* FCurentProcess;
	mutable NSHARE::CIPCSem FSem;
	mutable NSHARE::CIPCSem FFreeSem;//signaling when the allocated memory has been freed,
	//thereafter the other 'user' can try allocate the memory again
	//не надёжно - можно зависнуть в цикле если Post !=  Wait
	mutable NSHARE::CIPCSem FWaitForUnlock;//!<Wait for memory unlock memory
	mutable size_t FReserv;


	friend class CRAII<CSharedAllocator>;
};
template<> class SHARE_EXPORT CRAII<CSharedAllocator> : public CDenyCopying
{
public:
	explicit CRAII(CSharedAllocator const& aSem) :
			FSem(aSem)
	{
		MLock();
	}
	~CRAII()
	{
		MUnlock();
	}
	inline void MUnlock()
	{
		if (FIsLock)
			FSem.MUnlock();
		FIsLock = false;
	}
private:
	inline void MLock()
	{
		FSem.MLock();
		FIsLock =true;
	}
	CSharedAllocator const &FSem;
	volatile bool FIsLock;
};
} /* namespace NSHARE */
#endif /* CSHAREDALLOCATER_H_ */
