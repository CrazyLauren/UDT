/*
 * CTCPSelectReceiver.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CTCPSELECTRECEIVER_H_
#define CTCPSELECTRECEIVER_H_

#include <share/Socket/CTcpImplBase.h>

namespace NSHARE
{
/** @brief It provides API to receive data
 * from different sockets by TCP.
 * It crates loop socket for force unblock "select" methods
 *
 */
class CTCPSelectReceiver:public CTcpImplBase, public IIntrusived
{
public:
	/** Info about socket
	 *
	 */
	struct socket_info_t
	{
		socket_info_t();

		CSocket FSocket;///< The socket
		diagnostic_io_t FDiagnostic;///< Diagnostic info
		size_t FSize;///<The size of read data
		sent_state_t FState;///< Todo
	};
	typedef std::vector<socket_info_t> read_data_from_t;///< Info about read data

	typedef NSHARE::ISocket::data_t data_t;

	CTCPSelectReceiver();

	/** Stopped select socket
	 * if MReceiveData function is working it will return -1
	 *
	 */
	void MClose();

	/** Open the receiver
	 *
	 *	@return true if no error
	 */
	bool MOpenSelectReceiver();

	/** Returns true if is opened
	 *
	 * @return true if is opened
	 */
	bool MIsOpen() const;

	/** @brief Receives data from all clients
	 *
	 *
	 * @param aFrom [out] From whom the data is received
	 * @param aBuf [out] A pointer to a buffer where can store the message.
	 * @param aTime [in] not used
	 * @return amount of received bytes
	 */
	ssize_t MReceiveData(read_data_from_t*aFrom, data_t*aBuf, const float aTime);

	/** Remove client
	 *
	 * @param aSocket Client for closing
	 */
	bool MRemoveClient(CSocket& aSocket);

	/** Add client
	 *
	 * @param aSocket Client for closing
	 */
	bool MAddClient(CSocket& aSocket);

	/** It forces unlock #MReceiveData method.
	 *
	 */
	void MForceUnLock();

	/** Remove all clients from select
	 *
	 */
	void MRemoveAllClients();

	/** The number of byte is available for reading
	 *
	 * @return
	 */
	size_t MAvailable() const;
protected:
	/** It's called after #MRemoveClient method
	 *
	 * @param aSocket info about the client
	 */
	virtual void MClientIsRemoved(CSocket& aSocket) =0;

	/** It's called after #MAddClient method
	 *
	 * @param aSocket info about the client
	 */
	virtual void MClientIsAdded(CSocket& aSocket) =0;
private:
	typedef std::set<CSocket> info_fd_t;///< The list of sockets
	typedef NSHARE::CSafeData<info_fd_t>::RAccess<> const CRAccsess;///< Access for reading to #info_fd_t type
	typedef NSHARE::CSafeData<info_fd_t>::WAccess<> CWAccsess;///< Access for reading to #info_fd_t type


	int MWaitForSelectToRead(CSelectSocket::socks_t& aTo, float const aTime);
	bool MOpenLoopSocket();
	void MReserveMemory(data_t* aBuf, CSelectSocket::socks_t const&);
	int MReceiveFromAllSocket(read_data_from_t& aFrom,data_t& aBuf, CSelectSocket::socks_t &);
	bool MRemoveClientImpl(CSocket& aSocket);
	bool MAddClientImpl(CSocket& aSocket);
	std::pair<bool,unsigned> MFilteringSockets(CSelectSocket::socks_t& aTo);

	CLoopBack FLoopBack;///< Loop back socket for force unlock select
	CSelectSocket FSelectSock;///< The select socket
	CMutex FReceiveMutex;///< The mutex for lock receive method
	atomic_t FIsDoing;///< 0 - no working, 1 - is working
	CSelectSocket::socks_t FTo;///< The sockets from each the data is received
	NSHARE::CSafeData<info_fd_t> FSockets;///<The list of sockets
	const NSHARE::CBuffer FTestMsg;
};

}

namespace std
{
inline std::ostream& operator<<(std::ostream & aStream, NSHARE::CTCPSelectReceiver::socket_info_t const& aVal)
{
	using namespace NSHARE;

	aStream << "Socket port:"<<aVal.FSocket<<std::endl;
	aStream << "Size:"<<aVal.FSize<<std::endl;
	aStream << "State:"<<aVal.FState<<std::endl;
	aStream << "Diagnostic:"<<aVal.FDiagnostic<<std::endl;
	return aStream;
}

inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CTCPSelectReceiver::read_data_from_t& aData)
{
	NSHARE::CTCPSelectReceiver::read_data_from_t::const_iterator _it(aData.begin()), _it_end(
			aData.end());

	if (!aData.empty())
		do
		{
			aStream << *_it;
			if (++_it != _it_end)
				aStream << ", ";
		} while (_it != _it_end);
	else
		aStream << "no sockets";

	return aStream;
}
}
#endif /* CTCPSELECTRECEIVER_H_ */
