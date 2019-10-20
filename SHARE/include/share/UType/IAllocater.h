/*
 * IAllocater.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 14.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IALLOCATER_H_
#define IALLOCATER_H_
#include <share/UType/eAllocatorType.h>
namespace NSHARE
{
class IAllocater;
/**\brief definition default allocator
 *
 *	There is the same function without template. It's
 *	really default allocator. For more information  see class \see CCommonAllocater
 */
template<typename T>
inline IAllocater* get_default_allocator();
extern SHARE_EXPORT IAllocater* get_default_allocator();

/**\brief should evaluate to the alignment address for type T
 *
 */
template<typename T>
inline T* get_alignment_address(void*);

/**\brief Interface for allocate memory
 *
 *	There are two different type of allocators. The first is allocating memory
 *	in own process memory. The second is allocating in Shared Memory. The part
 *	of memory can be reserved see \see eAllocatorType.
 *\warning As Shared memory using only offset instead of pointer,
 *	 recommendation don't use the pointer!
 */
class SHARE_EXPORT IAllocater
{
public:
	typedef size_t size_type;
	typedef/*uint32_t*/uintptr_t offset_pointer_t; //fixme Maybe problem if using x64 and x86 program with SM
	//need using Small code model
	static const offset_pointer_t NULL_OFFSET;///< held in CCommonAllocater.cpp by historical reason

	/** Allocation memory status
	 *
	 */
	enum  eAllocationState
	{
		E_IS_ALLOCATED=1,//!< The memory block has been allocated
		E_ISNOT_ALLOCATED=-1,//!< The memory block hasn't been allocated
		E_CANNOT_CHECK_ALLOCATION,//!< Cannot check for allocation
	};
	virtual ~IAllocater()
	{
	}

	/** Allocate aligned memory
	 *
	 * @param aSize The number of bytes to allocate.
	 * @param aAlignment The alignment that you want to use for the memory
	 * @param aType  Where has to be allocated memory @see NSHARE::eAllocatorType
	 * @return A pointer to the allocated block, or NULL if an error occurred
	 */
	virtual void* MAllocate(size_type aSize, uint8_t aAlignment = __alignof(offset_pointer_t),
			eAllocatorType aType= ALLOCATE_FROM_COMMON) = 0;

	/** Deallocate a block of memory
	 *
	 * @param p A pointer to the block of memory that you want to free.
	 * 			It's safe to call with a NULL pointer.
	 * @param aSize Doesn't used
	 */
	virtual void MDeallocate(void* p, size_type aSize=0) = 0;

	/** Reallocate a block of memory
	 *
	 * @param p A pointer to the block of memory
	 * @param aSize The new number of bytes.
	 * @param aAlignment The alignment that you want to use for the memory
	 * @param aType Where has to be allocated memory
	 * @return A pointer to the allocated block, or NULL if an error occurred
	 */
	virtual void* MReallocate(void* p, size_type aSize, uint8_t aAlignment = __alignof(offset_pointer_t),
			eAllocatorType aType= ALLOCATE_FROM_COMMON) = 0;
//	virtual void* MMove(void* aWhat)=0; \todo

	/** Convert to offset from pointer to memory block
	 *
	 * @param aP A pointer to the block of memory
	 * @return Offset to memory block or #NULL_OFFSET
	 */
	virtual offset_pointer_t MOffset(void* aP) const
	{
		return (offset_pointer_t) (intptr_t) (aP);
	}

	/** Convert to  pointer to memory block from offset
	 *
	 * @param aP Offset to memory block
	 * @return A pointer to the block of memory
	 */
	virtual void* MPointer(offset_pointer_t aOffset) const
	{
		return reinterpret_cast<void*>(aOffset);
	}

	/** Check for allocation memory block
	 *
	 * @param pointer to allocated block
	 * @return E_IS_ALLOCATED if allocated or
	 */
	virtual eAllocationState MIsMemAllocated(void* aP) const
	{
		return E_CANNOT_CHECK_ALLOCATION;
	}
	/** The maximal size of memory block
	 *
	 *	The maximal size of memory which can be allocated
	 *	can be calculated as MMaxSize()-MGetUsedMemory()
	 * @return size in bytes
	 */
	virtual size_type MMaxSize() const=0;

