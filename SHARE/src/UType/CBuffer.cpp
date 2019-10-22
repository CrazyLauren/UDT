// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CBuffer.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 14.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <SHARE/crc8.h>

#include <SHARE/UType/CCommonAllocater.h>
#include <SHARE/UType/CBuffer.h>
#include <SHARE/UType/mallocallocater.h>

#ifdef NOATOMIC
#	define SAFETY_BUFFER_OPERATION
#	define SAFETY_BUFFER_READ_OPERATION
#endif

//#ifdef NDEBUG
//#	define BUFFER_NO_CHECK_CRC
//#endif

namespace NSHARE
{
COMPILE_ASSERT(sizeof(IAllocater::offset_pointer_t)==sizeof(CBuffer::offset_pointer_t) ,
		INVALID_SIZEOF_BUF_OFFSET);
//static CMutex g_mutex;
#define BUF_FACTOR 1.5//fixme
const size_t CBuffer::DEF_BUF_RESERVE = 256; //fixme

#define BUF_CRC_SIZE (sizeof(uint32_t)*2)//only  2 DWORD
#define BUF_CRC_BEGIN (sizeof(uint32_t))// from 4 bytes

enum eBufFlags
{
	E_BUF_RESORED = 0x1 << 0
};
struct raw_begin_offset_t
{
	raw_begin_offset_t(uint32_t const& aWhat) :
			FRawField(aWhat)
	{

	}
	union
	{

