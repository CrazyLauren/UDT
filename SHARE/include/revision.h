/*
 * revision.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 22.03.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
 
#ifndef CONTROL_VERSION_H_
#define CONTROL_VERSION_H_

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
EXTERN unsigned get_revision_number();
EXTERN unsigned get_major_version();
EXTERN unsigned get_minor_version();
EXTERN const char* get_revision_path();
EXTERN const char* get_compile_time();
EXTERN const char* get_compile_data();
#undef EXTERN

#define  REVISION get_revision_number()

#endif /*CONTROL_VERSION_H_*/
