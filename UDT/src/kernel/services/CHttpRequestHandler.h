/*
 * CHttpRequestHandler.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 13.05.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CHTTPREQUESTHANDLER_H_
#define CHTTPREQUESTHANDLER_H_

namespace NUDT
{
class CHttpRequest;
class CHttpRequestHandler
{
public:
	CHttpRequestHandler();
	virtual ~CHttpRequestHandler();
private:
	void MInit();
	static int sMHandleRequestId(CHardWorker* WHO, args_data_t* WHAT,
			void* YOU_DATA);
	void MHandleFrom(CHttpRequest const*, descriptor_t aFrom);
};

} /* namespace NUDT */
#endif /* CHTTPREQUESTHANDLER_H_ */
