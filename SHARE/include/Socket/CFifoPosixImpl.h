/*
 * CFifoPosixImpl.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 29.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CFIFOPOSIXIMPL_H_
#define CFIFOPOSIXIMPL_H_


namespace NSHARE
{
class CFifo::CImpl
{
public:
	static const unsigned ATOMIC_FIFO_BUUFER;

	CImpl(CFifo&);
	virtual ~CImpl();

	bool MOpen();
	bool MOpenRead();
	bool MOpenWrite();
	bool MIsOpen() const;
	void MClose();
	bool MIsMade() const;
	size_t MAvailable() const;
	virtual ssize_t MReceiveData(data_t*, const float aTime);
	virtual sent_state_t MSend(const void* const aData, std::size_t);
	const CSocket& MGetSocket() const;
	virtual void MFlush(const eFlush&);

	NSHARE::CMutex FMutexRead;
	NSHARE::CMutex FMutexWrite;

	bool MMakefifo(CText _path);
private:
	inline int MClose(CSocket &);
	inline void MSetBlock();
	inline void MSetUnBlock();
	inline void MUnlink(const CText& aClientPath);

	CFifo& FThis;
	CSocket FFdRead;
	CSocket FFdWrite;
	CSelectSocket FSelect;
	bool FIsOpen;
};
}//namespace NSHARE

#endif /* CFIFOPOSIXIMPL_H_ */
