/*
 * CBuffer.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 14.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CBUFFER_H_
#define CBUFFER_H_

#include <UType/buffer_value.h>
#include <UType/eAllocatorType.h>
namespace NSHARE
{
class IAllocater;

template<class Pointer, class Refer, class diff_type>
class  iterator_type;

template<typename TPod>
class  CPODBuffer;

/**\brief Class equal of Copy on write wrapper of std::vector<char>
 * specialization for storing data of buffer's.
 *
 */
class SHARE_EXPORT CBuffer
{
public:
	typedef uintptr_t offset_pointer_t;///< is used for storing pointer diff
	typedef buf_val_t<uint8_t> value_type;///< buffer value type (Similarly std::vector), 
					      //uint8_t is not used to avoid 
					      //problems with the template specialization,
					      //for example in stream.
	typedef value_type* pointer;		///< pointer to type (Similarly std::vector)
	typedef const value_type* const_pointer;///< pointer to const type (Similarly std::vector)
	typedef value_type& reference;		///< reference of type (Similarly std::vector)
	typedef const value_type& const_reference;///< reference of const type (Similarly std::vector)
	typedef size_t size_type;		///<buffer size type (similarly std::vector)
	typedef std::ptrdiff_t difference_type;///<pointer siff type (similarly std::vector)
	typedef iterator_type<pointer, reference, difference_type> iterator; ///<iterator to type (similarly std::vector)
	typedef iterator_type<const_pointer, const_reference, difference_type> const_iterator; ///<iterator to const type (similarly std::vector)
	typedef IAllocater allocator_type;///< Which allocator type is used, while its only one.

	const size_t BEGIN_SIZE;///< Number of bytes reserved
				//from begining of buffer
				//(usually usefully when need insert data 
				//into beginning of buffer without moving)
	static const size_t DEF_BUF_RESERVE;///< default number of bytes reserved
					    //from begining of buffer
	
	/*!\brief create empty buffer and use specified allocator
	 * 
	 *\param aAlloc pointer to allocator or null
	 *\param aType can allocate memory from?
	 * 
	 *\note BEGIN_SIZE is equal DEF_BUF_RESERVE
	 */ 
	explicit CBuffer(IAllocater* aAlloc,eAllocatorType aType=ALLOCATE_FROM_COMMON);
	
	/*!\brief create a buffer
	 *
	 * Create buffer of aSize bytes with
	 * reserved aBeginSize bytes from beginning of buffer 
	 * (for inserting into beginning without moving),
	 * by specified allocator
	 * in the reserved or common memory
	 * 
	 *\param aSize buffer size
	 *\param aBeginSize how much to reserve from beginning of buffer
	 * (if value less than zero than is  used DEF_BUF_RESERVE)
	 *\param aAlloc pointer to allocator or null
	 *\param aType can allocate memory from?
	 */
	explicit CBuffer(size_type aSize=0,int aBeginSize=0,IAllocater* aAlloc=NULL,eAllocatorType aType=ALLOCATE_FROM_COMMON);

