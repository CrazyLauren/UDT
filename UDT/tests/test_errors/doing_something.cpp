// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
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
#include <deftype>
#include <customer.h>
#include <map>

#include <map>
#include <stdio.h>

#include <customer.h>

#include <udt_example_protocol.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <pthread.h>
#	include <unistd.h>
#endif

#include "no_route_test_api.h"

namespace test_errors
{
static NSHARE::CMutex g_stream_mutex;///< A mutex for lock console output

static unsigned g_no_uuid_error=0;///< Amount of error no uuid
static unsigned g_invalid_version_error=0;///< Amount of error of invalid version
static unsigned g_parse_error=0;///< Amount of error during parsing of buffer
static unsigned g_parse_is_not_exist=0;///< Amount of no parser error

static unsigned g_amount_of_msg=0;///< Amount of received messages #MESSAGE_NUMBER
static unsigned g_amount_of_msg_test=0;///< Amount of received messages #example_of_user_protocol::E_MSG_TEST

NSHARE::version_t g_requrement_msg_version(1,1);

using namespace NUDT;

NSHARE::uuid_t g_sent_to(rand());
extern int msg_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::received_message_args_t structure */
	received_message_args_t const * _recv_arg=(received_message_args_t*)aWHAT;

	/*! 2) Handle the received data which is located
	 * from NUDT::received_data_t::FBegin
	 * to NUDT::received_data_t::FEnd*/
	const uint8_t* _it = _recv_arg->FMessage.FBegin;
	for (int i = 0; _it != _recv_arg->FMessage.FEnd; ++i, ++_it)
	{
		if (i % 255 != *_it)
		{
			std::cerr << "Fail data:" << i << "!=" << (unsigned) (*_it)
					<< std::endl;
			throw std::invalid_argument("Invalid argument");
		}
	}

	std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
			<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
			<< " bytes received from " << _recv_arg->FFrom << " by "
			<< _recv_arg->FProtocolName << std::endl;
	++g_amount_of_msg;
	return 0;
}
extern int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

	received_message_args_t * _recv_arg = (received_message_args_t*) aWHAT;

	/*! 2) Handle the received data which is located
	 * from NUDT::received_data_t::FBegin
	 * to NUDT::received_data_t::FEnd*/
	const uint8_t* _it = _recv_arg->FMessage.FBegin;
	for (int i = 0; _it != _recv_arg->FMessage.FEnd; ++i, ++_it)
	{
		if (i % 255 != *_it)
		{
			std::cerr << "Fail data:" << i << "!=" << (unsigned) (*_it)
					<< std::endl;
			throw std::invalid_argument("Invalid argument");
		}
	}

	std::cout << "Message #" << _recv_arg->FPacketNumber << " ver "
			<< _recv_arg->FHeader.FVersion << " size " << _recv_arg->FMessage.FBuffer.size()
			<< " bytes received from " << _recv_arg->FFrom << " by "
			<< _recv_arg->FProtocolName << std::endl;

	++g_amount_of_msg_test;
	return 0;
}
extern int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::subcribe_receiver_args_t structure */
	subcribe_receiver_args_t const* _recv_arg=(subcribe_receiver_args_t const*)aWHAT;


	/*! 2) Print information about new receivers and what messages they wanted to receive.*/
	for(auto const&_it:_recv_arg->FReceivers)
	{
		std::cout <<"*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*"<< std::endl;
		std::cout << "Now " << _it.FWho << " receive " << _it.FWhat.FRequired
				<< " from me by " << _it.FWhat.FProtocolName << " As its Request "
				<< _it.FWhat.FFrom << std::endl;
		std::cout <<"-------------------------------------"<< std::endl;
	}
	return 0;
}
extern int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	std::cout << "The udt library has been connected.." << std::endl;
	return 0;
}
extern int event_disconnect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	std::cout << "The udt library has been disconnected.."<< std::endl;
	return 0;
}
extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	/*! Algorithm:*/
	/*! 1) Convention void pointer to pointer to #NUDT::fail_sent_args_t structure */
	fail_sent_args_t const* _recv_arg = (fail_sent_args_t const*) aWHAT;

	/*! 2) Print information about for whom messages aren't delivered .*/
	std::cerr << "The packet " << _recv_arg->FPacketNumber
			<< " has not been delivered to ";

	for (auto const& _it :_recv_arg->FFails)
	{
		std::cerr << (_it) << ", ";
	}

	/*! 3) Print information about why the messages aren't delivered .*/
	CCustomer::sMPrintError(std::cerr, _recv_arg->FErrorCode);

	if (_recv_arg->FErrorCode & CCustomer::E_USER_ERROR_EXIST)
		std::cerr << " user's code=" << (unsigned) _recv_arg->FUserCode;
	std::cerr << std::endl;

	if (_recv_arg->FErrorCode & CCustomer::E_HANDLER_IS_NOT_EXIST)
		++g_no_uuid_error;

	if (_recv_arg->FErrorCode & CCustomer::E_PROTOCOL_VERSION_IS_NOT_COMPATIBLE)
		++g_invalid_version_error;

	if (_recv_arg->FErrorCode & CCustomer::E_CANNOT_PARSE_BUFFER)
		++g_parse_error;

	if (_recv_arg->FErrorCode & CCustomer::E_PARSER_IS_NOT_EXIST)
		++g_parse_is_not_exist;


	return 0;
}

