/*
 * CFifoWin32Impl.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 26.06.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CFIFOWIN32IMPL_H_
#define CFIFOWIN32IMPL_H_

namespace NSHARE
{

class CFifo::CImpl
{
public:
	static const unsigned ATOMIC_FIFO_BUUFER;

	CImpl(CFifo&);
	virtual ~CImpl();

	bool MOpen();
	bool MOpen(double aTime);
	bool MIsOpen() const;
	void MClose();
	bool MIsMade() const;
	size_t MAvailable() const;
	virtual ssize_t MReceiveData(data_t*, const float aTime);
	virtual sent_state_t MSend(const void* const aData, std::size_t);
	const CSocket& MGetSocket() const;
	virtual void MFlush(const eFlush&);
	CText MGetValidPath(CText const& _path);
	NSHARE::CMutex FMutexRead;
	NSHARE::CMutex FMutexWrite;
	NSHARE::CMutex FMutexConnect;

	bool MMakefifo(CText const&_path);
private:
	inline int MClose(CSocket &);

	OVERLAPPED FOReader;
	OVERLAPPED FOWriter;
	CFifo& FThis;
	HANDLE FHandler;
	size_t FBufSize;
	bool FIsOpen;
	uint8_t FBufferPipe[8192];
};

} /* namespace NSHARE */
#endif /* CFIFOWIN32IMPL_H_ */