		mutable uint32_t FRawField;
		struct
		{
			mutable volatile uint32_t FBeginOffset :16;
			mutable volatile uint32_t FFlags :8; //see eBufFlags
			mutable volatile uint32_t FCrc :8;
		};
	};
	uint32_t & MSetOffset(size_t const& aVal) const
	{
		FBeginOffset = aVal;
		return FRawField;
	}
	uint32_t & MSetCrc(uint8_t const& aVal) const
	{
		FCrc = aVal;
		return FRawField;
	}
	uint32_t & MInvalid() const
	{
		FRawField = 0;
		FCrc = 0x2;
		return FRawField;
	}
	uint32_t & MSetFlag(eBufFlags const& aFlag, bool aVal) const
	{
		FFlags = (aVal) ? (FFlags | aFlag) : (FFlags & (~aFlag));
		return FRawField;
	}
};

SHARED_PACKED(struct CBuffer::buf_info
{
private:
	typedef NSHARE::crc8_t<0x97> crc_t;

	mutable atomic_t FRawOffsetField;
	mutable atomic_t FBufSize;
	mutable atomic_t FSize;
	mutable atomic_t FCount;

	buf_info(buf_info const &);
	buf_info & operator=(buf_info const &);
public:
	buf_info(size_t const& aSize);
	size_t MStartOffset() const;
	void MSetStartOffset(size_t const& aVal, bool aIsSetCrc = true);
	size_t MSize() const;
	void MSetSize(size_t const& aVal);
	size_t MBufSize();
	void MSetBufSize(size_t const& aVal);
	uint32_t use_count() const;
	void add_ref_copy();

	uint32_t release();
	bool MCheckCrc() const;
	void MSetCrc();
	void MInvalid();

	bool MIs(eBufFlags aFlag) const;
	void MSetFlag(eBufFlags const& aFlag, bool aVal) const;

});
COMPILE_ASSERT(sizeof(CBuffer::buf_info) ==4*sizeof(atomic_t),
		IVALID_SIZEOF_BUF_INFO);
CBuffer::buf_info::buf_info(size_t const& aSize)
{
//	CHECK_NOTNULL(this);
	FRawOffsetField = 0;
	FSize = 0;
	FBufSize = 0;
	FCount = 1;
	VLOG(5) << "Add ref copy old 0. p=" << this;
	MSetBufSize(aSize);
}
size_t CBuffer::buf_info::MSize() const
{
//	CHECK_NOTNULL(this);
	return FSize;
}
void CBuffer::buf_info::MSetSize(size_t const& aVal)
{
//	CHECK_NOTNULL(this);
	CHECK_LE(aVal, std::numeric_limits<uint32_t>::max());
	FSize.MWrite((atomic_t::value_type)aVal);
	MSetCrc();
}
size_t CBuffer::buf_info::MBufSize()
{
//	CHECK_NOTNULL(this);
	return FBufSize;
}
void CBuffer::buf_info::MSetBufSize(size_t const& aVal)
{
//	CHECK_NOTNULL(this);
	CHECK_LE(aVal, std::numeric_limits<uint32_t>::max());
	FBufSize.MWrite((atomic_t::value_type)aVal);
	MSetCrc();
}
uint32_t CBuffer::buf_info::use_count() const
{
//	CHECK_NOTNULL(this);
	return FCount;
}
void CBuffer::buf_info::add_ref_copy()
{
//	CHECK_NOTNULL(this);
	++FCount;
	//MSetCrc();
}

uint32_t CBuffer::buf_info::release()
{
//	CHECK_NOTNULL(this);
	CHECK_GT(use_count(), 0);
	return FCount--;
	//MSetCrc();
}

inline size_t CBuffer::buf_info::MStartOffset() const
{
	//CHECK(MCheckCrc());
	return raw_begin_offset_t(FRawOffsetField).FBeginOffset;
}
inline void CBuffer::buf_info::MSetStartOffset(size_t const& aVal,
		bool aIsSetCrc)
{
	CHECK_LE(aVal, std::numeric_limits<uint32_t>::max());
#ifdef BUFFER_NO_CHECK_CRC
	FStartOffset.MWrite((static_cast<uint32_t>(aVal) <<8)&OF_MASK);
#else
	FRawOffsetField.MWrite(
			raw_begin_offset_t(FRawOffsetField).MSetOffset(aVal));
#endif
	//if(aIsSetCrc)MSetCrc();
}

inline bool CBuffer::buf_info::MCheckCrc() const
{
#ifndef BUFFER_NO_CHECK_CRC
//	uint8_t _crc[BUF_CRC_SIZE];
//	memcpy(_crc, this, sizeof(_crc));
//	uint8_t const*  _begin = (uint8_t*) _crc;

//	uint8_t const _cur = *_begin++;
	uint8_t const _crc = raw_begin_offset_t(FRawOffsetField).FCrc;

#ifdef USE_CRC_FOR_CHECK_BUF
	uint8_t const* _begin = ((uint8_t*) this) +BUF_CRC_BEGIN;
	uint8_t const * const _end = _begin + BUF_CRC_SIZE;
	uint8_t const _r = crc_t::sMCalcCRCofBuf(_begin, _end);
	return _crc == _cur;
#endif
	typedef uint8_t const _array_t[BUF_CRC_SIZE];

	_array_t const &_array =
			*reinterpret_cast<_array_t*>(((uint8_t const*) this) + BUF_CRC_BEGIN);
	uint8_t const _sum = _array[0] ^ _array[1] ^ _array[2] ^ _array[3]
			^ _array[4] ^ _array[5] ^ _array[6] ^ _array[7];
	COMPILE_ASSERT(8==BUF_CRC_SIZE,
			TheSizeOfArrayChangedButXorCrcIsNotCorrected);
	return _sum == _crc;
#else
	return true;
#endif
}
inline void CBuffer::buf_info::MInvalid()
{
	FRawOffsetField.MWrite(raw_begin_offset_t(FRawOffsetField).MInvalid());
}
inline void CBuffer::buf_info::MSetCrc()
{
#ifndef BUFFER_NO_CHECK_CRC
//	uint8_t _crc[BUF_CRC_SIZE];
//	memcpy(_crc, this, sizeof(_crc));
//	uint8_t const*  _begin = (uint8_t*) _crc;

#ifdef USE_CRC_FOR_CHECK_BUF
	uint8_t const* _begin = ((uint8_t*) this) +BUF_CRC_BEGIN;
	uint8_t const * const _end = _begin + BUF_CRC_SIZE;
	uint8_t const _r = crc_t::sMCalcCRCofBuf(_begin, _end);
#else
	typedef uint8_t const _array_t[BUF_CRC_SIZE];
	_array_t const &_array =
			*reinterpret_cast<_array_t*>(((uint8_t const*) this) + BUF_CRC_BEGIN);
	uint8_t const _r = _array[0] ^ _array[1] ^ _array[2] ^ _array[3] ^ _array[4]
			^ _array[5] ^ _array[6] ^ _array[7];
	COMPILE_ASSERT(8==BUF_CRC_SIZE,
			TheSizeOfArrayChangedButXorCrcIsNotCorrected);
//	uint8_t  _r= *_begin++;
//	for (; _begin != _end; ++_begin)
//		_r ^= *_begin;
#endif
	FRawOffsetField.MWrite(raw_begin_offset_t(FRawOffsetField).MSetCrc(_r));
#endif
}
bool CBuffer::buf_info::MIs(eBufFlags aFlag) const
{
	return (raw_begin_offset_t(FRawOffsetField).FFlags & aFlag) != 0;
}
void CBuffer::buf_info::MSetFlag(eBufFlags const& aFlag, bool aVal) const
{
	FRawOffsetField.MWrite(
			raw_begin_offset_t(FRawOffsetField).MSetFlag(aFlag, aVal));
	//MSetCrc()
}
#ifdef SAFETY_BUFFER_OPERATION
template<> class CRAII<CBuffer::buf_info> :public CRAII<IAllocater>
{
public:
	explicit CRAII(IAllocater * aMutex, void* aBuf):CRAII<IAllocater>(aMutex, aBuf)
	{

	}
};
#else
template<> class CRAII<CBuffer::buf_info> : public CDenyCopying
{
public:
	explicit CRAII(IAllocater * aMutex, void* aBuf)
	{

	}
	~CRAII()
	{

	}
	inline void MUnlock()
	{
	}
};
#endif
typedef CRAII<CBuffer::buf_info> _buffer_safety_operation_t;

const CBuffer::size_type CBuffer::_buffer_t::BUF_OFFSET =
		_impl::get_alligment_size_ref<CBuffer::buf_info, CBuffer::value_type>();

CBuffer::_buffer_t::_buffer_t(IAllocater* aAll, eAllocatorType aType) :
		FAllocator(aAll), FBeginOfStorage(NULL), FAllocatorType(aType)
{
}
CBuffer::_buffer_t::_buffer_t(IAllocater* aAlloc, offset_pointer_t aOffset,
		bool aCheckCrc, eAllocatorType aType) :
		FAllocator(aAlloc), //
		FAllocatorType(aType)
{
	VLOG(2) << "Allocate by " << aOffset;
	void* _p = FAllocator->MPointer(aOffset);
	LOG_IF(DFATAL,!_p) << "The offset " << aOffset << " is null";
	if (_p)
	{
		FBeginOfStorage = MGetPtr(_p);
		_buffer_safety_operation_t _block(FAllocator, FBeginOfStorage);
		buf_info& _buf = sMGetBuff(FBeginOfStorage);
		if ((aCheckCrc && !_buf.MCheckCrc()) || _buf.use_count() == 0)
		{
			VLOG(2) << "Invalid CRC Use Count=" << _buf.use_count();
			FBeginOfStorage = NULL;
		}
		else
		{
			VLOG(2) << "The buffer is exist in " << aOffset
								<< " use count = " << _buf.use_count() << " p="
								<< this;
			_buf.MSetFlag(E_BUF_RESORED, true);
		}
	}
	else
		FBeginOfStorage = NULL;

}
CBuffer::_buffer_t::_buffer_t(_buffer_t const & aR) :
		FAllocatorType(aR.FAllocatorType)
{
	if (aR.FBeginOfStorage != NULL)
		aR.add_ref_copy(true);

	FBeginOfStorage = aR.FBeginOfStorage;
	FAllocator = aR.FAllocator;
}
CBuffer::_buffer_t::~_buffer_t()
{
	if (!empty())
		release();
}
CBuffer::value_type* CBuffer::_buffer_t::MGetPtr(void* aVal) const
{
	return (value_type*) ((uint8_t*) aVal + BUF_OFFSET);
}
CBuffer::buf_info& CBuffer::_buffer_t::MGetBuff() const
{
	CHECK_NOTNULL(FBeginOfStorage);
	return sMGetBuff(FBeginOfStorage);
}
CBuffer::value_type* CBuffer::_buffer_t::MStorageBegin() const
{
	CHECK_NOTNULL(FBeginOfStorage);
	return (value_type*) ((uint8_t*) FBeginOfStorage - BUF_OFFSET);
}
CBuffer::buf_info& CBuffer::_buffer_t::sMGetBuff(value_type* aVal)
{
	CHECK_NOTNULL(aVal);
	return *(buf_info*) ((uint8_t*) aVal - BUF_OFFSET);
}
CBuffer::value_type* CBuffer::_buffer_t::MReAllocate(value_type* aP,
		size_t aSize)
{
	void* _ptr =
			aP != NULL ?
					FAllocator->MReallocate(aP - BUF_OFFSET, aSize + BUF_OFFSET,
							4, FAllocatorType) :
					FAllocator->MReallocate(
					NULL, aSize + BUF_OFFSET, 4, FAllocatorType);
	LOG_IF(ERROR,!_ptr) << "Cannot allocate " << aSize << " bytes.";

	if (!_ptr)
		return NULL;
	buf_info* _p = (buf_info*) _ptr;
	_p->MSetBufSize(aSize);
	return MGetPtr(_ptr);
}
CBuffer::value_type* CBuffer::_buffer_t::MAllocate(size_t aSize)
{
	void* _ptr = FAllocator->MAllocate(aSize + BUF_OFFSET, 4, FAllocatorType);
	LOG_IF(ERROR,!_ptr) << "Cannot allocate " << aSize << " bytes.";

	if (!_ptr)
		return NULL;

	new (_ptr) buf_info(aSize);
	VLOG(5) << " new buf for " << FAllocator->MOffset(_ptr);
	return MGetPtr(_ptr);
}
void CBuffer::_buffer_t::MDeallocate()
{
	if (!empty())
	{
		size_type _size = 0;
		{
			_buffer_safety_operation_t _block(FAllocator, FBeginOfStorage);
			buf_info& _buf = sMGetBuff(FBeginOfStorage);
			_size = _buf.MBufSize();
			_buf.MInvalid();
		}
		MDeallocate(FBeginOfStorage, _size);
	}
}
void CBuffer::_buffer_t::MDeallocate(value_type* aP, size_t __n)
{
	if (aP)
		FAllocator->MDeallocate(&sMGetBuff(aP), __n);
}
CBuffer::size_type CBuffer::_buffer_t::size() const
{
	if (empty())
		return 0;
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FAllocator, FBeginOfStorage); //it's atomic
#endif
	return sMGetBuff(FBeginOfStorage).MSize();
}