	/**\brief Lock atomic operation (if it not available on target platform)
	 *
	 *	The method is used for lock "atomic" operation.
	 *
	 *\warning Usually Allocators are definition only one mutex for
	 *	all memory. Therefore be carefully!
	 *
	 *	@see #CRAII<IAllocater>
	 */
	virtual bool MAtomicLock() const
	{
		return false;
	}

	/**\brief Unlock atomic operation
	 *
	 */
	virtual bool MAtomicUnlock() const
	{
		return false;
	}

	/**\brief Lock change memory in P if possibly
	 *
	 *	The method is used for lock "atomic" operation.
	 *
	 *\warning Usually Allocators are definition only one mutex for
	 *	all memory. Therefore be carefully!
	 *
	 *	@see #CRAII<IAllocater>
	 */
	virtual bool MLockBlock(void* aP) const
	{
		return false;
	}

	/**\brief Unlock change memory in P if possibly
	 *
	 */
	virtual bool MUnlockBlock(void* aP) const
	{
		return false;
	}
//	virtual const void* MAdd(const void* p, size_type x) const = 0;
//	virtual const void* MSubtract(const void* p, size_type x) const = 0;

	/** Amount of allocated memory
	 *
	 *
	 * @return size in bytes
	 */
	virtual size_t MGetUsedMemory() const
	{
		return 0;
	}

	virtual size_t MGetNumberOfAllocations() const
	{
		return 0;
	}
};
template<class T> T* allocate_object(IAllocater& aAllocator)
{
	return new (aAllocator.MAllocate(sizeof(T), __alignof(T))) T;
}

template<class T, class Param1> T* allocate_object(IAllocater& aAllocator, //
		const Param1& aParam1)
{
	return new (aAllocator.MAllocate(sizeof(T), __alignof(T))) T(aParam1);
}
template<class T, class Param1, class Param2> T* allocate_object(
		IAllocater& aAllocator, //
		const Param1& aParam1, const Param2& aParam2)
{
	return new (aAllocator.MAllocate(sizeof(T), __alignof(T))) T(aParam1,
			aParam2);
}
template<class T, class Param1, class Param2, class Param3> T* allocate_object(
		IAllocater& aAllocator, //
		const Param1& aParam1, const Param2& aParam2, const Param3& aParam3)
{
	return new (aAllocator.MAllocate(sizeof(T), __alignof(T))) T(aParam1,
			aParam2, aParam3);
}
template<class T, class Param1, class Param2, class Param3, class Param4> T* allocate_object(
		IAllocater& aAllocator, //
		const Param1& aParam1, const Param2& aParam2, const Param3& aParam3,
		const Param4& aParam4)
{
	return new (aAllocator.MAllocate(sizeof(T), __alignof(T))) T(aParam1,
			aParam2, aParam3, aParam4);
}
template<class T, class Param1, class Param2, class Param3, class Param4,
		class Param5> T* allocate_object(
		IAllocater& aAllocator, //
		const Param1& aParam1, const Param2& aParam2, const Param3& aParam3,
		const Param4& aParam4, const Param5& aParam5)
{
	return new (aAllocator.MAllocate(sizeof(T), __alignof(T))) T(aParam1,
			aParam2, aParam3, aParam4, aParam5);
}
//if you do not have enough "allocate_object" functions it means you have a bad constructor

