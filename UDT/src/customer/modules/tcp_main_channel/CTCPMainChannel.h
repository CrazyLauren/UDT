/*
 * CTCPMainChannel.h
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CTCPMAINCHANNEL_H_
#define CTCPMAINCHANNEL_H_

#if  defined( _WIN32 ) && defined(_MSC_VER) && !defined(TCP_MAIN_CHANNEL_STATIC)
#   ifdef TCP_MAIN_CHANNEL_EXPORTS
#       define TCP_MAIN_CHANNEL_EXPORT __declspec(dllexport)
#   else
#       define TCP_MAIN_CHANNEL_EXPORT __declspec(dllimport)
#   endif
#else
#       define TCP_MAIN_CHANNEL_EXPORT
#endif

#include <ILocalChannel.h>
namespace NUDT
{
class TCP_MAIN_CHANNEL_EXPORT CTCPMainChannel:public ILocalChannel
{
public:
	static NSHARE::CText const NAME;
	CTCPMainChannel();
	virtual ~CTCPMainChannel();

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
	template<class DG_T> void MFill(data_t*,DG_T const&,data_t::size_type aSize);
	template<class DG_T> void MProcess(const DG_T* aP, void*);

	static NSHARE::eCBRval sMReceiver(void*, void*, void*);
	void MReceiver();

	void MCloseImpl();
	int MSendMainChannelError(unsigned aError);
	IIOConsumer* FCustomer;

	NSHARE::CTCP FTcpClient;
	NSHARE::CThread FThreadReceiver;
	CInParser<CTCPMainChannel> FMainParser;
	bool FIsConnected;
	friend class CInParser<CTCPMainChannel>;
};
}
#endif /* CTCPMAINCHANNEL_H_ */