	/*!\brief create buffer and copy data
	 * 
	 * Create buffer by specified allocator
	 * and copy data from aBegin to aEnd into buffer.
	 * 
	 *\tparam ItT type of iterator
	 * 
	 *\param aBegin iterator to the beginning of data
	 *\param aEnd iterator to the end of data
	 *\param aBeginSize how much to reserve from beginning of buffer
	 * (if value less than zero than is  used DEF_BUF_RESERVE)
	 *\param aAlloc pointer to allocator or null
	 *\param aType can allocate memory from?
	 * 
	 *\note magic temaplate 
	 *\code
	 * typename ItT::reference (ItT::*op)() const =&ItT::operator* 
	 *\endcode
	 * is used to avoid calling the constructor for a non iterator type.
	 * For instance in this case:
	 *\code
	 * CBuffer _buffer(6,0);
	 *\endcode
	 */
#if __cplusplus >= 201103L
	template<class ItT,typename ItT::reference (ItT::*op)() const =&ItT::operator*>
	CBuffer(ItT aBegin, ItT aEnd,int aBeginSize=0,IAllocater* aAlloc=NULL,eAllocatorType aType=ALLOCATE_FROM_COMMON);	
#else
	template<class ItT>
	CBuffer(ItT aBegin, ItT aEnd,int aBeginSize=0,IAllocater* aAlloc=NULL,eAllocatorType aType=ALLOCATE_FROM_COMMON,
	  typename ItT::reference (ItT::*op)() const =&ItT::operator*);
#endif
	/*!\brief create buffer and copy data
	 * 
	 * Create buffer by specified allocator
	 * and copy data from aBegin to aEnd into buffer.
	 * 
	 *\tparam TPointer type of data
	 * 
	 *\param aBegin pointer to the beginning of data
	 *\param aEnd pointer to the end of data
	 *\param aBeginSize how much to reserve from beginning of buffer
	 * (if value less than zero than is  used DEF_BUF_RESERVE)
	 *\param aAlloc pointer to allocator or null
	 *\param aType can allocate memory from?
	 */
	template<class TPointer>
	CBuffer(TPointer* aBegin, TPointer* aEnd,int aBeginSize=0,IAllocater* aAlloc=NULL,eAllocatorType aType=ALLOCATE_FROM_COMMON);

	/*!\brief create buffer and copy data
	 * 
	 * Create buffer by specified allocator
	 * and copy data from aBegin to aEnd into buffer.
	 * 
	 *\param aBegin pointer to the beginning of data
	 *\param aEnd pointer to the end of data
	 *\param aBeginSize how much to reserve from beginning of buffer
	 * (if value less than zero than is  used DEF_BUF_RESERVE)
	 *\param aAlloc pointer to allocator or null
	 *\param aType can allocate memory from?
	 */
	CBuffer(void const* aBegin, void const* aEnd,int aBeginSize=0,IAllocater* aAlloc=NULL,eAllocatorType aType=ALLOCATE_FROM_COMMON);
	
	/*!\brief "recovery" buffer from memory
	 * 
	 * The buffer can be initialized from the specified address,
	 * by which is contained previously allocated buffer.
	 * As a pointer, the offset is used.
	 * The offset can be got by calling offset() method.
	 * 
	 *\param aAlloc reference to allocator by which buffer is allocated.
	 *\param Offset buffer offset.
	 *\param aCheckCrc if true The buffer will be checked for validity.
	 *\param aType What type of memory has to be used when need allocate memory 
	 */ 
	CBuffer(IAllocater& aAlloc, offset_pointer_t Offset,bool aCheckCrc=true,eAllocatorType aType=ALLOCATE_FROM_COMMON);

	~CBuffer();
	
	/*!\brief Copy constructor, reality the data is not copied.
	 * 
	 */ 
	CBuffer(const CBuffer&);
	
	/*!\brief Assignment operator, reality the data is not copied.
	 * 
	 */ 
	CBuffer const& operator=(const CBuffer&);
	
	/*!\brief deep copy (reality) buffer from
	 *
	 *\param  aFrom copy from
	 *\return *this 	
	 */ 
	CBuffer const& deep_copy(const CBuffer& aFrom);
	
	/*!\brief moving buffer to the other buffer
	 * This buffer becomes empty. 
	 * 
	 *\param aTo where to move
	 * 
	 */ 
	void MMoveTo(CBuffer& aTo);


	/*!\brief create buffer copy by the same allocator
	 *
	 *\param aType can allocate memory from?
	 *\return copy of buffer
	 */ 
	CBuffer MCreateCopy(eAllocatorType aType=ALLOCATE_FROM_COMMON);
	
	/*!\brief create buffer copy by specified allocator
	 *
	 *\param aAlloc pointer to allocator or null	  
	 *\param aType can allocate memory from?
	 *\return copy of buffer
	 */
	CBuffer MCreateCopy(IAllocater* aAlloc,eAllocatorType aType=ALLOCATE_FROM_COMMON);

	/*!\brief check for equals of the allocators
	 * 
	 *\param aAlloc comapared allocator
	 *\return true if alocators are equal or one of the allocator is null
	 */ 
	bool MIsAllocatorEqual(IAllocater* aAlloc) const;
	
