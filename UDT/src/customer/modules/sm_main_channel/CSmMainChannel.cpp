// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * CSmMainChannel.cpp
 *
 *  Created on: 25.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */

#include <deftype>
#include <SHARE/revision.h>
#include <SHARE/share_socket.h>
#include <udt/programm_id.h>
#include <udt_share.h>
#include <internel_protocol.h>
#include <CDataObject.h>
#include <udt/CCustomer.h>
#include <CLocalChannelFactory.h>
#include "receive_from.h"
#include <parser_in_protocol.h>
#include <sm_shared.h>
#include "CSmMainChannel.h"
#include "CMainSmRegister.h"
DECLARATION_VERSION_FOR(sm_main_channel)
//todo может добавить user id при иницилизации
//todo user_data_info_t has to be allocated in shared memory directly
namespace NUDT
{
NSHARE::CText const CSmMainChannel::NAME = "sm";
NSHARE::CText const CMainSmRegister::NAME = "sm_main";
using namespace NSHARE;
CSmMainChannel::CSmMainChannel() :
		ILocalChannel(NAME), FCustomer(NULL), FMainParser(this)
{
	FIsOpen = false;
	FThreadReceiver += NSHARE::CB_t(CSmMainChannel::sMReceiver, this);
	{
		callback_data_t _callbak(sMSend, this);
		FSendDataHandler = CDataObject::value_t(send_data_to_t::NAME, _callbak);
	}
}

CSmMainChannel::~CSmMainChannel()
{
	FSm.MClose();
	if (FThreadReceiver.MCancel() && FThreadReceiver.MIsRunning())
		FThreadReceiver.MJoin();
}
void CSmMainChannel::MInit(ICustomer *)
{
	;
}
bool CSmMainChannel::MOpen(const NSHARE::CThread::param_t* )
{
	return true;
}
bool CSmMainChannel::MOpen(IIOConsumer* aCustomer)
{
	FCustomer = aCustomer;
	//FSm.MOpen();
	FIsOpen = true;

	CDataObject::sMGetInstance() += FSendDataHandler;

	return true;
}
int CSmMainChannel::sMSend(CHardWorker* aWho, args_data_t* aWhat, void* aData)
{
	DCHECK_EQ(aWhat->FType, send_data_to_t::NAME);
	reinterpret_cast<CSmMainChannel*>(aData)->MSend(
			reinterpret_cast<send_data_to_t*>(aWhat->FPointToData)->FData);
	return E_CB_SAFE_IT;
}
bool CSmMainChannel::MIsConnected() const
{
	return MIsOpened() && FSm.MIsOpen() && FSm.MIsConnected();
}
bool CSmMainChannel::MIsOpened() const
{
	return FIsOpen;
}
void CSmMainChannel::MJoin()
{
	;
}
void CSmMainChannel::MClose()
{
	FSm.MClose();
	FThreadReceiver.MCancel();
	FIsOpen = false;
	FCounter=sm_counter_t();
	FCustomer=NULL;
	CDataObject::sMGetInstance() -= FSendDataHandler;
}
NSHARE::eCBRval CSmMainChannel::sMReceiver(void* aWho, void* aWhat, void*aData)
{
	reinterpret_cast<CSmMainChannel*>(aData)->MReceiver();
	return E_CB_SAFE_IT;
}
void CSmMainChannel::MCheckPacketSequence(const unsigned aPacket,
		unsigned & aLast)const
{
	flag_mask_t _last_counter;
	_last_counter.FData.FCounter = aLast;
	++_last_counter.FData.FCounter;
	LOG_IF(FATAL,_last_counter.FData.FCounter!=aPacket)
															<< "The packet has been lost. "
															<< " Counter="
															<< aPacket
															<< " Last counter="
															<< aLast;
	aLast = aPacket;
}
/** Push data to handle queue
 *
 * @param aData what is received
 */
void CSmMainChannel::MReceivedData(user_data_t& aData)
{
	recv_data_from_t _data;
	_data.FData.FDataId = aData.FDataId;
	aData.FData.MMoveTo(_data.FData.FData);

	CDataObject::sMGetInstance().MPush(_data);
}
void CSmMainChannel::MReceiveImpl(unsigned aType, NSHARE::CBuffer& _data,
		NSHARE::shared_identify_t const& _from)
{
	switch (aType)
	{
	case E_SM_INFO:
	{
		VLOG_IF(1,FRecv.first) << "The data in " << FRecv.first
										<< " is not handled.";
		user_data_info_t _info;
		deserialize_dg_head(_info,_data.ptr_const());
		FRecv.first = true;
		FRecv.second.FDataId = _info;
		VLOG(4) << "Data info:" << FRecv.second;
		break;
	}
	case E_SM_DATA:
	{
		CHECK(FRecv.first);
		VLOG(1) << "Receive packet #" << FRecv.second.FDataId.FPacketNumber
							<< " from " << _from;
		CHECK_NOTNULL(FCustomer);
		_data.MMoveTo(FRecv.second.FData);
		MReceivedData(FRecv.second);
		FRecv.first = false;
		break;
	}
	default:
		LOG(FATAL)<<"Unknown code "<<aType;
		break;
	};
};
void CSmMainChannel::MReceiver()
{
	VLOG(2) << "Async receive";
	using namespace NSHARE;
	for (HANG_INIT;MIsOpened() && !FSm.MIsOpen(); HANG_CHECK)
	{
		NSHARE::usleep(10000);
	}
	LOG(WARNING)<< "Async receive";
	NSHARE::shared_identify_t _from;
	for (; FSm.MIsOpen();)
	{
		VLOG(5) << "Receive data by SM";
		shared_identify_t _from;
		flag_mask_t _mask;
		CBuffer _data;
		bool const _is=FSm.MReceiveData(_data,&_from, &_mask.FMask);

		VLOG(2) << "Receive data from " << _from<<" is ="<<_is;
		VLOG_IF(2,_data.empty()) << "data empty from "<<_from;
		DCHECK(!_is||!_data.empty());

		if (_is && !_data.empty())
		{

			VLOG(2) << "Type=" << _mask.FData.FType << " Counter="
								<< _mask.FData.FCounter << " Last counter="
								<< FCounter.FFrom;
			MCheckPacketSequence(_mask.FData.FCounter, FCounter.FFrom);
			MReceiveImpl(_mask.FData.FType, _data, _from);

		}
	}
	LOG(WARNING)<< "Async receive ended";
}

//
//-----
//
template<>
void CSmMainChannel::MFill<main_channel_param_t>(data_t* aTo)
{
	VLOG(2) << "Create main channel param DG";

	NSHARE::shared_identify_t _ident = FSm.MIdentifier();
	main_ch_param_t _param;
	_param.FType = E_MAIN_CHANNEL_SM;
	if (_ident.MIsValid())
	{
		_param.FValue = _ident.MSerialize();
	}

	serialize<main_channel_param_t, main_ch_param_t>(aTo, _param, routing_t(), error_info_t());

}

//
//----------
//
void CSmMainChannel::MHandleServiceDG(main_channel_param_t const* aP)
{
	VLOG(2) << "SM Main channel handling \"Main channel parametrs\":";
	main_ch_param_t _sparam(deserialize<main_channel_param_t, main_ch_param_t>(aP, (routing_t*)NULL, (error_info_t*)NULL));
	VLOG(5) << _sparam.FValue.MToJSON(true);

	CHECK_EQ(_sparam.FType, NAME);

	NSHARE::CText _sm_name;	
	if (_sparam.FValue.MGetIfSet("path", _sm_name))
	{
		LOG_IF(DFATAL, _sm_name.empty()) << "The Sm name is not exist.";
		CHECK(!FSm.MIsOpen());
		bool _is = FSm.MOpen(_sm_name);
		LOG_IF(DFATAL,!_is)<<"Cannot open sm "<<_sm_name;
		if (FSm.MIsOpen())
		{
			_is = FSm.MConnect(0.1);
			NSHARE::CThread::param_t _param;
			NSHARE::CThread::eThreadPriority _priority =
					NSHARE::CThread::THREAD_PRIORITY_MAX;
			_param.priority = _priority;
			if (_is)
				FThreadReceiver.MCreate(&_param);
			else
			{
				MSendMainChannelError(main_channel_error_param_t::E_NOT_OPENED);
			}
		}
	}
	else
		LOG(FATAL) << "path is not exist";
}
int CSmMainChannel::MSendMainChannelError(unsigned aError)
{
	data_t _channel;
	_channel.resize(sizeof(main_channel_error_param_t));
	main_channel_error_param_t* _p =
			new (_channel.ptr()) main_channel_error_param_t;
	CHECK_NOTNULL(_p);
	strcpy((char*) _p->FType, NAME.c_str());

	_p->FError = static_cast<main_channel_error_param_t::eError>(aError);

	fill_dg_head(_channel.ptr(), _channel.size(), get_my_id());


	int _is= FCustomer->MSend(_channel);
	(void) _is;
	LOG_IF(ERROR,_is<0) << "Cannot accept main channel.";
	return _is;
}
void CSmMainChannel::MHandleServiceDG(request_main_channel_param_t const* aP)
{
	VLOG(2) << "Request main channel param " << *aP;
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);
	if (!MIsConnected())
	{
//		LOG(DFATAL)<<"Sm is not connected";
		return;
	}
	data_t _buf;
	MFill<main_channel_param_t>(&_buf);

