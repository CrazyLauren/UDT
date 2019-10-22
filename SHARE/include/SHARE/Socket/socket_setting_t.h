/*
 * socket_setting_t.h
 *
 * Copyright © 2019  https://github.com/CrazyLauren
 *
 *  Created on: 07.07.2019
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef SOCKET_SETTING_T_H_
#define SOCKET_SETTING_T_H_

namespace NSHARE
{
/** Settings of socket
 *
 */
struct SHARE_EXPORT socket_setting_t
{
	static long const DEF_BUF_SIZE;///< A default send and receive time
	static unsigned const DEF_KEEPALIVE_TIME;///< A default interval time of keep alive
	static unsigned const DEF_KEEPALIVE_COUNT;///< A default count of keep alive

	static const CText NAME;///< A serialization name
	static const CText KEY_SEND_BUFFER_SIZE;///<A key of type #FSendBufferSize
	static const CText KEY_RECV_BUFFER_SIZE;///<A key of type #FRecvBufferSize
	static const CText KEY_KEEPALIVE_TIME;///<A key of type #FKeepAliveTime
	static const CText KEY_KEEP_ALIVE_COUNT;///<A key of type #FKeepAliveCount

	static const CText ENABLE_KEEPALIVE;///< A key of type #E_KEEPALIVE
	static const CText SET_BUF_SIZE;///< A key of type #E_SET_BUF_SIZE
	static const CText ENABLE_REUSE_PORT;///< A key of type #E_REUSE_PORT


	/** Flags
	 *
	 */
	enum eFlags
	{
		E_KEEPALIVE=0x1<<0, ///< If set the keepalive probes is turned on
		E_SET_BUF_SIZE=0x1<<1, /*!< If set the buffer sizes will set.
								*	The values are #FSendBufferSize.
								*	The values are #FRecvBufferSize. */
		E_REUSE_PORT=0x1<<2,///< If set the port will reuse

		E_DEFAULT_FLAGS=E_KEEPALIVE|E_SET_BUF_SIZE|E_REUSE_PORT,
	};
	typedef CFlags<eFlags> flags_t;///< type of flags

	/** A default constructor
	 *
	 */
	socket_setting_t();

	/*! @brief Deserialize object
	 *
	 * To check the result of deserialization,
	 * used the MIsValid().
	 * @param aConf Serialized object
	 */
	explicit socket_setting_t(NSHARE::CConfig const& aConf);

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


	long FSendBufferSize;///< The maximum send buffer size by default  #DEF_BUF_SIZE
	long FRecvBufferSize;///< The maximum send buffer size by default  1.5* #DEF_BUF_SIZE
	unsigned FKeepAliveTime;///<  Seconds between probe attempts and time before probing starts
	unsigned FKeepAliveCount;///<  The number of keepalive probe attempts
	flags_t FFlags;///< Flags see #eFlags
};
}

namespace std
{
inline std::ostream& operator<<(std::ostream & aStream, NSHARE::socket_setting_t const& aVal)
{
	using namespace NSHARE;
	aStream << "Send buffer:"<<aVal.FSendBufferSize<<std::endl;
	aStream << "Recv buffer:"<<aVal.FRecvBufferSize<<std::endl;
	aStream << "Flags:"<<aVal.FFlags<<std::endl;
	aStream << "Keep alive time:"<<aVal.FKeepAliveTime<<std::endl;
	aStream << "Probe count:"<<aVal.FKeepAliveCount<<std::endl;

	return aStream;
}
}

#endif /* SOCKET_SETTING_T_H_ */
