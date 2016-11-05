/*
 * CFifo.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 12.02.2014
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CFIFO_H_
#define CFIFO_H_

#ifndef PIPE_SOCKET_EXIST
#	define PIPE_SOCKET_EXIST
#endif 
namespace NSHARE
{
class SHARE_EXPORT CFifo: public ISocket
{
public:
	static const NSHARE::CText NAME;
	static const unsigned ATOMIC_FIFO_BUUFER;
	enum eFlush
	{
		FLUSH_IN = 0x1 << 0, FLUSH_OUT = 0x1 << 1,
	};
	enum eType
	{
		READ_ONLY = 0, WRITE_ONLY, READ_WRITE,
	};
	typedef CText path_t;
	struct SHARE_EXPORT setting_t
	{
		static const CText FIFO_SERVER_PATH;
		static const CText FIFO_PATH;
		static const CText FIFO_TYPE;
		static const CText FIFO_R_ONLY;
		static const CText FIFO_W_ONLY;
		static const CText FIFO_RW;

		setting_t();
		setting_t(NSHARE::CConfig const& aConf);

		bool operator==(setting_t const& aRht) const
		{
			return FPath == aRht.FPath;
		}
		bool operator<(setting_t const& aRht) const
		{
			return FPath < aRht.FPath;
		}
		bool MIsValid() const
		{
			return !FPath.empty();
		}
		CConfig MSerialize() const;

		path_t FPath;
		path_t FServerPath;
		eType FType;
	};
	typedef int32_t mask_t;

	bool MMakefifo(path_t const&);

	CFifo(setting_t const& = setting_t());
	CFifo(NSHARE::CConfig const& aConf);
	virtual ~CFifo();

	virtual void MSetUP(setting_t const& aPath);
	setting_t const& MGetSetting() const;
	virtual NSHARE::CConfig MSettings(void) const
	{
		return MGetSetting().MSerialize();
	}
	bool MOpen();
	bool MReOpen();
	bool MIsOpen() const;
	void MClose();
	bool MIsMade() const;

	virtual size_t MAvailable() const;
	ssize_t MReceiveData(data_t*, const float aTime);
	using ISocket::MSend;
	sent_state_t MSend(const void*  aData, std::size_t);

	virtual void MFlush(eFlush const&);

	const CSocket& MGetSocket(void) const;
	std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
protected:
	class CImpl;
private:
	CImpl *FImpl;
	setting_t FSetting;
	diagnostic_io_t FDiagnostic;
};
} //namespace NSHARE

namespace std
{
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CFifo::eType& aClient)
{
	switch (aClient)
	{
		case NSHARE::CFifo::READ_ONLY:
			aStream << NSHARE::CFifo::setting_t::FIFO_R_ONLY;
			break;

		case NSHARE::CFifo::WRITE_ONLY:
			aStream << NSHARE::CFifo::setting_t::FIFO_W_ONLY;
			break;

		case NSHARE::CFifo::READ_WRITE:
			aStream << NSHARE::CFifo::setting_t::FIFO_RW;
			break;
	};
	return aStream;
}
inline std::ostream& operator <<(std::ostream& aStream,
		const NSHARE::CFifo::setting_t& aClient)
{
	using namespace NSHARE;
	if (!aClient.MIsValid())
		return aStream << "Setting is not valid";

	aStream << "Path:" << aClient.FPath<<" Server Path"<<aClient.FServerPath << ":"<<aClient.FType;

	return aStream<<".";
}
}
#endif /* CFIFO_H_ */
