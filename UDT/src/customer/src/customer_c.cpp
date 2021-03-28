/*
 * customer_c.h
 *
 *  Created on: 06.09.2020
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2020  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#include <deftype>
#include <customer.h>
#include <shared_types.h>
#include <UDT/config/config.h>
#include <UDT/config/customer/customer_config.h>
#include <UDT/customer_c.h>
#include <UDT/impl/customer_covert_impl.h>
using namespace NUDT;

namespace NUDT
{
static int receive_data_cb(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);

template<>
void convert<NUDT::callback_t, callback_received_data_t *>(
		NUDT::callback_t *aTo,
		callback_received_data_t * const & aWhat)
{
	aTo->FSignal = &NUDT::receive_data_cb;
	aTo->FYouData = (void*) aWhat;
}
template<> void convert<callback_received_data_t, NUDT::callback_t>
(callback_received_data_t *aTo, NUDT::callback_t const& aWhat)
{
	*aTo = *(callback_received_data_t const*) aWhat.FYouData;
}

template<> void convert<request_info_c_t, NUDT::request_info_t>
(request_info_c_t *aTo, NUDT::request_info_t const& aWhat)
{
	convert(&aTo->what, aWhat.FWhat);
	convert(&aTo->handler, aWhat.FHandler);
}

static int receive_data_cb(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA)
{
	callback_received_data_t * _ptr = (callback_received_data_t *) YOU_DATA;
	NUDT::received_message_args_t *_what =
			(NUDT::received_message_args_t *) WHAT;
	received_message_args_c_t _what_c;
	convert(&_what_c, *_what);
	int _rval = (*_ptr->signal)(&_what_c, _ptr->you_data);
	_what->FOccurUserError = _what_c.info.occur_user_error;
	return _rval;
}
}

int init_udt_library_default(
		char const* aName,
		int aMajorVersion,
		int aMinorVersion
		)
{
	version_c_t _ver;
	if(aMajorVersion >= 0)
		_ver.major = aMajorVersion;
	if(aMinorVersion >= 0)
		_ver.minor = aMinorVersion;
	_ver.release = 0;

	NSHARE::CProgramName const _name(aName);
	NSHARE::CText _str("unknown_file_of_");
	_str+=_name.MGetName();
	char const* _argv[1];
	_argv[0] = (char const*) _str.c_str();
	return init_udt_library(1, _argv, aName, &_ver, NULL);
}
int init_udt_library(int argc,
		char const* argv[],
		char const* aName,
		version_c_t* aVersion, //or null
		char const* aConfPath //or null
		)
{
	NSHARE::version_t _ver;
	if (aVersion != NULL)
		NUDT::convert(&_ver, *aVersion);

	if (aVersion != NULL && aConfPath != NULL)
		return NUDT::CCustomer::sMInit(argc, argv, aName, _ver,
				NSHARE::CText(aConfPath)
						);
	else if (aVersion != NULL)
		return NUDT::CCustomer::sMInit(argc, argv, aName, _ver);
	else
		return NUDT::CCustomer::sMInit(argc, argv, aName);
}
void free_udt_library()
{
	NUDT::CCustomer::sMFree();
}
int library_version(version_c_t* aTo)
{
	if (aTo)
		NUDT::convert(aTo, NUDT::CCustomer::sMVersion());
	return aTo != NULL;
}
int udt_is_connected()
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
		return NUDT::CCustomer::sMGetInstance().MIsConnected() ? 0 : 1;
	return -1;
}
int udt_is_opened()
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
		return NUDT::CCustomer::sMGetInstance().MIsOpened() ? 0 : 1;
	return -1;
}
int udt_open()
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
		return NUDT::CCustomer::sMGetInstance().MOpen() ? 0 : 1;
	return -1;
}
int udt_close()
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::CCustomer::sMGetInstance().MClose();
		return 0;
	}
	return -1;
}

int udt_wait_for_event(char const* aEvent, double aSec)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
		return NUDT::CCustomer::sMGetInstance().MWaitForEvent(aEvent, aSec);
	return -1;
}
int udt_get_id(program_id_c_t *aTo)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::convert(aTo, NUDT::CCustomer::sMGetInstance().MGetID());
		return 0;
	}
	return -1;
}
int udt_customers(program_id_c_t* aToArray, unsigned aSize)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::CCustomer::customers_t const _customer(
				NUDT::CCustomer::sMGetInstance().MCustomers());

		NUDT::CCustomer::customers_t::const_iterator _it = _customer.begin(),
				_it_end(_customer.end());
		int i = 0;
		for (; _it != _it_end
				&& i < (int) aSize
				; ++_it, ++i)
			NUDT::convert(aToArray++, (NUDT::program_id_t) *_it);

		DCHECK_LE(_customer.size(), aSize);
		return _it == _it_end ? i : -static_cast<int>(_customer.size());
	}
	return -1;
}
int udt_send(char const* aProtocolName,
		void* aBuffer, unsigned aSize,
		unsigned aFlags)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		return NUDT::CCustomer::sMGetInstance().MSend(aProtocolName, aBuffer,
				aSize, (NUDT::CCustomer::eSendToFlags) aFlags);
	}
	return -1;
}

int udt_send_raw(required_header_c_t aNumber,
		char const* aProtocolName,
		void const* aBuffer, unsigned aSize,
		unsigned aFlags
		)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::required_header_t _header;
		NUDT::convert(&_header, aNumber);
		buffer_c_t _buf;
		_buf.ptr = (char const*) aBuffer;
		_buf.size = aSize;

		NSHARE::CBuffer _buffer;
		NUDT::convert(&_buffer, _buf);

		if (_buffer.empty())
			return static_cast<int>(NUDT::CCustomer::ERROR_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE);

		return NUDT::CCustomer::sMGetInstance().MSend(_header,
				aProtocolName,
				_buffer, (NUDT::CCustomer::eSendToFlags) aFlags);
	}
	return -1;
}
int udt_send_to(char const* aProtocolName,
		void const* aBuffer, unsigned aSize,
		uint64_t aTo,
		unsigned aFlags)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		return NUDT::CCustomer::sMGetInstance().MSend(aProtocolName,
				aBuffer,
				aSize,
				NSHARE::uuid_t(aTo),
				(NUDT::CCustomer::eSendToFlags) aFlags);
	}
	return -1;
}

int udt_send_raw_to(required_header_c_t aNumber,
		char const* aProtocolName,
		void const* aBuffer,
		unsigned aSize,
		uint64_t aTo,
		unsigned aFlags
		)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::required_header_t _header;
		NUDT::convert(&_header, aNumber);

		buffer_c_t _buf;
		_buf.ptr = (char const*) aBuffer;
		_buf.size = aSize;

		NSHARE::CBuffer _buffer;
		NUDT::convert(&_buffer, _buf);

		if (_buffer.empty())
			return static_cast<int>(NUDT::CCustomer::ERROR_CANNOT_ALLOCATE_BUFFER_OF_REQUIREMENT_SIZE);

		return NUDT::CCustomer::sMGetInstance().MSend(_header,
				aProtocolName,
				_buffer, NSHARE::uuid_t(aTo),
				(NUDT::CCustomer::eSendToFlags) aFlags);
	}
	return -1;
}
int udt_receiving_msg(requirement_msg_info_c_t aMSG,
		callback_received_data_t aHandler)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::requirement_msg_info_t _msg;
		NUDT::callback_t _handler;
		NUDT::convert(&_msg, aMSG);

		callback_received_data_t *_ptr = new callback_received_data_t(
				aHandler);
		NUDT::convert(&_handler, _ptr);

		int const _rval =
				NUDT::CCustomer::sMGetInstance().MIWantReceivingMSG(
						_msg, _handler);
		if (_rval < 0)
		{
			delete _ptr;
		}
		return _rval;
	}
	return -1;
}
int udt_unreceiving_msg(unsigned aHandler,
		request_info_c_t* aReuqest)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::request_info_t _msg;
		int const _rval =
				NUDT::CCustomer::sMGetInstance().MDoNotReceiveMSG(
						aHandler, &_msg);
		if (_rval >= 0)
		{
			if (aReuqest != NULL)
			{
				NUDT::convert(aReuqest, _msg);
			}
			delete (callback_received_data_t *) _msg.FHandler.FYouData;
		}
		return _rval;
	}
	return -1;
}
int udt_list_of_receiving(request_info_c_t* aToArray,
		unsigned aSize)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		std::vector<NUDT::request_info_t> const _customer(
				NUDT::CCustomer::sMGetInstance().MGetMyWishForMSG());

		std::vector<NUDT::request_info_t>::const_iterator _it =
				_customer.begin(), _it_end(_customer.end());
		int i = 0;
		for (; _it != _it_end
				&& i < (int) aSize
				; ++_it, ++i)
			NUDT::convert<request_info_c_t, NUDT::request_info_t>(
					aToArray++,
					*_it);

		DCHECK_LE(_customer.size(), aSize);
		return _it == _it_end ? i : -static_cast<int>(_customer.size());
	}
	return -1;
}
namespace NUDT
{
struct empty_event_t
{
};
template<> void convert<NUDT::empty_event_t, empty_event_c_t>
(NUDT::empty_event_t *aTo, empty_event_c_t const& aWhat)
{
}
template<> void convert<empty_event_c_t, NUDT::empty_event_t>
(empty_event_c_t *aTo, NUDT::empty_event_t const& aWhat)
{
}
}

#define ADD_EVENT_FUNCTION_IMPL(aFunc,aFuncErase,aCBType_C,aArgumentType,aArgumentType_C,aKeyT)\
		namespace NUDT{static int aFunc##_impl(NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA);}\
		namespace  NUDT\
		{\
			static aCBType_C _cb_##aFunc ;\
			static int aFunc##_impl (NUDT::CCustomer* WHO, void* WHAT, void* YOU_DATA)\
			{\
				NUDT:: aArgumentType const* _what = (NUDT:: aArgumentType const *)WHAT;\
				aArgumentType_C _what_c;\
				if(_what != NULL) NUDT::convert(&_what_c, *_what);\
				int _rval = (* _cb_##aFunc .signal)(&_what_c, _cb_##aFunc .you_data);\
				if(_rval == NSHARE::E_CB_REMOVE)\
				{\
					_cb_##aFunc .you_data = NULL;\
					_cb_##aFunc .signal = 0;\
				}\
				return _rval;\
			}\
		}\
		int aFunc(aCBType_C aEvent, unsigned aPrior)\
		{\
			if (NUDT::CCustomer::sMGetInstancePtr() != NULL && NUDT:: _cb_##aFunc .signal == 0)\
			{\
				NUDT:: _cb_##aFunc = aEvent;\
				NUDT::event_handler_info_t _eval(NUDT::CCustomer:: aKeyT, NUDT:: aFunc##_impl, &NUDT:: _cb_##aFunc);\
				bool const _rval = NUDT::CCustomer::sMGetInstance().MAdd(_eval, aPrior);\
				return _rval ? 0:1;\
			}\
			return -1;\
		}\
		int aFuncErase(aCBType_C aEvent)\
		{\
			if (NUDT::CCustomer::sMGetInstancePtr() != NULL)\
			{\
				NUDT::event_handler_info_t _eval(NUDT::CCustomer:: aKeyT, NUDT:: aFunc##_impl, &NUDT:: _cb_##aFunc);\
				bool const _rval = NUDT::CCustomer::sMGetInstance().MErase(_eval);\
				if(_rval)\
				{\
					NUDT:: _cb_##aFunc .you_data = NULL;\
					NUDT:: _cb_##aFunc .signal = 0;\
				}\
				return _rval ? 0:1;\
			}\
			return -1;\
		}\
		/*END*/

