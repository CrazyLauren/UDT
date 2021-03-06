/*
 * CSmMainChannel.h
 *
 *  Created on: 01.07.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CSMMAINCHANNEL_H_
#define CSMMAINCHANNEL_H_

#if  defined( _WIN32 ) && defined(_MSC_VER) && !defined(SM_MAIN_CHANNEL_STATIC)
#   ifdef SM_MAIN_CHANNEL_EXPORTS
#       define SM_MAIN_CHANNEL_EXPORT __declspec(dllexport)
#   else
#       define SM_MAIN_CHANNEL_EXPORT __declspec(dllimport)
#   endif
#else
#       define SM_MAIN_CHANNEL_EXPORT
#endif

#include <ILocalChannel.h>
namespace NUDT
{
/** Class to transfer user data to kernel by shared memory
 *
 */
class SM_MAIN_CHANNEL_EXPORT CSmMainChannel:public ILocalChannel
{
public:
	static NSHARE::CText const NAME;
	CSmMainChannel();
	virtual ~CSmMainChannel();
	bool MOpen(const NSHARE::CThread::param_t* = NULL);
	virtual void MInit(ICustomer *);
	virtual bool MOpen(IIOConsumer*);
	virtual bool MIsOpened() const ;
	virtual bool MIsConnected() const;
	virtual void MClose();
	virtual void MJoin();
	virtual bool MSend(user_data_t & aVal);
	virtual void MHandleServiceDG(main_channel_param_t const* aP);
	virtual void MHandleServiceDG(main_channel_error_param_t const* aP);
	virtual void MHandleServiceDG(request_main_channel_param_t const* aP);
	virtual void MHandleServiceDG(close_main_channel_t const* aP);
	virtual data_t MGetNewBuf(size_t aSize) const;
private:
	struct sm_counter_t
	{
		sm_counter_t():FFrom(0),FTo(1)
		{

		}
		uint32_t FFrom;
		uint32_t FTo;
	};

	template<class DG_T> void MFill(data_t*);
	template<class DG_T> void MFill(user_data_t*);
//	template<class DG_T>
//	inline int MFill(user_data_t& _data);
//	template<class DG_T>
//	inline int MFill(const NSHARE::uuid_t& aTo, unsigned aSize, void* aBuffer,
//			data_t* _data, const NSHARE::CText& aProtocolType);
	template<class DG_T> void MProcess(const DG_T* aP, void*);
	static NSHARE::eCBRval sMReceiver(void*, void*, void*);
	static int sMSend(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	void MReceiver();
	void MCheckPacketSequence(const unsigned aPacket, unsigned & aLast) const;
	void MReceiveImpl(unsigned aType,
			NSHARE::CBuffer& _data,
			NSHARE::shared_identify_t const& _from);
	void MReceivedData(user_data_t& aData);
	bool MSendImpl(user_data_info_t const & aInfo,NSHARE::CBuffer& _data);
	bool  MSendInTwoParts(const size_t _size, const user_data_info_t& aInfo, NSHARE::CBuffer& _data_buf);
	bool  MSendInOnePart(const size_t _size, const user_data_info_t& aInfo, NSHARE::CBuffer& _data_buf);
	int MSendMainChannelError(unsigned aError);
	bool MMoveDataToShm(NSHARE::CBuffer& _data_buf,
			user_data_t& aVal);
	void MNoteFailedSend(user_data_info_t const& aVal) const;

	sm_counter_t FCounter;
	IIOConsumer* FCustomer;
	NSHARE::CSharedMemoryClient FSm;
	std::pair<bool, user_data_t> FRecv;
	NSHARE::CThread FThreadReceiver;
	CInParser<CSmMainChannel> FMainParser;
	bool FIsOpen;
	friend class CInParser<CSmMainChannel>;
	CDataObject::value_t FSendDataHandler;//!< Handler of data which has to be send
};
}
#endif /* CSMMAINCHANNEL_H_ */
