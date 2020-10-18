/*
 * customer_python_impl.h
 *
 *  Created on: 06.09.2020
 *      Author:  https://github.com/CrazyLauren
 *
 *	Copyright © 2020  https://github.com/CrazyLauren
 *
 *	Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 *	https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef PYTHON_CUSTOMER_PYTHON_IMPL_H_
#define PYTHON_CUSTOMER_PYTHON_IMPL_H_
#include <deftype>
#include <sstream>
#include <UDT/impl/customer_covert_impl.h>
template<class T>
inline T from_json_impl(const std::string& aData)
{
	typedef typename T::cpp_type_t cpp_type_t;

	std::istringstream _data(aData);
	NSHARE::CConfig _serialized;
	_serialized.MFromJSON(_data);
	cpp_type_t const _val(NSHARE::deserialize<cpp_type_t>(_serialized));
	T _rval;
	NUDT::convert(&_rval, _val);

	return _rval;
}

template<class T>
inline void udt_print_this(T* aData)
{
	typedef typename T::cpp_type_t cpp_type_t;
	cpp_type_t _val;
	NUDT::convert(&_val, *aData);
	std::cout<< _val;
}
template<class T>
inline std::string udt_to_json(T* aData,
    unsigned aPretty)
{
	typedef typename T::cpp_type_t cpp_type_t;
	cpp_type_t _val;
	NUDT::convert(&_val, *aData);

    NSHARE::CConfig const _serialized = NSHARE::serialize(_val);

    return _serialized.MToJSON( aPretty !=0 ).MToStdString();
}

#define ADD_CUSTOMER_C_FUNCTIONS(aType, aFunc)\
		inline aType aFunc(const std::string& aData)\
		{\
			return from_json_impl< aType >(aData);\
		}\
		inline void aType##_print_this(aType * aData)\
		{\
			return udt_print_this(aData);\
		}\
		inline std::string aType##_to_json(aType * aData,\
			    unsigned aPretty)\
		{\
			return udt_to_json(aData, aPretty);\
		}\
		/*END*/

ADD_CUSTOMER_C_FUNCTIONS(udt_customer_string_t, from_json_customer_string)
ADD_CUSTOMER_C_FUNCTIONS(version_c_t, from_json_customer_version)

ADD_CUSTOMER_C_FUNCTIONS(required_header_c_t, from_json_customer_required_header)
ADD_CUSTOMER_C_FUNCTIONS(received_message_info_c_t, from_json_customer_received_message_info)

ADD_CUSTOMER_C_FUNCTIONS(buffer_c_t, from_json_customer_buffer)
ADD_CUSTOMER_C_FUNCTIONS(id_c_t, from_json_customer_id)

ADD_CUSTOMER_C_FUNCTIONS(program_id_c_t, from_json_customer_program_id)

ADD_CUSTOMER_C_FUNCTIONS(requirement_msg_info_c_t, from_json_customer_requirement_msg_info)
ADD_CUSTOMER_C_FUNCTIONS(received_data_c_t, from_json_customer_received_data)

ADD_CUSTOMER_C_FUNCTIONS(received_message_args_c_t, from_json_customer_received_message_args)
ADD_CUSTOMER_C_FUNCTIONS(customers_updated_args_c_t, from_json_customer_customers_updated_args)


ADD_CUSTOMER_C_FUNCTIONS(subcribe_receiver_args_c_t, from_json_customer_subcribe_receiver_args)
ADD_CUSTOMER_C_FUNCTIONS(fail_sent_args_c_t, from_json_customer_fail_sent_args)



#endif /* PYTHON_CUSTOMER_PYTHON_IMPL_H_ */
