/*
 * CLinkDiagnostic.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.08.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CLINKDIAGNOSTIC_H_
#define CLINKDIAGNOSTIC_H_

namespace NUDT
{

class CLinkDiagnostic
{
public:
	CLinkDiagnostic();
	void MInput(const user_data_t& aVal);
	void MOutput(const user_data_t& aVal,bool aIs);

	void MSerialize(NSHARE::CConfig& aTo) const;
private:
	unsigned FCountDroped;
	unsigned FLastRecvPacketNumber;
	unsigned long long FSizeOfRecvUserData;

	unsigned FFailSent;
	unsigned FRepeatSent;
	unsigned long long FSizeOfSentUserData;
	unsigned FLastSentPacketNumber;
};

} /* namespace NUDT */
#endif /* CLINKDIAGNOSTIC_H_ */
