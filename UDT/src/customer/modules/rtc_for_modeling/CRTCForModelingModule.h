// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CRTCForModeling.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 25.08.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRTCFORMODELING_H_
#define CRTCFORMODELING_H_

#include <UType/CSingleton.h>
#include <UType/CSharedMemory.h>
#include <IRtcControl.h>
#include <udt_rtc_types.h>
#include <rtc_for_modeling_export.h>
namespace NUDT
{

/** RTC realized for modeling
 *
 */
class RTC_FOR_MODELING_EXPORT CRTCForModelingModule: public IRtcControl,
												NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;

	CRTCForModelingModule();

	/** Registry in RTC controller
	 *
	 *
	 * @param aName a name of RTC
	 */
	rtc_id_t MJoinToRTCWorker(name_rtc_t const& aName)
	{
		return rtc_id_t();
	}

	/** Gets ID by name
	 *
	 * @param aName a name of RTC
	 * @return ID of RTC
	 */
	virtual rtc_id_t MGetNameByRTC(name_rtc_t const& aName) const
	{
		return rtc_id_t();
	}

	/** Gets name by ID
	 *
	 * @param aName ID of RTC
	 * @return name of RTC
	 */
	virtual name_rtc_t MGetRTCByName(rtc_id_t const& aID) const
	{
		return name_rtc_t();
	}

	virtual IRtc* MGetRTC(name_rtc_t const& aID) const
	{
		return NULL;
	}
};
}

#endif /* CRTCFORMODELING_H_ */
