/*
 * CUDP.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.03.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CUDP_H_
#define CUDP_H_

#ifndef UDP_SOCKET_EXIST
#	define UDP_SOCKET_EXIST
#endif 
#include <SHARE/Socket/socket_setting_t.h>

namespace NSHARE
{
/** @brief UDP socket
 *
 *	@todo multicast udp
 */
class SHARE_EXPORT CUDP: public ISocket, NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;///<A serialization key

	/** @brief Settings of udp socket
	 *
	 * @note If port is zero it will be generated automatically
	 */
	struct SHARE_EXPORT settings_t
	{
		static const CText NAME;///< A serialization name
		static const CText UDP_PORT;///<A key of type #FPort
		static const CText ADDR_TO;///<A key of type #FSendTo
		static const CText TYPE;///< A key of type #FType
		static const CText TYPE_UNICAST;///< A key of type #eUNICAST
		static const CText TYPE_BROADCAST;///< A key of type #eBROADCAST
		static const CText TYPE_MULTICAST;///< A key of type #eMULTICAST
		static const CText REMOVE_INVALID_SEND_IP;///< A key of type #E_REMOVE_INVALID_SEND_IP
		static const CText REPLACE_FULL_BROADCAST_ADDR;///< A key of type #E_REPLACE_FULL_BROADCAST_ADDR
		static const CText RECEIVE_FROM;///< A key of type #FReceiveFrom

		/** Type of socket
		 *
		 */
		enum eType
		{
			eUNICAST,///< A normal udp socket
			eBROADCAST,///< A broadcasr UDP socket
			eMULTICAST,///< A multicast UDP socket
		};

		/** Flags
		 *
		 */
		enum eFlags
		{
			E_REMOVE_INVALID_SEND_IP=0x1<<0, ///< If set invalid send ip will be ignored
			E_REPLACE_FULL_BROADCAST_ADDR=0x1<<1, /*!< If set when full broadcast address 255.255.255.255
											* will replace to broadcast of
											* network interfaces
											* */

			E_DEFAULT_FLAGS=E_REMOVE_INVALID_SEND_IP|E_REPLACE_FULL_BROADCAST_ADDR,
		};

		typedef CFlags<eFlags> flags_t;///< type of flags

		/** A default constructor
		 *
		 */
		settings_t();

		/** @brief To constructor may be passed to be copied
		* a port number, a send to address,  and flags
		*
		* @param aPort A listened udp port
		* @param aParam The address where the data is sent or multicast address(see #eMULTICAST)
		* @param aType A type of port
		*/
		explicit settings_t(network_port_t aPort, net_address const& aParam=net_address(),eType const& aType =eUNICAST);

		/** @brief Adds a new address to send list
		 *
		 * @param aNewAddress a new address
		 * @return true if no error
		 */
		bool MAddSendAddr(net_address aNewAddress);

		/** @brief Removes the address from send list
		 *
		 * @param aAddress the removed address
		 * @return true if removed
		 */
		bool MRemoveSendAddr(net_address const& aAddress);

		/** @brief Adds a new ip address to receive list
		 *
		 * @param aNewAddress a new address
		 * @return true if no error
		 */
		bool MAddReceiveAddr(network_ip_t const& aNewAddress);

		/** @brief Receive only from passed IP
		 *
		 * @param aNewAddress receive only from with IP
		 * @return true if no error
		 */
		bool MSetReceiveAddr(network_ip_t const& aNewAddress);

		/** @brief Removes the ip address from receive list
		 *
		 * @param aAddress the removed address
		 * @return true if removed
		 */
		bool MRemoveReceiveAddr(network_ip_t const& aAddress);

		/*! @brief Deserialize object
		 *
		 * To check the result of deserialization,
		 * used the MIsValid().
		 * @param aConf Serialized object
		 */
		settings_t(NSHARE::CConfig const& aConf);

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

		network_ips_t FReceiveFrom;///< From whom receive the messages
		network_port_t FPort;///< A UDP port number
		net_addresses_t  FSendTo;///< Send to address
		eType FType;///< A type of socket
		flags_t FFlags;///< Flags see #eFlags
		socket_setting_t FSocketSetting;///< Socket setting of course flag E_SET_BUF_SIZE is not used

	};
	typedef network_ips_t list_of_broadcast_addr_t;///< info about broadcast addresses of interfaces

	/*!\brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 *\param aConf Serialized object
	 */
	CUDP(NSHARE::CConfig const& aConf);

	/** The default constructor
	 *
	 * @param aParam A socket parameters
	 * @warning if aParam is valid the socket will be opened
	 */
	CUDP(const settings_t& aParam= settings_t());

	/** The default desconstructor
	 *
	 * A socket closed
	 */
	virtual ~CUDP();

	/** @brief Set up the UDP socket
	 *
	 *	@param aParam A parameters
	 *	@return true if no error
	 */
	bool MSettingUp(const settings_t& aParam);

	/** @brief Sets up port and opens it
	 *
	 *	@param aParam A parameters
	 *	@return true if no error
	 */
	bool MOpen(const settings_t& aParam);

	/** @brief Opens port
	 *
	 * 	@return true if no error
	 */
	bool MOpen();

	/** @brief Adds address for send or remove send address
	 *
	 *
	 * @param aAddress An address for send
	 * @return true if no error
	 */
	bool MAddSendAddress(net_address const& aAddress);

	/** @brief Set uniquie send address
	 *
	 *
	 * @param aAddress An address for send
	 * @return true if no error
	 */
	bool MSetSendAddress(net_address const& aAddress);

	/** @brief Removes address for send
	 *
	 *
	 * @param aAddress An address for send
	 * @return true if no error
	 */
	bool MRemoveSendAddress(net_address const& aAddress);

	/** @brief Removes all send addresses
	 *
	 * @return true if no error
	 */
	bool MCleanSendAddress();

	bool MReOpen();
	bool MIsOpen() const;

	void MClose();
	const CSocket& MGetSocket(void) const;

	/**	@brief Send to addresses which specified in #settings_t::FSendTo
	 * field
	 *
	 * @param pData A pointer to data
	 * @param nSize A size of data
	 * @return information about sent to one address
	 * @note #settings_t::FSendTo if field is contained more than one address
	 * 		return send state of last address
	 */
	sent_state_t MSend(void const* pData, size_t nSize);

	/**	@brief Send to specified addresses
	 *
	 * @param aTo A send to IP and port
	 * @param pData A pointer to data
	 * @param nSize A size of data
	 * @return information about sent to the address
	 * @warning if is sending to broadcast address(.255) and
	 * 			socket type is not setting_t::eBROADCAST the data can be not send
	 */
	sent_state_t MSend(void const* pData, size_t nSize,NSHARE::CConfig  const& aTo);

	/**	@brief Send to specified addresses
	 *
	 * @param aTo A send to IP and port
	 * @param pData A pointer to data
	 * @param nSize A size of data
	 * @return information about sent to the address
	 * @warning if is sending to broadcast address(.255) and
	 * 			socket type is not setting_t::eBROADCAST the data can be not send
	 */
	sent_state_t MSend(void const* pData, size_t nSize, net_address const& aTo);

	/**	@brief Send to specified addresses
	 *
	 * @param aTo A send to IP and port
	 * @param pData A pointer to data
	 * @param nSize A size of data
	 * @return information about sent to the address
	 * @warning if is sending to broadcast address(.255) and
	 * 			socket type is not setting_t::eBROADCAST the data can be not send
	 */
	sent_state_t MSend(void const* pData, size_t nSize,	struct sockaddr_in const& aAddress);


	/**	@brief Send to addresses which specified in #settings_t::FSendTo
	 * field
	 *
	 * @param aVal A data
	 * @return information about sent to one address
	 * @note #settings_t::FSendTo if field is contained more than one address
	 * 		return send state of last address
	 */
	sent_state_t MSend(data_t const& aVal)
	{
		return ISocket::MSend(aVal);
	}
	/**	@brief Send to specified addresses
	 *
	 * @param aTo A send to IP and port
	 * @param aVal A data
	 * @return information about sent to the address
	 * @warning if is sending to broadcast address(.255) and
	 * 			socket type is not setting_t::eBROADCAST the data can be not send
	 */
	sent_state_t MSend(data_t const& aVal, NSHARE::CConfig  const& aTo)
	{
		return MSend(aVal.ptr_const(),aVal.size(),aTo);
	}
	/**	@brief Send to specified addresses
	 *
	 * @param aTo A send to IP and port
	 * @param aVal A data
	 * @return information about sent to the address
	 * @warning if is sending to broadcast address(.255) and
	 * 			socket type is not setting_t::eBROADCAST the data can be not send
	 */
	sent_state_t MSend(data_t const& aVal, net_address const& aTo)
	{
		return MSend(aVal.ptr_const(),aVal.size(),aTo);
	}

	/** @brief Receives message
	 *
	 * @param aFrom [out] From whom the data is received
	 * @param aBuf [out] A pointer to a buffer where can store the message.
	 * @param aTime [in] not used
	 * @return amount of received bytes
	 */
	ssize_t MReceiveData(net_address* aFrom,data_t *aBuf, float const aTime);

	/** @brief Receives message
	 *
	 * @param aBuf [out] A pointer to a buffer where can store the message.
	 * @param aTime [in] not used
	 * @return amount of received bytes
	 */
	ssize_t MReceiveData(data_t *aBuf, float const aTime)
	{
		return MReceiveData(NULL,aBuf, aTime);
	}

	/** Returns reference to current settings
	 *
	 * @return settings
	 */
	settings_t const& MGetSetting() const;

	/** Returns port number
	 *
	 * @return port number
	 */
	network_port_t MGetPort() const;

	/** Prints to stream state of udp port
	 *
	 * @param aStream Print to
	 * @return aStream
	 */
	std::ostream & MPrint(std::ostream & aStream) const;

	/** Returns amount of available bytes to read
	 *
	 * @return amount of bytes
	 */
	size_t MAvailable() const;

	/*!@brief Serialize object
	 *
	 * The key of serialized object is #NAME
	 *
	 * @return Serialized object.
	 */
	NSHARE::CConfig MSerialize() const;

	/*!@brief Returns current settings
	 *
	 * @return Serialized object.
	 */
	virtual NSHARE::CConfig MSettings(void) const;

	/** Returns list of broadcast addresses
	 *
	 * @return list of broadcast addresses
	 */
	static list_of_broadcast_addr_t sMGetBroadcast();

	/** Returns diagnostic information about
	 * socket
	 *
	 * @return reference to object
	 */
	diagnostic_io_t const& MGetDiagnosticState() const;
