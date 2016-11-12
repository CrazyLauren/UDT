/*
 * CTCPServer.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 05.11.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
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

class SHARE_EXPORT CTCPServer: public ISocket, NSHARE::CDenyCopying, public NSHARE::CEvents<
		NSHARE::CText>
{
public:
	static const NSHARE::CText NAME;
	static version_t sMGetVersion();

	typedef NSHARE::CEvents<NSHARE::CText> events_t;
	struct SHARE_EXPORT client_t
	{
		net_address FAddr;
		time_t FTime;
		bool operator==(client_t const& aRht) const;
		bool operator==(net_address const& aRht) const;
	};
	struct  recv_t
	{
		client_t FClient;
		data_t::iterator FBufBegin;
		data_t::difference_type FSize;
	};
	typedef std::vector<recv_t> recvs_t;

	//arg event type is client_t
	static events_t::key_t const EVENT_CONNECTED;
	static events_t::key_t const EVENT_DISCONNECTED;
	//static long const DEF_TCP_BUF_SIZE;

	CTCPServer(const net_address& aParam = net_address());
	CTCPServer(NSHARE::CConfig const& aConf);
	virtual ~CTCPServer();
	virtual bool MOpen(const net_address& aParam, int aFlags = 0);

	virtual void MClose();
	void MCloseAllClients();
	bool MCloseClient(const net_address&);

	bool MGetInitParam(net_address*) const;//deprecated using MGetSetting
	net_address MGetSetting() const;
	NSHARE::CConfig MSettings(void) const
	{
		net_address _addr;
		MGetInitParam(&_addr);
		return _addr.MSerialize();
	}
	sent_state_t MSend(const void* pData, size_t nSize, const net_address&);
	virtual sent_state_t MSend(const void* pData, size_t nSize);
	virtual sent_state_t MSend(const void* pData, size_t nSize, NSHARE::CConfig  const& aTo);
	sent_state_t MSendLoopBack(data_t const& aData);

	bool MIsOpen() const;
	bool MReOpen();

	ssize_t MReceiveData(recvs_t*, data_t*, const float aTime);
	ssize_t MReceiveData(data_t* aData, const float aTime)
	{
		return MReceiveData(NULL, aData, aTime);
	}
	ssize_t MReceiveData(data_t*, const float aTime, recvs_from_t* aFrom);

	std::ostream& MPrint(std::ostream& aStream) const;
	size_t MAvailable() const;
	//size_t MAvailable(CSocket const&) const;

	bool MCanReceive() const;
	bool MIsClients() const;
	bool MIsConnected() const
	{
		return MIsClients();
	}
	const CSocket& MGetSocket(void) const;
	NSHARE::CConfig MSerialize() const;
	std::pair<size_t,size_t> MBufSize()const;//first send
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
