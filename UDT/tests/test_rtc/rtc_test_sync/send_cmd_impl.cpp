// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * send_cmd.cpp
 *
 *  Created on: 20.01.2019
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2019  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>

#include "../rtc_test_sync/api_test.h"
using namespace NSHARE;
using namespace NUDT;
namespace rtc_test_inherent
{
void send_nextime(IRtc::time_in_second_t aTime, NSHARE::uuid_t const & aTo)
{
	send_nextime(static_cast<IRtc::millisecond_t>(aTime*1000.0*1000.0),aTo);
}
void send_nextime(IRtc::millisecond_t aTime, NSHARE::uuid_t const & aTo)
{
	LOG(INFO)<<"Send next time "<<aTime<<" to "<<aTo;
	required_header_t _header;
	msg_head_t *_msg = (msg_head_t*)_header.FMessageHeader;
	_msg->FType = E_MSG_RTC_NEXT_TIME;

	NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			sizeof(msg_next_time_t));	///< allocate the buffer for message without header

	if(_buf.empty())
	{
		LOCK_STREAM
		std::cerr << "Cannot open allocate memory"<<std::endl;
		exit(EXIT_FAILURE);
	}

	msg_next_time_t *const _data= (msg_next_time_t *)_buf.ptr();
	_data->FNextTime=aTime;

	int _num = aTo.MIsValid()?//
			CCustomer::sMGetInstance().MSend(_header,PROTOCOL_NAME, _buf,aTo)//Отправляем конкретному адресату
			:CCustomer::sMGetInstance().MSend(_header,PROTOCOL_NAME, _buf);//Отправляем всем
	if (_num > 0)	//Hurrah!!! The data has been sent
	{
		LOG(INFO)<<"Send next time  "<<aTime<< " packet#" << _num;
	}
	else //The buffer _buf is not freed as it's not sent.
	{
		LOG(ERROR)<< "Send error  " << _num;
	}
}

void send_cmd(msg_control_t::eCMD aCmd,NSHARE::uuid_t const & aTo)
{
	required_header_t _header;
	msg_head_t *_msg = (msg_head_t*)_header.FMessageHeader;
	_msg->FType = E_MSG_RTC_CONTROL;

	NSHARE::CBuffer _buf = CCustomer::sMGetInstance().MGetNewBuf(
			sizeof(msg_control_t));	///< allocate the buffer for message without header

	if(_buf.empty())
	{
		LOG(ERROR)<< "Cannot open allocate memory";
		LOCK_STREAM
		std::cerr << "Cannot open allocate memory"<<std::endl;
		exit(EXIT_FAILURE);
	}

	msg_control_t *const _data= (msg_control_t *)_buf.ptr();
	_data->FCommand=aCmd;
	memset(_data->FName, 0, sizeof(_data->FName));
	memcpy(_data->FName, g_rtc_name,
			std::min(sizeof(_data->FName), strlen(g_rtc_name)));


	int _num = aTo.MIsValid()?//
			CCustomer::sMGetInstance().MSend(_header,PROTOCOL_NAME, _buf,aTo)//Отправляем конкретному адресату
			:CCustomer::sMGetInstance().MSend(_header,PROTOCOL_NAME, _buf);//Отправляем всем
	if (_num > 0)	//Hurrah!!! The data has been sent
	{
		LOG(INFO)<<"Send command "<<aCmd<< " packet#" << _num;
		LOCK_STREAM
		std::cout <<"Send command "<<aCmd<< " packet#" << _num <<std::endl;
	}
	else //The buffer _buf is not freed as it's not sent.
	{
		LOG(ERROR)<< "Send error  " << _num;
		LOCK_STREAM
		std::cout << "Send error  " << _num << std::endl;
	}
}
}