	/*!\brief check for equals of the buffer's allocators
	 * 
	 *\param aAlloc comapared allocator
	 *\return true if alocators are equal or one of the allocator is null
	 */
	bool MIsAllocatorEqual(CBuffer const& aBuffer) const;
	
	/*!\brief check for equals of reference to buffer
	 */
	bool MReferenceEqual(CBuffer const& aBuffer) const;
	
	/*!\brief returns true if buffer alloacted
	 */ 
	bool MIsNull() const;
	
	/*!\brief return pointer to allocator
	 * 
	 *\return pointer to allocator
	 */ 
	IAllocater* MAllocator() const;
	
	/*!\brief check for correcting initialize buffer by pointer
	 * 
	 * For detail see "recovery" constuctor.
	 * 
	 *\return true if initialized successfully
	 * 
	 */ 
	bool MIsRestored() const;
	
	/*!\brief check for object has been automatically copied
	 * 
	 * If you use non const methods and you is not unique 
	 * buffer owner the buffer will automatically copied,
	 * 
	 *\return true if has been automatically copied
	 * 
	 */
	bool MIsDetached() const;
	
	/*!\brief functions similar functions of std vector 
	 * 
	 *\{
	 */
	
	/// \note for safety non const version is not exist
	const_reference operator[](size_type __n) const;
	/// \note for safety non const version is not exist 
	const_reference front() const;
	/// \note  for safety non const version is not exist
	const_reference back() const;
	
	/// \note  for safety non const version is not exist, use instead ptr() 
	const_pointer data() const;
	
	bool empty() const;
	size_type size() const;
	size_type max_size() const;
	void reserve(size_t aSize);
	size_type capacity() const;	
	void shrink_to_fit();
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	
	void clear();
	void insert(iterator __position, value_type const&);
	template<class T>
	void insert(iterator __position, T const& aVal);
	
	void insert(iterator __position, size_type __n, value_type const&);
	template<class T>
	void insert(iterator __position, size_type __n, T const&);

	void insert(iterator __position, iterator aBegin, iterator aEnd);
	void insert(iterator __position, const_iterator aBegin,
			const_iterator aEnd);
	template<class T>
	void insert(iterator __position, T const* aBegin, T const* aEnd);
	
	iterator erase(iterator aBegin, iterator aEnd);
	void push_back(const value_type& __x);
	void pop_back();
	
	void swap(CBuffer& aBuf);
	///\}

	/*!\brief returns the number of bytes that the buffer 
	 * has allocated from beginning
	 * 
	 * For detail see parameter aBeginSize of constuctors.
	 * 
	 *\return number of bytes
	 */	
	size_type begin_capacity() const;

	/*!\brief resize buffer
	 * 
	 *\param aSize new size
	 *\param fromBegin if false increase(decrease) from end (equal std::vector::resize)
	 * if true increase(decrease) from begin
	 *\param aCanDetach It was the result of bug fix.
	 * If the parameter is false than the buffer will not automatically copied
	 * when the buffer has not the only owner.
	 * In the other case if the buffer has more than one owner
	 * it will automatically copied.
	 */ 
	void resize(size_type aSize, bool fromBegin = false,bool aCanDetach=true);
	
	/*!\brief returns pointer to the buffer
	 */ 
	pointer ptr();
	
	/*!\brief returns pointer to the buffer
	 */ 
	const_pointer ptr_const() const;
	
	/*!\brief returns offset of buffer or 
	 * IAllocater::NULL_OFFSET if buffer is not allocated
	 */ 
	offset_pointer_t offset() const;
		
	/*!\brief Warning do not confuse the method with release()
	 * The object becomes only unreferenced with the allocated buffer.
	 * The ownership isn't released.(A number of buffer's owner 
	 * does not change)
	 * 
	 * Usually the method is used together with
	 * "recovery" constructor. 
	 * If Then a new object is not created by
	 * "recovered"  constructor,
	 * the memory leak will ocurred.
	 * 
	 *\return returns offset of the buffer or 
	 * IAllocater::NULL_OFFSET if the buffer is not allocated
	 * 
	 *\warning if you do not understand what you are doing, 
	 * do not call this method
	 */	
	offset_pointer_t refuse();
	
