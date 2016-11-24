/*
 * revision.c
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 15.10.2015
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 * revision file for GIT
 */ 
#include <stdlib.h>
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

#define MAJOR 0
#define MINOR 1
#define REVISION "0"
#define REVISION_PATH "no path"
#define TIME	"18:08:58"
#define DATA	"2016-11-24"
static char gPath[]=REVISION_PATH;
static char gTime[]=TIME;
static char gData[]=DATA;

EXTERN unsigned get_revision_number()
{
	return atoi(REVISION);
}

EXTERN const char* get_revision_path()
{
	return gPath;
}
EXTERN const char* get_compile_time()
{
	return gTime;
}
EXTERN const char* get_compile_data()
{
	return gData;
}
EXTERN unsigned get_major_version()
{
	return MAJOR;
}
EXTERN unsigned get_minor_version()
{
	return MINOR;
}
