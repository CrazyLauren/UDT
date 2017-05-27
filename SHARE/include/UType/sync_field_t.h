/*
 * sync_field_t.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 08.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef SYNC_FIELD_T_H_
#define SYNC_FIELD_T_H_

namespace NSHARE
{
template<typename _T,typename _Mutex=CMutexEmpty>
struct  sync_field_t
{
	typedef _T type_t;
	typedef _Mutex mutex_t;
	typedef sync_field_t<type_t> my_t;
	sync_field_t() :
			FVal(type_t()), FIsChanged(0)
	{
	}
	explicit sync_field_t(type_t const& aVal):FIsChanged(0)
	{
		MSet(aVal);
	}
	sync_field_t(const my_t& aRhs)
	{
		(*this) = aRhs;
	}
	my_t& operator =(const my_t& aRhs)
	{
		CRAII<mutex_t> _block(FMutex);
		FIsChanged = aRhs.FIsChanged;
		FVal = aRhs.FVal;
		return *this;
	}
	inline bool operator ==(const my_t& aRhs) const
	{
		return MGet() == aRhs.MGet();
	}
	inline bool operator !=(const my_t& aRhs) const
	{
		return !operator ==(aRhs);
	}

	const type_t& operator =(const type_t& aVal)
	{
		MSet(aVal);
		return MGet();
	}
	inline bool operator ==(const type_t& aVal) const
	{
		return MGet() == aVal;
	}
	inline bool operator !=(const type_t& aVal) const
	{
		return !operator ==(aVal);
	}
	inline bool operator >(const type_t& aVal) const
	{
		return MGet() > aVal;
	}
	inline bool operator >=(const type_t& aVal) const
	{
		return MGet() >= aVal;
	}
	inline bool operator <(const type_t& aVal) const
	{
		return MGet() < aVal;
	}
	inline bool operator <=(const type_t& aVal) const
	{
		return  MGet() <= aVal;
	}
	inline void MSet(type_t const& aVal)
	{
		CRAII<mutex_t> _block(FMutex);
		++FIsChanged;
		FVal = aVal;
	}
	inline void MUnSet()
	{
		CRAII<mutex_t> _block(FMutex);
		FIsChanged=0;
	}

	inline type_t const& MGet() const//FIXME
	{
		CRAII<mutex_t> _block(FMutex);
		return FVal;
	}
	template<typename _WT=type_t,typename _WMutex=mutex_t>
	struct  CWAcsess
	{
		typedef CWAcsess<_WT,_WMutex> type_t;
		CWAcsess(_WT& aVal, _WMutex& aMutex) :
			FVal(aVal),FMutex(aMutex)
		{
			FIsLock=FMutex.MLock();
		}
		CWAcsess(type_t const& aRht) :
				FVal(aRht.FVal),FMutex(aRht.FMutex)
		{
			FIsLock=aRht.FIsLock;
			aRht.FIsLock=false;
		}
		~CWAcsess()
		{
			FIsLock&&FMutex.MUnlock();
			FIsLock=false;
		}
		_WT& FVal;
	private:

		void operator=(type_t const& aRht)
		{

		}
		mutable bool FIsLock;
		_WMutex& FMutex;
	};
	inline CWAcsess<> MGetForChange()
	{
		CRAII<mutex_t> _block(FMutex);
		++FIsChanged;
		return CWAcsess<>(FVal,FMutex);
	}
/*	operator const type_t() const
	{
		return FVal;
	}*/
	inline unsigned MIsNeedHandle()
	{
		unsigned _tmp=MIsChanged();
		MUnSet();
		return _tmp;
	}
	inline unsigned MIsChanged() const
	{
		return FIsChanged;
	}

private:
	type_t FVal;
	unsigned FIsChanged;
	mutex_t FMutex;
};
}

namespace std
{
template<typename T,typename _Mutex>
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::sync_field_t<T,_Mutex> const& aVal)
{
	return aStream << aVal.MGet();
}
template<typename T,typename _Mutex>
std::istream& operator>>(std::istream & aStream,
		NSHARE::sync_field_t<T,_Mutex>& aBase)
{
	T _tmp;
	aStream>>_tmp;
	aBase=_tmp;
	return aStream;
}
}

#endif /* SYN_FIELD_T_H_ */