	/*!\brief releases the ownership 
	 */ 
	void release();
	
	/*!\brief Returns a number of owners
	 */ 
	unsigned use_count() const;
	
	/*!\brief functions similar functions of std list 
	 * 
	 *\{
	 */
	void push_front(const value_type& aVal);
	void pop_front();
	///\}
	
	/*!\brief helper functions for insert data of any types
	 * to the buffer
	 *\{
	 */

	///\}
	
	//void release_force() const; //todo

	/*!\brief returns pointer to default allocator
	 */
	static allocator_type * sMDefAllaocter();
	
	static bool sMUnitTest(IAllocater*);
	static bool sMUnitTest(size_t aSize, IAllocater*);
	struct buf_info;
private:



	struct SHARE_EXPORT _buffer_t
	{
		static const size_type BUF_OFFSET;

		_buffer_t(IAllocater* aAll,eAllocatorType);
		_buffer_t(IAllocater* aAlloc, offset_pointer_t Offset,bool aCheckCrc,eAllocatorType);
		_buffer_t(_buffer_t const & aR);
		_buffer_t & operator=(_buffer_t const & r);
		_buffer_t &deep_copy(_buffer_t const & r);
		void MMoveTo(_buffer_t& aTo);
		void swap(_buffer_t& aTo);
		~_buffer_t();

		value_type* MGetPtr(void* aVal) const;
		buf_info& MGetBuff() const;
		value_type* MStorageBegin() const;

		static buf_info& sMGetBuff(value_type* aVal);
		value_type* MReAllocate(value_type* ,size_t aSize);
		value_type* MAllocate(size_t aSize);
		void MDeallocate();
		void MDeallocate(value_type* aP, size_t __n);

		size_type size() const;
		uint32_t use_count() const;
		bool unique() const;
		void release();
		void add_ref_copy(bool aIsExist=false) const;
		bool empty() const;
		bool MIsRestored() const;

		allocator_type* FAllocator;
		value_type* FBeginOfStorage;
		eAllocatorType FAllocatorType;
	};


	pointer MInsertImpl(iterator __position, size_type __n, bool aCanDetach=true);

	void MFill(pointer  aPosition, value_type const& aVal,
			size_type aSize);
	void MFill(pointer ,const_pointer const&,const_pointer const&);

	value_type* MAllocate(size_t __n);
	value_type* MReAllocate(value_type*,size_t __n);
	size_type requred_buf_len(size_type aSize) const;

	allocator_type & get_allocator() const;
	void MDetach();
	bool MIsNeedDetach() const;




	_buffer_t FBuffer;
	bool FIsDetached;

};
/*!\brief iterator for CBuffer
 */ 
template<class Pointer, class Refer, class diff_type>
class  iterator_type
{
public:
	typedef iterator_type iterator_t;
	typedef Pointer pointer;
	typedef Refer reference;
	typedef diff_type difference_type;

	iterator_type() :
			_M_current(NULL)
	{
	}
	/**\note using only for const_iterator -> iterator convertion
	 *
	 */
	template<class Rdiff_type>
	iterator_type(iterator_type<const Pointer,const Refer,Rdiff_type>const & aRht) :
			_M_current(aRht._M_current)
	{
	}
	/**\note using only for const_iterator -> iterator convertion
	 *
	 */
	template<class Rdiff_type>
	iterator_type&
	operator=(iterator_type<const Pointer,const Refer,Rdiff_type>const & aRht)
	{
		_M_current = aRht._M_current;
		return *this;
	}
	iterator_type(iterator_type const & aRht) :
			_M_current(aRht._M_current)
	{
	}
	iterator_type&
	operator=(iterator_type const & aRht)
	{
		_M_current = aRht._M_current;
		return *this;
	}

	explicit iterator_type(const pointer& __i) :
			_M_current(__i)
	{
	}
	// Forward iterator requirements
	reference operator*() const
	{
		return *_M_current;
	}

	pointer operator->() const
	{
		return _M_current;
	}

	iterator_t& operator++()
	{
		++_M_current;
		return *this;
	}

	iterator_t operator++(int)
	{
		return iterator_t(_M_current++);
	}

