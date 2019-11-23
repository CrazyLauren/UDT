// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CText.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef NOLOG
#	define NOLOG
#endif
#include <deftype>
#include <stdarg.h>
#include <locale>
#include <iomanip>

#include <SHARE/random_value.h>
// Start of NSHARE namespace section
namespace NSHARE
{
//TODO to wstring
//todo match
//todo cmp no case


// definition of 'no position' value
const CText::size_type CText::npos = static_cast<CText::size_type>(-1);
const CText::size_type CText::S_MAX_SIZE = static_cast<size_type>(-1)
		/ sizeof(utf32);

std::locale utf8_locale()
{
	static std::locale loc;
	static bool _is = false;

	if (!_is)
	{
		_is = true;
		char _strs[][128] =
		{ "ru_RU.UTF-8", "C.UTF8", "en_US.UTF8" };
		for (unsigned i = 0; i < sizeof(_strs) / sizeof(_strs[0]); ++i)
			try
			{
				loc = std::locale(_strs[i]);
				break;
			} catch (std::runtime_error&)
			{
				continue;
			}
	}
	return loc;
}
CText::CText(allocator_type*aAllocator):
		FImpl(impl_t(aAllocator),aAllocator),//
        FStream(new CTextStream(*this))

{
	MInit();
}
CText::CText(const CText& str):
		FImpl(str.FImpl),//
		FCodePointLength(str.FCodePointLength),//
        FStream(new CTextStream(*this))
{
	//assign(str);
}
CText& CText::operator=(const CText& str)
{
	if (&str == this)
	{
		LOG(WARNING)<<"Self assignment "<<str;
		return *this;
	}
	FImpl=str.FImpl;
	FCodePointLength=str.FCodePointLength;
	return *this;
}
CText::CText(const CText& str, size_type str_idx, size_type str_num):
				FImpl(str.FImpl),//
				FCodePointLength(str.FCodePointLength),//
                FStream(new CTextStream(*this))
{
	assign(str, str_idx, str_num);
}
CText::CText(const std::string& std_str,allocator_type*aAllocator):
				FImpl(impl_t(aAllocator),aAllocator),//
                FStream(new CTextStream(*this))
{
	MInit();
	assign(std_str);
}
CText& CText::operator=(const std::string& std_str)
{
	return assign(std_str);
}
CText::CText(const std::string& std_str, size_type str_idx, size_type str_num,
		allocator_type*aAllocator) :
		FImpl(impl_t(aAllocator),aAllocator),//
        FStream(new CTextStream(*this))
{
	MInit();
	assign(std_str, str_idx, str_num);
}
CText::CText(size_type num, utf32 code_point,
		allocator_type*aAllocator) :
		FImpl(impl_t(aAllocator),aAllocator),//
        FStream(new CTextStream(*this))
{
	MInit();
	assign(num, code_point);
}
CText::CText(const_iterator const& iter_beg, const_iterator const& iter_end,
		allocator_type*aAllocator) :
		FImpl(impl_t(aAllocator),aAllocator),//
        FStream(new CTextStream(*this))
{
	MInit();
	append(iter_beg, iter_end);
}
CText::CText(utf8 const* cstr, ICodeConv const& aType,
		allocator_type*aAllocator):
				FImpl(impl_t(aAllocator),aAllocator),//
                FStream(new CTextStream(*this))
{
	MInit();
	assign(cstr, aType);
}
CText& CText::operator=(utf8 const* cstr)
{
	return assign(cstr);
}
CText::CText(utf8 const* chars, size_type chars_len, ICodeConv const& aType,
		allocator_type*aAllocator):
				FImpl(impl_t(aAllocator),aAllocator),//
                FStream(new CTextStream(*this))
{
	MInit();
	assign(chars, chars_len, aType);
}
CText& CText::operator=(utf32 code_point)
{
	return assign(1, code_point);
}
inline void CText::MInit()
{
	FCodePointLength=0;
}
CText::impl_t::impl_t(allocator_type* aAllocator):
		FSingleByteBuffer(NULL),//
		FSingleByteBufferLen(0),//
		FSingleByteDatalen(0),//
		FMultiByteBuf(NULL),//
		FMultiByteBufferLen(0),//
		FAllocator(aAllocator)
{
	;
}
CText::impl_t::impl_t(const impl_t& aRht) :
		FSingleByteBuffer(NULL), //
		FSingleByteBufferLen(0), //
		FSingleByteDatalen(0), //
		FMultiByteBuf(NULL),//
		FMultiByteBufferLen(0),//
		FAllocator(aRht.FAllocator)
{
	if (aRht.FMultiByteBuf)
	{
		CHECK(aRht.FMultiByteBufferLen);
		FMultiByteBufferLen=aRht.FMultiByteBufferLen;
		FMultiByteBuf=(utf32*)FAllocator->MAllocate(FMultiByteBufferLen * sizeof(FMultiByteBuf[0]));
		memcpy(FMultiByteBuf, aRht.FMultiByteBuf,
				FMultiByteBufferLen * sizeof(FMultiByteBuf[0]));//fixme don't coping all buffer!!
	}
}
CText::impl_t::~impl_t()
{
	if (FSingleByteBuffer)
	{
		FAllocator->MDeallocate(FSingleByteBuffer, FSingleByteBufferLen);
	}
	if(FMultiByteBuf)
	{
		FAllocator->MDeallocate(FMultiByteBuf, FMultiByteBufferLen* sizeof(FMultiByteBuf[0]));
	}

	FSingleByteBuffer=NULL;
	FMultiByteBuf=NULL;
}
CText::~CText()
{

}
CText::reference CText::at(size_type idx)
{
	CHECK_LE(idx, size()) << "Index is out of range for CText";
	return operator[](idx);
}
CText::const_reference CText::at(size_type idx) const
{
	CHECK_LE(idx, size()) << "Index is out of range for CText";

	return operator[](idx);
}
int CText::compare(size_type aFirst, size_type aLen, const CText& aStr,
		size_type aStrFirst, size_type aStrLen) const
{
	if (aStr.empty()) // empty
	{
		VLOG_IF(1,empty())
										<< "The empty string is comparing with empty string.";
		LOG_IF(WARNING,aLen>size()) << "Invalid length of string";
		aLen = std::min(aLen, size());
		return aLen ? 1 : 0;
	}
	else if (empty())
		return -1;

	CHECK_GT(size() , aFirst) << "Index was out of range for CText object";

	CHECK_GT(aStr.size() , aStrFirst)
			<< "Index was out of range for CText object. StrLen=" << aFirst
			<< ", Code point length=" << size();

	if ((aStrLen == npos) || (aStrFirst + aStrLen > aStr.size()))
		aStrLen = aStr.size() - aStrFirst;

	if (aLen)
	{
		size_type cp_count = std::min(aLen, aStrLen);
		const utf32* _src = ptr_const() + aFirst;
		const utf32* _rht = aStr.ptr_const() + aStrFirst;

		for (; cp_count--; _rht++, _src++)
			if (*_src != *_rht)
				return (*_src < *_rht) ? -1 : 1;
	}
	if (aLen < aStrLen)
		return -1;
	else if (aLen == aStrLen)
		return 0;
	else
		return 1;
}
int CText::compare(size_type aFirst, size_type aLen, const std::string& std_str,
		size_type str_idx, size_type str_len, ICodeConv const& aType) const
{
	if (std_str.empty()) // empty
	{
//		LOG_IF(WARNING,FUCS4Length)
//											<< "The Index was out of range for the empty object";

		LOG_IF(WARNING,aLen>size()) << "Invalid length of string";
		aLen = std::min(aLen, size());
		return aLen ? 1 : 0;
	}
	else if (empty())
		return -1;

	CHECK_GT(size() , aFirst) << "Index was out of range for CText object";

	CHECK_GT(std_str.size() , str_idx)
			<< "Index was out of range for std::string object";

	if ((str_len == npos) || (str_idx + str_len > std_str.size()))
		str_len = (size_type) aType.MLengthOf(std_str.c_str(), str_len) - str_idx;

	return compare(aFirst, aLen, &std_str.c_str()[str_idx], str_len);
}
int CText::compare(size_type aFirst, size_type aLen, utf8 const* aRhtStr,
		size_type aStrLen, ICodeConv const& aType) const
{
	if (aStrLen == 0) // empty
	{
//		LOG_IF(WARNING,FUCS4Length)
//											<< "The Index was out of range for the empty object";

		LOG_IF(WARNING,aLen>size()) << "Invalid length of string";
		aLen = std::min(aLen, size());
		return aLen ? 1 : 0;
	}
	else if (empty())
		return -1;

	CHECK_GT(size() , aFirst) << "Index was out of range for CText object";
	CHECK_NE(aStrLen , npos)
			<< "Length for utf8 const encoded string can not be 'npos'";

	if ((aLen == npos) || (aFirst + aLen > size()))
		aLen = size() - aFirst;

	if (aLen != 0)
	{
		utf8 const* const _rht_end = aRhtStr + strlen(aRhtStr);

		utf32 const* _src_begin = ptr_const() + aFirst;
		utf32 const* const _dest_end = _src_begin + std::min(aLen, aStrLen);

		for (utf32 _code; (aRhtStr != _rht_end) && (_src_begin != _dest_end);
				++_src_begin)
		{
			_code = aType.MNext(aRhtStr, _rht_end);
			if (*_src_begin != _code)
				return (*_src_begin < _code) ? -1 : 1;
		}
		LOG_IF(WARNING,_src_begin!=_dest_end) << " The string is small.";
	}
	if (aLen < aStrLen)
		return -1;
	else if (aLen == aStrLen)
		return 0;
	else
		return 1;
}
int CText::compare(const CText& str) const
{
	return compare(0, size(), str);
}
int CText::compare(const std::string& std_str) const
{
	return compare(0, size(), std_str);
}
CText::size_type CText::length_code(ICodeConv const& aType) const
{
	if (empty())
		return 0;
	return aType.MSizeOf(ptr_const(), ptr_const() + size());
}

int CText::compare(utf8 const* cstr, ICodeConv const& aType) const
{
	return compare(0, size(), cstr, aType.MLengthOf(cstr), aType);
}
int CText::compare(size_type idx, size_type len, utf8 const* cstr,
		ICodeConv const& aType) const
{
	return compare(idx, len, cstr, aType.MLengthOf(cstr), aType);
}
CText::size_type CText::copy(char* buf, size_type len, size_type idx,
		ICodeConv const& aType) const
{
	CHECK_GT(size() , idx) << "Index was out of range for CText object";

	if (len == npos)
		len = size();

	return MFromUCS4(&ptr_const()[idx], buf, npos, len, aType);
}

//////////////////////////////////////////////////////////////////////////
// Assignment Functions
//////////////////////////////////////////////////////////////////////////
CText& CText::assign(const CText& str, size_type str_idx, size_type str_num)
{
	MWillBeenChanged();
	VLOG_IF(5,str.empty()) << "String is NULL";
	if (str.empty())
	{
		MGrow(0);
		MSetLen(0);
		return *this;
	}
	CHECK_GT(str.size() , str_idx)
			<< "Index was out of range for std::string object";

	if ((str_num == npos) || (str_num > str.size() - str_idx))
		str_num = str.size() - str_idx;

	MGrow(str_num);
	MSetLen(str_num);
	memcpy(ptr(), &str.ptr_const()[str_idx], str_num * sizeof(utf32));

	return *this;
}

CText& CText::assign(const std::string& std_str, size_type str_idx,
		size_type str_num, ICodeConv const& aType)
{
	VLOG_IF(2,std_str.empty()) << "String is NULL";
	if (std_str.empty())
	{
		MWillBeenChanged();
		MGrow(0);
		MSetLen(0);
		return *this;
	}
	CHECK_GT(std_str.size() , str_idx)
			<< "Index was out of range for std::string object";

	if ((str_num == npos) || (str_num > (size_type) std_str.size() - str_idx))
	{
		LOG_IF(WARNING,str_num != npos) << "Size was out of range std::string";
		str_num = aType.MLengthOf(std_str.c_str(), str_num) - str_idx;
	}

	return assign(&std_str.c_str()[str_idx], str_num, aType);
}
CText& CText::assign(utf8 const* utf8_str, size_type str_num,
		ICodeConv const& aType)
{
	MWillBeenChanged();
	VLOG_IF(2,!str_num) << "String is NULL";
	if (!str_num)
	{
		MGrow(0);
		MSetLen(0);
		return *this;
	}

	CHECK_NE(str_num , npos)
			<< "Length for utf8 encoded string can not be 'npos'";
	CHECK_LE(str_num, aType.MLengthOf( utf8_str,str_num))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	VLOG(2) << utf8_str;
	MGrow(str_num);

	MToUCS4(utf8_str, ptr(), MGetBufferLength(), aType);
	MSetLen(str_num);
	return *this;
}
CText& CText::assign(size_type num, utf32 code_point)
{
	MWillBeenChanged();
	CHECK_NE (num , npos) << "Code point count can not be 'npos'";

	VLOG_IF(2,!num) << "String is NULL";
	if (!num)
	{
		MGrow(0);
		MSetLen(0);
		return *this;
	}

	MGrow(num);
	MSetLen(num);
	utf32* p = ptr();

	while (num--)
		*p++ = code_point;

	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Appending Functions
//////////////////////////////////////////////////////////////////////////
CText& CText::append(const CText& str, size_type str_idx, size_type str_num)
{
	LOG_IF(ERROR,str.empty()) << "Appending the empty string";
	if (str.empty())
		return *this;
	MWillBeenChanged();
	CHECK_GT (str.size() , str_idx) << "Index is out of range for CText";

	if ((str_num == npos) || (str_num > str.size() - str_idx))
		str_num = str.size() - str_idx;

	MGrow(size() + str_num);
	memcpy(&ptr()[size()], &str.ptr_const()[str_idx], str_num * sizeof(utf32));
	MSetLen(size() + str_num);
	return *this;
}
CText& CText::append(const std::string& std_str, size_type str_idx,
		size_type str_num, ICodeConv const& aType)
{
	LOG_IF(ERROR,std_str.empty()) << "Appending the empty string";
	if (std_str.empty())
		return *this;

	CHECK_GT (std_str.size() , str_idx)
			<< "Index is out of range for std::string";

	if ((str_num == npos) || (str_num > (size_type) std_str.size() - str_idx))
	{
		LOG_IF(WARNING,str_num != npos) << "Size was out of range std::string";
		str_num = (size_type) aType.MLengthOf(std_str.c_str(), str_num) - str_idx;
	}
	return append(&std_str.c_str()[str_idx], str_num);
}
CText& CText::append(utf8 const* utf8_str, size_type len,
		ICodeConv const& aType)
{
	LOG_IF(ERROR,!len) << "Appending the empty string";
	if (!len)
		return *this;
	MWillBeenChanged();
	CHECK_NE (len , npos) << "Length for  string can not be 'npos'";
	VLOG(2) << "Length of str:" << len << ", Real length:"
						<< aType.MLengthOf(utf8_str,len);

	CHECK_LE(len, aType.MLengthOf(utf8_str,len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	MGrow(size() + len);
	MToUCS4(utf8_str, ptr() + size(), len, aType);

	MSetLen(size() + len);

	return *this;
}
CText& CText::append(size_type num, utf32 code_point)
{
	CHECK_NE (num , npos) << "Code point count can not be 'npos'";
	MWillBeenChanged();
	size_type newsz = size() + num;
	MGrow(newsz);

	utf32* p = &ptr()[size()];

	while (num--)
		*p++ = code_point;

	MSetLen(newsz);

	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Insertion Functions
//////////////////////////////////////////////////////////////////////////
CText& CText::insert(size_type idx, const CText& str, size_type str_idx,
		size_type str_num)
{
	LOG_IF(ERROR,str.empty()) << "Appending the empty string";
	if (str.empty())
		return *this;
	MWillBeenChanged();
	if (!empty())
		CHECK_GT(size() , idx) << "Index is out of range for CText";
	CHECK_GT(str.size() , str_idx) << "Index is out of range for CText";

	if ((str_num == npos) || (str_num > str.size() - str_idx))
		str_num = str.size() - str_idx;

	size_type newsz = size() + str_num;
	MGrow(newsz);
	memmove(&ptr()[idx + str_num], &ptr()[idx],
			(size() - idx) * sizeof(utf32));
	memcpy(&ptr()[idx], &str.ptr_const()[str_idx], str_num * sizeof(utf32));
	MSetLen(newsz);

	return *this;
}
CText& CText::insert(size_type idx, const std::string& std_str,
		size_type str_idx, size_type str_num, ICodeConv const& aType)
{
	LOG_IF(ERROR,std_str.empty()) << "Appending the empty string";
	if (std_str.empty())
		return *this;

	if ((str_num == npos) || (str_num > (size_type) std_str.size() - str_idx))
	{
		LOG_IF(WARNING,str_num != npos) << "Size was out of range std::string";
		str_num = (size_type) aType.MLengthOf(std_str.c_str(), str_num) - str_idx;
	}
	return insert(idx, &std_str.c_str()[str_idx], str_num);
}
CText& CText::insert(size_type idx, utf8 const* utf8_str, size_type len,
		ICodeConv const& aType)
{
	MWillBeenChanged();
	if (!empty())
		CHECK_GT(size() , idx) << "Index is out of range for CText";
	CHECK_NE (len ,npos) << "Length of char array can not be 'npos'";

	CHECK_LE(len, aType.MLengthOf(utf8_str,len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	size_type newsz = size() + len;

	MGrow(newsz);
	memmove(ptr() + idx + len, ptr() + idx,
			(size() - idx) * sizeof(utf32));

	MToUCS4(utf8_str, ptr() + idx, len, aType);

	MSetLen(newsz);
	return *this;
}
CText& CText::insert(size_type idx, size_type num, utf32 code_point)
{
	LOG_IF(ERROR,code_point == '\0')<<"Unexpected behavior!!";
	if (num == 0 || code_point == '\0')
		(void) 0;
	else
	{

		MWillBeenChanged();
		if (!empty())
			CHECK_GT(size() , idx) << "Index is out of range for CText";

		CHECK_NE (num ,npos) << "Length of char array can not be 'npos'";

		size_type newsz = size() + num;
		MGrow(newsz);

		memmove(&ptr()[idx + num], &ptr()[idx],
				(size() - idx) * sizeof(utf32));

		utf32* pt = &ptr()[idx + num - 1];

		while (num--)
		*pt-- = code_point;

		MSetLen(newsz);
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Erasing characters
//////////////////////////////////////////////////////////////////////////
CText& CText::erase(size_type idx, size_type len)
{
	// cover the no-op case.
	if (len == 0)
		return *this;
	MWillBeenChanged();
	CHECK_GT(size(),idx) << "Index is out of range for CText";

	if (len == npos)
		len = size() - idx;

	size_type newsz = size() - len;

	memmove(&ptr()[idx], &ptr()[idx + len],
			(size() - idx - len) * sizeof(utf32));
	MSetLen(newsz);
	return *this;
}
//////////////////////////////////////////////////////////////////////////
// Resizing
//////////////////////////////////////////////////////////////////////////
void CText::resize(size_type num, utf32 code_point)
{
	if (num < size())
	{
		MWillBeenChanged();
		MSetLen(num);
	}
	else
	{
		append(num - size(), code_point);
	}

}
//////////////////////////////////////////////////////////////////////////
// Replacing Characters
//////////////////////////////////////////////////////////////////////////
CText& CText::replace(size_type idx, size_type len, const CText& str,
		size_type str_idx, size_type str_num)
{
	CHECK_GE(size() , idx) << "Index is out of range for CText";
	CHECK_GE(str.size() , str_idx) << "Index is out of range for CText";

	if (((str_idx + str_num) > str.size()) || (str_num == npos))
		str_num = str.size() - str_idx;

	if (((len + idx) > size()) || (len == npos))
		len = size() - idx;

	MWillBeenChanged();

	size_type newsz = size() + str_num - len;

	MGrow(newsz);

	if ((idx + len) < size())
		memmove(&ptr()[idx + str_num], &ptr()[len + idx],
				(size() - idx - len) * sizeof(utf32));

	memcpy(&ptr()[idx], &str.ptr_const()[str_idx], str_num * sizeof(utf32));
	MSetLen(newsz);

	return *this;
}
CText& CText::replace(size_type idx, size_type len, const std::string& std_str,
		size_type str_idx, size_type str_num)
{
	CHECK_GE(size() , idx) << "Index is out of range for CText";

	CHECK_GE(std_str.size() , str_idx)
			<< "Index is out of range for  std::string";

	if (((str_idx + str_num) > std_str.size()) || (str_num == npos))
		str_num = (size_type) std_str.size() - str_idx;

	if (((len + idx) > size()) || (len == npos))
		len = size() - idx;

	MWillBeenChanged();

	size_type newsz = size() + str_num - len;

	MGrow(newsz);

	if ((idx + len) < size())
		memmove(&ptr()[idx + str_num], &ptr()[len + idx],
				(size() - idx - len) * sizeof(utf32));

	utf32* pt = &ptr()[idx + str_num - 1];

	while (str_num--)
		*pt-- = static_cast<utf32>(static_cast<unsigned char>(std_str[str_idx
				+ str_num]));

	MSetLen(newsz);

	return *this;
}
CText& CText::replace(size_type idx, size_type len, utf8 const* utf8_str,
		size_type str_len, ICodeConv const& aType)
{
	CHECK_GE(size(), idx) << "Index is out of range for CText";
	CHECK_NE(str_len , npos)
			<< "Length for utf8 encoded string can not be 'npos'";

	CHECK_GE(len, aType.MLengthOf(utf8_str,len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	if (((len + idx) > size()) || (len == npos))
	{
		LOG_IF(WARNING,len != npos) << "Length was out of range ";
		len = size() - idx;
	}

	MWillBeenChanged();

	size_type newsz = size() + str_len - len;

	MGrow(newsz);

	if ((idx + len) < size())
		memmove(ptr() + idx + str_len, ptr() + len + idx,
				(size() - idx - len) * sizeof(utf32));

	MToUCS4(utf8_str, ptr() + idx, str_len, aType, str_len);

	MSetLen(newsz);
	return *this;
}
CText& CText::replace(size_type idx, size_type len, size_type num,
		utf32 code_point)
{
	CHECK_GE(size(), idx) << "Index is out of range for CText";
	CHECK_NE(len , npos) << "Length for utf8 encoded string can not be 'npos'";

	if (((len + idx) > size()) || (len == npos))
		len = size() - idx;

	MWillBeenChanged();

	size_type newsz = size() + num - len;

	MGrow(newsz);

	if ((idx + len) < size())
		memmove(&ptr()[idx + num], &ptr()[len + idx],
				(size() - idx - len) * sizeof(utf32));

	utf32* pt = &ptr()[idx + num - 1];

	while (num--)
		*pt-- = code_point;

	MSetLen(newsz);

	return *this;
}
CText& CText::replace(iterator iter_beg, iterator iter_end,
		const_iterator iter_newBeg, const_iterator iter_newEnd)
{
	if (iter_newBeg == iter_newEnd)
	{
		erase(MSafeIterDif(iter_beg, iterator(ptr())),
				MSafeIterDif(iter_end, iter_beg));
	}
	else
	{

		MWillBeenChanged();

		size_type str_len = MSafeIterDif(iter_newEnd, iter_newBeg);
		size_type idx = MSafeIterDif(iter_beg, iterator(ptr()));
		size_type len = MSafeIterDif(iter_end, iter_beg);

		if ((len + idx) > size())
			len = size() - idx;

		size_type newsz = size() + str_len - len;
		utf32* const _old_ptr=ptr();

		MGrow(newsz);

		utf32* const _ptr=ptr();

		if (_old_ptr != _ptr //
				&& ((ptr_const() < iter_newBeg.d_ptr
						&& iter_newBeg.d_ptr
								< (ptr_const() + MGetBufferLength())) || //
						(ptr_const() < iter_newEnd.d_ptr
								&& iter_newEnd.d_ptr
										< (ptr_const() + MGetBufferLength()))))
		{
			LOG(ERROR)<< "Undefined behavior! The buffer has been reallocated. Therefore iterator has been invalidated.";
			CText::size_type const _begin=MSafeIterDif(iter_newBeg, iterator(_old_ptr));
			CText::size_type const _end=MSafeIterDif(iter_newEnd, iterator(_old_ptr));
			iter_newBeg=const_iterator(_ptr+_begin);
			iter_newEnd=const_iterator(_ptr+_end);
		}
		if ((idx + len) < size())
			memmove(&_ptr[idx + str_len], &_ptr[len + idx],
					(size() - idx - len) * sizeof(utf32));

		memcpy(&_ptr[idx], iter_newBeg.d_ptr, str_len * sizeof(utf32));
		MSetLen(newsz);
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////////
// Find a code point
//////////////////////////////////////////////////////////////////////////

CText::size_type CText::find(utf32 code_point, size_type idx) const
{
	if (idx >= size())
	{
		VLOG(0) << "Invalid string index indx=" << idx;
		return npos;
	}

	const utf32* _begin = ptr_const() + idx;
	const utf32* _end = ptr_const() + size();

	for (; _begin < _end; ++_begin)
	{
		if (*_begin == code_point)
			return _begin - ptr_const();
	}

	return npos;
}
CText::size_type CText::rfind(utf32 code_point, size_type aIndx) const
{
	if (empty())
	{
		LOG(WARNING)<< "String Length is Null ="<<size();
		return npos;
	}
	if (aIndx >= size())
	aIndx = size() - 1;

	const utf32 *_begin = ptr_const()
	+ (aIndx < size() - 1 ? aIndx : size() - 1 );

	for(;;--_begin)
	{
		if (*_begin == code_point)
		return _begin- ptr_const(); // found a match

		if(_begin== ptr_const())
		break;
	}
	return npos;
}
CText::size_type CText::find(const CText& str, size_type aIndx) const
{
	if (((str.empty()) && (aIndx < size()))
			|| aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx<<" or String is Null Length="<<str.size();
		return npos;
	}

	// loop while search string could fit in to search area
	for (;(size() - aIndx) >= str.size();++aIndx)//FIXME
	{
		if (0 == compare(aIndx, str.size(), str)) return aIndx;
	}

	return npos;
}

CText::size_type CText::rfind(const CText& str, size_type idx) const
{
	if (str.empty())
		return (idx < size()) ? idx : size();

	if (str.size() > size())
		return npos;

	if (idx > (size() - str.size()))
		idx = size() - str.size();

	do
	{
		if (0 == compare(idx, str.size(), str))
			return idx;

	} while (idx-- != 0);

	return npos;
}
CText::size_type CText::find(const std::string& std_str, size_type idx) const
{
	std::string::size_type sze = std_str.size();

	if ((sze == 0) && (idx < size()))
		return idx;

	return find(std_str.c_str(), idx, std_str.length());
}
CText::size_type CText::rfind(const std::string& std_str, size_type idx) const
{
	std::string::size_type sze = std_str.size();

	if (sze == 0)
		return (idx < size()) ? idx : size();

	return rfind(std_str.c_str(), idx, std_str.length());
}

CText::size_type CText::find(utf8 const* utf8_str, size_type idx,
		size_type str_len, ICodeConv const& aType) const
{
	CHECK_NE (str_len , npos)
			<< "Length for utf8 encoded string can not be 'npos'";

	CHECK_LE(str_len, aType.MLengthOf(utf8_str,str_len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	if (idx >= size() || str_len == 0)
	{
		LOG(WARNING)<<"Invalid string length . Indx="<<idx
		<<" , Size="<<str_len<<", Code Point Length="<<size();
		return npos;
	}

	for (; (size() - idx) >= str_len; ++idx)
	{
		if (0 == compare(idx, str_len, utf8_str, str_len, aType))
			return idx;
	}

	return npos;
}
CText::size_type CText::rfind(utf8 const* utf8_str, size_type aPos,
		size_type str_len, ICodeConv const& aType) const
{
	CHECK_NE (str_len , npos)
			<< "Length for utf8 encoded string can not be 'npos'";

	CHECK_LE(str_len, aType.MLengthOf(utf8_str,str_len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	if (aPos >= size() || str_len == 0)
	{
		LOG(WARNING)<<"Invalid string length . Indx="<<aPos
		<<" , Size="<<str_len<<", Code Point Length="<<size();
		return npos;
	}
	VLOG(2) << "FCodePointLength=" << size();
	aPos = std::min(size() - str_len, aPos);

	do
	{
		VLOG(2) << "indx=" << aPos << " ; Size=" << str_len << " ;"
							<< utf8_str;
		if (0 == compare(aPos, str_len, utf8_str, str_len, aType))
			return aPos;

	} while (aPos-- != 0);

	return npos;
}

//////////////////////////////////////////////////////////////////////////
// Find first of different code-points
//////////////////////////////////////////////////////////////////////////
CText::size_type CText::find_first_of(const CText& str, size_type aIndx) const
{
	if (aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx;
		return npos;
	}

	const utf32* _begin = ptr_const()+aIndx;
	const utf32* _end = ptr_const()+size();

	for(;_begin != _end;++_begin)
	{
		if (npos != str.find(*_begin))
		return _begin- ptr_const();
	}
	return npos;
}
CText::size_type CText::find_first_not_of(const CText& str,
		size_type aIndx) const
{
	if (aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx;
		return npos;
	}

	const utf32* _begin = ptr_const()+aIndx;
	const utf32* _end = ptr_const()+size();

	for(;_begin != _end;++_begin)
	{
		if (npos == str.find(*_begin))
		return _begin- ptr_const();
	};

	return npos;
}
CText::size_type CText::find_first_of(const std::string& std_str,
		size_type aIndx) const
{
	if (aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx;
		return npos;
	}

	return find_first_of(std_str.c_str(),aIndx,std_str.length());
}
CText::size_type CText::find_first_not_of(const std::string& std_str,
		size_type aIndx) const
{
	if (aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx;
		return npos;
	}
	return find_first_not_of(std_str.c_str(),aIndx,std_str.length());

}
CText::size_type CText::find_first_of(utf8 const* utf8_str, size_type aIndx,
		size_type str_len, ICodeConv const& aType) const
{
	CHECK_NE(str_len , npos) << "Length for string can not be 'npos'";

	if (aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx;
		return npos;
	}

	CHECK_LE(str_len, aType.MLengthOf(utf8_str,str_len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	const utf32* _begin = ptr_const() + aIndx;
	const utf32* _end = ptr_const() + size();

	VLOG(3) << "New Search:";
	for (; _begin != _end; ++_begin)
	{
		VLOG(3) << "Charaster:" << static_cast<char>(*_begin);
		if (npos != MFindCodepoint(utf8_str, str_len, *_begin, aType)) //utf8 char
			return _begin - ptr_const();
	}

	return npos;
}

CText::size_type CText::find_first_not_of(utf8 const* utf8_str, size_type aIndx,
		size_type str_len, ICodeConv const& aType) const
{
	CHECK_NE(str_len , npos) << "Length for string can not be 'npos'";

	if (aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx;
		return npos;
	}
	CHECK_LE(str_len, aType.MLengthOf(utf8_str,str_len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	const utf32* _begin = ptr_const() + aIndx;
	const utf32* _end = ptr_const() + size();

	for (; _begin != _end; ++_begin)
	{
		if (npos == MFindCodepoint(utf8_str, str_len, *_begin, aType)) //utf8 char
			return _begin - ptr_const();
	}

	return npos;
}

CText::size_type CText::find_first_not_of(utf32 code_point,
		size_type aIndx) const
{
	if (aIndx >= size())
	{
		LOG(WARNING)<<"Invalid string index indx="<<aIndx;
		return npos;
	}
	const utf32* _begin = ptr_const();
	const utf32* _end = ptr_const() + size();

	for(;_begin!=_end;++_begin)
	{
		if (*_begin != code_point)
		return _begin- ptr_const();
	}
	return npos;
}

//////////////////////////////////////////////////////////////////////////
// Find last of different code-points
//////////////////////////////////////////////////////////////////////////
CText::size_type CText::find_last_of(const CText& str, size_type aIndx) const
{
	if (empty())
	{
		LOG(WARNING)<<"String is null.";
		return npos;
	}

	if (aIndx >= size())
	aIndx = size() - 1;

	const utf32* _begin = ptr_const() + aIndx;
	for (;;)
	{
		if (npos != str.find(*_begin))
		return _begin - ptr_const();

		if (_begin == ptr_const())
		break;
		_begin--;
	};

	return npos;
}
CText::size_type CText::find_last_not_of(const CText& str,
		size_type aIndx) const
{
	if (empty())
	{
		LOG(WARNING)<<"String is null.";
		return npos;
	}

	if (aIndx >= size())
	aIndx = size() - 1;

	const utf32* _begin = ptr_const() + aIndx;

	for (;;)
	{
		if (npos == str.find(*_begin))
		return _begin - ptr_const();

		if (_begin == ptr_const())
		break;
		_begin--;
	};

	return npos;
}
CText::size_type CText::find_last_of(utf8 const* utf8_str, size_type aIndx,
		size_type str_len, ICodeConv const& aType) const
{
	CHECK_NE(str_len , npos) << "Length for string can not be 'npos'";

	CHECK_LE(str_len, aType.MLengthOf(utf8_str,str_len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	if (empty())
	{
		LOG(WARNING)<<"String is null.";
		return npos;
	}

	if (aIndx >= size())
		aIndx = size() - 1;

	const utf32* _begin = ptr_const() + aIndx;
	for (;;)
	{
		if (npos != MFindCodepoint(utf8_str, str_len, *_begin, aType))
			return _begin - ptr_const();

		if (_begin == ptr_const())
			break;

		_begin--;
	};
	return npos;
}
CText::size_type CText::find_last_not_of(utf8 const* utf8_str, size_type aIndx,
		size_type str_len, ICodeConv const& aType) const
{
	CHECK_NE(str_len , npos) << "Length for string can not be 'npos'";

	CHECK_LE(str_len, aType.MLengthOf(utf8_str,str_len))
			<< ". The length is size of string, but not size of string memory buffer. "
					"For a Russian char It is not equal value. The Buffer size is more than the string size.";

	if (empty())
	{
		LOG(WARNING)<<"String is null.";
		return npos;
	}
	if (aIndx >= size())
		aIndx = size() - 1;

	const utf32* _begin = ptr_const() + aIndx;

	for (;;)
	{
		if (npos == MFindCodepoint(utf8_str, str_len, *_begin, aType))
			return _begin - ptr_const();

		if (_begin == ptr_const())
			break;

		_begin--;
	};

	return npos;
}
CText::size_type CText::find_last_of(const std::string& std_str,
		size_type idx) const
{
	return find_last_of(std_str.c_str(), idx, std_str.length());
}
CText::size_type CText::find_last_not_of(const std::string& std_str,
		size_type idx) const
{
	return find_last_not_of(std_str.c_str(), idx, std_str.length());
}

CText::size_type CText::find_last_not_of(utf32 code_point,
		size_type aIndx) const
{
	if (empty())
	{
		LOG(WARNING)<<"String is null.";
		return npos;
	}
	if (aIndx >= size())
	aIndx = size() - 1;

	const utf32* _begin = ptr_const() + aIndx;

	for (;;)
	{
		if (*_begin!= code_point)
		return _begin - ptr_const();

		if (_begin == ptr_const())
		break;

		_begin--;
	};
	return npos;
}
CText::size_type CText::find_last_of(utf32 code_point, size_type aIndx) const
{
	return rfind(code_point, aIndx);
}
//////////////////////////////////////////////////////////////////////////
// Substring
//////////////////////////////////////////////////////////////////////////
CText CText::substr(size_type idx, size_type len) const
{
	if (empty() || size() == idx)
		return CText();
	CHECK_GT(size(), idx) << "Index is out of range for CText";
	return CText(*this, idx, len);
}
/*************************************************************************
 Implementation Functions
 *************************************************************************/
bool CText::MGrow(size_type new_size)
{
	// check for too big
	CHECK_GT(max_size(),new_size) << "Resulting CText would be too big";

	// increase, as we always null-terminate the buffer.
	++new_size;

	if (new_size > MGetBufferLength())
	{
		const size_type _reserved_new_size=static_cast<size_type>(new_size*1.3);
		cow_impl_t _tmp = FImpl;

		cow_impl_t _new(impl_t(_tmp.MRead().FAllocator),_tmp.MGetAllocator());

		impl_t& _new_data=_new.MWrite();

		_new_data.FMultiByteBufferLen = _reserved_new_size;
		_new_data.FMultiByteBuf = (utf32*) _new_data.FAllocator->MAllocate(
				_new_data.FMultiByteBufferLen * sizeof(utf32));

		memcpy(_new_data.FMultiByteBuf, _tmp.MRead().FMultiByteBuf,
				size() * sizeof(utf32));
		FImpl=_new;
		return true;
	}

	return false;
}

CText& CText::MTrimSpace()
{
	if (empty())
		return *this;
	size_type _ind = find_last_not_of(static_cast<utf32>(' '));
	if (_ind == npos)
		clear();
	else if (_ind != (length() - 1))
		erase(_ind + 1, npos);

	return *this;
}
void CText::MRemoveBuffer(void)
{
	FImpl=cow_impl_t(impl_t(FImpl.MRead().FAllocator),FImpl.MGetAllocator());
}
void CText::clear()
{
	if(!empty())
	{
		MWillBeenChanged();
		MSetLen(0);
	}
}
inline void CText::MSetLen(size_type len)
{
	FCodePointLength = len;
	ptr()[len] = (utf32) (0);
}
bool CText::MInside(utf32* inptr)
{
	if (inptr < ptr() || ptr() + size() <= inptr)
		return false;
	else
		return true;
}
CText::size_type CText::MSafeIterDif(const const_iterator& iter1,
		const const_iterator& iter2) const
{
	return (iter1.d_ptr == 0) ? 0 : (iter1 - iter2);
}
CText::size_type CText::MFromUCS4(const utf32* aSrcBegin, char* aDest,
		size_type dest_len, size_type src_len, ICodeConv const& aType) const
{
	// count length for null terminated source...
	if (src_len == 0)
		src_len = MUCS4Length(aSrcBegin);

	char* const _start_dest = aDest;
	char* const _dest_end = aDest + dest_len;
	utf32 const* const _src_end = aSrcBegin + src_len;
	for (; (aSrcBegin != _src_end) && (aDest != _dest_end); ++aSrcBegin)
		if (!aType.MAppend(*aSrcBegin, aDest, _dest_end))
			break;

	return _start_dest - aDest;
}
inline CText::size_type CText::MToUCS4(utf8 const* aSrcBegin, utf32* aDest,
		size_type aDestLen, ICodeConv const& aType, size_type src_len) const
{
	src_len = src_len ? src_len : strlen(aSrcBegin);
	utf8 const* const _src_end = aSrcBegin + src_len;
	utf32* const _dest_end = aDest + aDestLen;
	for (; (aSrcBegin != _src_end) && (aDest != _dest_end); ++aDest)
		*aDest = aType.MNext(aSrcBegin, _src_end);

	return _dest_end - aDest;
}
CText::size_type CText::MUCS4Length(const utf32* utf32_str) const
{
	size_type cnt = 0;
	for (; *utf32_str; utf32_str++)
		cnt++;

	return cnt;
}
static const char g_null = '\0';
utf8* CText::MBuildUtf8Buff(ICodeConv const& _code) const
{
	if (empty())
		return (utf8 *) &g_null;

	impl_t const& _impl = FImpl.MRead();
	if (_impl.FSingleByteDatalen!=0)
		return _impl.FSingleByteBuffer;

	_impl.FSingleByteDatalen=length_code(_code);

	size_type const buffsize =_impl.FSingleByteDatalen + 1;
//	VLOG(2) << "Buff size=" << buffsize << ", UCS4 Length=" << FUCS4Length;
	if (buffsize > _impl.FSingleByteBufferLen)
	{

		if (_impl.FSingleByteBufferLen > 0)
		{
			_impl.FAllocator->MDeallocate(_impl.FSingleByteBuffer, _impl.FSingleByteBufferLen);
		}
		_impl.FSingleByteBufferLen=buffsize;
		_impl.FSingleByteBuffer = (utf8*) _impl.FAllocator->MAllocate(
				_impl.FSingleByteBufferLen);
	}
	MFromUCS4(ptr_const(), _impl.FSingleByteBuffer, _impl.FSingleByteDatalen, size(), _code);

	// always add a null at end
	_impl.FSingleByteBuffer[_impl.FSingleByteDatalen] = ((utf8) 0);

	return _impl.FSingleByteBuffer;
}
inline CText::size_type CText::MFindCodepoint(utf8 const* aStr,
		size_type chars_len, utf32 code_point, ICodeConv const& aType) const
{
	VLOG(2) << "Find Code point for " << static_cast<const char*>(aStr)
						<< " Len:" << chars_len << ", Real Len="
						<< aType.MLengthOf(aStr, chars_len) << "looking for:"
						<< static_cast<char>(code_point) << "(" << code_point
						<< ").Type:" << aType;
	utf8 const* const _src_end = aStr + strlen(aStr);

	for (size_type idx = 0; (aStr != _src_end); ++idx)
	{
		if (aType.MNext(aStr, _src_end) == code_point)
			return idx;
	}
	return npos;
}

CText::size_type CText::MGetBufferLength() const
{
	return FImpl.MRead().FMultiByteBufferLen;
}
utf32* CText::ptr(void)
{
	using namespace std;
	impl_t & _impl = FImpl.MWrite();
	return _impl.FMultiByteBuf;
}

const utf32* CText::ptr_const(void) const
{
	using namespace std;
	impl_t const& _impl = FImpl.MRead();
	return _impl.FMultiByteBuf;
}

//////////////////////////////////////////////////////////////////////////
// Swap
//////////////////////////////////////////////////////////////////////////
void CText::swap(CText& str)
{
	std::swap(FImpl,str.FImpl);
	std::swap(FCodePointLength,str.FCodePointLength);
}
NSHARE::CBuffer& CText::MToBuf(NSHARE::CBuffer& aBuf) const
{
	if (!empty())
	{
		COMPILE_ASSERT(sizeof(CCodeUTF8::utf8_t)>=sizeof(NSHARE::CBuffer::value_type),CannotConvertUtf8ToBuf);
		const size_t _befor = aBuf.size();
		
		aBuf.reserve(_befor +size());
		aBuf.resize(aBuf.capacity()-1/*de bene esse*/);

//		const unsigned _factor=sizeof(CCodeUTF8::utf8_t)/sizeof(NSHARE::CBuffer::value_type);

//		NSHARE::CBuffer::value_type _buf[_factor*4]; //max 4 byte
//		CBuffer::const_iterator const _p_buf(_buf);

		size_type const src_len = size();
		const utf32* _src_begin = ptr_const();
		utf32 const* const _src_end = _src_begin + src_len;
		
		size_t i=0;
		size_t _cur_size = aBuf.size();
		CCodeUTF8::utf8_t* _p_base = reinterpret_cast<CCodeUTF8::utf8_t* const>(aBuf.ptr() +_befor);
		for (; (_src_begin != _src_end); ++_src_begin)
		{
			if (_cur_size < (i + sizeof(utf32)))
			{
				aBuf.resize(_cur_size+ sizeof(utf32));
				_p_base = reinterpret_cast<CCodeUTF8::utf8_t* const>(aBuf.ptr() + _befor);
			}
			
			CCodeUTF8::utf8_t*const _p = _p_base +i;

			size_t _count = CCodeUTF8::sMAppend(
					*_src_begin, _p) - _p;
			i += _count;
			//CHECK_LE(_count, sizeof(_buf));

			//aBuf.insert(aBuf.end(),_p_buf,_p_buf+_count);

			if (_count == 0)
			{
				LOG(DFATAL)<<"Invalid utf8 char code:"<<(*_src_begin);
				break;
			}

		}
		aBuf.resize(_befor + i);
	}
	else
		VLOG(2) << "Print empty text";
	return aBuf;
}
std::ostream& CText::MPrint(std::ostream& aStream) const //optimized for utf8
{
	if (!empty())
	{
		CCodeUTF8::utf8_t _buf[4]; //max 4 byte
		size_type const src_len = size();
		const utf32* _src_begin = ptr_const();
		utf32 const* const _src_end = _src_begin + src_len;

		for (; (_src_begin != _src_end); ++_src_begin)
		{
			size_t _count = CCodeUTF8::sMAppend(*_src_begin, _buf) - _buf;
			CHECK_LE(_count, sizeof(_buf));
			for (size_t i = 0; i < _count; ++i)
				aStream << (char) _buf[i];
			if (_count == 0)
			{
				LOG(DFATAL)<<"Invalid utf8 char code:"<<(*_src_begin);
				break;
			}
		}
	}
	else
		VLOG(2) << "Print empty text";
	return aStream;
}
std::ostream& CText::MPrint(std::ostream& aStream, ICodeConv const& aType) const
{
	if (!empty())
	{
		impl_t const& _impl = FImpl.MRead();
		if (_impl.FSingleByteBufferLen > 0)
			aStream << (const char*) _impl.FSingleByteBuffer;
		else
		{
			aStream <<(const char*)MBuildUtf8Buff(aType);
		}
	}
	else
		VLOG(2) << "Print empty text";
	return aStream;
}

#define HANDLE_NUM(T , base )\
{\
	T i;\
	i = va_arg(argptr, T);\
	std::string _num;\
	bool _result = NSHARE::num_to_str(i, _num, base); /*FIXME format*/ \
	LOG_IF(DFATAL,!_result) << "Cannot read  the field width in " \
									<< CText(_start, _it) \
									<< " interpret as non-escape text"; \
	(void) _result; \
	subst+=_num;\
}\
/*END*/

#define HANDLE_FLOAT(T , precision )\
{\
	T i;\
	i = va_arg(argptr, T);\
	std::string _num;\
	bool _result = false;\
	if (precision != -1)\
		_result = NSHARE::float_to_str(i, _num, precision); /*FIXME format*/\
	else\
		_result = NSHARE::float_to_str(i, _num); /*FIXME format*/\
	LOG_IF(DFATAL,!_result) << "Cannot read  the field width in "\
									<< CText(_start, _it)\
									<< " interpret as non-escape text";\
	(void) _result;\
	subst+=_num;\
}\
/*END*/
enum _mode
{
	_none, _hh, _h, _l, _ll, _L, _j, _z, _t
};
static CText string_printf_v(ICodeConv const& aType, const CText& format,
		va_list argptr)
{
	//parse
	CText result;
	CText::const_iterator _it = format.begin();
	for (; _it != format.end(); ++_it)
	{
		// Copy non-escape chars to result
		if (*_it != '%')
		{
			result.push_back(*_it);
			continue;
		}

		CText::const_iterator _start = _it;
		++_it;
		if (_it == format.end())
		{
			LOG(WARNING)<<"% at the end of the Format String (sprintf) - interpret as text";
			result.push_back(*_it);
			break;
		}
		else if(*_it=='%')
		{
			result.push_back(*_it);
			continue;
		}
		std::string _num_flags;
		bool _is_left = false;
		for (; _it != format.end(); ++_it)
		{
			bool _is_end = false;
			switch (*_it)
			{
			case '#':
			case '0':
			case ' ':
			case '+':
				_num_flags.push_back(*_it);
				break;
			case '-':
				_is_left = true;
				break;

			default:
				_is_end = true;
				break;
			}
			if (_is_end)
				break;
		}
		if (_it == format.end())
		{
			LOG(WARNING)<<"incomplete escape, interpret as non-escape text";
			result.insert(result.end(),_start,format.end());
			break;
		}
		//parse width
		int width = -1;
		if (::isdigit(*_it))
		{
			std::string _str;
			for (; _it != format.end() && ::isdigit(*_it); ++_it)
				_str.push_back(*_it);
			bool _result = NSHARE::str_to_decimal(_str, width); //FIXME
			LOG_IF(DFATAL,!_result) << "Cannot read  the field width in "
											<< CText(_start, _it)
											<< " interpret as non-escape text";

			if (!_result)
			{
				result.insert(result.end(), _start, _it);
				--_it; //in cicle will be incremented
				continue;
			}
		}
		if (_it == format.end())
		{
			LOG(WARNING)<<"incomplete escape, interpret as non-escape text";
			result.insert(result.end(),_start,format.end());
			break;
		}
		int precision = -1; // -1 means unspecified
		if (*_it == '.')
		{
			++_it;
			if (::isdigit(*_it))
			{
				std::string _str;
				for (; _it != format.end() && ::isdigit(*_it); ++_it)
					_str.push_back(*_it);
				bool _result = NSHARE::str_to_decimal(_str, precision); //FIXME
				LOG_IF(DFATAL,!_result) << "Cannot read  the field width in "
												<< CText(_start, _it)
												<< " interpret as non-escape text";

				if (!_result)
				{
					result.insert(result.end(), _start, _it);
					--_it; //in cicle will be incremented
					continue;
				}
			}
		}
		if (_it == format.end())
		{
			LOG(WARNING)<<"incomplete escape, interpret as non-escape text";
			result.insert(result.end(),_start,format.end());
			break;
		}

		_mode length_mod = _none;
		switch (*_it)
		{
		case 'h':
			++_it;
			if (_it != format.end() && *_it == 'h')
			{
				length_mod = _hh;
				++_it;
			}
			else
				length_mod = _h;
			break;

		case 'l':
			++_it;
			if (_it != format.end() && *_it == 'l')
			{
				length_mod = _ll;
				++_it;
				;
			}
			else
				length_mod = _l;
			break;

		case 'L':
			++_it;
			length_mod = _L;
			break;

		case 'j':
			++_it;
			length_mod = _j;
			break;

		case 'z':
		case 'Z':
			++_it;
			length_mod = _z;
			break;

		case 't':
			++_it;
			length_mod = _t;
			break;

		default:
			break;
		}
		if (_it == format.end())
		{
			LOG(WARNING)<<"incomplete escape, interpret as non-escape text";
			result.insert(result.end(),_start,format.end());
			break;
		}
		CText subst;
		switch (*_it)
		{

		case 'd':
		case 'i':
		{
			switch (length_mod)
			{
			case _none:
            {
                HANDLE_NUM(int,10);
            }
				break;
			case _hh:
            {
                HANDLE_NUM(int,10);
            }
				break;
			case _h:
            {
                HANDLE_NUM(int,10);
            }
				break;
			case _l:
			case _j:
            {
                HANDLE_NUM(long int,10);
            }
				break;
			case _ll:
            {
                HANDLE_NUM(long long,10);
            }
				break;
			case _z:
            {
                HANDLE_NUM(size_t,10);
            }
				break;
			case _t:
            {
                HANDLE_NUM(int,10);
            }
				break;
			default:
				break;
			}
			break;
		}
		case 'o':
		case 'u':
		case 'x':
		case 'X':
		{
			int base = 10;
			switch (*_it)
			{
			case 'o':
				base = 8;
				break;
			case 'u':
				base = 10;
				break;
			case 'x':
			case 'X':
				base = 16;
				break;
			default:
				break;
			}

			switch (length_mod)
			{			
			case _none:
			case _hh:
			case _h:
			case _l:
            {
                HANDLE_NUM(unsigned ,base);
            }
				break;			
			case _ll:
            {
                HANDLE_NUM(unsigned long long,base);
            }
				break;
			case _z:
            {
                HANDLE_NUM(size_t,base);
            }
				break;
			default:
				break;
			}

			break;
		}
		case 'E':
		case 'e':
		case 'F':
		case 'f':
		case 'G':
		case 'g':
		case 'A':
		case 'a':
		{
			//FIXME fromating
			if (length_mod == _L)
            {
                HANDLE_FLOAT(long double,precision)
            }
			else
            {
                HANDLE_FLOAT(double,precision)
            }
			break;
		}
		case 'b':
		{
			bool i = va_arg(argptr, int) != 0;
			std::string _bool;
			bool _result = NSHARE::bool_to_str(i, _bool); //FIXME format
			LOG_IF(DFATAL,!_result) << "Cannot read  the field width in "
											<< CText(_start, _it)
											<< " interpret as non-escape text";
			(void) _result;
			subst += _bool;
			break;
		}
		case 'c':
		{
			if (length_mod == _l)
				subst += static_cast<utf32>(va_arg(argptr, int) );
			else
			{
				int _chat = va_arg(argptr, int);
				if (aType.MIsCodeValid(_chat))
					subst += static_cast<char>(va_arg(argptr, int) );
				else
				{
					LOG(DFATAL)<<"Invalid charaster in \""<<CText(_start, _it)
					<<"\" charester:"<<CText(1,static_cast<utf32>(_chat))<<"("<<_chat<<"). CodeConv:"<<aType;

					result.insert(result.end(),_start,format.end());
					break;
				}
			}
			break;
		}
		case 's':
		{
			if (length_mod == _l)
			{
				const int *buff = va_arg(argptr, const int*);
				CHECK_NOTNULL(buff);
				const utf32 *ch = reinterpret_cast<utf32 const*>(buff);
				for (;*ch != 0;++ch)
				subst.append(1,*ch);
			}
			else
			{
				subst.append(va_arg(argptr, const char*),aType);
			}
			if (precision != -1)
			subst.resize((size_t)precision);
			break;
		}
		case 'p':
		{
			void *arg = va_arg(argptr, void*);
			std::ptrdiff_t i=reinterpret_cast<std::ptrdiff_t>(arg);

			std::string _num;
			bool _result = NSHARE::num_to_str(i, _num, 16); //FIXME format
			LOG_IF(DFATAL,!_result) << "Cannot read  the field width in "
			<< CText(_start, _it)
			<< " interpret as non-escape text";
			(void)_result;
			subst+=_num;
			break;
		}
		case 'n':
		{
			switch (length_mod)
			{
				case _hh:
				{
					signed char *n = va_arg(argptr, signed char*);
					*n = static_cast<signed char>(result.length());
					break;
				}
				case _h:
				{
					short int *n = va_arg(argptr, short int*);
					*n = static_cast<short int>(result.length());
					break;
				}
				case _l:
				{
					long int *n = va_arg(argptr, long int*);
					*n = (long int)result.length();
					break;
				}
				case _ll:
				{
					long long *n = va_arg(argptr, long long*);
					*n = result.length();
					break;
				}
				default:
				{
					int *n = va_arg(argptr, int*);
					*n = (int)result.length();
					break;
				}
			}
			break;
		}
		default:
		{
			LOG(WARNING)<<"bad escape, treat as non-escape text";
			for (; _start != _it && _start!=format.end(); ++_start)
			result.append(1,*_it);
			continue;
		}
	}	; //switch
		if (_is_left)
			result.append(subst.MLeftJustified(width));
		else
			result.append(subst.MRightJustified(width));
	}
	return result;
}
CText CText::sMPrintf(ICodeConv const& aType, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);

	if (CCodeUTF8 const* _utf8 = dynamic_cast<CCodeUTF8 const*>(&aType))
	{
		(void) _utf8;
		CHECK(_utf8->MIsBufValid(format, format + strlen(format)));
	}
	CText _to = string_printf_v(aType, CText(format, aType), argptr);

	va_end(argptr);
	return _to;
}
CText& CText::MPrintf(const char* format, ...)
{

	MWillBeenChanged();

	va_list argptr;
	va_start(argptr, format);

	CCodeUTF8 const _utf8;
	CHECK(_utf8.MIsBufValid(format, format + strlen(format)));
	*this = string_printf_v(_utf8, CText(format, _utf8), argptr);

	va_end(argptr);
	return *this;
}

CText& CText::MLeftJustified(int width, utf32 fill, bool truncate)
{
	ssize_t padlen = (ssize_t)width - (ssize_t)length();
	if (padlen > 0)
		resize((size_t)width, fill);
	else if (truncate)
		erase(iterator(ptr()) + ::abs(padlen), end());
	return *this;
}
CText& CText::MRightJustified(int width, utf32 fill, bool truncate)
{
	ssize_t padlen = (ssize_t)width - (ssize_t)length();
	if (padlen > 0)
	{
		CText _fill_str(padlen, fill);
		insert(0, _fill_str);
	}
	else if (truncate)
		erase(iterator(ptr()) + ::abs(padlen), end());
	return *this;
}
int CText::MCompareNoCase(const CText& str) const
{
	return MCompareNoCase(0, size(), str);
}
static utf32 ucs4_tolower(utf32 c) //FIXME
{
	if (c == 0)
		return c;
	if (c >= 'A' && c <= 'Z')
		return c + 0x20;
	if (c >= 0x410 && c <= 0x42f)
		return c + 0x20;
	if (c == 0x401)
		return c + 0x50;
	if (((c >= (0x410 + 0x20)) && (c <= (0x42f + 0x20))) //
	|| (c == 0x401 + 0x50) //
			|| (/*c > 0 &&*/ c <= 127) //
			)
		return c;
	LOG(DFATAL)<<"Unknown \"tolower\" function for "<<CText(1,c) <<"("<<c<<")";
	return c;
}
static utf32 ucs4_toupper(utf32 c) //FIXME
{
	if (c == 0)
		return c;
	if (c >= 'a' && c <= 'z')
		return c - 0x20;
	if (c >= 0x430 && c <= 0x44f)
		return c - 0x20;
	if (c == 0x451)
		return 0x401;
	if (((c >= 0x410) && (c <= 0x42f)) //
	|| c == 0x401 || //
			(/*c > 0 &&*/ c <= 127))
		return c;
	LOG(DFATAL)<<"Unknown \"toupper\" function for "<<CText(1,c) <<"("<<c<<")";
	return c;
}
CText& CText::MToUpperCase()
{
	MWillBeenChanged();

	iterator _it = iterator(ptr());
	for (; _it != end(); ++_it)
		*_it = ucs4_toupper(*_it);
	return *this;
}
CText& CText::MToLowerCase()
{
	MWillBeenChanged();

	iterator _it = iterator(ptr());
	for (; _it != end(); ++_it)
		*_it = ucs4_tolower(*_it);
	return *this;
}
bool CText::MIsAscii() const
{
	CCodeANSII _anssi;
	const_iterator _it = begin();
	for (; _it != end(); ++_it)
		if (!_anssi.MIsCodeValid(*_it))
			return false;
	return true;
}
bool CText::sMIsMatch(const_iterator _pat_it, const_iterator _pat_end,
		const_iterator _it, const_iterator _it_end)
{
	bool _is_escape = false;
	for (; _pat_it != _pat_end;)
	{
		if (!_is_escape)
			switch (*_pat_it)
			{
			case '\\':
			{
				_is_escape = true;
				++_pat_it;
				continue;
			}
				break;
			case '?':
			{
				if (_it == _it_end)
					return false;
				++_it;
				++_pat_it;
				continue;
			}
				break;
			case '*':
			{
				if (++_pat_it == _pat_end)
					return true;
				for (; _it != _it_end; ++_it)
					if (sMIsMatch(_pat_it, _pat_end, _it, _it_end))
						return true;

				return false;
			}
				break;

			default:
				break;
			}

		if (*_pat_it != *_it)
			return false;

		_is_escape = false;
		++_it;
		++_pat_it;
	}

	return (_it == _it_end) ? true : false;
}

bool CText::MIsMatch(CText const& aStr) const
{
	CHECK(!aStr.empty());
	return sMIsMatch(aStr.begin(), aStr.end(), begin(), end());
}
unsigned int CText::MGetHash() const
{
	unsigned int _hash = 0;
	static const unsigned int _b = 378551;
	unsigned int _a = 63689;

	const_iterator _it = begin();
	for (; _it != end(); ++_it)
	{
		_hash = _hash * _a + *_it;
		_a *= _b;
	}
	return _hash;
}
std::string CText::MToStdString(ICodeConv const& aType) const //FIXME using MBuildUtf8Buff
{
	std::string _result;

	const_iterator _it = begin();
	utf8 _buf[4];
	for (; _it != end(); ++_it)
	{
		utf8* _last = _buf;
		if (!aType.MAppend(*_it, _last, _buf + sizeof(_buf)))
			return "";
		else
		{
			utf8* _begin = _buf;
			for (; _begin != _last; ++_begin)
				_result.push_back(*_begin);
		}
	}
	return _result;
}
int CText::MCompareNoCase(size_type aFirst, size_type aLen, const CText& aStr,
		size_type aStrFirst, size_type aStrLen) const
{
	if (empty() && !aFirst && aStr.empty())
		return 0;

	CHECK_GT(size() , aFirst) << "Index was out of range for CText object";

	CHECK_GT(aStr.size() , aStrFirst)
			<< "Index was out of range for CText object. StrLen=" << aFirst
			<< ", Code point length=" << size();

	if ((aStrLen == npos) || (aStrFirst + aStrLen > aStr.size()))
		aStrLen = aStr.size() - aStrFirst;

	if (aLen)
	{
		size_type cp_count = std::min(aLen, aStrLen);
		const utf32* _src = ptr_const() + aFirst;
		const utf32* _rht = aStr.ptr_const() + aStrFirst;

		for (; cp_count--; _rht++, _src++)
			if (ucs4_tolower(*_src) != ucs4_tolower(*_rht))
				return (*_src < *_rht) ? -1 : 1;
	}
	if (aLen < aStrLen)
		return -1;
	else if (aLen == aStrLen)
		return 0;
	else
		return 1;
}
CText& CText::MReplaceAll(CText const& aSubStr, CText const& aReplaceTo,
		size_type idx, size_type len)
{
	if (empty())
		return *this;

	if (aSubStr.empty() && aReplaceTo.empty())
	{
		VLOG(2) << "Replace range of characters is 0.";
		return *this;
	}
	LOG_IF(DFATAL,aSubStr.empty()) << "Cannot replace empty string";

	if (aSubStr.empty())
		return *this;

	VLOG(2) << "Replace all " << aSubStr << " to " << aReplaceTo;

	CHECK_GT (size() , idx) << "Index is out of range for CText";
	size_t _pos = 0;
	//if the aReplace is null
	//and the aSubStr is at the end of the Text
	//then _pos==length()
	//if There is not aSubStr in Text, _pos == npos
	for (; (_pos != length()) && ((_pos = find(aSubStr, _pos)) != npos);)
		replace(_pos, aSubStr.length(), aReplaceTo);

	return *this;
}
CText& CText::MMakeRandom(size_t aLen, ICodeConv const& aType)
{
	static char const _alphanum[] = "0123456789"
			"QWERTYUIOPASDFGHJKLZXCVBNM"
			"qwertyuiopasdfghjklzxcvbnm";
	//TODO utf8
	static size_t const _alphanum_size = sizeof(_alphanum) - 1;
	if (!aLen)
	{
		VLOG(1) << "The Random string size is null.";
		clear();
		return *this;
	}

	MWillBeenChanged();

	VLOG_IF(2,!empty()) << "Replace " << *this << " by random string";

	MGrow(aLen);
	MSetLen(aLen);

	generate_seed_if_need();
	uint64_t _val = get_uuid().FVal;
	for (unsigned i = 0; aLen != 0;)	//aLen всегда >0
	{
		uint8_t _pos=(_val>>(i*8))&0xff;
		ptr()[--aLen] = _alphanum[_pos % _alphanum_size];
		if(++i==sizeof(_val))
		{
			_val = rand();
			i=0;
		}
	}

	return *this;
}
bool CText::isalpha() const
{
	for (size_type _len = 0; _len < size(); ++_len)
		if (!std::isalpha<wchar_t>(ptr_const()[_len], utf8_locale()))
			return false;
	return true;
}
bool CText::isdigit() const
{
	for (size_type _len = 0; _len < size(); ++_len)
		if (!std::isdigit<wchar_t>(ptr_const()[_len], utf8_locale()))
			return false;
	return true;
}
bool CText::isalnum() const
{
	for (size_type _len = 0; _len < size(); ++_len)
		if (!std::isalnum<wchar_t>(ptr_const()[_len], utf8_locale()))
			return false;
	return true;
}
void CText::MWillBeenChanged()
{
	impl_t & _impl = FImpl.MWrite();
	if (_impl.FSingleByteBuffer)
	{
		_impl.FSingleByteDatalen = 0;
	}
}
bool CText::sMUnitTest()
{
	using namespace NSHARE;
	{
		NSHARE::CText a;
		CHECK_EQ(a, "");
		CHECK_EQ(a.length(), 0);
		a = "abc";
		CHECK_EQ(a, "abc");
		CHECK_EQ(a.length(), 3);
		CHECK_EQ(a.length_code(), 3);
	}
	{
		NSHARE::CText a("ABC");
		NSHARE::CText b("BCD");

//		CHECK_EQ(a, a);
//		CHECK_EQ(b, b);

		CHECK_NE(a, b);
	}
	{
		CText a, b, c;

		a.reserve(128);
		b.reserve(128);
		c.reserve(128);

		for (int i = 0; i < 2; ++i)
		{
			a = CText("Hello");
			b = CText(" world");
			c = CText("! How'ya doin'?");
			a += b;
			a += c;
			c = CText("Hello world! What's up?");
			CHECK_NE(c, a);
		}
	}
	{
		CText s("Hello, world!");
		CText _s2("Hello, world_");

		CHECK_EQ(!s.compare(_s2), false);

		CHECK_EQ(!s.compare(0, strlen("Hello"), "Hello"), true);
		CHECK_EQ(!s.compare(0, strlen("Hello, "), "Hello, "), true);
		CHECK_EQ(!s.compare(0, strlen("Hello, world!!!"), "Hello, world!!!"),
				false);
		std::string _str("Hello, world!");
		(void) _str;
		CHECK_EQ(s.compare(0, strlen(""), ""), _str.compare(0, strlen(""), ""));
		CHECK_EQ((s == ""), (_str == ""));
		CHECK_EQ((s != ""), (_str != ""));
		CHECK_EQ(!s.compare(0, strlen("Goodbye"), "Goodbye"), false);
	}
	{
		{
			CText _trim("  Test  ");
			_trim.MTrimSpace();
			CHECK_EQ(_trim.length(), 6);
			CHECK_EQ(_trim, CText("  Test"));
		}
		{
			CText _trim("        ");
			_trim.MTrimSpace();
			CHECK_EQ(_trim.length(), 0);
			CHECK_EQ(_trim, CText(""));
		}
		{
			CText _trim("    Test");
			_trim.MTrimSpace();
			CHECK_EQ(_trim, CText("    Test"));
		}
	}
	{
#define TEST_FIND( str , start , result )  \
		CHECK_EQ( CText(str).find("ell", start), result );

		TEST_FIND("Well, hello world", 0, 1);
		TEST_FIND("Well, hello world", 6, 7);
		TEST_FIND("Well, hello world", 9, CText::npos);
#undef TEST_FIND
	}
	{
#define TEST_RFIND( str , start , result )  \
   CHECK_EQ( CText(str).rfind("ell", start), result );

		TEST_RFIND("Well, hello world", 16, 7);
		TEST_RFIND("Well, hello world", 6, 1);
		TEST_RFIND("Well, hello world", 17, CText::npos);
#undef TEST_RFIND
	}
	{
#define TEST_FIND_FIRST( str , start , result )  \
   CHECK_EQ( CText(str).find_first_of("ell", start), result );

		TEST_FIND_FIRST("Well, hello world", 0, 1);
		TEST_FIND_FIRST("Well, hello world", 6, 7);
		TEST_FIND_FIRST("Well, hello world", 9, 9);
#undef TEST_FIND_FIRST
	}
	{
#define TEST_FIND_NFIRST( str , start , result )  \
   CHECK_EQ( CText(str).find_first_not_of("ell", start), result );

		TEST_FIND_NFIRST("Well, hello world", 1, 4);
		TEST_FIND_NFIRST("Well, hello world", 0, 0);
		TEST_FIND_NFIRST("Well, hello world", 4, 4);
#undef TEST_FIND_FIRST
	}
	{
#define TEST_FIND_LAST( str , start , result )  \
   CHECK_EQ( CText(str).find_last_of("ell", start), result );

		TEST_FIND_LAST("Well, hello world", 17, 15);
		TEST_FIND_LAST("Well, hello world", 14, 9);
		TEST_FIND_LAST("Well, hello world", 0, CText::npos);
#undef TEST_FIND_LAST
	}
	{
#define TEST_FIND_NLAST( str , start , result )  \
   CHECK_EQ( CText(str).find_last_not_of("ell", start), result );

		TEST_FIND_NLAST("Well, hello world", 17, 16);
		TEST_FIND_NLAST("Well, hello world", 9, 6);
		TEST_FIND_NLAST("Well, hello world", 0, 0);
#undef TEST_FIND_NLAST
	}
	{
#define TEST_REPLACE( original , pos , len , replacement , result ) \
		{ \
		CText s ( original); \
        	s.replace( pos , len , replacement ); \
        	MASSERT( result, s ); \
		}

		TEST_REPLACE("012-AWORD-XYZ", 4, 5, "BWORD", "012-BWORD-XYZ");
		TEST_REPLACE("increase", 0, 2, "de", "decrease");
		TEST_REPLACE("wxWindow", 8, 0, "s", "wxWindows");
		TEST_REPLACE("foobar", 3, 0, "-", "foo-bar");
		TEST_REPLACE("barfoo", 0, 6, "foobar", "foobar");
#undef TEST_REPLACE
	}
	{
		CText s1("AHH");
		CText eq("AHH");
		CText neq1("HAH");
		CText neq2("AH");
		CText neq3("AHHH");
		CText neq4("AhH");

		CHECK_EQ(s1, eq);
		CHECK_NE(s1, neq1);
		CHECK_NE(s1, neq2);
		CHECK_NE(s1, neq3);
		CHECK_NE(s1, neq4);

		CHECK_EQ(s1, "AHH");
		CHECK_NE(s1, "no");
		CHECK_LT(s1, "AZ");
		CHECK_LE(s1, "AZ");
		CHECK_LE(s1, "AHH");
		CHECK_GT(s1, "AA");
		CHECK_GE(s1, "AA");
		CHECK_GE(s1, "AHH");

		s1.insert(1, 1, '\0');
		eq.insert(1, 1, '\0');
		neq1.insert(1, 1, '\0');
		neq2.insert(1, 1, '\0');
		neq3.insert(1, 1, '\0');
		neq4.insert(1, 1, '\0');

		CHECK_EQ(s1, eq);
		CHECK_NE(s1, neq1);
		CHECK_NE(s1, neq2);
		CHECK_NE(s1, neq3);
		CHECK_NE(s1, neq4);

		CHECK_LT(CText("\n").compare(" "), 0);
		CHECK_GT(CText("'").compare("!"), 0);
		CHECK_LT(CText("!").compare("z"), 0);

		CText s1_copy = s1 + s1;
		s1.reserve(s1.size()*2);
		s1.insert(s1.end(), s1.begin(), s1.end()); //copy
		CHECK_EQ(s1, s1_copy);
	}

	{
		CText s("hello");
		CHECK_EQ(s[0], 'h');
		CHECK_EQ(s[2], 'l');

		CHECK_EQ(*s.c_str(), 'h');
		CHECK_EQ(*(s.c_str() + 2), 'l');
	}
	{
		char _utf8_str[] = "WъЧS";
		CText s(_utf8_str);
		CHECK_EQ(s[0], 'W');
		CHECK_EQ(s[3], 'S');

		CHECK_EQ(s, _utf8_str);
		CHECK_EQ(s.length(), 4);
		// the string "Déjà" in UTF-8
		const unsigned char utf8Buf[] =
		{ 0x44, 0xC3, 0xA9, 0x6A, 0xC3, 0xA0, 0 };
		const unsigned char utf8subBuf[] =
		{ 0x44, 0xC3, 0xA9, 0x6A, 0 }; // just "Déj"
		const char *utf8 = (char *) utf8Buf;
		const char *utf8sub = (char *) utf8subBuf;

		CText _s(utf8);
		CHECK_EQ(_s, utf8);

		CText _sub(utf8sub);
		CText _sub2(utf8, 3);
		CHECK_EQ(_sub, _sub2);
	}
	{
		NSHARE::CText a("AЭC");
		NSHARE::CText b("ЭCD");

//		CHECK_EQ(a, a);
//		CHECK_EQ(b, b);

		CHECK_NE(a, b);
	}
	{
		CText s1("АHH");
		CText eq("АHH");
		CText neq1("HАH");
		CText neq2("АH");
		CText neq3("АHHH");
		CText neq4("АhH");

		CHECK_EQ(s1, eq);
		CHECK_NE(s1, neq1);
		CHECK_NE(s1, neq2);
		CHECK_NE(s1, neq3);
		CHECK_NE(s1, neq4);

		CHECK_EQ(s1, "АHH");
		CHECK_NE(s1, "no");
		CHECK_LT(s1, "АZ");
		CHECK_LE(s1, "АZ");
		CHECK_LE(s1, "АHH");
		CHECK_GT(s1, "АA");
		CHECK_GE(s1, "АA");
		CHECK_GE(s1, "АHH");

		s1.insert(1, 1, '\0');
		eq.insert(1, 1, '\0');
		neq1.insert(1, 1, '\0');
		neq2.insert(1, 1, '\0');
		neq3.insert(1, 1, '\0');
		neq4.insert(1, 1, '\0');

		CHECK_EQ(s1, eq);
		CHECK_NE(s1, neq1);
		CHECK_NE(s1, neq2);
		CHECK_NE(s1, neq3);
		CHECK_NE(s1, neq4);

		CHECK_LT(CText("\n").compare(" "), 0);
		CHECK_GT(CText("'").compare("!"), 0);
		CHECK_LT(CText("!").compare("z"), 0);
	}
	{
#define TEST_FIND( str , start , result )  \
		CHECK_EQ( CText(str).find("ет", start), result );

		TEST_FIND("Шахуеть, привет мир", 0, 4);
		TEST_FIND("Шахуеть, привет мир", 5, 13);
		TEST_FIND("Шахуеть, привет мир", 14, CText::npos);
#undef TEST_FIND
	}
	//Format
	{
		CText s1, s2;
		s1.MPrintf("%d", 18);
		CHECK_EQ(s1, "18");

		s2.MPrintf("Number 18: %s", s1.c_str());
		CHECK_EQ(s2, "Number 18: 18");
	}
    //Format stream
    {
        CText s1, s2;
        s1<<18;
        CHECK_EQ(s1, "18");

        s2<<"Number "<<18<<": "<<s1;
        CHECK_EQ(s2, "Number 18: 18");
    }
    {
        CText _text,_text2;
        unsigned const _this[] = {0xFE,0xAA,0x1,0x20,0x10,0xFA,0xF1,0x1F};
        _text2.MPrintf("%02x-%02x-"
                      "%02x-%02x-"
                      "%02x-%02x-"
                      "%02x-%02x", _this[0], _this[1], _this[2], _this[3], _this[4],
            _this[5], _this[6], _this[7]);
        _text2.MToLowerCase();
        _text<<std::showbase<< std::setfill('0')<< std::setw(2)<<std::hex ;
        unsigned _count_array=sizeof(_this)/sizeof(_this[0]);
        for (unsigned i=0; i<_count_array; i++)
        {
            _text<<(unsigned)_this[i];
            if(i!=(_count_array-1))
                _text<<'-';
        }
        CHECK_EQ(_text, _text2)<<"Printf:"<<_text2<<" stream:"<<_text;
    }
	{
		static const size_t lengths[] =
		{ 1, 512, 1024, 1025, 2048, 4096, 4097 };
		for (size_t n = 0; n < ((sizeof(lengths) / sizeof(lengths[0]))); n++)
		{
			const size_t len = lengths[n];

			CText s(len, 'Z');
			CHECK_EQ(len,
					CText::sMPrintf(CCodeUTF8(), "%s", s.c_str()).length());
		}
	}
	{
		CText s1, s2;
		s1.MPrintf("%d", 18);
		CHECK_EQ(s1, "18");

		s2.MPrintf("Номер 18: %s", s1.c_str());
		CHECK_EQ(s2, "Номер 18: 18");

		CText s3;
		s3.MPrintf("Для паши %s", s2.c_str());

		CHECK_EQ(s3, "Для паши Номер 18: 18");
	}
	{
		CText s1, s2;
		s1<<18;
		CHECK_EQ(s1, "18");

		s2<<"Номер "<<18<<": "<<s1;
		CHECK_EQ(s2, "Номер 18: 18");

		CText s3;
		s3<<"Для паши "<<s2;

		CHECK_EQ(s3, "Для паши Номер 18: 18");
	}
	{
		CText s1("UpperLower");
		s1.MToLowerCase();
		CHECK_EQ(s1, "upperlower");
	}
	{
		CText s1("ВерхнийРегистр");
		s1.MToLowerCase();
		CHECK_EQ(s1, "верхнийрегистр");
	}
	{
		CText s1("ВерхнийРегистр");
		CText s2("верхнийрегистр");
		CHECK_EQ(s1.MCompareNoCase(s2), 0);
	}
	{
		CText s1("ВeрхнийРегистр");
		s1.MToUpperCase();
		CHECK_EQ(s1, "ВEРХНИЙРЕГИСТР");
	}
	{
#define TEST_MATCH( s1 , s2 , result ) \
		CHECK_EQ( CText(s1).MIsMatch(CText(s2)) , result )

		TEST_MATCH("русский", "рус*", true);
		TEST_MATCH("русский", "*сс*", true);
		TEST_MATCH("русский", "*кий", true);
		TEST_MATCH("русский", "???????", true);
		TEST_MATCH("русский", "ру??к*", true);
		TEST_MATCH("русский", "ру?к*", false);
		TEST_MATCH("русский", "*гск*", false);
		TEST_MATCH("русский", "*рус", false);
		TEST_MATCH("русскийрус", "*рус", true);
		TEST_MATCH("", "*", true);
		TEST_MATCH("", "?", false);
		TEST_MATCH("*", "\\*", true);

#undef TEST_MATCH
	}
	{
		CText s1("ВeрхнийРегистр");
		CText s2("ВeрхнийРегистр");
		CHECK_EQ(s1.MGetHash(), s2.MGetHash());
	}
	{
		CText _s("Hello world!!!");
		_s.MReplaceAll("l", "ж");
		CHECK_EQ(_s, CText("Heжжo worжd!!!"));
		_s.MReplaceAll("!", "г");
		CHECK_EQ(_s, CText("Heжжo worжdггг"));
	}
	{
		CText s1("Тестовая");
		CHECK(NSHARE::num_to_str(123456789, s1));
		CHECK_EQ(s1, "Тестовая123456789");
		CHECK(NSHARE::num_to_str(0, s1));
		CHECK_EQ(s1, "Тестовая1234567890");
		CHECK(NSHARE::num_to_str(-1, s1));
		CHECK_EQ(s1, "Тестовая1234567890-1");
	}
	{
		CText s1("ТолькоСимволы");
		CHECK(s1.isalpha());
		s1 += "1024245";
		CHECK(s1.isalnum());
		(void) s1;
		s1 = CText("1245452");
		CHECK(s1.isdigit());
	}
	return true;
}
static uint8_t g_buffer[sizeof(CText)+__alignof(CText)];
CText const& CText::sMEmpty()
{
	static CText& g_empty=
			*new (get_alignment_address<CText >(g_buffer)) CText;///< allocate to static memory
	return g_empty;
}
} // NSHARE