	int _is = FCustomer->MSend(_buf);
	(void) _is;
	LOG_IF(ERROR,_is<0) << "Cannot send main channel params.";
}
void CSmMainChannel::MHandleServiceDG(close_main_channel_t const* aP)
{
	CText _name((utf8 const*) aP->FType);
	CHECK_EQ(_name, NAME);

	MClose(); //todo
}
void CSmMainChannel::MHandleServiceDG(main_channel_error_param_t const* aP)
{
	VLOG(2) << "main channel error " << *aP;
//	FIsConnected = aP->FError == 0;
	LOG_IF(ERROR,aP->FError != 0) << "Error during setting main channel.";
	if (aP->FError != 0)
	{
		MSendMainChannelError(
				FSm.MIsConnected() ?
						main_channel_error_param_t::E_OK :
						main_channel_error_param_t::E_NOT_OPENED);
	}
}
NSHARE::CBuffer CSmMainChannel::MGetNewBuf(size_t aSize) const
{
	if (FSm.MIsOpen())
		return FSm.MAllocate(aSize,0);
	return NSHARE::CBuffer(aSize);
}
bool CSmMainChannel::MSendInOnePart(const size_t _size,
		const user_data_info_t& aInfo, NSHARE::CBuffer& _data_buf)
{
	VLOG(1) << "send info and data by one buffer";

	const size_t _before = _data_buf.size();
	const size_t _full_size = _before + _size + sizeof(uint32_t);
	_data_buf.resize(_full_size);
	CHECK(!_data_buf.MIsDetached());

	NSHARE::CBuffer::pointer const _header = _data_buf.ptr() + _before;
	fill_header(_header, aInfo, _before);

	NSHARE::CBuffer::pointer const _p_size = _data_buf.ptr() + _before + _size;
	uint32_t *_p = (uint32_t *) _p_size;
	*_p = static_cast<uint32_t>(_size);

	for (;;)
	{
		flag_mask_t _mask;
		_mask.FData.FCounter = FCounter.FTo;
		_mask.FData.FType = E_SM_DATA_INFO;
		NSHARE::CSharedMemoryClient::eSendState _state = FSm.MSend(_data_buf,
				false, _mask.FMask);
		VLOG_IF(1,_state==NSHARE::sent_state_t::E_SENDED)
																		<< _data_buf.size()
																		<< " bytes sent successfully ";
		switch (_state)
		{
		case NSHARE::sent_state_t::E_SENDED:
		{
			++_mask.FData.FCounter;
			VLOG(2) << "Next counter=" << _mask.FData.FCounter;
			FCounter.FTo = _mask.FData.FCounter;
			_data_buf.release(); //force release
			return true;
			break;
		}
		case NSHARE::sent_state_t::E_ERROR:
		{
			return false;
			break;
		}
		case NSHARE::sent_state_t::E_AGAIN:
			VLOG(1) << "Try send again.";
			NSHARE::CThread::sMYield();
			break;
		default:
			break;
		} //
	}
	return false;
}
bool  CSmMainChannel::MSendInTwoParts(const size_t _size, const user_data_info_t& aInfo, NSHARE::CBuffer& _data_buf)
{
	VLOG(2)<<"send data in two part";
	//The first is data info
	//The second is data
	NSHARE::CBuffer _buf(MGetNewBuf(_size));
	if (_buf.empty())
		return false;

	fill_header(_buf.ptr(), aInfo, _data_buf.offset());

	bool _is_info_sended = false;
	for (;;)
	{
		flag_mask_t _mask;
		_mask.FData.FCounter = FCounter.FTo;
		_mask.FData.FType = _is_info_sended ? E_SM_DATA : E_SM_INFO;

		NSHARE::CSharedMemoryClient::eSendState _state =
				_is_info_sended ?
						FSm.MSend(_data_buf, false, _mask.FMask) :
						FSm.MSend(_buf, false, _mask.FMask);
		VLOG_IF(1,_state==NSHARE::sent_state_t::E_SENDED)
																		<< _data_buf.size()
																		<< " bytes sent successfully ";
		switch (_state)
		{
		case NSHARE::sent_state_t::E_SENDED:
		{
			++_mask.FData.FCounter;
			VLOG(2) << "Next counter=" << _mask.FData.FCounter;
			FCounter.FTo = _mask.FData.FCounter;
			if (_is_info_sended)
				return true;
			else
				_is_info_sended = true;
			break;
		}
		case NSHARE::sent_state_t::E_ERROR:
		{
			return false;
			break;
		}
		case NSHARE::sent_state_t::E_AGAIN:
			VLOG(1) << "Try send again.";
			NSHARE::CThread::sMYield();
			break;
		default:
			break;
		} //

	}
	return false;
}
bool CSmMainChannel::MSendImpl(user_data_info_t const & aInfo, NSHARE::CBuffer& _data_buf)
{
	size_t const _size = get_full_size(aInfo);
	size_t const _remain=_data_buf.capacity()-_data_buf.size()-_data_buf.begin_capacity();
	return _size < _remain?MSendInOnePart(_size,aInfo,_data_buf):MSendInTwoParts(_size,aInfo,_data_buf);
}