	iterator_t&
	operator--()
	{
		--_M_current;
		return *this;
	}

	iterator_t operator--(int)
	{
		return iterator_t(_M_current--);
	}

	// Random access iterator requirements
	reference operator[](const difference_type& __n) const
	{
		return _M_current[__n];
	}

	iterator_t&
	operator+=(const difference_type& __n)
	{
		_M_current += __n;
		return *this;
	}

	iterator_t operator+(const difference_type& __n) const
	{
		return iterator_t(_M_current + __n);
	}

	iterator_t&
	operator-=(const difference_type& __n)
	{
		_M_current -= __n;
		return *this;
	}

	iterator_t operator-(const difference_type& __n) const
	{
		return iterator_t(_M_current - __n);
	}

	pointer& base()
	{
		return _M_current;
	}
	pointer const& base() const
	{
		return _M_current;
	}
protected:
	pointer _M_current;
};

inline CBuffer::const_iterator CBuffer::cbegin() const
{
	return begin();
}
inline CBuffer::const_iterator CBuffer::cend() const
{
	return end();
}
inline bool CBuffer::MIsDetached() const
{
	return FIsDetached;
}
inline CBuffer::const_pointer CBuffer::data() const
{
	return ptr_const();
}
inline bool CBuffer::MIsNull() const
{
  return FBuffer.FBeginOfStorage == NULL;
}
inline bool CBuffer::MReferenceEqual(CBuffer const& aBuffer) const
{
  return FBuffer.FBeginOfStorage != NULL && FBuffer.FBeginOfStorage==aBuffer.FBuffer.FBeginOfStorage;
}

template<class T>
inline void CBuffer::insert(iterator __position, T const* aBegin, T const* aEnd)
{
  // \todo optimize
	pointer _p = MInsertImpl(__position, (aEnd - aBegin)*sizeof(T));
	if (_p)
		MFill(_p, (const_pointer)aBegin, (const_pointer)aEnd);
}
template<class T>
inline void CBuffer::insert(iterator __position, T const& aVal)
{
  // \todo optimize
	pointer _p = MInsertImpl(__position, sizeof(T));
	if (_p)
		MFill(_p, (const_pointer)&aVal, (const_pointer)(&aVal+1));
}
template<class T>
inline void CBuffer::insert(iterator __position,size_type __n, T const& aVal)
{
  // \todo optimize
	pointer _p = MInsertImpl(__position, __n*sizeof(T));
	if (_p)
	{
		for (; __n != 0; --__n, _p += sizeof(T))
			MFill(_p, (const_pointer) &aVal, (const_pointer) (&aVal + 1));
	}
}

template<class Pointer, class Refer, class diff_type>
inline bool operator==(const iterator_type<Pointer, Refer, diff_type>& __lhs,
		const iterator_type<Pointer, Refer, diff_type>& __rhs)
{
	return __lhs.base() == __rhs.base();
}

template<class Pointer, class Refer, class diff_type>
inline bool operator!=(const iterator_type<Pointer, Refer, diff_type>& __lhs,
		const iterator_type<Pointer, Refer, diff_type>& __rhs)
{
	return __lhs.base() != __rhs.base();
}

template<class Pointer, class Refer, class diff_type>
inline bool operator<(const iterator_type<Pointer, Refer, diff_type>& __lhs,
		const iterator_type<Pointer, Refer, diff_type>& __rhs)
{
	return __lhs.base() < __rhs.base();
}

template<class Pointer, class Refer, class diff_type>
inline bool operator>(const iterator_type<Pointer, Refer, diff_type>& __lhs,
		const iterator_type<Pointer, Refer, diff_type>& __rhs)
{
	return __lhs.base() > __rhs.base();
}

template<class Pointer, class Refer, class diff_type>
inline bool operator<=(const iterator_type<Pointer, Refer, diff_type>& __lhs,
		const iterator_type<Pointer, Refer, diff_type>& __rhs)
{
	return __lhs.base() <= __rhs.base();
}

