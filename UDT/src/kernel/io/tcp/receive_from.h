/*
 * receive_from.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 23.11.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef RECEIVE_FROM_H_
#define RECEIVE_FROM_H_

#define RECEIVES /*получаемые пакеты*/ \
	RECEIVE(E_REQUEST_INFO,requiest_info_t)/*Запрос информации*/\
	RECEIVE(E_INFO,dg_info_t)/*Ответ на КД Запрос информации*/\
	RECEIVE(E_MAIN_CHANNEL_PARAM,main_channel_param_t)/*Настройки основного канала*/\
	/*END*/


#endif /* RECEIVE_FROM_H_ */
