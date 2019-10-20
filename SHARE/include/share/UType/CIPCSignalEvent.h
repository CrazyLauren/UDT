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

#include <share/UType/IConditionVariable.h>

#ifndef _WIN32
#	define IPC_USING_CONDVAR
#endif

namespace NSHARE
{
/**\brief Crossplatform realization of condition variable
 *
 * There are 3 realization:
 * 1) using CreateEvent (in Windows)
 * 2) using semaphore (in posix)
 * 3) using condvar (in posix)
 *
 * For working of condition variable need to allocate shared memory.
 */
class SHARE_EXPORT CIPCSignalEvent:public IConditionVariable,CDenyCopying
{
public:
	/**\brief Info about required buffer size
	 *
	 */
	enum
	{
# ifdef _WIN32
	eReguredBufSize=12+sizeof(NSHARE::atomic_t)*3
#elif defined(IPC_USING_CONDVAR)
	eReguredBufSize=SIZEOF_PTHREAD_COND_T+2*sizeof(uint32_t)+__alignof(void*)
#else//using sem_open
//#ifdef IPC_USING_CONDVAR_ON_SEMAPHORE
# 	error "Unknown target"
#endif	
	};

	/**@brief Condition variable creation control flags
	 *
	 */
	enum eOpenType
	{
		E_UNDEF,///< If the condition variable is exist then opens it, otherwise creates it
		E_HAS_TO_BE_NEW,///< если УП существует то возвращается ошибка,иначе создаётся
		E_HAS_EXIST,///< если УП существует то открыть, иначе создать
	};

	/** Default constructor
	 *
	 */
	CIPCSignalEvent();

	/**\brief Октрыть(создать) УП в буфере aBuf
	 *\param aBuf указатель на буфер, где будет храниться семафор
	 *\param aSize размер буфера (должен быть >=eReguredBufSize)
	 *\param aType \see eOpenType
	 */
	CIPCSignalEvent(uint8_t* aBuf, size_t aSize,eOpenType aIsNew=E_UNDEF);
	~CIPCSignalEvent();

	/**\brief Октрыть(создать) УП в буфере aBuf
	*	\param aBuf указатель на буфер, где будет храниться семафор
	*	\param aSize размер буфера (должен быть >=eReguredBufSize)
	*	\param aType \see eOpenType
	*/
	bool MInit(uint8_t* aBuf, size_t aSize,eOpenType aHasToBeNew=E_UNDEF);
	bool MSignal(void);
	bool MBroadcast(void);
	bool MTimedwait(IMutex *);
	bool MTimedwait(IMutex *, double const);

	/**\brief После вызова этого метода УП \aостается в памяти,
	 *  а объект становиться не инициализированным
	 */
	void MFree();
	bool MIsInited() const;

	/**\brief возвращет уникальный ID УП, если он существует
	 *
	 *\return - пустую строку в случае ошибки
	 * 			- ID УП
	 */
	NSHARE::CText const& MName() const;

	/**\brief удаляет условную переменную из памяти
	 *
	 */
	void MUnlink();

	/**\brief Возвращает в каком режиме была открыт УП
	 *
	 *\return E_HAS_TO_BE_NEW - УП была создан
	 * 		   E_HAS_EXIST - УП уже существовала
	 * 		   иное - УП не инициализирована
	 *
	 */
	eOpenType MGetType() const;
	static size_t sMRequredBufSize();

	/** Unit test
	 *
	 */
	static bool sMUnitTest();

	struct CImpl;
private:
	CImpl* FPImpl;
	eOpenType FType;
};

} /* namespace NSHARE */
#endif /* CIPCCONDVAR_H_ */
