/*
 * CSingleton.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSINGLETON_H_
#define CSINGLETON_H_

#if defined(_MSC_VER)
#	pragma warning (push)
#	pragma warning (disable:4661)
#endif
namespace NSHARE
{
template<typename T> class CSingleton
{
protected:
	static T* sFSingleton;
public:
	typedef CSingleton<T> singleton_t;
	typedef T* singleton_pnt_t;

	~CSingleton(void)
	{
		CHECK_NOTNULL(sFSingleton);
		sFSingleton = 0;
	}
	static T& sMGetInstance()
	{
		CHECK_NOTNULL(sFSingleton);
		return (*sFSingleton);
	}
	static T* sMGetInstancePtr()
	{
		return (sFSingleton);
	}
protected:
	CSingleton()
	{
		CHECK(!sFSingleton);
		sFSingleton = static_cast<T*>(this);
		VLOG(2)<<"Construct singelton "<<this;
	}
private:
	CSingleton& operator=(const CSingleton&)
	{
		return *this;
	}
	CSingleton(const CSingleton&)
	{
	}
};
}
#if defined(_MSC_VER)
#	pragma warning (pop)
#endif
#endif /* CSINGLETON_H_ */
