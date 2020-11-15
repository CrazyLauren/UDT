/*
 * CSingletonProcess.h
 *
 * Copyright © 2020  https://github.com/CrazyLauren
 *
 *  Created on: 01.11.2020
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CSINGLETONPROCESS_H_
#define CSINGLETONPROCESS_H_
#include <SHARE/Socket/CSocket.h>

namespace NSHARE
{
class SHARE_EXPORT CSingletonProcess:public NSHARE::IIntrusived
{
public:
	CSingletonProcess(uint16_t aPort);
	~CSingletonProcess();

	bool MIs() const;

	uint16_t const FPort;
private:
	void MCheck();

	CSocket FSockId;
	int FRc;
};
}


#endif /* CSINGLETONPROCESS_H_ */
