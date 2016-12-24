/*
 * CSocketFile.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 08.06.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CSOCKETFILE_H_
#define CSOCKETFILE_H_
#include <fstream>
#ifndef FILE_SOCKET_EXIST
#	define FILE_SOCKET_EXIST
#endif
namespace NSHARE
{
class SHARE_EXPORT CSocketFile: public ISocket
{
public:
	static const NSHARE::CText NAME;
	enum eDirection
	{
		eIn, eOut
	};
	struct SHARE_EXPORT param_t
	{
		static const CText FILE_PATH;
		static const CText  FILE_SIZE;
		static const CText  FILE_TIME;
		static const CText  FILE_DIRECT;
		static const CText  FILE_DIRECT_IN;
		static const CText  FILE_DIRECT_OUT;
		param_t(NSHARE::CConfig const& aConf);
		param_t();

		bool MIsValid() const;

		CText FPath;
		size_t FSize;
		size_t FTime; //ms
		eDirection FDirect;

		CConfig MSerialize() const;
	};
	CSocketFile(const param_t& =param_t());
	CSocketFile(NSHARE::CConfig const& aConf);
	virtual ~CSocketFile();
	virtual sent_state_t MSend(const void*  aData, std::size_t);
	virtual ssize_t MReceiveData(data_t*, const float aTime);
	const CSocket& MGetSocket(void) const;
	bool MOpen();
	bool MOpen(const param_t&);
	bool MReOpen();
	void MClose();
	bool MIsOpen() const;
	bool MIsAvailable();
	size_t MAvailable() const;
	param_t const & MGetSetting() const;
	NSHARE::CConfig MSettings(void) const
	{
		return MGetSetting().MSerialize();
	}
	std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
private:
	param_t FParam;
	mutable std::fstream FStream;
	diagnostic_io_t FDiagnostic;
	double FNextTime;
	friend std::ostream& operator <<(std::ostream& aStream,
			const param_t& aParam);
	friend class CSocketFileDuplex;
};
class CSocketFileDuplex: public ISocket, public CDenyCopying
{
	NSHARE::CSocketFile* FIn;
	NSHARE::CSocketFile* FOut;
public:
	static const NSHARE::CText NAME;
	CSocketFileDuplex(NSHARE::CSocketFile* aIn, NSHARE::CSocketFile* aOut);
	virtual ~CSocketFileDuplex();
	sent_state_t MSend(const void*  aData, std::size_t);
	ssize_t MReceiveData(data_t*, const float aTime);
	const CSocket& MGetSocket(void) const;
	void MClose();
	bool MIsOpen() const;
	bool MReOpen();
	size_t MAvailable() const;
	std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig MSettings(void) const;
};
}
namespace std{
inline std::ostream& operator<<(std::ostream & aStream,
		NSHARE::CSocketFile::eDirection const& aParam)
{
	switch (aParam)
	{
	case NSHARE::CSocketFile::eIn:
		aStream << NSHARE::CSocketFile::param_t::FILE_DIRECT_IN;

		break;
	case NSHARE::CSocketFile::eOut:
		aStream << NSHARE::CSocketFile::param_t::FILE_DIRECT_OUT;

		break;

	}
	return aStream;
}
}
#endif /* CSOCKETFILE_H_ */