template<class Pointer, class Refer, class diff_type>
inline bool operator>=(const iterator_type<Pointer, Refer, diff_type>& __lhs,
		const iterator_type<Pointer, Refer, diff_type>& __rhs)
{
	return __lhs.base() >= __rhs.base();
}
template<class Pointer, class Refer, class diff_type>
inline typename iterator_type<Pointer, Refer, diff_type>::difference_type operator-(
		const iterator_type<Pointer, Refer, diff_type>& __lhs,
		const iterator_type<Pointer, Refer, diff_type>& __rhs)
{
	return __lhs.base() - __rhs.base();
}

template<class Pointer, class Refer, class diff_type>
inline iterator_type<Pointer, Refer, diff_type> operator+(
		typename iterator_type<Pointer, Refer, diff_type>::difference_type __n,
		const iterator_type<Pointer, Refer, diff_type>& __i)
{
	return iterator_type<Pointer, Refer, diff_type>(__i.base() + __n);
}

inline CBuffer::CBuffer(void const* aBegin, void const* aEnd,int aBeginSize,
			IAllocater* aAlloc,eAllocatorType aType) :
	BEGIN_SIZE(aBeginSize < 0 ? DEF_BUF_RESERVE : aBeginSize),//
	FBuffer(aAlloc ? aAlloc : sMDefAllaocter(),aType),//
	FIsDetached(false)
{
	size_t const _size = (char*)aEnd - (char*)aBegin;
	resize(_size);
	MFill(ptr(), (const_pointer)aBegin, (const_pointer)aEnd);
}
template<class TPointer>
inline CBuffer::CBuffer(TPointer* aBegin, TPointer* aEnd,int aBeginSize,
			IAllocater* aAlloc,eAllocatorType aType) :
	BEGIN_SIZE(aBeginSize < 0 ? DEF_BUF_RESERVE : aBeginSize),//
	FBuffer(aAlloc ? aAlloc : sMDefAllaocter(), aType),//
	FIsDetached(false)
{
	const size_t _sizeof = sizeof(*aBegin) / sizeof(value_type);
	const size_t _size=(aEnd - aBegin)*_sizeof;
	resize(_size);
	MFill(ptr(), (const_pointer)aBegin, ((const_pointer)aBegin)+_size);
}
#if __cplusplus >= 201103L
template<class ItT, typename ItT::reference (ItT::*op)() const>
inline CBuffer::CBuffer(ItT aBegin, ItT aEnd,int aBeginSize,
			IAllocater* aAlloc,eAllocatorType aType)
#else
template<class ItT>
inline CBuffer::CBuffer(ItT aBegin, ItT aEnd,int aBeginSize,
			IAllocater* aAlloc,eAllocatorType aType, typename ItT::reference (ItT::*op)() const)
#endif
		:
		BEGIN_SIZE(aBeginSize < 0 ? DEF_BUF_RESERVE : aBeginSize),//
		FBuffer(aAlloc ? aAlloc : sMDefAllaocter(),aType),//
		FIsDetached(false)
{
	const size_t _sizeof = sizeof(*aBegin) / sizeof(value_type);
	resize((aEnd - aBegin)*_sizeof);
	iterator _begin = begin();
	//iterator _end = end();
	for (; aBegin != aEnd; ++aBegin)
	{
		for(unsigned i=0;i<_sizeof;++i, ++_begin)
		*_begin = reinterpret_cast<value_type const*>(&(*aBegin))[i];
	}
}
inline bool operator==(const CBuffer& __lhs, const CBuffer& __rhs)
{
	if(__lhs.MIsNull() && __rhs.MIsNull())
		return true;
	if(__lhs.MReferenceEqual(__rhs))
		return true;
	if (__lhs.size() != __rhs.size())
		return false;	
	CBuffer::const_iterator _begin = __lhs.begin();
	CBuffer::const_iterator _end = __lhs.end();
	CBuffer::const_iterator _begin2 = __rhs.begin();
	for (; _begin != _end; ++_begin, ++_begin2)
		if (*_begin != *_begin2)
			return false;
	return true;
}
inline bool operator!=(const CBuffer& __lhs, const CBuffer& __rhs)
{
	return !operator==(__lhs, __rhs);
}
template<typename TPod>
class  CPODBuffer
{
public:
	typedef CBuffer::offset_pointer_t offset_pointer_t;
	typedef buf_val_t<TPod> value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef CBuffer::size_type size_type;
	typedef CBuffer::difference_type difference_type;
	typedef iterator_type<pointer, reference, difference_type> iterator;
	typedef iterator_type<const_pointer, const_reference, difference_type> const_iterator;
	typedef CBuffer::allocator_type allocator_type;

