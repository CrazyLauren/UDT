/*
 * IRtc.h
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ICRTC_H_
#define ICRTC_H_

#include <udt/udt_rtc.h>
namespace NUDT
{
struct callback_rtc_t;

/** API for working with real time clock
 *
 * In practice, the time of system can differ from
 * "real" time. The kernel have inner module "RTC",
 * which is used for modeling purpose or when the
 * "RTC" is not  available in the hardware.
 * If the kernel "RTC" is turned off, calls of this
 * method is equal of calling the #SHARE::get_time()
 * function. Which in turn equals to call gettimeofday
 * of UNIX systems.
 * Thus, instead of using standard function
 * for gets the current system time (i.e. time(),
 * gettimeofday, clock_gettime (CLOCK_REALTIME))
 * it recommends to use interface.
 * This can save a lot of time in the future.
 *
 * For using the RTC You has to be connected to it by calling
 * #IRtc::MJoinToRTCWorker method.
 */
class IRtc
{
public:
	typedef double time_in_second_t;//!< time in second type
	typedef uint64_t millisecond_t;//!< time in miliseconds
	virtual ~IRtc(){}
	/** Registry in RTC controller
	 *
	 *
	 * @return true if noe error
	 */
	virtual bool MJoinToRTC() =0;

	/** Gets is joined to RTC
	 *
	 *
	 * @return true if joined
	 */
	virtual bool MIsJoinToRTC() const =0;

	/** UnRegistration in RTC controller
	 *
	 * * @return true if noe error
	 */
	virtual bool MLeaveFromRTC() =0;

	/** @brief Gets the current time in second
	 *
	 * @param aGroup A time of group (by default common time)
	 * @return current time in seconds
	 * 		   (precision - milliseconds)
	 */
	virtual time_in_second_t MGetCurrentTime() const=0;

	/** @brief Gets the current time in millisecond
	 *
	 * @param aGroup A time of group (by default common time)
	 * @return current time in millisecond
	 * 		   (precision - milliseconds)
	 */
	virtual millisecond_t MGetCurrentTimeMs() const=0;

	/** @brief Suspend a thread until the specified
	 * time comes
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method. If aTime is less or equal  of zero
	 * then it waits for the time is changed.
	 * In modeling purpose it's equal to send
	 * to modeling controller the time
	 * of "next call".
	 * @param aTime A time (absolute)
	 * @param aGroup A time of group (by default common time)
	 * @return current time in seconds
	 */
	virtual time_in_second_t MSleepUntil(time_in_second_t aTime) const =0;

	/** @brief Suspend a thread until the specified
	 * time comes
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method. If aTime is less or equal  of zero
	 * then it waits for the time is changed.
	 * In modeling purpose it's equal to send
	 * to modeling controller the time
	 * of "next call".
	 * @param aTime A time (absolute)
	 * @param aGroup A time of group (by default common time)
	 * @return current time in seconds
	 */
	virtual millisecond_t MSleepUntil(millisecond_t aTime) const =0;

	/** @brief Creates a timer
	 *
	 * The methods creates timer using the kernel "RTC"
	 * if is turn on or CLOCK_REALTIME if is turn off.
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method.
	 *
	 * @param aGroup A time of group (by default common time)
	 * @param aFirstCall A first expiry time (absolute)
	 * @param aIntervalCall A repetition interval or -1
	 * @param aHandler A pointer to the function for handling timer
	 */
	virtual bool MSetTimer(time_in_second_t aFirstCall, time_in_second_t aIntervalCall,
			callback_rtc_t const& aHandler)=0;

	/** @brief Creates a timer
	 *
	 * The methods creates timer using the kernel "RTC"
	 * if is turn on or CLOCK_REALTIME if is turn off.
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method.
	 *
	 * @param aGroup A time of group (by default common time)
	 * @param aFirstCall A first expiry time (absolute)
	 * @param aIntervalCall A repetition interval or 0
	 * @param aHandler A pointer to the function for handling timer
	 */
	virtual bool MSetTimer(millisecond_t aFirstCall, millisecond_t aIntervalCall,
			callback_rtc_t const& aHandler)=0;

	/** Returns info about used RTC
	 *
	 * What for it's necessary see  #MGetCurrentTime
	 * method.
	 * @param aGroup A time of group (by default common time)
	 * @return Info about RTC
	 */
	virtual rtc_info_t MGetRTCInfo() const=0;

	/** The current precision
	 *
	 * @return the minimal rtc step
	 */
	virtual time_in_second_t MGetPrecision() const =0;

	/** The current precision
	 *
	 * @return the minimal rtc step
	 */
	virtual millisecond_t MGetPrecisionMs() const =0;

