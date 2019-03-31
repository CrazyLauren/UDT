/*
 *
 * tasks.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 26.02.2013
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef TASKS_H_
#define TASKS_H_
namespace NSHARE
{
typedef std::vector<CThread::process_id_t> id_of_all_process_t;

extern SHARE_EXPORT NSHARE::CText process_name(CThread::process_id_t pid);
extern SHARE_EXPORT bool is_process_exist(CThread::process_id_t pid);
extern SHARE_EXPORT void pid_list(id_of_all_process_t* aOut);
extern SHARE_EXPORT void process_name_list(Strings *aOut);
}

#endif /* TASKS_H_ */
