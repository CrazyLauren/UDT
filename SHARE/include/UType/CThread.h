/*
 * CThread.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.02.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef _CTHREAD_H_
#define _CTHREAD_H_

namespace NSHARE
{
/** \brief Класс инкапсулирующий работу с потоками
 *
 *	Пример использования
 	\example
 	\code
	eCBRval thread1_run(void*, void*, void*)
	{
		return E_CB_SAFE_IT;
	}
	eCBRval thread2_run(void*, void*, void*)
	{
		return E_CB_SAFE_IT;
	}
 	CThread t1;
	t1 += CB_t(thread1_run, NULL);
	CThread t2;
	t2 += CB_t(thread2_run, NULL);
	\endcode
 *
 */
class SHARE_EXPORT CThread:public CEvent<>,CDenyCopying
{
public:

	/// \name Keys using for serialize
	/// \{
	static const NSHARE::CText NAME;
	static const NSHARE::CText RUN;
	static const NSHARE::CText THREAD_ID;
	static const NSHARE::CText PRIORITY;
	static const NSHARE::CText DETACHED;
	/// \}

	/** \brief sporadic scheduling parametrs
	 *	\todo implement
	 */
	struct sporadic_t//TODO
	{
		;
	};
	/** \brief Thread priority level
	 *
	 */
	enum eThreadPriority 
	{
        THREAD_PRIORITY_DEFAULT,//!< default priority
        THREAD_PRIORITY_MAX=1,//!< maximal priority
        THREAD_PRIORITY_HIGH,//!< medium between maximal and normal     priority
        THREAD_PRIORITY_NOMINAL,//!< normal     priority
        THREAD_PRIORITY_LOW,//!< medium between normal and minimal     priority
        THREAD_PRIORITY_MIN,//!< minimal     priority
		THREAD_PRIORITY_REAL_TIME,//!< setting thread for real time. todo linux
		THREAD_PRIORITY_AS_IDLE//!< priority of idle (using in deserialization method as fail mark)
	};
	/** \brief scheduling type
	 *
	 */
	enum eSched
	{
		FIFO = 1,//!< FIFO scheduling
		RR = 2,//!< round-robin scheduling
		SPORADIC=3,//!< sporadic scheduling(for qnx only)
	};

	/** \brief thread settings
	 *
	 */
	struct SHARE_EXPORT param_t
	{
		static const NSHARE::CText NAME;
		static const NSHARE::CText PRIORITY;
		static const NSHARE::CText SCHEDULING;
		static const NSHARE::CText CPU_NUMBER;

		sporadic_t sporadic;//!< use only for SPORADIC scheduling \warning not implemented
		eThreadPriority priority;//!< thread priority
		eSched	scheduling;//!< scheduling type
		int cpu_number;//!< Thread's CPU number, if FCPUNum<0 then using random CPU

		param_t();
		param_t(NSHARE::CConfig const& aConf);
		bool MIsValid() const;
		NSHARE::CConfig MSerialize() const;
	};

/** \brief constructor
 */
	CThread();
	explicit CThread(const param_t& aParam);
	explicit CThread(const NSHARE::CConfig& aConf);
	explicit CThread(CB_t const&,const param_t& aParam=param_t());
	CThread(CB_t const&,const NSHARE::CConfig& aConf);

	virtual ~CThread();

	/** \brief create thread
	 *	\param aParam -параметры потока, если NULL то используюся параметры указанные при инициализации
	 *	\return true - если создан
	 */
	bool MCreate(param_t const* aParam = NULL);

	/** \brief check if thread running
	 *	\return true - if running
	 */
	bool MIsRunning() const;

	/** \brief send kill signal
	 *
	 *	\warning Этот способ завершения потока не безопасен, лучше используйте MCancel
	 */
	void MKill();

	/** \brief force stopping thread
	*
	*	\return true - if successfully
	*/
	bool MCancel(); 


	/** \brief check if thread can be canceled
	*
	*	\return true - if can
	*/
	bool MTestCancel();

	/** \brief detach thread
	*
	*/
	void MDetach();

	/** \brief check if thred detached
	 *
	 */
	bool MIsDetached()const;

	/** \brief send signal to thread
	 *
	 *	\return true - if send
	 */
	bool MSignal(int signal);

	/** \brief join to thread
	 * \param aTime - waiting time in ns
	 */
	bool MJoin(uint64_t aTime = 0);

	/** \brief change thread priority
	 * 	\param priority - new priority
	 * 	\return true - if successfully
	 */
	bool MSetPriority(eThreadPriority priority);
	eThreadPriority MGetPriority()const;

	/** \brief change thread scheduling
	 * 	\param aPolicy - new scheduling
	 * 	\return true - if successfully
	 * 	\note Realtime Setting Policy is not supported
	 */
	bool MSetPolicy(eSched aPolicy);
	eSched MGetSchedulePolicy()const;

