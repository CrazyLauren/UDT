/*
 * CUnix.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 13.01.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */  
#ifndef CUNIX_H_
#define CUNIX_H_
//FIXME
#if defined (__QNX__) ||defined (unix)

#ifndef UNIX_SOCKET_EXIST
#	define UNIX_SOCKET_EXIST
#endif 
namespace NSHARE
{
class SHARE_EXPORT CUnixDGRAM: public ISocket, NSHARE::CDenyCopying
{
public:
	static const NSHARE::CText NAME;
	struct param_t
	{
		static const CText UNIX_FROM;
		static const CText UNIX_TO;

		param_t(NSHARE::CConfig const& aConf);
		param_t();

		bool MIsValid() const;
		CText FFrom;
		CText FTo;
		CConfig MSerialize() const;
	};
	CUnixDGRAM(NSHARE::CConfig const& aConf);
	CUnixDGRAM(param_t const& =param_t());
	virtual ~CUnixDGRAM();

	typedef int32_t mask_t;
	typedef int32_t flags_t;

	void MSetPath(const param_t&);
	const CText& MGetPathFrom() const;
	const CText& MGetPathTo() const;

	const CSocket& MGetSocket(void) const;

	bool MOpen(const param_t&);
	bool MOpen();
	bool MReOpen();
	bool MIsOpen() const;
	bool MIsOpenFrom() const;
	bool MIsOpenTo() const;
	//bool MSend(const NSHARE::String& aTo,const void* const aData, std::size_t);
	sent_state_t MSend(const void* const aData, std::size_t);

	size_t MAvailable() const;
	ssize_t MReceiveData(data_t*, const float aTime);
	std::ostream& MPrint(std::ostream& aStream) const;
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig MSettings(void) const
	{
		return FParam.MSerialize();
	}
	param_t const& MGetSetting() const
	{
		return FParam;
	}
private:
	class CUnixDGRAM_impl;

	void MClose();
	int MRemove();
	inline CSocket MNewSocket();
	inline int MBindSocket(CSocket const &, struct sockaddr_un const&);
	bool MIsPathExist();
	void MSetAddress();

	CSocket FSockFrom;
	CSocket FSockTo;

	param_t FParam;

	SHARED_PTR<CUnixDGRAM_impl> Impl;
	diagnostic_io_t FDiagnostic;
};
}
#endif//#if defined (__QNX__) ||defined (unix)
#endif /* CUNIX_H_ */
