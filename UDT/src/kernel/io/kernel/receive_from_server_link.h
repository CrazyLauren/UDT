/*
 * receive_from_server_link.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 29.07.2016
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef RECEIVE_FROM_SERVER_LINK_H_
#define RECEIVE_FROM_SERVER_LINK_H_

#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_PROTOCOL_MSG,protocol_type_dg_t)/*Протокол КД*/\
	RECEIVE(E_INFO,dg_info2_t)/*Ответ на КД Запрос информации*/\
	RECEIVE(E_KERNEL_INFO,kernels_info_t)/*Информация о клиентах*/\
	RECEIVE(E_MAIN_CHANNEL_PARAM,main_channel_param_t)/*Настройки основного канала*/\
	RECEIVE(E_REQUEST_MAIN_CHANNEL_PARAM,request_main_channel_param_t)/*Запрос текуших настроек основного канала*/\
	RECEIVE(E_CLOSE_MAIN_CHANNEL,close_main_channel_t)/*Закрытие основного канала*/\
	RECEIVE(E_MAIN_CHANNEL_ERROR,main_channel_error_param_t)/*Ошибки открытия, закрытияб работы основного канала*/\
	RECEIVE(E_CUSTOMER_FILTERS,custom_filters_dg2_t)/*Требуемые данные*/\
	RECEIVE(E_CUSTOMERS_DEMANDS,customers_demands_t)/*Требуемые данные*/\
	RECEIVE(E_USER_DATA_FAIL_SEND,user_data_fail_send_t)/*Данные не доставлены*/\
	/*END*/
//RECEIVE(E_USER_DATA_FAIL_SEND,user_data_fail_send_t)/*Ошибка при отправке данных*/
#endif /* RECEIVE_FROM_SERVER_LINK_H_ */