	/**\brief put cancellation cleanup function
	 *
	 */
	void MPutCleanUp(const CB_t& );
	bool MPopCleanUp(const CB_t& );
	void MRemoveAllCleanUp();

	/** \brief change thread's CPU
	 * 	\param cpunum - cpu number
	 * 	\return true - if successfully
	 * 	\note Realtime Setting cpu number is not supported
	 */
	bool MSetProcessor( unsigned int cpunum );

	/** \brief yield
	 */
	static bool sMYield();

	/** \brief return current thread id
	 *
	 */
	static unsigned sMThreadId();


	/** \brief return process id
	 *
	 */
	static unsigned sMPid()
	{
		return sProcessId();
	}
	static unsigned sProcessId();


	/** \brief return number of available CPU
	 *
	 */
	static unsigned sMNumberOfProcessor();

	/** \brief return thread id
	 *
	 */
	unsigned MThreadId()const;

	param_t const& MGetSetting() const;

	virtual std::ostream& MPrint(std::ostream& aVal)const;
	NSHARE::CConfig MSerialize() const;
protected:
	struct CImpl;
	virtual void MRun(void);
	virtual void MCancelCleanUp(void);//Event TODO

private:
	void MSetRunnig(bool aVal);
	inline void MInit();

	CImpl* FImpl;
	CEvent<> FCleanUp;
	volatile bool FIsRunning;
	volatile bool FIsDetached;
	param_t FParam;
};
inline bool CThread::MIsRunning() const
{
	return FIsRunning;
}
inline bool CThread::MIsDetached() const
{
	return FIsDetached;
}
inline void CThread::MSetRunnig(bool aVal)
{
	FIsRunning=aVal;
}
inline void CThread::MPutCleanUp(const CB_t& aVal)
{
	FCleanUp+=aVal;
}
inline bool CThread::MPopCleanUp(const CB_t& aVal)
{
	return FCleanUp-=aVal;
}
inline void CThread::MRemoveAllCleanUp()
{
	FCleanUp.MClear();
}
inline CThread::eThreadPriority CThread::MGetPriority()const
{
	return FParam.priority;
}
inline CThread::eSched CThread::MGetSchedulePolicy()const
{
	return FParam.scheduling;
}
inline CThread::param_t const& CThread::MGetSetting() const
{
	return FParam;
}
}//namespace USHARE
namespace NSHARE
{
SHARED_DEPRECATED_F( inline unsigned thread_id()
{
	return CThread::sMThreadId();
});
}

namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CThread const& aThread)
{
	return aThread.MPrint(aStream);
}
inline std::ostream& operator <<(std::ostream& aStream,
		NSHARE::CThread::eThreadPriority const& aPrior)
{
	switch (aPrior)
		{
			case NSHARE::CThread::THREAD_PRIORITY_MAX:
				aStream << "Max";
				break;
			case NSHARE::CThread::THREAD_PRIORITY_HIGH:
				aStream << "High";
				break;
			case NSHARE::CThread::THREAD_PRIORITY_NOMINAL:
				aStream << "Normal";
				break;
			case NSHARE::CThread::THREAD_PRIORITY_LOW:
				aStream << "Low";
				break;
			case NSHARE::CThread::THREAD_PRIORITY_MIN:
				aStream << "Min";
				break;
			case NSHARE::CThread::THREAD_PRIORITY_AS_IDLE:
				aStream << "Idle";
				break;
			case NSHARE::CThread::THREAD_PRIORITY_REAL_TIME:
				aStream << "RealTime";
				break;
			default:
				aStream << "Normal";
				break;
		}
	return aStream;
}
inline std::istream& operator>>(std::istream & aStream,
		NSHARE::CThread::eThreadPriority& aPrior)
{
	std::string _str;
	aStream >> _str;
	CHECK(!aStream.fail());

	std::string::iterator _it=_str.begin();

	for (; _it != _str.end(); ++_it)
		*_it = ::tolower(*_it);
	if(_str=="max")
		aPrior=NSHARE::CThread::THREAD_PRIORITY_MAX;
	else if(_str=="high")
		aPrior=NSHARE::CThread::THREAD_PRIORITY_HIGH;
	else if(_str=="normal")
		aPrior=NSHARE::CThread::THREAD_PRIORITY_NOMINAL;
	else if(_str=="low")
		aPrior=NSHARE::CThread::THREAD_PRIORITY_LOW;
	else if(_str=="min")
		aPrior=NSHARE::CThread::THREAD_PRIORITY_MIN;
	else if(_str=="idle")
		aPrior=NSHARE::CThread::THREAD_PRIORITY_AS_IDLE;
	else if(_str=="realtime")
		aPrior=NSHARE::CThread::THREAD_PRIORITY_REAL_TIME;
	else
		aStream.setstate(std::istream::failbit);

	return aStream;
}
}
#endif // #ifndef _CTHREAD_H_
