/*
 * CFlags.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 06.04.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef CFLAGS_H_
#define CFLAGS_H_
#include <bitset>
#ifndef SHARE_FLAGS_DEFINED
#	define SHARE_FLAGS_DEFINED
#endif
namespace NSHARE
{
class CNoMutex;

/**\brief класс для манипуляции с побитовыми флагами
 *
 *\tparam TFlags - enum в котором определены флаги
 *\tparam TVal - тип для хранения флагов
 */
SHARED_PACKED(template<typename TFlags = unsigned, typename TVal = unsigned,class TMutexType = CNoMutex>
class  CFlags:TMutexType///< Объект наследуется для того чтобы избежать проблем с размером при CNoMutex
{

public:
	typedef TFlags flags_t;
	typedef TVal value_type;
	typedef TMutexType mutex_t;

	explicit CFlags(TVal const& aVal = TFlags()) :
			FFlags(aVal)
	{
	}
	CFlags(CFlags const& aVal);
	CFlags& operator=(CFlags const& aRht);
	CFlags& operator=(value_type const& aRht);

	bool operator==(TFlags const& aFlag) const;
	bool operator==(CFlags const& aFlag) const;
	bool operator!=(CFlags const& aFlag) const;
	CFlags& operator+=(TFlags const& aFlag);
	CFlags& operator-=(TFlags const& aFlag);
	bool MGetFlag(TFlags const& aFlag) const;
	void MSetFlag(TVal aFlag, bool val);
	void MReset();
	TVal  MGetMask() const;

	static bool sMUnitTest();
private:
	inline mutex_t const & MGetMutex() const
	{
		return *this;
	}
	inline mutex_t & MGetMutex()
	{
			return *this;
	}

	TVal FFlags;
});

template<typename TFlags, typename TVal, class TMutexType>
inline CFlags<TFlags, TVal, TMutexType>::CFlags(CFlags const& aRht)
{
	;// \note don't copy mutex!
	CRAII<mutex_t> _blocked(aRht.MGetMutex());
	FFlags=aRht.FFlags;
}
template<typename TFlags, typename TVal, class TMutexType>
inline CFlags<TFlags, TVal, TMutexType>& CFlags<TFlags, TVal, TMutexType>::operator=(
		CFlags const& aRht)
{
	if (&aRht != this)
	{
		CRAII<mutex_t> _blocked(MGetMutex());
		CRAII<mutex_t> _blocked2(aRht.MGetMutex());
		FFlags = aRht.FFlags;
	}
	return *this;
}
template<typename TFlags, typename TVal, class TMutexType>
inline CFlags<TFlags, TVal, TMutexType>& CFlags<TFlags, TVal, TMutexType>::operator=(
		value_type const& aRht)
{
	{
		CRAII<mutex_t> _blocked(MGetMutex());
		FFlags =aRht;
	}
	return *this;
}
template<typename TFlags, typename TVal,class TMutexType>
inline void CFlags<TFlags, TVal,TMutexType>::MReset()
{
	CRAII<mutex_t> _blocked(MGetMutex());
	FFlags = TFlags();
}
template<typename TFlags, typename TVal,class TMutexType>
inline bool CFlags<TFlags, TVal,TMutexType>::MGetFlag(TFlags const& aFlag) const
{
	CRAII<mutex_t> _blocked(MGetMutex());
	return (FFlags & aFlag)!=0;
}
template<typename TFlags, typename TVal,class TMutexType>
inline bool CFlags<TFlags, TVal,TMutexType>::operator==(TFlags const& aFlag) const
{
	CRAII<mutex_t> _blocked(MGetMutex());
	return MGetFlag(aFlag);
}
template<typename TFlags, typename TVal,class TMutexType>
inline bool CFlags<TFlags, TVal,TMutexType>::operator==(CFlags const& aFlag) const
{
	CRAII<mutex_t> _blocked(MGetMutex());
	CRAII<mutex_t> _blocked2(aFlag.MGetMutex());
	return FFlags==aFlag.FFlags;
}
template<typename TFlags, typename TVal,class TMutexType>
inline bool CFlags<TFlags, TVal,TMutexType>::operator!=(CFlags const& aFlag) const
{
	CRAII<mutex_t> _blocked(MGetMutex());
	CRAII<mutex_t> _blocked2(aFlag.MGetMutex());

	return !(*this==aFlag);
}
template<typename TFlags, typename TVal,class TMutexType>
inline void CFlags<TFlags, TVal,TMutexType>::MSetFlag(TVal aFlag, bool val)
{
	CRAII<mutex_t> _blocked(MGetMutex());
	FFlags = (val) ? (FFlags | aFlag) : (FFlags & (~aFlag));
}
template<typename TFlags, typename TVal,class TMutexType>
inline CFlags<TFlags, TVal,TMutexType>& CFlags<TFlags, TVal,TMutexType>::operator+=(
		TFlags const& aFlag)
{
	MSetFlag(aFlag, true);
	return *this;
}
template<typename TFlags, typename TVal,class TMutexType>
inline CFlags<TFlags, TVal,TMutexType>& CFlags<TFlags, TVal,TMutexType>::operator-=(
		TFlags const& aFlag)
{
	MSetFlag(aFlag, false);
	return *this;
}
template<typename TFlags, typename TVal,class TMutexType>
inline TVal CFlags<TFlags, TVal,TMutexType>::MGetMask() const
{
	CRAII<mutex_t> _blocked(MGetMutex());
	return FFlags;
}
namespace detail
{
extern SHARE_EXPORT bool test_cflags();
}
template<typename TFlags, typename TVal,class TMutexType>
inline bool CFlags<TFlags, TVal,TMutexType>::sMUnitTest()
{
	return detail::test_cflags();
}
} //namespace USHARE
namespace std
{
template<class T,class Y>
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::CFlags<T,Y> const& aVal)
{
	std::bitset<sizeof(typename NSHARE::CFlags<T,Y>::value_type)*8> const _val(aVal.MGetMask());
	aStream<<_val;
	return aStream;
}
}
#endif /* CFLAGS_H_ */
