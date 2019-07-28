/*
 * CAntiHang.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 09.11.2015
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CANTIHANG_H_
#define CANTIHANG_H_

#ifndef NDEBUG
#	define HANG_INIT NSHARE::CAntiHang _hang(__FILE__,__LINE__)
#	define HANG_CHECK _hang()
#else
#	define HANG_INIT (void)0
#	define HANG_CHECK (void)0
#endif
namespace NSHARE
{
/**\brief Very usefully class for check to infinite loop
 *
 *	using as:
 *
 *	for(HANG_INIT;some conditions;HANG_CHECK)
 *
 */
class  CAntiHang
{
	double FTime;
	unsigned FCount;
	char const * FFile;
	unsigned const FLine;
public:
	CAntiHang(char const* aFile, const unsigned & aLine) : //
		FTime(0),//
		FCount(0),//
		FFile(aFile), //
			FLine(aLine) //
	{
	}
	CAntiHang() : //
			FTime(0),//
			FCount(0),//
			FFile(NULL), //
			FLine(0) //
	{
	}
	/**\brief Проверяем на зацикливание каждые 10 повторов
	 *
	 * Если в течении 10 повторовов квант вермени составил больше 10 мс
	 * то засыпаем 10 мс
	 *
	 */
	bool operator()()
	{
		++FCount;
		if ((FCount ==10) || //after the first 10 reps is done,
			(FCount % 15 == 0)//Hang checking every 15 reps will be started 
			) 
		{
			double _delta = NSHARE::get_time() - FTime;
			if (_delta < 0.01) //10 ms
			{
				LOG_IF(WARNING, !FFile) << "Hang occur in unknown place.";
				LOG_IF(WARNING, FFile) << "Hang occur in " << FFile << ":"
						<< FLine;
			}
			FTime = NSHARE::get_time();
		}
		return true;
	}
	unsigned MCounter() const
	{
		return FCount;
	}
};
} /* namespace NSHARE */
#endif /* CANTIHANG_H_ */
