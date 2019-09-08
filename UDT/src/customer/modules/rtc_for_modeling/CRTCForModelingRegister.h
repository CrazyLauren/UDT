/*
 * CRTCForModelingRegister.h
 *
 *  Created on: 08.09.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRTCFORMODELINGREGISTER_H_
#define CRTCFORMODELINGREGISTER_H_

#include <rtc_for_modeling_export.h>

namespace NUDT
{
class RTC_FOR_MODELING_EXPORT CRTCForModelingRegister:public NSHARE::CFactoryRegisterer
{
public:
	static NSHARE::CText const NAME;
	CRTCForModelingRegister();
	void MUnregisterFactory() const;
protected:
	void MAdding() const ;
	bool MIsAlreadyRegistered() const ;
};
}
#endif /* CRTCFORMODELINGREGISTER_H_ */
