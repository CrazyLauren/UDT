/*
 * CTCPServer.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CTCPSERVER_H_
#define CTCPSERVER_H_

/*
 *	chanelog
 *
 *	Версия 0.2 (29.01.2014)
 *
 *		- Исправлен баг с loopback socket.
 *  		Если кто-то подключался одновременно c "петлёй", то её сокет терялся.
 *			Теперь в MAddNewClient  присвается FIsLoopConnected true.
 *
 *	Версия 0.3 (01.02.2016)
 *		- Исправлен баг с loopback socket.
 *			Гонка потоков. Если connect loopback случался рашьше чем accept
 *			ресурсы утекали.
 *
 *	Версия 0.4 (05.02.2016)
 *		- loopback trick переписон через два udp socketa - теперь всё работает как часы.
 *
 *	Версия 0.5 (22.04.2016)
 *		- CImpl
 *		- убран костыль smart_recv_t
 *
 *	Версия 0.6 (23.04.2016)
 *		- Теперь можно открять TCP сервер со случайным портом
 *
 *	Версия 0.7 (22.05.2016)
 *		- LoopBack выделен в отдельный класс
 *
 *	Версия 1.0 (30.06.2016)
 *		- Смена API
 *
 *	Версия 1.1 (30.08.2016)
 *		- Добавлен метод MGetBuf
 *
 */
#include "CSelectSocket.h"

