/*
 * CFactoryRegisterer.h
 *
 * Copyright © 2016  https://github.com/CrazyLauren
 *
 *  Created on: 12.01.2016
 *      Author:  https://github.com/CrazyLauren
 *
 * Distributed under MPL 2.0 (See accompanying file LICENSE.txt or copy at
 * https://www.mozilla.org/en-US/MPL/2.0)
 */ 
#ifndef CFACTORYREGISTERER_H_
#define CFACTORYREGISTERER_H_

namespace NSHARE
{

class SHARE_EXPORT CFactoryRegisterer
{
public:
	virtual ~CFactoryRegisterer()
	{
		;
	}

	void MRegisterFactory() const//fixme to cpp
	{
		//VLOG(2) << "Registering Factory for " << FType;
		if (!MIsAlreadyRegistered())
			return MAdding();
//		LOG(ERROR) << "Factory for \"" << FType
//				<< "\" has to be  already registered.";
	}
	virtual void MUnregisterFactory() const = 0;

	const CText FType;
	const version_t FVersion;
	virtual NSHARE::CConfig MSerialize() const
	{
		NSHARE::CConfig _conf("reg");
		_conf.MAdd("type",FType);
		_conf.MAdd(FVersion.MSerialize());
		return _conf;
	}
protected:
	CFactoryRegisterer(const CText& aType,version_t const& aVer) :
			FType(aType),FVersion(aVer)
	{
		;
	}

	virtual void MAdding() const = 0;
	virtual bool MIsAlreadyRegistered() const = 0;
};
typedef std::vector<CFactoryRegisterer*> factory_registry_t;
typedef factory_registry_t*(*factory_registry_func_t)();
#define FACTORY_REGISTRY_FUNC_NAME "get_factory_registry"
}

#endif /* CFACTORYREGISTERER_H_ */
