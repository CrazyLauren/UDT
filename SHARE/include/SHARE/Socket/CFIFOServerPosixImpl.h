/*
 * CFIFOServerPosixImpl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 22.10.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CFIFOSERVERPOSIXIMPL_H_
#define CFIFOSERVERPOSIXIMPL_H_
#include <SHARE/Socket/CSelectSocket.h>
namespace NSHARE
{
class CFIFOServer::CImpl
{
public:

	CImpl(CFIFOServer&);
	virtual ~CImpl();
	bool MReconfigure();
	void MAddNewClient(const CFIFOServer::path_t&);
	bool MOpen();
	bool MIsOpen() const;
	void MClose();
	void MClose(const CFIFOServer::path_t& aClientPath);
	size_t MAvailable() const;
	ssize_t MReceiveData(data_t*, const float aTime, recvs_from_t * aFrom);
	sent_state_t MSend(const void* const aData, std::size_t,
			const NSHARE::CConfig& aTo);
	sent_state_t MSend(const void* const aData, std::size_t, CFIFOServer::path_t const& aTo);

	const CSocket& MGetSocket() const;
	void MFlush(const eFlush&);
	void MSerialize(NSHARE::CConfig &) const;

	bool MIsMade() const;
	NSHARE::CMutex FMutexRead;
	NSHARE::CMutex FMutexWrite;

private:
	struct _write_t
	{
		path_t FClient;
		time_t FTime;
		mutable CSocket FWSocket;
	};
	typedef std::map<CText, _write_t,NSHARE::CStringFastLessCompare> write_fd_t;
	typedef write_fd_t::iterator it_write_fd_t;
	typedef write_fd_t::const_iterator cit_write_fd_t;
	inline CSocket MGetWriteFd(const CFIFOServer::path_t&) const;
	inline int MClose(CSocket&);
	inline bool MOpenReadCanncel();
	inline void MUnlink(const CText& aClientPath);
	void MCloseAllWriteCannnel();
	void MCloseReadCannel();
	CFIFOServer& FThis;
	//ev_io reader_watcher;
	//struct ev_loop * FLoop;
	CSelectSocket FSelect;
	CSocket FFdRead;
	CSocket FFdFixWrite;
	mutable write_fd_t FFDWrites;
	bool FIsOpen;
};
}//namespace NSHARE
#endif /* CFIFOSERVERPOSIXIMPL_H_ */
