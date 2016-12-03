/*
 * deftype.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 22.03.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef DEFTYPE_H_
#define DEFTYPE_H_
#include <stdint.h>
#include <UType/buffer_value.h>
namespace NSHARE
{
template<class U, class T>
struct check_type
{
	typedef char False[1];
	typedef char True[2];

	static True& f(U*);
	static False& f(...);

	static T* MakeT();
	enum
	{
		result = sizeof(f(MakeT())) == sizeof(True),
	};
};

struct  angle_t
{
	angle_t():degree(0),min(0),sec(0),usec(0)
	{

	}
#ifdef M_PI
	angle_t(double const &aRad)
	{
		MSet(aRad);
	}
	void MSet(double aRad)
	{
		aRad =fmod (aRad,2 * M_PI);
		if (aRad < 0)
		aRad += 2 * M_PI;
		double _sec = aRad * 360 * 60 * 30 / M_PI;
		uint32_t _isec=static_cast<uint32_t>(_sec);
		usec = static_cast<uint32_t>((_sec - _isec) * 1000 * 1000);
		if(usec==999999) //погрешность получили
		{
			usec=0;
			_isec=static_cast<uint32_t>(round(_sec));
		}
		sec = _isec % 60;
		min = (_isec / 60)% 60;
		degree = _isec / 3600;
	}
	double MGetRad() const
	{
		double _rad = 0;
		_rad = static_cast<double>(usec) / 1000 / 1000;
		_rad += degree * 3600.0;
		_rad += min * 60.0;
		_rad += sec;
		_rad = _rad / 360 / 60 / 30 * M_PI;
		return _rad;
	}

	angle_t operator +(const angle_t& aRval) const
	{
		return this->operator +(aRval.MGetRad());
	}

	angle_t operator -(const angle_t& aRval)const
	{
		return this->operator -(aRval.MGetRad());
	}

	angle_t operator +(const double& aRval) const
	{
		angle_t _val(MGetRad()+aRval);
		return _val;
	}

	angle_t operator -(const double& aRval) const
	{
		angle_t _val(MGetRad()-aRval);
		return _val;
	}
	angle_t operator *(const double& aRval) const
	{
		angle_t _val(MGetRad()*aRval);
		return _val;
	}
	angle_t operator /(const double& aRval) const
	{
		if(aRval)
		{	angle_t _val(MGetRad()/aRval);
			return _val;
		}
		return *this;
	}
	angle_t& operator +=(const angle_t& aRval)
	{
		return *this=this->operator +(aRval);
	}

	angle_t& operator -=(const angle_t& aRval)
	{
		return *this=this->operator -(aRval);
	}

	angle_t& operator +=(const double& aRval)
	{
		return *this=this->operator +(aRval);
	}

	angle_t& operator -=(const double& aRval)
	{
		return *this=this->operator -(aRval);
	}
	angle_t& operator *=(const double& aRval)
	{
		return *this=this->operator *(aRval);
	}
	angle_t& operator /=(const double& aRval)
	{
		return *this=this->operator /(aRval);
	}
#endif
	int16_t degree;
	uint8_t min;
	uint8_t sec;
	uint32_t usec;
};

typedef std::string String;
typedef std::deque<String> Strings; //FIXME
typedef int signal_t(void* WHO, void* WHAT, void* YOU_DATA);
typedef int (*psignal_t)(void* WHO, void* WHAT, void* YOU_DATA);
template<typename TSignal> struct  Callback_t
{
	typedef TSignal pM;
	typedef void* arg_t;
	Callback_t() :
			FSignal(NULL), FYouData(NULL)
	{
		;
	}
	Callback_t(TSignal const& aSignal, void * const aData) :
			FSignal(aSignal), FYouData(aData)
	{
		;
	}
	Callback_t(Callback_t<TSignal> const& aCB) :
			FSignal(aCB.FSignal), FYouData(aCB.FYouData)
	{
		;
	}
	TSignal FSignal;
	void* FYouData;
#if __cplusplus >= 201103

	explicit operator bool() const
	{
		return MIs();
	}
#endif
	bool MIs()const
	{
		return FSignal!=NULL;
	}
	template<class T, class Y>
	int operator ()(T* aWho, Y * const aArgs) const
	{
		if (FSignal)
			return (*FSignal)(aWho, aArgs, FYouData);
		return -1;
	}
	template<class T>
	int operator ()(T* aWho, void * const aArgs) const
	{
		if (FSignal)
			return (*FSignal)(aWho, aArgs, FYouData);
		return -1;
	}
//	int operator ()(void* aWho, void * const aArgs) const
//	{
//		if (FSignal)
//			return (*FSignal)(aWho, aArgs, FYouData);
//		return -1;
//	}
	bool operator ==(Callback_t const& rihgt) const
	{
		return FSignal == rihgt.FSignal && FYouData == rihgt.FYouData;
	}
};
template struct SHARE_EXPORT Callback_t<psignal_t>;
typedef Callback_t<psignal_t> CB_t;

template<bool (*psignal_t)(void* WHO, void* WHAT, void* YOU_DATA)>
struct  CB_static_t
{
	typedef void* arg_t;
	CB_static_t() :
			FYouData(NULL)
	{
		;
	}
	CB_static_t(void * const aData) :
			FYouData(aData)
	{
		;
	}
	void* FYouData;
	bool operator ()(void* aWho, void * const aArgs)
	{
		return psignal_t(aWho, aArgs, FYouData);
	}
};

//template<typename T>
//struct IsThereX
//{
//	struct FallBack
//	{
//		int X;
//	};
//	struct Derived: T, FallBack
//	{
//	};
//
//	template<typename U,  U> struct Check;
//
//	typedef char False[1];
//	typedef char True[2];
//
//	template<typename U>
//	static True& f(Check<int (FallBack::*), & U::X> *);
//
//	template<typename U>
//	static False& f(...);
//	enum
//	{
//		result = sizeof(f < Derived > (0)) == sizeof(True),
//	};
//};
} //namespace USHARE
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		const NSHARE::Strings& aSrts)
{
	for (NSHARE::Strings::const_iterator _it = aSrts.begin();
			_it != aSrts.end(); ++_it)
		aStream << *_it << "\n";
	return aStream;
}

inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::angle_t const& aAngle)
{
	static const NSHARE::CText _win_fix(1,0xb0);
	static const NSHARE::CText _win_fix2(1,0xb4);
	aStream << (int) aAngle.degree << _win_fix << (unsigned) aAngle.min
			<< _win_fix2 << (unsigned) aAngle.sec << "."
			<< (unsigned) aAngle.usec << _win_fix2<<_win_fix2;
	return aStream;
}
template<typename TSignal>
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::Callback_t<TSignal> const& aCb)
{
	aStream.setf(ios::hex, ios::basefield);
	aStream << "Pointer to data:" << aCb.FYouData << "; Pointer to cb handler: "
			<< aCb.FSignal;
	aStream.unsetf(ios::hex);
	return aStream;
}
}
#endif /* DEFTYPE_H_ */
