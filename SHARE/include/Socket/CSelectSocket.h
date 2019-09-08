/*
 * CSelectSocket.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CSELECTSOCKET_H_
#define CSELECTSOCKET_H_

namespace NSHARE
{
/** Class for check for sockets that are ready for reading or writing
 *
 */
class SHARE_EXPORT CSelectSocket: CDenyCopying
{
public:
	typedef std::vector<CSocket> socks_t;///< The list of sockets

	CSelectSocket();
	virtual ~CSelectSocket();

	/** @brief Add the new socket to select list
	 *
	 * 	@param aSocket a Socket
	 */
	void MAddSocket(CSocket const& aSocket);

	/** @brief Remove the new socket to select list
	*
	* 	@param aSocket The Socket
	*/
	void MRemoveSocket(CSocket const& aSocket);

	/** @brief Removes all sockets
	 *
	 */
	void MRemoveAll();

	/** @brief Wait for socket is ready for
	 *
	 *	If setup flags E_READ_ONLY it waits for socket selected to read. \n
	 *	If setup flags E_WRITE_ONLY it waits for socket selected to read. \n
	 *	If both then it waits for socket selected to read and write
	 *	(In this case, the socket ready:
	 *	 @li for read is passed to aTo
	 *	 @li for write is passed to aToWrite
	 *	 )
	 *
	 *
	 * @param aTo [out] The sockets which is ready
	 * @param aToWrite [out] The sockets which is ready for write only()
	 * @param aTime  timeout
	 * @param aType  - ready for
	 * @return 0 - if timeout,
	 * 			>0 - the number of selected sockets
	 * 			<0 - error
	 */
	int MWaitData(socks_t& aTo, float const aTime, unsigned aType =
			E_READ_ONLY,socks_t* aToWrite=NULL) const;

	/** Gets the status of select list
	 *
	 * @return true if exist
	 */
	bool MIsSetUp() const
	{
		return FIsSetUp;
	}

	/** Gets list of sockets
	 *
	 * @return list of sockets
	 */
	const socks_t& MGetSockets()const
	{
		return FFds;
	}
private:
	struct CImpl;

	CImpl *FImpl;
	socks_t FFds;
	bool FIsSetUp;
	NSHARE::CMutex FMutex;
};

} /* namespace NSHARE */
#endif /* CSELECTSOCKET_H_ */
