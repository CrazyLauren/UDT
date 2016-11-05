/*
 * deffunc.h
 *
 * Copyright © 2016 Sergey Cherepanov (sergey0311@gmail.com)
 *
 *  Created on: 04.04.2013
 *      Author: Sergey Cherepanov (https://github.com/CrazyLauren)
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef DEFFUNC_H_
#define DEFFUNC_H_

#include <math.h>
namespace NSHARE
{
//deprecated using CText
/*	inline void  gen_random_string(char* aS, size_t aLen)
	{
		static char const _alphanum[]=
		"0123456789"
		"QWERTYUIOPASDFGHJKLZXCVBNM"
		"qwertyuiopasdfghjklzxcvbnm";
		static size_t const _alphanum_size=sizeof(_alphanum)-1;
		if(!aLen)
			return;
		aS[aLen]=0;
		for(;aLen!=0;)//aLen всегда >0
			aS[--aLen]=_alphanum[std::rand()%_alphanum_size];
	}
*/
}
#define DEG_TO_RAD(angle) ((angle)*M_PI/180.0)
#define RAD_TO_DEG(angle) ((angle)*180.0/M_PI)
#endif /* DEFFUNC_H_ */
