/*
 * test.cpp
 *
 *  Created on: 29.03.2016
 *      Author: Sergey
 */
#include <deftype>
#include <customer.h>

using namespace NUDT;
extern size_t g_buf_size;
size_t g_buf_size = 0;
double g_time = 0.0;
double g_start_time = 0.0;
NSHARE::CMutex g_stream_mutex;

unsigned long long g_recv_count = 0;
unsigned g_i = 0;
extern int msg_test_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	args_t const* _recv_arg = (args_t const*) aWHAT;

	//!<Now You can handle the received data.
	NSHARE::CBuffer::size_type const _data_size = _recv_arg->FBuffer.size();
	g_recv_count += _data_size;

	double const _current_time = NSHARE::get_time();
	double _delta = _current_time - g_time;
	if (_delta == 0.0)
		_delta = 0.0000000000001;

	double const _speed = ((_data_size / 1024.0 / 1024.0) / _delta);
	g_time = NSHARE::get_time();

	if ((++g_i % 1000) == 0)
	{
		NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
		std::cout << "Receive <==" << (g_recv_count / 1024 / 1024)
				<< " md; speed=" << _speed << " mb/s; Med="
				<< ((g_recv_count / 1024.0 / 1024.0)
						/ (_current_time - g_start_time)) << " mb/s."
				<< std::endl;

	}
	return 0;
}
extern int event_new_receiver(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	new_receiver_args_t* _recv_arg = (new_receiver_args_t*) aWHAT;

	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	std::vector<new_receiver_args_t::what_t>::const_iterator _it(
			_recv_arg->FReceivers.begin()), _it_end(
			_recv_arg->FReceivers.end());
	for (; _it != _it_end; ++_it)
	{
		std::cout << "*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*" << std::endl;
		std::cout << "Now " << _it->FWho << " receive " << _it->FWhat.FRequired
				<< " from me by " << _it->FWhat.FProtocolName
				<< " As its Request " << _it->FRegExp << std::endl;
		std::cout << "-------------------------------------" << std::endl;
	}
	return 0;
}
extern int event_connect_handler(CCustomer* WHO, void *aWHAT, void* YOU_DATA)
{
	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	std::cout << "The udt library has been connected.." << std::endl;

	return 0;
}
extern int event_fail_sent_handler(CCustomer* WHO, void* aWHAT, void* YOU_DATA)
{
	fail_sent_args_t* _recv_arg = (fail_sent_args_t*) aWHAT;

	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	std::cerr << "The packet " << _recv_arg->FPacketNumber
			<< " has not been delivered to ";

	std::vector<NSHARE::uuid_t>::const_iterator _it(_recv_arg->FFails.begin()),
			_it_end(_recv_arg->FFails.end());
	for (; _it != _it_end; ++_it)
	{
		std::cerr << (*_it) << ", ";
	}

	std::cerr << " by UDT kernel." << std::endl;

	return 0;
}
extern int event_customers_update_handler(CCustomer* WHO, void* aWHAT,
		void* YOU_DATA)
{
	customers_updated_args_t const* _recv_arg =
			(customers_updated_args_t*) aWHAT;

	NSHARE::CRAII<NSHARE::CMutex> _block(g_stream_mutex);
	if (!_recv_arg->FConnected.empty())
	{
		std::set<program_id_t>::const_iterator _it =
				_recv_arg->FConnected.begin(), _it_end(
				_recv_arg->FConnected.end());

		std::cout << "New customers in UDT:";
		for (; _it != _it_end; ++_it)
		{
			std::cout << "\t" << (*_it) << std::endl;
		}
		std::cout << std::endl;
	}
	if (!_recv_arg->FDisconnected.empty())
	{
		std::cout << "Some customers droped:";
		std::set<program_id_t>::const_iterator _it =
				_recv_arg->FDisconnected.begin(), _it_end(
				_recv_arg->FDisconnected.end());
		for (; _it != _it_end; ++_it)
		{
			std::cout << (*_it) << std::endl;
		}
		std::cout << std::endl;
	}

	return 0;
}

extern void doing_something()
{

	//!< Wait for connected to UDT
	for (; !CCustomer::sMGetInstance().MIsConnected(); NSHARE::usleep(100000))
		;

	g_time = NSHARE::get_time();
	g_start_time = NSHARE::get_time();

	for (;;)
	{

		NSHARE::CBuffer _buf;
		for (;
				(_buf = CCustomer::sMGetInstance().MGetNewBuf(g_buf_size)).empty();
				)
			//!< allocate the buffer for msg
			;

		//!< Send the message number 0 (It's not necessary to specify the Receiver  
		//as If Somebody want to receive the message number 0 from us, It call method MIWantReceivingMSG and
		//specify receiving the message number 0 from us.)
		CCustomer::sMGetInstance().MSend(0, _buf);
	};

}