extern void doing_tests()
{
	NSHARE::sleep(1);


	if(!test_invalid_uuid())
		exit(EXIT_FAILURE);

	if(!test_invalid_msg_version())
		exit(EXIT_FAILURE);

	if (!test_parsing_error())
		exit(EXIT_FAILURE);


	if (!test_no_parser_error())
		exit(EXIT_FAILURE);


	std::cout<<"Press any key... "<<std::endl;
	getchar();
}

bool test_invalid_uuid()
{
	///Algorithm:
	unsigned i = 0;
	for (; i < 5; ++i, NSHARE::sleep(1), g_sent_to = rand())
	{
		///1)  allocate the buffer for message without header
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
				MESSAGE_SIZE);
		/// 2) Filling message into allocated buffer (without header)
		{
			NSHARE::CBuffer::iterator _it = _buf.begin(), _it_end = _buf.end();
			for (int i = 0; _it != _it_end; ++i, ++_it)
			{
				*_it = i % 255;
			}
		}
		///3) Send the message number #MESSAGE_NUMBERE to random uuid
		int _num = CCustomer::sMGetInstance().MSend(MESSAGE_NUMBER, _buf,
				g_sent_to);
		if (_num > 0)
		{
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
			std::cout << "Send Packet#" << _num << std::endl;
		}
		else
		{
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
			std::cout << "Send error  " << _num << std::endl;
			return false;
		}
	};

	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
		if (i == g_no_uuid_error)
			std::cout << "Invalid uuid test finished for successfully "
					<< std::endl;
		else
			std::cerr << "Fail " << i << "!=" << g_no_uuid_error << std::endl;
	}
	return i == g_no_uuid_error;
}
/*\brief Send message #MESSAGE_NUMBERE of version
 *
 *\param aVer version of message
 */