uint32_t CBuffer::_buffer_t::use_count() const
{
	if (empty())
		return 0;
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FAllocator, FBeginOfStorage); //it's atomic
#endif
	return sMGetBuff(FBeginOfStorage).use_count();
}

bool CBuffer::_buffer_t::unique() const
{
	return use_count() == 1;
}
void CBuffer::_buffer_t::release()
{
	if (empty())
	{
		LOG(ERROR)<< "Empty release.";
		return;
	}
	bool _does_deallocate=false;
	buf_info& _buf = sMGetBuff(FBeginOfStorage);
	VLOG(5)<<"Release for "<<FAllocator->MOffset(MStorageBegin())<<" p="<<this;
	{
#ifdef SAFETY_BUFFER_READ_OPERATION
		_buffer_safety_operation_t _block(FAllocator, FBeginOfStorage); //it's atomic
#endif
		uint32_t const _release=_buf.release();
		VLOG(2) << "Release = " << _release<<" for "<<FAllocator->MOffset(MStorageBegin());
		_does_deallocate=_release == 1;
	}
	if (_does_deallocate)
	{
		MDeallocate();
	}
	FBeginOfStorage=NULL;
}
void CBuffer::_buffer_t::add_ref_copy(bool aIsExist) const
{
	if (empty())
	{
		LOG(ERROR)<< "Empty release.";
		return;
	}

	_buffer_safety_operation_t _block(FAllocator, FBeginOfStorage);
	VLOG(5)<<"Add ref copy "<<FAllocator->MOffset(MStorageBegin())<<" old="<<sMGetBuff(FBeginOfStorage).use_count()<<" p="<<this;
	sMGetBuff(FBeginOfStorage).add_ref_copy();
}
bool CBuffer::_buffer_t::empty() const
{
	return FBeginOfStorage == NULL;
}
void CBuffer::_buffer_t::swap(_buffer_t& aTo)
{
	VLOG(3) << "Swap";
	if (aTo.FBeginOfStorage != FBeginOfStorage)
	{
		std::swap(FAllocator,aTo.FAllocator);
		std::swap(FBeginOfStorage,aTo.FBeginOfStorage);
		std::swap(FAllocatorType,aTo.FAllocatorType);
	}
}
void CBuffer::_buffer_t::MMoveTo(_buffer_t& aTo)
{
	VLOG(2) << "Move data ";
	if (aTo.FBeginOfStorage != FBeginOfStorage)
	{
		if (aTo.FBeginOfStorage != 0)
			aTo.release();

		aTo.FBeginOfStorage = FBeginOfStorage;
		aTo.FAllocator = FAllocator;
		FBeginOfStorage = NULL;
	}
}
CBuffer::_buffer_t & CBuffer::_buffer_t::operator=(_buffer_t const & r)
{
	if (r.FBeginOfStorage != FBeginOfStorage)
	{
		CHECK_NE(this, &r);
		{
			if (FBeginOfStorage != NULL)
				release();

			if (r.FBeginOfStorage != NULL)
				r.add_ref_copy(true);

			FBeginOfStorage = r.FBeginOfStorage;
			FAllocator = r.FAllocator;
		}
	}
	return *this;
}
CBuffer::_buffer_t &CBuffer::_buffer_t::deep_copy(_buffer_t const & r)
{
	CHECK_NOTNULL(FAllocator);
	if (r.FBeginOfStorage != FBeginOfStorage)
	{
		CHECK_NE(this, &r);
		size_t _new_offset = 0;
		size_t _r_offset = 0;
		size_t _r_size = 0;
		{
			if (FBeginOfStorage != 0)
				release();

			FBeginOfStorage = NULL;

			buf_info const& _r_info = r.MGetBuff();
			_buffer_safety_operation_t _block(r.FAllocator, r.FBeginOfStorage);
			CHECK(_r_info.MCheckCrc());
			value_type* _buf_p = MAllocate(_r_info.MSize());
			CHECK_NOTNULL(_buf_p);

			buf_info& _new_info = sMGetBuff(_buf_p);
			_new_info.MSetSize(_r_info.MSize());
			FBeginOfStorage = _buf_p;
			_new_offset = _new_info.MStartOffset();
			_r_offset = _r_info.MStartOffset();
			_r_size = _r_info.MSize();
		}
		memcpy((uint8_t*) FBeginOfStorage + _new_offset,
				(uint8_t*) r.FBeginOfStorage + _r_offset, _r_size);

	}
	return *this;
}
bool CBuffer::_buffer_t::MIsRestored() const
{
	return FBeginOfStorage && MGetBuff().MIs(E_BUF_RESORED);
}
CBuffer::CBuffer(size_type aSize, int  aBeginSize, IAllocater* aAlloc,
		eAllocatorType aType) :
		BEGIN_SIZE(aBeginSize < 0 ? DEF_BUF_RESERVE : aBeginSize), //
		FBuffer(aAlloc ? aAlloc : sMDefAllaocter(), aType), //
		FIsDetached(false) //
{
	if (aSize)
		resize(aSize);
}
CBuffer::CBuffer(IAllocater* aAlloc, eAllocatorType aType) :
		BEGIN_SIZE(DEF_BUF_RESERVE), //
		FBuffer(aAlloc ? aAlloc : sMDefAllaocter(), aType), //
		FIsDetached(false)
{
}
CBuffer::CBuffer(IAllocater& aAlloc, offset_pointer_t Offset, bool aCheckCrc,
		eAllocatorType aType) :
		BEGIN_SIZE(DEF_BUF_RESERVE), //
		FBuffer(&aAlloc, Offset, aCheckCrc, aType), //
		FIsDetached(false)
{
	;
}
CBuffer::~CBuffer()
{
}
CBuffer::CBuffer(const CBuffer& aRht) :
		BEGIN_SIZE(aRht.BEGIN_SIZE), FBuffer(aRht.FBuffer), FIsDetached(
				aRht.FIsDetached)
{
	;
}
void CBuffer::MMoveTo(CBuffer& aTo)
{
	FBuffer.MMoveTo(aTo.FBuffer);
	FIsDetached = aTo.FIsDetached;
}

