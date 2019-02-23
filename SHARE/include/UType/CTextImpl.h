/*
 * CTextImpl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CTEXTIMPL_H_
#define CTEXTIMPL_H_

namespace NSHARE
{

inline CText::size_type const& CText::size() const
{
	return FCodePointLength;
}
inline CText::size_type const& CText::length() const
{
	return size();
}
inline bool CText::empty(void) const
{
	return !size();
}
inline CText::size_type CText::max_size() const
{
	return S_MAX_SIZE;
}
inline CText::size_type CText::capacity() const
{
	using  namespace std;
	CText::size_type const _length=MGetBufferLength();

	assert(_length && "Wtf? Invalid buffer length!");

	return  _length- 1;
}
inline void CText::reserve(size_type num)
{
	MWillBeenChanged();
	if (num == 0)
		MRemoveBuffer();
	else
		MGrow(num);
}

inline CText::reference CText::operator[](size_type idx)
{
	MWillBeenChanged();
	return (ptr()[idx]);
}
inline CText::const_reference CText::operator[](size_type idx) const
{
	return ptr()[idx];
}

inline utf8 * CText::data_unsafety(void)
{
	return MBuildUtf8Buff();
}
inline utf8 const* CText::data(void) const
{
	return MBuildUtf8Buff();
}

inline utf8 const* CText::c_str() const
{
	return (utf8 const*) MBuildUtf8Buff();
}
inline const utf32* CText::ptr(void) const
{
	return ptr_const();
}
inline void CText::push_back(utf32 code_point)
{
	append(1, code_point);
}

inline CText& CText::assign(utf8 const* cstr, ICodeConv const& aType)
{
	return assign(cstr,  aType.MLengthOf(cstr), aType);
}
inline CText& CText::operator+=(const CText& str)
{
	return append(str);
}

inline CText& CText::operator+=(const std::string& std_str)
{
	return append(std_str);
}
inline CText& CText::operator+=(utf8 const* utf8_str)
{
	return append(utf8_str);
}
inline CText& CText::operator+=(utf32 code_point)
{
	return append(1, code_point);
}

inline CText& CText::append(utf8 const* utf8_str, ICodeConv const& aType)
{
	return append(utf8_str, aType.MLengthOf(utf8_str), aType);
}
inline CText& CText::append(const_iterator iter_beg, const_iterator iter_end)
{
	return replace(end(), end(), iter_beg, iter_end);
}
inline CText& CText::insert(size_type idx, const CText& str)
{
	return insert(idx, str, 0, npos);
}
inline CText& CText::insert(size_type idx, const std::string& std_str)
{
	return insert(idx, std_str, 0, npos);
}
inline CText& CText::insert(size_type idx, utf8 const* utf8_str,
		ICodeConv const& aType)
{
	return insert(idx, utf8_str, aType.MLengthOf(utf8_str), aType);
}
inline void CText::insert(iterator pos, size_type num, utf32 code_point)
{
	insert(MSafeIterDif(pos, iterator(ptr())), num, code_point);
}
inline CText::iterator CText::insert(iterator pos, utf32 code_point)
{
	insert(pos, 1, code_point);
	return pos;
}
inline void CText::insert(iterator iter_pos, const_iterator iter_beg,
		const_iterator iter_end)
{
	replace(iter_pos, iter_pos, iter_beg, iter_end);
}
inline CText& CText::erase()
{
	clear();
	return *this;
}
inline CText& CText::erase(size_type idx)
{
	return erase(idx, 1);
}
inline CText& CText::erase(iterator pos)
{
	return erase(MSafeIterDif(pos, iterator(ptr())), 1);
}
inline CText& CText::erase(iterator iter_beg, iterator iter_end)
{
	return erase(MSafeIterDif(iter_beg, iterator(ptr())),
			MSafeIterDif(iter_end, iter_beg));
}
inline void CText::resize(size_type num)
{
	resize(num, utf32());
}
inline CText& CText::replace(size_type idx, size_type len, const CText& str)
{
	return replace(idx, len, str, 0, npos);
}
inline CText& CText::replace(iterator iter_beg, iterator iter_end,
		const CText& str)
{
	return replace(MSafeIterDif(iter_beg, iterator(ptr())),
			MSafeIterDif(iter_end, iter_beg), str, 0, npos);
}
inline CText& CText::replace(size_type idx, size_type len,
		const std::string& std_str)
{
	return replace(idx, len, std_str, 0, npos);
}
inline CText& CText::replace(iterator iter_beg, iterator iter_end,
		const std::string& std_str)
{
	return replace(MSafeIterDif(iter_beg, iterator(ptr())),
			MSafeIterDif(iter_end, iter_beg), std_str, 0, npos);
}
inline CText& CText::replace(iterator iter_beg, iterator iter_end,
		size_type num, utf32 code_point)
{
	return replace(MSafeIterDif(iter_beg, iterator(ptr())),
			MSafeIterDif(iter_end, iter_beg), num, code_point);
}
inline CText& CText::replace(size_type idx, size_type len, utf8 const* cstr,
		ICodeConv const& aType)
{
	return replace(idx, len, cstr, aType.MLengthOf(cstr), aType);
}

inline CText& CText::replace(iterator iter_beg, iterator iter_end, utf8 const* cstr,
		ICodeConv const& aType)
{
	return replace(iter_beg, iter_end, cstr, aType.MLengthOf(cstr), aType);
}
inline CText& CText::replace(iterator iter_beg, iterator iter_end, utf8 const* chars,
		size_type chars_len, ICodeConv const& aType)
{
	return replace(MSafeIterDif(iter_beg, iterator(ptr())),
			MSafeIterDif(iter_end, iter_beg), chars, chars_len, aType);
}
inline CText::size_type CText::find(utf8 const* cstr, size_type idx,
		ICodeConv const& aType) const
{
	return find(cstr, idx, aType.MLengthOf(cstr), aType);
}
inline CText::size_type CText::rfind(utf8 const* cstr, size_type idx,
		ICodeConv const& aType) const
{
	return rfind(cstr, idx, aType.MLengthOf(cstr), aType);
}
inline CText::size_type CText::find_first_of(utf32 code_point,
		size_type idx) const
{
	return find(code_point, idx);
}
inline CText::size_type CText::find_first_of(utf8 const* cstr, size_type idx,
		ICodeConv const& aType) const
{
	return find_first_of(cstr, idx, aType.MLengthOf(cstr), aType);
}
inline CText::size_type CText::find_first_not_of(utf8 const* cstr, size_type idx,
		ICodeConv const& aType) const
{
	return find_first_not_of(cstr, idx, aType.MLengthOf(cstr), aType);
}

inline CText::size_type CText::find_last_of(utf8 const* cstr, size_type idx,
		ICodeConv const& aType) const
{
	return find_last_of(cstr, idx, aType.MLengthOf(cstr), aType);
}
inline CText::size_type CText::find_last_not_of(utf8 const* cstr, size_type idx,
		ICodeConv const& aType) const
{
	return find_last_not_of(cstr, idx, aType.MLengthOf(cstr), aType);
}
inline CText::iterator CText::begin(void)
{
	MWillBeenChanged();
	return iterator(ptr());
}
inline CText::const_iterator CText::begin(void) const
{
	return const_iterator(ptr());
}

inline CText::iterator CText::end(void)
{
	return iterator(&ptr()[FCodePointLength]);
}

inline CText::const_iterator CText::end(void) const
{
	return const_iterator(&ptr()[FCodePointLength]);
}
inline CText::reverse_iterator CText::rbegin(void)
{
	return reverse_iterator(end());
}

inline CText::const_reverse_iterator CText::rbegin(void) const
{
	return const_reverse_iterator(end());
}

inline CText::reverse_iterator CText::rend(void)
{
	return reverse_iterator(begin());
}

inline CText::const_reverse_iterator CText::rend(void) const
{
	return const_reverse_iterator(begin());
}
//////////////////////////////////////////////////////////////////////////
// Comparison operators
//////////////////////////////////////////////////////////////////////////
inline bool operator==(const CText& str1, const CText& str2)
{
	return (str1.compare(str2) == 0);
}

inline bool operator==(const CText& str, const std::string& std_str)
{
	return (str.compare(std_str) == 0);
}

inline bool operator==(const std::string& std_str, const CText& str)
{
	return (str.compare(std_str) == 0);
}

#ifdef CEGUI_STRING_CLASS
inline bool operator==(const CEGUI::String& e_str, const CText& str)
{
	if(str.length()!=e_str.length())
		return false;
	CEGUI::String::const_iterator _it_e=e_str.begin();
	CText::const_iterator _it_c=str.begin();
	for(;_it_e!=e_str.end();++_it_e,++_it_c )
		if(*_it_e!=*_it_c)
			return false;
	return true;
}
inline bool operator==(const CText& std_str, const CEGUI::String& str)
{
	return operator==(str,std_str);
}
#endif

inline bool operator!=(const CText& str1, const CText& str2)
{
	return (str1.compare(str2) != 0);
}

inline bool operator!=(const CText& str, const std::string& std_str)
{
	return (str.compare(std_str) != 0);
}

inline bool operator!=(const std::string& std_str, const CText& str)
{
	return (str.compare(std_str) != 0);
}

inline bool operator<(const CText& str1, const CText& str2)
{
	return (str1.compare(str2) < 0);
}

inline bool operator<(const CText& str, const std::string& std_str)
{
	return (str.compare(std_str) < 0);
}

inline bool operator<(const std::string& std_str, const CText& str)
{
	return (str.compare(std_str) > 0);
}

inline bool operator>(const CText& str1, const CText& str2)
{
	return (str1.compare(str2) > 0);
}

inline bool operator>(const CText& str, const std::string& std_str)
{
	return (str.compare(std_str) > 0);
}

inline bool operator>(const std::string& std_str, const CText& str)
{
	return (str.compare(std_str) < 0);
}

inline bool operator<=(const CText& str1, const CText& str2)
{
	return (str1.compare(str2) <= 0);
}

inline bool operator<=(const CText& str, const std::string& std_str)
{
	return (str.compare(std_str) <= 0);
}

inline bool operator<=(const std::string& std_str, const CText& str)
{
	return (str.compare(std_str) >= 0);
}

inline bool operator>=(const CText& str1, const CText& str2)
{
	return (str1.compare(str2) >= 0);
}

inline bool operator>=(const CText& str, const std::string& std_str)
{
	return (str.compare(std_str) >= 0);
}

inline bool operator>=(const std::string& std_str, const CText& str)
{
	return (str.compare(std_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// c-string operators
//////////////////////////////////////////////////////////////////////////
inline bool operator==(const CText& str, utf8 const* c_str)
{
	return (str.compare(c_str) == 0);
}

inline bool operator==(utf8 const* c_str, const CText& str)
{
	return (str.compare(c_str) == 0);
}

inline bool operator!=(const CText& str, utf8 const* c_str)
{
	return (str.compare(c_str) != 0);
}

inline bool operator!=(utf8 const* c_str, const CText& str)
{
	return (str.compare(c_str) != 0);
}

inline bool operator<(const CText& str, utf8 const* c_str)
{
	return (str.compare(c_str) < 0);
}

inline bool operator<(utf8 const* c_str, const CText& str)
{
	return (str.compare(c_str) > 0);
}

inline bool operator>(const CText& str, utf8 const* c_str)
{
	return (str.compare(c_str) > 0);
}

inline bool operator>(utf8 const* c_str, const CText& str)
{
	return (str.compare(c_str) < 0);
}

inline bool operator<=(const CText& str, utf8 const* c_str)
{
	return (str.compare(c_str) <= 0);
}

inline bool operator<=(utf8 const* c_str, const CText& str)
{
	return (str.compare(c_str) >= 0);
}

inline bool operator>=(const CText& str, utf8 const* c_str)
{
	return (str.compare(c_str) >= 0);
}

inline bool operator>=(utf8 const* c_str, const CText& str)
{
	return (str.compare(c_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// Concatenation operator functions
//////////////////////////////////////////////////////////////////////////
inline CText operator+(const CText& str1, const CText& str2)
{
	CText temp(str1);
	temp.append(str2);
	return temp;
}

inline CText operator+(const CText& str, const std::string& std_str)
{
	CText temp(str);
	temp.append(std_str);
	return temp;
}

inline CText operator+(const std::string& std_str, const CText& str)
{
	CText temp(std_str);
	temp.append(str);
	return temp;
}

inline CText operator+(const CText& str, utf8 const* utf8_str)
{
	CText temp(str);
	temp.append(utf8_str);
	return temp;
}

inline CText operator+(utf8 const* utf8_str, const CText& str)
{
	CText temp(utf8_str);
	temp.append(str);
	return temp;
}

inline CText operator+(const CText& str, utf32 code_point)
{
	CText temp(str);
	temp.append(1, code_point);
	return temp;
}

inline CText operator+(utf32 code_point, const CText& str)
{
	CText temp(1, code_point);
	temp.append(str);
	return temp;
}
inline NSHARE::CBuffer& operator<<(NSHARE::CBuffer& s, const NSHARE::CText& str)
{
	return str.MToBuf(s);
}

}
namespace std
{
inline void swap(NSHARE::CText& str1, NSHARE::CText& str2)
{
	str1.swap(str2);
}
inline std::ostream& operator<<(std::ostream& s, const NSHARE::CText& str)
{
	return str.MPrint(s);
}
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::Strings& aSrts)
{
	for (NSHARE::Strings::const_iterator _it = aSrts.begin();
			_it != aSrts.end(); ++_it)
		aStream << *_it << std::endl;
	return aStream;
}
template<class _Elem, class _Traits>
inline basic_istream<_Elem, _Traits>& operator>>(
		basic_istream<_Elem, _Traits>& aStream, NSHARE::CText& _Str)
{	// extract a string
	typedef ctype<_Elem> _Ctype;
	typedef basic_istream<_Elem, _Traits> _Myis;
	typedef NSHARE::CText::size_type _Mysizt;

	ios_base::iostate _State = ios_base::goodbit;
	bool _Changed = false;
	typename _Myis::sentry _Ok(aStream);

	if (_Ok)
	{
		// state okay, extract characters
		const _Ctype& _Ctype_fac = use_facet<_Ctype>(aStream.getloc());
		_Str.erase();

		try
		{
			_Mysizt _Size =
					0 < aStream.width()
							&& (_Mysizt) aStream.width() < _Str.max_size() ?
							(_Mysizt) aStream.width() : _Str.max_size();
			typename _Traits::int_type _Meta = aStream.rdbuf()->sgetc();

			for (; 0 < _Size; --_Size, _Meta = aStream.rdbuf()->snextc())
				if (_Traits::eq_int_type(_Traits::eof(), _Meta))
				{	// end of file, quit
					_State |= ios_base::eofbit;
					break;
				}
				else if (_Ctype_fac.is(_Ctype::space,
						_Traits::to_char_type(_Meta)))
					break;	// whitespace, quit
				else
				{	// add character to string
					_Str.append(1, _Traits::to_char_type(_Meta));
					_Changed = true;
				}
		} catch (...)
		{
			aStream.setstate(ios_base::badbit);
		}
	}

	aStream.width(0);
	if (!_Changed)
		_State |= ios_base::failbit;
	aStream.setstate(_State);
	return (aStream);
}
template<>
struct less<NSHARE::CText>: public binary_function<NSHARE::CText, NSHARE::CText, bool>
{
	bool operator()(const NSHARE::CText& __x, const NSHARE::CText& __y) const
	{
		NSHARE::CStringFastLessCompare _comp;
		return _comp(__x,__y);
	}
};
template<>
struct greater<NSHARE::CText>: public binary_function<NSHARE::CText, NSHARE::CText, bool>
{
	bool operator()(const NSHARE::CText& __x, const NSHARE::CText& __y) const
	{
		NSHARE::CStringFastLessCompare _comp;
		return _comp(__y,__x);
	}
};
template<>
struct equal_to<NSHARE::CText>: public binary_function<NSHARE::CText, NSHARE::CText, bool>
{
	bool operator()(const NSHARE::CText& a, const NSHARE::CText& b) const
	{
		const size_t la = a.length();
		const size_t lb = b.length();
		if (la == lb)
		{
			if (la)
				return (memcmp(a.ptr(), b.ptr(), la * sizeof(NSHARE::utf32)) == 0);
			else
				return true;
		}
		return false;
	}
};

}

#endif /* CTEXTIMPL_H_ */
