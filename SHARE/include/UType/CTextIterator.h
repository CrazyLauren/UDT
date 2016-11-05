/*
 * CTextImpl.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.02.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CTEXTITERATOR_H_
#define CTEXTITERATOR_H_

namespace NSHARE
{
class  CText::iterator: public std::iterator<std::random_access_iterator_tag, utf32>
{
public:
	iterator() :
			d_ptr(0)
	{
	}
	explicit iterator(utf32* const ptr) :
			d_ptr(ptr)
	{
	}

	utf32& operator*() const
	{
		return *d_ptr;
	}

	utf32* operator->() const
	{
		return &**this;
	}

	iterator& operator++()
	{
		++d_ptr;
		return *this;
	}

	iterator operator++(int)
	{
		iterator temp = *this;
		++*this;
		return temp;
	}

	iterator& operator--()
	{
		--d_ptr;
		return *this;
	}

	iterator operator--(int)
	{
		iterator temp = *this;
		--*this;
		return temp;
	}

	iterator& operator+=(difference_type offset)
	{
		d_ptr += offset;
		return *this;
	}

	iterator operator+(difference_type offset) const
	{
		iterator temp = *this;
		return temp += offset;
	}

	iterator& operator-=(difference_type offset)
	{
		return *this += -offset;
	}

	iterator operator-(difference_type offset) const
	{
		iterator temp = *this;
		return temp -= offset;
	}

	utf32& operator[](difference_type offset) const
	{
		return *(*this + offset);
	}

	friend difference_type operator-(const iterator& lhs,
			const iterator& rhs)
	{
		return lhs.d_ptr - rhs.d_ptr;
	}

	friend iterator operator+(difference_type offset,
			const iterator& iter)
	{
		return iter + offset;
	}

	friend bool operator==(const iterator& lhs,
			const iterator& rhs)
	{
		return lhs.d_ptr == rhs.d_ptr;
	}

	friend bool operator!=(const iterator& lhs,
			const iterator& rhs)
	{
		return lhs.d_ptr != rhs.d_ptr;
	}

	friend bool operator<(const iterator& lhs,
			const iterator& rhs)
	{
		return lhs.d_ptr < rhs.d_ptr;
	}

	friend bool operator>(const iterator& lhs,
			const iterator& rhs)
	{
		return lhs.d_ptr > rhs.d_ptr;
	}

	friend bool operator<=(const iterator& lhs,
			const iterator& rhs)
	{
		return lhs.d_ptr <= rhs.d_ptr;
	}

	friend bool operator>=(const iterator& lhs,
			const iterator& rhs)
	{
		return lhs.d_ptr >= rhs.d_ptr;
	}

	utf32* d_ptr;
};

//! const iterator for String.
class  CText::const_iterator: public std::iterator<std::random_access_iterator_tag,
		const utf32>
{
public:
	const_iterator() :
			d_ptr(0)
	{
	}
	explicit const_iterator(const utf32* const ptr) :
			d_ptr(ptr)
	{
	}
	const_iterator(const CText::iterator& iter) :
			d_ptr(iter.d_ptr)
	{
	}

	const utf32& operator*() const
	{
		return *d_ptr;
	}

	const utf32* operator->() const
	{
		return &**this;
	}

	const_iterator& operator++()
	{
		++d_ptr;
		return *this;
	}

	const_iterator operator++(int)
	{
		const_iterator temp = *this;
		++*this;
		return temp;
	}

	const_iterator& operator--()
	{
		--d_ptr;
		return *this;
	}

	const_iterator operator--(int)
	{
		const_iterator temp = *this;
		--*this;
		return temp;
	}

	const_iterator& operator+=(difference_type offset)
	{
		d_ptr += offset;
		return *this;
	}

	const_iterator operator+(difference_type offset) const
	{
		const_iterator temp = *this;
		return temp += offset;
	}

	const_iterator& operator-=(difference_type offset)
	{
		return *this += -offset;
	}

	const_iterator operator-(difference_type offset) const
	{
		const_iterator temp = *this;
		return temp -= offset;
	}

	const utf32& operator[](difference_type offset) const
	{
		return *(*this + offset);
	}

	const_iterator& operator=(const CText::iterator& iter)
	{
		d_ptr = iter.d_ptr;
		return *this;
	}

	friend const_iterator operator+(difference_type offset,
			const const_iterator& iter)
	{
		return iter + offset;
	}

	friend difference_type operator-(const const_iterator& lhs,
			const const_iterator& rhs)
	{
		return lhs.d_ptr - rhs.d_ptr;
	}

	friend bool operator==(const const_iterator& lhs,
			const const_iterator& rhs)
	{
		return lhs.d_ptr == rhs.d_ptr;
	}

	friend bool operator!=(const const_iterator& lhs,
			const const_iterator& rhs)
	{
		return lhs.d_ptr != rhs.d_ptr;
	}

	friend bool operator<(const const_iterator& lhs,
			const const_iterator& rhs)
	{
		return lhs.d_ptr < rhs.d_ptr;
	}

	friend bool operator>(const const_iterator& lhs,
			const const_iterator& rhs)
	{
		return lhs.d_ptr > rhs.d_ptr;
	}

	friend bool operator<=(const const_iterator& lhs,
			const const_iterator& rhs)
	{
		return lhs.d_ptr <= rhs.d_ptr;
	}

	friend bool operator>=(const const_iterator& lhs,
			const const_iterator& rhs)
	{
		return lhs.d_ptr >= rhs.d_ptr;
	}

	const utf32* d_ptr;
};
}

#endif /* CTEXTITERATOR_H_ */
