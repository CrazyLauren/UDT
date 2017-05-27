/*
 * CSemaphore.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.09.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSEMAPHORE_H_
#define CSEMAPHORE_H_

#include <deftype>

namespace NSHARE
{
/**\brief кроссплатформенная реализация семафора
 *
 *
 * Существуют три реализации семафора:
 * 1) Через CreateSemaphore (в Windows)
 * 2) Через sem_init (в posix)
 * 3) Костыль на базе услов. перем. и .мютексов
 */
class SHARE_EXPORT CSemaphore: NSHARE::CDenyCopying
{
public:

	CSemaphore(unsigned int value = 1);
	virtual ~CSemaphore();

	void MWait();
	bool MTryWait();
	void MPost();
	int MValue() const;
	bool MIsInited() const;
private:
	struct CImpl;
	CImpl* FImpl;

};


template<> class SHARE_EXPORT CRAII<CSemaphore> : public CDenyCopying
{
public:
	explicit CRAII(CSemaphore & aSem) :
			FSem(aSem)
	{
		MLock();
	}
	~CRAII()
	{
		MUnlock();
	}
	inline void MUnlock()
	{
		if (FIsLock)
			FSem.MPost();
		FIsLock = false;
	}
private:
	inline void MLock()
	{
		FSem.MWait();
		FIsLock =true;
	}
	CSemaphore &FSem;
	volatile bool FIsLock;
};
}

#endif /* CSEMAPHORE_H_ */
