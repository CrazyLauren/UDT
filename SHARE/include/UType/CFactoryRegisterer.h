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
/** \brief класс для добавления фабрик при загрузки динамических библиотек
 *
 * Принцип работы следующий:
 * 1) В каждой библиотеке создается функция с сигнатурой factory_registry_func_t, содержащая
 * объекты наследники CFactoryRegisterer
 * 2) После вызова метода MLoad класса CResources во всех библиотеках ищется функция
 * с сигнатурой factory_registry_func_t (по умолчанию, с индетиф. ф-и FACTORY_REGISTRY_FUNC_NAME)
 * 3) Для всех CFactoryRegisterer вызывается метод MRegisterFactory
 * 4) Наследники класса CFactoryRegisterer добавляют новые фабрики
 */
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

/** \brief сигнатура ф-и регистратора
 *
 */
typedef factory_registry_t* (*factory_registry_func_t)(NSHARE::CConfig const*);

/** \brief идентификатор ф-ии регистратора по умолчанию
 *
 */
#define FACTORY_REGISTRY_FUNC_NAME "get_factory_registry"
}

#endif /* CFACTORYREGISTERER_H_ */