template<class T> void deallocate_object(IAllocater& aAllocator, T* aObject)
{
	if (!aObject)
	{
		return;
	}
	aObject->~T();
	aAllocator.MDeallocate(aObject, sizeof(T));
}
template<class T> void deallocate_object(IAllocater* aAllocator, T* aObject)
{
	if (!aAllocator)
	{
		return;
	}
	deallocate_object(*aAllocator, aObject);
}
namespace _impl
{
template<class TType, class TAligmentsType> size_t get_alligment_size_ref()
{
	const size_t _alligment = __alignof(TAligmentsType);

	size_t _length_size =
			sizeof(TType) > _alligment ? sizeof(TType) : _alligment;

	if (_length_size % _alligment > 0)
		_length_size += _length_size % _alligment;
	return _length_size;
}
template<class T> size_t get_size_of_array_len()
{
	const size_t _alligment = __alignof(T);

	size_t _length_size =
			sizeof(size_t) > _alligment ? sizeof(size_t) : _alligment;

	if (_length_size % _alligment > 0)
		_length_size += _length_size % _alligment;
	return _length_size;
}
}
/*
template<class T> T* allocate_array(IAllocater& aAllocator, size_t aCount,
		const T& aVal)
{
	//CHECK_NE(aCount, 0);

	//Calculate how much extra memory need allocate to store the length before the array
	size_t _length_size = _impl::get_size_of_array_len<T>();

	//CHECK_NE(_length_size, 0);

	//Allocate extra space to store array length
	char* _p = (char*) aAllocator.MAllocate(sizeof(T) * (aCount) + _length_size,
			__alignof(T));
	_p += _length_size;

	*(((size_t*) _p) - 1) = aCount;
	char* _end = _p + sizeof(T) * (aCount);
	for (char* _begin = _p; _begin != _end; _begin += sizeof(T))
		new (_begin) T(aVal);

	return (T*) _p;
}
template<class T> T* allocate_array(IAllocater& aAllocator, size_t aCount)
{
	//CHECK_NE(aCount, 0);

	//Calculate how much extra memory need allocate to store the length before the array
	size_t const _length_size = _impl::get_size_of_array_len<T>();

	//CHECK_NE(_length_size, 0);

	//Allocate extra space to store array length
	char* _p = (char*) aAllocator.MAllocate(sizeof(T) * (aCount) + _length_size,
			__alignof(T));
	_p += _length_size;

	*(((size_t*) _p) - 1) = aCount;
	return (T*) _p;
}
template<class T> size_t get_count_of_array(T* aArray)
{
	return *(((size_t*) aArray) - 1);
}
template<class T> size_t get_size_of_array(T* aArray)
{
	return get_count_of_array<T>(aArray)*sizeof(T);
}
template<class T> void deallocate_array(IAllocater& aAllocator, T* aArray)
{
	if (!aArray)
	{
		return;
	}

	size_t const _len = *(((size_t*) aArray) - 1);

	for (size_t i = 0; i < _len; i++)
		aArray[i].~T();

	size_t _length_size = _impl::get_size_of_array_len<T>();

	aAllocator.MDeallocate((char*) aArray - _length_size, _len*sizeof(T) + _length_size);
}*/
/** Blocking change of memory block (or memory manager if aBuf
 * doesn't passed)
 *
 */
template<> class CRAII<IAllocater> : public CDenyCopying
{
public:
	explicit CRAII(IAllocater * aMutex, void* aBuf) :
			FMutex(aMutex), FBuf(aBuf)
	{
		MLock();
	}
	explicit CRAII(IAllocater * aMutex) :
			FMutex(aMutex), FBuf(NULL)
	{
		MLock();
	}
	~CRAII()
	{
		MUnlock();

	}
	inline void MUnlock()
	{
		FBuf!=NULL?MUnlockBlock():MUnlockManager();
	}
private:
	void MLock()
	{
		FBuf!=NULL?MLockBlock():MLockManager();
	}
	inline void MUnlockBlock()
	{
		if(FIsLock) FMutex->MUnlockBlock(FBuf);
		FIsLock = false;
	}
	inline void MUnlockManager()
	{
		if(FIsLock) FMutex->MAtomicUnlock();
		FIsLock = false;
	}
	inline void MLockBlock()
	{
		FIsLock = FMutex && FMutex->MLockBlock(FBuf);
	}
	inline void MLockManager()
	{
		FIsLock = FMutex && FMutex->MAtomicLock();
	}

	IAllocater *FMutex;
	void* FBuf;
	volatile bool FIsLock;
};
template<typename T>
inline T* get_alignment_address(void* aBuf)
{
	size_t const _align_addr= __alignof(T)-1;
	return (T*)(((uintptr_t)aBuf +_align_addr)&~(_align_addr));
}
} /* namespace NSHARE */
#endif /* IALLOCATER_H_ */
