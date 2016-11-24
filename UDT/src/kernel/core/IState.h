/*
 * IState.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef ISTATE_H_
#define ISTATE_H_

namespace NUDT
{

class IState
{
public:
	IState(NSHARE::CText const& aName);//auto add to CDiagnostic
	virtual ~IState();
	virtual NSHARE::CConfig MSerialize() const=0;

	NSHARE::CConfig MSerializeRef() const;//return reference
};

} /* namespace NUDT */
#endif /* ISTATE_H_ */
