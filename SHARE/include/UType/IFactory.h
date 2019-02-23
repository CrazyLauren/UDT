/*
 * IFactory.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */
#ifndef IFACTORY_H_
#define IFACTORY_H_

#ifndef SHARE_EXPORT
#	define SHARE_EXPORT
#endif 
namespace NSHARE
{
/** \brief Базовый класс всех классов-фабрик
*
*/
class SHARE_EXPORT IFactory
{
public:
	/** \brief Return factory name
	*
	*/
	const CText& MGetType() const
	{
		return FType;
	}
	
	virtual ~IFactory()
	{
	}

protected:
	/** \brief Fuctory constructor
	* \param type Factory name
	*/
	IFactory(const CText& type) :
			FType(type)
	{
	}

protected:
	CText const FType;
};
}


#endif /* IFACTORY_H_ */
