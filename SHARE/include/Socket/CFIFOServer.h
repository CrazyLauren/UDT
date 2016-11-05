/*
 * CFIFOServer.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 22.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CFIFOSERVER_H_
#define CFIFOSERVER_H_

namespace NSHARE
{
class SHARE_EXPORT CFIFOServer: public ISocket, CDenyCopying
{
public:
	static const NSHARE::CText NAME;
	static const unsigned ATOMIC_FIFO_BUUFER;
	enum eFlush
	{
		FLUSH_IN = 0x1 << 0, FLUSH_OUT = 0x1 << 1,
	};
	typedef CFifo::path_t path_t;
	typedef std::vector<path_t> clients_t;
	struct SHARE_EXPORT server_setting_t
	{
		static const CText PATH;
		static const CText CLIENT;

		server_setting_t();
		server_setting_t(NSHARE::CConfig const& aConf);

		bool MIsValid() const
		{
			return !FPath.empty();
		}
		CConfig MSerialize() const;

		path_t FPath;
		clients_t FClients;
	};
	struct SHARE_EXPORT client_t
	{
		static const CText PATH;
		static const CText TIME;
		client_t() ;
		client_t(NSHARE::CConfig const&);

		NSHARE::CConfig MSerialize() const;
		bool MIsValid() const
		{
			return !FClient.empty();
		}
		std::ostream & MPrint(std::ostream & aStream) const
		{
			return aStream << FClient << " at " << FTime << "(unix time)";
		}
		bool operator==(CFifo::path_t const& aRht) const
		{
			return aRht == FClient;
		}

		path_t FClient;
		time_t FTime;
	};
	CFIFOServer(NSHARE::CConfig const& aConf);
	CFIFOServer(server_setting_t const& aPath = server_setting_t());
	virtual ~CFIFOServer();

	virtual void MSetUP(server_setting_t const& aPath);
	virtual server_setting_t const& MGetSetting() const;
	virtual NSHARE::CConfig MSettings(void) const
	{
		return MGetSetting().MSerialize();
	}
	virtual bool MOpen(path_t const& aClientPath);

	virtual bool MReOpen();
	virtual bool MIsOpen() const;
	virtual void MClose();
	void MClose(path_t const& aClientPath);
	virtual size_t MAvailable() const;

	virtual ssize_t MReceiveData(data_t *, float const aTime,
			recvs_from_t *aFrom);
	virtual ssize_t MReceiveData(data_t * aData, float const aTime)
	{
		return MReceiveData(aData, aTime, NULL);
	}
	sent_state_t MSend(void const*  aData, std::size_t, CFifo::path_t const&);
	sent_state_t MSend(const data_t&, CFifo::path_t const&);
	virtual sent_state_t MSend(void const*  aData, std::size_t);
	virtual sent_state_t MSend(void const*  aData, std::size_t,
			NSHARE::CConfig const& aTo);

	const CSocket& MGetSocket() const;

	virtual void MFlush(eFlush const&);

	virtual size_t MOpen(clients_t const& aClientsPath)
	{
		size_t _result = 0;
		for (clients_t::const_iterator _it = aClientsPath.begin();
				_it != aClientsPath.end(); ++_it)
			_result += MOpen(*_it);
		return _result;
	}
	virtual std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
	bool MIsClient(path_t const&) const;
	//bool MIsClient(slave_t const&) const;
protected:
	class CImpl;
private:
	CImpl *FImpl;
	server_setting_t FSetting;
	diagnostic_io_t FDiagnostic;
};
} //namespace
namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CFIFOServer::server_setting_t& aClient)
{
	using namespace NSHARE;
	if (!aClient.MIsValid())
		return aStream << "Setting is not valid";

	aStream << "Server Path:" << aClient.FPath << " Clients:";
	CFIFOServer::clients_t::const_iterator _it = aClient.FClients.begin();
	for (; !aClient.FClients.empty();)
	{
		aStream << _it.operator *();
		if (++_it == aClient.FClients.end())
			break;
		aStream << ", ";
	}
	return aStream;
}
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CFIFOServer::client_t& aClient)
{
	using namespace NSHARE;
	aClient.MPrint(aStream);
	return aStream;
}

}

#endif /* CFIFOSERVER_H_ */
