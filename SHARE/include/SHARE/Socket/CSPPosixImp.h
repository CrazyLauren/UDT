/*
 * CSPPosixImp.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 10.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CSPPOSIXIMP_H_
#define CSPPOSIXIMP_H_

namespace NSHARE
{
namespace NSerial
{
class CSerialPort::CImpl
{
	public:
		CImpl(CSerialPort& aThis);

		virtual ~CImpl();

		void MClose();
		bool MOpen();
		bool MIsOpen() const;

		bool MReconfigure();

		void MSetPort(const CText &port);
		CText MGetPort() const;
		const CSocket& MGetSocket(void) const;

		size_t MAvailable();

		void MWaitBytes(size_t aBytes);
		ssize_t MReceiveData(data_t *, std::size_t aCount, float const aTime);

		sent_state_t MSend(void const* const aData, std::size_t);

		void MFlush(eFlush const&); //FIXME to Socket

		void MSendBreak(int);

		bool MSignal(eLine const&, bool aVal);

		bool MLineState(eLine const&)const;

		bool MWaitForLineChanged();

		int MWaitData(float const aTime) const;

		NSHARE::CMutex FMutexRead;
		NSHARE::CMutex FMutexWrite;

	private:
		inline bool MIsFdValid();

		inline int MRead(data_t * aContainer,std::size_t) const;
		CSerialPort& FThis;
		CText FPort;
		CSocket FFd;
		uint32_t FByteTime;
		bool FIsOpen;

};
inline bool CSerialPort::CImpl::MIsOpen() const
{
	return FIsOpen;
}
inline void CSerialPort::CImpl::MSetPort(const CText &port)
{
	FPort = port;
}
inline CText CSerialPort::CImpl::MGetPort() const
{
	return FPort;
}
inline const CSocket& CSerialPort::CImpl::MGetSocket(void) const
{
	return FFd;
}
}
}

#endif /* CSPPOSIXIMP_H_ */