static bool send_message(NSHARE::version_t const & aVer)
{
	g_amount_of_msg = 0;
	g_invalid_version_error = 0;
	///1)  allocate the buffer for message without header
	NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(MESSAGE_SIZE);

	/// 2) Filling message into allocated buffer (without header)
	{
		auto _it = _buf.begin(), _it_end = _buf.end();
		for (int i = 0; _it != _it_end; ++i, ++_it)
		{
			*_it = i % 255;
		}
	}
	///3) Send the message number #MESSAGE_NUMBERE to random uuid
	int _num = CCustomer::sMGetInstance().MSend(MESSAGE_NUMBER, _buf, aVer);
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
		if (_num > 0)
			std::cout << "Send Packet#" << _num << std::endl;
		else
			std::cout << "Send error  " << _num << std::endl;
	}
	return _num > 0;
}
bool test_invalid_msg_version()
{
	///Algorithm:

	{
		/// 1) Send message with invalid minor version
		auto _ver = g_requrement_msg_version;
		--_ver.FMinor;

		if(!send_message(_ver))
			return false;
	}
	NSHARE::sleep(1);
	{
		///2) Checking amount of errors and received messages
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		if (g_amount_of_msg != 0 || g_invalid_version_error == 0)
		{
			std::cout << "Invalid version test finished unsuccessfully "
					<< std::endl;
			return false;
		}
		else
			std::cout << "Invalid version test finished successfully "
					<< std::endl;
	}
	{
		/// 3) Send message with invalid major version (less)
		auto _ver = g_requrement_msg_version;
		--_ver.FMajor;

		if (!send_message(_ver))
			return false;
	}
	NSHARE::sleep(1);
	{
		///4) Checking amount of errors and received messages
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		if (g_amount_of_msg != 0 || g_invalid_version_error == 0)
		{
			std::cout << "Invalid version test finished unsuccessfully "
					<< std::endl;
			return false;
		}
		else
			std::cout << "Invalid version test finished successfully "
					<< std::endl;
	}
	{
		/// 5) Send message with invalid major version (greater)
		auto _ver = g_requrement_msg_version;
		++_ver.FMajor;

		if (!send_message(_ver))
			return false;
	}
	NSHARE::sleep(1);
	{
		///6) Checking amount of errors and received messages
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		if (g_amount_of_msg != 0 || g_invalid_version_error == 0)
		{
			std::cout << "Invalid version test finished unsuccessfully "
					<< std::endl;
			return false;
		}
		else
			std::cout << "Invalid version test finished successfully "
					<< std::endl;
	}
	{
		///7) Send message with valid minor version (greater)
		auto _ver = g_requrement_msg_version;
		++_ver.FMinor;

		if (!send_message(_ver))
			return false;
	}
	NSHARE::sleep(1);
	{
		///8) Checking amount of errors and received messages
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		if (g_amount_of_msg == 0 || g_invalid_version_error != 0)
		{
			std::cout << "Invalid version test finished unsuccessfully "
					<< std::endl;
			return false;
		}
		else
			std::cout << "Invalid version test finished successfully "
					<< std::endl;
	}
	return true;
}
bool test_no_parser_error()
{
	unsigned _count = 0;
	///1)  Testing behavior without subscribers
	for (_count=0; _count < 5; ++_count)
	{
		///1.1) filing the message
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
		1000);

		{
			auto _it = _buf.begin(), _it_end = _buf.end();
			for (int i = 0; _it != _it_end; ++i, ++_it)
			{
				*_it = i % 255;
			}
		}
		///1.2) Send the message with random protocol name

		int _num = CCustomer::sMGetInstance().MSend(
				NSHARE::CText().MMakeRandom(10), _buf);

		{
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
			if (_num > 0)
				std::cout << "Send Packet#" << _num << std::endl;
			else
			{
				std::cout << "Send error  " << _num << std::endl;
				return false;
			}
		}

	}
	NSHARE::sleep(1);
	{
		/*! 1.3) Checking amount of errors
		 *  As no receivers of messages The error has not to be received.
		 */
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		if (g_parse_is_not_exist != 0)
		{
			std::cout
					<< "Invalid test for no parser error finished unsuccessfully: "
					<< std::endl;
			std::cout<<_count<<"!="<<g_parse_is_not_exist<<std::endl;
			return false;
		}
		else
			std::cout << "Invalid test for no parser  finished successfully "
					<< std::endl;
	}

	_count=0;

	///2)  Testing behavior if subscribers are exists

	///2.1) Create random protocol name
	NSHARE::CText _procol_name;
	_procol_name.MMakeRandom(10);

	///2.2) Subscribe to receive messages by this protocol

	CCustomer::sMGetInstance().MIWantReceivingMSG( //
			requirement_msg_info_t(_procol_name, required_header_t(),
			INDITIFICATION_NAME), //
			NULL);
	NSHARE::sleep(1);
	for (_count=0; _count < 5; ++_count)
	{

		///2.2) filing the message
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
		1000);

		{
			auto _it = _buf.begin(), _it_end = _buf.end();
			for (int i = 0; _it != _it_end; ++i, ++_it)
			{
				*_it = i % 255;
			}
		}
		///2.3) Send the message with created rendom protocol name

		int _num = CCustomer::sMGetInstance().MSend(
				_procol_name, _buf);

		{
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
			if (_num > 0)
				std::cout << "Send Packet#" << _num << std::endl;
			else
			{
				std::cout << "Send error  " << _num << std::endl;
				return false;
			}
		}

	}

	NSHARE::sleep(1);
	{
		/*! 2.4) Checking amount of errors
		 *  As there are receivers of messages The error has to be received.
		 */
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

		if (_count != g_parse_is_not_exist)
		{
			std::cout
					<< "Invalid test for no parser error finished unsuccessfully: "
					<< std::endl;
			std::cout<<_count<<"!="<<g_parse_is_not_exist<<std::endl;
			return false;
		}
		else
			std::cout << "Invalid test for no parser  finished successfully "
					<< std::endl;
	}

	return true;
}

