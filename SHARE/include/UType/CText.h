/*
 * CText.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CTEXT_H_
#define CTEXT_H_

#include <cstring>
#include <stdexcept>
#include <cstddef>
#include <UType/CCodeConv.h>
#if defined(_MSC_VER)
#	pragma warning (push)
#	pragma warning (disable:4251)
#endif
namespace NSHARE
{
class CBuffer;
typedef char utf8;
typedef uint32_t utf32;
typedef utf8 encoded_char;

//don't use memcpy!!!!!!!!!!!!!!!!!!!!!!!
class SHARE_EXPORT CText
{
public:
	/*************************************************************************
	 Integral Types
	 *************************************************************************/
	typedef utf32 value_type; //!< Basic 'code point' type used for CText (utf32)
	typedef size_t size_type; //!< Unsigned type used for size values and indices
	typedef std::ptrdiff_t difference_type;	//!< Signed type used for differences
	typedef utf32& reference;	//!< Type used for utf32 code point references
	typedef const utf32& const_reference;//!< Type used for constant utf32 code point references
	typedef utf32* pointer;			//!< Type used for utf32 code point pointers
	typedef const utf32* const_pointer;	//!< Type used for constant utf32 code point pointers
	typedef IAllocater allocator_type;

	class SHARE_EXPORT iterator;
	class SHARE_EXPORT const_iterator;

	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	static const size_type npos;//!< Value used to represent 'not found' conditions and 'all code points' etc.
