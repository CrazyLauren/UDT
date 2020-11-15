/*
 * CDoubleStartprotection.h
 *
 * Copyright © 2020  https://github.com/CrazyLauren
 *
 *  Created on: 04.11.2020
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CDOUBLESTARTPROTECTION_H_
#define CDOUBLESTARTPROTECTION_H_

namespace NUDT
{
/** Protect to double start of Kernel
 *
 * Uses port to check is Kernel started
 *
 */
class CDoubleStartprotection: public NSHARE::CSingleton<CDoubleStartprotection>,
	public ICore
{
public:
	static const NSHARE::CText NAME;///< A serialization key
	static const NSHARE::CText PORT;

	CDoubleStartprotection();
	virtual ~CDoubleStartprotection();

	/*! @brief Start working (has to be non-blocking)
	 *
	 *	@return true if started successfully
	 */
	bool MStart();
	void MStop();

	NSHARE::CConfig MSerialize() const;
private:
	NSHARE::intrusive_ptr<NSHARE::CSingletonProcess> FProcess;
};

} /* namespace NUDT */

#endif /* CDOUBLESTARTPROTECTION_H_ */
