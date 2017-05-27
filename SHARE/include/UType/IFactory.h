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
class SHARE_EXPORT IFactory
{
public:
	const CText& MGetType() const
	{
		return FType;
	}

	//! Destructor.
	virtual ~IFactory()
	{
	}

protected:
	IFactory(const CText& type) :
			FType(type)
	{
	}

protected:
	CText FType;
};
}


#endif /* IFACTORY_H_ */
