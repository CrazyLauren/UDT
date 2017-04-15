/*
 * CControlByTCP.h
 *
 *  Created on: 17.12.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 *	Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#ifndef CCONTROLBYTCP_H_
#define CCONTROLBYTCP_H_

#if  defined( _WIN32 ) && defined(_MSC_VER) && !defined( TCP_CLIENT_IO_MANAGER_STATIC)
#   ifdef TCP_CLIENT_IO_MANAGER_EXPORTS
#       define  TCP_CLIENT_IO_MANAGER_EXPORT __declspec(dllexport)
#   else
#       define TCP_CLIENT_IO_MANAGER_EXPORT __declspec(dllimport)
#   endif
#else
#       define TCP_CLIENT_IO_MANAGER_EXPORT
#endif

#include "IIOConsumer.h"
#include "ILocalChannel.h"
namespace NUDT
{
class TCP_CLIENT_IO_MANAGER_EXPORT CControlByTCP: public IIOConsumer, NSHARE::CAsyncSocket
{
public:
	static const NSHARE::CText NAME;
	static const NSHARE::CText PORT;

	CControlByTCP();
	virtual ~CControlByTCP();

	void MInit(ICustomer*);
	bool MOpen(const NSHARE::CThread::param_t* = NULL);
	bool MIsAvailable() const;
	bool MIsConnected() const;
	bool MIsOpened() const;
	void MClose();
	int MSend(data_t & aData);
	int MSend(user_data_t & aData);

	void MReceivedData(const data_t& aData,void*);
	void MReceivedData(user_data_t const & aData);
	void MReceivedData(progs_id_t const & aData);
	void MReceivedData(fail_send_t const & aData);
	void MReceivedData(demand_dgs_t const & aData);


	int MWaitForSend(unsigned aNumber, unsigned aTime = 0);
	//void MRecvListUpdated() ;//update filters

	NSHARE::ISocket* MGetSocket();
	const NSHARE::ISocket* MGetSocket() const;
	bool MIsKernel()const;
	virtual NSHARE::CBuffer MGetNewBuf(unsigned) const;
	int MSendMainChannelError(NSHARE::CText const& _channel_type, unsigned aError);
private:

	enum eState //don't use pattern
	//as it's  difficult for the class
	{
		E_CLOSED,//
		E_OPENED,//
		E_SERVICE_CONNECTED,//
		E_SETTING,//
		E_CONNECTED,//
	};
	static const double WAIT_ANSWER_BY_KERNEL;
	//convenient method for filling DG
	template<class DG_T> inline void MFill(user_data_t*);
	template<class DG_T> inline void MFill(data_t*);


	//method for handling input DG
	//Why it is template? For reliability, if you specializes it
	//by the incorrect structure(is not corresponding to
	//the structure in the parser_protocol class), the
	//program is not compiled.
	//
	template<class DG_T> void MProcess(const DG_T* aP, void*);

	void MInit(uint32_t aPort);
	void MReceivedData(const NSHARE::ISocket::data_t& aData);
	void MReplaceMainChannelTo(ILocalChannel*);
	int MOpenMainChannel(NSHARE::CText const & aType);

	static int sMConnect(void* aWho, void* aWhat, void* aThis);
	void MConnect(NSHARE::net_address* aVal);

	static int sMDisconnect(void* aWho, void* aWhat, void* aThis);
	void MDisconnect(NSHARE::net_address* aVal);
	int MCloseMain();
	int MSendIDInfo();
	void MSendFilters();
	void MSendFail(fail_send_t const&);

	static int sMUpdateList(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	static int sMFailSend(CHardWorker* aWho, args_data_t* aWhat, void* aData);
	void MWaitForKernelReceived(unsigned _num);
	void MKernelReceived(unsigned _num);
	bool MIsKernelReceived(unsigned _num) const;
	id_t const& MGetKernelID() const;
	unsigned MNextUserPacketNumber();
	void MSendProtocolType();
	void MSetStateConnected();

	NSHARE::CTCP* FTcpSocket;

	NSHARE::CB_t FCBServiceConnect;
	NSHARE::CB_t FCBServiceDisconncet;

	CInParser<CControlByTCP> FParser;
	mutable NSHARE::CMutex FMainLock;
	mutable NSHARE::CMutex FControlLock;
	std::set<unsigned> FSendUserFIFO;

	unsigned FPort; //FIXME it's unnecessary

	ICustomer* FCustomer;
	ILocalChannel* FMain;
	NSHARE::smart_field_t<program_id_t> FKernelId;
	eState FState;//for debuging only


	friend class CInParser<CControlByTCP> ;
};
}
#endif /* CCONTROLBYTCP_H_ */