ADD_EVENT_FUNCTION_IMPL(udt_add_event_connect,
		udt_erase_event_connect,
		callback_connect_t,
		empty_event_t,
		empty_event_c_t,
		EVENT_CONNECTED
		);

ADD_EVENT_FUNCTION_IMPL(udt_add_event_disconnect,
		udt_erase_event_disconnect,
		callback_connect_t,
		empty_event_t,
		empty_event_c_t,
		EVENT_DISCONNECTED
		);

ADD_EVENT_FUNCTION_IMPL(udt_add_event_receiver_subscribe,
		udt_erase_event_receiver_subscribe,
		callback_subcribe_receiver_t,
		subcribe_receiver_args_t,
		subcribe_receiver_args_c_t,
		EVENT_RECEIVER_SUBSCRIBE
		);

ADD_EVENT_FUNCTION_IMPL(udt_add_event_receiver_unsubscribe,
		udt_erase_event_receiver_unsubscribe,
		callback_subcribe_receiver_t,
		subcribe_receiver_args_t,
		subcribe_receiver_args_c_t,
		EVENT_RECEIVER_UNSUBSCRIBE
		);

ADD_EVENT_FUNCTION_IMPL(udt_add_event_fail_sent,
		udt_erase_event_fail_sent,
		callback_fail_sent_t,
		fail_sent_args_t,
		fail_sent_args_c_t,
		EVENT_FAILED_SEND
		);