//	static const size_type QUICKBUFF_SIZE = 32;
	static const size_type S_MAX_SIZE;

	explicit CText(allocator_type*aAllocator=get_default_allocator());

	CText(const CText& str);
	CText& operator=(const CText& str);

	CText(const CText& str, size_type str_idx, size_type str_num = npos);

	CText(const std::string& std_str, allocator_type*aAllocator =
			get_default_allocator());//todo utf 8
	CText& operator=(const std::string& std_str);

	CText(const std::string& std_str, size_type str_idx, size_type str_num =
			npos, allocator_type*aAllocator = get_default_allocator());	//todo utf 8

	CText(size_type num, utf32 code_point,allocator_type*aAllocator=get_default_allocator());
	CText& operator=(utf32 code_point);

	CText(const_iterator const& iter_beg, const_iterator const& iter_end,allocator_type*aAllocator=get_default_allocator());

	CText(utf8 const* cstr, ICodeConv const& aType = CCodeUTF8(),allocator_type*aAllocator=get_default_allocator());
	CText& operator=(utf8 const* cstr);

	CText(utf8 const* chars, size_type chars_len, ICodeConv const& aType =
			CCodeUTF8(),allocator_type*aAllocator=get_default_allocator());

	~CText();
	static CText const& sMEmpty();

	CText& operator+=(const CText& str);
	CText& operator+=(const std::string& std_str);
	CText& operator+=(utf32 code_point);
	CText& operator+=(utf8 const* cstr);

	size_type const& size() const;
	size_type const& length() const;
	size_type  length_code(ICodeConv const& aType =
			CCodeUTF8()) const;
	bool empty(void) const;
	size_type max_size() const;

	size_type capacity() const;

	void reserve(size_type num = 0);

	int compare(const CText& str) const;
	int compare(size_type idx, size_type len, const CText& str,
			size_type str_idx = 0, size_type str_len = npos) const;

	int compare(const std::string& std_str) const;
	int compare(size_type idx, size_type len, const std::string& std_str,
			size_type str_idx = 0, size_type str_len = npos,ICodeConv const& aType = CCodeUTF8()) const;

	int compare(utf8 const* cstr, ICodeConv const& aType = CCodeUTF8()) const;
	int compare(size_type idx, size_type len, utf8 const* chars,
			size_type chars_len, ICodeConv const& aType = CCodeUTF8()) const;
	int compare(size_type idx, size_type len, utf8 const* chars,
			ICodeConv const& aType = CCodeUTF8()) const;

	reference operator[](size_type idx);

	const_reference operator[](size_type idx) const;

	reference at(size_type idx);

	const_reference at(size_type idx) const;

	utf8 const* c_str() const;

	utf8 const* data(void) const;
	utf8* data_unsafety(void);


	const utf32* ptr(void) const;
	const utf32* ptr_const(void) const;

	size_type copy(char* buf, size_type len = npos, size_type idx = 0,
			ICodeConv const& aType = CCodeUTF8()) const;

	void swap(CText& str);
	void clear(void);
	void push_back(utf32 code_point);

	CText& assign(const CText& str, size_type str_idx = 0, size_type str_num =
			npos);
	CText& assign(const std::string& std_str, size_type str_idx = 0,
			size_type str_num = npos,ICodeConv const& aType = CCodeUTF8());
	CText& assign(size_type num, utf32 code_point);
	CText& assign(utf8 const* cstr, ICodeConv const& aType = CCodeUTF8());
	CText& assign(utf8 const* chars, size_type chars_len,
			ICodeConv const& aType = CCodeUTF8());

	CText& append(const CText& str, size_type str_idx = 0, size_type str_num =
			npos);
	CText& append(const std::string& std_str, size_type str_idx = 0,
			size_type str_num = npos,ICodeConv const& aType = CCodeUTF8());
	CText& append(utf8 const* utf8_str, ICodeConv const& aType = CCodeUTF8());
	CText& append(utf8 const* utf8_str, size_type len, ICodeConv const& aType =
			CCodeUTF8());
	CText& append(size_type num, utf32 code_point);
	CText& append(const_iterator iter_beg, const_iterator iter_end);

	CText& insert(size_type idx, const CText& str);
	CText& insert(size_type idx, const CText& str, size_type str_idx,
			size_type str_num);
	CText& insert(size_type idx, const std::string& std_str);
	CText& insert(size_type idx, const std::string& std_str, size_type str_idx,
			size_type str_num,ICodeConv const& aType = CCodeUTF8());
	CText& insert(size_type idx, size_type num, utf32 code_point);
	void insert(iterator pos, size_type num, utf32 code_point);
	iterator insert(iterator pos, utf32 code_point);
	void insert(iterator iter_pos, const_iterator iter_beg,
			const_iterator iter_end);

	CText& insert(size_type idx, utf8 const* utf8_str, ICodeConv const& aType =
			CCodeUTF8());
	CText& insert(size_type idx, utf8 const* utf8_str, size_type len,
			ICodeConv const& aType = CCodeUTF8());

	CText& erase();
	CText& erase(size_type idx);
	CText& erase(size_type idx, size_type len);
	CText& erase(iterator pos);
	CText& erase(iterator iter_beg, iterator iter_end);

	void resize(size_type num);
	void resize(size_type num, utf32 code_point);

	CText& replace(size_type idx, size_type len, const CText& str);
	CText& replace(iterator iter_beg, iterator iter_end, const CText& str);
	CText& replace(size_type idx, size_type len, const CText& str,
			size_type str_idx, size_type str_num);
	CText& replace(size_type idx, size_type len, const std::string& std_str);
	CText& replace(iterator iter_beg, iterator iter_end,
			const std::string& std_str);
	CText& replace(size_type idx, size_type len, const std::string& std_str,
			size_type str_idx, size_type str_num);
	CText& replace(size_type idx, size_type len, size_type num,
			utf32 code_point);
	CText& replace(iterator iter_beg, iterator iter_end, size_type num,
			utf32 code_point);
	CText& replace(iterator iter_beg, iterator iter_end,
			const_iterator iter_newBeg, const_iterator iter_newEnd);

	CText& replace(size_type idx, size_type len, utf8 const* cstr,
			ICodeConv const& aType = CCodeUTF8());
	CText& replace(iterator iter_beg, iterator iter_end, utf8 const* cstr,
			ICodeConv const& aType = CCodeUTF8());
	CText& replace(size_type idx, size_type len, utf8 const* chars,
			size_type chars_len, ICodeConv const& aType = CCodeUTF8());
	CText& replace(iterator iter_beg, iterator iter_end, utf8 const* chars,
			size_type chars_len, ICodeConv const& aType = CCodeUTF8());

	size_type find(utf32 code_point, size_type idx = 0) const;
	size_type rfind(utf32 code_point, size_type idx = npos) const;

	size_type find(const CText& str, size_type idx = 0) const;
	size_type rfind(const CText& str, size_type idx = npos) const;

	size_type find(const std::string& std_str, size_type idx = 0) const;
	size_type rfind(const std::string& std_str, size_type idx = npos) const;

	size_type find(utf8 const* utf8_str, size_type idx, size_type str_len,
			ICodeConv const& aType = CCodeUTF8()) const;
	size_type find(utf8 const* cstr, size_type idx = 0, ICodeConv const& aType =
			CCodeUTF8()) const;
	size_type rfind(utf8 const* utf8_str, size_type idx, size_type str_len,
			ICodeConv const& aType = CCodeUTF8()) const;

	size_type rfind(utf8 const* cstr, size_type idx = npos,
			ICodeConv const& aType = CCodeUTF8()) const;

	size_type find_first_of(const CText& str, size_type idx = 0) const;
	size_type find_first_not_of(const CText& str, size_type idx = 0) const;

	size_type find_first_of(const std::string& std_str,
			size_type idx = 0) const;
	size_type find_first_not_of(const std::string& std_str,
			size_type idx = 0) const;

	size_type find_first_of(utf8 const* cstr, size_type idx = 0,
			ICodeConv const& aType = CCodeUTF8()) const;
	size_type find_first_not_of(utf8 const* cstr, size_type idx = 0,
			ICodeConv const& aType = CCodeUTF8()) const;

	size_type find_first_of(utf8 const* chars, size_type idx,
			size_type chars_len, ICodeConv const& aType = CCodeUTF8()) const;
	size_type find_first_not_of(utf8 const* chars, size_type idx,
			size_type chars_len, ICodeConv const& aType = CCodeUTF8()) const;

	size_type find_first_of(utf32 code_point, size_type idx = 0) const;
	size_type find_first_not_of(utf32 code_point, size_type idx = 0) const;

	size_type find_last_of(const CText& str, size_type idx = npos) const;
	size_type find_last_not_of(const CText& str, size_type idx = npos) const;
	size_type find_last_of(const std::string& std_str,
			size_type idx = npos) const;
	size_type find_last_not_of(const std::string& std_str,
			size_type idx = npos) const;

	size_type find_last_of(utf8 const* utf8_str, size_type idx = npos,
			ICodeConv const& aType = CCodeUTF8()) const;
	size_type find_last_not_of(utf8 const* utf8_str, size_type idx = npos,
			ICodeConv const& aType = CCodeUTF8()) const;
	size_type find_last_of(utf8 const* utf8_str, size_type idx,
			size_type str_len, ICodeConv const& aType = CCodeUTF8()) const;
	size_type find_last_not_of(utf8 const* utf8_str, size_type idx,
			size_type str_len, ICodeConv const& aType = CCodeUTF8()) const;

	size_type find_last_of(utf32 code_point, size_type idx = npos) const;
	size_type find_last_not_of(utf32 code_point, size_type idx = npos) const;

	CText substr(size_type idx = 0, size_type len = npos) const;

	iterator begin(void);
	const_iterator begin(void) const;
	iterator end(void);
	const_iterator end(void) const;
	reverse_iterator rbegin(void);
	const_reverse_iterator rbegin(void) const;
	reverse_iterator rend(void);
	const_reverse_iterator rend(void) const;

	//------------additional API--------

	//!@brief Printing Text state, it isn't equal  the c_str() method
	NSHARE::CBuffer& MToBuf(NSHARE::CBuffer&s) const;
	std::ostream& MPrint(std::ostream&) const;
	std::ostream& MPrint(std::ostream&, ICodeConv const& aType) const;

	//!@brief trimming/padding whitespace (either side) and truncating
	//!@return *this
	CText& MTrimSpace();

	//!@brief printf for coding; In additional standard specificator there are  b - boolean;
	//! cl - uintf32 char code , c - is traditional char; sl - uint32 string, s -traditional
	static CText sMPrintf(ICodeConv const& aType, const char* cformat, ...);
	CText& MPrintf(const char* cformat, ...);	//UTF8

	//!@brief  returns true if the string matches the pattern in 'aStr'.
	//! The pattern consists of ordinary characters that must match exactly,
	//! ? that match one character and * that match zero of more characters.
	//! \ may be used to quite characters.
	bool MIsMatch(CText const& aStr) const;
	std::string MToStdString(ICodeConv const& aType = CCodeUTF8()) const;
	//ToWString
	//Capitalize

	CText& MToLowerCase();
	CText& MToUpperCase();
	bool MIsAscii() const;
	//IsFundamental
	//IsWorld
	unsigned int MGetHash() const;

	int MCompareNoCase(const CText& str) const;
	int MCompareNoCase(size_type idx, size_type len, const CText& str,
			size_type str_idx = 0, size_type str_len = npos) const;

	CText& MLeftJustified(int width, utf32 fill = ' ', bool truncate = false);
	CText& MRightJustified(int width, utf32 fill = ' ', bool truncate = false);

	//!@brief Replace All aSubStr to aReplaceTo from index idx
	CText& MReplaceAll(CText const& aSubStr,CText const& aReplaceTo,size_type idx=0, size_type len=npos);
	
	//!@brief Make random string
	CText& MMakeRandom(size_t aLen,ICodeConv const& aType = CCodeANSII());