//todo decrease the number of locks
#ifndef TCP_SOCKET_EXIST
#	define TCP_SOCKET_EXIST
#endif
namespace NSHARE
{
/** @brief TCP Server
 *
 * The TCP server have two event. When the client is connected to the server
 * the event #CTCPServer::EVENT_CONNECTED is called respectively
 * When it is disconnected the event #CTCPServer::EVENT_DISCONNECTED
 * is called. The argument of this events is pointer to #CTCPServer::client_t type
 * object. To disconnect the client You can call #CTCPServer::MCloseClient
 * Of course, for receiving data you has to be call #CTCPServer::MReceiveData
 * method. There two types of this method: The first method can return info
 * about the data sender, the other - no.
 * To send data you could call #CTCPServer::MSend method. Just like MReceiveData
 * It divides to the two types: The first method sends data to the specified
 * client, the other sends data to the all connected clients.
 *
 */
class SHARE_EXPORT CTCPServer: public ISocket, NSHARE::CDenyCopying, public NSHARE::CEvents<
		NSHARE::CText>
{
public:
	static const NSHARE::CText NAME;///< A serialization key
	static version_t sMGetVersion();///< The version of TCP server class

	typedef NSHARE::CEvents<NSHARE::CText> events_t;///< A type of events used

	/** @brief Connected client information
	 *
	 */
	struct SHARE_EXPORT client_t
	{
		net_address FAddr;///< A network address of client
		time_t FTime;///< The connection time

		bool operator==(client_t const& aRht) const;
		bool operator==(net_address const& aRht) const;
	};
	typedef std::vector<client_t> list_of_clients;///< List of clients

	/** @brief Information about the data received  from client
	 *
	 */
	struct  recv_t
	{
		client_t FClient;///< From whom the data.
		data_t::iterator FBufBegin;///< The pointer to the first byte of the data
		data_t::difference_type FSize;///< The size of the received data
	};
	typedef std::vector<recv_t> recvs_t;///< List of received data

	static events_t::key_t const EVENT_CONNECTED;/*!< It event is called when client is connected.
													* The argument of the event is pointer to the
													* CTCPServer::client_t object.
	 	 	 	 	 	 	 	 	 	 	 	 	*/
	static events_t::key_t const EVENT_DISCONNECTED;/*!< It event is called when client is disconected.
													* The argument of the event is pointer to the
													* CTCPServer::client_t object.
													*/

	/** @brief Create TCP server
	 *
	 * If You pass a valid port to aParam
	 *  then the TCP server begins to wait for clients.
	 * @param aParam The port of TCP server
	 */
	CTCPServer(const net_address& aParam = net_address());

	/** @brief Create TCP server
	 *
	 * If You pass a valid serialized port to aConf
	 *  then the TCP server begins to wait for clients.
	 * @param aConf The serialized port of TCP server (@see NSHARE::net_address)
	 */
	CTCPServer(NSHARE::CConfig const& aConf);

	/** @brief The destruction  of the object
	 *
	 * During the destruction of the object all clients are
	 * disconnecting.
	 */
	virtual ~CTCPServer();

	/** @brief Changes the TCP port and begins to wait for clients
	 *
	 * @param aParam The port of TCP server
	 * @param aFlags not used
	 *
	 */
	virtual bool MOpen(const net_address& aParam, int aFlags = 0);

	/** @brief  Closes the TCP server
	 *
	 */
	virtual void MClose();

	/** @brief Disconnects specified client
	*
	*	@param aClient A ip address of the client
	*	@return true if disconneceted
	*/
	bool MClose(const client_t& aClient);

	/** @brief Disconnects specified client
	*
	*	@param aIP A ip address of the client
	*	@return true if disconneceted
	*/
	bool MCloseClient(const net_address& aIP);

	/** @brief Disconnects all clients
	 *
	 */
	void MCloseAllClients();

	/** @brief Returns reference to settings of TCP Server
	 *
	 *	@return settings
	 */
	net_address const& MGetSetting() const;

	/** @brief Returns serialized settings of TCP Server
	 *
	 *  @return Serialized object.
	 */
	NSHARE::CConfig MSettings(void) const;

	/**	@brief Send to specified addresses
	 *
	 * @param aTo A send to IP and port
	 * @param pData A pointer to data
	 * @param nSize A size of data
	 * @return information about sent to the address
	 */
	sent_state_t MSend(const void* pData, size_t nSize, const net_address& aTo);

	/**	@brief Send to all connected clietns
	 *
	 * @param pData A pointer to data
	 * @param nSize A size of data
	 * @return information about sent
	 * If during  of sending at least one error is occured then E_ERROR
	 * is returned
	 *
	 */
	virtual sent_state_t MSend(const void* pData, size_t nSize);

	/**	@brief Send to specified addresses
	 *
	 * @param aTo A send to IP and port
	 * @param pData A pointer to data
	 * @param nSize A size of data
	 * @return information about sent to the address
	 */
	virtual sent_state_t MSend(const void* pData, size_t nSize, NSHARE::CConfig  const& aTo);

	/** @brief Force unlocks of the receiving loop
	 *
	 */
	void MForceUnLock();

	bool MIsOpen() const;
	bool MReOpen();

	/** @brief Receives data from all clients
	 *
	 *
	 * @param aFrom [out] From whom the data is received
	 * @param aBuf [out] A pointer to a buffer where can store the message.
	 * @param aTime [in] not used
	 * @return amount of received bytes
	 */
	ssize_t MReceiveData(recvs_t* aFrom, data_t* aBuf, const float aTime);

	/** @brief Receives data from all clients
	 *
	 *
	 * @param aBuf [out] A pointer to a buffer where can store the message.
	 * @param aTime [in] not used
	 * @return amount of received bytes
	 */
	ssize_t MReceiveData(data_t* aBuf, const float aTime);

	/** @brief Receives data from all clients
	 *
	 *
	 * @param aFrom [out] From whom the data is received
	 * @param aBuf [out] A pointer to a buffer where can store the message.
	 * @param aTime [in] not used
	 * @return amount of received bytes
	 */
	ssize_t MReceiveData(data_t* aBuf, const float aTime, recvs_from_t* aFrom);

	/** Prints to stream state of tcp server port
	 *
	 * @param aStream Print to
	 * @return aStream
	 */
	std::ostream& MPrint(std::ostream& aStream) const;

	/** Returns amount of available bytes to read
	 *
	 * @return amount of bytes
	 */
	size_t MAvailable() const;

	/** Returns true if clients are exist
	 *
	 * @return true if at least one client is exist
	 */
	bool MIsClients() const;

	/** @brief Returns info about connected clients
	 *
	 * @return list of connected clients
	 */
	list_of_clients MGetClientInfo() const;

	const CSocket& MGetSocket(void) const;

	/*!	@brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/** @brief Returns information about receive and send
	 * buffer size
	 *
	 * @return first - the send buffer size \n
	 * 		   second - the receive buffer size
	 */
	std::pair<size_t,size_t> MBufSize()const;

	/** Returns diagnostic information about
	 * socket
	 *
	 * @return reference to object
	 */
	diagnostic_io_t const& MGetDiagnosticState() const;
private:


	struct CImpl;
	struct loop_back_t;
	NSHARE::intrusive_ptr<CImpl> FImpl;
};

} /* namespace NSHARE */
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CTCPServer::client_t& aAddress)
{
	return aStream << aAddress.FAddr << " at " << aAddress.FTime
			<< "(unix time)";
}
}
#endif /* CTCPSERVER_H_ */