private:

	typedef	std::vector<struct sockaddr_in> send_to_t;



	sent_state_t MSendImpl(void const* pData, size_t nSize,
			struct sockaddr_in const& aAddress);
	void MUpdateSendList();
	bool MAddBroadcastAddresses();

	bool MRemoveInvalidBroadcastAddresses();
	unsigned MAddBroadcastInterfaceAddresses();

	CSocket FSock;///<The socket of udp
	send_to_t FSendToAddr;///< Where the data is sent by default
	diagnostic_io_t FDiagnostic;///<Some diagnostic parameters
	settings_t FParam;///<A current settings of port
	size_t FMaxSendSize;///< Max send buffer size
	CMutex FReceiveThreadMutex; ///<It's used for lock receive operation
	atomic_t FIsWorking;///< true if UDP is working
	atomic_t FIsReceiving;///< If true then object is locked on recvfrom (see #MClose)
};
} //namespace USHARE
namespace std
{
inline std::ostream& operator<<(std::ostream & aStream, NSHARE::CUDP::settings_t::eType const& aVal)
{
	using namespace NSHARE;
	switch (aVal)
	{
	case CUDP::settings_t::eUNICAST:
		aStream<<CUDP::settings_t::TYPE_UNICAST;
		break;
	case CUDP::settings_t::eBROADCAST:
		aStream<<CUDP::settings_t::TYPE_BROADCAST;
		break;
	case CUDP::settings_t::eMULTICAST:
		aStream<<CUDP::settings_t::TYPE_MULTICAST;
		break;
	}
	return aStream;
}
inline std::ostream& operator<<(std::ostream & aStream, NSHARE::CUDP::settings_t const& aVal)
{
	using namespace NSHARE;

	aStream << "listen port:"<<aVal.FPort<<std::endl;
	aStream << "Type:"<<aVal.FType<<std::endl;
	aStream << "Flags:"<<aVal.FFlags<<std::endl;
	aStream << "Send to:"<<aVal.FSendTo<<std::endl;
	aStream << "Receive from:"<<aVal.FReceiveFrom<<std::endl;
	aStream << "Socket:"<<aVal.FSocketSetting<<std::endl;

	return aStream;
}
}
#endif /* CUDP_H_ */
