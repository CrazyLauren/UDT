/*
 * CSocket.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 05.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CSOCKET_H_
#define CSOCKET_H_

namespace NSHARE
{
enum eSocketType
{
	E_READ_ONLY = 0x1 << 0, E_WRITE_ONLY = 0x1 << 1,
};
class SHARE_EXPORT CSocket: CFlags<eSocketType, unsigned>
{

public:
#ifdef _WIN32
	typedef long int socket_t;
#else
	typedef int socket_t;
#endif
	typedef CFlags<eSocketType, unsigned> CSocketFlags;

	using CSocketFlags::MGetFlag;
	using CSocketFlags::MGetMask;

	CSocket(socket_t aFd = -1,unsigned aFlags=E_READ_ONLY|E_WRITE_ONLY);
	CSocket(CSocket const& aRht);
	CSocket& operator=(CSocket const&aRht);
	CSocket& operator=(socket_t aFd);
	virtual ~CSocket();
	static bool sMIsValid(socket_t aFd);
	bool MIsValid() const;

	socket_t MGet(void) const;

	void MSet(socket_t aFd, eSocketType aType);
	virtual void MClose();

	bool operator<(CSocket const& aRht) const
	{
		return FFd < aRht.FFd;
	}
	bool operator==(CSocket const& aRht) const
	{
		return FFd == aRht.FFd;
	}
	bool operator!=(CSocket const& aRht) const
	{
		return !operator==(aRht);
	}
	operator socket_t()
	{
		return FFd;
	}
	bool FIsCloseOnDestructor;
	unsigned const FOrderNumber;
private:
	socket_t FFd;

	static unsigned g_counter;
};
}
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CSocket& aSocket)
{
	if(!aSocket.MIsValid())
	{
		aStream<<"Invalid socket";
		return aStream;
	}
	aStream << "Id=" << aSocket.MGet() << ", Order Number="
			<< aSocket.FOrderNumber<<", Type:";
	if (aSocket.MGetFlag(NSHARE::E_READ_ONLY)
			&& aSocket.MGetFlag(NSHARE::E_WRITE_ONLY))
		aStream << "RW";
	else if (aSocket.MGetFlag(NSHARE::E_READ_ONLY))
		aStream << "RO";
	else if (aSocket.MGetFlag(NSHARE::E_WRITE_ONLY))
		aStream << "WO";
	else
		aStream << "Invalid";
	return aStream ;
}
}

#endif /* CSOCKET_H_ */
