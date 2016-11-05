/*
 * CFlags.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 06.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef CFLAGS_H_
#define CFLAGS_H_

namespace NSHARE
{
//Flag 0 - �� ���������� ��������
template<typename TFlags = unsigned, typename TVal = unsigned>
class  CFlags
{
public:
	explicit CFlags(TVal const& aVal = TFlags()) :
			FFlags(aVal)
	{
	}
	bool operator==(TFlags const& aFlag) const;
	bool operator==(CFlags const& aFlag) const;
	bool operator!=(CFlags const& aFlag) const;
	CFlags& operator+=(TFlags const& aFlag);
	CFlags& operator-=(TFlags const& aFlag);
	bool MGetFlag(TFlags const& aFlag) const;
	void MSetFlag(TVal aFlag, bool val);
	void MReset();
	TVal const& MGetMask() const;
private:
	TVal FFlags;
};
template<typename TFlags, typename TVal>
inline void CFlags<TFlags, TVal>::MReset()
{
	FFlags = TFlags();
}
template<typename TFlags, typename TVal>
inline bool CFlags<TFlags, TVal>::MGetFlag(TFlags const& aFlag) const
{
	return (MGetMask() & aFlag)!=0;
}
template<typename TFlags, typename TVal>
inline bool CFlags<TFlags, TVal>::operator==(TFlags const& aFlag) const
{
	return MGetFlag(aFlag);
}
template<typename TFlags, typename TVal>
inline bool CFlags<TFlags, TVal>::operator==(CFlags const& aFlag) const
{
	return FFlags==aFlag.FFlags;
}
template<typename TFlags, typename TVal>
inline bool CFlags<TFlags, TVal>::operator!=(CFlags const& aFlag) const
{
	return !(*this==aFlag);
}
template<typename TFlags, typename TVal>
inline void CFlags<TFlags, TVal>::MSetFlag(TVal aFlag, bool val)
{
	FFlags = (val) ? (FFlags | aFlag) : (FFlags & (~aFlag));
}
template<typename TFlags, typename TVal>
inline CFlags<TFlags, TVal>& CFlags<TFlags, TVal>::operator+=(
		TFlags const& aFlag)
{
	MSetFlag(aFlag, true);
	return *this;
}
template<typename TFlags, typename TVal>
inline CFlags<TFlags, TVal>& CFlags<TFlags, TVal>::operator-=(
		TFlags const& aFlag)
{
	MSetFlag(aFlag, false);
	return *this;
}
template<typename TFlags, typename TVal>
inline TVal const& CFlags<TFlags, TVal>::MGetMask() const
{
	return FFlags;
}
} //namespace USHARE

#endif /* CFLAGS_H_ */