ADD_EVENT_FUNCTION_IMPL(udt_add_event_customer_update,
		udt_erase_event_customer_update,
		callback_customers_updated_t,
		customers_updated_args_t,
		customers_updated_args_c_t,
		EVENT_CUSTOMERS_UPDATED
		);

int udt_join()
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::CCustomer::sMGetInstance().MJoin();
		return 1;
	}
	return -1;
}
namespace NUDT
{
template<>
void convert<rtc_info_c_t, NUDT::IRtc>(rtc_info_c_t *aTo,
		NUDT::IRtc const& aWhat)
{
	aTo->rtc_type = static_cast<unsigned>(aWhat.MGetRTCInfo().FRTCType);
	convert(&aTo->name, aWhat.MGetRTCInfo().FName.MGetRawName());

	aTo->precision = aWhat.MGetPrecision();
	aTo->precision_ms = aWhat.MGetPrecisionMs();
}
}
int rtc_c_get_rtc_info(rtc_info_c_t* aToArray, unsigned aSize)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NUDT::CCustomer::rtc_list_t const _rtcs(
				NUDT::CCustomer::sMGetInstance().MGetListOfRTC());

		NUDT::CCustomer::rtc_list_t::const_iterator _it = _rtcs.begin(),
				_it_end(_rtcs.end());
		int i = 0;
		for (; _it != _it_end
				&& i < (int) aSize
				; ++_it, ++i)
		{
			DCHECK(_it->MIs());
			NUDT::convert(aToArray++, *(*_it));
		}

		DCHECK_LE(_rtcs.size(), aSize);
		return _it == _it_end ? i : -static_cast<int>(_rtcs.size());
	}
	return -1;
}
namespace NUDT
{
static IRtc* g_rtc[CUSTOMER_C_MAX_RTC_NUMBER] =
		{ NULL };
static NSHARE::CMutex g_lock_rtc;
}
int rtc_c_join(udt_customer_string_t aRtcName)
{
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		NSHARE::CText _val;
		convert(&_val, aRtcName);
		IRtc* const _rtc = NUDT::CCustomer::sMGetInstance().MGetRTC(_val);
		if (_rtc && _rtc->MJoinToRTC())
		{
			NSHARE::CRAII<NSHARE::CMutex> _lock(g_lock_rtc);
			unsigned _i = 0;
			for (; g_rtc[0] != NULL && _i < sizeof(g_rtc) / sizeof(g_rtc[0]);
					++_i)
				;

			CHECK_LT(_i, sizeof(g_rtc) / sizeof(g_rtc[0]));
			g_rtc[_i] = _rtc;

			return _i;
		}
		else
			return _rtc != NULL ? -3 : -2;
	}
	return -1;
}
int rtc_c_get_rtc_info_for(rtc_info_c_t* aTo, unsigned aId)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));

	if (NUDT::g_rtc[aId] != NULL)
		NUDT::convert(aTo, *(NUDT::g_rtc[aId]));

	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL;
}
int rtc_c_is_joined(unsigned aId)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::g_rtc[aId] != NULL;
}
int rtc_c_leave_from_rtc(unsigned aId)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		if (g_rtc[aId] != NULL)
		{
			bool const _is = g_rtc[aId]->MLeaveFromRTC();
			if (_is)
			{
				NSHARE::CRAII<NSHARE::CMutex> _lock(g_lock_rtc);
				g_rtc[aId] = NULL;

				return 1;
			}
			else
				return -3;
		}
		else
			return -2;
	}
	return -1;
}
rtc_c_time_in_second_t rtc_c_get_current_time(unsigned aId)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL ?
			NUDT::g_rtc[aId]->MGetCurrentTime() : -1;
}
rtc_c_millisecond_t rtc_c_get_current_time_ms(unsigned aId)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL ?
			NUDT::g_rtc[aId]->MGetCurrentTimeMs() :
			std::numeric_limits<rtc_c_millisecond_t>::max();
}
rtc_c_time_in_second_t rtc_c_sleep_until(unsigned aId,
		rtc_c_time_in_second_t aTime)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL ?
			NUDT::g_rtc[aId]->MSleepUntil(aTime) :
			-1;
}
rtc_c_millisecond_t rtc_c_sleep_until_ms(unsigned aId,
		rtc_c_millisecond_t aTime)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL ?
			NUDT::g_rtc[aId]->MSleepUntil(aTime) :
			std::numeric_limits<rtc_c_millisecond_t>::max();
}
namespace NUDT
{
template<>
void convert<callback_rtc_t, callback_rtc_c_t>(callback_rtc_t *aTo,
		callback_rtc_c_t const& aWhat)
{

	aTo->FSignal = aWhat.signal;
	aTo->FYouData = aWhat.you_data;
}
}
int rtc_c_set_timer(
		unsigned aId, rtc_c_time_in_second_t aFirstCall,
		rtc_c_time_in_second_t aIntervalCall,
		callback_rtc_c_t aCb)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		if (g_rtc[aId] != NULL)
		{
			callback_rtc_t _cb;
			convert(&_cb, aCb);
			bool const _is = g_rtc[aId]->MSetTimer(aFirstCall, aIntervalCall,
					_cb);
			return _is ? 1 : 0;
		}
		else
			return -2;
	}
	return -1;
}
int rtc_c_set_timer_ms(
		unsigned aId, rtc_c_millisecond_t aFirstCall,
		rtc_c_millisecond_t aIntervalCall,
		callback_rtc_c_t aCb)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	if (NUDT::CCustomer::sMGetInstancePtr() != NULL)
	{
		if (g_rtc[aId] != NULL)
		{
			callback_rtc_t _cb;
			convert(&_cb, aCb);
			bool const _is = g_rtc[aId]->MSetTimer(aFirstCall, aIntervalCall,
					_cb);
			return _is ? 1 : 0;
		}
		else
			return -2;
	}
	return -1;
}
rtc_c_time_in_second_t rtc_c_next_time(unsigned aId,
		rtc_c_time_in_second_t aTime)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL ?
											NUDT::g_rtc[aId]->MNextTime(aTime) :
											-1;
}
rtc_c_millisecond_t rtc_c_next_time_ms(unsigned aId,
		rtc_c_millisecond_t aTime)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL ?
			NUDT::g_rtc[aId]->MNextTime(aTime) :
			std::numeric_limits<rtc_c_millisecond_t>::max();
}
int rtc_c_amount_of_joined(unsigned aId)
{
	CHECK_LT(aId, sizeof(NUDT::g_rtc) / sizeof(NUDT::g_rtc[0]));
	return NUDT::CCustomer::sMGetInstancePtr() != NULL
			&& NUDT::g_rtc[aId] != NULL ?
			NUDT::g_rtc[aId]->MGetAmountOfJoined() : -1;
}
