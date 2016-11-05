/*
 * CSPWin32Impl.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 10.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */   
#ifndef CSPWIN32IMPL_H_
#define CSPWIN32IMPL_H_

#include <windows.h>

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

	void MSetPort(const CText &port);
	CText  MGetPort() const;
	const CSocket&  MGetSocket(void) const;

	size_t MAvailable();

	int MWaitData(float const aTime) const;


	ssize_t MReceiveData(data_t *, std::size_t aCount, float const aTime);

	sent_state_t MSend(void const* const aData, std::size_t);


	void MFlush(eFlush const&); //FIXME to Socket

	void MSendBreak(int);

	bool MSignal(eLine const&, bool aVal);

	bool MLineState(eLine const&)const;

	bool MWaitForLineChanged();

	bool MReconfigure();

	NSHARE::CMutex FMutexRead;
	NSHARE::CMutex FMutexWrite;

private:
	inline bool MIsHandleValid();
	OVERLAPPED FOReader;
	OVERLAPPED FOWriter;
	CSerialPort& FThis;
	std::wstring FPort;
	HANDLE FFd;
	mutable CSocket FFixSock;
	bool FIsOpen;
};
inline bool CSerialPort::CImpl::MIsOpen() const
{
	return FIsOpen;
}
}
}
#endif /* CSPWIN32IMPL_H_ */
