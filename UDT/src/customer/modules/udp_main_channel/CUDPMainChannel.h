/*
 * CUDPMainChannel.h
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CUDPMAINCHANNEL_H_
#define CUDPMAINCHANNEL_H_

#if  defined( _WIN32 ) && defined(_MSC_VER) && !defined(UDP_MAIN_CHANNEL_STATIC)
#   ifdef UDP_MAIN_CHANNEL_EXPORTS
#       define UDP_MAIN_CHANNEL_EXPORT __declspec(dllexport)
#   else
#       define UDP_MAIN_CHANNEL_EXPORT __declspec(dllimport)
#   endif
#else
#       define UDP_MAIN_CHANNEL_EXPORT
#endif

#include <ILocalChannel.h>
namespace NUDT
{
class UDP_MAIN_CHANNEL_EXPORT CUDPMainChannel:public ILocalChannel
{
public:
	static NSHARE::CText const NAME;
	CUDPMainChannel();
	virtual ~CUDPMainChannel();
	virtual bool MOpen(IIOConsumer*);
	virtual bool MIsOpened() const ;
	virtual bool MIsConnected() const;
	virtual void MClose();
	virtual bool MSend(user_data_t & aVal);
	virtual void MHandleServiceDG(main_channel_param_t const* aP);
	virtual void MHandleServiceDG(main_channel_error_param_t const* aP);
	virtual void MHandleServiceDG(request_main_channel_param_t const* aP);
	virtual void MHandleServiceDG(close_main_channel_t const* aP);
	virtual data_t MGetNewBuf(size_t aSize) const;
private:

	template<class DG_T> void MFill(data_t*);
//	template<class DG_T>
//	inline int MFill(user_data_t& _data);
//	template<class DG_T>
//	inline int MFill(const NSHARE::uuid_t& aTo, unsigned aSize, void* aBuffer,
//			data_t* _data, const NSHARE::CText& aProtocolType);
	template<class DG_T> void MProcess(const DG_T* aP, void*);
	static NSHARE::eCBRval sMReceiver(void*, void*, void*);
	void MReceiver();

	IIOConsumer* FCustomer;

	NSHARE::CUDP FUdp;
	NSHARE::CThread FThreadReceiver;
	CInParser<CUDPMainChannel> FMainParser;
	bool FIsConnected;
	friend class CInParser<CUDPMainChannel>;
};
}
#endif /* CUDPMAINCHANNEL_H_ */
