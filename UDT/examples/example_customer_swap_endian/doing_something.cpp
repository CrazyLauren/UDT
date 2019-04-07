/*
 * doing_something.cpp
 *
 *  Created on: 29.03.2016
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2016  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <customer.h>
#include <map>
#include <stdio.h>


#include <udt_example_protocol.h>
#ifdef _WIN32
#define _USE_MATH_DEFINES
#	include <windows.h>
::CRITICAL_SECTION _stream_mutex;
#	define STREAM_MUTEX_LOCK ::EnterCriticalSection(&_stream_mutex);
#	define STREAM_MUTEX_UNLOCK ::LeaveCriticalSection(&_stream_mutex);
#	ifdef uuid_t
#		undef uuid_t //"Fucking Microsoft! It's macro in lowercase."
#	endif

#else
#	include <pthread.h>
#	include <unistd.h>
pthread_mutex_t _stream_mutex;
#	define STREAM_MUTEX_LOCK pthread_mutex_lock( &_stream_mutex );
#	define STREAM_MUTEX_UNLOCK pthread_mutex_unlock( &_stream_mutex );
#	define Sleep(aVal) usleep((aVal)*1000)
#endif
#include <math.h>
using namespace NUDT;

inline bool double_equal(float a, float b, float epsilon =
		std::numeric_limits<float>::epsilon())
{
	const float _a = fabs(a);
	const float _b = fabs(b);
	const float _diff = fabs(a - b);
	return _diff <= ((_b > _a ? _b : _a) * epsilon);
}

extern int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	received_message_args_t const* _recv_arg = (received_message_args_t const*) aWHAT;

	msg_data const *_msg = (msg_data const*) _recv_arg->FBegin;

	const unsigned char _i = _msg->FNumber;

	if (_msg->FUint16 != (uint16_t) _i * _i) //see doing_something function
	{
		STREAM_MUTEX_LOCK
		std::cerr << "Fail uint16:" << (unsigned)_i * _i << "!=" << (unsigned) (_msg->FUint16)
				<< std::endl;
		STREAM_MUTEX_UNLOCK
		throw;

	}else if (_msg->FInt16 != (-(int16_t) _i) * _i)
	{
		STREAM_MUTEX_LOCK
		std::cerr << "Fail int16:" << ((-(int16_t) _i) * _i)<< "!=" << (_msg->FInt16)
				<< std::endl;
		STREAM_MUTEX_UNLOCK
		throw;
	}
	else if (_msg->FUint32 != (uint32_t) _msg->FUint16 * _msg->FUint16)
	{
		STREAM_MUTEX_LOCK
		std::cerr << "Fail uint32:" << (uint32_t) _msg->FUint16 * _msg->FUint16
				<< "!=" << _msg->FUint32 << std::endl;
		STREAM_MUTEX_UNLOCK
		throw;
	}
	else if (_msg->FInt32 != -(int32_t) _msg->FUint16 * (int32_t) _msg->FUint16)
	{
		STREAM_MUTEX_LOCK
		std::cerr << "Fail int32:"
				<< (-(int32_t) _msg->FUint16 * (int32_t) _msg->FUint16) << "!="
				<< _msg->FInt32 << std::endl;
		STREAM_MUTEX_UNLOCK
		throw;
	}
	else if (!double_equal(_msg->FFloat, (_msg->FNumber *(float) M_PI)))
	{
		STREAM_MUTEX_LOCK
		std::cerr << "Fail double:"
				<< _msg->FFloat << "!="
				<< _msg->FNumber * M_PI << std::endl;
		STREAM_MUTEX_UNLOCK
		throw;
	}
	else
	{
		STREAM_MUTEX_LOCK
		std::cout << "Receiving packet #" << _recv_arg->FPacketNumber
				<< " size " << _recv_arg->FBuffer.size() << " bytes from "
				<< _recv_arg->FFrom << " by " << _recv_arg->FProtocolName
				<< std::endl;
		STREAM_MUTEX_UNLOCK
	}
	return 0;
}
extern int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	new_receiver_args_t* _recv_arg=(new_receiver_args_t*)aWHAT;
	STREAM_MUTEX_LOCK
	std::vector<new_receiver_args_t::what_t>::const_iterator _it(
			_recv_arg->FReceivers.begin()), _it_end(
			_recv_arg->FReceivers.end());
	for(;_it!=_it_end;++_it)
	{
		std::cout <<"*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*"<< std::endl;
		std::cout << "Now " << _it->FWho << " receive " << _it->FWhat.FRequired
				<< " by " << _it->FWhat.FProtocolName << " As in "
				<< _it->FRegExp << std::endl;
		std::cout <<"-------------------------------------"<< std::endl;
	}

	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	STREAM_MUTEX_LOCK
	std::cout << "The udt library has been connected.."<< std::endl;
	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	fail_sent_args_t* _recv_arg=(fail_sent_args_t*)aWHAT;

	STREAM_MUTEX_LOCK
	std::cerr<<"The packet "<<_recv_arg->FPacketNumber<<" has not been delivered to ";
	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FFails.begin()),
			_it_end(_recv_arg->FFails.end());

	for (; _it != _it_end; ++_it)
	{
		std::cerr<<(*_it)<<", ";
	}
	std::cerr << " by UDT kernel as ";
	CCustomer::sMPrintError(std::cerr,_recv_arg->FErrorCode);
	if(_recv_arg->FErrorCode & CCustomer::E_USER_ERROR_EXIST)
				std::cerr<<" user's code="<<(unsigned)_recv_arg->FUserCode;
	std::cerr<< std::endl;

	STREAM_MUTEX_UNLOCK
	return 0;
}
extern int event_customers_update_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	customers_updated_args_t const* _recv_arg=(customers_updated_args_t*)aWHAT;
	STREAM_MUTEX_LOCK
	if(!_recv_arg->FConnected.empty())
	{
		std::set<program_id_t>::const_iterator _it=_recv_arg->FConnected.begin(),_it_end(_recv_arg->FConnected.end());
		std::cout << "Connected customers:";
		for (; _it != _it_end; ++_it)
		{
			std::cout<<"\t" << (*_it) << std::endl;
		}
		std::cout << " to UDT." << std::endl;
	}
	if(!_recv_arg->FDisconnected.empty())
	{
		std::cout << "Disconnected customers:";
		std::set<program_id_t>::const_iterator _it =
				_recv_arg->FDisconnected.begin(), _it_end(
				_recv_arg->FDisconnected.end());
		for (; _it != _it_end; ++_it)
		{
			std::cout << (*_it)<< std::endl;
		}
		std::cout<<"\t"  << " from UDT." << std::endl;
	}
	STREAM_MUTEX_UNLOCK
	return 0;
}


extern void doing_something()
{
#ifdef _WIN32
	::InitializeCriticalSection(&_stream_mutex);
#else
	pthread_mutex_init(&_stream_mutex, NULL);
#endif
	for (; !CCustomer::sMGetInstance().MIsConnected(); Sleep(1000))
		;


	for (unsigned i=0;;++i)
	{
		{
			STREAM_MUTEX_LOCK
			std::cout << "Press any key to send message" << std::endl;
			STREAM_MUTEX_UNLOCK
			getchar();
		}

		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
				sizeof(test_msg_t));	///< allocate the buffer for msg

		//filing the head of msg
		test_msg_t *_msg = (test_msg_t*) _buf.ptr();
		memset(_msg,0,sizeof(test_msg_t));

		_msg->FHead.FType = E_MSG_SWAP_BYTE_TEST;
		_msg->FHead.FSize = sizeof(test_msg_t);

		_msg->FData.FNumber=i;

		_msg->FData.FUint16=i*i;
		_msg->FData.FInt16=-((int)i)*i;

		_msg->FData.FUint32=_msg->FData.FUint16*_msg->FData.FUint16;
		_msg->FData.FInt32=-((int)_msg->FData.FUint16)*_msg->FData.FUint16;


		_msg->FData.FFloat=_msg->FData.FNumber*(float)M_PI;

		//send the data
		int _num = CCustomer::sMGetInstance().MSend(PROTOCOL_NAME, _buf);
		if (_num > 0)	//Hurrah!!! The data has been sent
		{
			//Warning!!! As The buffer is sent, it's freed. Thus calling _buf.size() return 0.
			STREAM_MUTEX_LOCK
			std::cout << "Send Packet#" << _num << " size of " << PACKET_SIZE
					<< " bytes." << std::endl;
			STREAM_MUTEX_UNLOCK

		}
		else //The buffer _buf is not freed as it's not sent.
		{
			STREAM_MUTEX_LOCK
			std::cout << "Send error  " << _num << std::endl;
			STREAM_MUTEX_UNLOCK
		}

	};

}

