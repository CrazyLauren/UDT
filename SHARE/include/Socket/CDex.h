/*
 * CDex.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 15.10.2014
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef CDEX_H_
#define CDEX_H_

#ifdef HAVE_DEX
#ifndef DEX_SOCKET_EXIST
#	define DEX_SOCKET_EXIST
#endif 
struct tag_DEXBUFFER;
#include <Socket/diagnostic_io_t.h>
namespace NSHARE
{
class CDex: public NSHARE::ISocket
{
public:
	static const NSHARE::CText NAME;
	enum eDirection
	{
		eDef, eIn, eOut
	};
	struct param_t
	{
		static const CText DEX_NAME;
		static const CText DEX_SIZE;
		static const CText DEX_QUANTITY;
		static const CText DEX_IN;
		static const CText DEX_OUT;
		static const CText DEX_DIRECT;

		param_t(NSHARE::CConfig const& aConf);
		param_t();

		bool MIsValid() const;
		CText FName;
		size_t FSize;
		size_t FQuantity;
		eDirection FDirect;
		CConfig MSerialize() const;
	};
	CDex();
	CDex(NSHARE::CConfig const& aConf);
	virtual ~CDex();
	virtual sent_state_t MSend(const void* const aData, std::size_t);
	virtual ssize_t MReceiveData(data_t*, const float aTime);
	const CSocket& MGetSocket(void) const;
	bool MOpen(const param_t&);
	bool MReOpen();
	void MClose();
	bool MIsOpen() const;
	bool MIsAvailable();
	const param_t& MGetParam()const;
	size_t MAvailable() const;
	std::ostream & MPrint(std::ostream & aStream) const;
	NSHARE::CConfig MSerialize() const;
	NSHARE::CConfig MSettings(void) const
	{
		return FParam.MSerialize();
	}
private:
	//Method
	void MInit(void);
	void MInitFields();
	bool MOpen();
	bool MCanReceive();
	bool MCanSend();
	//	bool MStartReceiver();
	//	static void sMReceive(void* aWho, void* aWhat, void* aData);
	//	void MReceive();
	//Field
	tag_DEXBUFFER* FpBuffer;
	CSocket FId;
	param_t FParam;
	diagnostic_io_t FDiagnostic;
	//
	friend std::ostream& operator <<(std::ostream& aStream,
			const param_t& aParam);
	friend class CDexDuplex;
};
class CDexDuplex: public NSHARE::ISocket,public NSHARE::CDenyCopying
{
	NSHARE::CDex* FIn;
	NSHARE::CDex* FOut;
public:
	static const NSHARE::CText NAME;
	CDexDuplex(NSHARE::CDex* aIn, NSHARE::CDex* aOut);
	virtual ~CDexDuplex();
	sent_state_t MSend(const void* const aData, std::size_t);
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
#endif//#ifdef HAVE_DEX
#endif /*CDEX_H_*/
