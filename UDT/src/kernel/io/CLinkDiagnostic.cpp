// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CLinkDiagnostic.cpp
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <udt/udt_types.h>
#include <shared_types.h>
#include "CLinkDiagnostic.h"

namespace NUDT
{

CLinkDiagnostic::CLinkDiagnostic()
{
	memset(this,0,sizeof(*this));
}
void CLinkDiagnostic::MOutput(const user_data_t& aVal,bool aSent)
{
	unsigned const aNumber = aVal.FDataId.FPacketNumber, aSize =
			(unsigned)aVal.FData.size();
	if (aSent)
	{
		FSizeOfSentUserData += aSize;
		VLOG(1) << "Packet#" << aNumber << " totalsize="
							<< (FSizeOfSentUserData / 1024 / 1024) << " mb;";
		if (!aNumber)
		{
			LOG(ERROR)<<"Zero number of packet.";
			FLastSentPacketNumber=aNumber;
			return;
		}
		if (!FLastSentPacketNumber)
			FLastSentPacketNumber = aNumber - 1;

		if (FLastSentPacketNumber == aNumber)
		{
			++FRepeatSent;
			//LOG(WARNING)<<FLastSentPacketNumber<<" has been repeated.";
		}
		FLastSentPacketNumber = aNumber;
	}
	else
		++FFailSent;


}
void CLinkDiagnostic::MInput(const user_data_t& aVal)
{
	unsigned const aNumber = aVal.FDataId.FPacketNumber, aSize =
			(unsigned)aVal.FData.size();
	FSizeOfRecvUserData += aSize;
	VLOG(1) << "Packet#" << aNumber << " totalsize="
						<< (FSizeOfRecvUserData / 1024 / 1024) << " mb;";
	if (!aNumber)
	{
		LOG(ERROR)<<"Zero number of packet.";
		FLastRecvPacketNumber=aNumber;
		return;
	}
	if (!FLastRecvPacketNumber)
		FLastRecvPacketNumber = aNumber - 1;

	if ((FLastRecvPacketNumber + 1) != aNumber)
	{
		++FCountDroped;
		//LOG(WARNING)<<FLastRecvPacketNumber<<" has been dropped.";
	}
	FLastRecvPacketNumber = aNumber;
}
void CLinkDiagnostic::MSerialize(NSHARE::CConfig& _dig) const
{
	_dig.MAdd("drop",FCountDroped);
	_dig.MAdd("rud",FSizeOfRecvUserData);

	_dig.MAdd("fs",FFailSent);
	_dig.MAdd("rs",FRepeatSent);
	_dig.MAdd("sud",FSizeOfSentUserData);
}
} /* namespace NUDT */