/** Move data into shared memory
 *
 * @param aTo Copy to
 * @param aVal Copy From
 * @return true if no error
 */
bool CSmMainChannel::MMoveDataToShm(NSHARE::CBuffer& aTo,
		user_data_t& aVal)
{
	bool _is=true;

	DLOG(ERROR)<< "It's not the recommended  allocator";
	aTo = MGetNewBuf(aVal.FData.size());
	if (aTo.size() != aVal.FData.size())
	{
		DLOG(ERROR) << "Cannot allocate buffer.";
		_is = false;
	}
	else
	{
		aTo.deep_copy(aVal.FData);
		aVal.FData.release();
	}
	return _is;
}
/** Notify about message isn't send
 *
 * @param aVal That isn't sent
 */
void CSmMainChannel::MNoteFailedSend(user_data_info_t const& aVal) const
{
	fail_send_id_t _fail;
	_fail.FData = fail_send_t(aVal);
	_fail.FData.MSetError(E_SOCKET_CLOSED);
	CDataObject::sMGetInstance().MPush(_fail);
}

bool CSmMainChannel::MSend(user_data_t & aVal)
{
	VLOG(2) << "Sending user data.";
	bool _is=FSm.MIsConnected();

	LOG_IF(ERROR,!_is)<<"The Main channel is not opened";

	if (_is)
	{
		//aVal.FDataId.FFrom = CCustomer::sMGetInstance().MGetID().FId;
		NSHARE::CBuffer _data_buf;
		if (aVal.FData.MIsAllocatorEqual(FSm.MGetAllocator()))
		{
			///Data in Shm yet
			VLOG(2) << "It's the recommended  allocator";
			aVal.FData.MMoveTo(_data_buf);
		}
		else
			_is = MMoveDataToShm(_data_buf, aVal);

		if (_is)
		{
			DCHECK_EQ(_data_buf.use_count(), 1);

			_is = MSendImpl(aVal.FDataId, _data_buf);
			if (!_is)
			{
				//repair buffer
				aVal.FData = _data_buf;
			}
			_data_buf.release();
		}
	}

	if(!_is)
	{
		MNoteFailedSend(aVal.FDataId);
	}
	return _is;
}