bool test_parsing_error()
{
	using namespace example_of_user_protocol;

	{
		///1)  allocate the buffer for message with header
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			MSG_TEST_SIZE);

		///2) filing the head of message with invalid size info see #CExampleProtocolParser
		msg_head_t *_msg = (msg_head_t*) _buf.ptr();
		_msg->FType = E_MSG_TEST;
		_msg->FSize = MSG_TEST_SIZE + 1/*Imitation error.*/;

		/// 3) Filling message data into buffer
		{
			auto _it = _buf.begin() + sizeof(msg_head_t),
					_it_end = _buf.end();
			for (int i = 0; _it != _it_end; ++i, ++_it)
			{
				*_it = i % 255;
			}
		}
		///4) Send the message number #example_of_user_protocol::E_MSG_TEST of user protocol #example_of_user_protocol::PROTOCOL_NAME

		int _num = CCustomer::sMGetInstance().MSend(PROTOCOL_NAME, _buf);

		{
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
			if (_num > 0)
				std::cout << "Send Packet#" << _num << std::endl;
			else
			{
				std::cout << "Send error  " << _num << std::endl;
				return false;
			}
		}

		NSHARE::sleep(1);

		{
			///8) Checking amount of errors
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

			if (g_parse_error == 0 || g_amount_of_msg_test!=0)
			{
				std::cout << "Invalid parse error finished unsuccessfully "
						<< std::endl;
				return false;
			}
			else
				std::cout << "Invalid parse error finished successfully "
						<< std::endl;
		}
	}
	{
		g_parse_error=0;

		///9)  allocate the buffer for 3 message with header
		NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			MSG_TEST_SIZE //1-st message
			+ sizeof(msg_head_t) //2-d message
			+ MSG_TEST_SIZE //3-d message
		);

		///10) filing the first head of message without error
		msg_head_t *_msg = (msg_head_t*) _buf.ptr();
		_msg->FType = E_MSG_TEST;
		_msg->FSize = MSG_TEST_SIZE;

		///11) Filling message data into buffer
		{
			auto _it = _buf.begin() + sizeof(msg_head_t),
					_it_end = _it+ (MSG_TEST_SIZE-sizeof(msg_head_t));
			for (int i = 0; _it != _it_end; ++i, ++_it)
			{
				*_it = i % 255;
			}
		}

		/*! 12) filing the second head of message with invalid size info
		 * and without data see #CExampleProtocolParser
		 *
		 */
		auto const _second_msg_begin=_buf.ptr() + MSG_TEST_SIZE;

		msg_head_t *_msg2 = (msg_head_t*) (_second_msg_begin);
		_msg2->FType = E_MSG_TEST;
		_msg2->FSize = MSG_TEST_SIZE + 1/*Imitation error.*/;

		/*!13) filing the third head of message without error
		 *
		 */
		auto const _third_msg_begin=_second_msg_begin+sizeof(msg_head_t);

		msg_head_t *_msg3 = (msg_head_t*) (_third_msg_begin);
		_msg3->FType = E_MSG_TEST;
		_msg3->FSize = MSG_TEST_SIZE;

		///14) Filling  third message data into buffer
		{
			auto _it =_third_msg_begin + sizeof(msg_head_t),
					_it_end = _it+ MSG_TEST_SIZE-sizeof(msg_head_t);
			for (int i = 0; _it != _it_end; ++i, ++_it)
			{
				*_it = i % 255;
			}
		}
		///15) Send the buffer

		int _num = CCustomer::sMGetInstance().MSend(PROTOCOL_NAME, _buf);

		{
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
			if (_num > 0)
				std::cout << "Send Packet#" << _num << std::endl;
			else
			{
				std::cout << "Send error  " << _num << std::endl;
				return false;
			}
		}

		NSHARE::sleep(1);

		{
			///8) Checking amount of errors
			NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);

			if (g_parse_error != 1 || g_amount_of_msg_test!=2)
			{
				std::cout << "Invalid parse error finished unsuccessfully "
						<< std::endl;
				return false;
			}
			else
				std::cout << "Invalid parse error finished successfully "
						<< std::endl;
		}
	}
	return true;
}
}

