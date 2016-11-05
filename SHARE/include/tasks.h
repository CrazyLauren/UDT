/*
 *
 * tasks.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 26.02.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef TASKS_H_
#define TASKS_H_
namespace NSHARE
{
SHARE_EXPORT NSHARE::CText process_name(int pid);
SHARE_EXPORT bool is_process_exist(int pid);
SHARE_EXPORT void pid_list(std::list<unsigned long long>* aOut);
SHARE_EXPORT void process_name_list(std::list<NSHARE::CText> *aOut);
}

#endif /* TASKS_H_ */
