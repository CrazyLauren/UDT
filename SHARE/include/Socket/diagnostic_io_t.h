/*
 * diagnostic_io_t.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 11.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  

#ifndef DIAGNOSTIC_IO_T_H_
#define DIAGNOSTIC_IO_T_H_

namespace NSHARE
{
struct SHARE_EXPORT diagnostic_io_t
{
	static const NSHARE::CText NAME;
	diagnostic_io_t();

	void MRecv(size_t aCount) const;
	void MSend(size_t aCount) const;
	CConfig MSerialize() const;

	mutable size_t FRecvData;
	mutable size_t FRecvCount;
	mutable size_t FSentData;
	mutable size_t FSentCount;

};
}


#endif /* DIAGNOSTIC_IO_T_H_ */
