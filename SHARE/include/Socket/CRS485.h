/*
 * CRS485.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 22.03.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CRS485_H_
#define CRS485_H_
namespace NSHARE
{
namespace NSerial
{
class SHARE_EXPORT CRS485: public CSerialPort
{
public:
	CRS485(NSHARE::CConfig const& aConf);
	CRS485();
	virtual ~CRS485();
	virtual bool MOpen(CText const& aNamePort);
	virtual sent_state_t MSend(void const* aData, std::size_t);
	virtual ssize_t MReceiveData(data_t *, float const aTime);
	virtual std::ostream& MPrint(std::ostream & aStream)const;
protected:
	sent_state_t MSendWithEcho(void const*const aData, std::size_t);
	virtual bool MIsEcho();
	bool MIsSendingDataCorrectly(void const* const aData, std::size_t aByte);
private:
	void MPortToWrite(bool);
	sent_state_t MSendWithDTR(void const* const aData, std::size_t aByte);
	void MEraseEcho(data_t& _received);

	//size_t FSizeSendData;
	data_t FSendBuffer;
};
} /* namespace NSerial */
}//namespace USHARE

#endif /* CRS485_H_ */