CBuffer const& CBuffer::operator=(const CBuffer& aRht)
{
	FBuffer = aRht.FBuffer;
	FIsDetached = aRht.FIsDetached;
	return *this;
}
CBuffer const& CBuffer::deep_copy(const CBuffer& aRht)
{
	FBuffer.deep_copy(aRht.FBuffer);
	return *this;
}
CBuffer::size_type CBuffer::size() const
{
	if (FBuffer.empty())
		return 0;
	buf_info const& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return size_type(_info.MSize());
}
CBuffer::size_type CBuffer::max_size() const
{
	return get_allocator().MMaxSize();
}
CBuffer::size_type CBuffer::capacity() const
{
	if (FBuffer.empty())
		return 0;
	buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return _info.MBufSize();
}
bool CBuffer::empty() const
{
	return size() == 0;
}
CBuffer::iterator CBuffer::begin()
{
	if (FBuffer.empty())
		return CBuffer::iterator();
	MDetach();
	buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return CBuffer::iterator(FBuffer.FBeginOfStorage + _info.MStartOffset());
}
CBuffer::const_iterator CBuffer::begin() const
{
	if (FBuffer.empty())
		return CBuffer::const_iterator();

	buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return CBuffer::const_iterator(
			FBuffer.FBeginOfStorage + _info.MStartOffset());
}
CBuffer::iterator CBuffer::end()
{
	if (FBuffer.empty())
		return CBuffer::iterator();

	buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return CBuffer::iterator(
			FBuffer.FBeginOfStorage + _info.MStartOffset() + _info.MSize());
}
CBuffer::const_iterator CBuffer::end() const
{
	if (FBuffer.empty())
		return CBuffer::const_iterator();

	buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return CBuffer::const_iterator(
			FBuffer.FBeginOfStorage + _info.MStartOffset() + _info.MSize());
}
CBuffer::reference CBuffer::at(size_type __n)
{
	MDetach();
	CHECK_LT(__n, size());
	CHECK(!FBuffer.empty());
	return *(begin() + __n);
}
CBuffer::const_reference CBuffer::operator[](size_type __n) const
{
	CHECK_LT(__n, size());
	CHECK(!FBuffer.empty());
	return *(begin() + __n);
}
CBuffer::const_reference CBuffer::at(size_type __n) const
{
	return (*this)[__n];
}
//CBuffer::reference CBuffer::front()
//{
//	CHECK(!FBuffer.empty());
//	MDetach();
//	return *begin();
//}
CBuffer::const_reference CBuffer::front() const
{
	CHECK(!FBuffer.empty());
	return *begin();
}
//CBuffer::reference CBuffer::back()
//{
//	CHECK(!FBuffer.empty());
//	MDetach();
//	return *(end() - 1);
//}
CBuffer::const_reference CBuffer::back() const
{
	CHECK(!FBuffer.empty());
	return *(end() - 1);
}
void CBuffer::resize(size_type aNewSize, bool fromBegin, bool aCanDetach)
{
	if (FBuffer.empty() && aNewSize)
	{
		value_type* _buf_p = MAllocate(aNewSize);
		if (!_buf_p)
		{
			LOG(ERROR)<<"Cannot resize buffer."<<FBuffer.FAllocator->MMaxSize();
			return;
		}
		{
			//_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);//it's the new buffer
			buf_info& _new_info = _buffer_t::sMGetBuff(_buf_p);
			_new_info.MSetSize(aNewSize);
		}
		FBuffer.FBeginOfStorage = _buf_p;
	}
	else if (aNewSize < size())
	{
		if (!aNewSize)
			release();
		else
		{
			if (aCanDetach)
				MDetach();
			{
				_buffer_safety_operation_t _block(FBuffer.FAllocator,
						FBuffer.FBeginOfStorage);
				buf_info& _info = FBuffer.MGetBuff();
				CHECK(_info.MCheckCrc());
				if (!fromBegin)
					_info.MSetSize(aNewSize);
				else
					_info.MSetStartOffset(
							_info.MStartOffset() + size() - aNewSize);
			}
		}
	}
	else if (aNewSize != size())
		MInsertImpl(fromBegin ? begin() : end(), aNewSize - size(), aCanDetach);
}
void CBuffer::insert(iterator __position, size_type __n, value_type const& __x)
{
	pointer _p = MInsertImpl(__position, __n);
	if (_p)
		MFill(_p, __x, __n);
}
void CBuffer::insert(iterator __position, value_type const& __x)
{
	return insert(__position, 1, __x);
}
void CBuffer::insert(iterator __position, iterator aBegin, iterator aEnd)
{
	pointer _p = MInsertImpl(__position, aEnd - aBegin);
	if (_p)
		MFill(_p, aBegin.base(), aEnd.base());
}
void CBuffer::insert(iterator __position, const_iterator aBegin,
		const_iterator aEnd)
{
	CHECK(aEnd > aBegin);
	pointer _p = MInsertImpl(__position, aEnd - aBegin);
	if (_p)
		MFill(_p, aBegin.base(), aEnd.base());
}