	CPODBuffer(IAllocater* aAlloc,eAllocatorType aType=ALLOCATE_FROM_COMMON):
		FBuffer(0,0,aAlloc,aType)//
	{
		;
	}
	CPODBuffer(size_t aSize=0,int aBeginSize=0,IAllocater* aAlloc=NULL,eAllocatorType aType=ALLOCATE_FROM_COMMON):
		FBuffer(aSize*sizeof(TPod),aBeginSize*sizeof(TPod),aAlloc,aType)//
	{
		;
	}

//	template<class ItT>
//	CPODBuffer(IAllocater* aAlloc,ItT aBegin, ItT aEnd,eAllocatorType=ALLOCATE_FROM_COMMON);
//	template<class TPointer>
//	CPODBuffer(IAllocater* aAlloc, TPointer* aBegin, TPointer* aEnd, eAllocatorType = ALLOCATE_FROM_COMMON);

	CPODBuffer(IAllocater* aAlloc, void const* aBegin, void const* aEnd,eAllocatorType aType=ALLOCATE_FROM_COMMON):
		FBuffer(aAlloc,aBegin,aEnd,aType)//
	{
		;
	}
	CPODBuffer(IAllocater& aAlloc, offset_pointer_t Offset,bool aCheckCrc=true,eAllocatorType aType=ALLOCATE_FROM_COMMON):
		FBuffer(aAlloc,Offset,aCheckCrc,aType)//
	{
		;
	}
	CPODBuffer(CBuffer const& aBuf):
		FBuffer(aBuf)
	{

	}
	CPODBuffer(CBuffer& aBuf,bool aMoveIt)
	{
		aBuf.MMoveTo(FBuffer);
	}
	CPODBuffer const& deep_copy(const CPODBuffer& aBuf)
	{
		FBuffer.deep_copy(aBuf);
		return *this;
	}
	void MMoveTo(CPODBuffer& aTo)
	{
		FBuffer.MMoveTo(aTo);
	}

	size_type size() const
	{
		return FBuffer.size()/sizeof(TPod);
	}
	size_type max_size() const
	{
		return FBuffer.max_size()/sizeof(TPod);
	}
	size_type capacity() const
	{
		return FBuffer.capacity() / sizeof(TPod);
	}
	size_type begin_capacity() const
	{
		return FBuffer.begin_capacity() / sizeof(TPod);
	}
	bool empty() const
	{
		return FBuffer.empty();
	}
	iterator begin()
	{
		return (pointer)FBuffer.begin().base();
	}
	iterator end()
	{
		return (pointer)FBuffer.end().base();
	}
	const_iterator begin() const
	{
		return (const_pointer)FBuffer.begin().base();
	}
	const_iterator end() const
	{
		return (const_pointer)FBuffer.end().base();
	}
	const_iterator cbegin() const
	{
		return begin();
	}
	const_iterator cend() const
	{
		return cend();
	}

	void swap(CPODBuffer& aBuf)
	{
		FBuffer.swap(aBuf);
	}
	//reference operator[](size_type __n);//for safety
	const_reference operator[](size_type __n) const
	{
		return reinterpret_cast<const_reference>(FBuffer[__n*sizeof(TPod)]);
	}

//	reference front(); //for safety
	const_reference front() const
	{
		return reinterpret_cast<const_reference>(FBuffer.front());
	}
//	reference back();//for safety
	const_reference back() const
	{
		return reinterpret_cast<const_reference>(FBuffer.back());
	}
	void resize(size_type __new_size, bool fromBegin = false,bool aCanDetach=true)//bug fix. aCanDetach - kostil'!
	{
		FBuffer.resize(__new_size* sizeof(TPod), fromBegin, aCanDetach);
	}

	void clear()
	{
		FBuffer.clear();
	}

