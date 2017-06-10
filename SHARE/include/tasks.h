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
extern SHARE_EXPORT NSHARE::CText process_name(int pid);
extern SHARE_EXPORT bool is_process_exist(int pid);
extern SHARE_EXPORT void pid_list(std::list<unsigned long long>* aOut);
extern SHARE_EXPORT void process_name_list(std::list<NSHARE::CText> *aOut);
}

#endif /* TASKS_H_ */
