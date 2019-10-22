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

#include "CSelectSocket.h"

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
 *	If the port number is zero than the random free port number is selected
 */
class SHARE_EXPORT CTCPServer: public ISocket, NSHARE::CDenyCopying, public NSHARE::CEvents<
		NSHARE::CText>
{
public:
	static const NSHARE::CText NAME;///< A serialization key
	static version_t sMGetVersion();///< The version of TCP server class

	typedef NSHARE::CEvents<NSHARE::CText> events_t;///< A type of events used

	/** @brief Client information
	 *
	 */
	struct SHARE_EXPORT client_t
	{
		static const CText NAME; ///< A serialization name
		static const CText KEY_CONNECTION_TIME; ///< A serialization key of #FTime
		static const CText KEY_DISCONNECT_TIME; ///< A serialization key of #FDiconnectTime

		net_address FAddr;///< A network address of client
		double FTime;///< The connection time
		double FDiconnectTime;///< The disconnection time
		mutable diagnostic_io_t FDiagnostic;///< Diagnostic info

		/** Create the new object.
		 *
		 * The client network address can be passed to
		 * the  object constructor.
		 * If IP is valid, the current time is passed to #FTime field.
		 *
		 * @param aAddr A net address of the client
		 *
		 */
		explicit client_t(net_address const & aAddr=net_address());

		/*! @brief Deserialize object
		 *
		 * To check the result of deserialization,
		 * used the MIsValid().
		 * @param aConf Serialized object
		 */
		explicit client_t(NSHARE::CConfig const& aConf);

		/*! @brief Serialize object
		 *
		 * The key of serialized object is #NAME
		 *
		 * @return Serialized object.
		 */
		CConfig MSerialize() const;


		/*! @brief Checks object for valid
		 *
		 * Usually It's used after deserializing object
		 * @return true if it's valid.
		 */
		bool MIsValid() const;

		bool operator==(client_t const& aRht) const;
		bool operator==(net_address const& aRht) const;
	};
	typedef std::vector<client_t> list_of_clients;///< List of clients

	/** @brief Settings of TCP server
	 *
	 */
	struct SHARE_EXPORT settings_t
	{
		static const unsigned DEFAULT_LIST_QUEUE_LEN;///<default value of #FListenQueue
		static const CText NAME; ///< A serialization name
		static const CText KEY_LIST_QUEUE_LEN; ///< A serialization key of #FListenQueue

		/** @brief To constructor may be passed to be copied
		 * a server address
		 *
		 * @param aParam The address where the data is sent
		 */
		explicit settings_t(net_address const& aParam);

		/** @brief To constructor may be passed to be copied
		 * a server port
		 *
		 * @param aParam The port where the data is sent
		 */
		explicit settings_t(network_port_t const& aParam =network_port_t());

		/*! @brief Deserialize object
		 *
		 * To check the result of deserialization,
		 * used the MIsValid().
		 * @param aConf Serialized object
		 */
		explicit settings_t(NSHARE::CConfig const& aConf);

		/*! @brief Serialize object
		 *
		 * The key of serialized object is #NAME
		 *
		 * @return Serialized object.
		 */
		CConfig MSerialize() const;

		/*! @brief Checks object for valid
		 *
		 * Usually It's used after deserializing object
		 * @return true if it's valid.
		 */
		bool MIsValid() const;

		net_address FServerAddress; ///< The server IP address
		socket_setting_t FSocketSetting;///< The socket setting
		unsigned FListenQueue;///<The maximum length that the queue of pending connections may grow to
	};

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
	static events_t::key_t const EVENT_DISCONNECTED;/*!< It event is called when client is disconnected.
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

	/** @brief Changes the TCP port
	 *
	 * @param aParam The port of TCP server
	 *
	 * @return true if successfully
	 */
	bool MSetAddress(const net_address& aParam);

	/** @brief  Closes the TCP server
	 *
	 */
	virtual void MClose();

	/** Returns true if clients are exist
	 *
	 * @return true if at least one client is exist
	 */
	bool MIsClients() const;

	/** Returns true if clients are exist
	 *
	 * @return true if at least one client is exist
	 */
	bool MIsClient(const net_address& aIP) const;


	/** @brief Returns info about connected clients
	 *
	 * @return list of connected clients
	 */
	list_of_clients MGetClientInfo() const;

	/** @brief Returns info about disconnected clients
	 *
	 * @return list of connected clients
	 */
	list_of_clients MGetDisconnectedClientInfo() const;

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
	settings_t const& MGetSetting() const;

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
	 * @param aTo A send to IP and port or serialized object type of CTCPServer::client_t
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
inline std::ostream& operator<<(std::ostream & aStream, NSHARE::CTCPServer::settings_t const& aVal)
{
	using namespace NSHARE;

	aStream << "Connect to:"<<aVal.FServerAddress<<std::endl;
	aStream << "Socket:"<<aVal.FSocketSetting<<std::endl;
	aStream << "Listen Queue:"<<aVal.FListenQueue<<std::endl;

	return aStream;
}
}
#endif /* CTCPSERVER_H_ */