CMainSmRegister::CMainSmRegister() :
		NSHARE::CFactoryRegisterer(NAME, NSHARE::version_t(MAJOR_VERSION_OF(sm_main_channel), MINOR_VERSION_OF(sm_main_channel), REVISION_OF(sm_main_channel)))
{

}
void CMainSmRegister::MUnregisterFactory() const
{
	CLocalChannelFactory::sMGetInstance().MRemoveFactory(CSmMainChannel::NAME);
}
void CMainSmRegister::MAdding() const
{
	//CSmMainChannel* _p =
	CLocalChannelFactory::sMAddFactory<CSmMainChannel>();
}
bool CMainSmRegister::MIsAlreadyRegistered() const
{
	if (CLocalChannelFactory::sMGetInstancePtr())
		return CLocalChannelFactory::sMGetInstance().MIsFactoryPresent(
				CSmMainChannel::NAME);
	return false;

}
}
#if !defined(SM_MAIN_CHANNEL_STATIC)
static NSHARE::factory_registry_t g_factory;
extern "C" SM_MAIN_CHANNEL_EXPORT NSHARE::factory_registry_t* get_factory_registry()
{
	if (g_factory.empty())
	{
		g_factory.push_back(new NUDT::CMainSmRegister());
	}
	return &g_factory;
}
#else
#	include <load_static_module.h>
namespace
{
	static NUDT::CStaticRegister<NUDT::CMainSmRegister> _reg;
}
#endif