	void insert(iterator __position, size_type __n, value_type const& aVal)
	{
		CBuffer::iterator const _pos((CBuffer::pointer) __position.base());
		FBuffer.insert(_pos, __n, aVal);
	}
	void insert(iterator __position, value_type const& aVal)
	{
		CBuffer::iterator const _pos((CBuffer::pointer) __position.base());
		FBuffer.insert(_pos, aVal);
	}

	void insert(iterator __position, iterator aBegin, iterator aEnd)
	{
		CBuffer::iterator const _pos((CBuffer::pointer) __position.base());
		FBuffer.insert(_pos, aBegin.base(),aEnd.base());
	}
	void insert(iterator __position, const_iterator aBegin,
			const_iterator aEnd)
	{
		CBuffer::iterator const _pos((CBuffer::pointer) __position.base());
		FBuffer.insert(_pos, aBegin.base(),aEnd.base());
	}
	iterator erase(iterator aBegin, iterator aEnd)
	{
		CBuffer::iterator const _begin((CBuffer::pointer) aBegin.base());
		return iterator((pointer)FBuffer.erase(_begin,_begin+(aEnd-aBegin)*sizeof(TPod)));
	}
	void push_back(const value_type& __x)
	{
		if(sizeof(TPod)==1)
			FBuffer.push_back(__x);
		else
			FBuffer.insert(FBuffer.end(), __x);
	}
	void pop_back()
	{
		if (sizeof(TPod) == 1)
			FBuffer.pop_back();
		else if (FBuffer.size() == 1)
			FBuffer.clear();
		else
		{
			CBuffer::iterator const _begin((CBuffer::pointer) ((end() - 1)).base());
			FBuffer.erase(_begin,FBuffer.end());
		}
	}
	void push_front(const value_type& __x)
	{
		if(sizeof(TPod)==1)
			FBuffer.push_front(__x);
		else
			FBuffer.insert(FBuffer.begin(), __x);
	}
	void pop_front()
	{
		if(sizeof(TPod)==1)
			FBuffer.pop_front();
		else if(FBuffer.size()==1)
			FBuffer.clear();
		else
			FBuffer.erase(FBuffer.begin(), FBuffer.begin() + sizeof(TPod));
	}
	void release()
	{
		FBuffer.release();
	}
	void reserve(size_t aSize)
	{
		FBuffer.reserve(aSize*sizeof(TPod));
	}
	const_pointer ptr_const() const
	{
		return reinterpret_cast<const_pointer>(FBuffer.ptr_const());
	}

	pointer ptr()
	{
		return reinterpret_cast<pointer>(FBuffer.ptr());
	}
	offset_pointer_t refuse()
	{
		return FBuffer.refuse();
	}

	offset_pointer_t offset() const
	{
		return FBuffer.offset();
	}

	unsigned use_count() const
	{
		return FBuffer.use_count();
	}
	bool MIsDetached() const
	{
		return FBuffer.MIsDetached();
	}
	bool MIsAllocatorEqual(IAllocater* aAlloc) const
	{
		return FBuffer.MIsAllocatorEqual(aAlloc);
	}
	bool MIsAllocatorEqual(CPODBuffer const& aPod) const
	{
		return FBuffer.MIsAllocatorEqual(aPod.FBuffer);
	}
	IAllocater* MAllocator() const
	{
		return FBuffer.MAllocator();
	}
	bool MIsRestored() const
	{
		return FBuffer.MIsRestored();
	}
	CBuffer const & MGetBuffer() const
	{
		return FBuffer;
	}
	operator CBuffer const() const
	{
		return MGetBuffer();
	}
private:
	CBuffer FBuffer;
};
} /* namespace NSHARE */
namespace std
{
inline void swap(NSHARE::CBuffer& aBuf, NSHARE::CBuffer& aBuf2)
{
	aBuf.swap(aBuf2);
}
inline std::ostream& operator<<(std::ostream& aStream,
		NSHARE::CBuffer const& aVal)
{
	aStream << "Buffer size =" << aVal.size() << ",\n";
	NSHARE::print_buffer(aStream, aVal.begin(), aVal.end());
	return aStream;
}
}
#endif /* CBUFFER_H_ */
