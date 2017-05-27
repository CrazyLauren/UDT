/*
 * CFIFOServerWin32Impl.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 27.06.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CFIFOSERVERWIN32IMPL_H_
#define CFIFOSERVERWIN32IMPL_H_

namespace NSHARE
{
class CFIFOServer::CImpl
{
	struct client_data_t
	{
		CText FPath;
		OVERLAPPED ovRead;
		OVERLAPPED ovConnect;
		HANDLE pipe;
		bool pipeConnected;
		time_t FTime;
	};
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
	int MWaitForEvent(HANDLE hEvents[]);
	ssize_t MReceiveData(data_t*, const float aTime, recvs_from_t * aFrom);
	sent_state_t MSend(const void* const aData, std::size_t,
			const NSHARE::CConfig& aTo);
	sent_state_t MSend(const void* const aData, std::size_t,
			CFIFOServer::path_t const& aTo);
	void MSerialize(NSHARE::CConfig &) const;

	const CSocket& MGetSocket() const;
	void MFlush(const eFlush&);

	bool MIsMade() const;
	NSHARE::CMutex FMutexRead;
	NSHARE::CMutex FMutexWrite;

	typedef SHARED_PTR<client_data_t> smart_client_t;
	typedef std::vector<smart_client_t> clients_data_t;
	typedef CSafeData<clients_data_t> safe_clients_t;

//	typedef write_fd_t::iterator it_write_fd_t;
//	typedef write_fd_t::const_iterator cit_write_fd_t;
	inline void MClose(SHARED_PTR<client_data_t>& aClient);
	inline void MReconnectClient(client_data_t& aPipe);
	CText MGetValidPath(CText const& _path);
	ssize_t MReceiveDataImpl(data_t*, const float aTime, recvs_from_t * aFrom);
	smart_client_t MAddNewClientImpl(const CFIFOServer::path_t&);
	sent_state_t MSendImpl(const void* const aData, std::size_t,
			CFIFOServer::path_t const& aTo);

	uint8_t FBufferPipe[8192];
	CFIFOServer& FThis;
	bool FIsOpen;
	HANDLE FResetEvent;
	safe_clients_t FClients;
};
} //namespace NSHARE
#endif /* CFIFOSERVERWIN32IMPL_H_ */