void CBuffer::MFill(pointer aPosition, value_type const& aVal, size_type aSize)
{
	memset(aPosition, aVal, aSize);
}
void CBuffer::MFill(pointer aPosition, const_pointer const& aBegin,
		const_pointer const& aEnd)
{
	CHECK_LE(aBegin, aEnd);
	memcpy(aPosition, aBegin, aEnd - aBegin);

}
CBuffer::pointer CBuffer::MInsertImpl(iterator aPosition, size_type aSize,
		bool aCanDetach)
{
	VLOG_IF(1,aSize == 0) << " aSize is zero.";
	if (aSize == 0)
		return NULL;
	pointer _position = NULL;
	if (FBuffer.FBeginOfStorage)
	{
		_buffer_safety_operation_t _block(FBuffer.FAllocator,
				FBuffer.FBeginOfStorage);
		//guarantee valid of varibales
		buf_info& _info = FBuffer.MGetBuff();
		CHECK(_info.MCheckCrc());
		size_type const _free_begin_mem = _info.MStartOffset();
		size_type const _cur_size = _info.MSize();

		pointer const _begin = FBuffer.FBeginOfStorage + _free_begin_mem;
		pointer const _end = _begin + _cur_size;
		_position = aPosition.base();
		size_type const _elems_befor = _position - _begin;
		size_type const _elems_after = _end - _position;
		size_type const _free_end_mem = _info.MBufSize() - _cur_size
				- _free_begin_mem;
		uint32_t const _use_count=_info.use_count();
		_block.MUnlock(); //<---- force inlock guarantee valid of

		const bool _insert_to_begin = (_elems_befor == 0
				&& _free_begin_mem >= aSize);

		VLOG_IF
		(1,_elems_befor == 0 && _free_begin_mem < aSize)
																		<< " reallocate memory.";
		NSHARE::smart_field_t<size_t> _new_start_offset;
		NSHARE::smart_field_t<size_t> _new_size;

		if (_insert_to_begin)
		{ //+
			if (aCanDetach)
				MDetach();

			_new_start_offset = _free_begin_mem - aSize;
			_new_size = _cur_size + aSize;
			_position -= aSize;
		}
		else if (_free_end_mem >= aSize) //there is  memory
		{
			const bool _begin_use_up = (_elems_befor == 0) 	//warning elements no copied from begin,
															//see if (_begin_use_up)
			//&& (!_insert_to_begin) //Commented as if (_insert_to_begin) below
					&& (_free_begin_mem < BEGIN_SIZE);

			VLOG_IF(1,_begin_use_up) << "The begin buffer is use up.";

			if (aCanDetach)
				MDetach();

			if (_elems_after == 0)
			{
				; //+
			}
			else if (_begin_use_up) //moving all
			{ //+
				const size_t _reserv = _free_end_mem - aSize;
				const size_t _move_on = std::min(_reserv,
						BEGIN_SIZE - _free_begin_mem);

				// copy suffix
				memcpy(_end + _move_on, _end - aSize, aSize);
				// move hole
				memmove(_position + aSize + _move_on, _position,
						_elems_after - aSize);
				// copy prefix
				//Comments as no elements from begin see _begin_use_up variable
				//memmove(_begin + _move_on, _begin, _elems_befor);

				_position += _move_on;
				_new_start_offset = _free_begin_mem + _move_on;
			}
			else if (_elems_after > aSize) // new stuff can all be assigned
			{ //+
			  // copy suffix
				memcpy(_end, _end - aSize, aSize);

				// copy hole
				memmove(_position + aSize, _position, _elems_after - aSize);
			}
			else
				//+
				// new stuff spills off end
				memcpy(_position + aSize, _position, _elems_after);

			_new_size = _cur_size + aSize;
		}
		else if ((_free_end_mem + _free_begin_mem) >= aSize) //there is  memory in begin
		{ //+
			if (aCanDetach)
				MDetach();

			const ssize_t _move_on = 0 - _free_begin_mem;
			// move prefix
			memmove(_begin + _move_on, _begin, _elems_befor);

			// move suffix
			memmove(_position + aSize + _move_on, _position, _elems_after);

			_position += _move_on;

			_new_start_offset = _free_begin_mem + _move_on;
			_new_size = _cur_size + aSize;

		}
		else //alocate new buffer
		{
			LOG_IF(ERROR,_use_count>1) << "The buffer is held by "
														<< _use_count
														<< " holders."
																" The current operation  can be applied to only one holder.";

			if (_use_count>1)
			{
				value_type* _buf_p = MAllocate(_cur_size + aSize);
				if (_buf_p != NULL)
				{
					buf_info& _new_info = _buffer_t::sMGetBuff(_buf_p);
					//_new_size=_info.MSize() + aSize;
					_new_info.MSetSize(_cur_size + aSize);
					pointer _data_ptr = _buf_p + _new_info.MStartOffset();

					if (_elems_befor > 0)
					{
						memcpy(_data_ptr, _begin, _elems_befor);
						_data_ptr += _elems_befor;
					}

					pointer _p_coping = _data_ptr;

					_data_ptr += aSize;

					if (_elems_after > 0)
					{
						memcpy(_data_ptr, _position, _elems_after);
						_data_ptr += _elems_after;
					}
					CHECK_EQ(
							(size_t )(_data_ptr - _buf_p
									- _new_info.MStartOffset()),
							_cur_size + aSize);
					{
						_buffer_safety_operation_t _block(FBuffer.FAllocator,
								FBuffer.FBeginOfStorage);
						FBuffer.release();
					}
					FBuffer.FBeginOfStorage = _buf_p;

					_position = _p_coping;
				}
				else
				{
					LOG(DFATAL)<<"Cannot allocate buffer";
				}
			}
			else
			{
				value_type* _buf_p = MReAllocate(FBuffer.FBeginOfStorage,_cur_size + aSize);
				if(_buf_p)
				{
					buf_info& _new_info = _buffer_t::sMGetBuff(_buf_p);
					_new_info.MSetSize(_new_info.MSize() + aSize);

					pointer _data_ptr = _buf_p + _new_info.MStartOffset();
					_data_ptr += _elems_befor;

					pointer _p_coping = _data_ptr;

					_data_ptr += aSize;
					if(_elems_after)
					{
						memmove(_data_ptr, _p_coping, _elems_after);
						_data_ptr += _elems_after;
					}

					CHECK_EQ(
							(size_t )(_data_ptr - _buf_p- _new_info.MStartOffset()),_new_info.MSize()); //

					_position = _p_coping;
					FBuffer.FBeginOfStorage = _buf_p;

				}
				else
				{
					LOG(DFATAL)<<"Cannot allocate buffer";
				}

			}

		}
		if (_new_start_offset.MIs() || _new_size.MIs())
		{
			_buffer_safety_operation_t _block(FBuffer.FAllocator,
					FBuffer.FBeginOfStorage);
			CHECK_EQ(_free_begin_mem, _info.MStartOffset());
			if (_new_start_offset.MIs())
				_info.MSetStartOffset(_new_start_offset.MGetConst(),
						!_new_size.MIs());
			if (_new_size.MIs())
				_info.MSetSize(_new_size.MGetConst());
		}

	}
	else
	{
		value_type* _buf_p = MAllocate(aSize);
		buf_info& _new_info = _buffer_t::sMGetBuff(_buf_p);
		_new_info.MSetSize(aSize);
		FBuffer.FBeginOfStorage = _buf_p;
		_position = _buf_p + _new_info.MStartOffset();
	}
	//MFill(aVal, _position, aSize);
	return _position;
}
void CBuffer::clear()
{
	if (FBuffer.empty())
		return;
	MDetach();
	CHECK(!FBuffer.empty());
	{
		_buffer_safety_operation_t _block(FBuffer.FAllocator,
				FBuffer.FBeginOfStorage);
		buf_info& _info = FBuffer.MGetBuff();
		_info.MSetSize(0);
	}
}
void CBuffer::pop_back()
{
	if (empty())
		return;
	MDetach();
	CHECK(!FBuffer.empty());
	{
		_buffer_safety_operation_t _block(FBuffer.FAllocator,
				FBuffer.FBeginOfStorage);
		buf_info& _info = FBuffer.MGetBuff();
		_info.MSetSize(_info.MSize() - 1);
	}
}
void CBuffer::push_back(const value_type& __x)
{
	insert(end(), 1, NSHARE::smart_field_t<value_type>(__x));
}
void CBuffer::pop_front()
{
	if (empty())
		return;
	MDetach();
	CHECK(!FBuffer.empty());
	{
		_buffer_safety_operation_t _block(FBuffer.FAllocator,
				FBuffer.FBeginOfStorage);
		buf_info& _info = FBuffer.MGetBuff();
		_info.MSetStartOffset(_info.MStartOffset() + 1);
	}
}
void CBuffer::push_front(const value_type& __x)
{
	insert(begin(), 1, NSHARE::smart_field_t<value_type>(__x));
}
CBuffer::size_type CBuffer::requred_buf_len(size_type aSize) const
{
	CHECK_NOTNULL(FBuffer.FAllocator);
	const size_t _max_size = FBuffer.FAllocator->MMaxSize();
	if (aSize > _max_size)
	{
		LOG(ERROR)<<"The required buffer size "<<aSize<<" is more than max size "<<_max_size;
		return 0;
	}

	const size_type _req_len = aSize;
	size_type _size = aSize;
	if (aSize <= (DEF_BUF_RESERVE * 0.75)) //128
		_size = DEF_BUF_RESERVE;
	else if (aSize <= (0x800)) //1024
		_size = static_cast<size_type>((double) _req_len * BUF_FACTOR);
	else if (aSize <= (0x1000)) //1024-4096
	{
		_size = _req_len + 512;
	}
	else if (aSize <= 0x3000) //12288
		_size = _req_len + 1024;
	else if (aSize <= 0xFFFF) //
	{
		_size = _req_len + 512;
	}
	else
	{
		_size = _req_len + DEF_BUF_RESERVE;
	}
	const size_type _len = _size + BEGIN_SIZE;
	return (_len < size() || _len > _max_size) ? _max_size : _len;
}
CBuffer::value_type* CBuffer::MReAllocate(value_type* aP, size_t aSize)
{
	size_type _len = requred_buf_len(aSize);
	if (!_len)
	{
		return NULL;
	}
	value_type* _p = FBuffer.MReAllocate(aP, _len);
	if (!_p)
	{
		return NULL;
	}
	return _p;
}
CBuffer::value_type* CBuffer::MAllocate(size_t aSize)
{
	size_type _len = requred_buf_len(aSize);
	if (!_len)
	{
		return NULL;
	}
	value_type* _p = FBuffer.MAllocate(_len);
	if (!_p)
	{
		return NULL;
	}

	{
		//_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);/it's the new buffer
		buf_info& _info = _buffer_t::sMGetBuff(_p);
		_info.MSetStartOffset(BEGIN_SIZE);
	}
	return _p;
}

