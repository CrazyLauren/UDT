/*
 * CTCP.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 04.03.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CTCP_H_
#define CTCP_H_

#ifndef TCP_SOCKET_EXIST
#	define TCP_SOCKET_EXIST
#endif 
#include <share/Socket/socket_setting_t.h>

namespace NSHARE
{
/** @brief TCP client
 *
 * It try connection during operation receive
 * Thus, until the operation receive isn't called
 * TCP client will not connected to server.
 * When the client is connected to the server
 * the event #CTCP::EVENT_CONNECTED is called, respectively
 * When it is disconnected the event #CTCP::EVENT_DISCONNECTED
 * is called. The both events are called in thread of the receive operation
 * If IP is empty the TCP client tries to connect by localhost.
 */
class SHARE_EXPORT CTCP: public ISocket, NSHARE::CDenyCopying, public NSHARE::CEvents<
		NSHARE::CText>
{
public:
	static const NSHARE::CText NAME;///< A serialization key

	/** @brief Settings of TCP socket
	 *
	 */
	struct SHARE_EXPORT settings_t
	{
		static const CText NAME;///< A serialization name


		/** @brief To constructor may be passed to be copied
		 * a server address
		 *
		 * @param aParam The address where the data is sent
		 */
		explicit settings_t(net_address const& aParam =net_address());

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

		net_address  FServerAddress;///< The server IP address
		socket_setting_t FSocketSetting;///< The socket setting
	};

	typedef NSHARE::CEvents<NSHARE::CText> events_t;///< A type of events used
	static events_t::key_t const EVENT_CONNECTED;/*!< It event is called when client is connected to server.
													* The argument of the event is pointer to the
													* NSHARE::net_address object.
													*/
	static events_t::key_t const EVENT_DISCONNECTED;/*!< It event is called when client is disconnected to server.
													* The argument of the event is pointer to the
													* NSHARE::net_address object.
													*/

	/** @brief Create TCP client
	 *
	 * If IP server isn't valid then it TCP client has to be opened (MOpen()).
	 *
	 *	@param aParam the IP address of server
	 */
	CTCP(net_address const& aParam);

	/** @brief Create TCP client
	 *
	 * If IP server isn't valid then it TCP client has to be opened (MOpen()).
	 *
	 *	@param aParam the IP address of server
	 */
	CTCP(settings_t const& aParam = settings_t());

	/** @brief Create TCP client
	 *
	 * If IP server is valid then it opens TCP client too.
	 *
	 *	@param aParam the serialized  IP address of server
	 */
	CTCP(NSHARE::CConfig const& aConf);

	/** @brief Disconnect from server and destruction
	 * of the object
	 *
	 */
	virtual ~CTCP();

	/** @brief Open TCP client with specified TCP server IP address
	 *
	 *	@brief aIP The TCP server IP
	 *	@return true if open
	 */
	bool MOpen(net_address const& aIP);


	/** @brief Open TCP client with specified in to settings_t server IP
	 *
	 *	@brief aIP The TCP server IP
	 *	@return true if open
	 */
	bool MOpen();

	/** @brief Force try to connect
	 *
	 *	@return true if connected
	 */
	bool MConnect();

	/** @brief Close client
	 *
	 */
	void MClose();

	/** Returns reference to TCP client setting
	 *
	 */
	settings_t const& MGetSetting() const;

	/** Returns serialized TCP client setting
	 *
	 */
	NSHARE::CConfig MSettings(void) const;

	sent_state_t MSend(void const* pData, size_t nSize);
	sent_state_t MSend(void const*  aData, std::size_t aSize,
			NSHARE::CConfig  const& aTo)
	{
		return MSend(aData, aSize);
	}
	sent_state_t MSend(data_t const& aVal)
	{
		return ISocket::MSend(aVal);
	}
	sent_state_t MSend(data_t const& aVal, NSHARE::CConfig  const& aTo)
	{
		return MSend(aVal.ptr_const(),aVal.size(),aTo);
	}

	/** @brief Returns true if TCP client is opened
	 *
	 */
	bool MIsOpen() const;

	/** @brief Returns true if TCP client is connected to server
	 *
	 */
	bool MIsConnected() const;

	bool MReOpen();
	const CSocket& MGetSocket(void) const;

	ssize_t MReceiveData(data_t *, float const aTime);

	/** Prints to stream state of tcp client port
	 *
	 * @param aStream Print to
	 * @return aStream
	 */
	std::ostream & MPrint(std::ostream & aStream) const;

	/*!	@brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/** Returns amount of available bytes to read
	 *
	 * @return amount of bytes
	 */
	size_t MAvailable() const;

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
	struct CClientImpl;
	//Method
	//Field
	NSHARE::intrusive_ptr<CClientImpl> FClientImpl;

};
} //namespace USHARE
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream, NSHARE::CTCP::settings_t const& aVal)
{
	using namespace NSHARE;

	aStream << "Connect to:"<<aVal.FServerAddress<<std::endl;
	aStream << "Socket:"<<aVal.FSocketSetting<<std::endl;

	return aStream;
}
}
#endif /* CTCP_H_ */
