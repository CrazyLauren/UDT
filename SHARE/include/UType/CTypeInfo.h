/*
 * CTypeInfo.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 11.09.2011
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTYPEINFO_H_
#define CTYPEINFO_H_

#include <typeinfo>

namespace NSHARE
{

class  CTypeInfo
{
public:
	template<class Type>
	struct SHARE_EXPORT _info
	{

	};
	template<class T>
	inline CTypeInfo(_info<T> const&):FInfo(typeid(T))
	{
	}
	template<class T>
	explicit inline CTypeInfo(const T&):FInfo(typeid(T))
	{
	}
	inline CTypeInfo(const std::type_info& ti) :
			FInfo(ti)
	{
	}
	inline const std::type_info& MGet() const
	{
		return FInfo;
	}

	inline bool MIsBefore(const CTypeInfo& aRhs) const//not guarantees
	{
		return FInfo.before(aRhs.FInfo) != 0;
	}
	inline const char* MName() const
	{
		return FInfo.name();
	}
	inline bool operator==(const CTypeInfo& aRhs)const
	{
		return (MGet() == aRhs.MGet()) != 0;
	}
	inline bool operator<(const CTypeInfo& aRhs)const
	{
		return MIsBefore(aRhs);
	}

	inline bool operator!=(const CTypeInfo& aRhs)const
	{
		return !this->operator ==(aRhs);
	}

	inline bool operator>(const CTypeInfo& aRhs)const
	{
		return aRhs.operator<(*this);
	}

	inline bool operator<=(const CTypeInfo& aRhs)const
	{
		return !(this->operator>(aRhs));
	}

	inline bool operator>=(const CTypeInfo& aRhs)const
	{
		return !(aRhs.operator<(*this));
	}
	inline std::ostream& MPrint(std::ostream& aStream) const
	{
		aStream<<MName();
		return aStream;
	}
private:
	const std::type_info& FInfo;
};
template<class T>
CTypeInfo get_type_info()
{
	return CTypeInfo(CTypeInfo::_info<T >());
}
} //NSHARE
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CTypeInfo const& aInfo)
{
	aInfo.MPrint(aStream);
	return aStream;
}
}

#endif // #ifndef CTYPEINFO_H_