CBuffer::allocator_type & CBuffer::get_allocator() const
{
	CHECK_NOTNULL(FBuffer.FAllocator);
	return *FBuffer.FAllocator;
}
bool CBuffer::MIsNeedDetach() const
{
	return !(FBuffer.empty() || FBuffer.unique());
}
CBuffer CBuffer::MCreateCopy(eAllocatorType aType)
{
	return MCreateCopy(FBuffer.FAllocator,aType);
}
CBuffer CBuffer::MCreateCopy(IAllocater* aAlloc,eAllocatorType aType)
{
	CBuffer _rval(aAlloc, aType);
	_rval.deep_copy(*this);
	return _rval;
}
void CBuffer::MDetach()
{
	if (MIsNeedDetach())
	{
		VLOG(2) << "Detaching...";
		_buffer_t _tmp_buf(FBuffer.FAllocator, FBuffer.FAllocatorType);
		_tmp_buf.deep_copy(FBuffer);
		FBuffer = _tmp_buf;
		FIsDetached = true;
	}
}
void CBuffer::release()
{
	if (!FBuffer.empty())
		FBuffer.release();
}
CBuffer::offset_pointer_t CBuffer::refuse()
{
	offset_pointer_t _offset = offset();
	if (_offset != IAllocater::NULL_OFFSET)
		FBuffer.FBeginOfStorage = NULL;
	return _offset;
}
unsigned CBuffer::use_count() const
{
	if (FBuffer.empty())
		return 0;
	buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return _info.use_count();
}
CBuffer::offset_pointer_t CBuffer::offset() const
{
	if (!FBuffer.FAllocator || !FBuffer.FBeginOfStorage)
		return IAllocater::NULL_OFFSET;
	VLOG(2) << "Begin of storage " << FBuffer.MStorageBegin();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return FBuffer.FAllocator->MOffset(FBuffer.MStorageBegin());
}
CBuffer::const_pointer CBuffer::ptr_const() const
{
	if (FBuffer.empty())
		return NULL;
	return begin().base();
}
CBuffer::pointer CBuffer::ptr()
{
	if (FBuffer.empty())
		return NULL;
	MDetach();
	return begin().base();
}
CBuffer::size_type CBuffer::begin_capacity() const
{
	if (FBuffer.empty())
		return 0;
	buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
	_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
#endif
	return _info.MStartOffset();
}
void CBuffer::reserve(size_t aSize)
{
	if (aSize <= capacity())
		return;
	if (FBuffer.FBeginOfStorage != NULL)
	{
		buf_info& _info = FBuffer.MGetBuff();
#ifdef SAFETY_BUFFER_READ_OPERATION
		uint32_t  _use_count;
		{
			_buffer_safety_operation_t _block(FBuffer.FAllocator, FBuffer.FBeginOfStorage);
			_use_count =_info.use_count();
		}
#else
		uint32_t const _use_count = _info.use_count();
#endif

		LOG_IF(WARNING,_use_count>1) << "The buffer is held by " << _use_count
												<< " holders."
														" The operation 'reserve' can be applied to only one holder. ";
		if (_use_count > 1)
		{
			VLOG(1) << "With detach";
			value_type* _buf_p = MAllocate(aSize);

			if (_buf_p != NULL)
			{
				buf_info& _new_info = _buffer_t::sMGetBuff(_buf_p);
				_new_info.MSetSize(_info.MSize());

				memcpy(_buf_p + _new_info.MStartOffset(),
						FBuffer.FBeginOfStorage + _info.MStartOffset(),
						_info.MSize());
			}

			FBuffer.release();
			FBuffer.FBeginOfStorage = _buf_p;
		}
		else
		{
			VLOG(4) << "With out detach";
			value_type* _buf_p = MReAllocate(FBuffer.FBeginOfStorage, aSize); //todo reallocate
			if (_buf_p == NULL)
			{
				LOG(DFATAL)<<"Cannot reallocate buffer";
				//FBuffer.release();
			}
			FBuffer.FBeginOfStorage = _buf_p;
		}
	}
	else
	{
		value_type* _buf_p = MAllocate(aSize);
		buf_info& _new_info = _buffer_t::sMGetBuff(_buf_p);
		_new_info.MSetSize(0);
		FBuffer.FBeginOfStorage = _buf_p;
	}
}
CBuffer::iterator CBuffer::erase(iterator aBegin, iterator aEnd)
{
	if (!empty() && aBegin < aEnd)
	{
		buf_info& _info = FBuffer.MGetBuff();
		_buffer_safety_operation_t _block(FBuffer.FAllocator,
				FBuffer.FBeginOfStorage);
		const size_t _remove=aEnd - aBegin;
		if (aEnd == end())
			_info.MSetSize(_info.MSize() - _remove);
		else if (aBegin==begin())
		{
			_info.MSetStartOffset(_info.MStartOffset() + _remove);
		}
		else
		{
			LOG_IF(FATAL,aEnd<begin()) << " Segmentation failed";
			LOG_IF(FATAL,aEnd>end()) << " Segmentation failed";
			memmove(aBegin.base(), aEnd.base(), end() - aEnd);
			_info.MSetSize(_info.MSize() - _remove);
		}
	}
	return aBegin;
}
bool CBuffer::sMUnitTest(IAllocater* aAlloc)
{
	bool _is=true;
	for (size_t _size = 0x1 << 2; _size <= 65536; _size = _size << 1)
		_is=_is&&sMUnitTest(_size, aAlloc);
	return _is;
}
bool CBuffer::sMUnitTest(size_t aSize, IAllocater* aAlloc)
{
	//std::cout<<aSize<<std::endl;
	if(!aAlloc)
		aAlloc=CBuffer::sMDefAllaocter();
	size_t const _size = aSize;
	size_t const _check_size = _size;

	size_t const _max = std::numeric_limits<CBuffer::value_type>::max();
	CBuffer _buffer(aAlloc);
	{
		CHECK(_buffer.ptr()== NULL);
		_buffer.resize(_size);
		CHECK_EQ(_buffer.size(), _size);
	}
	{
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; i <= _check_size; ++_it, ++i)
		{
			*_it = i % _max;
			CHECK_LE(i % _max, _size);
		}
		//std::cout << "EOK" << std::endl;
	}
	{
		const size_t _new_size = _size / 2;
		_buffer.resize(_new_size);
		CHECK_EQ(_buffer.size(), _new_size);
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{

			CHECK_EQ(i % _max, *_it);
		}
		//std::cout << "EOK2" << std::endl;
	}
	{
		const size_t _new_size = _size;
		_buffer.resize(_new_size);
		CHECK_EQ(_buffer.size(), _new_size);
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{

			CHECK_EQ(i % _max, *_it);
		}
		//std::cout << "EOK2.1" << std::endl;
	}
	const bool _is_begin_capacity = _buffer.begin_capacity() != 0;
	{
		CBuffer::const_pointer _p = _buffer.ptr_const();
		const size_t _new_size = _buffer.capacity() - _buffer.begin_capacity();
		_buffer.resize(_new_size);
		CHECK_EQ(_buffer.size(), _new_size);
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; i <= _check_size; ++_it, ++i)
		{

			CHECK_EQ(i % _max, *_it);
		}
		CHECK_EQ((_check_size - 1) % _max, _buffer[_check_size - 1] % _max);
		CHECK_EQ(_p, _buffer.ptr_const());
		CHECK_EQ(_is_begin_capacity, _buffer.begin_capacity() != 0);
		//std::cout << "EOK3" << std::endl;

	}
	{
		const size_t _new_size = _buffer.capacity()
				- _buffer.begin_capacity() / 2;
		const size_t _begin_capacity = _buffer.begin_capacity();
		CBuffer::const_pointer _p = _buffer.ptr_const();
		_buffer.resize(_new_size);
		CHECK_EQ(_buffer.size(), _new_size);
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; i <= _check_size; ++_it, ++i)
		{

			CHECK_EQ(i % _max, *_it);
		}
		CHECK_EQ(_p, _buffer.ptr_const() + _begin_capacity);
		CHECK_GT(_buffer.capacity(), _buffer.size());
		CHECK_EQ(_buffer.begin_capacity(), 0);
		//std::cout << "EOK4" << std::endl;

	}

	{
		const size_t _new_size = _buffer.capacity();
		CBuffer::const_pointer _p = _buffer.ptr_const();

		_buffer.resize(_new_size);
		CHECK_EQ(_buffer.size(), _new_size);
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; i <= _check_size; ++_it, ++i)
		{

			CHECK_EQ(i % _max, *_it);
		}

		CHECK_EQ(_buffer.capacity(), _new_size);
		CHECK_EQ(_buffer.begin_capacity(), 0);
		CHECK_EQ(_p, _buffer.ptr_const());

		//std::cout << "EOK5" << std::endl;
	}

	{
		const size_t _new_size = _buffer.capacity() + 1;
		_buffer.resize(_new_size);
		CHECK_EQ(_buffer.size(), _new_size);
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; i <= _check_size; ++_it, ++i)
		{

			CHECK_EQ(i % _max, *_it);
		}

		CHECK_GT(_buffer.capacity(), _new_size);
		//std::cout << "EOK6" << std::endl;
	}
	//insert test
	{
		_buffer.resize(_check_size);
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
	}
	{
		const size_t _mid = _check_size / 3;
		CBuffer _buf(aAlloc);
		_buf.resize(_mid);
		CBuffer::iterator _it = _buf.begin();
		for (size_t i = _mid; _it != _buf.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_it = _buffer.begin() + _mid;
		for (size_t i = 2 * _mid; _it != _buffer.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_buffer.insert(_buffer.begin() + _mid, _buf.begin(), _buf.end());

		_it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			CHECK_EQ(i % _max, *_it);
		}

		//std::cout << "EOK7" << std::endl;
	}
	if (_check_size > 10)
	{
		const size_t _mid = _check_size / 3;
		CBuffer _buf(aAlloc);
		_buf.resize(_mid);
		CBuffer::iterator _it = _buf.begin();
		for (size_t i = _mid; _it != _buf.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_it = _buffer.begin() + _mid;
		for (size_t i = 2 * _mid; _it != _buffer.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_buffer.resize(2 * _mid - 2); //remove last
		_buffer.insert(_buffer.begin() + _mid, _buf.begin(), _buf.end());

		_it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			CHECK_EQ(i % _max, *_it);
		}

		//std::cout << "EOK8" << std::endl;
	}
	{
		CBuffer _buf(aAlloc);
		_buf.resize(_check_size);
		CBuffer::iterator _it = _buf.begin();
		for (size_t i = _check_size; _it != _buf.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_buffer.resize(_check_size);
		_it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_buffer.insert(_buffer.end(), _buf.begin(), _buf.end());
		CHECK_EQ(_buffer.size(), _check_size + _buf.size());
		_it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			CHECK_EQ(i % _max, *_it);
		}
		//std::cout << "EOK9" << std::endl;
	}
	{
		CBuffer _buf(aAlloc);
		_buf.resize(_check_size);
		CHECK_GT(_buf.capacity(), 0);

		size_t const _insert_size = std::min(_buf.capacity(), _check_size);

		_buf.resize(_insert_size / 2);
		CBuffer::iterator _it = _buf.begin();
		for (size_t i = _insert_size / 2; _it != _buf.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		CBuffer _buf2(aAlloc);
		_buf2.resize(_insert_size / 2);

		_it = _buf2.begin();
		for (size_t i = 0; _it != _buf2.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}

		_buf.insert(_buf.begin(), _buf2.begin(), _buf2.end());
		CHECK_EQ(_buf.size(), _insert_size);
		_it = _buf.begin();
		for (size_t i = 0; _it != _buf.end(); ++_it, ++i)
		{
			CHECK_EQ(i % _max, *_it);
		}
		//std::cout << "EOK10" << std::endl;
	}
	{
		CBuffer _buf(aAlloc);
		_buf.resize(_check_size);
		CHECK_GT(_buf.capacity(), 0);

		size_t const _capacity = _buf.begin_capacity();
		size_t const _insert_size = _buf.begin_capacity() + _check_size;

		CBuffer _buf2(aAlloc);
		_buf2.resize(_buf.begin_capacity());
		CBuffer::iterator _it = _buf2.begin();
		for (size_t i = 0; _it != _buf2.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}

		_it = _buf.begin();
		for (size_t i = _buf.begin_capacity(); _it != _buf.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_buf.insert(_buf.begin(), _buf2.begin(), _buf2.end());

		CHECK_EQ(_buf.size(), _insert_size);
		_it = _buf.begin();
		for (size_t i = 0; _it != _buf.end(); ++_it, ++i)
		{
			CHECK_EQ(i % _max, *_it);
		}
		VLOG(2)<<"Begin capacity "<<_buf.begin_capacity();
		CHECK_EQ(_buf.begin_capacity(), 0);
		size_t const _full_capacity=_buf.capacity();
		_buf.insert(_buf.begin(), 0);
		size_t const _n_size=_buf.size();
		size_t const _begin=_buf.begin_capacity();
		CHECK_EQ(_begin,std::min(_full_capacity-_n_size,_capacity));

		//std::cout << "EOK11" << std::endl;
	}
	{
		CBuffer _buf(aAlloc);
		_buf.resize(_check_size);

		const size_t _new_size = _buf.capacity() - _buf.begin_capacity();
		const size_t _begin_capacity = _buf.begin_capacity();
		_buf.resize(_new_size);

		CBuffer::const_pointer _p = (_buf.end()).base();
		CBuffer::iterator _it = _buf.begin();
		for (size_t i = 0; _it != _buf.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		CBuffer _buf2(aAlloc);
		_buf2.resize(_begin_capacity);
		_it = _buf2.begin();
		for (size_t i = _new_size; _it != _buf2.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_it = _buf.begin();
		for (size_t i = 0; _it != _buf.end(); ++_it, ++i)
		{
			CHECK_EQ(i % _max, *_it);
		}
		_buf.insert(_buf.end(), _buf2.begin(), _buf2.end());

		CHECK_EQ(_buf.size(), _buf.capacity());

		CHECK_EQ(_p, _buf.ptr_const() + _buf.capacity());
		//std::cout << "EOK12" << std::endl;
	}
	{
		CBuffer _buf(aAlloc);
		_buf.resize(_check_size);

		const size_t _new_size = _buf.capacity();
		const size_t _third = (size_t) (_new_size / 3.0);
		_buf.resize(_new_size);
		CHECK_EQ(_buf.size(), _buf.capacity());
		CBuffer::iterator _it = _buf.begin();
		for (size_t i = 0; i < _third; ++_it, ++i)
		{
			*_it = i % _max;
		}
		for (size_t i = 2 * _third; _it != _buf.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		CBuffer _buf2(aAlloc);
		_buf2.resize(_third);
		_it = _buf2.begin();
		for (size_t i = _third; _it != _buf2.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		_buf.insert(_buf.begin() + _third, _buf2.begin(), _buf2.end());

		_it = _buf.begin();
		for (size_t i = 0; _it != _buf.end(); ++_it, ++i)
		{
			CHECK_EQ(i % _max, *_it);
		}
		//std::cout << "EOK13" << std::endl;
	}
	{
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		CBuffer _buffer2(aAlloc);
		_buffer2 = _buffer;
		CHECK_EQ(_buffer2.ptr_const(), _buffer.ptr_const());
		CHECK_EQ(_buffer2.size(), _buffer.size());

		CBuffer::const_iterator _it2 = _buffer2.cbegin();
		for (size_t i = 0; _it2 != _buffer2.cend(); ++_it2, ++i)
		{
			CHECK_EQ(i % _max, *_it2);
			CHECK_EQ(_buffer[i], *_it2);
		}
		CHECK_EQ(_buffer2.ptr_const(), _buffer.ptr_const());

		CBuffer::const_pointer _p = _buffer2.ptr_const();
		_it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		CHECK_EQ(_buffer2.size(), _buffer.size());
		CHECK_NE(_buffer2.ptr_const(), _buffer.ptr_const());
		_it = _buffer2.begin();
		CHECK_EQ(_p, _buffer2.ptr_const());
		for (size_t i = 0; _it != _buffer2.end(); ++_it, ++i)
		{
			CHECK_EQ(_buffer[i], *_it);
		}
		//std::cout << "EOK14" << std::endl;
	}
	{
		CHECK(!_buffer.empty());
		CBuffer::iterator _it = _buffer.begin();
		for (size_t i = 0; _it != _buffer.end(); ++_it, ++i)
		{
			*_it = i % _max;
		}
		CBuffer::offset_pointer_t _offset = _buffer.refuse();

		CHECK(_buffer.empty());

		CBuffer _buffer2(*aAlloc, _offset);
		_it = _buffer2.begin();
		for (size_t i = 0; _it != _buffer2.end(); ++_it, ++i)
		{
			CHECK_EQ(*_it, i % _max);
		}

//		_buffer=_buffer2;
//		_buffer2.release();
	}
	return true;
}
CBuffer::allocator_type * CBuffer::sMDefAllaocter()
{
	return get_default_allocator();
}
bool CBuffer::MIsAllocatorEqual(CBuffer const& aRht) const
{
	return MIsAllocatorEqual(aRht.MAllocator());
}
bool CBuffer::MIsAllocatorEqual(IAllocater* aAlloc) const
{
	if (!MAllocator() || !aAlloc)
		return true;
	return FBuffer.FAllocator == aAlloc;
}
IAllocater* CBuffer::MAllocator() const
{
	return FBuffer.FAllocator;
}
bool CBuffer::MIsRestored() const
{
	return FBuffer.MIsRestored();
}
void CBuffer::swap(CBuffer& aBuf)
{
	FBuffer.swap(aBuf.FBuffer);
	std::swap(FIsDetached,aBuf.FIsDetached);
}
}
/* namespace NSHARE */