	/** Wait for next time is occured
	 *
	 * @param aTime A time (absolute) or -1
	 * @return current time
	 * @warning The method can be unlocked before specified time
	 */
	virtual time_in_second_t MNextTime(time_in_second_t aTime) =0;

	/** Wait for next time is occured
	 *
	 * @param aTime A time (absolute) or 0
	 * @return current time
	 * @warning The method can be unlocked before specified time
	 */
	virtual millisecond_t MNextTime(millisecond_t aTime=0) =0;

	/** Gets amount of joined
	 *
	 * @return number of joined or -1
	 */
	virtual int MGetAmountOfJoined() const =0;
};

/*!\brief type of callback function which used in IRtc
 *
 *
 *\param WHO - pointer to IRtc
 *\param WHAT - A pointer to a structure that describes
 *				 the event that caused the callback to be
 *				 invoked, or NULL if there isn't an event.
 *				 The format of the data varies with event type.
 *				 (see *_args_t structures)
 *\param YOU_DATA -A pointer to data that you wanted to pass
 *					 as the third parameter(FYouData) callback_rtc_t structure.
 *
 *\return by default Callback functions must return 0
 *			for detail see NSHARE::eCBRval
 *
 *\see callback_rtc_t#operator()()
 */
typedef int (*signal_rtc_t)(IRtc* WHO, void* WHAT, void* YOU_DATA);

/*!\brief Regular callback structure used in "IRtc" structure
 *
 *\see signal_rtc_t
 */
struct callback_rtc_t
{
	signal_rtc_t FSignal; ///< A pointer to the callback function
	void* 	FYouData;/*!<A pointer to data that you
					 want to pass as the third parameter
					 to the callback function when it's invoked.*/

	/*!\brief The default constructor initializes
	 * @c FSignal and @c FYouData to NULL.
	 */
	callback_rtc_t();

	/*!\brief Only a pointer to the callback function
	 * will be passed to constructor.
	 *
	 * To the third parameter to the callback function will
	 * be passed NULL value.
	 *
	 *\param aSignal - A pointer to the callback function
	 */
	callback_rtc_t(signal_rtc_t const& aSignal);

	/*!\brief Two objects may be passed to a @c callback_rtc_t
	 * constructor to be copied
	 */
	callback_rtc_t(signal_rtc_t const& aSignal, void * const aData);

	/*!\brief There is also a copy constructor for
	 * the @c callback_rtc_t class itself.
	 */
	callback_rtc_t(callback_rtc_t const& aCB);

	/*!\brief callback_rtc_t assignment operator.
	 *
	 *\param  aCB  A callback_rtc_t of identical element.
	 *
	 *\return reference to this
	 */
	callback_rtc_t& operator=(callback_rtc_t const& aCB);

	/*!\brief Return true if A pointer to the callback
	 *function is exist
	 *
	 *\return true if @c FSignal is not NULL
	 */
	bool MIs() const;

	/*!\brief Invoke the callback and pass to it
	 *first argument aWho, second argument aArgs, third
	 *argument FYouData.
	 *
	 *\param aWho Who is invoking the callback
	 *\param aArgs a pointer to structure which provides information about related
	 *to the event callback being invoked. The format of the data varies with event type.
	 */
	int operator ()(IRtc* aWho, void * const aArgs) const;

	/*!\brief Two @c callback_rtc_t are equal if their members are equal.
	 *
	 *\param aRht What to compare object with
	 *\return true if the objects are equal
	 */
	bool operator ==(callback_rtc_t const& aRht) const;
};

inline callback_rtc_t::callback_rtc_t() :
		FSignal(NULL), FYouData(NULL)
{
	;
}
inline callback_rtc_t::callback_rtc_t(signal_rtc_t const& aSignal):
		FSignal(aSignal),//
		FYouData(NULL)
{
	;
}
inline callback_rtc_t::callback_rtc_t(signal_rtc_t const& aSignal, void * const aData) :
		FSignal(aSignal), FYouData(aData)
{
	;
}
inline callback_rtc_t::callback_rtc_t(callback_rtc_t const& aCB) :
		FSignal(aCB.FSignal), FYouData(aCB.FYouData)
{
	;
}
inline callback_rtc_t& callback_rtc_t::operator=(callback_rtc_t const& aCB)
{
	FSignal = aCB.FSignal;
	FYouData = aCB.FYouData;
	return *this;
}
inline bool callback_rtc_t::MIs()const
{
	return FSignal!=NULL;
}
inline int callback_rtc_t::operator ()(IRtc* aWho, void * const aArgs) const
{
	if (FSignal)
		return (*FSignal)(aWho, aArgs, FYouData);
	return -1;
}
inline bool callback_rtc_t::operator ==(callback_rtc_t const& rihgt) const
{
	return FSignal == rihgt.FSignal && FYouData == rihgt.FYouData;
}
}

#endif /* ICRTC_H_ */