//	bool iscntrl();
//	bool isprint();
//	bool isspace();
//	bool isblank();
//	bool isgraph();
//	bool ispunct();
//	bool ispunct();
	bool isalnum()const;
	bool isalpha()const;
//	bool isupper();
//	bool islower();
	bool isdigit() const;
//	bool isxdigit ();

	static bool sMUnitTest();
private:
	struct SHARE_EXPORT impl_t
	{
		impl_t(allocator_type*);
		impl_t(const impl_t&);
		~impl_t();
		/** \note Single buffer is used when:
		 * called c_str() or data (for converting MultiByte to Single Byte)
		 */

		mutable utf8* FSingleByteBuffer;	//!< holds string data encoded as utf8 (generated only by calls to c_str() and data())
		mutable size_type FSingleByteDatalen;//!< holds length of encoded data (in case it's smaller than buffer).
		mutable size_type FSingleByteBufferLen;//!< length of above buffer (since buffer can be bigger then the data it holds to save re-allocations).

		/** \note MultiByte buffer is used when:
		 *  - FCodePointLength >= countof(FQuickUCS4)
		 */
		size_type FMultiByteBufferLen;	//!< code point reserve size (currently allocated buffer size in code points).
		utf32* FMultiByteBuf; //!< Pointer the the main buffer memory.  This is only valid when quick-buffer is not being used

		allocator_type* FAllocator;//!<Allocator
	private:
		impl_t& operator=(const impl_t&){
			return *this;
		}
	};
	typedef CCOWPtr<impl_t> cow_impl_t;

	// string management
	// change size of allocated buffer so it is at least 'new_size'.
	// May or may not cause re-allocation and copy of buffer if size is larger
	// will never re-allocate to make size smaller.  (see trim())
	bool MGrow(size_type new_size);

	void MRemoveBuffer(void);

	// set the length of the string, and terminate it,
	//according to the given value (will not re-allocate, use grow() first).
	void MSetLen(size_type len);

	void MInit();

	bool MInside(utf32* inptr);

	// compute distance between two iterators, returning a 'safe' value
	size_type MSafeIterDif(const const_iterator& iter1,
			const const_iterator& iter2) const;

	// encoding functions
	// for all:
	//	src_len is in code units, or 0 for null terminated string.
	//	dest_len is in code units.
	//	returns number of code units put into dest buffer.
	/*inline*/ size_type MFromUCS4(const utf32* src, char* dest, size_type dest_len,
			size_type src_len, ICodeConv const& aType) const;

	/*inline*/ size_type MToUCS4(utf8 const* src, utf32* dest, size_type dest_len,
			ICodeConv const& aType, size_type src_len = 0) const;

	// return number of code units in a null terminated string
	/*inline*/ size_type MUCS4Length(const utf32* utf32_str) const;

	// build an internal buffer with the string encoded as utf8 (remains valid until string is modified).
	utf8* MBuildUtf8Buff(ICodeConv const& aType = CCodeUTF8()) const;



	/*inline*/ size_type MFindCodepoint(utf8 const* chars, size_type chars_len,
			utf32 code_point, ICodeConv const& aType) const;

	static bool sMIsMatch(const_iterator _pat_it, const_iterator _pat_end,
			const_iterator _it, const_iterator _it_end);


	void MWillBeenChanged();

	/** \brief return buffer length
	 *
	 */
	size_type MGetBufferLength() const;

	/** \brief pointer to the buffer
	 *
	 */
	utf32* ptr(void);

	cow_impl_t FImpl;//!< Copy on write data
	size_type FCodePointLength;//!< holds length of string in code points (not including null termination)
};
typedef std::deque<CText> Strings;

/*!
 \brief
 The Functor is faster than using the default, but the map will no longer be sorted alphabetically.
 */
struct CStringFastLessCompare
{
	bool operator()(const CText& a, const CText& b) const
	{
		const size_t la = a.length();
		const size_t lb = b.length();
		if (la == lb)
			return (memcmp(a.ptr(), b.ptr(), la * sizeof(utf32)) < 0);

		return (la < lb);
	}
};
} //namespace NSHARE
#include <UType/CTextIterator.h>
#include <UType/CTextImpl.h>

#if defined(_MSC_VER)
#	pragma warning (pop)
#endif
#endif	// #ifndef CTEXT_H_
