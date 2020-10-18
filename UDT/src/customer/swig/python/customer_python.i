%module(package="UDT") customer_python
%{
/* Includes the header in the wrapper code */
#include "UDT/customer_export.h"
#include <deftype>
#include <customer.h>
#include "UDT/customer_c.h"
#include "customer_python_impl.h"
%}
%include  "std_string.i"
%include  "UDT/customer_export.h"
%include <UDT/customer/receive_data_info_c.h>
%include "UDT/customer/customer_c_type.h"
%include "UDT/customer_c.h"

udt_customer_string_t from_json_customer_string(const std::string& aData);
%extend udt_customer_string_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

version_c_t from_json_customer_version(const std::string& aData);
%extend version_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

required_header_c_t from_json_customer_required_header(const std::string& aData);
%extend required_header_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

received_message_info_c_t from_json_customer_received_message_info(const std::string& aData);
%extend received_message_info_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

buffer_c_t from_json_customer_buffer(const std::string& aData);
%extend buffer_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

id_c_t from_json_customer_id(const std::string& aData);
%extend id_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

program_id_c_t from_json_customer_program_id(const std::string& aData);
%extend program_id_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

requirement_msg_info_c_t from_json_customer_requirement_msg_info(const std::string& aData);
%extend requirement_msg_info_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

received_data_c_t from_json_customer_received_data(const std::string& aData);
%extend received_data_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

received_message_args_c_t from_json_customer_received_message_args(const std::string& aData);
%extend received_data_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

customers_updated_args_c_t from_json_customer_customers_updated_args(const std::string& aData);
%extend customers_updated_args_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}

subcribe_receiver_args_c_t from_json_customer_subcribe_receiver_args(const std::string& aData);
%extend subcribe_receiver_args_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}
fail_sent_args_c_t from_json_customer_fail_sent_args(const std::string& aData);
%extend fail_sent_args_c_t {
    void print_this();
    std::string to_json( unsigned aPretty);
}
