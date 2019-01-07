/*
 * CIPCCondvar.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 03.04.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CIPCCONDVAR_H_
#define CIPCCONDVAR_H_

#ifndef _WIN32
#	if defined(__linux__) && defined(USING_FUTEX)
#		define SE_USING_FUTEX
#	else
#		include <semaphore.h>
#		define SE_USING_SEM_INIT
#	endif
#endif

namespace NSHARE
{
/**\brief кроссплатформенная межпроцессная условная переменная(неполноценная)
 *
 * Существуют 4 реализации:
 * 1) Через CreateEvent (в Windows)
 * 2) Через futex (в linux если стоит препроцессор USING_FUTEX)
 * 3) Через sem_init (в posix)
 * 4) Через sem_open (в posix)
 * Для работы  реализаций 2,3 нужно выделить разделяемую память
 * (shared memory).
 * Для работы  реализаций 1,4 нужно или выделить разделяемую память
 * (shared memory) или вкачестве буфера передать указатель на строку, содержашую
 * имя услдовной переменной длинной eReguredBufSize-1.
 */
class CIPCSignalEvent:CDenyCopying
{
public:
	/** \brief размер буфера необходимого для хранения условной переменной
	 *
	 *	\note при использовании sem_open в буфере сохраняется имя условной переменной
	 */
	enum{
# ifdef _WIN32
	eReguredBufSize=16
#elif defined(SE_USING_FUTEX)
	eReguredBufSize=sizeof(int32_t)+2*4+4
#elif defined(SE_USING_SEM_INIT)
	eReguredBufSize=sizeof(sem_t)+2*4+__alignof(sem_t)
#else//using sem_open
	eReguredBufSize=16
#endif	
	};
	/** \brief флаг управления созданием условной переменной
	 *
	 */
	enum eOpenType
	{
		E_UNDEF,//!< если УП существует то открыть, иначе создать
		E_HAS_TO_BE_NEW,//!< если УП существует то возвращается ошибка,иначе создаётся
		E_HAS_EXIST,//!< если УП существует то открыть, иначе создать
	};
	CIPCSignalEvent();

	/** \brief Октрыть(создать) УП в буфере aBuf
	 *	\param aBuf указатель на буфер, где будет храниться семафор
	 *	\param aSize размер буфера (должен быть >=eReguredBufSize)
	 *	\param aType \see eOpenType
	 */
	CIPCSignalEvent(uint8_t* aBuf, size_t aSize,eOpenType aIsNew=E_UNDEF);
	~CIPCSignalEvent();

	/** \brief Октрыть(создать) УП в буфере aBuf
	*	\param aBuf указатель на буфер, где будет храниться семафор
	*	\param aSize размер буфера (должен быть >=eReguredBufSize)
	*	\param aType \see eOpenType
	*/
	bool MInit(uint8_t* aBuf, size_t aSize,eOpenType aHasToBeNew=E_UNDEF);
	bool MSignal(void);
	bool MTimedwait(CIPCSem *, const struct timespec* = NULL);
	bool MTimedwait(CIPCSem *, double const);

	/** \brief После вызова этого метода УП \aостается в памяти,
	 *  а объект становиться не инициализированным
	 */
	void MFree();
	bool MIsInited() const;

	/** \brief возвращет уникальный ID УП, если он существует
	 *
	 * \return - пустую строку в случае ошибки
	 * 			- ID УП
	 */
	NSHARE::CText const& MName() const;

	/** \brief удаляет условную переменную из памяти
	 *
	 */
	void MUnlink();

	/** \brief Возвращает в каком режиме была открыт УП
	 *
	 * \return E_HAS_TO_BE_NEW - УП была создан
	 * 		   E_HAS_EXIST - УП уже существовала
	 * 		   иное - УП не инициализирована
	 *
	 */
	eOpenType MGetType() const;
	static size_t sMRequredBufSize();
private:
	struct CImpl;
	CImpl* FPImpl;
};

} /* namespace NSHARE */
#endif /* CIPCCONDVAR_H_ */
