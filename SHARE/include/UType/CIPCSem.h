/*
 * CIPCSem.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 06.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CIPCSEM_H_
#define CIPCSEM_H_

#ifndef _WIN32
#	if defined(__linux__) && defined(USING_FUTEX)
#		define SEM_USING_FUTEX
#	else
#		include <semaphore.h>
#		define SM_USING_SEM_INIT
#	endif
#endif
namespace NSHARE
{
/**\brief кроссплатформенная межпроцессный семафор
 *
 * Существуют 4 реализации семафора:
 * 1) Через CreateSemaphore (в Windows)
 * 2) Через futex (в linux если стоит препроцессор USING_FUTEX)
 * 3) Через sem_init (в posix)
 * 4) Через sem_open (в posix)
 * Для работы  реализаций 2,3 нужно выделить разделяемую память
 * (shared memory).
 * Для работы  реализаций 1,4 нужно или выделить разделяемую память
 * (shared memory) или вкачестве буфера передать указатель на строку, содержашую
 * имя семафора длинной eReguredBufSize-1.
 */
class SHARE_EXPORT CIPCSem:CDenyCopying
{
public:
	/**\brief размер буфера необходимого для хранения семафора
	 *
	 *\note при использовании sem_open в буфере сохраняется имя семафора
	 */
	enum
	{
# ifdef _WIN32
		eReguredBufSize=16
#elif defined(SEM_USING_FUTEX)
		eReguredBufSize=sizeof(int32_t)+2*4+4
#elif defined(SM_USING_SEM_INIT)
		eReguredBufSize=(sizeof(sem_t)+2*4+__alignof(sem_t))
#else//using sem_open
		eReguredBufSize=16
#endif	
	};
	/**\brief флаг управления созданием семафора
	 *
	 */
	enum eOpenType
	{
		E_UNDEF,///< если сем. существует то открыть, иначе создать
		E_HAS_TO_BE_NEW,///< если семафор существует то возвращается ошибка,иначе создаётся
		E_HAS_EXIST///< если семафор \aне существует то возвращается ошибка,иначе открывется
	};
	/**\brief максимальное значение симафора
	 *
	 */
	static int const MAX_VALUE;

	CIPCSem();

	/**\brief Октрыть(создать) семафор в буфере aBuf
	 *\param aBuf указатель на буфер, где будет храниться семафор
	 *\param aSize размер буфера (должен быть >=eReguredBufSize)
	 *\param value начальное значение семафора(игнорируется если сем. существует)
	 *\param aType \see eOpenType
	 */
	CIPCSem(uint8_t* aBuf, size_t aSize,unsigned int value,eOpenType const aType=E_UNDEF,int aInitvalue=-1);
	~CIPCSem();

	/**\brief Октрыть(создать) семафор в буфере aBuf
	 *\param aBuf указатель на буфер, где будет храниться семафор
	 *\param aSize размер буфера (должен быть >=eReguredBufSize)
	 *\param value начальное значение семафора(игнорируется если сем. существует)
	 *\param aType \see eOpenType
	 *\return true - в случае успеха
	 */
	bool MInit(uint8_t* aBuf, size_t aSize,unsigned int value,eOpenType =E_UNDEF,int aInitvalue=-1);

	/**\brief После вызова этого метода семафор \aостается в памяти,
	 *  а объект становиться не инициализированным
	 */
	void MFree();
	bool MIsInited()const;
	bool MWait(void);
	bool MWait(double const);
	bool MTryWait(void);
	bool MPost(void);

	/**\brief возвращет текущее значение семафора
	 *
	 *\return -1 - в случае ошибки
	 * 			>=0 - значение семафора
	 */
	int MValue() const;

	/**\brief возвращет уникальный ID семафора, если он существует
	 *
	 *\return - пустую строку в случае ошибки
	 * 			- ID семафора
	 */
	NSHARE::CText const& MName()const;

	/**\brief удаляет семафор из памяти
	 *
	 */
	void MUnlink();

	/**\brief Возвращает в каком режиме был открыт семафор
	 *
	 *\return E_HAS_TO_BE_NEW - семафор был создан
	 * 		   E_HAS_EXIST - семафор уже существовал
	 * 		   иное - семфор не инициализирован
	 *
	 */
	eOpenType MGetType() const;

	static size_t sMRequredBufSize();
private:
	struct CImpl;
	CImpl *FImpl;
	eOpenType FType;
};
inline CIPCSem::eOpenType CIPCSem::MGetType() const
{
	return FType;
}
template<> class SHARE_EXPORT CRAII<CIPCSem> : public CDenyCopying
{
public:
	explicit CRAII(CIPCSem & aSem) :
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
	CIPCSem &FSem;
	volatile bool FIsLock;
};
}
/* namespace NSHARE */
#endif /* CIPCSEM_H_ */
