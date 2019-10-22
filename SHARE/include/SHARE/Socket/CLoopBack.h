/*
 * CLoopBack.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 02.08.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CLOOPBACK_H_
#define CLOOPBACK_H_

#ifndef LOOP_BACK_SOCKET_EXIST
#	define LOOP_BACK_SOCKET_EXIST
#endif
#include <SHARE/Socket/diagnostic_io_t.h>
namespace NSHARE
{
/** It's a loop back socket which is based on UDP socket.
 * When it's opening, it chooses the free UPD  port.
 *
 *
 */
class SHARE_EXPORT CLoopBack: public NSHARE::ISocket
{
public:
	static const NSHARE::CText NAME;
	static const CText MAX_SPEED;
	CLoopBack(bool aNeedOpen=false,unsigned aMaxSpeed=0);
	CLoopBack(NSHARE::CConfig const& aConf);

	CSocket const& MGetSocket() const;
	void MClose();
	bool MReOpen();
	bool MOpen();
	sent_state_t MSend(void const*  aData, std::size_t aSize);
	sent_state_t MSend(data_t const& aVal);

	bool MReadAll();

	size_t MAvailable() const;
	bool MIsOpen() const;



	ssize_t MReceiveData(data_t *aData, float const aTime);
	std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig MSettings(void) const;
private:
	NSHARE::CUDP FClient;
	NSHARE::CUDP FServer;
	bool FIsOpened;
	std::list<data_t> FData;
	mutable NSHARE::CMutex FMutex;
	unsigned FMaxSpeed;
	double FCurrentSpeed;
	double FCurrentTime;
	diagnostic_io_t FDiagnostic;
};
} /* namespace NSHARE */
#endif /* CLOOPBACK_H_ */
