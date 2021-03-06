/*
 * udt_rtc.h
 *
 *  Created on: 05.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef UDT_RTC_H_
#define UDT_RTC_H_

#include "udt_share_macros.h"

namespace NUDT
{
typedef  NSHARE::CProgramName name_rtc_t;//!< Type of name RTC

/** Type of real-time clock which is used in kernel
 *
 */
enum eRTCType
{
	eRTC_TURN_OFF, /*!<The RTC of kernel is turn off. //!< eRTC_TURN_OFF
	 Direct call of the similar system function is used.
	 Thus this is the faster RTC as
	 does not have any additional cost.
	 */
	eRTC_TURN_ON, /*!<The RTC of kernel is turn on.   //!< eRTC_TURN_ON
	 It's realization own time counter,
	 without using counter of the OS.
	 Usually it's used in embedded computer
	 which has the specified RTC counter but
	 the driver to OS cannot be written.
	 */
	eRTC_MODELING, /*!<The kernel is modeling the RTC,//!< eRTC_MODELING
	 Or course, it's not RTC in the truest
	 sense of the word, but if the program
	 isn't communicating with real hardware
	 this mode is equal of RTC.
	 */
	eRTC_DEFAULT = eRTC_TURN_OFF                      //!< eRTC_DEFAULT
};

/** Info about RTC
 *
 */
struct  UDT_SHARE_EXPORT rtc_info_t
{
	static const NSHARE::CText NAME;///< A serializing key
	static const NSHARE::CText KEY_RTC_TYPE;///< A serializing key of #FRTCType
	static const NSHARE::CText KEY_NAME;///< A serializing key of #FName
	static const NSHARE::CText KEY_NULL_OFFSET;///< A serializing key of #FOffset

	eRTCType FRTCType; //!< Type of RTC
	name_rtc_t FName;//!< The unique name of RTC
	NSHARE::IAllocater::offset_pointer_t FOffset;//!< offset to info in shared memory

	/*! @brief The default constructor
	 */
	rtc_info_t();

	/*! @brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 *@param aConf Serialized object
	 */
	rtc_info_t(NSHARE::CConfig const& aConf);

	/*! @brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/*! @brief Checks object for valid
	 *
	 * Usually It's used after deserializing object
	 * @return true if it's valid.
	 */
	bool MIsValid()const;

	/** Check for equal
	 *
	 * @param aInfo
	 * @return true if equal
	 */
	bool operator==(rtc_info_t const& aInfo) const;
};

/** Info about available real time clocks
 *
 */
struct UDT_SHARE_EXPORT real_time_clocks_t:std::vector<rtc_info_t>
{
	static const NSHARE::CText NAME;///< A serializing key
	static const NSHARE::CText SHARED_MEMORY_NAME; ///< A serialization key of shared memory unique name

	NSHARE::CText  FShdMemName;//!< shared memory name
	/*! @brief The default constructor
	 */
	real_time_clocks_t()
	{
	}

	/*! @brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 *@param aConf Serialized object
	 */
	real_time_clocks_t(NSHARE::CConfig const& aConf);

	/*! @brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/*! @brief Checks object for valid
	 *
	 * Usually It's used after deserializing object
	 * @return true if it's valid.
	 */
	bool MIsValid()const;
};
}

namespace std
{
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::eRTCType const& aVal)
{
	using namespace NUDT;

	switch(aVal)
	{
	case eRTC_TURN_OFF:
		aStream << "turn off";
		break;
	case eRTC_TURN_ON:
		aStream << "turn on";
		break;

	case eRTC_MODELING:
		aStream << "for modeling";
		break;
	}
	return aStream;
}

inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::rtc_info_t const& aVal)
{
	aStream << "Type:" << aVal.FRTCType << std::endl;
	aStream << "Name:" << aVal.FName << std::endl;
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream,
		NUDT::real_time_clocks_t const& aVal)
{
	for (NUDT::real_time_clocks_t::const_iterator _it = aVal.begin();
			_it != aVal.end();)
	{
		aStream << (*_it);
		if (++_it != aVal.end())
			aStream << std::endl;
	}
	return aStream;
}
}
#endif /* UDT_RTC_H_ */
